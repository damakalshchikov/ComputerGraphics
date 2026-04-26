//----------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>
#include <math.h>
//----------------------------------------------------------------------------
POINT tri[3] = {{0, 100}, {-90, -70}, {90, -70}};

const int CCX = 0;
const int CCY = -50;
const int CR = 50;
const int N = 41;
const double PI = 3.14159265358979;

const int WIDTH = 400;
const int HEIGHT = 300;
//----------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;
    static HBRUSH hBrush;
    static POINT arc[N + 1];

    switch (message)
    {
    case WM_CREATE:
    {
        hBrush = CreateSolidBrush(RGB(255, 0, 0));

        for (int i = 0; i <= N; i++)
        {
            double t = PI * i / N;
            arc[i].x = (LONG)(CCX + CR * cos(t));
            arc[i].y = (LONG)(CCY + CR * sin(t));
        }
        break;
    }

    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        SetMapMode(hdc, MM_ANISOTROPIC);
        SetWindowExtEx(hdc, WIDTH, -HEIGHT, NULL);
        SetViewportExtEx(hdc, sx, sy, NULL);
        SetViewportOrgEx(hdc, sx / 2, sy / 2, NULL);
        SelectObject(hdc, hBrush);

        BeginPath(hdc);

        Polyline(hdc, tri, 3);
        CloseFigure(hdc);

        Polyline(hdc, arc, N + 1);
        CloseFigure(hdc);

        EndPath(hdc);

        SetPolyFillMode(hdc, ALTERNATE);
        FillPath(hdc);

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
