#include "random.h"
#include "sphere.h"
#include "camera.h"
#include "bvh.h"
#include "triangle.h"
#include "medium.h"
#include "stb_image_write.h"
#include "stats.h"

#include <iostream>
#include <thread>

const Vec3 up(0., 1., 0.);
const int SamplesPerPixel = 100;
const int maxDepth = 30;
const int scene = 1;
const bool sky = false;
const Vec3 skyDown(.18, .09, .03), skyUp(0., 0., 0.);

Camera camera;
int imgWidth, imgHeight;

void rayColor(const Ray &ray, const Hittable *world, Color &color) {
	Vec3 mult(1., 1., 1.);
	Ray currentRay = ray;
	int depth = 0;
	HitRecord record;
	rayTrace:
	if(world->hit(currentRay, std::numeric_limits<Scalar>::max(), record)) {
		Color emitted, attenuation;
		Ray scattered;
		bool newRay = record.hittable->scatter(currentRay, record, emitted, attenuation, scattered);
		color += mult * emitted;
		if(newRay && ++depth < maxDepth) {
			mult *= attenuation;
			if(mult.maxCoeff() > 1e-4) {
				currentRay = scattered;
				goto rayTrace;
			}
		}
	} else if(sky) {
		Scalar t = .5 * (currentRay.direction.y + 1.);
		color += mult * ((1.-t) * skyDown + t * skyUp);
	}
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
	if(bunny) loadOBJ("../meshes/bunny.obj", world, Vec3(0., 1, 0.), 90., 2., Vec3(4., .96, 1.),
								std::make_shared<Metal>(Color(.53, .35, .05), .07));
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
	camera = Camera(camPos, direction, up, fov, 1., aperture, 600.);
	imgWidth = imgHeight = 720;

	// Materials
	std::shared_ptr<Material>
		red = std::make_shared<Lambertian>(Color(.65, .05, .05)),
		white = std::make_shared<Lambertian>(Color(.73, .73, .73)),
		green = std::make_shared<Lambertian>(Color(.12, .45, .15)),
		// light = std::make_shared<DiffuseLight>(Color(7., 7., 7.));
		light = std::make_shared<DiffuseLight>(Color(15., 15., 15.));
	
	world.add(std::make_shared<Quad>(Vec3(555, 0, 0), Vec3(555, 0, 555), Vec3(555, 555, 0), green));
	world.add(std::make_shared<Quad>(Vec3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
	// world.add(std::make_shared<Quad>(Vec3(113, 554, 127), Vec3(443, 554, 127), Vec3(113, 554, 432), light));
	world.add(std::make_shared<Quad>(Vec3(213, 554, 227), Vec3(343, 554, 227), Vec3(213, 554, 332), light));
	world.add(std::make_shared<Quad>(Vec3(0, 555, 555), Vec3(555, 555, 555), Vec3(0, 0, 555), white));
	world.add(std::make_shared<Quad>(Vec3(0, 0, 0), Vec3(0, 0, 555), Vec3(555, 0, 0), white));
	world.add(std::make_shared<Quad>(Vec3(0, 555, 0), Vec3(555, 555, 0), Vec3(0, 555, 555), white));

	/*
	loadOBJ("../meshes/bunny.obj", world, Vec3(0., 1, 0.), 180., 320., Vec3(278., 153., 320.),
			std::make_shared<Lambertian>(std::make_shared<NoiseTexture>(.09)));
	std::shared_ptr<HittableList> box1 = std::make_shared<HittableList>();
	addBoxRotY(*box1, Vec3(165., 330., 165.), Vec3(265., 0., 295.), -15., white);
	world.add(std::make_shared<ConstantMedium>(box1, .01, Color(0., 0., 0.)));
	std::shared_ptr<HittableList> box2 = std::make_shared<HittableList>();
	addBoxRotY(*box2, Vec3(165., 165., 165.), Vec3(130., 0., 65.), 18., white);
	world.add(std::make_shared<ConstantMedium>(box2, .01, Color(1., 1., 1.)));
	*/
	addBoxRotY(world, Vec3(165., 330., 165.), Vec3(265., 0., 295.), -15., white);
	addBoxRotY(world, Vec3(165., 165., 165.), Vec3(130., 0., 65.), 18., white);
	
	return world;
}

HittableList nextWeekScene() {
	HittableList world;

	// Camera
	const Scalar fov = 40.;
	const Scalar aperture = 3.;
	const Scalar aspectRatio = 1.;
	const Vec3 camPos(478., 278., -600.);
	const Vec3 direction(-200., 0., 600.);
	camera = Camera(camPos, direction, up, fov, aspectRatio, aperture, 600.);
	imgWidth = 720;
	imgHeight = imgWidth / aspectRatio;

	// Ground
	std::shared_ptr<Material> groundMat = std::make_shared<Lambertian>(Color(.48, .83, .53));
	std::shared_ptr<Material> glassMat = std::make_shared<Dielectric>(1.5);
	const Scalar boxWidth = 100.;
	for(int i = -5; i < 8; ++i) {
		for(int j = -2; j < 7; ++j) {
			if(i == 0 && j == 2) continue;
			const Vec3 a(boxWidth * i, 0., boxWidth * j);
			const Scalar y = Random::realRange(1., 100.);
			addBoxRotY(world, Vec3(boxWidth, y, boxWidth), a, 0., groundMat);
		}
	}
	addBoxRotY(world, Vec3(.999*boxWidth, 106., .999*boxWidth), Vec3(.0005*boxWidth, 0., boxWidth * 2.0005), 0., glassMat);
	world.add(std::make_shared<Sphere>(Vec3(50., 50., 250.), 25.,
								std::make_shared<DiffuseLight>(Color(2., 2., 2.))));

	// Bunny
	HittableList bunny;
	loadOBJ("../meshes/bunny.obj", bunny, up, 180., 140., Vec3(60., 175.336, 250.),
								std::make_shared<Metal>(Color(.53, .35, .05), .07));
	world.add(std::make_shared<BVHNode>(bunny));

	// Light
	world.add(std::make_shared<Quad>(Vec3(123, 554, 147), Vec3(423, 554, 147), Vec3(113, 554, 412),
								std::make_shared<DiffuseLight>(Color(7., 7., 7.))));
	
	// Some spheres
	world.add(std::make_shared<Sphere>(Vec3(415., 400., 200.), 50.,
								std::make_shared<Lambertian>(Color(.7, .3, .1))));
	world.add(std::make_shared<Sphere>(Vec3(260., 150., 45.), 50., glassMat));
	world.add(std::make_shared<Sphere>(Vec3(0., 150., 145.), 50.,
								std::make_shared<Metal>(Color(.8, .8, .9), .8)));
	world.add(std::make_shared<Sphere>(Vec3(400., 200., 400.), 100.,
								std::make_shared<Lambertian>(std::make_shared<ImageTexture>("../textures/earthmap.jpg"))));
	world.add(std::make_shared<Sphere>(Vec3(220., 280., 300.), 80.,
								std::make_shared<Lambertian>(std::make_shared<NoiseTexture>(.1))));

	// Medium
	std::shared_ptr<Hittable> mediumBound = std::make_shared<Sphere>(Vec3(360., 150., 145.), 70., glassMat);
	world.add(mediumBound);
	world.add(std::make_shared<ConstantMedium>(mediumBound, .02, Color(.2, .4, .9)));
	mediumBound = std::make_shared<Sphere>(Vec3(100., 50., 200.), 800., glassMat);
	world.add(std::make_shared<ConstantMedium>(mediumBound, 8e-5, Color(1., 1., 1.)));

	// Many balls
	HittableList ballBox;
	std::shared_ptr<Material> white = std::make_shared<Lambertian>(Color(.73, .73, .73));
	const int nBalls = 1000;
	const Scalar angle = -15. * M_PI / 180.;
	const Scalar co = std::cos(angle), si = std::sin(angle);
	for(int i = 0; i < nBalls; ++i) {
		Vec3 r = Vec3::randomRange(0., 165.);
		if(r.y > 10. && r.y < 158. && std::max(r.x, 165.-r.z) < 135. && std::max(165-r.x, r.z) > 40.) continue;
		ballBox.add(std::make_shared<Sphere>(Vec3(-100. + r.x*co - r.z*si, 270. + r.y, 395. + r.x*si + r.z*co), 10., white));
	}
	world.add(std::make_shared<BVHNode>(ballBox));
	
	return world;
}

Hittable *world;
u_char *img;
std::atomic<int> I;
int spp = 5;

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
				rayColor(camera.getRay(x / imgWidth, y / imgHeight), world, col);
			}
			u_char* pix = img + 3 * (i + (imgHeight - 1 - j) * imgWidth);
			col /= spp;
			col.x = std::max(1e-4, col.x);
			col.y = std::max(1e-4, col.y);
			col.z = std::max(1e-4, col.z);
			Scalar mul = std::min(1., 1. / col.maxCoeff());
			pix[0] = std::pow(.5 * (mul*col.x + std::min(.999, col.x)), 1./2.2) * 256.;
			pix[1] = std::pow(.5 * (mul*col.y + std::min(.999, col.y)), 1./2.2) * 256.;
			pix[2] = std::pow(.5 * (mul*col.z + std::min(.999, col.z)), 1./2.2) * 256.;
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
	HittableList list;
	switch(scene) {
	case 0:
		list = randomScene(false, false);
		break;
	case 1:
		list = cornellBox();
		break;
	default:
		list = nextWeekScene();
		break;
	}
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