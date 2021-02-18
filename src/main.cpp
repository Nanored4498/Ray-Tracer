#include "random.h"
#include "sphere.h"
#include "camera.h"
#include "bvh.h"
#include "triangle.h"
#include "stb_image_write.h"
#include "stats.h"

#include <iostream>
#include <thread>

const Scalar aperture = 0.09;
const Vec3 up(0., 1., 0.);
const int sqrtSamplesPerPixel = 20;
const int maxDepth = 40;

Camera camera;
int imgWidth, imgHeight;
Scalar fogDensity, fogHeight, fogRadius;
const bool sky = false;

Color rayColor(const Ray &ray, const Hittable *world, int depth) {
	if(depth <= 0) return Color(0., 0., 0.);
	HitRecord record;
	if(world->hit(ray, std::numeric_limits<Scalar>::max(), record)) {
		Color emitted, attenuation;
		Ray scattered;
		if(record.hittable->scatter(ray, record, emitted, attenuation, scattered)) {
			Scalar fogCoeff = std::exp(- fogDensity * record.t * (1. - .5*(ray.origin().y + scattered.origin().y)/fogHeight));
			return emitted + fogCoeff * attenuation * rayColor(scattered, world, depth-1);
		} else return emitted;
	}
	// background color
	if(sky) {
		Scalar rayFogDist = ray.direction().y < 0. ? fogRadius : std::min(fogRadius, (fogHeight - ray.origin().y) / ray.direction().y);
		Scalar fogCoeff = std::exp(- fogDensity * rayFogDist * .5 * (1. - ray.origin().y/fogHeight));
		Scalar t = .5 * (ray.direction().y + 1.);
		return fogCoeff * Color(.42-.42*t, .21-.21*t, .07-.07*t);
	}
	return Color(0., 0., 0.);
}

HittableList randomScene() {
	HittableList world;

	// Camera
	const Scalar fov = 30.;
	const Scalar aspectRatio = 16. / 9.;
	const Vec3 camPos(13.6, 2., 3.6);
	camera = Camera(camPos, -camPos, up, fov, aspectRatio, aperture, 10.);
	imgWidth = 1280;
	imgHeight = imgWidth / aspectRatio;

	// Fog
	fogDensity = 2.e-2;
	fogHeight = 8.;
	fogRadius = 24.;

	// Ground
	world.add(std::make_shared<Sphere>(Vec3(0., -5000., 0.), 5000.,
				// std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(Color(.6, .6, .6), Color(1., .3, .1)))));
				std::make_shared<Lambertian>(std::make_shared<NoiseTexture>(4.))));

	// Grid of spheres
	for(int x = -10; x <= 9; ++x) {
		for(int z = -8; z <= 4; ++z) {
			Vec3 center(x + .66 * Random::real(), .2, z + .66 * Random::real());
			if((center - Vec3(3.75, .2, 1.)).norm2() < 1.) continue;
			/*
			if((center - Vec3(4., .9, 0.)).norm2() < 1.21) continue;
			if((center - Vec3(0., .95, 0.)).norm2() < 1.33) continue;
			if((center - Vec3(-4., 1., 0.)).norm2() < 1.44) continue;
			*/
			std::shared_ptr<Material> mat;
			Scalar rand_mat = Random::real();
			if(rand_mat < .5) mat = std::make_shared<Lambertian>(Color::random() * Color::random());
			else if(rand_mat < .8) mat = std::make_shared<Metal>(Color::randomRange(.5, 1.), Random::realRange(0., 0.5));
			else if(rand_mat < .9) mat = std::make_shared<DiffuseLight>(Color::randomRange(.5, 2.8));
			else mat = std::make_shared<Dielectric>(1.5);
			world.add(std::make_shared<Sphere>(center, .2, mat));
		}
	}

	/*
	// big spheres
	world.add(std::make_shared<Sphere>(Vec3(-4., 1., 0.), 1., std::make_shared<Lambertian>(Vec3(.4, .2, .1))));
	world.add(std::make_shared<Sphere>(Vec3(0., .95, 0.), .95, std::make_shared<Dielectric>(1.5)));
	world.add(std::make_shared<Sphere>(Vec3(0., .95, 0.), .75, std::make_shared<Dielectric>(1.5), true));
	world.add(std::make_shared<Sphere>(Vec3(4., .9, 0.), .9, std::make_shared<Metal>(Vec3(.7, .6, .5), 0.)));
	*/

	// Bunny
	loadOBJ("../meshes/bunny.obj", world, Vec3(0., 1, 0.), 90., 2., Vec3(4., .96, 1.));

	// Earth
	world.add(std::make_shared<Sphere>(Vec3(4., 1.3, 2.7), .5,
				std::make_shared<Lambertian>(std::make_shared<ImageTexture>("../textures/earthmap.jpg"))));

	return world;
}

HittableList cornellBox() {
	HittableList world;

	// Camera
	const Scalar fov = 40.;
	const Scalar aperture = 0.09;
	const Vec3 camPos(278., 278., -800.);
	const Vec3 direction(0., 0., 1.);
	camera = Camera(camPos, direction, up, fov, 1., aperture, 800.);
	imgWidth = imgHeight = 720;

	// Fog
	fogDensity = 3.e-4;
	fogHeight = 600.;
	fogRadius = 600.;

	// Materials
	std::shared_ptr<Material>
		red = std::make_shared<Lambertian>(Color(.65, .05, 0.5)),
		white = std::make_shared<Lambertian>(Color(.73, .73, .73)),
		green = std::make_shared<Lambertian>(Color(.12, .45, .15)),
		light = std::make_shared<DiffuseLight>(Color(15., 15., 15.));
	
	world.add(std::make_shared<Triangle>(Vec3(555, 0, 0), Vec3(555, 0, 555), Vec3(555, 555, 555), green));
	world.add(std::make_shared<Triangle>(Vec3(555, 0, 0), Vec3(555, 555, 555), Vec3(555, 555, 0), green));
	world.add(std::make_shared<Triangle>(Vec3(0, 0, 0), Vec3(0, 555, 555), Vec3(0, 0, 555), red));
	world.add(std::make_shared<Triangle>(Vec3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 555, 555), red));
	world.add(std::make_shared<Triangle>(Vec3(213, 554, 227), Vec3(343, 554, 227), Vec3(213, 554, 332), light));
	world.add(std::make_shared<Triangle>(Vec3(343, 554, 227), Vec3(343, 554, 332), Vec3(213, 554, 332), light));
	world.add(std::make_shared<Triangle>(Vec3(0, 555, 555), Vec3(555, 555, 555), Vec3(0, 0, 555), white));
	world.add(std::make_shared<Triangle>(Vec3(555, 555, 555), Vec3(555, 0, 555), Vec3(0, 0, 555), white));
	world.add(std::make_shared<Triangle>(Vec3(0, 0, 0), Vec3(0, 0, 555), Vec3(555, 0, 0), white));
	world.add(std::make_shared<Triangle>(Vec3(555, 0, 555), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.add(std::make_shared<Triangle>(Vec3(0, 555, 0), Vec3(555, 555, 0), Vec3(0, 555, 555), white));
	world.add(std::make_shared<Triangle>(Vec3(555, 555, 555), Vec3(0, 555, 555), Vec3(555, 555, 0), white));
	loadOBJ("../meshes/bunny.obj", world, Vec3(0., 1, 0.), 180., 320., Vec3(278., 153., 320.));
	
	return world;
}

Hittable *world;
u_char *img;
std::atomic<int> I;
int sspp = 2;

void work() {
	for(int i = I++; i < imgWidth; i = I++) {
		for(int j = 0; j < imgHeight; ++j) {
			Color col(0., 0., 0.);
			for(int sx = 0; sx < sspp; ++sx) {
				for(int sy = 0; sy < sspp; ++sy) {
					Scalar x = (i + (sx + Random::real()) / sspp) / imgWidth;
					Scalar y = (j + (sy + Random::real()) / sspp) / imgHeight;
					col += rayColor(camera.getRay(x, y), world, maxDepth);
				}
			}
			col /= sspp * sspp;
			int pix = 3 * (i + (imgHeight - 1 - j) * imgWidth);
			col.x = std::max(1e-4, col.x);
			col.y = std::max(1e-4, col.y);
			col.z = std::max(1e-4, col.z);
			Scalar mul = std::min(1., 1. / col.maxCoeff());
			img[pix+0] = std::pow(.5 * (mul*col.x + std::min(.999, col.x)), 1./2.2) * 256.;
			img[pix+1] = std::pow(.5 * (mul*col.y + std::min(.999, col.y)), 1./2.2) * 256.;
			img[pix+2] = std::pow(.5 * (mul*col.z + std::min(.999, col.z)), 1./2.2) * 256.;
		}
	}
	Stats::aggregateLocalStats();
}

void render() {
	auto start = std::chrono::high_resolution_clock::now();

	I = 0;
	int T = (int) std::thread::hardware_concurrency() - 1;
	std::vector<std::thread> threads(T);
	for(int i = 0; i < T; ++i) threads[i] = std::thread(work);
	work();
	for(int i = 0; i < T; ++i) threads[i].join();

	auto end = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Time: " << time << " (ms)\n";
	#ifdef STATS
	std::cout << "Sphere tests: " << Stats::sphereRayTest << "\n";
	std::cout << "Triangle tests: " << Stats::triangleRayTest << "\n";
	std::cout << "Node tests: " << Stats::nodeRayTest << "\n";
	std::cout << "HitBox tests: " << Stats::hitBoxTest << "\n";
	#endif
}

int main() {
	Random::init();
	// HittableList list = randomScene();
	HittableList list = cornellBox();
	world = new BVHNode(list);
	// world = new BVHTree(list);
	img = new u_char[imgWidth * imgHeight * 3];

	render();
	stbi_write_png("pre.png", imgWidth, imgHeight, 3, img, 0);
	sspp = sqrtSamplesPerPixel;
	render();
	stbi_write_png("out.png", imgWidth, imgHeight, 3, img, 0);

	delete world;
	delete[] img;

	return 0;
}