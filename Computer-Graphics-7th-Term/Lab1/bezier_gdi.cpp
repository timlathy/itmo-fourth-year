#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define MAX_LOADSTRING 100

#define PAINT_AREA_WIDTH 640
#define PAINT_AREA_HEIGHT 480

#define NUM_CURVES (sizeof(ptCurves)/sizeof(POINT)/4)
POINT ptCurves[8][4] =
{
	{{140,133},{107,132},{98,265},{55,257}},
	{{55,257},{24,252},{39,211},{57,208}},
	{{57,208},{74,206},{62,227},{62,227}},
	{{62,227},{66,214},{58,206},{41,222}},

	{{71,253},{120,225},{107,142},{140,133}},

	{{134,138},{134,138},{89,264},{150,252}},

	{{134,138},{84,246},{130,254},{142,254}},
	{{142,254},{150,254},{157,248},{161,243}},
};
#define NUM_FILL_PTS (sizeof(ptFill)/sizeof(POINT))
POINT ptFill[3] =
{
	{65,216},
	{98,211},
	{114,211}
};
COLORREF cForeground = RGB(248, 145, 7);

void DoPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HPEN hPen = CreatePen(PS_SOLID, 1, cForeground);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	HBRUSH hBrush = CreateSolidBrush(cForeground);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	for (int i = 0; i < NUM_CURVES; ++i)
		PolyBezier(hdc, ptCurves[i], 4);

	for (int i = 0; i < NUM_FILL_PTS; ++i)
		ExtFloodFill(hdc, ptFill[i].x, ptFill[i].y, cForeground, FLOODFILLBORDER);

	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		DoPaint(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WCHAR szTitle[MAX_LOADSTRING];
	WCHAR szWindowClass[MAX_LOADSTRING]; 

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CGLAB, szWindowClass, MAX_LOADSTRING);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CGLAB));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME /* non resizable */,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
