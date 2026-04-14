//----------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>
//----------------------------------------------------------------------------
POINT pt[5] = {{0, 100}, {-59, -81}, {95, 31}, {-95, 31}, {59, -81}};
const int WIDTH = 400;
const int HEIGHT = 300;
//----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;
    static int k;
    static HBRUSH hBrush;

    switch (message)
    {
    case WM_CREATE:
    {
        int i = MessageBox(hWnd,
                           _T("Будем рисовать красную звезду?"),
                           _T("Политический вопрос"),
                           MB_YESNO | MB_ICONQUESTION);
        k = (i == IDYES) ? 1 : 0;
        hBrush = CreateSolidBrush(RGB(255, 0, 0));
        break;
    }

    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        if (k)
        {
            SetMapMode(hdc, MM_ANISOTROPIC);
            SetWindowExtEx(hdc, WIDTH, -HEIGHT, NULL);
            SetViewportExtEx(hdc, sx, sy, NULL);
            SetViewportOrgEx(hdc, sx / 2, sy / 2, NULL);

            BeginPath(hdc);
            Polyline(hdc, pt, 5);
            CloseFigure(hdc);
            EndPath(hdc);

            SelectObject(hdc, hBrush);
            SetPolyFillMode(hdc, WINDING);
            FillPath(hdc);
        }

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        DeleteObject(hBrush);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
