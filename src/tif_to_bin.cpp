#include <tiffio.h>
#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include "BinaryHeader.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Использование: " << argv[0] << " <входной TIFF-файл> <выходной бинарный файл>" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];


    TIFFSetWarningHandler(NULL);
    TIFF *tif = TIFFOpen(inputFilename.c_str(), "r");

    if (!tif)
    {
        std::cerr << "Ошибка: Не удалось открыть файл " << inputFilename << std::endl;
        return 1;
    }

    BinaryHeader header;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &header.width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &header.height);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &header.samplesPerPixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &header.bitsPerSample);

    if (header.bitsPerSample != 16)
    {
        std::cerr << "Ошибка: Изображение не 16-битное!" << std::endl;
        TIFFClose(tif);
        return 1;
    }

    size_t scanlineSize = TIFFScanlineSize(tif) / 2; // 16 bits = 2 bytes
    std::vector<uint16_t> scanline(scanlineSize);

    std::ofstream outFile(outputFilename, std::ios::binary);
    if (!outFile)
    {
        std::cerr << "Ошибка: Не удалось создать бинарный файл " << outputFilename << std::endl;
        TIFFClose(tif);
        return 1;
    }

    header.dataOffset = sizeof(BinaryHeader); // дата сразу после заголовка

    outFile.write(reinterpret_cast<const char *>(&header), sizeof(BinaryHeader));

    for (uint32_t row = 0; row < header.height; ++row)
    {
        if (TIFFReadScanline(tif, scanline.data(), row) < 0)
        {
            std::cerr << "Ошибка при чтении строки: " << row << std::endl;
            TIFFClose(tif);
            outFile.close();
            return 1;
        }

        outFile.write(reinterpret_cast<const char *>(scanline.data()), scanline.size() * sizeof(uint16_t));
    }

    TIFFClose(tif);
    outFile.close();

    std::cout << "Изображение успешно записано в бинарный файл: " << outputFilename << std::endl;

    return 0;
}
