/*
 * ClearRegistration.cpp
 *
 *  Created on: 26-Aug-2014
 *      Author: dinesh
 */

#include "../Main.h"
#include "ClearRegistration.h"

using namespace std;

/**
 *  Method to clear registration details
 * @return
 */
bool clearRegistration::clearregistration()
{
	sqliteDB sdb;
	if(!sdb.sqliteDBwriteString("venid","", ven_param, "venparam"))
		return false;
	if(!sdb.sqliteDBwriteString("registrationid","", ven_param, "venparam"))
		return false;
	if(!sdb.sqliteDBwriteString("vtnid","", ven_param, "venparam"))
		return false;

	return true;
}
