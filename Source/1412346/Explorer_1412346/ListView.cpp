#include "stdafx.h"
#include "Explorer_1412346.h"
#include <winioctl.h>
#include <shlwapi.h>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <shellapi.h>
#pragma comment(lib, "shlwapi.lib")

#define BYTES_IN_GB 1073741824
#define BYTES_IN_MB 1048576
#define BYTES_IN_KB 1024

HIMAGELIST lhImageList;

int liFolderID;
int liHardID;
int liFileID;

void CreateImageListView(HWND _Listview, HINSTANCE hIns)
{
	InitCommonControls();

	lhImageList = ImageList_Create(16, 16, ILC_COLOR32, 2, 0);
	HICON hIcon;
	// load the picture from the resource

	hIcon = LoadIcon(hIns, MAKEINTRESOURCE(IDI_DISK));
	liHardID = ImageList_AddIcon(lhImageList, hIcon, NULL);

	hIcon = LoadIcon(hIns, MAKEINTRESOURCE(IDI_FILE));
	liFileID = ImageList_AddIcon(lhImageList, hIcon, NULL);

	hIcon = LoadIcon(hIns, MAKEINTRESOURCE(IDI_FOLDER));
	liFolderID = ImageList_AddIcon(lhImageList, hIcon, NULL);
	ListView_SetImageList(_Listview, lhImageList, LVSIL_SMALL);
}

HWND Create_Load_ListView(HWND parentWnd, long ID, HINSTANCE hParentInst,
	int nWidth, int nHeight,
	int x, int y, long lExtStyle, long lStyle)
{
	HWND _ListView = CreateWindow(WC_LISTVIEW, NULL, WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_EX_CLIENTEDGE,
		x, y, nWidth, nHeight,
		parentWnd, (HMENU)ID,
		GetModuleHandle(NULL),
		NULL);

	CreateImageListView(_ListView, hParentInst);

	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 120;
	lvCol.pszText = _T("Name");
	ListView_InsertColumn(_ListView, 0, &lvCol);

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT | LVCF_WIDTH;
	lvCol.cx = 80;
	lvCol.pszText = _T("Type");
	ListView_InsertColumn(_ListView, 1, &lvCol);

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_RIGHT;
	lvCol.pszText = _T("Size");
	ListView_InsertColumn(_ListView, 2, &lvCol);

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.pszText = _T("Description");
	ListView_InsertColumn(_ListView, 3, &lvCol);

	LoadComputer(_ListView);

	return _ListView;
}

void LoadComputer(HWND _ListView)
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_TEXT | LVCF_FMT;
	lvCol.fmt = LVCFMT_LEFT | LVCF_WIDTH;
	lvCol.cx = 100;
	lvCol.pszText = _T("Type");
	ListView_SetColumn(_ListView, 1, &lvCol);

	lvCol.mask = LVCF_TEXT | LVCF_FMT;
	lvCol.fmt = LVCFMT_RIGHT | LVCF_WIDTH;
	lvCol.cx = 80;
	lvCol.pszText = _T("Size");
	ListView_SetColumn(_ListView, 2, &lvCol);

	lvCol.mask = LVCF_TEXT | LVCF_FMT;
	lvCol.fmt = LVCFMT_RIGHT | LVCF_WIDTH;
	lvCol.cx = 80;
	lvCol.pszText = _T("Free Space");
	ListView_SetColumn(_ListView, 3, &lvCol);

	//Computer
	ListView_DeleteAllItems(_ListView);
	LV_ITEM lv;

	TCHAR cDriveLetter = 'A';
	DWORD dwDrivemap = GetLogicalDrives();
	TCHAR *buffDisplay;
	TCHAR *buffTemp;
	TCHAR *buffSize;
	TCHAR *buffFreeSpace;

	__int64 nSize = 0;
	__int64 nFreeSize = 0;
	int  id = 0;

	while (cDriveLetter <= 'Z')
	{
		if (0 < (dwDrivemap & 0x00000001L))
		{
			TCHAR *buffName = new TCHAR[10];
			buffDisplay = new TCHAR[10];
			buffTemp = new TCHAR[10];
			buffSize = new TCHAR[20];
			buffFreeSpace = new TCHAR[20];

			wsprintf(buffName, _T("%c:\\\0"), cDriveLetter);

			SHGetDiskFreeSpaceEx(buffName, NULL, (PULARGE_INTEGER)&nSize, NULL);
			buffSize = ConvertSize(nSize);

			GetDiskFreeSpaceEx(buffName, NULL, NULL, (PULARGE_INTEGER)&nFreeSize);
			buffFreeSpace = ConvertSize(nFreeSize);

			GetVolumeInformation(buffName, buffDisplay, 10, NULL, NULL, NULL, NULL, 0);

			wcscpy(buffTemp, buffDisplay);
			wsprintf(buffDisplay, _T("%s(%c:)\0"), buffTemp, cDriveLetter);

			lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE ;
			lv.iItem = id++;
			lv.iSubItem = 0;
			lv.pszText = buffDisplay;
			lv.lParam = (LPARAM)buffName;
			lv.iImage = liHardID;
			ListView_InsertItem(_ListView, &lv);

			lv.mask = LVIF_TEXT;

			lv.iSubItem = 1;
			ListView_SetItem(_ListView, &lv);

			lv.iSubItem = 2;
			lv.pszText = buffSize;
			ListView_SetItem(_ListView, &lv);

			lv.iSubItem = 3;
			lv.pszText = buffFreeSpace;
			ListView_SetItem(_ListView, &lv);
		}
		cDriveLetter++;
		dwDrivemap = dwDrivemap >> 1;
	}
}



void LoadFileAndFolder(HWND _ListView, LPCWSTR path)
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_WIDTH;
	lvCol.cx = 180;
	ListView_SetColumn(_ListView, 0, &lvCol);

	lvCol.cx = 30;
	lvCol.mask = LVCF_TEXT | LVCF_FMT;
	lvCol.fmt = LVCFMT_RIGHT;
	lvCol.pszText = _T("Size");
	ListView_SetColumn(_ListView, 1, &lvCol);


	lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 120;
	lvCol.pszText = _T("Type");
	ListView_SetColumn(_ListView, 2, &lvCol);

	lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 200;
	lvCol.pszText = _T("Date Modified");
	ListView_SetColumn(_ListView, 3, &lvCol);

	ListView_DeleteAllItems(_ListView);

	TCHAR buffer[MAX_PATH_LENGTH];

	swprintf(buffer, _T("%s\\*"), path);

	WIN32_FIND_DATA fd;
	HANDLE hFile;
	LV_ITEM lv;
	TCHAR * folderPath;
	int itemID = 0;
	TCHAR *filePath;
	LARGE_INTEGER a;
	__int64 sizeNum;

	hFile = FindFirstFileW(buffer, &fd);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN) &&
			(StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 4];

			wsprintf(folderPath, L"%s\\%s", path, fd.cFileName);

			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = itemID;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.iImage = liFolderID;
			lv.lParam = (LPARAM)folderPath;

			ListView_InsertItem(_ListView, &lv);
			ListView_SetItemText(_ListView, itemID, 2, _T("Folder"));
			ListView_SetItemText(_ListView, itemID, 3, _GetDateModified(fd.ftLastWriteTime));
			
			itemID++;
		}
		else if ((StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			TCHAR *size = new TCHAR[20];

			filePath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 4];

			wsprintf(filePath, L"%s\\%s", path, fd.cFileName);
			lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			lv.iItem = itemID;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.iImage = liFileID;
			lv.lParam = (LPARAM)filePath;

			//Get full of size, this is a bug in your Explorer
			a.HighPart = fd.nFileSizeHigh;
			a.LowPart = fd.nFileSizeLow;

			sizeNum = a.QuadPart;

			size = ConvertSize(sizeNum);

			ListView_InsertItem(_ListView, &lv);
			ListView_SetItemText(_ListView, itemID, 1, size);
			ListView_SetItemText(_ListView, itemID, 2, _GetType(fd));
			ListView_SetItemText(_ListView, itemID, 3, _GetDateModified(fd.ftLastWriteTime));
		}
	} while (FindNextFileW(hFile, &fd));
}

LPCWSTR GetItemPath(HWND _ListView, int iItem)
{
	if (iItem == -1)
		return NULL;

	LVITEM lv;
	lv.mask = LVIF_PARAM;
	lv.iItem = iItem;
	ListView_GetItem(_ListView, &lv);
	return (LPCWSTR)lv.lParam;
}

void ExecuteSelection(HWND _ListView)
{
	LPCWSTR path = GetItemPath(_ListView, ListView_GetSelectionMark(_ListView));

	if (path == NULL)
		return;

	WIN32_FIND_DATA fd;
	GetFileAttributesEx(path, GetFileExInfoStandard, &fd);

	if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		ListView_DeleteAllItems(_ListView);
		LoadFileAndFolder(_ListView, path);
	}
	else
		ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWNORMAL);
}

void LoadChild(HWND _ListView, LPCWSTR path)
{
	if (path == NULL || _ListView == NULL)
		return;

	if (!StrCmp(path, _T("Computer")))
		LoadComputer(_ListView);
	else
		LoadFileAndFolder(_ListView, path);
}

TCHAR* ConvertSize(__int64 sizeInBytes)
{
	TCHAR rBuff[12];
	TCHAR* buffSize = new TCHAR[12];
	if (sizeInBytes > BYTES_IN_GB)
	{
		double size =(double) sizeInBytes / BYTES_IN_GB;
		std::ostringstream ss;
		ss << size;
		std::string s = ss.str();
		buffSize[s.size()] = 0;
		std::copy(s.begin(), s.end(), buffSize);
		wcscat(buffSize, _T(" GB"));
	}
	else if (sizeInBytes > BYTES_IN_MB)
	{
		double size = (double)sizeInBytes / BYTES_IN_MB;
		std::ostringstream ss;
		ss << size;
		std::string s = ss.str();
		buffSize[s.size()] = 0;
		std::copy(s.begin(), s.end(), buffSize);
		wcscat(buffSize, _T(" MB"));
	}
	else if (sizeInBytes > BYTES_IN_KB)
	{
		double size = (double)sizeInBytes / BYTES_IN_KB;
		std::ostringstream ss;
		ss << size;
		std::string s = ss.str();
		buffSize[s.size()] = 0;
		std::copy(s.begin(), s.end(), buffSize);
		wcscat(buffSize, _T(" KB"));
	}
	else
	{
		std::ostringstream ss;
		ss << sizeInBytes;
		std::string s = ss.str();
		buffSize[s.size()] = 0;
		std::copy(s.begin(), s.end(), buffSize);
		wcscat(buffSize, _T(" Bytes"));
	}

	return buffSize;
}


LPWSTR _GetType(const WIN32_FIND_DATA &fd)
{
	int length = wcslen(fd.cFileName);

	int dotPos = length;
	while (fd.cFileName[dotPos] != L'.' && dotPos >= 0)
	{
		dotPos--;
	}

	if (dotPos == 0)
		return L"Unknown";

	TCHAR *szExtension = new TCHAR[length - dotPos + 1];

	int j = 0;
	for (int i = dotPos; i < length; i++)
	{
		szExtension[j++] = fd.cFileName[i];
	}
	szExtension[j] = 0;

	TCHAR pszOut[256];
	HKEY hKey;
	DWORD dwType = REG_SZ;
	DWORD dwSize = 256;

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szExtension, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return _T("Unknown");
	}

	delete szExtension;

	if (RegQueryValueEx(hKey, NULL, NULL, &dwType, (PBYTE)pszOut, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return _T("Unknown");
	}
	RegCloseKey(hKey);

	TCHAR *pszPath = new TCHAR[1000];
	dwSize = 1000;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, pszOut, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return _T("Unknown");
	}

	if (RegQueryValueEx(hKey, NULL, NULL, &dwType, (PBYTE)pszPath, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return _T("Unknown");
	}
	RegCloseKey(hKey);

	return pszPath;
}

LPWSTR _GetDateModified(const FILETIME &ftLastWrite)
{
	SYSTEMTIME stUTC, stLocal;
	FileTimeToSystemTime(&ftLastWrite, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	TCHAR *buffer = new TCHAR[50];
	wsprintf(buffer, _T("%02d/%02d/%04d %02d:%02d"), stLocal.wDay, stLocal.wMonth, stLocal.wYear,stLocal.wHour,stLocal.wMinute);

	return buffer;
}