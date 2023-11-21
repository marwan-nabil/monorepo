void TestVGA()
{
    for (u32 Y = 0; Y < VGA_SCREEN_HEIGHT; Y++)
    {
        for (u32 X = 0; X < VGA_SCREEN_WIDTH; X++)
        {
            u8 Color = X + Y;
            char Character = X + Y;
            PrintCharacterColored(Character, Color, X, Y);
        }
    }
}