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

        // // градиент
        // for (uint32_t row = 0; row < imgData.height; ++row)
        // {
        //     for (uint32_t col = 0; col < imgData.width; ++col)
        //     {
        //         for (uint32_t channel = 0; channel < imgData.samplesPerPixel; ++channel)
        //         {
        //             if (channel == 0)
        //                 imgData.at(row, col, channel) = static_cast<uint16_t>((row + col) % 65536);
        //             else if (channel == 1)
        //                 imgData.at(row, col, channel) = static_cast<uint16_t>((row * 2) % 65536);
        //             else
        //                 imgData.at(row, col, channel) = static_cast<uint16_t>((col * 2) % 65536);
        //         }
        //     }
        // }
        std::string outputFilename = "binary/output.bin";
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
