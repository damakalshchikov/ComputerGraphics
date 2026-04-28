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

static HPEN hDash, hBezier;
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
        hBezier = CreatePen(PS_SOLID, 4, RGB(0, 0, 255));
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

        SelectObject(hdc, hBezier);
        PolyBezier(hdc, pt, count);

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
        DeleteObject(hBezier);
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
