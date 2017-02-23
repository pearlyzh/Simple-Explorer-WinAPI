#include "stdafx.h"
#include <string.h>
#include <iostream>
#include "resource.h"
#include "Explorer_1412346.h"
#include <windef.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


HIMAGELIST hImageList;

int iFolderID;
int iHardID;
int iComputerID;
int iDesktopID;

void CreateImageList(HWND _TreeView, HINSTANCE hIns)
{
	InitCommonControls();

	hImageList = ImageList_Create(16, 16, ILC_COLOR32, 2, 0);
	HICON hIcon; 
	// load the picture from the resource
	hIcon = LoadIcon(hIns, MAKEINTRESOURCE(IDI_DESKTOP));
	iDesktopID = ImageList_AddIcon(hImageList, hIcon, NULL);

	hIcon = LoadIcon(hIns, MAKEINTRESOURCE(IDI_COMPUTER));
	iComputerID = ImageList_AddIcon(hImageList, hIcon, NULL);

	hIcon = LoadIcon(hIns, MAKEINTRESOURCE(IDI_DISK));
	iHardID = ImageList_AddIcon(hImageList, hIcon, NULL);

	hIcon = LoadIcon(hIns, MAKEINTRESOURCE(IDI_FOLDER));
	iFolderID = ImageList_AddIcon(hImageList, hIcon, NULL);
	TreeView_SetImageList(_TreeView, hImageList, TVSIL_NORMAL);
}

void Create_Load_TreeView(HWND &_TreeView, HWND parentHWND, long ID, HINSTANCE hParentInst, int tWidth, int tHeight, long lExStyle,
	long lStyle, int x, int y)
{
	_TreeView = CreateWindowEx(lExStyle, WC_TREEVIEW, _T("Tree View"),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_VSCROLL | WS_TABSTOP | lStyle,
		x, y, tWidth, tHeight, parentHWND,
		(HMENU)ID, hParentInst, NULL);

	CreateImageList(_TreeView, hParentInst);

	TV_INSERTSTRUCT tvInsert;
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = TVI_ROOT;
	tvInsert.item.pszText = _T("Desktop");
	tvInsert.item.lParam = (LPARAM)_T("Desktop");
	tvInsert.item.iImage = iDesktopID;
	tvInsert.item.iSelectedImage = iDesktopID;
	HTREEITEM hDesktop = TreeView_InsertItem(_TreeView, &tvInsert);

	tvInsert.hParent = hDesktop;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.pszText = _T("Computer");
	tvInsert.item.lParam = (LPARAM)_T("Computer");
	tvInsert.item.iImage = iComputerID;
	tvInsert.item.iSelectedImage = iComputerID;
	HTREEITEM hMyComputer = TreeView_InsertItem(_TreeView, &tvInsert);

	TCHAR cDriveLetter = 'A';
	DWORD dwDrivemap = GetLogicalDrives();

	while (cDriveLetter <= 'Z')
	{
		TCHAR *buffName = new TCHAR[10];
		TCHAR *buffDisplay = new TCHAR[10];
		TCHAR *buffTemp = new TCHAR[10];
		if (0 < (dwDrivemap & 0x00000001L))
		{
			wsprintf(buffName, _T("%c:\\\0"), cDriveLetter);

			TV_INSERTSTRUCT tvInsert;
			tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

			GetVolumeInformation(buffName, buffDisplay, 10, NULL, NULL, NULL, NULL, 0);
			wcscpy(buffTemp, buffDisplay);
			wsprintf(buffDisplay, _T("%s(%c:)\0"), buffTemp, cDriveLetter);

			tvInsert.hParent = hMyComputer;
			tvInsert.item.pszText = buffDisplay;
			tvInsert.item.lParam = (LPARAM)buffName;
			tvInsert.item.cChildren = 1;
			tvInsert.item.iImage = iHardID;
			tvInsert.item.iSelectedImage = iHardID;
			TreeView_InsertItem(_TreeView, &tvInsert);
		}
		cDriveLetter++;
		dwDrivemap = dwDrivemap >> 1;
	}

	TreeView_Expand(_TreeView, hMyComputer, TVE_EXPAND);
	TreeView_SelectItem(_TreeView, hMyComputer);
	SetFocus(_TreeView);
}

LPCWSTR GetPathItem(HWND _TreeView, HTREEITEM hItem)
{
	TVITEM tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(_TreeView, &tv);
	return (LPCWSTR)tv.lParam;
}

void LoadChildrenItem(HWND &_TreeView, HTREEITEM &hParent, LPCWSTR pathParent)
{
	HTREEITEM hCurSelChild = TreeView_GetChild(_TreeView, hParent);

	while (hCurSelChild != NULL)
	{
		SendMessage(_TreeView, TVM_DELETEITEM, 0, (LPARAM)hCurSelChild);
		hCurSelChild = TreeView_GetChild(_TreeView, hParent);
	}

	TCHAR buffer[MAX_PATH_LENGTH];
	wsprintf(buffer, L"%s\\*", pathParent);

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	TCHAR *folderPath;

	do
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			tvInsert.item.pszText = fd.cFileName;

			folderPath = new TCHAR[_tcslen(fd.cFileName) + _tcslen((WCHAR*)(pathParent)) + 4];

			swprintf(folderPath, _T("%s\\%s"), pathParent, fd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;
			tvInsert.item.iImage = iFolderID;
			tvInsert.item.iSelectedImage = iFolderID;

			swprintf(buffer, _T("%s\\*"), (LPARAM)folderPath);

			WIN32_FIND_DATA fd1;
			HANDLE hFileq = FindFirstFileW(buffer, &fd1);

			if (hFileq == INVALID_HANDLE_VALUE)
			{
				tvInsert.item.cChildren = 0;
				TreeView_InsertItem(_TreeView, &tvInsert);
				return;
			}

			bool found = false;

			do
			{
				if ((fd1.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					&& ((fd1.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
					&& ((fd1.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
					&& (StrCmp(fd1.cFileName, _T(".")) != 0) && (StrCmp(fd1.cFileName, _T("..")) != 0))
				{
					tvInsert.item.cChildren = 1;
					TreeView_InsertItem(_TreeView, &tvInsert);
					found = true;
					break;
				}
			} while (FindNextFileW(hFileq, &fd1));

			if (!found)
			{
				tvInsert.item.cChildren = 0;
				TreeView_InsertItem(_TreeView, &tvInsert);
			}
		}

	} while (FindNextFileW(hFile, &fd));

}

LPCWSTR GetCurentItemPath(HWND _TreeView)
{
	TVITEM tv;
	tv.mask = TVIF_PARAM | TVIF_HANDLE;
	tv.hItem = TreeView_GetNextItem(_TreeView, NULL, TVGN_NEXTSELECTED);
	TreeView_GetItem(_TreeView, &tv);

	return GetPathItem(_TreeView, tv.hItem);
}


void LoadAndExpandTreeView(HWND _TreeView, HTREEITEM hCurSelect)
{
	if (hCurSelect == TreeView_GetRoot(_TreeView) || StrCmp(GetPathItem(_TreeView, hCurSelect), _T("Computer")) == 0)
		return;

	TVITEM tv;
	tv.mask = TVIF_CHILDREN | TVIF_HANDLE;
	tv.hItem = hCurSelect;
	TreeView_GetItem(_TreeView, &tv);

	if (tv.cChildren != 0)
	{
		LoadChildrenItem(_TreeView, hCurSelect, GetPathItem(_TreeView, hCurSelect));
	}
}

//these functions are used for nice UI, using Mr.Quang sources
void AlignTreeView(HWND _TreeView, int cy)
{
	RECT treeRC;
	GetWindowRect(_TreeView, &treeRC);

	MoveWindow(_TreeView, 0, 0, treeRC.right - treeRC.left, cy, SWP_SHOWWINDOW);
}

void AlignListView(HWND _ListView, HWND parenthWnd)
{
	RECT tree;
	GetWindowRect(GetDlgItem(parenthWnd, IDC_TREEVIEW), &tree);

	RECT main;
	GetWindowRect(parenthWnd, &main);

	MoveWindow(_ListView, tree.right - tree.left, 0, main.right - tree.right - 10, tree.bottom - tree.top, TRUE);
}

void DoResize(HWND _TreeView, HWND _ListView, HWND parentHWND, RECT g_TreeViewRect)
{
	RECT newTreeRC;
	GetClientRect(_TreeView, &newTreeRC);

	if (newTreeRC.right != g_TreeViewRect.right)
	{
		AlignListView(_ListView, parentHWND);
		g_TreeViewRect = newTreeRC;
	}
}
