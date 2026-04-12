#include <windows.h>
#include <math.h>
extern TCHAR WinName[];

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;
    static HPEN hpen, hpen1;
    int a, b, n, x_scr, y_scr, z_scr;
    double x, y, z;
    double x_min, x_max, y_min, y_max;
    double Kx, Ky;
    double M_PI = 3.14159;
    switch (message)
    {
    case WM_CREATE:
        hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        hpen1 = NULL;
        break;
    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        a = sx / 2;
        b = sy / 2;
        x_min = -100;
        x_max = 100;
        y_min = -100;
        y_max = 100;
        Kx = (sx - 220.0) / (x_max - x_min);
        Ky = (sy - 220.0) / (y_max - y_min);
        SelectObject(hdc, hpen);
        MoveToEx(hdc, 0, b, NULL);
        LineTo(hdc, sx, b);
        MoveToEx(hdc, a, 0, NULL);
        LineTo(hdc, a, sy);
        MoveToEx(hdc, 20, sy - 20, NULL);
        LineTo(hdc, sx - 20, 20);
        n = (int)(y_max - y_min);
        for (y = y_min; y < y_max; y += 5)
        {
            hpen1 = CreatePen(
                PS_SOLID,
                2,
                RGB(
                    255,
                    (int)(255 - 255.0 / n * (y - y_min)),
                    (int)(255 - 255.0 / n * (y - y_min))));
            SelectObject(hdc, hpen1);
            z = b - 100 * sin(M_PI / 50 * sqrt(x_min * x_min + y * y));
            MoveToEx(hdc, (int)(x_min * Kx + a + y), (int)(z + y), NULL);
            for (x = x_min; x < x_max; x += 5)
            {
                z = b - 100 * sin(M_PI / 50 * sqrt(x * x + y * y));
                LineTo(hdc, (int)(x * Kx + a + y), (int)(z + y));
            }
            DeleteObject(hpen1);
            hpen1 = NULL;
        }
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        if (hpen)
            DeleteObject(hpen);
        if (hpen1)
            DeleteObject(hpen1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
