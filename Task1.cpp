#include <windows.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

float v11, v12, v13, v21, v22, v23, v31, v32, v33, v43;
float rho = 300.f, thetta = 75.f, phi = 30.f, ScreenDist = 500.f;
float A, B, C, D, An, Bn, Cn;
float xt[3], yt[3], zt[3];
float Al, Bl, Cl, alpha;
float th, ph, costh, cosph, sinth, sinph;
float factor = atan(1.0f) / 45.f;

PAINTSTRUCT ps;
static HBRUSH hBrush;

class TFPoint
{
public:
    float X, Y, Z;
};

TFPoint CubePoints[] = {
    {-50, -50, -50},
    {50, -50, -50},
    {50, 50, -50},
    {-50, 50, -50},
    {-50, 50, 50},
    {-50, -50, 50},
    {50, -50, 50},
    {50, 50, 50}};

int Gran[6][4] = {
    {0, 3, 4, 5},
    {0, 5, 6, 1},
    {2, 7, 4, 3},
    {7, 6, 5, 4},
    {0, 1, 2, 3},
    {2, 1, 6, 7}};

void VidMatCoeff(float rho, float thetta, float phi)
{
    th = thetta * factor;
    ph = phi * factor;
    costh = cos(th);
    sinth = sin(th);
    cosph = cos(ph);
    sinph = sin(ph);

    v11 = -sinth;
    v12 = -cosph * costh;
    v13 = -sinph * costh;
    v21 = costh;
    v22 = -cosph * sinth;
    v23 = -sinph * sinth;
    v31 = 0.f;
    v32 = sinph;
    v33 = -cosph;
    v43 = rho;
}

POINT Perspective(float x, float y, float z)
{
    POINT point;
    VidMatCoeff(rho, thetta, phi);

    float xe = v11 * x + v21 * y;
    float ye = v12 * x + v22 * y + v32 * z;
    float ze = v13 * x + v23 * y + v33 * z + v43;

    point.x = (LONG)(ScreenDist * xe / ze + 400.f);
    point.y = (LONG)(ScreenDist * ye / ze + 300.f);
    return point;
}

POINT point1[4];
HDC hdc;
int sx, sy, zDelta;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEMOVE:
    {
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        thetta += ((sx % 180) - 90) / 10;
        phi += ((sy % 180) - 90) / 10;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        zDelta = (short)HIWORD(wParam);
        ScreenDist -= zDelta / 10;
        if (ScreenDist < 100)
            ScreenDist = 100;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        th = thetta * factor;
        ph = phi * factor;
        costh = cos(th);
        sinth = sin(th);
        cosph = cos(ph);
        sinph = sin(ph);

        A = rho * sinph * costh;
        B = rho * sinph * sinth;
        C = rho * cosph;
        float len = sqrt(A * A + B * B + C * C);
        Al = A / len;
        Bl = B / len;
        Cl = C / len;

        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                xt[j] = CubePoints[Gran[i][j]].X;
                yt[j] = CubePoints[Gran[i][j]].Y;
                zt[j] = CubePoints[Gran[i][j]].Z;
            }

            A = yt[0] * (zt[1] - zt[2]) - yt[1] * (zt[0] - zt[2]) + yt[2] * (zt[0] - zt[1]);
            B = -(xt[0] * (zt[1] - zt[2]) - xt[1] * (zt[0] - zt[2]) + xt[2] * (zt[0] - zt[1]));
            C = xt[0] * (yt[1] - yt[2]) - xt[1] * (yt[0] - yt[2]) + xt[2] * (yt[0] - yt[1]);

            float nlen = sqrt(A * A + B * B + C * C);
            An = A / nlen;
            Bn = B / nlen;
            Cn = C / nlen;
            alpha = An * Al + Bn * Bl + Cn * Cl;

            for (int j = 0; j < 4; j++)
                point1[j] = Perspective(
                    CubePoints[Gran[i][j]].X,
                    CubePoints[Gran[i][j]].Y,
                    CubePoints[Gran[i][j]].Z);

            D = (float)(point1[0].x * (point1[1].y - point1[2].y) - point1[1].x * (point1[0].y - point1[2].y) + point1[2].x * (point1[0].y - point1[1].y));

            if (D < 0)
            {
                hBrush = CreateSolidBrush(RGB(
                    (BYTE)((1.f - alpha) * 255),
                    (BYTE)((1.f - alpha) * 255),
                    (BYTE)((1.f - alpha) * 255)));
                SelectObject(hdc, hBrush);
                Polygon(hdc, point1, 4);
            }
        }

        EndPaint(hWnd, &ps);
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
