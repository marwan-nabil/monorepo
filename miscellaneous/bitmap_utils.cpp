inline void
WriteBitmapImage(u32 *Pixels, u32 WidthInPixels, u32 HeightInPixels, const char *FileName)
{
    u32 OutputPixelSize = WidthInPixels * HeightInPixels * sizeof(u32);

    bitmap_header BitmapHeader = {};
    BitmapHeader.FileType = 0x4D42;
    BitmapHeader.FileSize = sizeof(BitmapHeader) + OutputPixelSize;
    BitmapHeader.BitmapOffset = sizeof(BitmapHeader);
    BitmapHeader.Size = sizeof(BitmapHeader) - 14;
    BitmapHeader.Width = WidthInPixels;
    BitmapHeader.Height = -(i32)HeightInPixels;
    BitmapHeader.Planes = 1;
    BitmapHeader.BitsPerPixel = 32;
    BitmapHeader.Compression = 0;
    BitmapHeader.SizeOfBitmap = OutputPixelSize;
    BitmapHeader.HorizontalResolution = 0;
    BitmapHeader.VerticalResolution = 0;
    BitmapHeader.ColorsUsed = 0;
    BitmapHeader.ColorsImportant = 0;

    FILE *OutputFile = fopen(FileName, "wb");
    if (OutputFile)
    {
        fwrite(&BitmapHeader, sizeof(BitmapHeader), 1, OutputFile);
        fwrite((void *)Pixels, OutputPixelSize, 1, OutputFile);
        fclose(OutputFile);
    }
    else
    {
        printf("ERROR: unable to create the output bitmap file.\n");
    }
}