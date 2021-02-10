#include "random.h"
#include "sphere.h"
#include "camera.h"
#include "stb_image_write.h"

#include <iostream>

const double fov = 30.;
const double aperture = 0.09;
const double aspectRatio = 16. / 9.;
const int imgWidth = 1280;
const int sqrtSamplesPerPixel = 18;
const int maxDepth = 60;

const double fogDensity = 3.e-2;
const double fogHeight = 8.;
const double fogRadius = 24.;

const int imgHeight = imgWidth / aspectRatio;

Color rayColor(const Ray &ray, const Hittable &world, int depth) {
	if(depth > 0) {
		HitRecord record;
		if(world.hit(ray, std::numeric_limits<double>::max(), record)) {
			Color attenuation;
			Ray scattered;
			if(record.material->scatter(ray, record, attenuation, scattered)) {
				double fogCoeff = std::exp(- fogDensity * record.t * (1. - .5*(ray.origin().y + scattered.origin().y)/fogHeight));
				return fogCoeff * attenuation * rayColor(scattered, world, depth-1);
			} else return Color(0., 0., 0.);
		}
		// background color
		double t = .5 * (ray.direction().y + 1.);
		double rayFogDist = ray.direction().y < 0. ? fogRadius : std::min(fogRadius, (fogHeight - ray.origin().y) / ray.direction().y);
		double fogCoeff = std::exp(- fogDensity * rayFogDist * .5 * (1. - ray.origin().y/fogHeight));
		return fogCoeff * 1.2 * Color(1. - .5*t, 1. - .3*t, 1.);
	}
	return Color(0., 0., 0.);
}

HittableList randomScene() {
	HittableList world;

	// Ground
	world.add(new Sphere(Vec3(0., -5000., 0.), 5000., new Lambertian(Color(.5, .5, .5))));

	// Grid of spheres
	for(int x = -10; x <= 9; ++x) {
		for(int z = -8; z <= 4; ++z) {
			Vec3 center(x + .75 * Random::real(), .2, z + .75 * Random::real());
			if((center - Vec3(4., .9, 0.)).norm2() < 1.21) continue;
			if((center - Vec3(0., .95, 0.)).norm2() < 1.33) continue;
			if((center - Vec3(-4., 1., 0.)).norm2() < 1.44) continue;
			Material *mat;
			double rand_mat = Random::real();
			if(rand_mat < .7) mat = new Lambertian(Color::random() * Color::random());
			else if(rand_mat < .95) mat = new Metal(Color::randomRange(.5, 1.), Random::realRange(0., 0.5));
			else mat = new Dielectric(1.5);
			world.add(new Sphere(center, .2, mat));
		}
	}

	world.add(new Sphere(Vec3(-4., 1., 0.), 1., new Lambertian(Vec3(.4, .2, .1))));
	world.add(new Sphere(Vec3(0., .95, 0.), .95, new Dielectric(1.5)));
	world.add(new Sphere(Vec3(0., .95, 0.), .75, new Dielectric(1.5), true));
	world.add(new Sphere(Vec3(4., .9, 0.), .9, new Metal(Vec3(.7, .6, .5), 0.)));
	return world;
}

int main() {
	Random::init();
	HittableList world = randomScene();

	Vec3 camPos(13.6, 2., 3.6);
	Camera camera(camPos, -camPos, Vec3(0., 1., 0.), fov, aspectRatio, aperture, 10.);

	u_char *img = new u_char[imgWidth * imgHeight * 3];
	#pragma omp parallel for
	for(int i = 0; i < imgWidth; ++i) {
		for(int j = 0; j < imgHeight; ++j) {
			Color col(0., 0., 0.);
			for(int sx = 0; sx < sqrtSamplesPerPixel; ++sx) {
				for(int sy = 0; sy < sqrtSamplesPerPixel; ++sy) {
					double x = (i + (sx + Random::real()) / sqrtSamplesPerPixel) / imgWidth;
					double y = (j + (sy + Random::real()) / sqrtSamplesPerPixel) / imgHeight;
					col += rayColor(camera.getRay(x, y), world, maxDepth);
				}
			}
			col /= sqrtSamplesPerPixel * sqrtSamplesPerPixel;
			int pix = 3 * (i + (imgHeight - 1 - j) * imgWidth);
			img[pix] = std::min(.999, std::max(0., std::pow(col.x, 1./2.2))) * 256.;
			img[pix+1] = std::min(.999, std::max(0., std::pow(col.y, 1./2.2))) * 256.;
			img[pix+2] = std::min(.999, std::max(0., std::pow(col.z, 1./2.2))) * 256.;
		}
	}
	stbi_write_png("out.png", imgWidth, imgHeight, 3, img, 0);

	return 0;
}