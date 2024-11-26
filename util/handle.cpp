#include <tiffio.h>
#include <iostream>
#include <vector>
#include <cstdint>

int main()
{
    const char *inputFilename = "images/DSC00494.TIF"; // origin
    const char *outputFilename = "images/DSC00494_bw.TIF"; // new

    // open tiff file
    TIFFSetWarningHandler(NULL);
    TIFF *inputTif = TIFFOpen(inputFilename, "r");

    if (!inputTif)
    {
        std::cerr << "Ошибка: Не удалось открыть файл " << inputFilename << std::endl;
        return 1;
    }

    // parameters of image
    uint32_t width, height;
    uint16_t samplesPerPixel, bitsPerSample;
    TIFFGetField(inputTif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(inputTif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(inputTif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    TIFFGetField(inputTif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);

    std::cout << "Размеры изображения: " << width << "x" << height << std::endl;
    std::cout << "Каналов: " << samplesPerPixel << ", Бит на канал: " << bitsPerSample << std::endl;

    // check 16 bits
    if (bitsPerSample != 16)
    {
        std::cerr << "Ошибка: Изображение не 16-битное!" << std::endl;
        TIFFClose(inputTif);
        return 1;
    }

    // new file open
    TIFF *outputTif = TIFFOpen(outputFilename, "w");
    if (!outputTif)
    {
        std::cerr << "Ошибка: Не удалось создать файл " << outputFilename << std::endl;
        TIFFClose(inputTif);
        return 1;
    }

    // copy parameters
    TIFFSetField(outputTif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(outputTif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(outputTif, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
    TIFFSetField(outputTif, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
    TIFFSetField(outputTif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(outputTif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(outputTif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(outputTif, width * samplesPerPixel));

    // malloc
    size_t scanlineSize = TIFFScanlineSize(inputTif) / 2; // 16 bits = 2 bytes
    std::vector<uint16_t> inputScanline(scanlineSize);
    std::vector<uint16_t> outputScanline(scanlineSize);

    // handling raws
    for (uint32_t row = 0; row < height; ++row)
    {
        if (TIFFReadScanline(inputTif, inputScanline.data(), row) < 0)
        {
            std::cerr << "Ошибка при чтении строки: " << row << std::endl;
            TIFFClose(inputTif);
            TIFFClose(outputTif);
            return 1;
        }

        // to_gray
        for (uint32_t col = 0; col < width; ++col)
        {
            uint32_t pixelIndex = col * samplesPerPixel;
            uint32_t sum = 0;

            for (uint16_t channel = 0; channel < samplesPerPixel; ++channel)
            {
                sum += inputScanline[pixelIndex + channel];
            }

            // gray = average
            uint16_t grayValue = static_cast<uint16_t>(sum / samplesPerPixel);

            for (uint16_t channel = 0; channel < samplesPerPixel; ++channel)
            {
                outputScanline[pixelIndex + channel] = grayValue;
            }
        }

        // write
        if (TIFFWriteScanline(outputTif, outputScanline.data(), row) < 0)
        {
            std::cerr << "Ошибка при записи строки: " << row << std::endl;
            TIFFClose(inputTif);
            TIFFClose(outputTif);
            return 1;
        }
    }

    // files close
    TIFFClose(inputTif);
    TIFFClose(outputTif);

    std::cout << "Изображение успешно преобразовано в черно-белое." << std::endl;

    return 0;
}
