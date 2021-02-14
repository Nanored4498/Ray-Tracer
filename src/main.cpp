#include "random.h"
#include "sphere.h"
#include "camera.h"
#include "bvh.h"
#include "triangle.h"
#include "stb_image_write.h"

#include <iostream>

const Scalar fov = 30.;
const Scalar aperture = 0.09;
const Scalar aspectRatio = 16. / 9.;
const int imgWidth = 1280;
const int sqrtSamplesPerPixel = 7;
const int maxDepth = 60;

const Scalar fogDensity = 3.e-2;
const Scalar fogHeight = 8.;
const Scalar fogRadius = 24.;

const int imgHeight = imgWidth / aspectRatio;

Color rayColor(const Ray &ray, const Hittable *world, int depth) {
	if(depth > 0) {
		HitRecord record;
		if(world->hit(ray, std::numeric_limits<Scalar>::max(), record)) {
			Color attenuation;
			Ray scattered;
			if(record.material->scatter(ray, record, attenuation, scattered)) {
				Scalar fogCoeff = std::exp(- fogDensity * record.t * (1. - .5*(ray.origin().y() + scattered.origin().y())/fogHeight));
				return fogCoeff * attenuation * rayColor(scattered, world, depth-1);
			} else return Color(0., 0., 0.);
		}
		// background color
		Scalar t = .5 * (ray.direction().y() + 1.);
		Scalar rayFogDist = ray.direction().y() < 0. ? fogRadius : std::min(fogRadius, (fogHeight - ray.origin().y()) / ray.direction().y());
		Scalar fogCoeff = std::exp(- fogDensity * rayFogDist * .5 * (1. - ray.origin().y()/fogHeight));
		return fogCoeff * 1.2 * Color(1. - .5*t, 1. - .3*t, 1.);
	}
	return Color(0., 0., 0.);
}

Hittable* randomScene() {
	HittableList world;

	// Ground
	world.add(new Sphere(Vec3(0., -5000., 0.), 5000., new Lambertian(Color(.5, .5, .5))));

	// Grid of spheres
	for(int x = -10; x <= 9; ++x) {
		for(int z = -8; z <= 4; ++z) {
			Vec3 center(x + .75 * Random::real(), .2, z + .75 * Random::real());
			// if((center - Vec3(4., .9, 0.)).norm2() < 1.21) continue;
			if((center - Vec3(5.2, .9, 0.)).norm2() < 1.41) continue;
			if((center - Vec3(0., .95, 0.)).norm2() < 1.33) continue;
			if((center - Vec3(-4., 1., 0.)).norm2() < 1.44) continue;
			Material *mat;
			Scalar rand_mat = Random::real();
			if(rand_mat < .7) mat = new Lambertian(Color::random() * Color::random());
			else if(rand_mat < .95) mat = new Metal(Color::randomRange(.5, 1.), Random::realRange(0., 0.5));
			else mat = new Dielectric(1.5);
			world.add(new Sphere(center, .2, mat));
		}
	}

	/*
	// big spheres
	world.add(new Sphere(Vec3(-4., 1., 0.), 1., new Lambertian(Vec3(.4, .2, .1))));
	world.add(new Sphere(Vec3(0., .95, 0.), .95, new Dielectric(1.5)));
	world.add(new Sphere(Vec3(0., .95, 0.), .75, new Dielectric(1.5), true));
	world.add(new Sphere(Vec3(4., .9, 0.), .9, new Metal(Vec3(.7, .6, .5), 0.)));
	*/

	// Bunny
	loadOBJ("../meshes/bunny.obj", world, new Metal(Vec3(.5, .35, .1), .1));

	return new BVHNode(world);
}

int main() {
	auto start = std::chrono::high_resolution_clock::now();
	Random::init();
	Hittable *world = randomScene();

	Vec3 camPos(13.6, 2., 3.6);
	Camera camera(camPos, -camPos, Vec3(0., 1., 0.), fov, aspectRatio, aperture, 10.);

	u_char *img = new u_char[imgWidth * imgHeight * 3];
	#pragma omp parallel for schedule(dynamic, 1)
	for(int i = 0; i < imgWidth; ++i) {
		for(int j = 0; j < imgHeight; ++j) {
			Color col(0., 0., 0.);
			for(int sx = 0; sx < sqrtSamplesPerPixel; ++sx) {
				for(int sy = 0; sy < sqrtSamplesPerPixel; ++sy) {
					Scalar x = (i + (sx + Random::real()) / sqrtSamplesPerPixel) / imgWidth;
					Scalar y = (j + (sy + Random::real()) / sqrtSamplesPerPixel) / imgHeight;
					col += rayColor(camera.getRay(x, y), world, maxDepth);
				}
			}
			col /= sqrtSamplesPerPixel * sqrtSamplesPerPixel;
			int pix = 3 * (i + (imgHeight - 1 - j) * imgWidth);
			img[pix] = std::min(.999, std::max(0., std::pow(col.x(), 1./2.2))) * 256.;
			img[pix+1] = std::min(.999, std::max(0., std::pow(col.y(), 1./2.2))) * 256.;
			img[pix+2] = std::min(.999, std::max(0., std::pow(col.z(), 1./2.2))) * 256.;
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Time: " << time << " (ms)\n";
	#ifdef SPHERE_STATS
	std::cout << "Sphere intersections: " << Sphere::getNbIntersections() << "\n";
	#endif
	#ifdef BVH_STATS
	std::cout << "BVHNode intersections: " << BVHNode::getNbIntersections() << "\n";
	#endif

	stbi_write_png("out.png", imgWidth, imgHeight, 3, img, 0);

	delete world;

	return 0;
}