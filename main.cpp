#include <tiffio.h>
#include <iostream>
#include <vector>
#include <cstdint>

int main()
{
    const char *filename = "DSC00494.TIF"; // Путь к вашему TIFF файлу

    // Открываем TIFF файл
    TIFFSetWarningHandler(NULL);
    TIFF *tif = TIFFOpen(filename, "r");
    
    if (!tif)
    {
        std::cerr << "Ошибка: Не удалось открыть файл " << filename << std::endl;
        return 1;
    }

    // Получение размеров изображения
    uint32_t width, height;
    uint16_t samplesPerPixel, bitsPerSample;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);

    std::cout << "Размеры изображения: " << width << "x" << height << std::endl;
    std::cout << "Каналов: " << samplesPerPixel << ", Бит на канал: " << bitsPerSample << std::endl;

    // Проверяем, что изображение 16-битное
    if (bitsPerSample != 16)
    {
        std::cerr << "Ошибка: Ожидалось 16-битное изображение!" << std::endl;
        TIFFClose(tif);
        return 1;
    }

    // Расчет размера строки в пикселях и выделение памяти для одной строки
    size_t scanlineSize = TIFFScanlineSize(tif) / 2; // 16 бит = 2 байта
    std::vector<uint16_t> scanline(scanlineSize);

    // Чтение пикселей построчно
    for (uint32_t row = 0; row < height; ++row)
    {
        if (TIFFReadScanline(tif, scanline.data(), row) < 0)
        {
            std::cerr << "Ошибка при чтении строки: " << row << std::endl;
            TIFFClose(tif);
            return 1;
        }

        // Пример обработки первого пикселя строки
        for (uint32_t col = 0; col < width; ++col)
        {
            for (uint16_t channel = 0; channel < samplesPerPixel; ++channel)
            {
                // Каждый канал пикселя
                uint16_t pixelValue = scanline[col * samplesPerPixel + channel];
                std::cout << "Пиксель (" << row << ", " << col << "), Канал "
                          << channel << ": " << pixelValue << std::endl;
            }

            if (col == 0)
            {
                break;
            }
        }

        // Прерываем вывод после одной строки для примера
        if (row == 0)
            break;
    }

    // Закрываем файл
    TIFFClose(tif);
    std::cout << "Чтение завершено успешно." << std::endl;

    return 0;
}
