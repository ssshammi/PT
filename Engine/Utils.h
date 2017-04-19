#pragma once

#ifndef H_UTILS
#define H_UTILS

#include <iostream>

class Utils {
public:
	static float RandomFloat(float a, float b) {
		float random = ((float) rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}
	/*
	static float LerpAngle(float start, float end, float amount) {
	
		float shortest_angle = fmod((fmod((end - start) ,360.0f) + 540.0f) , 360.0f) - 180.0f;
		return shortest_angle * amount;
	}*/

	//Lerp between 2 angles to the closest outcome
	static float LerpAngle(float start, float end, float amount) {
		float difference = abs(end - start);
		if (difference > 180)
		{
			// We need to add on to one of the values.
			if (end > start)
			{
				// We'll add it on to start...
				start += 360;
			}
			else
			{
				// Add it on to end.
				end += 360;
			}
		}

		// Interpolate it.
		float value = (start + ((end - start) * amount));

		// Wrap it..
		float rangeZero = 360;

		if (value > 0 && value < 360)
			return value;

		return (fmod(value , rangeZero));
	}

	//Lerp between 2 angles to the closest out come (Radians)
	static float LerpRadians(float start, float end, float amount) {
		const float ratio = 180.0f/3.14f;
		end *= ratio;
		start *= ratio;
		return LerpAngle(start,end,amount)/ratio;
	}
};
#endif // !H_UTILS

