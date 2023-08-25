void DisplayRenderBufferInWindow(HWND Window, HDC DeviceContext, rendering_buffer *Buffer)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    i32 WindowWidth = ClientRect.right - ClientRect.left;
    i32 WindowHeight = ClientRect.bottom - ClientRect.top;

    if ((WindowWidth == 1920) && (WindowHeight == 1027))
    {
        i32 DestinationWidth = (i32)(Buffer->Width * 1.3);
        i32 DestinationHeight = (i32)(Buffer->Height * 1.3);
        StretchDIBits
        (
            DeviceContext,
            0, 0, WindowWidth, WindowHeight,
            0, 0, Buffer->Width, Buffer->Height,
            Buffer->Memory, &Buffer->Bitmapinfo, DIB_RGB_COLORS, SRCCOPY
        );
    }
    else
    {
        i32 OffsetX = 10;
        i32 OffsetY = 10;

        PatBlt(DeviceContext, 0, 0, WindowWidth, OffsetY, WHITENESS); // top bar
        PatBlt(DeviceContext, 0, 0, OffsetX, WindowHeight, WHITENESS); // left bar
        PatBlt(DeviceContext, WindowWidth - OffsetX, 0, OffsetX, WindowHeight, WHITENESS); // right bar
        PatBlt(DeviceContext, 0, WindowHeight - OffsetY, WindowWidth, OffsetY, WHITENESS); // bottom bar

        StretchDIBits
        (
            DeviceContext,
            OffsetX, OffsetY, WindowWidth - 2 * OffsetX, WindowHeight - 2 * OffsetX,
            0, 0, Buffer->Width, Buffer->Height,
            Buffer->Memory, &Buffer->Bitmapinfo, DIB_RGB_COLORS, SRCCOPY
        );
    }
}

void DrawRectangle(rendering_buffer *Buffer, v2 MinCorner, v2 MaxCorner, v4 RectColor)
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
    if (MaxX > (i32)Buffer->Width)
    {
        MaxX = (i32)Buffer->Width;
    }
    if (MaxY > (i32)Buffer->Height)
    {
        MaxY = (i32)Buffer->Height;
    }

    u32 Color =
    (
        (RoundF32ToU32(RectColor.Alpha * 255.0f) << 24) |
        (RoundF32ToU32(RectColor.Red * 255.0f) << 16) |
        (RoundF32ToU32(RectColor.Green * 255.0f) << 8) |
        RoundF32ToU32(RectColor.Blue * 255.0f)
    );

    u8 *Row = 
        (u8 *)Buffer->Memory + 
        MinX * Buffer->BytesPerPixel + 
        MinY * Buffer->Pitch;

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
DrawLine(rendering_buffer *Buffer, v2 Start, v2 End, u32 LineColor)
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
    if (MaxX > (i32)Buffer->Width)
    {
        MaxX = (i32)Buffer->Width;
    }
    if (MaxY > (i32)Buffer->Height)
    {
        MaxY = (i32)Buffer->Height;
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

void DrawFilledCircle(rendering_buffer *Buffer, v2 CenterPosition, f32 CircleRadius, u32 PointColor)
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
    if (MaxX > (i32)Buffer->Width)
    {
        MaxX = (i32)Buffer->Width;
    }
    if (MaxY > (i32)Buffer->Height)
    {
        MaxY = (i32)Buffer->Height;
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

void DrawGraph(rendering_buffer *Buffer, u32 *DataPoints, u32 XAxisCount, u32 YAxisRange, rectangle2 GraphRectangle)
{
    DrawRectangle(Buffer, GraphRectangle.MinPoint, GraphRectangle.MaxPoint, V4(1.0f, 1.0f, 1.0f, 1.0f));

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
