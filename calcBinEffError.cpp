//*******************************************************************************
//	Filename:	calcBinEffError.cpp
//	Authors:	Andres Florez 
//	Last update:	Sep. 21, 2013
//
//	This file contains a function to calculate the Bayesian efficiency error.
//*******************************************************************************

float calcBinEffError (float numerator, float denominator)
{
  float efficiency = 0.0;
  float efferror   = 0.0;
  if ( denominator > 0 )
   {
     efficiency = numerator/denominator;
     efferror = sqrt(efficiency*(1.0-efficiency)/denominator);
   }
  return efferror;
}
