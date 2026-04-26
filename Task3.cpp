#include <windows.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;

    POINT pt[4] = {{-100, 100}, {-100, -100}, {100, -100}, {100, 100}};
    POINT pt1[4];

    int n = 8;
    double alpha = M_PI / n;

    switch (message)
    {
    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        {
            int i, j;
            for (i = 0; i <= n; i++)
            {
                HBRUSH hBrush = CreateSolidBrush(RGB(255,
                                                     (int)(255 - 255.0 / n * i),
                                                     (int)(255 - 255.0 / n * i)));
                SelectObject(hdc, hBrush);

                for (j = 0; j <= 3; j++)
                {
                    pt1[j].x = 500 + (int)(pt[j].x * cos(alpha * i) - pt[j].y * sin(alpha * i)) + i * 10;
                    pt1[j].y = 300 + (int)(pt[j].x * sin(alpha * i) + pt[j].y * cos(alpha * i));
                }

                Polygon(hdc, pt1, 4);
                DeleteObject(hBrush);
            }
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
