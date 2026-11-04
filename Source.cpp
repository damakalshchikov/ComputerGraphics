// Подключаем необходимые библиотеки для работы, объявляем прототип оконной функции WndProc и задаём имя класса окна WinName
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define NOMINMAX
#include <windows.h>
#include <tchar.h>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
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
    BITMAPINFOHEADER bi{};
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
    SelectObject(memDC, hOldBitmap); // GetDIBits требует невыбранный bitmap
    GetDIBits(memDC, hBitmap, 0, height, &pixels[0], (BITMAPINFO *)&bi, DIB_RGB_COLORS);
    // Создаем заголовок BMP файла
    BITMAPFILEHEADER bf{};
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
    BITMAPFILEHEADER bf{};
    file.read((char *)&bf, sizeof(BITMAPFILEHEADER));
    if (bf.bfType != 0x4D42)
    { // Проверяем, что это BMP файл
        return false;
    }
    // Читаем информацию о битмапе
    BITMAPINFOHEADER bi{};
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
    BITMAPINFO bmi{};
    bmi.bmiHeader = bi;
    bmi.bmiColors[0].rgbBlue = 0;
    bmi.bmiColors[0].rgbGreen = 0;
    bmi.bmiColors[0].rgbRed = 0;
    bmi.bmiColors[0].rgbReserved = 0;
    // Устанавливаем пиксели в битмап
    SelectObject(memDC, hOldBitmap);
    SetDIBits(memDC, hBitmap, 0, height, &pixels[0], &bmi, DIB_RGB_COLORS);
    SelectObject(memDC, hBitmap);
    // Определение номера изображения
    std::string firstChar(1, filename[0]);
    int k = std::stoi(firstChar);
    // Копируем битмап в hdc
    BitBlt(hdc, ((k - 1) % 3) * width, ((k - 1) / 3) * height,
           width, height, memDC, 0, 0, SRCCOPY);
    // Освобождаем ресурсы
    SelectObject(memDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memDC);
    return true;
}
// Главная функция приложения
int APIENTRY WinMain(HINSTANCE This, HINSTANCE, LPSTR,
                     int mode)
{
    HWND hWnd;     // Дескриптор главного окна программы
    MSG msg{};     // Структура для хранения сообщений
    WNDCLASS wc{}; // Класс окна
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
    RECT rect{0, 0, 960, 640};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    // Создание окна
    hWnd = CreateWindow(WinName,                   // Имя класса окна
                        _T("Работа с М-образами"), // Заголовок окна
                        WS_OVERLAPPEDWINDOW,       // Стиль окна
                        CW_USEDEFAULT,             // X
                        CW_USEDEFAULT,             // Y
                        rect.right - rect.left,    // Width
                        rect.bottom - rect.top,    // Height
                        HWND_DESKTOP,              // Дескриптор родительского окна
                        NULL,                      // Нет меню
                        This,                      // Дескриптор приложения
                        NULL);                     // Дополнительной информации нет
    ShowWindow(hWnd, mode);                        // Показать окно
    // Цикл обработки сообщений
    int status;
    while ((status = GetMessage(&msg, NULL, 0, 0)) > 0) // цикл получения сообщений
    {
        TranslateMessage(&msg); // Функция трансляции кодов нажатой клавиши
        DispatchMessage(&msg);  // Посылает сообщение функции WndProc()
    }
    return status < 0 ? 1 : static_cast<int>(msg.wParam); // при положительном завершении программы в главную функцию возвращается "0"
}
// Обработка сообщений окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    const int SIZE_X = 320, SIZE_Y = 320;
    static HBITMAP hBitmap[5] = {};
    static HDC hdcMem[5] = {};
    static HGDIOBJ hOldBitmap[5] = {};
    switch (message)
    {
    case WM_CREATE:
    {
        HDC hdc = GetDC(hWnd);
        // Отрисовка bitmap'ов
        for (int k = 0; k < 5; ++k)
        {
            int width = k == 4 ? 2 * SIZE_X : SIZE_X;
            hdcMem[k] = CreateCompatibleDC(hdc);
            hBitmap[k] = CreateCompatibleBitmap(hdc, width, SIZE_Y);
            if (!hdcMem[k] || !hBitmap[k])
            {
                for (int t = 0; t <= k; ++t)
                {
                    if (hOldBitmap[t])
                        SelectObject(hdcMem[t], hOldBitmap[t]);
                    if (hBitmap[t])
                        DeleteObject(hBitmap[t]);
                    if (hdcMem[t])
                        DeleteDC(hdcMem[t]);
                }
                ReleaseDC(hWnd, hdc);
                return -1;
            }
            hOldBitmap[k] = SelectObject(hdcMem[k], hBitmap[k]);
            PatBlt(hdcMem[k], 0, 0, width, SIZE_Y, WHITENESS);
        }

        for (int i = 0; i < SIZE_X; ++i)
            for (int j = 0; j < SIZE_Y; ++j)
            {
                double x = -1.5 + (i + 0.5) * 3.0 / SIZE_X;
                double y = 1.5 - (j + 0.5) * 3.0 / SIZE_Y;
                double w1 = 1 - (x + 0.4) * (x + 0.4) - y * y;
                double w2 = 0.25 - (x - 0.6) * (x - 0.6) - y * y;
                double d = std::sqrt(w1 * w1 + w2 * w2);
                double w = w1 + w2 - d;
                if (d < 1e-12)
                    d = 1e-12;
                double wx = -2 * (x + 0.4) - 2 * (x - 0.6) - (w1 * (-2 * (x + 0.4)) + w2 * (-2 * (x - 0.6))) / d;
                double wy = -4 * y - (w1 * (-2 * y) + w2 * (-2 * y)) / d;
                double n[4] = {-wx, -wy, 1, x * wx + y * wy - w};
                double length = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2] + n[3] * n[3]);
                for (int k = 0; k < 4; ++k)
                {
                    int gray = static_cast<int>(127.5 * (1 + n[k] / length));
                    SetPixel(hdcMem[k], i, j, RGB(gray, gray, gray));
                }
            }
        // Объединяем последние две bitmap, чтобы отобразить пересечение и подписи
        for (int i = 0; i < 2 * SIZE_X; ++i)
            for (int j = 0; j < SIZE_Y; ++j)
            {
                double x = -3 + (i + 0.5) * 6.0 / (2 * SIZE_X);
                double y = 1.5 - (j + 0.5) * 3.0 / SIZE_Y;
                double w1 = 1 - (x + 0.4) * (x + 0.4) - y * y;
                double w2 = 0.25 - (x - 0.6) * (x - 0.6) - y * y;
                double w = w1 + w2 - std::sqrt(w1 * w1 + w2 * w2);
                double t = 1 + w / 5;
                if (t < 0)
                    t = 0;
                if (t > 1)
                    t = 1;
                COLORREF color = w >= 0 ? RGB(255, 255, 255) : RGB(0, 0, int(255 * t));
                SetPixel(hdcMem[4], i, j, color);
            }
        // Расчёт площади
        double area1 = 0, area2 = 0, areaIntersection = 0;
        const int N = 2000;
        double h = 3.0 / N;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
            {
                double x = -1.5 + (i + 0.5) * h;
                double y = -1.5 + (j + 0.5) * h;
                double w1 = 1 - (x + 0.4) * (x + 0.4) - y * y;
                double w2 = 0.25 - (x - 0.6) * (x - 0.6) - y * y;
                double w = w1 + w2 - std::sqrt(w1 * w1 + w2 * w2);
                if (w1 >= 0)
                    area1 += h * h;
                if (w2 >= 0)
                    area2 += h * h;
                if (w >= 0)
                    areaIntersection += h * h;
            }
        HFONT font = CreateFont(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial"));
        HGDIOBJ oldFont = SelectObject(hdcMem[4], font);
        SetBkMode(hdcMem[4], OPAQUE);
        SetBkColor(hdcMem[4], RGB(255, 255, 255));
        SetTextColor(hdcMem[4], RGB(0, 0, 0));
        TCHAR text[128];
        _sntprintf(text, 128, _T("S первого круга: %.6f"), area1);
        TextOut(hdcMem[4], 12, 12, text, lstrlen(text));
        _sntprintf(text, 128, _T("S второго круга: %.6f"), area2);
        TextOut(hdcMem[4], SIZE_X + 12, 38, text, lstrlen(text));
        _sntprintf(text, 128, _T("S пересечения: %.6f"), areaIntersection);
        TextOut(hdcMem[4], 140, SIZE_Y - 28, text, lstrlen(text));
        SelectObject(hdcMem[4], oldFont);
        DeleteObject(font);

        for (int k = 0; k < 4; ++k)
        {
            std::string name = std::to_string(k + 1) + ".BMP";
            SaveHDCToFile(hdcMem[k], 0, SIZE_X, SIZE_Y, name.c_str());
        }
        SaveHDCToFile(hdcMem[4], 0, 2 * SIZE_X, SIZE_Y, "intersection.BMP");
        HDC allDC = CreateCompatibleDC(hdc);
        HBITMAP allBitmap = CreateCompatibleBitmap(hdc, 3 * SIZE_X, 2 * SIZE_Y);
        if (allDC && allBitmap)
        {
            HGDIOBJ old = SelectObject(allDC, allBitmap);
            for (int k = 0; k < 4; ++k)
                BitBlt(allDC, (k % 3) * SIZE_X, (k / 3) * SIZE_Y, SIZE_X, SIZE_Y,
                       hdcMem[k], 0, 0, SRCCOPY);
            BitBlt(allDC, SIZE_X, SIZE_Y, 2 * SIZE_X, SIZE_Y, hdcMem[4], 0, 0, SRCCOPY);
            SaveHDCToFile(allDC, 0, 3 * SIZE_X, 2 * SIZE_Y, "lab2.BMP");
            SelectObject(allDC, old);
        }
        if (allBitmap)
            DeleteObject(allBitmap);
        if (allDC)
            DeleteDC(allDC);
        ReleaseDC(hWnd, hdc);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        for (int k = 0; k < 4; ++k)
            BitBlt(hdc, (k % 3) * SIZE_X, (k / 3) * SIZE_Y, SIZE_X, SIZE_Y,
                   hdcMem[k], 0, 0, SRCCOPY);
        BitBlt(hdc, SIZE_X, SIZE_Y, 2 * SIZE_X, SIZE_Y, hdcMem[4], 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        for (int k = 0; k < 5; ++k)
        {
            SelectObject(hdcMem[k], hOldBitmap[k]);
            DeleteObject(hBitmap[k]);
            DeleteDC(hdcMem[k]);
        }
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
