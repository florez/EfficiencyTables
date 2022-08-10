// Header guard
#ifndef CALCULATEEFFICIENCIES_H
#define CALCULATEEFFICIENCIES_H

//=======================================================
// Contents of header file
//=======================================================

// Load C++ libraries
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>
// Load ROOT library
#include <TError.h>

// Load libraries (automatically included by ROOT)
#include <TFile.h>
#include <TH1.h>
#include <TCollection.h>
#include <TDirectory.h>
#include <TKey.h>
#include <TGraphAsymmErrors.h>
#include <TClass.h>

using namespace std;

string inputType;				// Type of info from input file
string inputDir;
vector<string> inputRootFile;			// ROOT file storing cut information
vector<string> inputDirectoryName;
void readConfigFile (string configFileList, string InputDirec);

#endif

