#include "texture.h"
#include "hittable.h"

#include "stb_image.h"

#include <algorithm>

NoiseTexture::NoiseTexture(double scale): scale(scale) {
	vecs = new Vec3[nbVals];
	for(int i = 0; i < nbVals; ++i) vecs[i] = Vec3::randomSphere();
	for(int i = 0; i < 2; ++i) {
		perms[i] = new int[nbVals];
		for(int j = 0; j < nbVals; ++j) perms[i][j] = j;
		std::random_shuffle(perms[i], perms[i] + nbVals);
	}
}

Color NoiseTexture::value(const Hittable *, const Vec3 &p, const Vec3 &) const {
	const int depth = 6;
	Scalar gray = 0., fr = scale, weight = 1.;
	for(int d = 0; d < depth; ++d) {
		Scalar u = fr * p.x, v = fr * p.y, w = fr * p.z;
		int i = static_cast<int>(std::floor(u));
		int j = static_cast<int>(std::floor(v));
		int k = static_cast<int>(std::floor(w));
		u -= i;
		v -= j;
		w -= k;
		u *= u * (3. - 2*u);
		v *= v * (3. - 2*v);
		w *= w * (3. - 2*w);
		Scalar w2 = 1. - w;
		int i2 = (i+1) & (nbVals-1);
		int j2 = perms[0][(j+1) & (nbVals-1)];
		int k2 = perms[1][(k+1) & (nbVals-1)];
		i &= (nbVals-1);
		j = perms[0][j & (nbVals-1)];
		k = perms[1][k & (nbVals-1)];
		const auto fun = [&](int i, Scalar x, Scalar y, Scalar z) { return vecs[i].x*x + vecs[i].y*y + vecs[i].z*z; };
		Scalar add = (1. - u) * ((1. - v) * (w2 * fun(i ^ j ^ k, u, v, w) + w * fun(i ^ j ^ k2, u, v, w-1.))
						+ v * (w2 * fun(i ^ j2 ^ k, u, v-1., w) + w * fun(i ^ j2 ^ k2, u, v-1., w-1.)))
					+ u * ((1. - v) * (w2 * fun(i2 ^ j ^ k, u-1., v, w) + w * fun(i2 ^ j ^ k2, u-1., v, w-1.))
						+ v * (w2 * fun(i2 ^ j2 ^ k, u-1., v-1., w) + w * fun(i2 ^ j2 ^ k2, u-1., v-1., w-1.)));
		gray += weight * add;
		fr *= 2.;
		weight *=.5;
	}
	return Vec3(1., 1., 1.) * .5 * (1. + std::sin(scale * p.z + 8.*gray));
}

ImageTexture::ImageTexture(std::string fileName) {
	data = stbi_load(fileName.c_str(), &W, &H, &C, 3);
	if(!data) {
		W = H = 0;
		throw std::runtime_error("Failed to load the file " + fileName + "!!!");
	}
}
#include <iostream>
Color ImageTexture::value(const Hittable *hittable, const Vec3 &p, const Vec3 &normal) const {
	Vec2 uv = hittable->getUV(p, normal);
	int i = std::min(int(uv.x * W), W-1), j = std::min(int((1. - uv.y) * H), H-1);
	u_char* pix = data + C * (i + W * j);
	const Scalar mult = 1. / 255.;
	return Color(mult * pix[0], mult * pix[1], mult * pix[2]);
}