// Lab1WinApi__.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "Lab1WinApi__.h"
#include <vector>
#include <time.h>

#define WM_ANIMATE (WM_USER + 0x0001)
#define BOARD_SIZE 4
#define SCORE_BOARD 70
#define MAX_LOADSTRING 100
#define SHIFT 10


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hMainWnd;
HWND hSWnd;
HWND hScore[2];
HWND hBoxes[BOARD_SIZE][BOARD_SIZE];
HWND hSBoxes[BOARD_SIZE][BOARD_SIZE];
HWND hMessage[2] = { NULL,NULL };
int values[BOARD_SIZE*BOARD_SIZE];
RECT MRect;
int score = 0;
int goal = 2048;
bool gameOver = false;
bool animate = false;
bool saved = false;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                MyRegisterBoxClass(HINSTANCE hInstance);
ATOM                MyRegisterScoreClass(HINSTANCE hInstance);
ATOM                MyRegisterMessageClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    MessageWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ScoreWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    BoxWndProc(HWND, UINT, WPARAM, LPARAM);
bool                Collision(RECT, RECT);
void                DrawAlphaBlend(HWND hWnd, HDC hdcwnd,COLORREF col);
void                UpdateBox(HWND hWnd, HWND hWnd2, bool merge);
void                AddNumber();
int                 CanMove(int, int, int, int, bool*);
bool                Move(int);
bool                ShadowMove(int dir);
void                EndGame(bool won);
void                NewGame();
bool                CheckIfWin();
void                SelectMenuItem(HWND hWnd, int flag);
bool                ReadFromFile(const wchar_t* path);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1WINAPI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    MyRegisterBoxClass(hInstance);
    MyRegisterScoreClass(hInstance);
    MyRegisterMessageClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1WINAPI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(CreateSolidBrush(RGB(250, 247, 238)));
    wcex.lpszMenuName   = (LPCWSTR)IDC_LAB1WINAPI;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

ATOM MyRegisterBoxClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = BoxWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(204, 192, 174)));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"BoxClass";
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}

ATOM MyRegisterScoreClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = ScoreWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(204, 192, 174)));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"ScoreClass";
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}

ATOM MyRegisterMessageClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MessageWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0, 0, 0)));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"MessageClass";
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   bool read = ReadFromFile(L"./2048.ini");
   srand(time(0));
   hInst = hInstance; // Store instance handle in our global variable
   int x = GetSystemMetrics(SM_CXSCREEN);
   int y = GetSystemMetrics(SM_CYSCREEN);
   x /= 2;
   y /= 2;
   RECT rc = { 0,0,290,290 + SCORE_BOARD};
   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, true);
   MRect = rc;
   hMainWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME,
      200, 200, rc.right-rc.left, rc.bottom-rc.top + SCORE_BOARD, nullptr, nullptr, hInstance, nullptr);

   if (!hMainWnd)
   {
      return FALSE;
   }

   ShowWindow(hMainWnd, nCmdShow);
   UpdateWindow(hMainWnd);

   hScore[0] = CreateWindowW(L"ScoreClass", L"Score", WS_VISIBLE | WS_CHILD,
       10, 10, 270, SCORE_BOARD -10, hMainWnd, nullptr, hInstance, nullptr);

   for (int i = 0; i < BOARD_SIZE; i++)
   {
       for (int j = 0; j < BOARD_SIZE; j++)
       {
           hBoxes[j][i] = CreateWindowW(L"BoxClass", L"Box", WS_CHILD | WS_VISIBLE,
               10 + 70 * i , 10 + 70 * j + SCORE_BOARD, 60, 60, hMainWnd, (HMENU)(BOARD_SIZE*j+i) , hInstance, nullptr); // Zmiana
       }
   }
   RECT rc2;
   GetWindowRect(hMainWnd, &rc2);
   int x1 = x - rc2.left;
   int y1 = y - rc2.top;
   hSWnd=CreateWindowW(szWindowClass, szTitle, WS_VISIBLE | WS_POPUP | WS_CAPTION,
       x+x1-(rc2.right-rc2.left), y+y1-(rc2.bottom-rc2.top), rc.right - rc.left, rc.bottom - rc.top,hMainWnd, nullptr, hInstance, nullptr);

   if (!hSWnd)
   {
       return FALSE;
   }

   hScore[1] = CreateWindowW(L"ScoreClass", L"Score", WS_VISIBLE | WS_CHILD,
       10, 10, 270, SCORE_BOARD - 10, hSWnd, nullptr, hInstance, nullptr);

   SetWindowLong(hSWnd, GWL_EXSTYLE,
       GetWindowLong(hSWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
   SetLayeredWindowAttributes(hSWnd, 0, 255, LWA_ALPHA);

   for (int i = 0; i < BOARD_SIZE; i++)
   {
       for (int j = 0; j < BOARD_SIZE; j++)
       {
           hSBoxes[j][i] = CreateWindowW(L"BoxClass", L"Box", WS_CHILD | WS_VISIBLE,
               10 + 70 * i, 10 + 70 * j + SCORE_BOARD, 60, 60, hSWnd, (HMENU)(BOARD_SIZE*j+i), hInstance, nullptr);
       }
   }

   if (!read)
   {
       NewGame();
       goal = 2048;
   }

   switch (goal)
   {
   case 8:
       SelectMenuItem(hMainWnd, ID_GOAL_8);
       SelectMenuItem(hSWnd, ID_GOAL_8);
       break;
   case 16:
       SelectMenuItem(hMainWnd, ID_GOAL_16);
       SelectMenuItem(hSWnd, ID_GOAL_16);
       break;
   case 64:
       SelectMenuItem(hMainWnd, ID_GOAL_64);
       SelectMenuItem(hSWnd, ID_GOAL_64);
       break;
   case 2048:
       SelectMenuItem(hMainWnd, ID_GOAL_2048);
       SelectMenuItem(hSWnd, ID_GOAL_2048);
       break;
   }

   if (CheckIfWin())
        EndGame(true);
   else if(!ShadowMove(1) && !ShadowMove(2) && !ShadowMove(3) && !ShadowMove(4))
        EndGame(false);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_GOAL_8:
                goal = 8;
                SelectMenuItem(hMainWnd, ID_GOAL_8);
                SelectMenuItem(hSWnd, ID_GOAL_8);
                break;
            case ID_GOAL_16:
                goal = 16;
                SelectMenuItem(hMainWnd, ID_GOAL_16);
                SelectMenuItem(hSWnd, ID_GOAL_16);
                break;
            case ID_GOAL_64:
                goal = 64;
                SelectMenuItem(hMainWnd, ID_GOAL_64);
                SelectMenuItem(hSWnd, ID_GOAL_64);
                break;
            case ID_GOAL_2048:
                goal = 2048;
                SelectMenuItem(hMainWnd, ID_GOAL_2048);
                SelectMenuItem(hSWnd, ID_GOAL_2048);    
                break;
            case IDM_NEWGAME:
                NewGame();
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_KEYDOWN:
    {
        if (hMessage[0] != NULL || animate) break;
        bool moved = false;
        switch (wParam)
        {
        case 0x41: //A
        {
            moved = Move(1);
        }
        break;
        case 0x53: //S
        {
            moved = Move(2);
        }
        break;
        case 0x44: //D
        {
            moved = Move(3);
        }
        break;
        case 0x57: //W
        {
            moved = Move(4);
        }
        break;
        }
        if (moved && CheckIfWin()) EndGame(true);
    }
    break;
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;
        minMaxInfo->ptMaxSize.x = minMaxInfo->ptMinTrackSize.x = minMaxInfo->ptMaxTrackSize.x = MRect.right - MRect.left;
        minMaxInfo->ptMaxSize.y = minMaxInfo->ptMinTrackSize.y = minMaxInfo->ptMaxTrackSize.y = MRect.bottom - MRect.top;
    }
    break;
    case WM_MOVING:
    {
        int x = GetSystemMetrics(SM_CXSCREEN);
        int y = GetSystemMetrics(SM_CYSCREEN);
        x /= 2;
        y /= 2;
        RECT rc2, rc;

        GetWindowRect(hMainWnd, &rc2);
        GetWindowRect(hSWnd, &rc);
        
        if (hWnd == hMainWnd)
        {
            int x1 = x - rc2.left;
            int y1 = y - rc2.top;
            MoveWindow(hSWnd, x + x1 - (rc2.right - rc2.left), y + y1 - (rc2.bottom - rc2.top), rc.right - rc.left, rc.bottom - rc.top, TRUE);
        }
        else if (hWnd == hSWnd)
        {
            int x1 = x - rc.left;
            int y1 = y - rc.top;
            MoveWindow(hMainWnd, x + x1 - (rc.right - rc.left), y + y1 - (rc.bottom - rc.top), rc2.right - rc2.left, rc2.bottom - rc2.top, TRUE);
        }

        RECT frame1, frame2;

        DwmGetWindowAttribute(hSWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame1, sizeof(RECT));
        DwmGetWindowAttribute(hMainWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame2, sizeof(RECT));
        
        if (Collision(frame1, frame2))
        {
            SetLayeredWindowAttributes(hSWnd, 0, (255 * 50) / 100, LWA_ALPHA);
        }
        else
        {
            SetLayeredWindowAttributes(hSWnd, 0, 255, LWA_ALPHA);
        }
    }
    break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        if (!saved)
        {
            wchar_t buf[256],buf2[256];
            buf[0] = '\0';
            buf2[0] = '\0';
            for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
            {
                _stprintf_s(buf2, 256, L"%d;", values[i]);
                _tcscat_s(buf, 256, buf2);
            }
            WritePrivateProfileString(L"2048", L"Board", buf, L"./2048.ini");
            _stprintf_s(buf2, 256, L"%d;", score);
            WritePrivateProfileString(L"2048", L"Score", buf2, L"./2048.ini");
            _stprintf_s(buf2, 256, L"%d;", goal);
            WritePrivateProfileString(L"2048", L"Goal", buf2, L"./2048.ini");
            saved = true;
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK BoxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int count = 0;
    switch (message)
    {
    case WM_COMMAND:
    break;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT rc;
        GetClientRect(hWnd, &rc);
        int size = rc.right - rc.left;
        HDC hdc = BeginPaint(hWnd, &ps);
        HFONT font = CreateFont(25, 0, 0, 0, FW_BOLD, false, FALSE, 0, EASTEUROPE_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T(" Verdana "));
        COLORREF col;
        int ind = (int)GetMenu(hWnd);
        switch (values[ind])
        {
        case 2:
            col = RGB(238, 228, 198);
            break;
        case 4:
            col = RGB(239, 225, 218);
            break;
        case 8:
            col = RGB(243, 179, 124);
            break;
        case 16:
            col = RGB(246, 153, 100);
            break;
        case 32:
            col = RGB(246, 125, 98);
            break;
        case 64:
            col = RGB(247, 93, 60);
            break;
        case 128:
            col = RGB(237, 206, 116);
            break;
        case 256:
            col = RGB(239, 204, 98);
            break;
        case 512:
            col = RGB(243, 201, 85);
            break;
        case 1024:
            col = RGB(238, 200, 72);
            break;
        case 2048:
            col = RGB(239, 192, 47);
            break;
        default:
            col = RGB(204, 192, 174);
        }

        HBRUSH Brush, OldBrush;
        HPEN Pen, OldPen;
        Brush = CreateSolidBrush(col);
        Pen = CreatePen(PS_NULL, NULL, NULL);
        OldBrush = (HBRUSH)SelectObject(hdc, Brush);
        OldPen = (HPEN)SelectObject(hdc, Pen);
        RoundRect(hdc, 0, 0, size, size, 15, 15);
        SelectObject(hdc, OldBrush);
        SelectObject(hdc, OldPen);
        DeleteObject(Pen);
        DeleteObject(Brush);

        if (values[ind]!=0)
        {
            TCHAR buf[256];
            RECT rc;
            GetClientRect(hWnd, &rc);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));
            HFONT oldFont = (HFONT)SelectObject(hdc, font);
            _stprintf_s(buf, 256, _T("%d"),values[ind]);
            DrawText(hdc, buf, (int)_tcslen(buf), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldFont);
        }
        DeleteObject(font);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_ANIMATE:
    {
        SetTimer(hWnd, 10, 20, NULL);
        count++;
        animate = true;
    }
    break;
    case WM_TIMER:
    {
        if (wParam == 10)
        {
            RECT rc;
            GetWindowRect(hWnd, &rc);
            MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&rc, 2);
            int size = rc.right - rc.left + SHIFT;

            if (size == 80)
            {
                MoveWindow(hWnd, rc.left + SHIFT/2, rc.top+SHIFT/2, size-20, size-20, TRUE);
                KillTimer(hWnd, 10);
                count--;
                if(count==0)
                    animate = false;
            }
            else
            {
                MoveWindow(hWnd, rc.left - SHIFT / 2, rc.top - SHIFT / 2, size, size, TRUE);
            }
                
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK ScoreWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
   
    }
    break;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        TCHAR buf[256];
        RECT rc;
        GetClientRect(hWnd, &rc);

        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH Brush, OldBrush;
        HPEN Pen, OldPen;
        Brush = CreateSolidBrush(RGB(204, 192, 174));
        Pen = CreatePen(PS_NULL, NULL, NULL);
        OldBrush = (HBRUSH)SelectObject(hdc, Brush);
        OldPen = (HPEN)SelectObject(hdc, Pen);
        RoundRect(hdc, 0, 0, 270, SCORE_BOARD-10, 15, 15);
        SelectObject(hdc, OldBrush);
        SelectObject(hdc, OldPen);
        DeleteObject(Pen);
        DeleteObject(Brush);

        HFONT font = CreateFont(
            - MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72), // Height
            0, // Width
            0, // Escapement
            0, // Orientation
            FW_BOLD, // Weight
            false, // Italic
            FALSE, // Underline
            0, // StrikeOut
            EASTEUROPE_CHARSET, // CharSet
            OUT_DEFAULT_PRECIS, // OutPrecision
            CLIP_DEFAULT_PRECIS, // ClipPrecision
            DEFAULT_QUALITY, // Quality
            DEFAULT_PITCH | FF_SWISS, // PitchAndFamily
            _T(" Verdana "));
        HFONT oldFont = (HFONT)SelectObject(hdc, font);
        _stprintf_s(buf, 256, _T("%d"), score);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        DrawText(hdc, buf, (int)_tcslen(buf), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldFont);
        DeleteObject(font);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    break;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HFONT font = CreateFont(
            50, // Height
            0, // Width
            0, // Escapement
            0, // Orientation
            FW_BOLD, // Weight
            false, // Italic
            FALSE, // Underline
            0, // StrikeOut
            EASTEUROPE_CHARSET, // CharSet
            OUT_DEFAULT_PRECIS, // OutPrecision
            CLIP_DEFAULT_PRECIS, // ClipPrecision
            DEFAULT_QUALITY, // Quality
            DEFAULT_PITCH | FF_SWISS, // PitchAndFamily
            _T(" Verdana "));
        HFONT oldFont = (HFONT)SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));

        if (!gameOver)
        {
            DrawAlphaBlend(hWnd, hdc, 0x00ff00);
            DrawText(hdc, L"WIN!", 4, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }    
        else
        {
            DrawAlphaBlend(hWnd, hdc, 0xff0000);
            DrawText(hdc, L"Game Over!", 10, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
            
        SelectObject(hdc, oldFont);
        DeleteObject(font);
        EndPaint(hWnd, &ps);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//Check colision between two rects
bool Collision(RECT rect1, RECT rect2)
{
    if (rect1.left < rect2.left + rect2.right - rect2.left &&
        rect1.left + rect1.right - rect1.left > rect2.left&&
        rect1.top < rect2.top + rect2.bottom - rect2.top &&
        rect1.top + rect1.bottom - rect1.top > rect2.top) {
        return true;
    }
    return false;
}

//Add new tile to the board
void AddNumber()
{
    std::vector<int> v;
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    {
        if (values[i] == 0) v.push_back(i);
    }
    int rnd = rand() % v.size();
    rnd = v[rnd];
    int j = rnd % BOARD_SIZE;
    int i = rnd / BOARD_SIZE;
    values[rnd] = 2;
    UpdateBox(hBoxes[i][j], NULL, true);
    UpdateBox(hSBoxes[i][j], NULL, true);

    //Check if player can move
    if (!ShadowMove(1) && !ShadowMove(2) && !ShadowMove(3) && !ShadowMove(4)) EndGame(false);
}

//Check if tile can move one, two ,three or four tiles forward
int CanMove(int ind, int ind2, int ind3, int ind4, bool* checked)
{
    int m = -1;
    if (ind4 != -1 && !checked[ind4])
    {

        if (values[ind2] == 0 && values[ind3] == 0 && (values[ind4] == 0 || values[ind4] == values[ind])
            && !checked[ind2] && !checked[ind3])
        {
            m = 3;
            if (values[ind4] != 0)
                checked[ind4] = true;
        }
    }
    if (ind3 != -1 && m == -1 && !checked[ind3])
    {
        if (values[ind2] == 0 && (values[ind3] == 0 || values[ind3] == values[ind])
            && !checked[ind2] && !checked[ind3])
        {
            m = 2;
            if (values[ind3] != 0)
                checked[ind3] = true;
        }
    }
    if ((values[ind2] == 0 || values[ind] == values[ind2]) && m == -1 && !checked[ind2])
    {
        m = 1;
        if (values[ind2] != 0)
            checked[ind2] = true;
    }
    return m;
}

//Update tile after value change
void UpdateBox(HWND hWnd,HWND hWnd2, bool merge)
{
    if (merge)
    {
        RECT rc;
        GetWindowRect(hWnd, &rc);
        MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&rc, 2);
        MoveWindow(hWnd, rc.left + 20, rc.top + 20, 20, 20, TRUE);
        SendMessage(hWnd, WM_ANIMATE, NULL, NULL);
        if(hWnd2!=NULL)
            InvalidateRect(hWnd2,NULL,TRUE);
    }
    else
    {
        InvalidateRect(hWnd, NULL, TRUE);
        if(hWnd2!=NULL)
            InvalidateRect(hWnd2, NULL, TRUE);
    }
}

//Makes a player move
    //dir
    // 1 - A
    // 2 - S
    // 3 - D
    // 4 - W
bool Move(int dir)
{
    int m = 0;
    int res = 0;
    bool checked[BOARD_SIZE * BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) checked[i] = false;
    bool moved = false;
    bool scored = false;
    switch (dir)
    {
    case 1: //A
        for (int j = 1; j < BOARD_SIZE; j++)
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                scored = false;
                int ind = (int)GetMenu(hBoxes[i][j]);
                if (values[ind] == 0)continue;
                int ind2 = (int)GetMenu(hBoxes[i][j - 1]);
                int ind3 = -1;
                int ind4 = -1;
                if (j - 2 >= 0) ind3 = (int)GetMenu(hBoxes[i][j - 2]);
                if (j - 3 >= 0) ind4 = (int)GetMenu(hBoxes[i][j - 3]);
                m = CanMove(ind, ind2, ind3, ind4, checked);
                if (m == -1) continue;
                moved = true;
                res = (int)GetMenu(hBoxes[i][j - m]);
                if (values[res] != 0) scored = true;
                values[res] += values[ind];
                if (scored) score += values[res];
                values[ind] = 0;

                UpdateBox(hBoxes[i][j - m], hBoxes[i][j], scored);
                UpdateBox(hSBoxes[i][j - m], hSBoxes[i][j], scored);
            }
        break;
    case 2: //S
        for (int j = 0; j < BOARD_SIZE; j++)
            for (int i = BOARD_SIZE - 2; i >= 0; i--)
            {
                scored = false;
                int ind = (int)GetMenu(hBoxes[i][j]);
                if (values[ind] == 0)continue;
                int ind2 = (int)GetMenu(hBoxes[i + 1][j]);
                int ind3 = -1;
                int ind4 = -1;
                if (i + 2 < BOARD_SIZE) ind3 = (int)GetMenu(hBoxes[i + 2][j]);
                if (i + 3 < BOARD_SIZE) ind4 = (int)GetMenu(hBoxes[i + 3][j]);
                m = CanMove(ind, ind2, ind3, ind4, checked);
                if (m == -1) continue;
                moved = true;
                res = (int)GetMenu(hBoxes[i + m][j]);
                if (values[res] != 0) scored = true;
                values[res] += values[ind];
                if (scored) score += values[res];
                values[ind] = 0;

                UpdateBox(hBoxes[i + m][j], hBoxes[i][j], scored);
                UpdateBox(hSBoxes[i + m][j], hSBoxes[i][j], scored);
            }
        break;
    case 3: //D
        for (int j = BOARD_SIZE - 2; j >= 0; j--)
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                scored = false;
                int ind = (int)GetMenu(hBoxes[i][j]);
                if (values[ind] == 0)continue;
                int ind2 = (int)GetMenu(hBoxes[i][j + 1]);
                int ind3 = -1;
                int ind4 = -1;
                if (j + 2 < BOARD_SIZE) ind3 = (int)GetMenu(hBoxes[i][j + 2]);
                if (j + 3 < BOARD_SIZE) ind4 = (int)GetMenu(hBoxes[i][j + 3]);
                m = CanMove(ind, ind2, ind3, ind4, checked);
                if (m == -1) continue;
                moved = true;
                res = (int)GetMenu(hBoxes[i][j + m]);
                if (values[res] != 0) scored = true;
                values[res] += values[ind];
                if (scored) score += values[res];
                values[ind] = 0;

                UpdateBox(hBoxes[i][j + m], hBoxes[i][j], scored);
                UpdateBox(hSBoxes[i][j + m], hSBoxes[i][j], scored);
            }
        break;
    case 4: //W
        for (int i = 1; i < BOARD_SIZE; i++)
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                scored = false;
                int ind = (int)GetMenu(hBoxes[i][j]);
                if (values[ind] == 0)continue;
                int ind2 = (int)GetMenu(hBoxes[i - 1][j]);
                int ind3 = -1;
                int ind4 = -1;
                if (i - 2 >= 0) ind3 = (int)GetMenu(hBoxes[i - 2][j]);
                if (i - 3 >= 0) ind4 = (int)GetMenu(hBoxes[i - 3][j]);
                m = CanMove(ind, ind2, ind3, ind4, checked);
                if (m == -1) continue;
                moved = true;
                res = (int)GetMenu(hBoxes[i - m][j]);
                if (values[res] != 0) scored = true;
                values[res] += values[ind];
                if (scored) score += values[res];
                values[ind] = 0;

                UpdateBox(hBoxes[i - m][j], hBoxes[i][j], scored);
                UpdateBox(hSBoxes[i - m][j], hSBoxes[i][j], scored);
            }
    }
    if (moved)
    {
        AddNumber();
        InvalidateRect(hScore[0], NULL, TRUE);
        InvalidateRect(hScore[1], NULL, TRUE);
    }
    return moved;
}

//Fuction used to check if player can move
bool ShadowMove(int dir)
{
    int m = 0;
    int res = 0;
    bool checked[BOARD_SIZE * BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) checked[i] = false;
    switch (dir)
    {
    case 1: //A
        for (int j = 1; j < BOARD_SIZE; j++)
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                int ind = (int)GetMenu(hBoxes[i][j]);
                if (values[ind] == 0)continue;
                int ind2 = (int)GetMenu(hBoxes[i][j - 1]);
                int ind3 = -1;
                int ind4 = -1;
                if (j - 2 >= 0) ind3 = (int)GetMenu(hBoxes[i][j - 2]);
                if (j - 3 >= 0) ind4 = (int)GetMenu(hBoxes[i][j - 3]);
                m = CanMove(ind, ind2, ind3, ind4, checked);
                if (m == -1) continue;
                return true;
            }
        break;
    case 2: //S
        for (int j = 0; j < BOARD_SIZE; j++)
            for (int i = BOARD_SIZE - 2; i >= 0; i--)
            {
                int ind = (int)GetMenu(hBoxes[i][j]);
                if (values[ind] == 0)continue;
                int ind2 = (int)GetMenu(hBoxes[i + 1][j]);
                int ind3 = -1;
                int ind4 = -1;
                if (i + 2 < BOARD_SIZE) ind3 = (int)GetMenu(hBoxes[i + 2][j]);
                if (i + 3 < BOARD_SIZE) ind4 = (int)GetMenu(hBoxes[i + 3][j]);
                m = CanMove(ind, ind2, ind3, ind4, checked);
                if (m == -1) continue;
                return true;
            }
        break;
    case 3: //D
        for (int j = BOARD_SIZE - 2; j >= 0; j--)
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                int ind = (int)GetMenu(hBoxes[i][j]);
                if (values[ind] == 0)continue;
                int ind2 = (int)GetMenu(hBoxes[i][j + 1]);
                int ind3 = -1;
                int ind4 = -1;
                if (j + 2 < BOARD_SIZE) ind3 = (int)GetMenu(hBoxes[i][j + 2]);
                if (j + 3 < BOARD_SIZE) ind4 = (int)GetMenu(hBoxes[i][j + 3]);
                m = CanMove(ind, ind2, ind3, ind4, checked);
                if (m == -1) continue;
                return true;
            }
        break;
    case 4: //W
        for (int i = 1; i < BOARD_SIZE; i++)
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                int ind = (int)GetMenu(hBoxes[i][j]);
                if (values[ind] == 0)continue;
                int ind2 = (int)GetMenu(hBoxes[i - 1][j]);
                int ind3 = -1;
                int ind4 = -1;
                if (i - 2 >= 0) ind3 = (int)GetMenu(hBoxes[i - 2][j]);
                if (i - 3 >= 0) ind4 = (int)GetMenu(hBoxes[i - 3][j]);
                m = CanMove(ind, ind2, ind3, ind4, checked);
                if (m == -1) continue;
                return true;
            }
    }
    return false;
}

//https://docs.microsoft.com/pl-pl/windows/win32/gdi/alpha-blending-a-bitmap?redirectedfrom=MSDN
void DrawAlphaBlend(HWND hWnd, HDC hdcwnd,COLORREF col)
{
    HDC hdc;               // handle of the DC we will create  
    BLENDFUNCTION bf;      // structure for alpha blending 
    HBITMAP hbitmap;       // bitmap handle 
    BITMAPINFO bmi;        // bitmap header 
    VOID* pvBits;          // pointer to DIB section 
    ULONG   ulWindowWidth, ulWindowHeight;      // window width/height 
    ULONG   ulBitmapWidth, ulBitmapHeight;      // bitmap width/height 
    RECT    rt;            // used for getting window dimensions 
    UINT32   x, y;          // stepping variables 
    UCHAR ubAlpha;         // used for doing transparent gradient 
    UCHAR ubRed;
    UCHAR ubGreen;
    UCHAR ubBlue;
    float fAlphaFactor;    // used to do premultiply 

    // get window dimensions 
    GetClientRect(hWnd, &rt);

    // calculate window width/height 
    ulWindowWidth = rt.right - rt.left;
    ulWindowHeight = rt.bottom - rt.top;

    // make sure we have at least some window size 
    if ((!ulWindowWidth) || (!ulWindowHeight))
        return;

    // divide the window into 3 horizontal areas 
    ulWindowHeight = ulWindowHeight;

    // create a DC for our bitmap -- the source DC for AlphaBlend  
    hdc = CreateCompatibleDC(hdcwnd);

    // zero the memory for the bitmap info 
    ZeroMemory(&bmi, sizeof(BITMAPINFO));

    // setup bitmap info  
    // set the bitmap width and height to 60% of the width and height of each of the three horizontal areas. Later on, the blending will occur in the center of each of the three areas. 
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = ulBitmapWidth = ulWindowWidth;
    bmi.bmiHeader.biHeight = ulBitmapHeight = ulWindowHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = ulBitmapWidth * ulBitmapHeight;

    // create our DIB section and select the bitmap into the dc 
    hbitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
    if (!hbitmap) return;
    SelectObject(hdc, hbitmap);

    // in top window area, constant alpha = 50%, but no source alpha 
    // the color format for each pixel is 0xaarrggbb  
    // set all pixels to blue and set source alpha to zero 
    for (y = 0; y < ulBitmapHeight; y++)
        for (x = 0; x < ulBitmapWidth; x++)
            ((UINT32*)pvBits)[x + y * ulBitmapWidth] = col;

    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 0x7f;  // half of 0xff = 50% transparency 
    bf.AlphaFormat = 0;             // ignore source alpha channel 
    AlphaBlend(hdcwnd, 0, 0, ulBitmapWidth, ulBitmapHeight, hdc, 0, 0, ulBitmapWidth, ulBitmapHeight, bf);
    return;                     // alpha blend failed 

    // do cleanup 
    DeleteObject(hbitmap);
    DeleteDC(hdc);

}

//Change game state and display message
void EndGame(bool won)
{
    RECT rc;
    GetClientRect(hMainWnd, &rc);
    gameOver = !won;
    if (hMessage[0] != NULL)
    {
        DestroyWindow(hMessage[0]);
        DestroyWindow(hMessage[1]);
    }
    hMessage[0] = CreateWindowEx(WS_EX_TOPMOST, L"MessageClass", L"MessageWnd", WS_CHILD | WS_VISIBLE, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hMainWnd, NULL, hInst, NULL);
    hMessage[1] = CreateWindowEx(WS_EX_TOPMOST, L"MessageClass", L"MessageWnd", WS_CHILD | WS_VISIBLE, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hSWnd, NULL, hInst, NULL);
}

//Restart board
void NewGame()
{
    if (hMessage[0])
    {
        DestroyWindow(hMessage[0]);
        DestroyWindow(hMessage[1]);
        hMessage[0] = hMessage[1] = NULL;
    }
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    {
        if (values[i] != 0)
        {
            values[i] = 0;
            int q = i / BOARD_SIZE;
            int p = i % BOARD_SIZE;
            InvalidateRect(hBoxes[q][p], NULL, TRUE);
            InvalidateRect(hSBoxes[q][p], NULL, TRUE);
        }
    }
    score = 0;
    animate = false;
    gameOver = false;
    InvalidateRect(hScore[0], NULL, TRUE);
    InvalidateRect(hScore[1], NULL, TRUE);
    AddNumber();
    AddNumber();
}

//Check if player won
bool CheckIfWin()
{
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    {
        if (values[i] == goal)
            return true;
    }
    return false;
}

//Read goal value from menu
void SelectMenuItem(HWND hWnd, int flag)
{
    CheckMenuItem(GetMenu(hWnd), flag, MF_CHECKED);

    if (flag != ID_GOAL_8)
        CheckMenuItem(GetMenu(hWnd), ID_GOAL_8, MF_UNCHECKED);
    if (flag != ID_GOAL_16)
        CheckMenuItem(GetMenu(hWnd), ID_GOAL_16, MF_UNCHECKED);
    if (flag != ID_GOAL_64)
        CheckMenuItem(GetMenu(hWnd), ID_GOAL_64, MF_UNCHECKED);
    if (flag != ID_GOAL_2048)
        CheckMenuItem(GetMenu(hWnd), ID_GOAL_2048, MF_UNCHECKED);

    if (CheckIfWin()) EndGame(true);
    else if (hMessage[0] != NULL)
    {
        DestroyWindow(hMessage[0]);
        DestroyWindow(hMessage[1]);
        hMessage[0] = hMessage[1] = NULL;
    }
}

//Load game state from file
bool ReadFromFile(const wchar_t* path)
{
    wchar_t buf[256];
    wchar_t buf2[256];
    int it = -1;
    int ind = 0;
    int size = GetPrivateProfileString(L"2048", L"Board", NULL, buf, 256, path);
    if (size == 0) return false;
    for (int i = 1; i < (int)_tcslen(buf); i++)
    {
        if (buf[i] == ';')
        {
            wcsncpy_s(buf2, buf + it + 1, i - it - 1);
            buf2[i] = '\0';
            it = i;
            values[ind] = _wtoi(buf2);
            ind++;
        }
    }
    GetPrivateProfileString(L"2048", L"Score", NULL, buf, 256, path);
    score = _wtoi(buf);
    GetPrivateProfileString(L"2048", L"Goal", NULL, buf, 256, path);
    goal = _wtoi(buf);
    return true;
}