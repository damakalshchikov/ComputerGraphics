#include <windows.h>
#include <tchar.h>
#include <fstream>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static int sx, sy;
const int SCALE = 1000;
const int MARK = 4;

void DcInLp(POINT &point)
{
    point.x = point.x * SCALE / sx;
    point.y = SCALE - point.y * SCALE / sy;
}

void transform(HDC &hdc)
{
    SetMapMode(hdc, MM_ANISOTROPIC);
    SetWindowExtEx(hdc, SCALE, -SCALE, NULL);
    SetViewportExtEx(hdc, sx, sy, NULL);
    SetViewportOrgEx(hdc, 0, sy, NULL);
}

static HPEN hDash, hSpline;
static HBRUSH hRect, hSel;
static POINT pt[20];
static POINT point;
RECT rt;
static int count, index;
static bool capture;
int i;

std::ifstream in;
std::ofstream out;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_CREATE:
    {
        in.open("dat.txt");
        if (in.fail())
        {
            MessageBox(hWnd,
                       _T("Файл dat.txt не найден. Создайте его в папке с программой!"),
                       _T("Открытие файла"), MB_OK | MB_ICONEXCLAMATION);
            PostQuitMessage(0);
            return 1;
        }
        for (count = 0; in >> pt[count].x; count++)
            in >> pt[count].y;
        in.close();

        hDash = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
        hSpline = CreatePen(PS_SOLID, 4, RGB(0, 0, 255));
        hRect = CreateSolidBrush(RGB(128, 0, 128));
        hSel = CreateSolidBrush(RGB(255, 0, 0));
        break;
    }
    case WM_SIZE:
    {
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        point.x = LOWORD(lParam);
        point.y = HIWORD(lParam);
        DcInLp(point);
        for (i = 0; i < count; i++)
        {
            SetRect(&rt,
                    pt[i].x - MARK, pt[i].y - MARK,
                    pt[i].x + MARK, pt[i].y + MARK);
            if (PtInRect(&rt, point))
            {
                index = i;
                capture = true;
                hdc = GetDC(hWnd);
                transform(hdc);
                FillRect(hdc, &rt, hSel);
                ReleaseDC(hWnd, hdc);
                SetCapture(hWnd);
                return 0;
            }
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        if (capture)
        {
            ReleaseCapture();
            capture = false;
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (capture)
        {
            point.x = LOWORD(lParam);
            point.y = HIWORD(lParam);
            DcInLp(point);
            pt[index] = point;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);
        transform(hdc);

        SelectObject(hdc, hDash);
        Polyline(hdc, pt, count);

        SelectObject(hdc, hSpline);
        {
            int first = 1;
            float xA, xB, xC, xD, yA, yB, yC, yD;
            float a3, b3, a2, b2, a1, b1, a0, b0;
            float t, X, Y;
            const int N = 20;

            for (int i = 1; i < count - 2; i++)
            {
                xA = (float)pt[i - 1].x;
                xB = (float)pt[i].x;
                xC = (float)pt[i + 1].x;
                xD = (float)pt[i + 2].x;
                yA = (float)pt[i - 1].y;
                yB = (float)pt[i].y;
                yC = (float)pt[i + 1].y;
                yD = (float)pt[i + 2].y;

                a3 = (-xA + 3 * (xB - xC) + xD) / 6.0;
                b3 = (-yA + 3 * (yB - yC) + yD) / 6.0;
                a2 = (xA - 2 * xB + xC) / 2.0;
                b2 = (yA - 2 * yB + yC) / 2.0;
                a1 = (-xA + xC) / 2.0;
                b1 = (-yA + yC) / 2.0;
                a0 = (xA + 4 * xB + xC) / 6.0;
                b0 = (yA + 4 * yB + yC) / 6.0;

                for (int j = 0; j <= N; j++)
                {
                    t = (float)j / (float)N;
                    X = ((a3 * t + a2) * t + a1) * t + a0;
                    Y = ((b3 * t + b2) * t + b1) * t + b0;

                    if (first)
                    {
                        first = 0;
                        MoveToEx(hdc, (int)X, (int)Y, NULL);
                    }
                    else
                    {
                        LineTo(hdc, (int)X, (int)Y);
                    }
                }
            }
        }

        for (i = 0; i < count; i++)
        {
            SetRect(&rt,
                    pt[i].x - MARK, pt[i].y - MARK,
                    pt[i].x + MARK, pt[i].y + MARK);
            FillRect(hdc, &rt, hRect);
        }

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        DeleteObject(hDash);
        DeleteObject(hSpline);
        DeleteObject(hRect);
        DeleteObject(hSel);

        out.open("dat.txt");
        for (i = 0; i < count; i++)
            out << pt[i].x << '\t' << pt[i].y << '\n';
        out.close();

        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
