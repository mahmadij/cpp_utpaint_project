#if !defined GUARD_THICKCHILD_HPP
#define GUARD_THICKCHILD_HPP

/** @file Thick.hpp
@author Mehrnaz Ahmadi Joobaneh
@date 2015-02-08
@version 1.0.0
@brief Thick class template.
*/

#include "Thick.hpp"

//The purpose of this sub-class is to restrict access to the Thick layer
class ThickChild : Thick
{
	friend int main();
	//ctor
	ThickChild(){};

	//dtor
	~ThickChild(){}

};//end ThickChild

#endif