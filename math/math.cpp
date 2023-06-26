/***************************************************************/
/*                      scalar math                            */
/***************************************************************/
i32 RoundF32ToI32(f32 Real)
{
    i32 Result = (i32)roundf(Real);
    return Result;
}

u32 RoundF32ToU32(f32 Real)
{
    u32 Result = (u32)roundf(Real);
    return Result;
}

i32 FloorF32ToI32(f32 Real)
{
    i32 Result = (i32)floorf(Real);
    return Result;
}

i32 CeilingF32ToI32(f32 Real)
{
    i32 Result = (i32)ceilf(Real);
    return Result;
}

i32 TruncateF32ToI32(f32 Real)
{
    i32 Result = (i32)(Real);
    return Result;
}

u32 TruncateF32ToU32(f32 Real)
{
    u32 Result = (u32)(Real);
    return Result;
}

f32 Square(f32 A)
{
    return A * A;
}

f32 SquareRoot(f32 A)
{
    f32 Result = sqrtf(A);
    return Result;
}

/***************************************************************/
/*                      V2 math                                */
/***************************************************************/
struct v2
{
    f32 X;
    f32 Y;
};

v2 V2(f32 X, f32 Y)
{
    v2 Result;
    Result.X = X;
    Result.Y = Y;
    return Result;
}

v2 operator+(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return Result;
}

v2 &operator+=(v2 &A, v2 B)
{
    A.X += B.X;
    A.Y += B.Y;
    return A;
}

v2 operator-(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    return Result;
}

v2 operator-(v2 A)
{
    v2 Result;
    Result.X = -A.X;
    Result.Y = -A.Y;
    return Result;
}

v2 &operator-=(v2 &A, v2 B)
{
    A.X -= B.X;
    A.Y -= B.Y;
    return A;
}

v2 operator*(f32 A, v2 B)
{
    v2 Result;
    Result.X = A * B.X;
    Result.Y = A * B.Y;
    return Result;
}

v2 operator*(v2 A, f32 B)
{
    v2 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    return Result;
}

v2 &operator*=(v2 &A, f32 B)
{
    A.X *= B;
    A.Y *= B;
    return A;
}

v2 operator/(v2 A, f32 B)
{
    v2 Result;
    Result.X = A.X / B;
    Result.Y = A.Y / B;
    return Result;
}

v2 &operator/=(v2 &A, f32 B)
{
    A.X /= B;
    A.Y /= B;
    return A;
}

f32 InnerProduct(v2 A, v2 B)
{
    f32 Result = A.X * B.X + A.Y * B.Y;
    return Result;
}

f32 LengthSquared(v2 A)
{
    f32 Result = InnerProduct(A, A);
    return Result;
}

f32 Length(v2 A)
{
    f32 Result = SquareRoot(LengthSquared(A));
    return Result;
}

v2 Normalize(v2 A)
{
    v2 Result;
    Result = A / SquareRoot(LengthSquared(A));
    return Result;
}


/***************************************************************/
/*                      V3 math                                */
/***************************************************************/
struct v3
{
    f32 X;
    f32 Y;
    f32 Z;
};

v3 V3(f32 X, f32 Y, f32 Z)
{
    v3 Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    return Result;
}

v3 operator+(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    return Result;
}

v3 &operator+=(v3 &A, v3 B)
{
    A.X += B.X;
    A.Y += B.Y;
    A.Z += B.Z;
    return A;
}

v3 operator-(v3 A, v3 B)
{
    v3 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    return Result;
}

v3 operator-(v3 A)
{
    v3 Result;
    Result.X = -A.X;
    Result.Y = -A.Y;
    Result.Z = -A.Z;
    return Result;
}

v3 &operator-=(v3 &A, v3 B)
{
    A.X -= B.X;
    A.Y -= B.Y;
    A.Z -= B.Z;
    return A;
}

v3 operator*(f32 A, v3 B)
{
    v3 Result;
    Result.X = A * B.X;
    Result.Y = A * B.Y;
    Result.Z = A * B.Z;
    return Result;
}

v3 operator*(v3 A, f32 B)
{
    v3 Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    Result.Z = A.Z * B;
    return Result;
}

v3 &operator*=(v3 &A, f32 B)
{
    A.X *= B;
    A.Y *= B;
    A.Z *= B;
    return A;
}

v3 operator/(v3 A, f32 B)
{
    v3 Result;
    Result.X = A.X / B;
    Result.Y = A.Y / B;
    Result.Z = A.Z / B;
    return Result;
}

v3 &operator/=(v3 &A, f32 B)
{
    A.X /= B;
    A.Y /= B;
    A.Z /= B;
    return A;
}

f32 InnerProduct(v3 A, v3 B)
{
    f32 Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;
    return Result;
}

f32 LengthSquared(v3 A)
{
    f32 Result = InnerProduct(A, A);
    return Result;
}

f32 Length(v3 A)
{
    f32 Result = SquareRoot(LengthSquared(A));
    return Result;
}

v3 Normalize(v3 A)
{
    v3 Result;
    Result = A / SquareRoot(LengthSquared(A));
    return Result;
}

/***************************************************************/
/*                      V4 math                                */
/***************************************************************/
struct v4
{
    f32 X;
    f32 Y;
    f32 Z;
    f32 W;
};

/***************************************************************/
/*                      rectangle2 math                        */
/***************************************************************/
struct rectangle2
{
    v2 MinPoint;
    v2 MaxPoint;
};

rectangle2 RectMinMax(v2 Min, v2 Max)
{
    rectangle2 Result = {Min, Max};
    return Result;
}

rectangle2 RectCenterRadius(v2 Center, v2 Radius)
{
    rectangle2 Result = {};
    Result.MinPoint = Center - Radius;
    Result.MaxPoint = Center + Radius;
    return Result;
}

rectangle2 RectCenterDiameter(v2 Center, v2 Diameter)
{
    rectangle2 Result = {};
    Result.MinPoint = Center - 0.5f * Diameter;
    Result.MaxPoint = Center + 0.5f * Diameter;
    return Result;
}

rectangle2 RectMinDiameter(v2 MinPoint, v2 Diameter)
{
    rectangle2 Result = {};
    Result.MinPoint = MinPoint;
    Result.MaxPoint = MinPoint + Diameter;
    return Result;
}

b32 IsPointInRectangle(rectangle2 Rectangle, v2 TestPoint)
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

