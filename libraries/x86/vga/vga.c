void PrintCharacterColored(char Character, u8 Color, u8 X, u8 Y)
{
    u8 *ScreenBuffer = (u8 *)VGA_SCREEN_BUFFER_ADDRESS;
    u32 ScreenBufferOffset = 2 * (X + Y * VGA_SCREEN_WIDTH);
    ScreenBuffer[ScreenBufferOffset] = Character;
    ScreenBuffer[ScreenBufferOffset + 1] = Color;
}

void ClearScreen()
{
    for (u32 Y = 0; Y < VGA_SCREEN_HEIGHT; Y++)
    {
        for (u32 X = 0; X < VGA_SCREEN_WIDTH; X++)
        {
            PrintCharacterColored('\0', 0x07, X, Y);
        }
    }
}

void SetCursorPosition(u32 X, u32 Y)
{
    u32 LinearPosition = X + Y * VGA_SCREEN_WIDTH;
    WriteByteToOutputPort(0x03D4, 0x0F);
    WriteByteToOutputPort(0x03D5, (u8)(LinearPosition & 0xFF));
    WriteByteToOutputPort(0x03D4, 0x0E);
    WriteByteToOutputPort(0x03D5, (u8)((LinearPosition >> 8) & 0xFF));
}