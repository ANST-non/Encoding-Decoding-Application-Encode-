#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <locale.h>
#include <windows.h>
using namespace std;

class QuotedPrintable
{
public:
    //-----------------------Кодирование строки---------------------------
    static string encode(const string& input)
    {
        stringstream result;
        int lineLength = 0;

        for (size_t i = 0; i < input.length(); i++)
        {
            unsigned char c = input[i];

            // Проверяем, нужно ли кодировать символ
            if (needsEncoding(c))
            {
                // Кодируем символ
                string encoded = "=" + byteToHex(c);

                // Проверяем, не превысит ли строка максимальную длину
                if (lineLength + encoded.length() > MAX_LINE_LENGTH - 2)
                {
                    result << "=\r\n";
                    lineLength = 0;
                }

                // Добавляем закодированный символ и увеличиваем счетчик длины
                result << encoded;
                lineLength += encoded.length();

            }
            else {
                // Проверяем специальные случаи для пробела и табуляции
                if (c == ' ' || c == '\t') {
                    // Если символ в конце строки - кодируем
                    if (i == input.length() - 1 ||
                        (i < input.length() - 1 && (input[i + 1] == '\r' || input[i + 1] == '\n'))) {

                        string encoded = "=" + byteToHex(c);

                        // Проверяем длину 
                        if (lineLength + encoded.length() > MAX_LINE_LENGTH - 2)
                        {
                            result << "=\r\n";
                            lineLength = 0;
                        }

                        // Добавляем закодированный символ и увеличиваем счетчик длины
                        result << encoded;
                        lineLength += encoded.length();
                        continue;
                    }
                }

                // Проверяем максимальную длину строки
                if (lineLength + 1 > MAX_LINE_LENGTH - 2) {
                    result << "=\r\n";
                    lineLength = 0;
                }

                // Переносим символ как есть
                result << c;
                lineLength++;

                // Обработка символов конца строки
                if (c == '\r' && i < input.length() - 1 && input[i + 1] == '\n') {
                    result << '\n';
                    i++;
                    lineLength = 0;
                }
                else if (c == '\n') {
                    lineLength = 0;
                }
            }
        }

        return result.str();
    }

    //---------------------Декодирование строки---------------------
    static string decode(const string& input) {
        stringstream result;

        for (size_t i = 0; i < input.length(); i++)
        {
            char c = input[i];

            if (c == '=')
            {
                // Проверяем, достаточно ли символов для чтения
                if (i + 2 >= input.length())
                {
                    // Неполная последовательность - оставляем как есть
                    result << c;
                    continue;
                }

                // Проверяем на перевод строки
                if (input[i + 1] == '\r' && input[i + 2] == '\n')
                {
                    i += 2; // Пропускаем \r\n
                    continue;
                }

                // Декодируем hex-последовательность
                if (isxdigit(input[i + 1]) && isxdigit(input[i + 2]))
                {
                    string hexStr = input.substr(i + 1, 2);
                    unsigned char decodedChar = hexToByte(hexStr);
                    result << decodedChar;
                    i += 2; // Пропускаем два hex-символа
                }
                else {
                    // Некорректная последовательность - оставляем как есть
                    result << c;
                }
            }
            else {
                result << c;
            }
        }

        return result.str();
    }

private:
    // Максимальная длина строки - 76
    static const int MAX_LINE_LENGTH = 76;

    // Проверяем, какие нужно кодировать символ, а какие нет
    static bool needsEncoding(unsigned char c) {
        // Не кодируем символы с 33 по 60 и с 62 по 126 в ASCII 
        if ((c >= 33 && c <= 60) || (c >= 62 && c <= 126))
        {
            return false;
        }

        // ТАВ и пробел обрабатываются отдельно
        if (c == 9 || c == 32)
        {
            return false;
        }

        // Символы конца строки
        if (c == 13 || c == 10)
        {
            return false;
        }

        // Всё остально кодируем
        return true;
    }

    // Преобразует байт в hex-строку
    static string byteToHex(unsigned char c)
    {
        stringstream ss;
        ss << uppercase << hex << setw(2) << setfill('0') << (int)c;
        return ss.str();
    }

    // Преобразует hex-строку в байт
    static unsigned char hexToByte(const string& hex)
    {
        return static_cast<unsigned char>(stoi(hex, nullptr, 16));
    }
};

// Функция для кодирования фразы
void EncodePhrase()
{
    cout << "\n";
    cout << "-----------------Кодировка Фразы-----------------\n\n";
    string input;
    cout << "> ";
    getline(cin, input);
    cout << "Фраза: ";
    cout << quoted(input);
    cout << "\n";

    string encoded = QuotedPrintable::encode(input);

    cout << "-----------------------------------------------\n";
    cout << "> ";
    cout << encoded << "\n";

    cout << "\n";
    cout << "Нажмите Enter, чтобы продолжить...";
    cin.get();
}

// Функция для декодирования фразы
void DecodePhrase()
{
    cout << "\n";
    cout << "----------------Расшифрофка фразы----------------\n\n";
    string input;
    cout << "> ";
    getline(cin, input);
    cout << "Фраза: ";
    cout << quoted(input);
    cout << "\n";

    string decoded = QuotedPrintable::decode(input);

    cout << "-----------------------------------------------\n";
    cout << "> ";
    cout << decoded << "\n";

    cout << "\n";
    cout << "Нажмите Enter, чтобы продолжить...";
    cin.get();
}

// Функция для кодирования файла
void EncodeFile()
{
    cout << "\n";
    cout << "----------------Кодировка Файла----------------\n\n";
    string filename;
    cout << "Введите имя файла ('name.txt'): ";
    getline(cin, filename);

    // Проверяем точно ли такой файл существует
    ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        cout << "\n";
        cout << "Ошибка: файл не удалось открыть\n";
        cout << "Нажмите Enter, чтобы продолжить...";
        cin.get();
        return;
    }

    // Читаем содержимое файла построчно и закрываем его
    string content;
    string line;
    while (getline(inputFile, line))
    {
        content += line + "\n";
    }
    inputFile.close();

    // Кодируем, что прочли
    string encoded = QuotedPrintable::encode(content);

    // Создаем новый файл для кодированной части и записываем туда или выводим ошибку
    string outputFilename = "encoded_" + filename;
    ofstream outputFile(outputFilename);
    if (outputFile.is_open())
    {
        outputFile << encoded;
        outputFile.close();

        cout << "\n";
        cout << "Файл успешно закодирован.\n";
        cout << "Результат сохранен в: " << quoted(outputFilename) << "\n";
    }
    else
    {
        cout << "\n";
        cout << "Ошибка: не удалось создать выходной файл\n";
    }
    cout << "\n";
    cout << "Нажмите Enter, чтобы продолжить...";
    cin.get();
}

// Функция для декодирования файла
void DecodeFile()
{
    cout << "----------------Decode File----------------\n\n";

    string filename;
    cout << "Введите имя файла ('name.txt'): ";
    getline(cin, filename);

    // Проверяем можно ли открыть файл, который запросили
    ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        cout << "\n";
        cout << "Ошибка: файл не удалось открыть'" << quoted(filename) << "'\n";
        cout << "Нажмите Enter, чтобы продолжить...";
        cin.get();
        return;
    }

    // Читаем содержимое файла построчно и запоминаем
    string content;
    string line;
    while (getline(inputFile, line))
    {
        content += line + "\n";
    }
    inputFile.close();

    // Декодируем, что прочли
    string decoded = QuotedPrintable::decode(content);

    // Создаем новый файл с соответсвующим названием
    string outputFilename;
    outputFilename = "decoded_" + filename;

    // Записываем в файл, что декодировали, и выводим результат
    ofstream outputFile(outputFilename);
    if (outputFile.is_open())
    {
        outputFile << decoded;
        outputFile.close();

        cout << "\n";
        cout << "Файл успешно расшифрован!\n";
        cout << "Результат сохранен в " << quoted(outputFilename) << "\n";
    }
    else // Или выводим ошибку
    {
        cout << "\n";
        cout << "Ошибка: не удалось создать выходной файл\n";
    }
    cout << "\n";
    cout << "Нажмите Enter, чтобы продолжить...";
    cin.get();
}

// Функции для управления стрелочками
//map<string, string> test;
//
//int indexUI = 0;
//void SetIndexUI(int i) {
//    indexUI = i;
//}
//int GetIndexUI() {
//    return 0;
//}
//
//void UpdateUI() {
//    
//}

void About()
{
    cout << "\n";
    cout << "Приложение создано для ... \n";
    cout << "Приложение создано для ... \n";
    cout << "Приложение создано для ... \n";
    cout << "Приложение создано для ... \n";
    cout << "Приложение создано для ... \n";
    cout << "\n";
    cout << "Нажмите Enter, чтобы продолжить...";
    cin.get();
}

// 0 - сброс
// 30 - черный
// 31m - красный
// 32m - зеленый
// 33m - желтый
// 34m - синий
// 35m - пурпурный
// 36m - голубой
// 37m - белый
// 1m - жирный
// 4m - подчеркнутый
// 91m - ярко-красный
// 92m - ярко-зеленый
// 93m - ярко-желтый
// 94m - ярко-синий
// 95m - ярко-пурпурный
// 96m - ярко-голубой
// 40m - чёрный фон
// 41m - красный фон
// 42m - зелёный фон
// 43m - жёлтый фон
// 44m - синий фон
// 45m - пурпурный фон
// 46m - голубой фон
// 47m - белый фон
// Функция для цветов (ANSI коды, которые выше)
string color(int code)
{
    return "\033[" + to_string(code) + "m";
}

// Основная функция вывода приложения
int main()
{
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    string choice;

    cout << "  ----------------------------------------------- \n" << color(0);
    cout << " |  _____  _   _  _____   _______  _____  _____  |\n";
    cout << " | |  ___|| \\ | |/  ___\\ /  ___  \\|  __ \\|  ___| |\n";
    cout << " | | |__  |  \\| || |     | |   | || |  ||| |__   |\n";
    cout << " | |  __| |     || |     | |   | || |  |||  __|  |\n";
    cout << " | | |___ | |\\  || |___  | |___| || |__||| |___  |\n";
    cout << " | \\_____/|_| \\_|\\_____/ \\_______/|_____/\\_____/ |\n";
    cout << "  ----------------------------------------------- \n";
    cout << "      Приложение для кодирования/декодирования\n";
    cout << "      отдельных фраз или текстовых документов\n";
    cout << "                 (by EncodeSquad)";
    cout << "\n\n";

    while (true)
    {
        cout << endl;
        cout << color(37) << " 1. " << color(36) << "Закодировать фразу" << color(0) << endl;
        cout << color(37) << " 2. " << color(35) << "Расшифровать фразу" << color(0) << endl;
        cout << color(37) << " 3. " << color(36) << "Закодировать файл" << color(0) << endl;
        cout << color(37) << " 4. " << color(35) << "Расшифровать файл" << color(0) << endl;
        cout << color(37) << " 5. " << color(31) << "Выход" << color(0) << endl;

        cout << "\n";
        cout << color(37) << " Выберите действие: " << color(0);
        cin >> choice;
        cin.ignore();


        switch (choice[0])
        {
        case '1':
            EncodePhrase();
            break;
        case '2':
            DecodePhrase();
            break;
        case '3':
            EncodeFile();
            break;
        case '4':
            DecodeFile();
            break;
        case '5':
            cout << "\n";
            cout << "Спасибо, что используете 'Encode'!\n\n";
            return 0;
            break;
        default:
            cout << "\n";
            cout << "Ошибка: неправильный выбор.\n";
            cout << "Нажмите Enter, чтобы продолжить...";
            cin.get();
            break;
        }
    }
}