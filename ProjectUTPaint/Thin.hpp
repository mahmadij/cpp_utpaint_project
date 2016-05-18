#if !defined GUARD_THIN_HPP
#define GUARD_THIN_HPP

/** @file Thin.hpp
@author Mehrnaz Ahmadi Joobaneh
@date 2015-02-08
@version 1.0.0
@brief Thin class template.
*/

#include <cassert>
#include <Windows.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <conio.h>

using namespace std;
#define PERR(res)\
if (!res){\
	cout << "ERROR: " << GetLastError() << " line=" << __LINE__ << endl; \
}

//this class acts as the thin layer and includes all the settings for initializing/restoring
//window as well as necessary variables
class Thin
{
	friend class Thick;

	//variables
private:
	HANDLE hStdin;
	HANDLE hStdout;
	unique_ptr<CHAR_INFO[], default_delete<CHAR_INFO[]>> pSaveBuffer;
	CONSOLE_SCREEN_BUFFER_INFO oldSCBI;
	SMALL_RECT srSaveRegion;
	COORD saveCoord;
	BOOL res;
	CONSOLE_CURSOR_INFO oldCCI;
	CONSOLE_CURSOR_INFO cci;
	
	//ctor
	Thin(){};

	//gets the old window buffer size
	void GetOldWinBuffSize()
	{
		res = GetConsoleScreenBufferInfo(hStdout, &oldSCBI);
		PERR(res);
	}//end GetOldWinBuffSize()

	//saves the initial info for desktop
	void SaveDesktop()
	{
		pSaveBuffer = make_unique<CHAR_INFO[]>(oldSCBI.dwSize.X*oldSCBI.dwSize.Y);
		GetConsoleCursorInfo(hStdout, &oldCCI);

		// smaller/safer than: vector<CHAR_INFO> saveBuffer(oldSCBI.dwSize.X*oldSCBI.dwSize.Y);
		saveCoord = { 0 };
		srSaveRegion = oldSCBI.srWindow;
		res = ReadConsoleOutput(hStdout, pSaveBuffer.get(), oldSCBI.dwSize, saveCoord, &srSaveRegion);
		PERR(res);
	}//end SaveDesktop()

	//sets the window size and buffer the same size
	void BuffSizeSame(SHORT const &cols, SHORT const &rows)
	{
		SMALL_RECT sr{ 0 };
		res = SetConsoleWindowInfo(hStdout, TRUE, &sr);
		PERR(res);
		// set screen buffer to correct size
		COORD bufferSize;
		bufferSize.X = 70;
		bufferSize.Y = 50;
		res = SetConsoleScreenBufferSize(hStdout, bufferSize);
		PERR(res);
		// set window to correct size
		sr.Right = 70-1;
		sr.Bottom = 50-1;
		res = SetConsoleWindowInfo(hStdout, TRUE, &sr);
		PERR(res);
	}//BuffSizeSame

	//clears the console
	void ClearConsole() {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hStdout, &csbi);
		DWORD consoleSize = csbi.dwSize.X*csbi.dwSize.Y;

		// fill the screen buffer with blanks
		COORD cursorHomeCoord{ 0 };
		DWORD charsWritten;
		FillConsoleOutputCharacter(hStdout, (TCHAR)' ', consoleSize, cursorHomeCoord, &charsWritten);
		FillConsoleOutputAttribute(hStdout, csbi.wAttributes, consoleSize, cursorHomeCoord, &charsWritten);
		SetConsoleCursorPosition(hStdout, cursorHomeCoord);
	}//end ClearConsole()

	//sets the cursor
	void SetCursorInfo()
	{
		CONSOLE_CURSOR_INFO cci;
		cci.bVisible = FALSE;
		cci.dwSize = 1;
		SetConsoleCursorInfo(hStdout, &cci);
	}//end SetCursorInfo()

	//restores to original state
	void RestoreOriginal()
	{
		{ SMALL_RECT sr{ 0 };
		res = SetConsoleWindowInfo(hStdout, TRUE, &sr);
		PERR(res);
		}
		res = SetConsoleScreenBufferSize(hStdout, oldSCBI.dwSize);
		PERR(res);
		res = SetConsoleWindowInfo(hStdout, TRUE, &oldSCBI.srWindow);

		// restore the desktop data
		res = WriteConsoleOutput(hStdout, pSaveBuffer.get(), oldSCBI.dwSize, saveCoord, &srSaveRegion);
		PERR(res);

		// restore the cursor
		res = SetConsoleCursorInfo(hStdout, &oldCCI);
		PERR(res)
			res = SetConsoleCursorPosition(hStdout, oldSCBI.dwCursorPosition);
		PERR(res);
	}//RestoreOriginal()

};//end class Thin


#endif