#!/bin/bash

INPUT_FILE="$1"
OUTPUT_FILE="$2"

# Название исходного файла с кодом и исполняемого файла
SOURCE_FILE="src/tif_to_bin.cpp"
EXECUTABLE_FILE="app/tif_to_bin"

# Проверяем наличие исходного кода
if [[ ! -f "$SOURCE_FILE" ]]; then
    echo "Ошибка: Файл с исходным кодом $SOURCE_FILE не найден!"
    exit 1
fi

# Компилируем программу
echo "Компиляция программы..."
g++ -o "$EXECUTABLE_FILE" "$SOURCE_FILE" -ltiff
if [[ $? -ne 0 ]]; then
    echo "Ошибка: Компиляция не удалась!"
    exit 1
fi
echo "Компиляция успешна."

# Проверяем существование входного файла
if [[ ! -f "$INPUT_FILE" ]]; then
    echo "Ошибка: Входной файл $INPUT_FILE не существует!"
    exit 1
fi

# Запускаем программу
echo "Запуск программы..."
./"$EXECUTABLE_FILE" "$INPUT_FILE" "$OUTPUT_FILE"
