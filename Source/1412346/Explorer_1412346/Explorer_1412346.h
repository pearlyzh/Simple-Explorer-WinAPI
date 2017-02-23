#pragma once

#include "resource.h"
#include <windowsX.h>
#include <winuser.h>
#include <commctrl.h>
#include <windowsx.h>
#include <windows.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")
#define MAX_PATH_LENGTH 10240
#define MAX_LOADSTRING 100

void OnDestroy(HWND hwnd);
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
void OnSize(HWND parentHWND, UINT state, int cx, int cy);
void OnPaint(HWND hWnd);

//Get things to do for List
void LoadComputer(HWND _ListView);
TCHAR* ConvertSize(__int64 sizeInBytes);
LPWSTR _GetType(const WIN32_FIND_DATA &fd);
LPWSTR _GetDateModified(const FILETIME &ftLastWrite);

//Align Controls 
void AlignTreeView(HWND _TreeView, int cy);
void AlignListView(HWND _ListView, HWND parenthWnd);
void DoResize(HWND _TreeView, HWND _ListView, HWND parentHWND, RECT g_TreeViewRect);

//TreeView
void Create_Load_TreeView(HWND &_TreeView, HWND parentHWND, long ID, HINSTANCE hParentInst, int tWidth, int tHeight, long lExStyle,
	long lStyle, int x, int y);
void LoadAndExpandTreeView(HWND _TreeView, HTREEITEM hCurSelect);
LPCWSTR GetCurentItemPath(HWND _TreeView);

//ListView
void LoadChild(HWND _ListView, LPCWSTR path);
HWND Create_Load_ListView(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth,
	int nHeight, int x = CW_USEDEFAULT, int y = 0,
	long lExtStyle = WS_EX_CLIENTEDGE,
	long lStyle = LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS);
void ExecuteSelection(HWND _ListView);