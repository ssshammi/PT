#pragma once
#ifndef VORNOI
#define VORNOI

class Vornoi {
public:
	struct VoronoiPoint {
		float x, y, z;
		int index;
		float height;
	};

	struct VoronoiData {
		VoronoiPoint* VorPoint;
		float dist;
	};
};


#endif