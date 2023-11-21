void PrintCharacterColored(char Character, u8 Color, u8 X, u8 Y)
{
    u8 *ScreenBuffer = (u8 *)VGA_SCREEN_BUFFER_ADDRESS;
    u32 ScreenBufferOffset = 2 * (X + Y * VGA_SCREEN_WIDTH);
    ScreenBuffer[ScreenBufferOffset] = Character;
    ScreenBuffer[ScreenBufferOffset + 1] = Color;
}