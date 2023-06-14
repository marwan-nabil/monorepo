
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

bool32 IsPointInRectangle(rectangle2 Rectangle, v2 TestPoint)
{
    bool32 Result =
    (
        TestPoint.X >= Rectangle.MinPoint.X &&
        TestPoint.Y >= Rectangle.MinPoint.Y &&
        TestPoint.X < Rectangle.MaxPoint.X &&
        TestPoint.Y < Rectangle.MaxPoint.Y
    );
    return Result;
}
