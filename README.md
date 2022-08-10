# EfficiencyTables
The code runs over the output root files produced by the BSM3G analyzer. 
You need to make a list of the ROOT files you want to run over, as the one provided in this repository, called "RootFiles.txt".
Currenty the code runs on a number of root files that must be at least 5 samples or multiples of 5 (10, 15. 20, etc). 
If you have for example 8 files, the code will crash. For this scenario, please you can either modify the code your self for your files, 
or include two more root files (you can repeat root files), to make the 10 files, and then delete 
the repited entries and modify the "tabular" command on of the second table. 
There is a file called "InputDir.txt" that you also have to modify to include the names of the directories for your analysis. 
The code makes tables for the relative efficiencies of each cut and also calculates the cumulative efficiency for each process. 
The tool is intended to make all the efficiency tables we often are requested to make by conveners. We usually have over 40 root files, counting 
the samples that are binned on HT or masss, so the tool intends to help make the process fast and simple.
Finally, note that the code uses the "Met" histogram to estimate the relative efficiencies. You need to have this hisotogram or replace it by a different 
histogram that has one entry per event. 
