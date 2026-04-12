#include <windows.h>
#include <math.h>
extern TCHAR WinName[];

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;
    static HPEN hpen1, hpen2;
    int x_scr, y_scr;
    double x, y, h;
    double x_min, x_max, y_min, y_max;

    switch (message)
    {
    case WM_CREATE:
        hpen1 = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        hpen2 = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        break;

    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        x_min = -3.0;
        x_max = 5.0;
        y_min = exp(x_min);
        y_max = exp(x_max);

        x_scr = int((0 - x_min) / (x_max - x_min) * sx);

        SelectObject(hdc, hpen1);
        MoveToEx(hdc, x_scr, 0, NULL);
        LineTo(hdc, x_scr, sy);

        SelectObject(hdc, hpen2);
        x = x_min;
        y = exp(x);
        x_scr = int((x - x_min) / (x_max - x_min) * sx);
        y_scr = int(sy - (y - y_min) / (y_max - y_min) * sy);
        MoveToEx(hdc, x_scr, y_scr, NULL);
        h = 0.01;
        for (x = x_min; x <= x_max; x += h)
        {
            y = exp(x);
            x_scr = int((x - x_min) / (x_max - x_min) * sx);
            y_scr = int(sy - (y - y_min) / (y_max - y_min) * sy);
            LineTo(hdc, x_scr, y_scr);
        }
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        DeleteObject(hpen1);
        DeleteObject(hpen2);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
