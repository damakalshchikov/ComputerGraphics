//----------------------------------------------------------------------------
#include <windows.h>
extern TCHAR WinName[];
//---------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    int x, y;
    static int sx, sy;

    switch (message)
    {
    case WM_SIZE:
        sx = LOWORD(lParam); // Ширина окна
        sy = HIWORD(lParam); // Высота окна
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // Вертикальные линии сетки
        for (x = 0; x < sx; x += sx / 10)
        {
            MoveToEx(hdc, x, 0, NULL);
            LineTo(hdc, x, sy);
        }
        // Горизонтальные линии сетки
        for (y = 0; y < sy; y += sy / 10)
        {
            MoveToEx(hdc, 0, y, NULL);
            LineTo(hdc, sx, y);
        }
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}