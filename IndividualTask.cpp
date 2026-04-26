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
    static HPEN hpen;

    const POINT tri_obj[3] = {{0, 100}, {-90, -70}, {90, -70}};

    const int CCX = 0, CCY = -50, CR = 50;
    const int N = 41;
    const double PI = M_PI;

    const int n = 8;
    const int shift_x = 10;
    const double alpha = M_PI / n;

    switch (message)
    {
    case WM_CREATE:
        hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        break;

    case WM_SIZE:
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        int cx = sx / 2;
        int cy = sy / 2;

        for (int i = 0; i <= n; i++)
        {
            HBRUSH hBrush = CreateSolidBrush(RGB(255,
                                                 (int)(255 - 255.0 / n * i),
                                                 (int)(255 - 255.0 / n * i)));
            SelectObject(hdc, hBrush);

            double angle = alpha * i;
            double cos_a = cos(angle);
            double sin_a = sin(angle);
            int dx = i * shift_x;

            POINT tri[3];
            for (int j = 0; j < 3; j++)
            {
                double x = tri_obj[j].x;
                double y = tri_obj[j].y;
                tri[j].x = cx + (int)(x * cos_a - y * sin_a) + dx;
                tri[j].y = cy - (int)(x * sin_a + y * cos_a);
            }

            POINT arc[N + 1];
            for (int k = 0; k <= N; k++)
            {
                double t = PI * k / N;
                double x = CCX + CR * cos(t);
                double y = CCY + CR * sin(t);
                arc[k].x = cx + (int)(x * cos_a - y * sin_a) + dx;
                arc[k].y = cy - (int)(x * sin_a + y * cos_a);
            }

            BeginPath(hdc);

            MoveToEx(hdc, tri[0].x, tri[0].y, NULL);
            LineTo(hdc, tri[1].x, tri[1].y);
            LineTo(hdc, tri[2].x, tri[2].y);
            CloseFigure(hdc);

            MoveToEx(hdc, arc[0].x, arc[0].y, NULL);
            for (int k = 1; k <= N; k++)
                LineTo(hdc, arc[k].x, arc[k].y);
            CloseFigure(hdc);

            EndPath(hdc);
            SetPolyFillMode(hdc, ALTERNATE);
            FillPath(hdc);

            SelectObject(hdc, hpen);
            MoveToEx(hdc, tri[0].x, tri[0].y, NULL);
            LineTo(hdc, tri[1].x, tri[1].y);
            LineTo(hdc, tri[2].x, tri[2].y);
            LineTo(hdc, tri[0].x, tri[0].y);

            MoveToEx(hdc, arc[0].x, arc[0].y, NULL);
            for (int k = 1; k <= N; k++)
                LineTo(hdc, arc[k].x, arc[k].y);
            LineTo(hdc, arc[0].x, arc[0].y);

            DeleteObject(hBrush);
        }

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        DeleteObject(hpen);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
