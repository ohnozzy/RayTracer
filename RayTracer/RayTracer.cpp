// RayTracer.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "readfile.h"
#define MAINPROGRAM
#include "variables.h"
#include "RayTracer.h"
#include <FreeImage.h>




#define PaintEnd 0xF000
#define MAX_LOADSTRING 100
#define THREAD_NUM 4
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
FIBITMAP* bitmap;
PAINTSTRUCT ps;
HDC hdc;
HWND hWnd;
UINT render(LPVOID param);
unsigned threadcount;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
	int argc;
	LPWSTR *szArglist=CommandLineToArgvW(lpCmdLine, &argc);
	
	readfile(szArglist[0]);
	tan_half_fov = tan(glm::radians(fovy / 2));

	mat4 modelview=Transform::lookAt(eye, center, up);
	for (unsigned i = 0; i < numused; i++) {
		matransform(modelview, &lightposn[4 * i], &lightransf[4 * i]);
	}
	unsigned sphere_num = spheres.size();
	for (unsigned i = 0; i < sphere_num; i++) {
		spheres[i].forward =modelview*spheres[i].transform;
		spheres[i].inverse =glm::inverse(spheres[i].forward);

		
	}
	unsigned tri_num = triangles.size();
	for (unsigned i = 0; i < tri_num; i++) {
		triangles[i].point[0] = modelview*triangles[i].point[0];
		triangles[i].point[1] = modelview*triangles[i].point[1];
		triangles[i].point[2] = modelview*triangles[i].point[2];
	}
	
	bitmap = FreeImage_Allocate(w, h, 24);
	Render_Range range[THREAD_NUM];
	range[0].min_i = 0;
	range[0].max_i = floor(w / 2);
	range[0].min_j = 0;
	range[0].max_j = floor(h / 2);

	range[1].min_i = floor(w / 2);
	range[1].max_i = w;
	range[1].min_j = 0;
	range[1].max_j = floor(h / 2);
	
	range[2].min_i = 0;
	range[2].max_i = floor(w / 2);
	range[2].min_j = floor(h / 2);
	range[2].max_j = h;

	range[3].min_i = floor(w / 2);
	range[3].max_i = w;
	range[3].min_j = floor(h / 2);
	range[3].max_j = h;
	
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RAYTRACER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RAYTRACER));

    MSG msg;

    // Main message loop:
	
	hdc = BeginPaint(hWnd, &ps);
	for (unsigned i = 0; i < THREAD_NUM; i++) {
		AfxBeginThread(render, &range[i]);
	}
	

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RAYTRACER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RAYTRACER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, w+20, h+80, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

UINT render(LPVOID param) {
	Render_Range* range = (Render_Range*)param;
	RGBQUAD rgbcolor;
	
	for (LONG j = range->min_j; j < range->max_j; j++) {
		for (LONG i = range->min_i; i < range->max_i; i++) {

			float alpha = tan_half_fov*((i + 0.5f - w / 2) / (h / 2));
			float beta = tan_half_fov*((h / 2 - j - 0.5f) / (h / 2));
			vec3 ray = vec3(alpha, beta, -1);
			ray = glm::normalize(ray);
			Ray camray;
			camray.direction = ray;
			camray.origin = vec3(0, 0, 0);
			//Intersection* hit = Intersect(camray);
			vec3 color = raytrace(camray, maxdepth, 0);
			for (unsigned index = 0; index < 3; index++) {
				if (color[index]>1) {
					color[index] = 1;
				}
			}
			
			
			rgbcolor.rgbRed = 255.0 * color[0];
			rgbcolor.rgbGreen = 255.0 * color[1];
			rgbcolor.rgbBlue = 255.0 * color[2];
			//SetPixel(hdc, i, j, RGB(rgbcolor.rgbRed, rgbcolor.rgbGreen, rgbcolor.rgbBlue));
			
			FreeImage_SetPixelColor(bitmap, i, h - j - 1, &rgbcolor);
			SendNotifyMessage(hWnd, WM_PAINT, j*w + i, 1);
			
		}
	}
	SendMessage(hWnd, WM_PAINT, 0, 2);
	return 0;
}
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
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            
            // TODO: Add any drawing code that uses hdc here...
			RGBQUAD rgbcolor;
			unsigned i = wParam%w;
			unsigned j = wParam/w;
			if (lParam == 1) {
				FreeImage_GetPixelColor(bitmap, i, h - j - 1, &rgbcolor);
				SetPixel(hdc, i, j, RGB(rgbcolor.rgbRed, rgbcolor.rgbGreen, rgbcolor.rgbBlue));
			}
			else if(lParam == 2){
				threadcount++;
				if (threadcount == THREAD_NUM) {
					EndPaint(hWnd, &ps);
					FreeImage_Initialise();
					FreeImage_Save(FIF_PNG, bitmap, outputfilename.c_str(), 0);
					FreeImage_DeInitialise();
				}
			}
			/*HANDLE h[2];
			h[0] = threadid1->m_hThread;
			h[1] = threadid2->m_hThread;
			WaitForMultipleObjects(2, h, true, INFINITE);*/
			/*RGBQUAD rgbcolor;
			for (LONG j = 0; j < h; j++) {
				for (LONG i = 0; i < w; i++) {
					
					float alpha = tan_half_fov*((i+0.5f - w / 2) / (h / 2));
					float beta = tan_half_fov*(( h / 2 -j-0.5f) / (h / 2));
					vec3 ray = vec3(alpha, beta, -1);
					ray = glm::normalize(ray);
					Ray camray;
					camray.direction = ray;
					camray.origin = vec3(0,0,0);
					//Intersection* hit = Intersect(camray);
					vec3 color = raytrace(camray, maxdepth, 0);
					for (unsigned i = 0; i < 3; i++) {
						if (color[i]>1) {
							color[i] = 1;
						}
					}
					
					rgbcolor.rgbRed = 255.0 * color[0];
					rgbcolor.rgbGreen = 255.0 * color[1];
					rgbcolor.rgbBlue = 255.0 * color[2];
					SetPixel(hdc, i, j, RGB(rgbcolor.rgbRed, rgbcolor.rgbGreen, rgbcolor.rgbBlue));
					FreeImage_SetPixelColor(bitmap, i, h-j-1, &rgbcolor);
					
					
				}
			}*/
			
			
           
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
