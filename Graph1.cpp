#include <windows.h>
#include <math.h>
extern TCHAR WinName[];

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;
    static HPEN hpen1, hpen2;
    int cx, cy, x_scr, y_scr;
    double x, y, h;
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
        cx = sx / 2;
        cy = sy / 2;

        SelectObject(hdc, hpen1);
        MoveToEx(hdc, 0, cy, NULL);
        LineTo(hdc, sx, cy);
        MoveToEx(hdc, cx, 0, NULL);
        LineTo(hdc, cx, sy);
        SelectObject(hdc, hpen2);
        x = -3.14;
        y = sin(x);
        x_scr = int(cx + x * sx / 6.28);
        y_scr = int(cy - y * sy / 2);
        MoveToEx(hdc, x_scr, y_scr, NULL);
        h = 0.01;
        for (x = -3.14; x <= 3.14; x += h)
        {
            y = sin(x);
            x_scr = int(cx + x * sx / 6.28);
            y_scr = int(cy - y * sy / 2);
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