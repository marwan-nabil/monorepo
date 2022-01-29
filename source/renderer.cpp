
struct render_buffer
{
    void *Memory;
    i32 Width;
    i32 Height;
    i32 BytesPerPixel;
    i32 Pitch;
};

struct color
{
    f32 A, R, G, B;
};

void
DrawRectangle(render_buffer *Buffer, v2 MinCorner, v2 MaxCorner, color RectColor)
{
    i32 MinX = RoundF32ToI32(MinCorner.X);
    i32 MinY = RoundF32ToI32(MinCorner.Y);
    i32 MaxX = RoundF32ToI32(MaxCorner.X);
    i32 MaxY = RoundF32ToI32(MaxCorner.Y);

    if (MinX < 0)
    {
        MinX = 0;
    }
    if (MinY < 0)
    {
        MinY = 0;
    }
    if (MaxX > Buffer->Width)
    {
        MaxX = Buffer->Width;
    }
    if (MaxY > Buffer->Height)
    {
        MaxY = Buffer->Height;
    }

    u32 Color = (u32)(RoundF32ToU32(RectColor.A * 255.0f) << 24 |
                      RoundF32ToU32(RectColor.R * 255.0f) << 16 |
                      RoundF32ToU32(RectColor.G * 255.0f) << 8 |
                      RoundF32ToU32(RectColor.B * 255.0f));

    u8 *Row = ((u8 *)Buffer->Memory + MinX * Buffer->BytesPerPixel + MinY * Buffer->Pitch);
    for (i32 Y = MinY; Y < MaxY; Y++)
    {
        u32 *Pixel = (u32 *)Row;
        for (i32 X = MinX; X < MaxX; X++)
        {
            f32 SourceAlpha = (f32)((Color >> 24) & 0xff) / 255.0f;
            f32 SourceRed = (f32)((Color >> 16) & 0xff);
            f32 SourceGreen = (f32)((Color >> 8) & 0xff);
            f32 SourceBlue = (f32)((Color >> 0) & 0xff);

            f32 DestinationRed = (f32)((*Pixel >> 16) & 0xff);
            f32 DestinationGreen = (f32)((*Pixel >> 8) & 0xff);
            f32 DestinationBlue = (f32)((*Pixel >> 0) & 0xff);

            f32 ResultRed = SourceAlpha * SourceRed + (1 - SourceAlpha) * DestinationRed;
            f32 ResultGreen = SourceAlpha * SourceGreen + (1 - SourceAlpha) * DestinationGreen;
            f32 ResultBlue = SourceAlpha * SourceBlue + (1 - SourceAlpha) * DestinationBlue;

            *Pixel++ =
                ((u32)(ResultRed + 0.5f) << 16) |
                ((u32)(ResultGreen + 0.5f) << 8) |
                ((u32)(ResultBlue + 0.5f) << 0);
        }
        Row += Buffer->Pitch;
    }
}

void
DrawLine(render_buffer *Buffer, v2 Start, v2 End, u32 LineColor)
{
    i32 MinX = RoundF32ToI32(Start.X);
    i32 MinY = RoundF32ToI32(Start.Y);
    i32 MaxX = RoundF32ToI32(End.X);
    i32 MaxY = RoundF32ToI32(End.Y);

    if (MinX < 0)
    {
        MinX = 0;
    }
    if (MinY < 0)
    {
        MinY = 0;
    }
    if (MaxX > Buffer->Width)
    {
        MaxX = Buffer->Width;
    }
    if (MaxY > Buffer->Height)
    {
        MaxY = Buffer->Height;
    }

    f32 LineMCoefficient = (f32)(MaxY - MinY) / (f32)(MaxX - MinX);

    u8 *DestinationRow = ((u8 *)Buffer->Memory + MinX * Buffer->BytesPerPixel + MinY * Buffer->Pitch);

    for (i32 Y = MinY; Y < MaxY; Y++)
    {
        u32 *Pixel = (u32 *)DestinationRow;

        for (i32 X = MinX; X < MaxX; X++)
        {
            if (RoundF32ToI32(LineMCoefficient * X) == Y)
            {
                *Pixel = LineColor;
            }
            Pixel++;
        }

        DestinationRow += Buffer->Pitch;
    }
}

void DrawFilledCircle(render_buffer *Buffer, v2 CenterPosition, f32 CircleRadius, u32 PointColor)
{
    i32 MinX = RoundF32ToI32(CenterPosition.X - CircleRadius);
    i32 MinY = RoundF32ToI32(CenterPosition.Y - CircleRadius);
    i32 MaxX = RoundF32ToI32(CenterPosition.X + CircleRadius);
    i32 MaxY = RoundF32ToI32(CenterPosition.Y + CircleRadius);

    if (MinX < 0)
    {
        MinX = 0;
    }
    if (MinY < 0)
    {
        MinY = 0;
    }
    if (MaxX > Buffer->Width)
    {
        MaxX = Buffer->Width;
    }
    if (MaxY > Buffer->Height)
    {
        MaxY = Buffer->Height;
    }

    u8 *DestinationRow = ((u8 *)Buffer->Memory + MinX * Buffer->BytesPerPixel + MinY * Buffer->Pitch);

    for (i32 Y = MinY; Y < MaxY; Y++)
    {
        u32 *Pixel = (u32 *)DestinationRow;

        for (i32 X = MinX; X < MaxX; X++)
        {
            f32 PointCenterRelativeX = (f32)(X - MinX) - CircleRadius;
            f32 PointCenterRelativeY = (f32)(Y - MinY) - CircleRadius;

            if (Length(v2{PointCenterRelativeX, PointCenterRelativeY}) <= CircleRadius)
            {
                *Pixel = PointColor;
            }

            Pixel++;
        }

        DestinationRow += Buffer->Pitch;
    }
}

void DrawGraph(render_buffer *Buffer, u32 *DataPoints, u32 XAxisCount, u32 YAxisRange, rectangle2 GraphRectangle)
{
    DrawRectangle(Buffer, GraphRectangle.MinPoint, GraphRectangle.MaxPoint, color{1.0f,1.0f,1.0f,1.0f});
    
    f32 XPadding = 40;
    f32 YPadding = 40;

    f32 XStepSize = (GraphRectangle.MaxPoint.X - GraphRectangle.MinPoint.X - 2 * XPadding) / XAxisCount;
    f32 XAxisLength = XStepSize * XAxisCount;

    v2 XAxisStartPoint = v2{GraphRectangle.MinPoint.X + XPadding, GraphRectangle.MinPoint.Y + YPadding};
    v2 XAxisEndPoint = v2{XAxisStartPoint.X + XAxisLength, XAxisStartPoint.Y};

    DrawLine(Buffer, XAxisStartPoint, XAxisEndPoint, 0xff335577);

    f32 YStepSize = (GraphRectangle.MaxPoint.Y - GraphRectangle.MinPoint.Y - 2 * YPadding) / YAxisRange;
    f32 YAxisLength = YStepSize * YAxisRange;

    v2 YAxisStartPoint = XAxisStartPoint;
    v2 YAxisEndPoint = v2{YAxisStartPoint.X, YAxisStartPoint.Y + YAxisLength};

    DrawLine(Buffer, YAxisStartPoint, YAxisEndPoint, 0xff335577);
}

void
RenderSimulation(render_buffer *Buffer, simulation_state *SimulationState)
{
    // Buffer is 960 X 540 pixels

    //DrawRectangle(Buffer, v2{1, 1}, v2 {400, 400}, .8f, .8f, 0.1f, 1.0f);
    //u32 LineColor = 0xff335577;
    //DrawLine(Buffer, v2{4, 4}, v2{300, 300}, LineColor);
    //DrawFilledCircle(Buffer, v2{40, 70}, 30, LineColor);

    u32 Data[20] = {};
    for (u32 Index = 0; Index < 20; Index++)
    {
        Data[Index] = Index;
    }

    rectangle2 GraphRect;
    GraphRect.MinPoint = v2{0, 0};
    GraphRect.MaxPoint = v2{500, 500};
    DrawGraph(Buffer, Data, 20, 30, GraphRect);
}