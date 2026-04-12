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

    switch (message)
    {
    case WM_CREATE:
        hpen1 = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        hpen2 = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
        break;

    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        SelectObject(hdc, hpen1);
        MoveToEx(hdc, sx / 2, 0, NULL);
        LineTo(hdc, sx / 2, sy);
        SelectObject(hdc, hpen2);
        x = -1.0;
        y = x * x;
        x_scr = int((x + 1.0) * sx / 2.0);
        y_scr = int(sy - y * sy);
        MoveToEx(hdc, x_scr, y_scr, NULL);
        h = 0.001;

        for (x = -1.0; x <= 1.0; x += h)
        {
            y = x * x;
            x_scr = int((x + 1.0) * sx / 2.0);
            y_scr = int(sy - y * sy);
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