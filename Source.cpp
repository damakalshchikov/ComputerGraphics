#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define NOMINMAX
#include <windows.h>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <fstream>

const int CELL = 320;
const int WIDTH = 3 * CELL;
const int HEIGHT = 2 * CELL;

HDC imageDC = nullptr;
HBITMAP bitmap = nullptr;
HGDIOBJ oldBitmap = nullptr;
std::uint32_t *pixels = nullptr;

void SaveBMP()
{
    GdiFlush();
    BITMAPFILEHEADER bf{};
    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(bi);
    bi.biWidth = WIDTH;
    bi.biHeight = -HEIGHT;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = WIDTH * HEIGHT * 4;
    bf.bfType = 0x4D42;
    bf.bfOffBits = sizeof(bf) + sizeof(bi);
    bf.bfSize = bf.bfOffBits + bi.biSizeImage;
    std::ofstream file("lab2.BMP", std::ios::binary);
    file.write(reinterpret_cast<const char *>(&bf), sizeof(bf));
    file.write(reinterpret_cast<const char *>(&bi), sizeof(bi));
    file.write(reinterpret_cast<const char *>(pixels), bi.biSizeImage);
}

void DrawImages()
{
    // Четыре М-образа пересечения двух кругов
    for (int j = 0; j < CELL; ++j)
        for (int i = 0; i < CELL; ++i)
        {
            double x = -1.5 + (i + 0.5) * 3.0 / CELL;
            double y = 1.5 - (j + 0.5) * 3.0 / CELL;
            double w1 = 1.0 - (x + 0.4) * (x + 0.4) - y * y;
            double w2 = 0.25 - (x - 0.6) * (x - 0.6) - y * y;
            double d = std::sqrt(w1 * w1 + w2 * w2);
            double w = w1 + w2 - d;
            if (d < 1e-12)
                d = 1e-12;
            double wx = -2 * (x + 0.4) - 2 * (x - 0.6) - (w1 * (-2 * (x + 0.4)) + w2 * (-2 * (x - 0.6))) / d;
            double wy = -4 * y - (w1 * (-2 * y) + w2 * (-2 * y)) / d;
            double n[4] = {-wx, -wy, 1.0, x * wx + y * wy - w};
            double length = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2] + n[3] * n[3]);
            for (int k = 0; k < 4; ++k)
            {
                int gray = static_cast<int>(127.5 * (1 + n[k] / length));
                int px = (k % 3) * CELL + i;
                int py = (k / 3) * CELL + j;
                pixels[py * WIDTH + px] = (gray << 16) | (gray << 8) | gray;
            }
        }

    // Нижние две ячейки
    for (int j = 0; j < CELL; ++j)
        for (int i = 0; i < 2 * CELL; ++i)
        {
            double x = -3.0 + (i + 0.5) * 6.0 / (2 * CELL);
            double y = 1.5 - (j + 0.5) * 3.0 / CELL;
            double w1 = 1.0 - (x + 0.4) * (x + 0.4) - y * y;
            double w2 = 0.25 - (x - 0.6) * (x - 0.6) - y * y;
            double w = w1 + w2 - std::sqrt(w1 * w1 + w2 * w2);
            double t = 1.0 + w / 5.0;
            if (t < 0)
                t = 0;
            if (t > 1)
                t = 1;
            pixels[(CELL + j) * WIDTH + CELL + i] =
                w >= 0 ? 0x00FFFFFF : static_cast<int>(255 * t);
        }

    // Считаем площади
    double area1 = 0, area2 = 0, areaIntersection = 0;
    const int N = 2000;
    const double h = 3.0 / N;
    for (int j = 0; j < N; ++j)
        for (int i = 0; i < N; ++i)
        {
            double x = -1.5 + (i + 0.5) * h;
            double y = -1.5 + (j + 0.5) * h;
            double w1 = 1.0 - (x + 0.4) * (x + 0.4) - y * y;
            double w2 = 0.25 - (x - 0.6) * (x - 0.6) - y * y;
            double w = w1 + w2 - std::sqrt(w1 * w1 + w2 * w2);
            if (w1 >= 0)
                area1 += h * h;
            if (w2 >= 0)
                area2 += h * h;
            if (w >= 0)
                areaIntersection += h * h;
        }
    HFONT font = CreateFontW(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
    HGDIOBJ oldFont = SelectObject(imageDC, font);
    SetBkColor(imageDC, RGB(255, 255, 255));
    SetTextColor(imageDC, RGB(0, 0, 0));
    wchar_t text[128];
    std::swprintf(text, 128, L"S первого круга: %.6f", area1);
    TextOutW(imageDC, CELL + 12, CELL + 12, text, lstrlenW(text));
    std::swprintf(text, 128, L"S второго круга: %.6f", area2);
    TextOutW(imageDC, 2 * CELL + 12, CELL + 38, text, lstrlenW(text));
    std::swprintf(text, 128, L"S пересечения: %.6f", areaIntersection);
    TextOutW(imageDC, CELL + 140, HEIGHT - 28, text, lstrlenW(text));
    GdiFlush();
    SelectObject(imageDC, oldFont);
    DeleteObject(font);
}

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    switch (message)
    {
    case WM_CREATE:
    {
        HDC dc = GetDC(window);
        imageDC = CreateCompatibleDC(dc);
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = WIDTH;
        info.bmiHeader.biHeight = -HEIGHT;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        void *bits = nullptr;
        bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        ReleaseDC(window, dc);
        if (!imageDC || !bitmap)
        {
            if (bitmap)
                DeleteObject(bitmap);
            if (imageDC)
                DeleteDC(imageDC);
            return -1;
        }
        pixels = static_cast<std::uint32_t *>(bits);
        oldBitmap = SelectObject(imageDC, bitmap);
        DrawImages();
        SaveBMP();
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(window, &ps);
        BitBlt(dc, 0, 0, WIDTH, HEIGHT, imageDC, 0, 0, SRCCOPY);
        EndPaint(window, &ps);
        return 0;
    }
    case WM_DESTROY:
        SelectObject(imageDC, oldBitmap);
        DeleteObject(bitmap);
        DeleteDC(imageDC);
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(window, message, wp, lp);
    }
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show)
{
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.lpszClassName = L"Lab2";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    if (!RegisterClassW(&wc))
        return 1;
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT rect{0, 0, WIDTH, HEIGHT};
    AdjustWindowRect(&rect, style, FALSE);
    HWND window = CreateWindowW(wc.lpszClassName, L"Лабораторная работа №2",
                                style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
                                rect.bottom - rect.top, nullptr, nullptr, instance, nullptr);
    if (!window)
        return 1;
    ShowWindow(window, show);
    MSG msg{};
    int result;
    while ((result = GetMessageW(&msg, nullptr, 0, 0)) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return result < 0 ? 1 : static_cast<int>(msg.wParam);
}
