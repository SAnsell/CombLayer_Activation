/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/tallyProcess.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <climits>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <iterator>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "regexSupport.h"
#include "doubleErr.h"
#include "mathSupport.h"
#include "BUnit.h"
#include "Boundary.h"
#include "WorkData.h"
#include "tallyProcess.h"


std::ostream&
operator<<(std::ostream& OX,const tallyProcess& HT)
  /*!
    Write out to standard stream
    \param OX :: Output stream
    \param HT :: tallyProcess to write
    \return output stream 
  */
{
  HT.write(OX);
  return OX;
}


tallyProcess::tallyProcess() :
  nps(0)
  /*!
    Constructor
  */
{}

tallyProcess::tallyProcess(const tallyProcess& A) : 
  nps(A.nps),cellFlux(A.cellFlux)
  /*!
    Copy constructor
    \param A :: tallyProcess to copy
  */
{}

tallyProcess&
tallyProcess::operator=(const tallyProcess& A)
  /*!
    Assignment operator
    \param A :: tallyProcess to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nps=A.nps;
      cellFlux=A.cellFlux;
    }
  return *this;
}

tallyProcess::~tallyProcess()
  /*!
    Destructor
  */
{}

const WorkData&
tallyProcess::getWorkData(const int cellN) const
  /*!
    Get a flux for a beamline
    \param cellN :: cell number
    \return flux as WorkData
   */
{
  ELog::RegMethod Rega("tallyProcess","getWorkData");

  CTYPE::const_iterator mc=cellFlux.find(cellN);
  if (mc==cellFlux.end())
    throw ColErr::InContainerError<int>(cellN,"Cell number in cellFlux");
    
  return mc->second;   
}


void
tallyProcess::addFlux(const int cellN,const WorkData& WD)
  /*!
    Add a flux to the tally process
    -- assumes that weight has been fixed
    \param cellN :: cellNumber
    \param WD :: work data [normalized]
   */
{
  ELog::RegMethod RegA("tallyProcess","addFlux");

  CTYPE::iterator mc=cellFlux.find(cellN);
  if (mc==cellFlux.end())
    cellFlux.emplace(cellN,WD);
  else
    mc->second+=WD;
  return;
}
  

int
tallyProcess::find1Tally(std::istream& IX,int& tallyN,long int& nps)  
   /*!
     Read the file and find any 1tally 
     \param IX :: input stream
     \param tallyN :: tally number
     \param nps :: number of points
     \return true  on success
   */
{
  ELog::RegMethod RegA("tallyProcess","find1Tally");

  
  boost::regex tallySearch("^1tally\\s+(\\d+)\\s+nps =\\s+(\\d+)");

  
  std::vector<int> OutPts;
  std::string SLine=StrFunc::getLine(IX);  
  while (IX.good())
    {
      if (StrFunc::StrFullSplit(SLine,tallySearch,OutPts) &&
	  OutPts.size()>1)
	{
	  tallyN=OutPts[0];
	  nps=OutPts[1];
	  return 1;
	}
      SLine=StrFunc::getLine(IX);  
    }
  return 0;
}

void
tallyProcess::getFluxTally(std::istream& IX,
                           const long int npsFile)
  /*!
    Read the individual tally and make a workset 
    \param IX :: Input stream
    \param npsFile :: number of points in current file
   */
{
  ELog::RegMethod RegA("tallyProcess","getFluxTally");


  // FIRST : read file until :
  //   (a) cell: string string string
  //   (b) energy  is the NEXT line
  std::string testItem;
  int cellItem;
  
  std::string SLine=StrFunc::getLine(IX);

  while (IX.good() && SLine.find("=======")==std::string::npos)
    {

      
      if (StrFunc::section(SLine,testItem))
	{
          // both possible
	  if (testItem=="cell" || testItem=="cell:") 
	    {
	      std::vector<int> cellName;
	      // cell name : name : name 
	      while(StrFunc::section(SLine,cellItem))
		cellName.push_back(cellItem);
	      // energy line 
	      SLine=StrFunc::getLine(IX);
	      if (StrFunc::section(SLine,testItem) &&
		  testItem=="energy")
		{
		  readWorkEnergy(IX,npsFile,cellName);
		}
	    }
	}	      
      SLine=StrFunc::getLine(IX);

    }
  nps+=npsFile;
  return;
}

void
tallyProcess::cinderRebin(std::vector<WorkData>& FluxWork)
  /*!
    Rebin the fluxes to correct output stream
    \param FluxWork :: Data set
  */
{
  ELog::RegMethod RegA("tallyProcesss","cinderRebin");
  
  static const std::vector<double> Energy({

     0.000000,  5.000e-9,  1.000e-8,  1.500e-8,  2.000e-8, 2.500e-8,
     3.000e-8,  3.500e-8,  4.200e-8,  5.000e-8,  5.800e-8, 6.700e-8,
     8.000e-8,  1.000e-7,  1.520e-7,  2.510e-7,  4.140e-7, 6.830e-7,
     1.125e-6,  1.855e-6,  3.059e-6,  5.043e-6,  8.315e-6, 1.371e-5,
     2.260e-5,  3.727e-5,  6.144e-5,  1.013e-4,  1.670e-4, 2.754e-4,
     4.540e-4,  7.485e-4,  1.234e-3,  2.035e-3,  2.404e-3, 2.840e-3,
     3.355e-3,  5.531e-3,  9.119e-3,  1.503e-2,  1.989e-2, 2.554e-2,
     4.087e-2,  6.738e-2,  1.111e-1,  1.832e-1,  3.020e-1, 3.887e-1,
     4.979e-1,  0.639279,  0.820850,  1.108030,  1.353350, 1.737740,
     2.231300,  2.865050,  3.678790,  4.965850,  6.065000, 10.00000,
     14.91820,  16.90460,  20.00000,  25.00000
       });

  for(WorkData& CV : FluxWork)
    CV.rebin(Energy);

  return;
}
  

void
tallyProcess::readWorkEnergy(std::istream& IX,const long int npsFile,
			     const std::vector<int>& cellName)
  /*!
    Read cell fluxes from IX for each cell in cellName
    \param IX :: Input stream
    \param npsFile :: nps points in the fiel
    \param cellName :: cell list
  */
{
  ELog::RegMethod RegA("tallyProcess","readWorkEnergy");

  const size_t cnt(cellName.size());

  std::vector<WorkData> FluxWork(cnt);
  for(size_t index=0;index<cnt;index++)
    FluxWork[index].initX(0.0);
  
  boost::regex tallySearch("^\\s+cell\\s+(\\d+)");
  std::string SLine=StrFunc::getLine(IX);
  double energy;
  DError::doubleErr flux;

  while(IX.good() && SLine.find("total")==std::string::npos)
    {
      size_t index(0);
      if (StrFunc::section(SLine,energy))
	{
	  for(index=0;index<cnt;index++)
	    {
	      if (!StrFunc::section(SLine,flux))
		throw ColErr::FileError(static_cast<int>(index),
					"Error with data in tally",SLine);
	      FluxWork[index].pushData(energy,flux);
	    }
	}
      SLine=StrFunc::getLine(IX);	    
    }
  
  cinderRebin(FluxWork);
  for(size_t index=0;index<cnt;index++)
    {
      FluxWork[index].setWeight(static_cast<double>(npsFile));
      addFlux(cellName[index],FluxWork[index]);
    }
  
  return;
}

void
tallyProcess::readMCNP(const std::string& FName)
  /*!
    Read the mcnp file
    \param FName :: file to open
  */
{
  ELog::RegMethod RegA("tallyProcess","readMCNP");

  std::ifstream IX;
  if (!FName.empty())
    IX.open(FName.c_str());
  if (!IX.good())
    throw ColErr::FileError(0,FName,"MCNP File not opened");

  int tallyN(0);
  long int npsFile(0);
  
  while(find1Tally(IX,tallyN,npsFile))
    getFluxTally(IX,npsFile);
  
  if (!tallyN)
      throw ColErr::FileError(1,FName,"MCNP 1Tally not found");
  return;
}

bool
tallyProcess::isValid(const int cellN,
		      const double Tol) const
  /*!
    Write out the  fluxes in cinder format
    \param cellN :: cell number
    \param Tol :: tolerance
  */
{
  ELog::RegMethod RegA("tallyProcess","isValid");

  const WorkData& WD=getWorkData(cellN);
  DError::doubleErr IVal=WD.integrate(0,25.0);
  return (IVal.getVal()<Tol) ? 0 : 1;
}

void
tallyProcess::writeFluxes(const std::string& FName,
                          const int cellN) const
  /*!
    Write out the  fluxes in cinder format
    \param FName :: Filename
    \param cellN :: cell number
  */
{
  ELog::RegMethod RegA("tallyProcess","writeFluxes");

  boost::format ALineFMT("  Beamline%|74t|%d 0");
  boost::format BLineFMT
    ("tally  4     Integral of Rebinned flux is         %10.4e");
  boost::format CLineFMT(" %9.3e");


  const WorkData& WD=getWorkData(cellN);
  

  if (FName.empty()) return;
  std::ofstream OX(FName.c_str());

  OX<<(ALineFMT % WD.getSize())<<std::endl;;
  OX<<(BLineFMT % WD.integrate(0,1000.0).getVal());

  const std::vector<DError::doubleErr>& FD=WD.getYdata();
  for(size_t i=0;i<FD.size();i++)
    {
      if (!(i % 8))
	OX<<std::endl;
      OX<<(CLineFMT % FD[i].getVal());
    }
  OX<<std::endl;
  OX.close();
  return;
}

void
tallyProcess::write(std::ostream& OX) const
  /*!
    Write out some stuff : mainly debug
    \parma OX :: Output stream						
   */
{
  OX<<"Flux Cells::"<<std::endl;
  for(const CTYPE::value_type& Item : cellFlux)
    {
      OX<<"Cell Number: "<<Item.first<<std::endl;
      
      Item.second.write(OX);
    }
  return;
}

