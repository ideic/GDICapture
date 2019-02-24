// GDICapture.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>

//#pragma comment (lib, "Shcore.lib")
LOGFONTW * font = nullptr;
int CALLBACK EnumFamCallBack(LOGFONTW *lpelf, CONST TEXTMETRICW *lpntm, DWORD FontType, LPARAM lParam) {
	std::wstring faceName(lpelf->lfFaceName);
	if (faceName == L"Algerian") {
		font = lpelf;
		return FALSE;
	}
	return TRUE;
}

void SaveBitmap(HBITMAP hBitmap)
{
	HDC					hdc = NULL;
	LPVOID				pBuf = NULL;
	BITMAPINFO			bmpInfo;
	BITMAPFILEHEADER	bmpFileHeader;
	std::string _output = "ScreenCap";

	do {

		hdc = GetDC(NULL);
		ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS); //DIB_RGB_COLORS

		if (bmpInfo.bmiHeader.biSizeImage <= 0)
			bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount + 7) / 8;

		if ((pBuf = malloc(bmpInfo.bmiHeader.biSizeImage)) == NULL)
		{
			std::cerr << "Unable to Allocate Bitmap Memory" <<std::endl;
			break;
		}

		bmpInfo.bmiHeader.biCompression = BI_RGB;
		GetDIBits(hdc, hBitmap, 0, bmpInfo.bmiHeader.biHeight, pBuf, &bmpInfo, DIB_RGB_COLORS); // DIB_RGB_COLORS

		std::ofstream outputFile;
		outputFile.open(_output + ".bmp", std::ofstream::binary);
		if (outputFile.fail()) {
			std::cout << "Error"<< std::string(strerror(errno)) << std::endl;
			return;
		}

		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;
		bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType = 'MB';
		bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		outputFile.write(reinterpret_cast<char*>(&bmpFileHeader), sizeof(BITMAPFILEHEADER));
		outputFile.write(reinterpret_cast<char*>(&bmpInfo.bmiHeader), sizeof(BITMAPINFOHEADER));
		outputFile.write(reinterpret_cast<char*>(pBuf), bmpInfo.bmiHeader.biSizeImage);


		outputFile.close();

	} while (false);

	if (hdc)
		ReleaseDC(NULL, hdc);

	if (pBuf)
		free(pBuf);

}


int main()
{
	auto dpires =  SetProcessDPIAware();
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //SM_CXVIRTUALSCREEN
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN); // SM_CYVIRTUALSCREEN

	std::cout << "Width:" << std::to_string(nScreenWidth) << std::endl;
	std::cout << "Height:" << std::to_string(nScreenHeight) << std::endl;

	std::cout << "Virtual Width:" << std::to_string(GetSystemMetrics(SM_CXVIRTUALSCREEN)) << std::endl;
	std::cout << "Virtual Height:" << std::to_string(GetSystemMetrics(SM_CYVIRTUALSCREEN)) << std::endl;

	std::cout << "Fulll Width:" << std::to_string(GetSystemMetrics(SM_CXFULLSCREEN)) << std::endl;
	std::cout << "Full Height:" << std::to_string(GetSystemMetrics(SM_CYFULLSCREEN)) << std::endl;

	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
	SelectObject(hCaptureDC, hBitmap);
	BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);

	CURSORINFO cursor = { sizeof(cursor) };
	::GetCursorInfo(&cursor);
	if (cursor.flags == CURSOR_SHOWING) {
		RECT rcWnd;
		::GetWindowRect(hDesktopWnd, &rcWnd);
		ICONINFOEXW info = { sizeof(info) };
		::GetIconInfoExW(cursor.hCursor, &info);
		const int x = cursor.ptScreenPos.x - rcWnd.left - info.xHotspot;
		const int y = cursor.ptScreenPos.y - rcWnd.top - info.yHotspot;
		BITMAP bmpCursor = { 0 };
		::GetObject(info.hbmColor, sizeof(bmpCursor), &bmpCursor);
		::DrawIconEx(hCaptureDC, x, y, cursor.hCursor, bmpCursor.bmWidth, bmpCursor.bmHeight, 0, NULL, DI_NORMAL);
	}

	{
		std::wstring text(L"2019. 02. 23. 18:46:05 TCAndC");
		RECT rcTextWnd;
		::GetWindowRect(hDesktopWnd, &rcTextWnd);
		//DrawText(hCaptureDC, text.c_str(), text.length(), &rcTextWnd, DT_CENTER || DT_TOP);
		
		auto r =  SetMapMode(hCaptureDC, MM_TEXT);

		r= SetBkMode(hCaptureDC, TRANSPARENT);
		//r = SetTextAlign(hCaptureDC, TA_LEFT || TA_TOP);

	/*	auto brush =(HBRUSH) GetStockObject(WHITE_BRUSH);
		auto oldBrush = (HBRUSH)SelectObject(hCaptureDC, brush);*/

		//auto hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

		EnumFontFamilies(hCaptureDC, (LPCTSTR)NULL,(FONTENUMPROC)EnumFamCallBack, NULL);
		
		//font->lfHeight = -48;//  MulDiv(200, GetDeviceCaps(hCaptureDC, LOGPIXELSY), 72);//-font->lfHeight * 5;
		//font->lfWidth = 0;// font->lfWidth * 5;
		r = SetTextColor(hCaptureDC, RGB(0xaa, 0xaa, 0xaa));
		
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(logFont));
		logFont.lfHeight = -96; // see PS
		logFont.lfWeight = FW_BOLD;
		wcscpy(logFont.lfFaceName, std::wstring(L"Algerian").c_str());

		//MoveToEx(hCaptureDC, x,  y, (LPPOINT)NULL);
		HFONT cFont = ::CreateFontIndirect(font);

		auto oldFont = SelectObject(hCaptureDC, &cFont);

		SIZE size;
		auto tres = GetTextExtentPoint32(hCaptureDC, text.c_str(), text.length(), &size);

		auto x = rcTextWnd.right - size.cx;
		auto y = GetSystemMetrics(SM_CYFULLSCREEN) - size.cy;


		//r = SetTextJustification(hCaptureDC, )
		//r= ExtTextOut(hCaptureDC, rcTextWnd.left, rcTextWnd.top, ETO_NUMERICSLATIN, &rcTextWnd,text.c_str(), text.length(), NULL);
		r= TextOut(hCaptureDC, x-300, y-300, text.c_str(), text.length());

		DeleteObject(cFont);
	}

	HDC					hdc = NULL;
	LPVOID				pBuf = NULL;
	BITMAPINFO			bmpInfo;

	hdc = GetDC(NULL);
	ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);

	if (bmpInfo.bmiHeader.biSizeImage <= 0)
		bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount + 7) / 8;

	if ((pBuf = malloc(bmpInfo.bmiHeader.biSizeImage)) == NULL)
	{
		throw std::runtime_error("Unable to Allocate Bitmap Memory");
	}

	bmpInfo.bmiHeader.biCompression = BI_RGB;
	GetDIBits(hdc, hBitmap, 0, bmpInfo.bmiHeader.biHeight, pBuf, &bmpInfo, DIB_RGB_COLORS);

	SaveBitmap(hBitmap);
	ReleaseDC(hDesktopWnd, hDesktopDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hBitmap);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
