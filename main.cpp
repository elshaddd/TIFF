#include <tiffio.h>
#include <iostream>
#include <vector>
#include <cstdint>

int main()
{
    const char *filename = "images/DSC00494.TIF"; // path

    // open tiff file
    TIFFSetWarningHandler(NULL);
    TIFF *tif = TIFFOpen(filename, "r");
    
    if (!tif)
    {
        std::cerr << "Ошибка: Не удалось открыть файл " << filename << std::endl;
        return 1;
    }

    // parameters of image
    uint32_t width, height;
    uint16_t samplesPerPixel, bitsPerSample;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);

    std::cout << "Размеры изображения: " << width << "x" << height << std::endl;
    std::cout << "Каналов: " << samplesPerPixel << ", Бит на канал: " << bitsPerSample << std::endl;

    // check 16 bits
    if (bitsPerSample != 16)
    {
        std::cerr << "Ошибка: Изображение не 16-битное !" << std::endl;
        TIFFClose(tif);
        return 1;
    }

    // malloc
    size_t scanlineSize = TIFFScanlineSize(tif) / 2; // 16 bits = 2 bytes
    std::vector<uint16_t> scanline(scanlineSize);

    // read
    for (uint32_t row = 0; row < height; ++row)
    {
        if (TIFFReadScanline(tif, scanline.data(), row) < 0)
        {
            std::cerr << "Ошибка при чтении строки: " << row << std::endl;
            TIFFClose(tif);
            return 1;
        }

        // first pixel handling
        for (uint32_t col = 0; col < width; ++col)
        {
            for (uint16_t channel = 0; channel < samplesPerPixel; ++channel)
            {
                // each channel of a pixel
                uint16_t pixelValue = scanline[col * samplesPerPixel + channel];
                std::cout << "Пиксель (" << row << ", " << col << "), Канал "
                          << channel << ": " << pixelValue << std::endl;
            }
            
            // interrupt
            if (col == 0)
            {
                break;
            }
        }

        // interrupt
        if (row == 0)
            break;
    }

    // file close
    TIFFClose(tif);
    std::cout << "Чтение завершено успешно." << std::endl;

    return 0;
}
