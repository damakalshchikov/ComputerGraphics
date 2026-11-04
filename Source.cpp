// Подключаем необходимые библиотеки для работы, объявляем прототип оконной функции WndProc и задаём имя класса окна WinName
#include <windows.h>
#include <tchar.h>
#include <fstream>
#include <string>
#include <vector>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR WinName[] = _T("MainFrame");
// Функции записи и чтения образов
// Запись фрагмента изображения по номеру (k)
void SaveHDCToFile(HDC hdc, int k, int width, int height, const char *filename)
{
    // Создаем контекст устройства в памяти
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);
    // Копируем изображение из hdc в memDC
    BitBlt(memDC, 0, 0, width, height, hdc, k * width, 0, SRCCOPY);
    // Получаем информацию о битмапе
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height; // Отрицательное значение для верхнего левого угла
    bi.biPlanes = 1;
    bi.biBitCount = 32; // 32 бита на пиксель
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    // Выделяем память для пикселей
    std::vector<BYTE> pixels(width * height * 4);
    // Получаем пиксели
    GetDIBits(memDC, hBitmap, 0, height, &pixels[0], (BITMAPINFO *)&bi, DIB_RGB_COLORS);
    // Создаем заголовок BMP файла
    BITMAPFILEHEADER bf;
    bf.bfType = 0x4D42; // "BM"
    bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pixels.size();
    bf.bfReserved1 = 0;
    bf.bfReserved2 = 0;
    bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    // Сохраняем в файл
    std::ofstream file(filename, std::ios::binary);
    if (file)
    {
        file.write((char *)&bf, sizeof(BITMAPFILEHEADER));
        file.write((char *)&bi, sizeof(BITMAPINFOHEADER));
        file.write((char *)&pixels[0], pixels.size());
        file.close();
    }
    // Освобождаем ресурсы
    SelectObject(memDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memDC);
}
// Чтение образа из файла и вывод на экран
bool LoadBMPToHDC(const char *filename, HDC hdc)
{
    // Открываем файл
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        return false;
    }
    // Читаем заголовок BMP файла
    BITMAPFILEHEADER bf;
    file.read((char *)&bf, sizeof(BITMAPFILEHEADER));
    if (bf.bfType != 0x4D42)
    { // Проверяем, что это BMP файл
        return false;
    }
    // Читаем информацию о битмапе
    BITMAPINFOHEADER bi;
    file.read((char *)&bi, sizeof(BITMAPINFOHEADER));
    // Проверяем, что битмап имеет формат, который мы можем обработать
    if (bi.biBitCount != 24 && bi.biBitCount != 32)
    {
        return false;
    }
    // Вычисляем размер изображения
    int width = bi.biWidth;
    int height = abs(bi.biHeight);
    int bitCount = bi.biBitCount;
    int rowSize = ((width * bitCount + 31) / 32) * 4; // Размер строки в байтах
    int imageSize = rowSize * height;
    // Выделяем память для пикселей
    std::vector<BYTE> pixels(imageSize);
    // Пропускаем заголовок и читаем пиксели
    file.seekg(bf.bfOffBits, std::ios::beg);
    file.read((char *)&pixels[0], imageSize);
    // Создаем контекст устройства в памяти
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);
    // Устанавливаем информацию о битмапе
    BITMAPINFO bmi;
    bmi.bmiHeader = bi;
    bmi.bmiColors[0].rgbBlue = 0;
    bmi.bmiColors[0].rgbGreen = 0;
    bmi.bmiColors[0].rgbRed = 0;
    bmi.bmiColors[0].rgbReserved = 0;
    // Устанавливаем пиксели в битмап
    SetDIBits(memDC, hBitmap, 0, height, &pixels[0], &bmi, DIB_RGB_COLORS);
    // Определение номера изображения
    std::string firstChar(1, filename[0]);
    int k = std::stoi(firstChar);
    // Копируем битмап в hdc
    BitBlt(hdc, (k - 1) * width, height, k * width, height * 2, memDC, 0, 0, SRCCOPY);
    // Освобождаем ресурсы
    SelectObject(memDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memDC);
    return true;
}
// Главная функция приложения
int APIENTRY WinMain(HINSTANCE This, HINSTANCE Prev, LPSTR cmd,
                     int mode)
{
    HWND hWnd;   // Дескриптор главного окна программы
    MSG msg;     // Структура для хранения сообщений
    WNDCLASS wc; // Класс окна
    // Определение класса окна
    wc.hInstance = This;
    wc.lpszClassName = WinName;                 // Имя класса окна
    wc.lpfnWndProc = WndProc;                   // Функция окна
    wc.style = CS_HREDRAW | CS_VREDRAW;         // Стиль окна
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Стандартная иконка
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);   // Стандартный курсор
    wc.lpszMenuName = NULL;                     // Нет меню
    wc.cbClsExtra = 0;                          // Нет дополнительных данных класса
    wc.cbWndExtra = 0;                          // Нет дополнительных данных окна
    // Заполнение окна белым цветом
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // установка цвета фона
    if (!RegisterClass(&wc))
        return 0; // Регистрация класса окна
    // Создание окна
    hWnd = CreateWindow(WinName,                   // Имя класса окна
                        _T("Работа с М-образами"), // Заголовок окна
                        WS_OVERLAPPEDWINDOW,       // Стиль окна
                        CW_USEDEFAULT,             // X
                        CW_USEDEFAULT,             // Y
                        CW_USEDEFAULT,             // Width
                        CW_USEDEFAULT,             // Height
                        HWND_DESKTOP,              // Дескриптор родительского окна
                        NULL,                      // Нет меню
                        This,                      // Дескриптор приложения
                        NULL);                     // Дополнительной информации нет
    ShowWindow(hWnd, mode);                        // Показать окно
    // Цикл обработки сообщений
    while (GetMessage(&msg, NULL, 0, 0)) // цикл получения сообщений
    {
        TranslateMessage(&msg); // Функция трансляции кодов нажатой клавиши
        DispatchMessage(&msg);  // Посылает сообщение функции WndProc()
    }
    return 0; // при положительном завершении программы в главную функцию возвращается "0"
}
// Обработка сообщений окна
// Оконная функция вызываемая операционной системой
// и получает сообщения из очереди для данного приложения
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
                         WPARAM wParam, LPARAM lParam)
{
    int k;
    int SIZE_X = 200;
    int SIZE_Y = 200;
    std::string name;
    static HBITMAP hBitmap[4] = {};
    static HDC hdcMem[4] = {};
    static HGDIOBJ hOldBitmap[4] = {};
    // Обработчик сообщений
    switch (message)
    {
    case WM_CREATE:
    {
        // Получение контекста устройства для окна
        HDC hdc = GetDC(hWnd);
        for (k = 0; k < 4; k++)
        {
            // Создание совместимого контекста устройства
            hdcMem[k] = CreateCompatibleDC(hdc);
            // Создание Bitmap
            hBitmap[k] = CreateCompatibleBitmap(hdc, SIZE_X, SIZE_Y);
            // Выбор Bitmap в контекст устройства
            hOldBitmap[k] = SelectObject(hdcMem[k], hBitmap[k]);
            // Очистка Bitmap
            PatBlt(hdcMem[k], 0, 0, SIZE_X, SIZE_Y, WHITENESS);
        }
        // Заливка четырёх образов. Проходим вложенным циклом по всем точкам квадрата
        for (int i = 0; i < SIZE_X; i++)
        {
            for (int j = 0; j < SIZE_Y; j++)
            {
                // SetPixel красит одну точку (i, j) в указанный цвет, его задаёт цветовая RGB модель
                // При смешении трёх составляющих (красный, зелёный, синий) получается итоговый цвет
                SetPixel(hdcMem[0], i, j, RGB(255, 165, 0)); // Комбинация 255, 165, 0 даёт оранжевый цвет
                SetPixel(hdcMem[1], i, j, RGB(128, 0, 128)); // Комбинация 128, 0, 128 даёт фиолетовый цвет
                SetPixel(hdcMem[2], i, j, RGB(0, 128, 128)); // Комбинация 0, 128, 128 даёт бирюзовый цвет
                SetPixel(hdcMem[3], i, j, RGB(255, 255, 0)); // Комбинация 255, 255, 0 даёт жёлтый цвет
            }
        }
        // Освобождение контекста устройства
        ReleaseDC(hWnd, hdc);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        for (k = 0; k < 4; k++)
        {
            // Копирование Bitmap на экран
            BitBlt(hdc, k * SIZE_X, 5, (k + 1) * SIZE_X, SIZE_Y + 5, hdcMem[k], 0, 0, SRCCOPY);
            // Сохранение образов в файл
            name = std::to_string(k + 1) + ".BMP";
            SaveHDCToFile(hdc, k, SIZE_X, SIZE_Y, name.c_str());
            // Чтение образов из файла с именем формата "число.BMP" и вывод на hdc
            LoadBMPToHDC(name.c_str(), hdc);
        }
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        for (k = 0; k < 4; k++)
        {
            // Очистка ресурсов
            SelectObject(hdcMem[k], hOldBitmap[k]);
            DeleteObject(hBitmap[k]);
            DeleteDC(hdcMem[k]);
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
