#pragma once

struct Vector3
{
	union
	{
		struct { float X, Y, Z; };
		float Components[3];
	};

	Vector3() :X(0), Y(0), Z(0) {}

	Vector3(float x, float y, float z) :X(x), Y(y), Z(z) {}

	Vector3 operator*(const float& Scalar)
	{
		return Vector3(X*Scalar, Y*Scalar, Z*Scalar);
	}

	Vector3 operator+(const Vector3& Other)
	{
		return Vector3(X + Other.X, Y + Other.Y, Z + Other.Z);
	}

	Vector3& operator+=(const Vector3& Other)
	{
		X += Other.X;
		Y += Other.Y;
		Z += Other.Z;

		return *this;
	}
};

struct Vector4
{
	union
	{
		struct { float X, Y, Z, W; };
		float Components[4];
	};


	Vector4() :X(0), Y(0), Z(0), W(0) {}

	Vector4(float x, float y, float z, float w) :X(x), Y(y), Z(z), W(w) {}

	Vector4& operator=(const Vector4& Other)
	{
		X = Other.X;
		Y = Other.Y;
		Z = Other.Z;
		W = Other.W;
		return *this;
	}
};

struct Matrix4x4
{
	union
	{
		float Elements[16];
	};

	Matrix4x4(float Diagonal)
	{
		for (int i = 0; i < 4 * 4; ++i) {
			Elements[i] = 0;
		}

		Elements[0] = Diagonal;
		Elements[5] = Diagonal;
		Elements[10] = Diagonal;
		Elements[15] = Diagonal;
	}

	Matrix4x4()
	{
		for (int i = 0; i < 4 * 4; ++i) {
			Elements[i] = 0;
		}
	}

	static Matrix4x4 Identity()
	{
		return Matrix4x4(1.0f);
	}

	static Matrix4x4 Translation(float X, float Y, float Z)
	{
		Matrix4x4 Result(1.0f);

		Result.Elements[12] = X;
		Result.Elements[13] = Y;
		Result.Elements[14] = Z;

		return Result;
	}

	static Matrix4x4 Translation(Vector3 TranslationVector)
	{
		Matrix4x4 Result(1.0f);

		Result.Elements[12] = TranslationVector.X;
		Result.Elements[13] = TranslationVector.Y;
		Result.Elements[14] = TranslationVector.Z;

		return Result;
	}

	static Matrix4x4 Scaling(float X, float Y, float Z)
	{
		Matrix4x4 Result(1.0f);

		Result.Elements[0] = X;
		Result.Elements[5] = Y;
		Result.Elements[10] = Z;

		return Result;
	}

	static Matrix4x4 Scaling(Vector3 ScalingVector)
	{
		Matrix4x4 Result(1.0f);

		Result.Elements[0] = ScalingVector.X;
		Result.Elements[5] = ScalingVector.Y;
		Result.Elements[10] = ScalingVector.Z;

		return Result;
	}

	static Matrix4x4 Ortographic(float ViewLeft,
		float ViewRight,
		float ViewBottom,
		float ViewTop,
		float NearZ,
		float FarZ)
	{
		Matrix4x4 Result(1.0f);

		float ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
		float ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
		float fRange = 1.0f / (FarZ - NearZ);


		Result.Elements[0] = ReciprocalWidth + ReciprocalWidth;
		Result.Elements[5] = ReciprocalHeight + ReciprocalHeight;
		Result.Elements[10] = fRange;

		Result.Elements[12] = -(ViewLeft + ViewRight) * ReciprocalWidth;
		Result.Elements[13] = -(ViewTop + ViewBottom) * ReciprocalHeight;
		Result.Elements[14] = -fRange * NearZ;
		return Result;
	}

	Matrix4x4 Multiply(const Matrix4x4& Left, const Matrix4x4& Right)
	{
		Matrix4x4 Result;

		Result.Elements[0] = Right.Elements[0] * Left.Elements[0] + Right.Elements[1] * Left.Elements[4] + Right.Elements[2] * Left.Elements[8] + Right.Elements[3] * Left.Elements[12];
		Result.Elements[1] = Right.Elements[0] * Left.Elements[1] + Right.Elements[1] * Left.Elements[5] + Right.Elements[2] * Left.Elements[9] + Right.Elements[3] * Left.Elements[13];
		Result.Elements[2] = Right.Elements[0] * Left.Elements[2] + Right.Elements[1] * Left.Elements[6] + Right.Elements[2] * Left.Elements[10] + Right.Elements[3] * Left.Elements[14];
		Result.Elements[3] = Right.Elements[0] * Left.Elements[3] + Right.Elements[1] * Left.Elements[7] + Right.Elements[2] * Left.Elements[11] + Right.Elements[3] * Left.Elements[15];
		Result.Elements[4] = Right.Elements[4] * Left.Elements[0] + Right.Elements[5] * Left.Elements[4] + Right.Elements[6] * Left.Elements[8] + Right.Elements[7] * Left.Elements[12];
		Result.Elements[5] = Right.Elements[4] * Left.Elements[1] + Right.Elements[5] * Left.Elements[5] + Right.Elements[6] * Left.Elements[9] + Right.Elements[7] * Left.Elements[13];
		Result.Elements[6] = Right.Elements[4] * Left.Elements[2] + Right.Elements[5] * Left.Elements[6] + Right.Elements[6] * Left.Elements[10] + Right.Elements[7] * Left.Elements[14];
		Result.Elements[7] = Right.Elements[4] * Left.Elements[3] + Right.Elements[5] * Left.Elements[7] + Right.Elements[6] * Left.Elements[11] + Right.Elements[7] * Left.Elements[15];
		Result.Elements[8] = Right.Elements[8] * Left.Elements[0] + Right.Elements[9] * Left.Elements[4] + Right.Elements[10] * Left.Elements[8] + Right.Elements[11] * Left.Elements[12];
		Result.Elements[9] = Right.Elements[8] * Left.Elements[1] + Right.Elements[9] * Left.Elements[5] + Right.Elements[10] * Left.Elements[9] + Right.Elements[11] * Left.Elements[13];
		Result.Elements[10] = Right.Elements[8] * Left.Elements[2] + Right.Elements[9] * Left.Elements[6] + Right.Elements[10] * Left.Elements[10] + Right.Elements[11] * Left.Elements[14];
		Result.Elements[11] = Right.Elements[8] * Left.Elements[3] + Right.Elements[9] * Left.Elements[7] + Right.Elements[10] * Left.Elements[11] + Right.Elements[11] * Left.Elements[15];
		Result.Elements[12] = Right.Elements[12] * Left.Elements[0] + Right.Elements[13] * Left.Elements[4] + Right.Elements[14] * Left.Elements[8] + Right.Elements[15] * Left.Elements[12];
		Result.Elements[13] = Right.Elements[12] * Left.Elements[1] + Right.Elements[13] * Left.Elements[5] + Right.Elements[14] * Left.Elements[9] + Right.Elements[15] * Left.Elements[13];
		Result.Elements[14] = Right.Elements[12] * Left.Elements[2] + Right.Elements[13] * Left.Elements[6] + Right.Elements[14] * Left.Elements[10] + Right.Elements[15] * Left.Elements[14];
		Result.Elements[15] = Right.Elements[12] * Left.Elements[3] + Right.Elements[13] * Left.Elements[7] + Right.Elements[14] * Left.Elements[11] + Right.Elements[15] * Left.Elements[15];


		return Result;
	}

	Matrix4x4& operator*=(const Matrix4x4& Right)
	{
		*this = Multiply(*this, Right);

		return *this;
	}
};