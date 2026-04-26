#include <windows.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    static int sx, sy;
    static HPEN hpen;
    float x[4] = {6.0, 6.0, 5.9, 6.1};
    float y[4] = {-0.25, 0.25, 0.0, 0.0};
    float xMax = 6.5, yMax = 6.5;
    float Kx, Ky;

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
            float pi, phi, cos_phi, sin_phi, xx, yy;
            pi = 4.0 * atan(1.0);
            phi = 6 * pi / 180;
            cos_phi = cos(phi);
            sin_phi = sin(phi);

            SelectObject(hdc, hpen);
            Kx = sx / xMax;
            Ky = sy / yMax;

            for (i = 1; i <= 14; i++)
            {
                for (j = 0; j <= 3; j++)
                {
                    xx = x[j];
                    yy = y[j];
                    x[j] = xx * cos_phi - yy * sin_phi;
                    y[j] = xx * sin_phi + yy * cos_phi;
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
