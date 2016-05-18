/** @file PaintMain.cpp
@author Mehrnaz Ahmadi Joobaneh
@date 2015-02-08
@version 1.0.0
@brief execution point.
*/
#include "ThickChild.hpp"

int main()
{
	try
	{
		//Create a source of access to Thick class
		ThickChild* myApp = new ThickChild();

		//execute the program by accessing the all-in-one Thick function Execute()
		myApp->Execute();

		//cleaning up
		delete myApp;
	}
	catch (exception e)
	{

	}

}//end main()
