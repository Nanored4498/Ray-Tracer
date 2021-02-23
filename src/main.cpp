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
const int SamplesPerPixel = 500;
const int maxDepth = 30;
const bool sky = false;

Scalar focalDistance;
Camera camera;
int imgWidth, imgHeight;
Scalar fogDensity, fogHeight, fogRadius;

struct PixelData {
	Color color, emit, att;
	Vec3 pos, normal;
};

void rayColor(const Ray &ray, const Hittable *world, PixelData &data) {
	Vec3 mult(1., 1., 1.);
	Ray currentRay = ray;
	int depth = 0;
	HitRecord record;
	rayTrace:
	if(world->hit(currentRay, std::numeric_limits<Scalar>::max(), record)) {
		Color emitted, attenuation;
		Ray scattered;
		bool newRay = record.hittable->scatter(currentRay, record, emitted, attenuation, scattered);
		if(depth == 0) {
			data.emit += emitted;
			data.att += attenuation;
			Vec3 pos = currentRay.at(record.t);
			data.pos += pos;
			data.normal += record.hittable->getNormal(pos, currentRay);
		}
		data.color += mult * emitted;
		if(newRay && ++depth < maxDepth) {
			Scalar fogCoeff = std::exp(- fogDensity * record.t * (1. - .5*(currentRay.origin().y + scattered.origin().y)/fogHeight));
			mult *= fogCoeff * attenuation;
			if(mult.maxCoeff() > 1e-5) {
				currentRay = scattered;
				goto rayTrace;
			}
		}
	} else if(sky) {
		Scalar rayFogDist = currentRay.direction().y < 0. ? fogRadius : std::min(fogRadius, (fogHeight - currentRay.origin().y) / currentRay.direction().y);
		Scalar fogCoeff = std::exp(- fogDensity * rayFogDist * .5 * (1. - currentRay.origin().y/fogHeight));
		Scalar t = .5 * (currentRay.direction().y + 1.);
		Color skyE = .42 * Color(.42-.42*t, .21-.21*t, .07-.07*t);
		if(depth == 0) data.emit += skyE;
		data.color += mult * fogCoeff * skyE;
	}
}

HittableList randomScene(bool bunny = true, bool noisyGround = true) {
	HittableList world;

	// Camera
	const Scalar fov = 30.;
	const Scalar aperture = 0.09;
	const Scalar aspectRatio = 16. / 9.;
	const Vec3 camPos(13.6, 2., 3.6);
	focalDistance = 10.;
	camera = Camera(camPos, -camPos, up, fov, aspectRatio, aperture, focalDistance);
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
	focalDistance = 600.;
	camera = Camera(camPos, direction, up, fov, 1., aperture, focalDistance);
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
PixelData *data;
std::atomic<int> I;
int spp = 4;
int totSPP = 0;

void work() {
	const Scalar phi = 1.324717957244746026;
	const Scalar ax = 1. / phi;
	const Scalar ay = ax*ax;
	Scalar x = Random::real();
	Scalar y = Random::real();
	for(int i = I++; i < imgWidth; i = I++) {
		for(int j = 0; j < imgHeight; ++j) {
			PixelData &pd = data[i + (imgHeight - 1 - j) * imgWidth];
			for(int s = 0; s < spp; ++s) {
				x += ax;
				y += ay;
				x += i - std::floor(x);
				y += j - std::floor(y);
				rayColor(camera.getRay(x / imgWidth, y / imgHeight), world, pd);
			}
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
	totSPP += spp;

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

inline void color2uhar(const Vec3 &col, u_char *pix) {
	Scalar r = std::max(1e-4, col.x);
	Scalar g = std::max(1e-4, col.y);
	Scalar b = std::max(1e-4, col.z);
	Scalar mul = std::min(1., 1. / col.maxCoeff());
	pix[0] = std::pow(.5 * (mul*r + std::min(.999, r)), 1./2.2) * 256.;
	pix[1] = std::pow(.5 * (mul*g + std::min(.999, g)), 1./2.2) * 256.;
	pix[2] = std::pow(.5 * (mul*b + std::min(.999, b)), 1./2.2) * 256.;
}

int main() {
	Random::init();
	// HittableList list = randomScene(false, false);
	HittableList list = cornellBox();
	world = new BVHNode(list);
	// world = new BVHTree(list);
	size_t imgSize = imgWidth * imgHeight;
	data = new PixelData[imgSize];
	img = new u_char[imgSize * 3];

	render();
	for(size_t i = 0; i < imgSize; ++i) color2uhar(data[i].color / totSPP, img + 3*i);
	stbi_write_png("pre.png", imgWidth, imgHeight, 3, img, 0);
	spp = SamplesPerPixel;
	render();

	const int h = 10;
	const double iSigmaP = 3e3, iSigmaN = 1e2;
	Vec3 minPos(1e9, 1e9, 1e9), maxPos(-1e9, -1e9, -1e9);
	for(size_t i = 0; i < imgSize; ++i) {
		PixelData &pd = data[i];
		pd.color = pd.att.maxCoeff() < 1e-6 ? Vec3(0., 0., 0.) : (pd.color - pd.emit) / pd.att;
		pd.att /= totSPP;
		pd.emit /= totSPP;
		pd.normal /= totSPP;
		pd.pos = (pd.pos / totSPP - camera.getPos()) / focalDistance;
		minPos = min(minPos, pd.pos);
		maxPos = max(maxPos, pd.pos);
	}
	maxPos -= minPos;
	for(int i = 0; i < (int) imgSize; ++i) {
		Scalar sw = 0.;
		Color in(0., 0., 0.);
		int x = i % imgWidth, y = i / imgWidth;
		int af = std::min(imgWidth, x+h+1);
		int bf = std::min(imgHeight, y+h+1);
		for(int a = std::max(0, x-h); a < af; ++a) {
			for(int b = std::max(0, y-h); b < bf; ++b) {
				int j = a + b * imgWidth;
				if(data[j].color.maxCoeff() == 0.) continue;
				Scalar w = std::exp(- iSigmaP * (data[i].pos - data[j].pos).norm2() - iSigmaN * (data[i].normal - data[j].normal).norm2());
				sw += w;
				in += w * data[j].color;
			}
		}
		if(sw > 0.) in /= sw;
		color2uhar(data[i].emit + data[i].att * in, img + 3*i);
	}
	stbi_write_png("out.png", imgWidth, imgHeight, 3, img, 0);
	if(1) {
		for(size_t i = 0; i < imgSize; ++i) color2uhar(data[i].att, img + 3*i);
		stbi_write_png("att.png", imgWidth, imgHeight, 3, img, 0);
		for(size_t i = 0; i < imgSize; ++i) color2uhar(data[i].color, img + 3*i);
		stbi_write_png("incident.png", imgWidth, imgHeight, 3, img, 0);
		for(size_t i = 0; i < imgSize; ++i) color2uhar(.5 * (data[i].normal + Vec3(1., 1., 1.)), img + 3*i);
		stbi_write_png("normal.png", imgWidth, imgHeight, 3, img, 0);
		for(size_t i = 0; i < imgSize; ++i) color2uhar((data[i].pos - minPos) / maxPos, img + 3*i);
		stbi_write_png("pos.png", imgWidth, imgHeight, 3, img, 0);
	}

	delete world;
	delete[] data;
	delete[] img;

	return 0;
}