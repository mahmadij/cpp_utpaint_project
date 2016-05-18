#if !defined GUARD_THICK_HPP
#define GUARD_THICK_HPP

/** @file Thick.hpp
@author Mehrnaz Ahmadi Joobaneh
@date 2015-02-08
@version 1.0.0
@brief Thick class template.
*/

#include "Thin.hpp"

bool done;
//handler for ^C event
BOOL CtrlHandler(DWORD ctrlType) {
	switch (ctrlType) {
	case CTRL_C_EVENT:
		done = true;
		return TRUE;		// I handled this event
	}
	return FALSE;			// Event is not handled, use the default
}

//Thick layer is wrapping the Thin layer for access reasons
//This class contains all operations related to paint and related eventhandlers
class Thick 
{
private:
	//variables
	Thin thin;
	POINT pt;
	unsigned counter;
	unsigned i;
	vector<WORD> attr;
	//const WORD * dat;
	COORD loc;
	COORD bufferLoc;
	DWORD nCharsWritten;
	string msg = " ";
	SHORT const cols = 50;
	SHORT const rows = 70;

	////////Events handlers////////

	//Event handler to capture c key event
	void ProcessKeyEvent(KEY_EVENT_RECORD const& ker)
	{
		if (unsigned(ker.uChar.AsciiChar) == 99)
			thin.ClearConsole();
	}//end ProcessKeyEvent()

	//Event handler to capture mouse event
	void ProcessMouseEvent(MOUSE_EVENT_RECORD const& mer)
	{
		switch (mer.dwEventFlags)
		{
		case 0:	// button pressed or released
		{
			auto mask = mer.dwButtonState;
			if (mask&FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				LeftClick(mer);
			}
			if (mask&RIGHTMOST_BUTTON_PRESSED)
			{
				RightClick();
			}

			break;
		}
		case MOUSE_MOVED: //left button drag
			{
				if (mer.dwButtonState&FROM_LEFT_1ST_BUTTON_PRESSED)
				{
				
					bufferLoc = mer.dwMousePosition;
					DWORD charsWritten;
					FillConsoleOutputCharacter(thin.hStdout, (TCHAR)' ', msg.size(), bufferLoc, &charsWritten);
					FillConsoleOutputAttribute(thin.hStdout, attr[i], msg.size(), bufferLoc, &charsWritten);
				}//end if
				break;
			}//end case
		}//end switch
	}//end ProcessMouseEvent()
	
	//helper for right click event
	void RightClick()
	{
		++counter;
		i = counter % 3;
		//dat = &attr[i];
		GetCursorPos(&pt);
		loc = { (short)pt.x, (short)pt.y };
	}//end RightClick

	//helper for left click event
	void LeftClick(MOUSE_EVENT_RECORD const& mer)
	{
		bufferLoc = mer.dwMousePosition;
		FillConsoleOutputCharacter(thin.hStdout, (TCHAR)' ', msg.size(), bufferLoc, &nCharsWritten);
		FillConsoleOutputAttribute(thin.hStdout, attr[i], msg.size(), bufferLoc, &nCharsWritten);
	}//end LeftClick()

	int Paint()
	{

		//thin.BuffSizeSame(cols, rows);
		// Install the control handler to trap ^C
		if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE)) {
			cout << "Terminate program with Ctrl-C\n";
		}
		else {
			cerr << "ERROR: failed to install control handler." << endl;
			return EXIT_FAILURE;
		}

		//set the console mode
		DWORD consoleMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;

		// Read input memory buffer
		vector<INPUT_RECORD> inBuffer(128);
		while (!done) 
		{
			DWORD numEvents;
			if (!ReadConsoleInput(thin.hStdin, inBuffer.data(), inBuffer.size(), &numEvents)) 
			{
				cerr << "Failed to read console input\n";
				break;
			}
			
				// process buffer contents
				for (size_t iEvent = 0; iEvent < numEvents; ++iEvent)
				{
					switch (inBuffer[iEvent].EventType)
					{
					case MOUSE_EVENT:
						ProcessMouseEvent(inBuffer[iEvent].Event.MouseEvent);
						break;
					case KEY_EVENT:
						ProcessKeyEvent(inBuffer[iEvent].Event.KeyEvent);
						break;
					case CTRL_C_EVENT:
						CtrlHandler(CTRL_C_EVENT);
						break;
					}//end switch
				}//end for			
		}//end while
		return 0;
	}//end paint
	
	protected:
		
		//The program runs from here
		int Execute()
		{
			try
			{
				//Saving the original info
				thin.GetOldWinBuffSize();
				thin.SaveDesktop();

				// make the screen buff and screen size the same
				thin.BuffSizeSame(cols, rows);

				//Set the cursor
				thin.SetCursorInfo();

				//Clear console
				thin.ClearConsole();


				//Do the work for drawing
				Paint();

				//Restore console to the original state
				thin.RestoreOriginal();

				//Close handles
				CloseHandle(thin.hStdin);
				CloseHandle(thin.hStdout);
			}
			catch (exception ex)
			{
				return EXIT_FAILURE;
			}
			return 0;

		}//end Execute()

		//ctor
		Thick()
		{
			thin.hStdin = GetStdHandle(STD_INPUT_HANDLE);
			SetConsoleMode(thin.hStdin, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);
			thin.hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			done = false;
			counter = 0;
			i = 0;
			attr = {
				FOREGROUND_RED | BACKGROUND_RED,
				FOREGROUND_GREEN | BACKGROUND_GREEN,
				FOREGROUND_BLUE | BACKGROUND_BLUE
			};
		}//end ctor


		//dtor
		~Thick()
		{

		}//end dtor

};//end class


#endif