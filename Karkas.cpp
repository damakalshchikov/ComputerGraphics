//----------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR WinName[] = _T("MainFrame");
//----------------------------------------------------------------------------

int APIENTRY WinMain(HINSTANCE This, HINSTANCE Prev, LPSTR cmd, int mode)
{
    HWND hWnd;   // Дескриптор главного окна программы
    MSG msg;     // Структура для хранения сообщений
    WNDCLASS wc; // Класс окна

    // Определение класса окна
    wc.hInstance = This;
    wc.lpszClassName = WinName;                    // Имя класса окна
    wc.lpfnWndProc = WndProc;                      // Функция окна
    wc.style = CS_HREDRAW | CS_VREDRAW;            // Стиль окна
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);    // Стандартная иконка
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // Стандартный курсор
    wc.lpszMenuName = NULL;                        // Нет меню
    wc.cbClsExtra = 0;                             // Нет дополнительных данных класса
    wc.cbWndExtra = 0;                             // Нет дополнительных данных окна

    // Заполнение окна белым цветом
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Установка цвета фона
    if (!RegisterClass(&wc)) return 0; // Регистрация класса окна

    // Создание окна
    hWnd = CreateWindow(WinName,                         // Имя класса окна
                        _T("Каркас Windows-приложения"), // Заголовок окна
                        WS_OVERLAPPEDWINDOW,             // Стиль окна
                        CW_USEDEFAULT,                   // X
                        CW_USEDEFAULT,                   // Y
                        CW_USEDEFAULT,                   // Width
                        CW_USEDEFAULT,                   // Height
                        HWND_DESKTOP,                    // Дескриптор родительского окна
                        NULL,                            // Нет меню
                        This,                            // Дескриптор приложения
                        NULL);                           // Дополнительной информации нет
    ShowWindow(hWnd, mode);                              // Показать окно

    // Цикл обработки сообщений
    while (GetMessage(&msg, NULL, 0, 0)) // Цикл получения сообщений
    {
        TranslateMessage(&msg);          // Функция трансляции кодов нажатой клавиши // Цикл получения сообщений
        DispatchMessage(&msg);           // Посылает сообщение функции WndProc()
    }
    return 0; // При положительном завершении программы в главную функцию возвращается «0»
}
//---------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    int x, y;
    static int sx, sy;

    switch (message)
    {
    case WM_SIZE:
        sx = LOWORD(lParam); // Ширина окна
        sy = HIWORD(lParam); // Высота окна
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // Вертикальные линии сетки
        for (x = 0; x < sx; x += sx / 10)
        {
            MoveToEx(hdc, x, 0, NULL);
            LineTo(hdc, x, sy);
        }
        // Горизонтальные линии сетки
        for (y = 0; y < sy; y += sy / 10)
        {
            MoveToEx(hdc, 0, y, NULL);
            LineTo(hdc, sx, y);
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