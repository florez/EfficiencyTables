//*******************************************************************************
//	Filename:       CalculateEfficiencies	
//	Authors:	Andres Florez 
//	Last update:	Jul. 25, 2022
//*******************************************************************************

//--------------------------------------------------------------------------------------------------------------
// Load supplied header file and source codes
#include "CalculateEfficiencies.h"
#include "calcBayEffError.cpp"
#include "calcBinEffError.cpp"
//--------------------------------------------------------------------------------------------------------------

// Main function to normalize histograms
void CalculateEfficiencies()
{

  string inputRootFilePath = "RootFiles.txt"; 
  string inputDirectories  = "InputDir.txt";
  readConfigFile (inputRootFilePath, inputDirectories);
  
}

//=======================================================
// Store information from configuration file (e.g. root files, scale factors, cross-sections).
// The config. file lists each piece of information (its type and content) on a new line.
//=======================================================
void readConfigFile(string configFileList, string inputDir)
{
  ifstream inFile;		// Input stream for data file
  // Open input file
  inFile.open (configFileList.c_str(), ios::in);
  
  
  ifstream inDir;
  inDir.open(inputDir.c_str(), ios::in);
  
  while (inDir >> inputDir)
    {
     string mydir = inputDir;
      inputDirectoryName.push_back(mydir.c_str());
      
   } 
  // If can't open input file, exit the code
  if (!inFile)
    {
      cerr << "ERROR: Can't open input file: " << configFileList << endl;
      exit (1);
    }
  
  // Get all relevant information from the input file
  while (inFile >> inputType)
    {
      
      string configfile=inputType;
      
      TEnv *params = new TEnv ("config_file");
      params->ReadFile (configfile.c_str(), kEnvChange);
      // Get Root file name
      string rootFileName = params->GetValue ("rootfile", "thefile");
      inputRootFile.push_back (configfile.c_str() );
      
    }
  
  inFile.close();
  map<string, map<string, double>> map_eff_val;
  map<string, map<string, double>> map_eff_err;
  vector<string> file_names;
  // Loop over input ROOT files
  for (int j = 0; j < inputRootFile.size(); j++)
    {
      int numMax = 0;			// Total entries of maxHistoContent
      
      vector<double> numerator;
      vector<double> denominator;
      // Access input ROOT file (can access over secure shell)
      TFile* rootTFile = (TFile*) TFile::Open (inputRootFile.at(j).c_str());
      
      auto h_events = (TH1F*)rootTFile->Get("NRecoVertex/Events");
      
      // Remove characters form the string that make the inputRoot file name long.
      // This will be used later for the tables
      string file_name =  inputRootFile.at(j).c_str();
      size_t found =  file_name.find(".root");
      string theCut = ".root";
      file_name.erase(found, theCut.size());
      found = file_name.find("ToLL_M");
      if (found!=string::npos)
	{
	  theCut = "ToLL_M";
	  file_name.erase(found, theCut.size());
	}
      found = file_name.find("50_");
      if (found!=string::npos)
	{
	  theCut = "50_";
	  file_name.erase(found, theCut.size());
	}
      found = file_name.find("_InclusiveDecays");
      if (found!=string::npos)
	{
	  theCut = "_InclusiveDecay";
	  file_name.erase(found, theCut.size());
	}
      found = file_name.find("_inclusiveDecays");
      if (found!=string::npos)
	{
	  theCut = "_inclusiveDecay";
	  file_name.erase(found, theCut.size());
	}
      found = file_name.find("_leptonDecays");
      if (found!=string::npos)
	{
	  theCut = "_leptonDecays";
	  file_name.erase(found, theCut.size());
	}
      found = file_name.find("channel_");
      if (found!=string::npos)
	{
	  theCut = "channel_";
	  file_name.erase(found, theCut.size());
	}
      found = file_name.find("ToLNu");
      if (found!=string::npos)
	{
	  theCut = "ToLNu";
	  file_name.erase(found, theCut.size());
	}
      
      file_names.push_back(file_name.c_str());      
      for (int d = 0; d < inputDirectoryName.size(); d++){
	
        if (d == 0)
	  {
	    numerator.push_back(h_events->GetBinContent(2));
	    denominator.push_back(h_events->GetBinContent(1));
	  }
        else 
	  { 
	    auto h_met_num = (TH1F*)rootTFile->Get(inputDirectoryName.at(d).c_str()); 
	    auto h_met_den = (TH1F*)rootTFile->Get(inputDirectoryName.at(d-1).c_str());
	    numerator.push_back(h_met_num->Integral());
	    denominator.push_back(h_met_den->Integral());
	  }
      }
      for ( int k = 0; k < denominator.size(); k++)
	{
	  double efficiency = 0.0;
	  double efferror   = 0.0;
	  if ( (numerator.at(k) > 0.) && (denominator.at(k) > 0.))
	    {
              efficiency = numerator.at(k)/denominator.at(k);
              efferror   = calcBinEffError(numerator.at(k), denominator.at(k));
              if ((efficiency+efferror >= 1.) || ((efficiency-efferror) <= 0.))
		{
		  efferror = calcBayEffError(numerator.at(k), denominator.at(k)); 
		}
            }
	  if ((numerator.at(k)== 0.) && (denominator.at(k) > 0.))
	    {
              efficiency = 0.;
              efferror = calcBayEffError(1.0, denominator.at(k));
            }
	  if ((numerator.at(k)== 0.) && (denominator.at(k) == 0.))
	    {
              efficiency = 0.;
              efferror = calcBayEffError (1.0, denominator.at(0));
            }
	  // Define 2D maps
	  map_eff_val[file_name.c_str()][inputDirectoryName.at(k).c_str()] = efficiency*100;
	  map_eff_err[file_name.c_str()][inputDirectoryName.at(k).c_str()] = efferror*100;    
        }
      auto h_cumu = (TH1F*)rootTFile->Get("NDiJetCombinations/Events");
      double comulative_eff = 0.0;
      double comulative_eff_err = 0.0;       
      if (h_cumu->GetBinContent(2) > 0.)
	{
	  comulative_eff = h_cumu->GetBinContent(2)/h_cumu->GetBinContent(1);
	  comulative_eff_err =  calcBinEffError (h_cumu->GetBinContent(2), h_cumu->GetBinContent(1) );
	  if ((comulative_eff+comulative_eff_err >= 1.) || ((comulative_eff-comulative_eff_err) <= 0.))
	    {
              comulative_eff_err = calcBayEffError(h_cumu->GetBinContent(2), h_cumu->GetBinContent(1)); 
            }
	  
        }
      if (h_cumu->GetBinContent(2) == 0.)
	{
	  comulative_eff_err =  calcBayEffError (1.0, h_cumu->GetBinContent(1));
        }
      map_eff_val[file_name.c_str()]["Cumulative"] = comulative_eff*100;
      map_eff_err[file_name.c_str()]["Cumulative"] = comulative_eff_err*100;
      //cout << "Comulative efficiency =  " <<  comulative_eff*100. << " +/-  " << comulative_eff_err*100 << endl; 
      //cout << "================================================================= " << endl; 
      //cout << " " << endl;
    }
  
  inputDirectoryName.push_back("Cumulative");
  int fl = 0;
  int fl2 = 0;
  const int n_tab = (file_names.size()/5); 
  for (int t = 0; t <  n_tab; t++)
    {
      cout << "  " << endl;
      cout << " ====================================================================  " << endl;
      cout << "  " << endl;
      cout << "\\begin{table}[]"<< endl;
      cout << "\\begin{center}" << endl;
      cout << "\\caption {Table of Efficiencies}" << endl;
      cout << "\\resizebox{\\textwidth}{!}{" << endl;
      cout << "\\begin{tabular}{c | c | c | c | c  | c}\\hline" << endl;
      cout << "Criterion" << " & ";
      cout << file_names.at(fl).c_str() << " & ";
      fl++;
      cout << file_names.at(fl).c_str() << " & ";
      fl++;
      cout << file_names.at(fl).c_str() << " & ";
      fl++;
      cout << file_names.at(fl).c_str() << " & ";
      fl++;
      cout << file_names.at(fl).c_str() << " \\\\\\hline " << endl;
      cout << setprecision(2) << fixed;

      for (int dir = 0; dir < inputDirectoryName.size(); dir ++)
	{
	  if (dir == (inputDirectoryName.size() -1))
	    {  
	       cout << "\\hline" << endl;
	       cout << std::scientific << endl;
	    }	
	  fl2=fl-4;
	  cout << inputDirectoryName.at(dir).c_str() << " &  ";
	  cout << map_eff_val[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " $ \\pm $ ";
	  cout << map_eff_err[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " & ";
	  fl2++;
	  cout << map_eff_val[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " $ \\pm $ ";
	  cout << map_eff_err[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " & ";
	  fl2++;
	  cout << map_eff_val[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " $ \\pm $ ";
	  cout << map_eff_err[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " & ";
	  fl2++;
	  cout << map_eff_val[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " $ \\pm $ ";
	  cout << map_eff_err[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " & ";
	  fl2++;
	  cout << map_eff_val[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " $ \\pm $ ";
	  cout << map_eff_err[file_names.at(fl2).c_str()][inputDirectoryName.at(dir).c_str()] << " \\\\ " << endl;
	  
        }
      
      cout << "\\hline" << endl; 
      cout <<  "\\end{tabular}" << endl;
      cout << "}";
      cout << "\\end{center}" << endl;
      cout << "\\end{table}" <<endl;
      cout << "  " << endl;
      			
      
    }  
  
}   
