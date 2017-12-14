/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/materialProcess.cxx
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
#include "stringCombine.h"
#include "doubleErr.h"
#include "mathSupport.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "materialProcess.h"


std::ostream&
operator<<(std::ostream& OX,const materialProcess& MT)
  /*!
    Write out to standard stream
    \param OX :: Output stream
    \param MT :: materialProcess to write
    \return output stream 
  */
{
  MT.write(OX);
  return OX;
}


materialProcess::materialProcess() 
  /*!
    Constructor
  */
{}

materialProcess::materialProcess(const materialProcess& A) : 
  matStore(A.matStore)
  /*!
    Copy constructor
    \param A :: materialProcess to copy
  */
{}

materialProcess&
materialProcess::operator=(const materialProcess& A)
  /*!
    Assignment operator
    \param A :: materialProcess to copy
    \return *this
  */
{
  if (this!=&A)
    {
      matStore=A.matStore;
    }
  return *this;
}


materialProcess::~materialProcess()
  /*!
    Destructor
  */
{}

 
void
materialProcess::procMaterial(const int matN,std::string matStr)
  /*!
    Add the material to the material dat base
    \param matN :: material number
    \param matStr :: material string
   */
  
{
  ELog::RegMethod RegA("materialProcess","procMaterial");

  if (matN)
    {

      MTYPE::const_iterator mc=matStore.find(matN);
      if (mc!=matStore.end())
	throw ColErr::InContainerError<int>(matN,"Matnumber exists");

      std::string matItems,testItem;
      while(StrFunc::section(matStr,testItem) &&
	    !testItem.empty() && std::isdigit(testItem[0]))
	matItems+=testItem+" ";
			     
      MonteCarlo::Material A;
      if (A.setMaterial(matN,matItems,"",""))
	throw ColErr::InvalidLine("MatN:"+StrFunc::makeString(matN),
				  matStr,0);
      matStore.emplace(matN,A);
    }
  
  return;
}


void
materialProcess::processCellCards(std::istream& IX,
				  std::map<int,int>& cellMat) const
  /*!
    Read material number for each cell from IX 
    This reads the mcnp input file (which is repoduced in the
    output file) -- it could read table 60 but is that universal?
    \param IX :: Input stream
    \param cellMat :: Cells : Materials to find
  */
{
  ELog::RegMethod RegA("materialProcess","processCellCards");


  std::string SLine=StrFunc::getLine(IX);
  std::string numberItem;

  int cNum,matNum;
  double density;
  std::map<int,int>::iterator mc;
  std::set<int> active;
  
  while(IX.good() && SLine.find("++ END ++")==std::string::npos)
    {
      // throw away the line-nubmer values
      if (StrFunc::section(SLine,numberItem) &&
	  StrFunc::section(SLine,cNum) &&
	  StrFunc::section(SLine,matNum) &&
	  StrFunc::section(SLine,density) )
	{
	  if (matNum!=0 && density>0.0 && density<2.0)
	    {
	      mc=cellMat.find(cNum);
	      if (mc!=cellMat.end())
		{
		  mc->second=matNum;
		  active.insert(cNum);
		}
	    }
	}
      SLine=StrFunc::getLine(IX);
    }
  
  if (active.size()!=cellMat.size())
    ELog::EM<<"MisMatch on cell/activesize" <<ELog::endErr;

  ELog::EM<<"Processing Cell: "<<active.size()<<ELog::endDiag;
  
  return;
}




void
materialProcess::processMaterialCards(std::istream& IX)
  /*!
    Read materials from IX 
    \param IX :: Input stream
  */
{
  ELog::RegMethod RegA("materialProcess","processMaterialCards");


  boost::regex mCard("^\\s*m(\\d+)\\s+");
  boost::regex nonCard("^\\s*[a-zA-Z]+");


  std::string SLine=StrFunc::getLine(IX);

  std::string testItem,numberItem;
  int mIndex(0);


  std::string matLine;
  while(IX.good() && SLine.find("++ END ++")==std::string::npos)
    {
      if (StrFunc::section(SLine,numberItem))
	{		
	  // new material line found:
	  int newMIndex;
	  if (StrFunc::StrComp(SLine,mCard,newMIndex,0))
	    {
	      procMaterial(mIndex,matLine);
	      mIndex=newMIndex;
	      StrFunc::section(SLine,testItem);
	      matLine=SLine;
	    }
	  // only add number lines
	  else if (!StrFunc::StrLook(SLine,nonCard) )
	    {
	      matLine+=SLine;
	    }
	}
      SLine=StrFunc::getLine(IX);
    }
  // PROCESS LAST Material
  if(mIndex)
    procMaterial(mIndex,matLine);
  
  return;
}

int
materialProcess::findMaterialCards(std::istream& IX) const
  /*!
    Find the start of the material cards
    \param IX :: Input stream
    \return true if found
  */
{
  ELog::RegMethod RegA("materialProcess","findMaterialCards");

  std::string SLine=StrFunc::getLine(IX);
  while(IX.good())
    {
      if (SLine.find("MATERIAL CARDS")!=std::string::npos)
	return 1;
      SLine=StrFunc::getLine(IX);
    }
  
  return 0;
}


int
materialProcess::findCellCards(std::istream& IX) const
  /*!
    Find the start of the material cards
    \param IX :: Input stream
    \return true if found
  */
{
  ELog::RegMethod RegA("materialProcess","findCellCards");

  std::string SLine=StrFunc::getLine(IX);
  while(IX.good())
    {
      if (SLine.find("CELL CARDS")!=std::string::npos)
	return 1;
      SLine=StrFunc::getLine(IX);
    }  
  return 0;
}


void
materialProcess::readMCNP(const std::string& FName,
			  std::map<int,int>& cellMat)
  /*!
    Read the mcnp file
    \param FName :: file to open
  */
{
  ELog::RegMethod RegA("materialProcess","readMCNP");

  std::ifstream IX;
  if (FName.empty())
    throw ColErr::FileError(0,"Empty filename given","");
    IX.open(FName.c_str());
  if (!IX.good())
    throw ColErr::FileError(0,FName,"MCNP File not opened");


  if (findCellCards(IX))
    {
      processCellCards(IX,cellMat);
    }
  else
    {
      ELog::EM<<"Filename == "<<FName<<ELog::endDiag;
      throw ColErr::FileError(0,"CellCards:",FName);
    }
  
  if (findMaterialCards(IX))
    {
      processMaterialCards(IX);
    }
  else
    throw ColErr::FileError(0,"Material Cards",FName);
  
  return;
}

void
materialProcess::writeMaterials(const std::string& FName) const
  /*!
    Write out the 
   */
{
  ELog::RegMethod Rega("materialProcess","writeMaterial");
  if (FName.empty()) return;
  std::ofstream OX(FName.c_str());

  OX<<" Beamline"<<std::endl;
  for(const MTYPE::value_type& Item : matStore)
    {
      Item.second.writeCinder(OX);
    }
  OX.close();
  return;
}
  

void
materialProcess::write(std::ostream& OX) const
  /*!
    Write out some stuff : mainly debug
    \parma OX :: Output stream						
   */
{
  OX<<"Materials::"<<std::endl;
  for(const MTYPE::value_type& Item : matStore)
    {
      OX<<"Mat Number: "<<Item.first<<std::endl;
      Item.second.writeCinder(OX);
    }
  return;
}

