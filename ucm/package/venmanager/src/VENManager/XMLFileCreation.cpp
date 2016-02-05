/*
 * XMLFileCreation.cpp
 *
 *  Created on: 26-Aug-2014
 *      Author: dinesh
 */

#include "stdio.h"
#include <string.h>
#include "iostream"
#include <fstream>
#include <oadr/ven/VEN2b.h>
#include "XMLFileCreation.h"
#include "Main.h"

/**
    Method to log XML request and response to XML file
    @param filename - here file name is specified
    @param message - here request or response message is specified
*/
xmlFileCreation::xmlFileCreation(string fileName, string message) :
fileName(fileName), message(message)
{
	ofstream myfile;
	string file = XMLFilePath + fileName;
	myfile.open (file.c_str());
	myfile << message;
	myfile.close();

}

