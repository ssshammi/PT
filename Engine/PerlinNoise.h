#pragma once
#ifndef  PERLIN_NOISE
#define PERLIN_NOISE


class PerlinNoise
{

public:
	static void initialize();
	static void Release();
	// Classic Perlin noise, 3D version
	static double noise(double x, double y, double z);

	PerlinNoise();
	~PerlinNoise();

private:
	static int **grad3;

	static int *p;
	// To remove the need for index wrapping, double the permutation table length
	static int *perm;

	// This method is a *lot* faster than using (int)Math.floor(x)
	static int fastfloor(double x) {
		return x>0 ? (int)x : (int)x - 1;
	}
	 static double dot(int g[], double x, double y, double z) {
		return g[0] * x + g[1] * y + g[2] * z;
	}
	 static double mix(double a, double b, double t) {
		return (1 - t)*a + t*b;
	}
	 static double fade(double t) {
		return t*t*t*(t*(t * 6 - 15) + 10);
	}
};


#endif // ! PERLIN_NOISE