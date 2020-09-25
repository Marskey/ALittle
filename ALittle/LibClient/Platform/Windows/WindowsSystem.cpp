
#ifdef _WIN32

#include "WindowsSystem.h"

#include "ALittle/LibClient/RenderSystem/RenderSystem.h"
#include "ALittle/LibClient/ScheduleSystem/ScheduleSystem.h"
#include "ALittle/LibClient/ScriptSystem/ScriptSystem.h"

#include "Carp/carp_string.hpp"

#include <Windows.h>
#include <Iphlpapi.h>
#include <WinInet.h>
#include <ShlObj.h>

namespace ALittle
{

void Windows_InstallExe(const char* file_path)
{
	STARTUPINFO si;  
	PROCESS_INFORMATION pi;  
	ZeroMemory(&si, sizeof(si));  
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	CreateProcess(NULL, (LPTSTR)file_path, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
}

void Windows_StartApp(const char* package_name)
{
	STARTUPINFO si;  
	PROCESS_INFORMATION pi;  
	ZeroMemory(&si, sizeof(si));  
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	CreateProcess(NULL, (LPTSTR)package_name, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
}

void Windows_MoveTaskToBack()
{
	g_RenderSystem.HideView();
}

std::string s_temp_string;
const char* Windows_GetNetworkType()
{
	DWORD flag = 0;
	if (InternetGetConnectedState(&flag, 0))
	{
		s_temp_string = "OtherSystem_NotifyNetwork_wifi";
		return s_temp_string.c_str();
	}

	s_temp_string = "OtherSystem_NotifyNetwork_none";
	return s_temp_string.c_str();
}

static BOOL s_network_connected = FALSE;
static int s_network_last_time = 0;
static bool s_network_listener = false;

int Windows_StartNetworkListener()
{
	DWORD flag = 0;
	s_network_connected = InternetGetConnectedState(&flag, 0);
	s_network_listener = true;
	return 1;
}

void Windows_UpdateNetwork(int cur_time)
{
	if (s_network_listener == false) return;
	if (cur_time - s_network_last_time < 2000) return;
	s_network_last_time = cur_time;

	DWORD flag = 0;
	BOOL new_connected = InternetGetConnectedState(&flag, 0);
	if (new_connected == s_network_connected) return;

	s_network_connected = new_connected;
	if (s_network_connected)
		g_ScheduleSystem.PushExternalEvent("__ALITTLEAPI_NetworkChanged", -1, "OtherSystem_NotifyNetwork_wifi", -1);
	else
		g_ScheduleSystem.PushExternalEvent("__ALITTLEAPI_NetworkChanged", -1, "OtherSystem_NotifyNetwork_none", -1);
}

const char* Windows_GetDeviceID()
{
	static std::string info;
	if (info.size()) return info.c_str();

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	// alloc memory
	ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *) HeapAlloc(GetProcessHeap(), 0, sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) return info.c_str();

	// get info, alloc memory
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		HeapFree(GetProcessHeap(), 0, pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) HeapAlloc(GetProcessHeap(), 0, ulOutBufLen);
		if (pAdapterInfo == NULL) return info.c_str();
	}
	// get info
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			if (pAdapter->Type == MIB_IF_TYPE_ETHERNET) {
				for (UINT i = 0; i < pAdapter->AddressLength; ++i) {
					char text[32] = {0};
					sprintf_s(text, "%.2X", (int)pAdapter->Address[i]);
					info.append(text);
				}
				break;
			}
			pAdapter = pAdapter->Next;
		}
	}
	// release
	if (pAdapterInfo)
		HeapFree(GetProcessHeap(), 0, pAdapterInfo);

	return info.c_str();
}

int Windows_GetScreenWidth()
{
	return GetSystemMetrics(SM_CXSCREEN); 
}

int Windows_GetScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

void Windows_SystemSelectFile(const char* init_dir)
{
	wchar_t path_name[MAX_PATH] = {0};
	OPENFILENAMEW ofn = { OPENFILENAME_SIZE_VERSION_400 };				// or {sizeof (OPENFILENAME)}
	ofn.hwndOwner = g_RenderSystem.GetSysWMinfo().info.win.window;	// set parent window
	ofn.lpstrFilter = NULL;				// not set filter
	ofn.lpstrFile = path_name;			// set path
	ofn.nMaxFile = sizeof(path_name);	// size of buffer
	ofn.lpstrTitle = L"ѡ���ļ�";			// set title
	wchar_t cur_dir[MAX_PATH] = {0};
	GetCurrentDirectoryW(sizeof(cur_dir), cur_dir);	// get current path
	std::wstring gbk_dir = cur_dir;
	if (init_dir)
		gbk_dir = CarpString::UTF82Unicode(init_dir);
	ofn.lpstrInitialDir = gbk_dir.c_str();			// init first dir
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	BOOL bOk = GetOpenFileNameW(&ofn);				// open dialog
	SetCurrentDirectoryW(cur_dir);					// reset old dir

	std::string utf8_path_name = CarpString::Unicode2UTF8(path_name);
	if (bOk)
		g_ScriptSystem.Invoke("__ALITTLEAPI_SystemSelectFile", utf8_path_name.c_str());
	else
		g_ScriptSystem.Invoke("__ALITTLEAPI_SystemSelectFile");
}

void Windows_SystemSelectDirectory(const char* init_dir)
{
	wchar_t path_name[MAX_PATH] = { 0 };
	BROWSEINFOW bInfo = { 0 };				// or {sizeof (OPENFILENAME)}
	bInfo.hwndOwner = g_RenderSystem.GetSysWMinfo().info.win.window;	// set parent window
	bInfo.lpszTitle = L"ѡ���ļ���";			// set title
	bInfo.pszDisplayName = path_name;
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI; /*����һ���༭�� �û������ֶ���д·�� �Ի�����Ե�����С֮���..*/
	LPITEMIDLIST lpDlist = SHBrowseForFolderW(&bInfo);
	if (lpDlist != NULL)
	{
		SHGetPathFromIDListW(lpDlist, path_name);
		std::string utf8_path_name = CarpString::Unicode2UTF8(path_name);
		g_ScriptSystem.Invoke("__ALITTLEAPI_SystemSelectDirectory", utf8_path_name.c_str());
	}
	else
	{
		g_ScriptSystem.Invoke("__ALITTLEAPI_SystemSelectDirectory");

	}
}

void Windows_SystemSaveFile(const char* utf8_file_name, const char* init_dir)
{
	std::wstring file_name;
	if (utf8_file_name) file_name = CarpString::UTF82Unicode(utf8_file_name);
	
	wchar_t path_name[MAX_PATH];
	OPENFILENAMEW ofn = { OPENFILENAME_SIZE_VERSION_400 };//or  {sizeof (OPENFILENAME)}  
	// lStructSize  
	// ָ������ṹ�Ĵ�С�����ֽ�Ϊ��λ��  
	// Windows 95/98��Windows NT 4.0������ΪWindows 95/98��Windows NT 4.0��������WINVER��_WIN32_WINNT >= 0x0500����ʱ��  
	//  Ϊ�����Աʹ��OPENFILENAME_SIZE_VERSION_400��  
	// Windows 2000�����߰汾���������ʹ��sizeof (OPENFILENAME) ��  
	ofn.hwndOwner = GetForegroundWindow();// ��OR�����ļ��Ի���ĸ�����  
	ofn.lpstrFilter = NULL; 
	//������ ���Ϊ NULL ��ʹ�ù�����  
	//�����÷�������  ע�� /0  
	path_name[0] =  0;
	wcscpy_s(path_name, file_name.c_str());
	ofn.lpstrFile = path_name;  
	ofn.nMaxFile = sizeof(path_name);//����û�ѡ���ļ��� ·�����ļ��� ������  
	ofn.lpstrTitle = L"�����ļ�";//ѡ���ļ��Ի������  
	wchar_t szCurDir[MAX_PATH];
	GetCurrentDirectoryW(sizeof(szCurDir),szCurDir);
	std::wstring gbk_dir = szCurDir;
	if (init_dir)
		gbk_dir = CarpString::UTF82Unicode(init_dir);
	ofn.lpstrInitialDir = gbk_dir.c_str();			// init first dir
	ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;//�����Ҫѡ�����ļ� ��������  OFN_ALLOWMULTISELECT��־  

	BOOL bOk = GetSaveFileNameW(&ofn);//���öԻ�����ļ�
	SetCurrentDirectoryW(szCurDir); 

	std::string utf8_path_name = CarpString::Unicode2UTF8(path_name);
	if (bOk)
		g_ScriptSystem.Invoke("__ALITTLEAPI_SystemSaveFile", utf8_path_name.c_str());
	else
		g_ScriptSystem.Invoke("__ALITTLEAPI_SystemSaveFile");
}

void Windows_OpenUrlBySystemBrowser(const char* url)
{
	if (url == 0) return;

	std::string commond = "start \"\" ";
	commond.append("\"").append(url).append("\"");
	_wsystem(CarpString::UTF82Unicode(commond.c_str()).c_str());
}

bool Windows_HasClipboardImage()
{
	const SDL_SysWMinfo& wm_info = g_RenderSystem.GetSysWMinfo();
	if (wm_info.info.win.window == 0) return false;
	if (!IsClipboardFormatAvailable(CF_BITMAP) || !::OpenClipboard(wm_info.info.win.window)) return false;
	HANDLE hBitmap = ::GetClipboardData(CF_BITMAP);
	::CloseClipboard();
	return hBitmap != NULL;
}

SDL_Surface* Windows_GetClipboardImage()
{
	SDL_Surface* surface = 0;

	const SDL_SysWMinfo& wm_info = g_RenderSystem.GetSysWMinfo();
	if (wm_info.info.win.window == 0) return surface;
	if (!IsClipboardFormatAvailable(CF_BITMAP) || !::OpenClipboard(wm_info.info.win.window)) return surface;
	HANDLE hMem = ::GetClipboardData(CF_BITMAP);
	if (hMem)
	{
		HBITMAP hBitmap = (HBITMAP)GlobalLock(hMem);  
		
		BITMAP bmp;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		BITMAPINFOHEADER bmi = { 0 };  
		bmi.biSize = sizeof(BITMAPINFOHEADER);  
		bmi.biPlanes = 1;  
		bmi.biBitCount = 32;  
		bmi.biWidth = bmp.bmWidth;
		bmi.biHeight = -bmp.bmHeight;  
		bmi.biCompression = BI_RGB;  
		bmi.biSizeImage = bmp.bmWidth*bmp.bmHeight;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		int rmask = 0xff000000;
		int gmask = 0x00ff0000;
		int bmask = 0x0000ff00;
		int amask = 0x000000ff;
#else
		int rmask = 0x000000ff;
		int gmask = 0x0000ff00;
		int bmask = 0x00ff0000;
		int amask = 0xff000000;
#endif
		surface = SDL_CreateRGBSurface(0, bmp.bmWidth, bmp.bmHeight, 32, rmask, gmask, bmask, amask);
		if (surface)
		{
			GetDIBits(wm_info.info.win.hdc, hBitmap, 0, bmp.bmHeight, surface->pixels, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

			int pixel_count = bmp.bmWidth * bmp.bmHeight;
			for (int i = 0; i < pixel_count; ++i)
			{
				unsigned char* pixel = (unsigned char*)surface->pixels + i * 4;

				// swap r and b
				unsigned char tmp = pixel[0];
				pixel[0] = pixel[2];
				pixel[2] = tmp;
			}
		}

		GlobalUnlock(hMem);
	}
	::CloseClipboard();
	return surface;
}

bool Windows_SetClipboardImage(SDL_Surface* surface)
{
	if (surface == 0) return false;

	const SDL_SysWMinfo& wm_info = g_RenderSystem.GetSysWMinfo();
	if (wm_info.info.win.window == 0) return false;

	if (!::OpenClipboard(wm_info.info.win.window)) return false;
	::EmptyClipboard();

	bool result = false;

	int pixel_count = surface->w * surface->h;
	for (int i = 0; i < pixel_count; ++i)
	{
		unsigned char* pixel = (unsigned char*)surface->pixels + i * 4;

		// swap r and b
		unsigned char tmp = pixel[0];
		pixel[0] = pixel[2];
		pixel[2] = tmp;
	}

	HBITMAP hBitmap = CreateCompatibleBitmap(wm_info.info.win.hdc, surface->w, surface->h);
	if (hBitmap)
	{
		BITMAPINFOHEADER bmi = { 0 };  
		bmi.biSize = sizeof(BITMAPINFOHEADER);  
		bmi.biPlanes = 1;  
		bmi.biBitCount = 32;
		bmi.biWidth = surface->w;  
		bmi.biHeight = -surface->h;  
		bmi.biCompression = BI_RGB;  
		bmi.biSizeImage = surface->w*surface->h;

		SelectObject(wm_info.info.win.hdc, hBitmap);		
		SetDIBits(wm_info.info.win.hdc, hBitmap, 0, surface->h, surface->pixels, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
		result = (SetClipboardData(CF_BITMAP, hBitmap) != NULL);
		DeleteObject(hBitmap);
	}

	::CloseClipboard();

	for (int i = 0; i < pixel_count; ++i)
	{
		unsigned char* pixel = (unsigned char*)surface->pixels + i * 4;

		// swap b and r
		unsigned char tmp = pixel[0];
		pixel[0] = pixel[2];
		pixel[2] = tmp;
	}

	return result;
}

} // ALittle

#endif // _WIN32
