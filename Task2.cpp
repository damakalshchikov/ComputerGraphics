#include <windows.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;
    static HPEN hpen;
    float x[4] = {0.0, 0.0, -0.08, 0.08};
    float y[4] = {-0.25, 0.25, 0.0, 0.0};
    float xMax = 8.5, yMax = 8.5;
    float Kx, Ky;
    float x0 = 4.5, y0 = 4.5;
    float r = 3.0;

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
        hdc = BeginPaint(hWnd, &ps);
        {
            int i, j;
            float pi, phi, cos_phi, sin_phi, dx, dy;
            pi = 4.0 * atan(1.0);
            phi = 12 * pi / 180;
            cos_phi = cos(phi);
            sin_phi = sin(phi);

            SelectObject(hdc, hpen);
            Kx = sx / xMax;
            Ky = sy / yMax;

            for (j = 0; j < 4; j++)
            {
                x[j] += x0 + r;
                y[j] += y0;
            }

            for (i = 0; i < 30; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    dx = x[j] - x0;
                    dy = y[j] - y0;
                    x[j] = x0 + dx * cos_phi - dy * sin_phi;
                    y[j] = y0 + dx * sin_phi + dy * cos_phi;
                }
                MoveToEx(hdc, (int)(x[0] * Kx), (int)(sy - y[0] * Ky), NULL);
                for (j = 1; j <= 3; j++)
                    LineTo(hdc, (int)(x[j] * Kx), (int)(sy - y[j] * Ky));
                LineTo(hdc, (int)(x[1] * Kx), (int)(sy - y[1] * Ky));
            }
        }
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        DeleteObject(hpen);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
