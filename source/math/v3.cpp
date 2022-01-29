
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
