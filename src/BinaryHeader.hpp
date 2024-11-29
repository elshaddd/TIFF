#include <cstdint>

struct BinaryHeader
{
    uint32_t width;           // ширина изображения
    uint32_t height;          // высота изображения
    uint16_t samplesPerPixel; // количество каналов
    uint16_t bitsPerSample;   // бит на канал
    uint64_t dataOffset;      // смещение данных от начала файла
};