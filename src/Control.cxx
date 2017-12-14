/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/Control.cxx
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
#include <regex>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <dirent.h>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "regexBuild.h"
#include "regexSupport.h"
#include "stringCombine.h"
#include "doubleErr.h"
#include "mathSupport.h"
#include "Glob.h"
#include "BUnit.h"
#include "Boundary.h"
#include "WorkData.h"
#include "cinderOption.h"
#include "cinderHistory.h"
#include "htapeProcess.h"
#include "tallyProcess.h"
#include "materialProcess.h"
#include "runProgs.h"

#include "Control.h"

Control::Control() :
  libraryPath("/home/stuartansell/cinder-1.05/data/c90lib0742"),
  outDirBase("Cell"),htapeNorm(-1.0)
  /*!
    Constructor
  */
{}

Control::Control(const Control& A) : 
  libraryPath(A.libraryPath),matFile(A.matFile),
  mcnpOFiles(A.mcnpOFiles),mcnpHFiles(A.mcnpHFiles),
  outDirBase(A.outDirBase),COpt(A.COpt),
  htapeNorm(A.htapeNorm),srcNorm(A.srcNorm),
  VolName(A.VolName),Vols(A.Vols),MatNumber(A.MatNumber),
  CellReMap(A.CellReMap),history(A.history),
  HT(A.HT),fluxes(A.fluxes),matCards(A.matCards)
  /*!
    Copy constructor
    \param A :: Control to copy
  */
{}

Control&
Control::operator=(const Control& A)
  /*!
    Assignment operator
    \param A :: Control to copy
    \return *this
  */
{
  if (this!=&A)
    {
      libraryPath=A.libraryPath;
      matFile=A.matFile;
      mcnpOFiles=A.mcnpOFiles;
      mcnpHFiles=A.mcnpHFiles;
      outDirBase=A.outDirBase;
      COpt=A.COpt;
      srcNorm=A.srcNorm;
      VolName=A.VolName;
      Vols=A.Vols;
      MatNumber=A.MatNumber;
      CellReMap=A.CellReMap;
      history=A.history;
      HT=A.HT;
      fluxes=A.fluxes;
      matCards=A.matCards;
    }
  return *this;
}


Control::~Control()
  /*!
    Destructor
  */
{}


void
Control::setLibrary(const std::string& LibPath)
  /*!
    Set the library name
    \param LibPath :: cinder database library
  */
{
  ELog::RegMethod RegA("Control","setLibrary");

  if (!boost::filesystem::exists(LibPath))
    throw ColErr::FileError(0,"LibPath",LibPath);

  libraryPath=LibPath;
  return;
}

void
Control::setBaseName(const std::string& baseN)
  /*!
    Set the basename for directories
    \param baseN :: basename library
  */
{
  ELog::RegMethod RegA("Control","setBaseName");
  if (!baseN.empty())
    outDirBase=baseN;
  else
    throw ColErr::EmptyValue<std::string>("baseN from option: dname");
  return;
}

void
Control::writeInput(const std::string& FName,
                    const int cellN,
                    const double vol) const
  /*!
    Write the input file for cinder
    \param FName :: File to use 
    \param cellN :: Cell number
    \param vol :: Volume
   */
{
  ELog::RegMethod RegA("Control","writeInput");

  if (!FName.empty())
    {
      std::ofstream OX;
      OX.open(FName.c_str());  //+StrFunc::makeString(CV.first));
      OX<<"  Beamline"<<std::endl;
      COpt.write(OX,getCellMat(cellN),vol,srcNorm);
      history.write(OX);
      OX.close();
    }
  return;
}


void
Control::writeLibrary(const std::string& FName) const
  /*!
    Write the library info to a file 
    \param FName :: File to use [needs to end in locate]
   */
{
  ELog::RegMethod RegA("Control","writeLibrary");

  if (!FName.empty())
    {
      if (FName.find("locate")==std::string::npos)
        ELog::EM<<"\nWARNING : locate file does not end in locate::"
                <<FName<<"\n"<<ELog::endCrit;

      std::ofstream OX;
      OX.open(FName.c_str());
      OX<<libraryPath<<std::endl;
      OX.close();
    }
  return;
}

void
Control::procCellReMap(const std::string& name,
		       std::string line)
  /*!
    Process the renumber line
    - currently only 
     -- A-B : newStartIndex  
     -- A : newIndex 
    \param name :: Name of cell group
    \param line :: extra line after the files tag
   */
{
  ELog::RegMethod RegA("Control","procCellReMap");

  int OA,OB;
  int NA;
  std::string::size_type pos=name.find('-');
  if (pos==std::string::npos || !pos)
    {

      if (StrFunc::convert(name,OA) &&
	  StrFunc::convert(line,NA) )
	{
	  if (OA!=NA)
	    CellReMap.emplace(OA,NA);
	}
      return;
    }

  if (StrFunc::convert(name.substr(0,pos),OA) &&
      StrFunc::convert(name.substr(pos+1),OB) &&
      StrFunc::convert(line,NA) )
    {
      for(;OA<=OB;OA++,NA++)
	CellReMap.emplace(OA,NA);	  
    }
  return;
}
  

void
Control::procCellList(const std::string& name,
		      std::string line)
  /*!
    Process the : files commandlines
    \param name :: Name of cell group
    \param line :: extra line after the files tag
   */
{
  ELog::RegMethod RegA("Control","procCellList");

  int cellN;
  double volume;
  std::string itemName;
  std::string cellFile;
  int tallyNumber;
  if (StrFunc::section(line,cellN) &&
      StrFunc::section(line,volume))
    {
      if (cellN>0)
	{
	  VolName.emplace(cellN,name);
	  Vols.emplace(cellN,volume);
	}
    }
  else if (StrFunc::section(line,itemName))
    {
      if (itemName=="tally" &&
          StrFunc::section(line,cellFile) &&
          StrFunc::section(line,tallyNumber))
        {
          addTallyCells(cellFile,tallyNumber);          
        }
    }
  return;
}


void
Control::addTallyCells(const std::string& fileName,const int tallyNumber)
  /*!
    Add volumes for each cell in the tally
    \param fileName :: Input file (AA.x)
    \param tallyNumber :: tally number
  */
{
  ELog::RegMethod RegA("Control","addTallyCells");
  std::ifstream IX;
  IX.open(fileName.c_str());

  //  std::regex tallySearch("^f(\\d+):");
  std::string tallySearch("^f(\\d+):");
  
  std::string SLine=StrFunc::getLine(IX);
  int tNum(0);
  std::string AItem;
  while(tNum!=tallyNumber && IX.good())
    {
      if (!StrFunc::section(SLine,AItem) ||
          !StrFunc::StrComp(AItem,tallySearch,tNum,0) ||
          tNum!=tallyNumber)
        SLine=StrFunc::getLine(IX);
    }
  if (!IX.good())
    throw ColErr::FileError(tallyNumber,"Tally not found",fileName);

  int cellN;
  size_t index;
  size_t outCnt(0);
  ELog::EM<<"volume: "<<ELog::endDiag;
  do
    {
      index=0;
      while(StrFunc::section(SLine,cellN))
        {
	  VolName.emplace(cellN,StrFunc::makeString(cellN));
	  Vols.emplace(cellN,1.0);
          index=1;
	  ELog::EM<<"  "<<cellN;
	  if (!(++outCnt % 12)) ELog::EM<<ELog::endDiag;
        }
      SLine=StrFunc::getLine(IX);
    } while (IX.good() && index);
  if (outCnt % 12) ELog::EM<<ELog::endDiag;
  ELog::EM<<"VOL Total == "<<outCnt<<ELog::endDiag; 


  IX.close();
  return;
}

void
Control::addMCNPOutFiles(std::string& component)
  /*!
    Add a file to the main vector list
    \param component :: Component of spc separated values
  */
{
  ELog::RegMethod RegA("Control","addMCNPOutFiles");
  
  std::string FName;
  while(StrFunc::section(component,FName))
    {
      mcnpOFiles.push_back(FName);
    }
  return;
}

void
Control::addMCNPHistpFiles(std::string& component)
  /*!
    Add a file to the htape vector list
    \param component :: Component of spc separated values
  */
{
  ELog::RegMethod RegA("Control","addMCNPHistpFiles");
  
  std::string FName;
  while(StrFunc::section(component,FName))
    {
      mcnpHFiles.push_back(FName);
    }
  return;
}


void
Control::procFiles(const std::string& tag,
		   std::string line)
  /*!
    Process the : files commandlines
    \param tag :: file type
    \param line :: extra line after the files tag
   */
{
  ELog::RegMethod RegA("Control","procFiles");

  runProgs& RP=runProgs::Instance();
  std::string component;
  if (StrFunc::section(line,component))
    {
      if (tag=="library")
	setLibrary(component);
      else if (tag=="dname")
	setBaseName(component);
      else if (tag=="mat_file")
        {
          matFile=component;
        }
      else if (tag=="mcnpx_outp")
        {
          addMCNPOutFiles(component);
          addMCNPOutFiles(line);
        }
      else if (tag=="mcnpx_histp")
        {
          addMCNPHistpFiles(component);
          addMCNPHistpFiles(line);
        }
      else if (tag=="cinder_exe")
        RP.setCinderEXE(component);
      else if (tag=="htape_exe")
        RP.setHTapeEXE(component);
      else if (tag=="tabcode_exe")
	RP.setTabcodeEXE(component);
      else
	throw ColErr::InContainerError<std::string>(tag,"Tag");
    }
  return;
}

void
Control::procNormalization(const std::string& tag,
			   std::string line)
  /*!
    Process the normalization lines
    \param tag :: identifier
    \param line :: extra line after the files tag
   */
{
  ELog::RegMethod RegA("Control","procNormalization");

  double V;
  if (StrFunc::section(line,V))
    {
      if (tag=="snorm")
	srcNorm=V;
      else if (tag=="hnorm")
	htapeNorm=V;
      else
	throw ColErr::InContainerError<std::string>(tag,"Tag");
    }
  return;
}

void
Control::readControlFile(const std::string& FName) 
  /*!
    Writes out information about the DataMap
    \param FName :: filename
  */
{
  ELog::RegMethod RegA("Control","readControlFile");

  const std::set<std::string> KeyWords
    ({"title_lines","files","run_options",
        "cinder_options","normalization",
        "history","cell_list"});
  
  std::ifstream IX;
  if (!FName.empty())
    IX.open(FName.c_str());
  if (!IX.is_open() || !IX.good())
    ELog::EM<<"Failed to open file:"<<FName<<ELog::endErr;


  std::string key;
  std::string AWord;
  size_t keyIndex(0);
  while(IX.good())
    {
      std::string line=StrFunc::getLine(IX,256);
      if (StrFunc::section(line,AWord))
        {

          if (KeyWords.find(AWord)!=KeyWords.end())
            {
              key=AWord;
	      keyIndex++;
            }
          else if (key=="title_lines")
            {
            }
          else if (key=="files")
            {
	      procFiles(AWord,line);
            }
          else if (key=="run_options")
            {
            }
          else if (key=="cinder_options")
            {
	      COpt.addString(AWord,line);
            }
          else if (key=="normalization")
            {
	      procNormalization(AWord,line);	      
            }
          else if (key=="history")
            {
              history.addLine(AWord,line);
            }
          else if (key=="cell_remap")
            {
	      //	      procCellList(AWord,line);
            }
          else if (key=="cell_list")
            {
	      procCellList(AWord,line);
            }
	  else
	    {
	      ELog::EM<<"Failed onkeyWord: "<<key<<ELog::endDiag;
	    }
        }
    }

  IX.close();
  ELog::EM<<"keywords read: "<<keyIndex<<ELog::endDiag;
  return;
}

void
Control::runHTape()
  /*!
    Construct the spltprod table
   */
{
  ELog::RegMethod RegA("Control","runHTape");

  for(const std::string& hFile : mcnpHFiles)
    {
      glob::Glob htapeFiles(hFile);
      while(htapeFiles)
        {
          ELog::EM<<"HTAPE == "<<htapeFiles.getFileName()<<ELog::endDiag;
          HT.addSProdFile(htapeFiles.getFileName(),Vols);
          htapeFiles.next();
        }
    }
  
  if (htapeNorm>0.0)
    {
      ELog::EM<<"HNorm == "<<htapeNorm<<ELog::endDiag;
      HT.scale(htapeNorm/srcNorm);
    }
  return;
}

void
Control::readFluxes()
  /*!
    Read the fluxes
  */
{
  ELog::RegMethod RegA("Control","readFluxes");

  std::string mcnpFile;
  for(const std::string& mcnpFile : mcnpOFiles)
    {
      glob::Glob fluxFiles(mcnpFile);
      while(fluxFiles)
        {
          ELog::EM<<"File == "<<fluxFiles.getFileName()<<ELog::endDiag;
          fluxes.readMCNP(fluxFiles.getFileName());
          fluxFiles.next();
        }
    }

  return;
}

void
Control::readMaterials()
  /*!
    Read the material
  */
{
  ELog::RegMethod RegA("Control","readMaterials");

  MatNumber.erase(MatNumber.begin(),MatNumber.end());
  for(const std::map<int,double>::value_type VT : Vols)
    MatNumber.emplace(VT.first,0);

  if (!matFile.empty())
    matCards.readMCNP(matFile,MatNumber);

  
  return;
}

int
Control::getCellMat(const int cellN) const
  /*!
    Simple search for material number
    \param cellN :: cell number
    \return material number
   */
{
  ELog::RegMethod RegA("Control","getCellMat");

  std::map<int,int>::const_iterator mc=MatNumber.find(cellN);
  if (mc==MatNumber.end())
    throw ColErr::InContainerError<int>(cellN,"cellN in MatNumber");
  
  return mc->second;
}

std::string
Control::getOutDir(const int cN) const
  /*!
    Determine if the cellNumber needs remapping
    and return the BaseDirectory name
    \param cN :: cell Number
    \
   */
{
  std::map<int,int>::const_iterator mc=CellReMap.find(cN);
  return (mc==CellReMap.end()) ?
    outDirBase+StrFunc::makeString(cN) :
    outDirBase+StrFunc::makeString(mc->second);
}

void
Control::writeCinderInput() const
  /*!
    Write the cinder input deck
  */
{
  ELog::RegMethod RegA("Control","writeCinderInput");

  const boost::filesystem::path topDir(boost::filesystem::current_path());
  int index(1);
  for(const std::map<int,double>::value_type& CV : Vols)
    {
      // If work to do
      if (fluxes.isValid(CV.first,1e-6))
	{
	  const std::string dirName=getOutDir(CV.first);
	  const boost::filesystem::path BDir(dirName);
	  
	  if(boost::filesystem::create_directories(BDir))
	    ELog::EM<<"Create DIR:"<<dirName<<ELog::endWarn;
	  boost::filesystem::current_path(BDir);
	  
	  writeLibrary("locate");
	  writeInput("input",CV.first,CV.second);
	  HT.writeSprods("splprods",CV.first,CV.second);
	  matCards.writeMaterials("material");
	  fluxes.writeFluxes("fluxes",CV.first);

	  const std::string cinderTXT=(index) ?
	    "cinderTXT"+StrFunc::makeString(index)+".log" : "";
	  const std::string tabcodeTXT=(index) ?
	    "tabcodeTXT"+StrFunc::makeString(index)+".log" : "";
	  if (runProgs::Instance().runCinder("",cinderTXT))
	    ELog::EM<<"Failed on CINDER "<<ELog::endErr;
	  if (runProgs::Instance().runTabCode("",tabcodeTXT))
	    ELog::EM<<"Failed on TABCODE "<<ELog::endErr;

	  index++;
	  boost::filesystem::current_path(topDir);
	}
      else
	ELog::EM<<"Cell "<<CV.first<<" has zero flux"<<ELog::endDiag;
    }

  return;
}
