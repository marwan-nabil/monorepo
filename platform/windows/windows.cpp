void DisplayRenderBufferInWindow
(
    HWND Window, HDC DeviceContext,
    void *BufferMemory, u32 BufferWidth, u32 BufferHeight,
    BITMAPINFO *BufferBitmapInfo
)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    i32 WindowWidth = ClientRect.right - ClientRect.left;
    i32 WindowHeight = ClientRect.bottom - ClientRect.top;

    if ((WindowWidth == 1920) && (WindowHeight == 1027))
    {
        i32 DestinationWidth = (i32)(BufferWidth * 1.3);
        i32 DestinationHeight = (i32)(BufferHeight * 1.3);
        StretchDIBits
        (
            DeviceContext,
            0, 0, WindowWidth, WindowHeight,
            0, 0, BufferWidth, BufferHeight,
            BufferMemory, BufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY
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
            0, 0, BufferWidth, BufferHeight,
            BufferMemory, BufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY
        );
    }
}