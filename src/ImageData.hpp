#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include "BinaryHeader.hpp"

struct ImageData
{
    std::vector<std::vector<std::vector<uint16_t>>> data;
    uint32_t width;
    uint32_t height;
    uint32_t samplesPerPixel;
};

ImageData binaryToArray(const std::string &filename)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile)
    {
        throw std::runtime_error("Ошибка: Не удалось открыть бинарный файл: " + filename);
    }

    BinaryHeader header;
    inFile.read(reinterpret_cast<char *>(&header), sizeof(BinaryHeader));
    if (!inFile)
    {
        throw std::runtime_error("Ошибка: Не удалось прочитать заголовок!");
    }

    if (header.bitsPerSample != 16)
    {
        throw std::runtime_error("Ошибка: Изображение не 16-битное!");
    }

    std::vector<std::vector<std::vector<uint16_t>>> image(
        header.height,
        std::vector<std::vector<uint16_t>>(header.width,
                                           std::vector<uint16_t>(header.samplesPerPixel)));

    for (uint32_t row = 0; row < header.height; ++row)
    {
        for (uint32_t col = 0; col < header.width; ++col)
        {
            inFile.read(reinterpret_cast<char *>(image[row][col].data()),
                        header.samplesPerPixel * sizeof(uint16_t));
            if (!inFile)
            {
                throw std::runtime_error("Ошибка: Не удалось прочитать данные пикселей!");
            }
        }
    }

    inFile.close();

    return {image, header.width, header.height, header.samplesPerPixel};
}

void arrayToBinary(const std::string &filename, const ImageData &imgData)
{
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile)
    {
        throw std::runtime_error("Ошибка: Не удалось создать бинарный файл: " + filename);
    }

    BinaryHeader header;
    header.width = imgData.width;
    header.height = imgData.height;
    header.samplesPerPixel = imgData.samplesPerPixel;
    header.bitsPerSample = 16;
    header.dataOffset = sizeof(BinaryHeader);

    outFile.write(reinterpret_cast<const char *>(&header), sizeof(BinaryHeader));

    for (uint32_t row = 0; row < imgData.height; ++row)
    {
        for (uint32_t col = 0; col < imgData.width; ++col)
        {
            outFile.write(reinterpret_cast<const char *>(imgData.data[row][col].data()),
                          imgData.samplesPerPixel * sizeof(uint16_t));
        }
    }

    outFile.close();
    std::cout << "Бинарный файл успешно сохранён: " << filename << std::endl;
}
