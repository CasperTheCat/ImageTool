#pragma once

struct float3
{
	float R;
	float G;
	float B;

	float3 RGB() const
	{
		return float3(R, G, B);
	}

	float3(float x, float y, float z)
	{
		R = x;
		G = y;
		B = z;
	}

	float3()
	{

	}
#pragma region Overload Opers
#pragma region SELF
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

#pragma region float4 <oper> float
	float3 operator - (float n_fx)
	{
		return float3{ R - n_fx, G - n_fx, B - n_fx };
	}

	float3 operator + (float n_fx)
	{
		return float3{ R + n_fx, G + n_fx, B + n_fx };
	}

	float3 operator * (float n_fx)
	{
		return float3{ R * n_fx, G * n_fx, B * n_fx };
	}

	float3 operator / (float n_fx)
	{
		return float3{ R / n_fx, G / n_fx, B / n_fx };
	}
#pragma endregion

#pragma region float3 <oper> float3
	float3 operator - (float3 n_fx)
	{
		return float3{ R - n_fx.R, G - n_fx.G, B - n_fx.B };
	}

	float3 operator + (float3 n_fx)
	{
		return float3{ R + n_fx.R, G + n_fx.G, B + n_fx.B };
	}

	float3 operator * (float3 n_fx)
	{
		return float3{ R * n_fx.R, G * n_fx.G, B * n_fx.B };
	}

	float3 operator / (float3 n_fx)
	{
		return float3{ R / n_fx.R, G / n_fx.G, B / n_fx.B };
	}

#pragma endregion

#pragma endregion
};



// Float4, HLSL Style - can handle anything!
struct float4
{
	float R;
	float G;
	float B;
	float A;

	// Give me a float3
	float3 RGB()
	{
		return float3(R, G, B);
	}

#pragma region Overload Opers
#pragma region SELF
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

#pragma region float4 <oper> float
	float4 operator - (float n_fx)
	{
		return float4{ R - n_fx, G - n_fx, B - n_fx, A - n_fx };
	}

	float4 operator + (float n_fx)
	{
		return float4{ R + n_fx, G + n_fx, B + n_fx, A + n_fx };
	}

	float4 operator * (float n_fx)
	{
		return float4{ R * n_fx, G * n_fx, B * n_fx, A * n_fx };
	}

	float4 operator / (float n_fx)
	{
		return float4{ R / n_fx, G / n_fx, B / n_fx, A / n_fx };
	}
#pragma endregion

#pragma region float4 <oper> float4
	float4 operator - (float4 n_fx)
	{
		return float4{ R - n_fx.R, G - n_fx.G, B - n_fx.B, A - n_fx.A };
	}

	float4 operator + (float4 n_fx)
	{
		return float4{ R + n_fx.R, G + n_fx.G, B + n_fx.B, A + n_fx.A };
	}

	float4 operator * (float4 n_fx)
	{
		return float4{ R * n_fx.R, G * n_fx.G, B * n_fx.B, A * n_fx.A };
	}

	float4 operator / (float4 n_fx)
	{
		return float4{ R / n_fx.R, G / n_fx.G, B / n_fx.B, A / n_fx.A };
	}

	void operator = (float4 n_fx)
	{
		this->R = n_fx.R;
		this->B = n_fx.B;
		this->G = n_fx.G;
		this->A = n_fx.A;
	}

#pragma endregion

	void operator = (float3 n_fx)
	{
		this->R = n_fx.R;
		this->B = n_fx.B;
		this->G = n_fx.G;
		this->A = 1;
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

inline float Immato_Clamp(float in_t, int high_i, int low_i)
{
	return (in_t < low_i) ? low_i : (in_t > high_i) ? high_i : in_t;
}

inline float Immato_Clamp(float in_t)
{
	return (in_t < 0.f) ? 0.f : (in_t > 1.f) ? 1.f : in_t;
}

