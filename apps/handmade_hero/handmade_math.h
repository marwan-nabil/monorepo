#if !defined(HANDMADE_MATH_H)

#define PI 3.14159265359f

////////////////////////////////
//			Scalars			  //
////////////////////////////////

inline f32
Square(f32 A)
{
	return A * A;
}

inline f32
SquareRoot(f32 A)
{
	f32 Result = sqrtf(A);
	return Result;
}

inline f32
SafeRatioN(f32 Dividend, f32 Divisor, f32 AltValue)
{
	f32 Result = AltValue;
	if (Divisor != 0)
	{
		Result = Dividend / Divisor;
	}
	return Result;
}

inline f32
SafeRatio0(f32 Dividend, f32 Divisor)
{
	return SafeRatioN(Dividend, Divisor, 0);
}

inline f32
SafeRatio1(f32 Dividend, f32 Divisor)
{
	return SafeRatioN(Dividend, Divisor, 1.0f);
}

inline f32
Lerp(f32 A, f32 B, f32 T)
{
	f32 Result = (1.0f - T) * A + T * B;
	return Result;
}

inline f32
Clamp(f32 Value, f32 Min, f32 Max)
{
	f32 Result = Value;
	if (Result < Min)
	{
		Result = Min;
	}
	else if (Result > Max)
	{
		Result = Max;
	}
	return Result;
}


inline f32
Clamp01(f32 Value)
{
	return Clamp(Value, 0.0f, 1.0f);
}

////////////////////////////////
//			V2				  //
////////////////////////////////

union v2
{
	struct
	{
		f32 X;
		f32 Y;
	};
	f32 E[2];
};

inline v2 
V2(f32 X, f32 Y)
{
	v2 Result;
	Result.X = X;
	Result.Y = Y;
	return Result;
}

inline v2 
operator+(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;
	return Result;
}

inline v2 &
operator+=(v2 &A, v2 B)
{
	A.X += B.X;
	A.Y += B.Y;
	return A;
}

inline v2
operator-(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X - B.X;
	Result.Y = A.Y - B.Y;
	return Result;
}

inline v2
operator-(v2 A)
{
	v2 Result;
	Result.X = -A.X;
	Result.Y = -A.Y;
	return Result;
}

inline v2 &
operator-=(v2 &A, v2 B)
{
	A.X -= B.X;
	A.Y -= B.Y;
	return A;
}

inline v2
operator*(f32 A, v2 B)
{
	v2 Result;
	Result.X = A * B.X;
	Result.Y = A * B.Y;
	return Result;
}

inline v2
operator*(v2 A, f32 B)
{
	return B * A;
}

inline v2 
Hadamard(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X * B.X;
	Result.Y = A.Y * B.Y;
	return Result;
}

inline v2 &
operator*=(v2 &A, f32 B)
{
	A.X *= B;
	A.Y *= B;
	return A;
}

inline v2
operator/(v2 A, f32 B)
{
	v2 Result;
	Result.X = A.X / B;
	Result.Y = A.Y / B;
	return Result;
}

inline f32
Inner(v2 A, v2 B)
{
	f32 Result = A.X * B.X + A.Y * B.Y;
	return Result;
}

inline f32
LengthSquared(v2 A)
{
	f32 Result = Inner(A, A);
	return Result;
}

inline f32
Length(v2 A)
{
	f32 Result = SquareRoot(LengthSquared(A));
	return Result;
}

inline v2
Normalize(v2 A)
{
	v2 Result;
	Result = A / SquareRoot(LengthSquared(A));
	return Result;
}

inline v2
Clamp01(v2 A)
{
	v2 Result;
	Result.X = Clamp01(A.X);
	Result.Y = Clamp01(A.Y);
	return Result;
}

////////////////////////////////
//			V3				  //
////////////////////////////////

union v3
{
	struct
	{
		f32 X;
		f32 Y;
		f32 Z;
	};
	struct
	{
		f32 Red;
		f32 Green;
		f32 Blue;
	};
	struct
	{
		v2 XY;
		f32 Ignored0;
	};
	struct
	{
		f32 Ignored1;
		v2 YZ;
	};
	f32 E[3];
};

inline v3
V3(f32 X, f32 Y, f32 Z)
{
	v3 Result;
	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	return Result;
}

inline v3
V3(v2 XY, f32 Z)
{
	v3 Result;
	Result.X = XY.X;
	Result.Y = XY.Y;
	Result.Z = Z;
	return Result;
}

inline v3
operator+(v3 A, v3 B)
{
	v3 Result;
	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;
	Result.Z = A.Z + B.Z;
	return Result;
}

inline v3 &
operator+=(v3 &A, v3 B)
{
	A.X += B.X;
	A.Y += B.Y;
	A.Z += B.Z;
	return A;
}

inline v3
operator-(v3 A, v3 B)
{
	v3 Result;
	Result.X = A.X - B.X;
	Result.Y = A.Y - B.Y;
	Result.Z = A.Z - B.Z;
	return Result;
}

inline v3
operator-(v3 A)
{
	v3 Result;
	Result.X = -A.X;
	Result.Y = -A.Y;
	Result.Z = -A.Z;
	return Result;
}

inline v3
operator/(v3 A, f32 B)
{
	v3 Result;
	Result.X = A.X / B;
	Result.Y = A.Y / B;
	Result.Z = A.Z / B;
	return Result;
}

inline v3
operator*(f32 A, v3 B)
{
	v3 Result;
	Result.X = A * B.X;
	Result.Y = A * B.Y;
	Result.Z = A * B.Z;
	return Result;
}

inline v3
operator*(v3 A, f32 B)
{
	return B * A;
}

inline v3 &
operator*=(v3 &A, f32 B)
{
	A.X *= B;
	A.Y *= B;
	A.Z *= B;
	return A;
}

inline v3 &
operator-=(v3 &A, v3 B)
{
	A.X -= B.X;
	A.Y -= B.Y;
	A.Z -= B.Z;
	return A;
}

inline v3
Hadamard(v3 A, v3 B)
{
	v3 Result;
	Result.X = A.X * B.X;
	Result.Y = A.Y * B.Y;
	Result.Z = A.Z * B.Z;
	return Result;
}

inline f32
Inner(v3 A, v3 B)
{
	f32 Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;
	return Result;
}

inline f32
LengthSquared(v3 A)
{
	f32 Result = Inner(A, A);
	return Result;
}

inline f32
Length(v3 A)
{
	f32 Result = SquareRoot(LengthSquared(A));
	return Result;
}

inline v3
Normalize(v3 A)
{
	v3 Result;
	Result = A / SquareRoot(LengthSquared(A));
	return Result;
}

inline v3
Clamp01(v3 A)
{
	v3 Result;
	Result.X = Clamp01(A.X);
	Result.Y = Clamp01(A.Y);
	Result.Z = Clamp01(A.Z);
	return Result;
}

////////////////////////////////
//			V4				  //
////////////////////////////////

union v4
{
	struct
	{
		f32 X;
		f32 Y;
		f32 Z;
		f32 W;
	};
	struct
	{
		f32 Red;
		f32 Green;
		f32 Blue;
		f32 Alpha;
	};
	f32 E[4];
};

inline v4
V4(f32 X, f32 Y, f32 Z, f32 W)
{
	v4 Result;
	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	Result.W = W;
	return Result;
}

////////////////////////////////
//			rectangle2   	  //
////////////////////////////////

struct rectangle2
{
	v2 MinPoint;
	v2 MaxPoint;
};

inline rectangle2
RectMinMax(v2 Min, v2 Max)
{
	rectangle2 Result = {Min, Max};
	return Result;
}

inline rectangle2
RectCenterRadius(v2 Center, v2 Radius)
{
	rectangle2 Result = {};
	Result.MinPoint = Center - Radius;
	Result.MaxPoint = Center + Radius;
	return Result;
}

inline rectangle2
RectCenterDiameter(v2 Center, v2 Diameter)
{
	rectangle2 Result = {};
	Result.MinPoint = Center - 0.5f * Diameter;
	Result.MaxPoint = Center + 0.5f * Diameter;
	return Result;
}

inline rectangle2
RectMinDiameter(v2 MinPoint, v2 Diameter)
{
	rectangle2 Result = {};
	Result.MinPoint = MinPoint;
	Result.MaxPoint = MinPoint + Diameter;
	return Result;
}

inline b32
IsInRectangle(rectangle2 Rectangle, v2 TestPoint)
{
	b32 Result =
	(
		TestPoint.X >= Rectangle.MinPoint.X &&
		TestPoint.Y >= Rectangle.MinPoint.Y &&
		TestPoint.X < Rectangle.MaxPoint.X &&
		TestPoint.Y < Rectangle.MaxPoint.Y
	);

	return Result;
}

inline rectangle2
ExpandRectangle(rectangle2 Rectangle, v2 Increment)
{
	rectangle2 Result = Rectangle;
	v2 HalfIncrementVector = 0.5f * Increment;
	Result.MinPoint -= HalfIncrementVector;
	Result.MaxPoint += HalfIncrementVector;
	return Result;
}

inline v2
GetMinCorner(rectangle2 Rect)
{
	v2 Result = Rect.MinPoint;
	return Result;
}

inline v2
GetMaxCorner(rectangle2 Rect)
{
	v2 Result = Rect.MaxPoint;
	return Result;
}

inline v2
GetCenter(rectangle2 Rect)
{
	v2 Result = 0.5f * (Rect.MinPoint + Rect.MaxPoint);
	return Result;
}

internal b32
DoRectanglesOverlap(rectangle2 A, rectangle2 B)
{
	b32 NotOverlappingCondition =
	(
		(B.MaxPoint.X < A.MinPoint.X) || (B.MinPoint.X > A.MaxPoint.X)
		||
		(B.MaxPoint.Y < A.MinPoint.Y) || (B.MinPoint.Y > A.MaxPoint.Y)
	);

	return !NotOverlappingCondition;
}

inline v2
GetbarycentricPoint(rectangle2 Volume, v2 Point)
{
	v2 Result;
	Result.X = SafeRatio0((Point.X - Volume.MinPoint.X), (Volume.MaxPoint.X - Volume.MinPoint.X));
	Result.Y = SafeRatio0((Point.Y - Volume.MinPoint.Y), (Volume.MaxPoint.Y - Volume.MinPoint.Y));
	return Result;
}

////////////////////////////////
//			rectangle3   	  //
////////////////////////////////

struct rectangle3
{
	v3 MinPoint;
	v3 MaxPoint;
};

inline rectangle3
RectMinMax(v3 Min, v3 Max)
{
	rectangle3 Result = {Min, Max};
	return Result;
}

inline rectangle3
RectCenterRadius(v3 Center, v3 Radius)
{
	rectangle3 Result = {};
	Result.MinPoint = Center - Radius;
	Result.MaxPoint = Center + Radius;
	return Result;
}

inline rectangle3
RectCenterDiameter(v3 Center, v3 Diameter)
{
	rectangle3 Result = {};
	Result.MinPoint = Center - 0.5f * Diameter;
	Result.MaxPoint = Center + 0.5f * Diameter;
	return Result;
}

inline rectangle3
RectMinDiameter(v3 MinPoint, v3 Diameter)
{
	rectangle3 Result = {};
	Result.MinPoint = MinPoint;
	Result.MaxPoint = MinPoint + Diameter;
	return Result;
}

inline b32
IsInRectangle(rectangle3 Rectangle, v3 TestPoint)
{
	b32 Result =
	(
		TestPoint.X >= Rectangle.MinPoint.X &&
		TestPoint.Y >= Rectangle.MinPoint.Y &&
		TestPoint.Z >= Rectangle.MinPoint.Z &&
		TestPoint.X < Rectangle.MaxPoint.X &&
		TestPoint.Y < Rectangle.MaxPoint.Y && 
		TestPoint.Z < Rectangle.MaxPoint.Z
	);
	return Result;
}

inline rectangle3
ExpandRectangle(rectangle3 Rectangle, v3 Increment)
{
	rectangle3 Result = Rectangle;
	v3 HalfIncrementVector = 0.5f * Increment;
	Result.MinPoint -= HalfIncrementVector;
	Result.MaxPoint += HalfIncrementVector;
	return Result;
}

inline v3
GetMinCorner(rectangle3 Rect)
{
	v3 Result = Rect.MinPoint;
	return Result;
}

inline v3
GetMaxCorner(rectangle3 Rect)
{
	v3 Result = Rect.MaxPoint;
	return Result;
}

inline v3
GetCenter(rectangle3 Rect)
{
	v3 Result = 0.5f * (Rect.MinPoint + Rect.MaxPoint);
	return Result;
}

inline b32
DoRectanglesOverlap(rectangle3 A, rectangle3 B)
{
	b32 NotOverlappingCondition =
	(
		(B.MaxPoint.X <= A.MinPoint.X) || (B.MinPoint.X >= A.MaxPoint.X) ||
		(B.MaxPoint.Y <= A.MinPoint.Y) || (B.MinPoint.Y >= A.MaxPoint.Y) ||
		(B.MaxPoint.Z <= A.MinPoint.Z) || (B.MinPoint.Z >= A.MaxPoint.Z)
	);

	return !NotOverlappingCondition;
}

inline v3
GetbarycentricPoint(rectangle3 Volume, v3 Point)
{
	v3 Result;
	Result.X = SafeRatio0((Point.X - Volume.MinPoint.X), (Volume.MaxPoint.X - Volume.MinPoint.X));
	Result.Y = SafeRatio0((Point.Y - Volume.MinPoint.Y), (Volume.MaxPoint.Y - Volume.MinPoint.Y));
	Result.Z = SafeRatio0((Point.Z - Volume.MinPoint.Z), (Volume.MaxPoint.Z - Volume.MinPoint.Z));
	return Result;
}

inline rectangle2
Rectangle3ToRectangle2(rectangle3 A)
{
	rectangle2 Result;
	Result.MinPoint = A.MinPoint.XY;
	Result.MaxPoint = A.MaxPoint.XY;
	return Result;
}

#define HANDMADE_MATH_H
#endif