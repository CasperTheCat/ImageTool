#pragma once


// Float3, HLSL - can handle anything!
struct float4
{
	float R;
	float G;
	float B;
	float A;

#pragma region Overload Opers
	void operator += (float n_x)
	{
		R = R + n_x;
		G = G + n_x;
		B = B + n_x;
	}
	void operator *= (float n_x)
	{
		R = R * n_x;
		G = G * n_x;
		B = B * n_x;
	}
	void operator -= (float n_x)
	{
		R = R - n_x;
		G = G - n_x;
		B = B - n_x;
	}
	void operator /= (float n_x)
	{
		R = R / n_x;
		G = G / n_x;
		B = B / n_x;
	}
#pragma endregion
};

// Define my V3, can handle 40BPP
struct bVec4
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

inline float Immato_Clamp(float in_t,int high_i,int low_i)
{
	return (in_t < low_i) ? low_i : (in_t > high_i) ? high_i : in_t;
}

inline float Immato_Clamp(float in_t)
{
	return (in_t < 0.f) ? 0.f : (in_t > 1.f) ? 1.f : in_t;
}

