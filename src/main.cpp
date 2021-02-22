#include "random.h"
#include "sphere.h"
#include "camera.h"
#include "bvh.h"
#include "triangle.h"
#include "stb_image_write.h"
#include "stats.h"

#include <iostream>
#include <thread>

const Vec3 up(0., 1., 0.);
const int SamplesPerPixel = 40;
const int maxDepth = 40;
const bool sky = false;

Camera camera;
int imgWidth, imgHeight;
Scalar fogDensity, fogHeight, fogRadius;

Color rayColor(const Ray &ray, const Hittable *world) {
	Vec3 color(0., 0., 0.), mult(1., 1., 1.);
	Ray currentRay = ray;
	for(int depth = 0; depth < maxDepth; ++depth) {
		HitRecord record;
		if(world->hit(currentRay, std::numeric_limits<Scalar>::max(), record)) {
			Color emitted, attenuation;
			Ray scattered;
			if(record.hittable->scatter(currentRay, record, emitted, attenuation, scattered)) {
				Scalar fogCoeff = std::exp(- fogDensity * record.t * (1. - .5*(currentRay.origin().y + scattered.origin().y)/fogHeight));
				color += mult * emitted;
				mult *= fogCoeff * attenuation;
				if(mult.maxCoeff() < 1e-5) break;
				currentRay = scattered;
			} else {
				color += mult * emitted;
				break;
			}
		} else if(sky) {
			Scalar rayFogDist = currentRay.direction().y < 0. ? fogRadius : std::min(fogRadius, (fogHeight - currentRay.origin().y) / currentRay.direction().y);
			Scalar fogCoeff = std::exp(- fogDensity * rayFogDist * .5 * (1. - currentRay.origin().y/fogHeight));
			Scalar t = .5 * (currentRay.direction().y + 1.);
			color += mult * fogCoeff * .42 * Color(.42-.42*t, .21-.21*t, .07-.07*t);
			break;
		} else break;
	}
	return color;
}

HittableList randomScene(bool bunny = true, bool noisyGround = true) {
	HittableList world;

	// Camera
	const Scalar fov = 30.;
	const Scalar aperture = 0.09;
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
	if(noisyGround) world.add(std::make_shared<Sphere>(Vec3(0., -5000., 0.), 5000.,
								std::make_shared<Lambertian>(std::make_shared<NoiseTexture>(4.))));
	else world.add(std::make_shared<Sphere>(Vec3(0., -5000., 0.), 5000.,
						std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(Color(.6, .6, .6), Color(1., .3, .1)))));

	// Grid of spheres
	for(int x = -10; x <= 9; ++x) {
		for(int z = -8; z <= 4; ++z) {
			Vec3 center(x + .66 * Random::real(), .2, z + .66 * Random::real());
			if(bunny) {
				if((center - Vec3(4., .2, 1.)).norm2() < 1.) continue;
			} else {
				if((center - Vec3(4., .9, 0.)).norm2() < 1.21) continue;
				if((center - Vec3(0., .95, 0.)).norm2() < 1.33) continue;
				if((center - Vec3(-4., 1., 0.)).norm2() < 1.44) continue;
			}
			std::shared_ptr<Material> mat;
			Scalar rand_mat = Random::real();
			if(rand_mat < .5) mat = std::make_shared<Lambertian>(Color::random() * Color::random());
			else if(rand_mat < .8) mat = std::make_shared<Metal>(Color::randomRange(.5, 1.), Random::realRange(0., 0.5));
			else if(rand_mat < .9) mat = std::make_shared<DiffuseLight>(Color::randomRange(.5, 2.8));
			else mat = std::make_shared<Dielectric>(1.5);
			world.add(std::make_shared<Sphere>(center, .2, mat));
		}
	}

	// Big spheres or Bunny
	if(bunny) loadOBJ("../meshes/bunny.obj", world, Vec3(0., 1, 0.), 90., 2., Vec3(4., .96, 1.));
	else {
		world.add(std::make_shared<Sphere>(Vec3(-4., 1., 0.), 1., std::make_shared<Lambertian>(Vec3(.4, .2, .1))));
		world.add(std::make_shared<Sphere>(Vec3(0., .95, 0.), .95, std::make_shared<Dielectric>(1.5)));
		world.add(std::make_shared<Sphere>(Vec3(0., .95, 0.), .75, std::make_shared<Dielectric>(1.5), true));
		world.add(std::make_shared<Sphere>(Vec3(4., .9, 0.), .9, std::make_shared<Metal>(Vec3(.7, .6, .5), 0.)));
	}

	// Earth
	world.add(std::make_shared<Sphere>(Vec3(4., 1.3, 2.7), .5,
				std::make_shared<Lambertian>(std::make_shared<ImageTexture>("../textures/earthmap.jpg"))));

	return world;
}

HittableList cornellBox() {
	HittableList world;

	// Camera
	const Scalar fov = 40.;
	const Scalar aperture = 3.;
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
		red = std::make_shared<Lambertian>(Color(.65, .05, .05)),
		white = std::make_shared<Lambertian>(Color(.73, .73, .73)),
		green = std::make_shared<Lambertian>(Color(.12, .45, .15)),
		light = std::make_shared<DiffuseLight>(Color(15., 15., 15.));
	
	world.add(std::make_shared<Quad>(Vec3(555, 0, 0), Vec3(555, 0, 555), Vec3(555, 555, 0), green));
	world.add(std::make_shared<Quad>(Vec3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
	world.add(std::make_shared<Quad>(Vec3(213, 554, 227), Vec3(343, 554, 227), Vec3(213, 554, 332), light));
	world.add(std::make_shared<Quad>(Vec3(0, 555, 555), Vec3(555, 555, 555), Vec3(0, 0, 555), white));
	world.add(std::make_shared<Quad>(Vec3(0, 0, 0), Vec3(0, 0, 555), Vec3(555, 0, 0), white));
	world.add(std::make_shared<Quad>(Vec3(0, 555, 0), Vec3(555, 555, 0), Vec3(0, 555, 555), white));
	loadOBJ("../meshes/bunny.obj", world, Vec3(0., 1, 0.), 180., 320., Vec3(278., 153., 320.));
	
	return world;
}

Hittable *world;
u_char *img;
std::atomic<int> I;
int spp = 4;

void work() {
	const Scalar phi = 1.324717957244746026;
	const Scalar ax = 1. / phi;
	const Scalar ay = ax*ax;
	Scalar x = Random::real();
	Scalar y = Random::real();
	for(int i = I++; i < imgWidth; i = I++) {
		for(int j = 0; j < imgHeight; ++j) {
			Color col(0., 0., 0.);
			for(int s = 0; s < spp; ++s) {
				x += ax;
				y += ay;
				x += i - std::floor(x);
				y += j - std::floor(y);
				col += rayColor(camera.getRay(x / imgWidth, y / imgHeight), world);
			}
			col /= spp;
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
	// HittableList list = randomScene(false, false);
	HittableList list = cornellBox();
	world = new BVHNode(list);
	// world = new BVHTree(list);
	img = new u_char[imgWidth * imgHeight * 3];

	render();
	stbi_write_png("pre.png", imgWidth, imgHeight, 3, img, 0);
	spp = SamplesPerPixel;
	render();
	stbi_write_png("out.png", imgWidth, imgHeight, 3, img, 0);

	delete world;
	delete[] img;

	return 0;
}