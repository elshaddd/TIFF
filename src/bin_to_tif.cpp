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
        std::cerr << "Использование: " << argv[0] << " <входной бинарный файл> <выходной TIFF-файл>" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];

    std::ifstream inFile(inputFilename, std::ios::binary);
    if (!inFile)
    {
        std::cerr << "Ошибка: Не удалось открыть бинарный файл " << inputFilename << std::endl;
        return 1;
    }

    BinaryHeader header;
    inFile.read(reinterpret_cast<char *>(&header), sizeof(BinaryHeader));
    if (!inFile)
    {
        std::cerr << "Ошибка: Не удалось прочитать заголовок из файла " << inputFilename << std::endl;
        return 1;
    }

    std::cout << "Читаем изображение из бинарного файла:" << std::endl;
    std::cout << "Ширина: " << header.width << ", Высота: " << header.height << std::endl;
    std::cout << "Каналов: " << header.samplesPerPixel << ", Бит на канал: " << header.bitsPerSample << std::endl;

    if (header.bitsPerSample != 16)
    {
        std::cerr << "Ошибка: Ожидается изображение с 16 битами на канал!" << std::endl;
        return 1;
    }

    inFile.seekg(header.dataOffset, std::ios::beg);
    if (!inFile)
    {
        std::cerr << "Ошибка: Не удалось перейти к данным изображения!" << std::endl;
        return 1;
    }

    TIFF *tif = TIFFOpen(outputFilename.c_str(), "w");
    if (!tif)
    {
        std::cerr << "Ошибка: Не удалось создать TIFF-файл " << outputFilename << std::endl;
        return 1;
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, header.width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, header.height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, header.samplesPerPixel);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, header.bitsPerSample);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    size_t scanlineSize = header.width * header.samplesPerPixel;
    std::vector<uint16_t> scanline(scanlineSize);

    for (uint32_t row = 0; row < header.height; ++row)
    {
        inFile.read(reinterpret_cast<char *>(scanline.data()), scanline.size() * sizeof(uint16_t));
        if (!inFile)
        {
            std::cerr << "Ошибка: Недостаточно данных в бинарном файле!" << std::endl;
            TIFFClose(tif);
            return 1;
        }

        if (TIFFWriteScanline(tif, scanline.data(), row) < 0)
        {
            std::cerr << "Ошибка при записи строки: " << row << std::endl;
            TIFFClose(tif);
            return 1;
        }
    }

    inFile.close();
    TIFFClose(tif);

    std::cout << "TIFF-изображение успешно создано: " << outputFilename << std::endl;

    return 0;
}
