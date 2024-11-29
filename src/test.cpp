#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include "ImageData.hpp"

int main()
{
    try
    {
        std::string inputFilename = "binary/494.bin";
        ImageData imgData = binaryToArray(inputFilename);

        std::cout << "Ширина: " << imgData.width
                  << ", Высота: " << imgData.height
                  << ", Каналы: " << imgData.samplesPerPixel << std::endl;

        // gradient
        for (uint32_t row = 0; row < imgData.height; ++row)
        {
            for (uint32_t col = 0; col < imgData.width; ++col)
            {
                imgData.data[row][col][0] = static_cast<uint16_t>((row + col) % 65536);
                imgData.data[row][col][1] = static_cast<uint16_t>((row * 2) % 65536);
                imgData.data[row][col][2] = static_cast<uint16_t>((col * 2) % 65536);
            }
        }
        std::string outputFilename = "binary/gradient.bin";
        arrayToBinary(outputFilename, imgData);

        std::cout << "Данные успешно обработаны и сохранены!" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
