#include <windows.h>
#include <math.h>
extern TCHAR WinName[];

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;
    static HPEN hpen_axis, hpen_sin;
    int x_scr, y_scr;
    double x, h;
    // Физические границы области вывода
    double x_min = -3.14, x_max = 3.14;
    double y_min = -1.0, y_max = 1.0;
    // Коэффициенты масштабирования
    double Kx, Ky;
    // Начало координат в экранных координатах
    int x0, y0;

    switch (message)
    {
    case WM_CREATE:
        hpen_axis = CreatePen(PS_SOLID, 1, RGB(0, 0, 255)); // синие оси
        hpen_sin = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));  // красный синус
        break;

    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        Kx = sx / (x_max - x_min);
        Ky = sy / (y_max - y_min);

        x0 = (int)((-x_min) * Kx);
        y0 = (int)(y_max * Ky);

        // Ось X
        SelectObject(hdc, hpen_axis);
        MoveToEx(hdc, 0, y0, NULL);
        LineTo(hdc, sx, y0);

        // Ось Y
        MoveToEx(hdc, x0, 0, NULL);
        LineTo(hdc, x0, sy);

        // График синуса
        SelectObject(hdc, hpen_sin);
        x_scr = x0 + (int)(x_min * Kx);
        y_scr = y0 - (int)(sin(x_min) * Ky);
        MoveToEx(hdc, x_scr, y_scr, NULL);

        for (x = x_min; x <= x_max; x += 0.01)
        {
            x_scr = x0 + (int)(x * Kx);
            y_scr = y0 - (int)(sin(x) * Ky);
            LineTo(hdc, x_scr, y_scr);
        }

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        DeleteObject(hpen_axis);
        DeleteObject(hpen_sin);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
