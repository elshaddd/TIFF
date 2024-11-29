#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include "BinaryHeader.hpp"

struct ImageData
{
    std::vector<uint16_t> data;
    uint32_t width;
    uint32_t height;
    uint32_t samplesPerPixel;

    uint16_t &at(uint32_t row, uint32_t col, uint32_t channel)
    {
        return data[(row * width + col) * samplesPerPixel + channel];
    }

    const uint16_t &at(uint32_t row, uint32_t col, uint32_t channel) const
    {
        return data[(row * width + col) * samplesPerPixel + channel];
    }
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

    ImageData imgData;
    imgData.width = header.width;
    imgData.height = header.height;
    imgData.samplesPerPixel = header.samplesPerPixel;

    // alloc
    imgData.data.resize(header.width * header.height * header.samplesPerPixel);

    inFile.read(reinterpret_cast<char *>(imgData.data.data()),
                imgData.data.size() * sizeof(uint16_t));
    if (!inFile)
    {
        throw std::runtime_error("Ошибка: Не удалось прочитать данные изображения!");
    }

    inFile.close();
    return imgData;
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

    outFile.write(reinterpret_cast<const char *>(imgData.data.data()),
                  imgData.data.size() * sizeof(uint16_t));

    outFile.close();
    std::cout << "Бинарный файл успешно сохранён: " << filename << std::endl;
}
