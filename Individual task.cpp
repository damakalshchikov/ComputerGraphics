//----------------------------------------------------------------------------
#include <windows.h>
extern TCHAR WinName[];
//----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;
    HPEN hpen;
    HBRUSH hbrush;

    switch (message)
    {
    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        int cx = sx / 2;
        int cy = sy / 2;
        int w = sx / 3;       // ширина корпуса
        int h = (sy * 2) / 3; // высота корпуса

        int left = cx - w / 2;
        int right = cx + w / 2;
        int top = cy - h / 2;
        int bottom = cy + h / 2;

        // Корпус
        hpen = CreatePen(PS_SOLID, 2, RGB(30, 30, 30));
        hbrush = CreateSolidBrush(RGB(65, 65, 65));
        SelectObject(hdc, hpen);
        SelectObject(hdc, hbrush);
        Rectangle(hdc, left, top, right, bottom);
        DeleteObject(hpen);
        DeleteObject(hbrush);

        // Экран
        int sLeft = left + w / 10;
        int sRight = right - w / 10;
        int sTop = top + h / 7;
        int sBottom = bottom - h / 7;

        hpen = CreatePen(PS_SOLID, 1, RGB(10, 10, 10));
        hbrush = CreateSolidBrush(RGB(15, 90, 170));
        SelectObject(hdc, hpen);
        SelectObject(hdc, hbrush);
        Rectangle(hdc, sLeft, sTop, sRight, sBottom);
        DeleteObject(hpen);
        DeleteObject(hbrush);

        // Динамик
        hpen = CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
        hbrush = CreateSolidBrush(RGB(30, 30, 30));
        SelectObject(hdc, hpen);
        SelectObject(hdc, hbrush);
        Rectangle(hdc, cx - w / 8, top + h / 25, cx + w / 8, top + h / 12);
        DeleteObject(hpen);
        DeleteObject(hbrush);

        // Фронтальная камера
        hpen = CreatePen(PS_SOLID, 1, RGB(10, 10, 10));
        hbrush = CreateSolidBrush(RGB(15, 15, 15));
        SelectObject(hdc, hpen);
        SelectObject(hdc, hbrush);
        int camR = w / 30;
        int camY = top + h / 20;
        Ellipse(hdc, cx + w / 5 - camR, camY - camR, cx + w / 5 + camR, camY + camR);
        DeleteObject(hpen);
        DeleteObject(hbrush);

        // Кнопка "Домой"
        hpen = CreatePen(PS_SOLID, 1, RGB(40, 40, 40));
        hbrush = CreateSolidBrush(RGB(100, 100, 100));
        SelectObject(hdc, hpen);
        SelectObject(hdc, hbrush);
        int btnR = w / 16;
        int btnY = bottom - h / 14;
        Ellipse(hdc, cx - btnR, btnY - btnR, cx + btnR, btnY + btnR);
        DeleteObject(hpen);
        DeleteObject(hbrush);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}