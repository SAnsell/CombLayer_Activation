/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   work/htapeProcess.cxx
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
#include "stringCombine.h"
#include "regexSupport.h"
#include "doubleErr.h"
#include "mathSupport.h"
#include "cinderOption.h"
#include "cinderHistory.h"
#include "runProgs.h"
#include "cellProduction.h"
#include "htapeProcess.h"


std::ostream&
operator<<(std::ostream& OX,const htapeProcess& HT)
  /*!
    Write out to standard stream
    \param OX :: Output stream
    \param HT :: htapeProcess to write
    \return output stream 
  */
{
  HT.write(OX);
  return OX;
}


htapeProcess::htapeProcess() :
  nps(0)
  /*!
    Constructor
  */
{}

htapeProcess::htapeProcess(const htapeProcess& A) : 
  nps(A.nps),cellProd(A.cellProd)
  /*!
    Copy constructor
    \param A :: htapeProcess to copy
  */
{}

htapeProcess&
htapeProcess::operator=(const htapeProcess& A)
  /*!
    Assignment operator
    \param A :: htapeProcess to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nps=A.nps;
      cellProd=A.cellProd;
    }
  return *this;
}

htapeProcess::~htapeProcess()
  /*!
    Destructor
  */
{}


void
htapeProcess::processHTape(const std::map<int,double>& cellVols)
  /*!
    Process the htape
    \param cellVols :: Cell volumes
  */
{
  ELog::RegMethod RegA("htapProcess","processHTape");

  typedef std::map<int,double> MTYPE;
  
  std::ofstream H8;
  std::ofstream H14;
  std::ofstream H15;

  H8.open("int08");
  H14.open("int14");
  H15.open("int15");

  std::string extra;
  H8<<"AUTOMATED ACTIVATION SCRIPT FOR ISOTOPE PRODUCTION DATA"<<std::endl;
  H14<<"AUTOMATED ACTIVATION SCRIPT FOR GAS PRODUCTION DATA"<<std::endl;
  H15<<"AUTOMATED ACTIVATION SCRIPT FOR ISOTOPE DESTRUCTION DATA"<<std::endl;
  for(const MTYPE::value_type& mc : cellVols)
    {
      H8 <<"for cell: "<<mc.first<<extra<<std::endl;
      H14<<"for cell: "<<mc.first<<extra<<std::endl;
      H15<<"for cell: "<<mc.first<<extra<<std::endl;
      H8 <<"108,0,0,0,0,1,0,1.0,0/"<<std::endl;
      H14<<"114,0,0,0,0,1,0/"<<std::endl;
      H15<<"115,0,0,0,2,1,0,1.0,0/"<<std::endl;
      H8 <<mc.first<<"/"<<std::endl;
      H14<<mc.first<<"/"<<std::endl;
      H15<<mc.first<<"/"<<std::endl;
      extra="\n";
    }
  H8.close();
  H14.close();
  H15.close();
  
  return;
}

void
htapeProcess::readZaid(const size_t prodType,
		       cellProduction& CProd,
		       std::istream& IX)
  /*!
    Read production part of an htape output file
    \param prodType :: Type of produciton [0-2]
    \param CProd :: Cell production unit
    \param IX :: Input stream
  */
{
  ELog::RegMethod RegA("htapeProcess","readZaid");

  // find : z = 12 n = 12  1.4d-4 0.3 
  boost::regex zaidSearch("z =\\s*(\\d+)\\s*n =\\s*(\\d+)\\s+(\\S+)\\s+(\\S+)");
  boost::regex midSearch("n =\\s*(\\d+)\\s+(\\S+)\\s+(\\S+)");
  std::string SLine=StrFunc::getLine(IX,512);

  int z(0);
  int n;
  double frac,errFrac;
  while(IX.good())
    {
      std::vector<std::string> Comp;
      if (SLine.find("complete")!=std::string::npos)
	return;
      
      if (StrFunc::StrSingleSplit(SLine,zaidSearch,Comp))
	{
	  if (StrFunc::section(Comp[0],z) &&
	      StrFunc::section(Comp[1],n) &&
	      StrFunc::sectionMCNPX(Comp[2],frac) &&
	      StrFunc::sectionMCNPX(Comp[3],errFrac) )
	    {
	      CProd.cellIndexProd(prodType,z,n,0,frac,errFrac);
	    }	  
	}
      else if (StrFunc::StrSingleSplit(SLine,midSearch,Comp))
        {
	  if (StrFunc::section(Comp[0],n) &&
	      StrFunc::sectionMCNPX(Comp[1],frac) &&
	      StrFunc::sectionMCNPX(Comp[2],errFrac) )
	    {
	      CProd.cellIndexProd(prodType,z,n,0,frac,errFrac);
	    }	  
        }
      SLine=StrFunc::getLine(IX,512);
    }
      
  ELog::EM<<"Returning on empty file"<<ELog::endDiag;
  return;
}

long int 
htapeProcess::readHeader(const size_t prodType,std::istream& IX,
                         CTYPE& fileProd)  const
   /*!
     Read the header of a production type from the htape output
     file
     \param prodType :: type number 0-2
     \param IX :: input stream
   */
{
  ELog::RegMethod RegA("htapeProcess","readHeader");

  
  boost::regex npsSearch("statistical degrees of freedom.*=\\s+(\\d+)");
  boost::regex caseSearch("^1\\s+case no.\\s+(\\d+)");
  boost::regex cellSearch("for cell:\\s*(\\d+)");

  long int npsFile(0);
  int caseNum,cellNum;  
  std::string SLine=StrFunc::getLine(IX,512);
  size_t statusFlag(0);
  cellProduction* CellPtr(0);     // cell point to use later:
  size_t outCnt(0);
  ELog::EM<<"htapeCells: "<<ELog::endDiag;
  while (IX.good())
    {

      switch (statusFlag)
	{
        case 0:
          if (StrFunc::StrComp(SLine,npsSearch,npsFile,0))
            {
              ELog::EM<<"NPS == "<<npsFile<<ELog::endDiag;
              statusFlag=1;
            }
	case 1:
	  if (StrFunc::StrComp(SLine,caseSearch,caseNum,0))
	    {
	      statusFlag=2;
	    }
	  break;
	case 2:  // cell number
	  if (StrFunc::StrComp(SLine,cellSearch,cellNum,0))
	    {
	      CellPtr=findCellProd(fileProd,cellNum);
	      statusFlag=3;
	      ELog::EM<<"  "<<cellNum;
	      if (!(++outCnt % 12)) ELog::EM<<ELog::endDiag;
		      
	    }
	  break;

	case 3: // production:
	  readZaid(prodType,*CellPtr,IX);
	  statusFlag=1;
	  break;
	}
      SLine=StrFunc::getLine(IX);
    }
  if (outCnt % 12) ELog::EM<<ELog::endDiag;
  ELog::EM<<"CELL Total == "<<outCnt<<" "<<npsFile<<ELog::endDiag; 
  return npsFile;
}

long int
htapeProcess::procProduction(CTYPE& prodMap)
  /*!
    Process the outt08 isotope production tape
    \param prodMap :: production map to add results too
    \return number of points
  */
{
  ELog::RegMethod RegA("htapeProcess","procProduction");
  std::ifstream IX;
  IX.open("outt08");
  
  if (!IX.good())
    throw ColErr::FileError(8,"outt08","File no open");
  return readHeader(0,IX,prodMap);
}

void
htapeProcess::procGas(CTYPE& prodMap)
  /*!
    Process the outt14 isotope gas production tape
    \param prodMap :: production map to add results too
  */
{
  ELog::RegMethod RegA("htapeProcess","procGas");
  std::ifstream IX;
  IX.open("outt14");

  const size_t prodType(0);   // PRODUCTION
  if (!IX.good())
    throw ColErr::FileError(14,"outt14","File no open");

  // lines are:
  //     (hydrogen  deuterium  tritium   total h)
  //     (helium-3  helium-4   total he  gas(h&he)
  // followed by values + error
  //

  boost::regex caseSearch("^1\\s+case no.\\s+(\\d+)");
  boost::regex cellSearch("for cell:\\s*(\\d+)");

  boost::regex hydrogenSearch("^\\s+hydrogen\\s+deuterium");
  boost::regex heliumSearch("^\\s+helium-3\\s+helium-4");
  
  std::string SLine=StrFunc::getLine(IX,512);
  size_t statusFlag(0);
  cellProduction* CellPtr(0);     // cell point to use later:

  int caseNum,cellNum;
  double A,B,C;
  double AErr,BErr,CErr;
  while (IX.good())
    {
      switch (statusFlag)
	{
	case 0:
	  if (StrFunc::StrComp(SLine,caseSearch,caseNum,0))
	    {
	      statusFlag=1;
	    }
	  break;
	case 1:  // cell number
	  if (StrFunc::StrComp(SLine,cellSearch,cellNum,0))
	    {
	      CellPtr=findCellProd(prodMap,cellNum);
	      statusFlag=2;
	    }
	  break;
	case 2: // production:
	  if (StrFunc::StrLook(SLine,hydrogenSearch))
	    statusFlag=3;
	  break;
	case 3:  // read line
	  if (StrFunc::sectionMCNPX(SLine,A) &&
	      StrFunc::sectionMCNPX(SLine,AErr) &&
	      StrFunc::sectionMCNPX(SLine,B) &&
	      StrFunc::sectionMCNPX(SLine,BErr) &&
	      StrFunc::sectionMCNPX(SLine,C) &&
	      StrFunc::sectionMCNPX(SLine,CErr))
	    {
              // CHANGE HERE
	      CellPtr->cellIndexProd(prodType,1,0,0,A,AErr);  // hydrogen
	      CellPtr->cellIndexProd(prodType,1,1,0,B,BErr);  // deuterium
	      CellPtr->cellIndexProd(prodType,1,2,0,C,CErr);  // tritium
	      statusFlag=4;
	    }
	  break;
	case 4: // production:
	  if (StrFunc::StrLook(SLine,heliumSearch))
	    statusFlag=5;
	  else if (!StrFunc::isEmpty(SLine))
	    ELog::EM<<"Failed line "<<SLine<<ELog::endDiag;

	  break;
	case 5:  // read line
	  if (StrFunc::sectionMCNPX(SLine,A) &&
	      StrFunc::sectionMCNPX(SLine,AErr) &&
	      StrFunc::sectionMCNPX(SLine,B) &&
	      StrFunc::sectionMCNPX(SLine,BErr) )
	    {
	      CellPtr->cellIndexProd(prodType,2,1,0,A,AErr);  // he-3
	      CellPtr->cellIndexProd(prodType,2,2,0,B,BErr);  // he-4
	    }
	  statusFlag=0;
	}
      SLine=StrFunc::getLine(IX);
    }
  if (statusFlag!=0)
    ELog::EM<<"Failed to read gas production file statusFlag="
	    <<statusFlag<<ELog::endErr;
  return;
}

void
htapeProcess::procDestruction(CTYPE& prodMap)
  /*!
    Process the outt15 isotope destruction tape
  */
{
  ELog::RegMethod RegA("htapeProcess","procDestruction");
  std::ifstream IX;
  IX.open("outt15");
  
  if (!IX.good())
    throw ColErr::FileError(15,"outt15","File no open");
  readHeader(2,IX,prodMap);
  return;
}

cellProduction*
htapeProcess::findCellProd(const int cellN)
  /*!
    Find or make a cellProd entry for cellN
    \param cellN :: Cell number to use
    \return pointer to cellProduciton
   */
{
  ELog::RegMethod RegA("htapeProcess","findCellProd");
  return findCellProd(cellProd,cellN);
}

cellProduction*
htapeProcess::findCellProd(CTYPE& prod,const int cellN) const
  /*!
    Find or make a cellProd entry for cellN
    \param prod :: producution cell
    \param cellN :: Cell number to use
    \return pointer to cellProduciton
   */
{
  ELog::RegMethod RegA("htapeProcess","findCellProd(Cell,CN)");

  CTYPE::iterator mc;
  mc=prod.find(cellN);
  if (mc!=prod.end())
    return &mc->second;

  // This may overwrite the first [e.g don't need the test??]
  std::pair<CTYPE::iterator,bool> Res=
    prod.emplace(cellN,cellProduction());
  return &Res.first->second;
}

void
htapeProcess::scale(const double Frac)
  /*!
    Scale the htape relative to the flux values
    \param Frac :: Fractional bias
  */
{
  ELog::RegMethod RegA("htapeProcess","scale");

  for(CTYPE::value_type& Prod : cellProd)
    {
      Prod.second.scale(Frac);
    }
  return;
}


void
htapeProcess::addCells(const long int newNPS,
		       const CTYPE& newProd)
  /*!
    Add the cells to the main celllist
    \param newNPS :: new point
    \param newProd :: production
   */
{
  ELog::RegMethod RegA("htapeProcess","addCells");

  // Need to loop over ALL Points in BOTH newProd and cellProd.
  // Both need to be used since either add nothing (rescale)
  // create, or sum

  // Items just to be scaled:
  const double oldFactor(nps/static_cast<double>(nps+newNPS));
  for(CTYPE::value_type& Item : cellProd)
    {
      CTYPE::const_iterator mc=newProd.find(Item.first); 
      if (mc==newProd.end())                  // simple scale
        Item.second.scale(oldFactor);
      else                                    // simple addition
        Item.second.addComponent(mc->second,nps,newNPS);          
    }

  // ADD items that exist in new set but not old
  const double newFactor(newNPS/static_cast<double>(nps+newNPS));
  for(const CTYPE::value_type& Item : newProd)
    {
      CTYPE::iterator mc=cellProd.find(Item.first); 
      if (mc==cellProd.end())                  // simple scale
        {
          std::pair<CTYPE::iterator,bool> Ptr=
	    cellProd.emplace(Item.first,Item.second);
	  Ptr.first->second.scale(newFactor);
        }
    }
  nps+=newNPS;
  return;
}
		       
void
htapeProcess::addSProdFile(const std::string& htapeFile,
                           const std::map<int,double>& cellVols)
  /*!
    Add the sprod file 
    \param htapeFile :: MCNPX htape output file
    \param cellVols :: Cell volumes file
   */ 
{
  ELog::RegMethod RegA("htape","addSProdFile");

  // Check file:
  if (!boost::filesystem::exists(htapeFile))
    throw ColErr::FileError(0,"htape File:",htapeFile);
  
  runProgs& RP=runProgs::Instance();

  std::map<int,double>::const_iterator mc=cellVols.begin();

  long int npts;
  size_t index(1);
  CTYPE fileProd;
  while(mc!=cellVols.end())
    {
      for(const std::string& outName : {"outt08","outt14","outt15"})
	{
	  if (boost::filesystem::exists(outName))
	    boost::filesystem::remove(outName);
	}
      
      std::map<int,double> cellCut;
      for(size_t i=0;i<50 && mc!=cellVols.end();i++)
	cellCut.insert(*mc++);
	  
      processHTape(cellCut);
      std::string Out08,Out14,Out15;
      if (index)
        {
          Out08="Out8_"+StrFunc::makeString(index)+".log";
          Out14="Out14_"+StrFunc::makeString(index)+".log";
          Out15="Out15_"+StrFunc::makeString(index)+".log";
        }
      if (RP.runHTape("int=int08 outt=outt08 histp="+htapeFile,Out08))
	ELog::EM<<"Failed on HTAPE int08 "<<ELog::endErr;
      if (RP.runHTape("int=int14 outt=outt14 histp="+htapeFile,Out14))
        ELog::EM<<"Failed on HTAPE int14 "<<ELog::endErr;
      if (RP.runHTape("int=int15 outt=outt15 histp="+htapeFile,Out15))
        ELog::EM<<"Failed on HTAPE int15 "<<ELog::endErr;

      npts=procProduction(fileProd);
      //      procGas(fileProd);
      //      procDestruction(fileProd);
      index++;
    }
  ELog::EM<<"Npts == "<<npts<<ELog::endDiag;
  addCells(npts,fileProd);
      
  return;
}

void
htapeProcess::writeSprods(const std::string& FName,
                          const int cellN,
			  const double Vol) const
  /*!
    Write out the production
    \param FName :: Filename
    \param cellN :: cell number
    \param Vol :: Volume
   */
{
  ELog::RegMethod RegA("htapeProcess","writeSprods");

  boost::format FMT("%s%|69t|V= %9.3e");
  boost::format CellFMT("%s%d%|70t|%9.3e %9.3e");
  
  if (FName.empty()) return;
  std::ofstream OX(FName.c_str());

  CTYPE::const_iterator mc=cellProd.find(cellN);
  
  if (mc==cellProd.end())
    {
      ELog::EM<<"Warning no cell:"<<cellN<<ELog::endWarn;
    }
  else
    {
      const DError::doubleErr Total=mc->second.getTotal();
      OX<<(FMT % "distribution of residual nuclei" % Vol)
	<<std::endl;
      OX<<(CellFMT % "in cells " % cellN % Total.getVal() % Total.getErr())
	<<std::endl;
    }
      
  mc->second.writeSprods(OX);
  OX.close();
  return;
}



void
htapeProcess::write(std::ostream& OX) const
  /*!
    Write out some stuff : mainly debug
    \parma OX :: Output stream						
   */
{
  OX<<"Production Cells::"<<std::endl;
  for(const CTYPE::value_type& Item : cellProd)
    {
      OX<<"Cell Number: "<<Item.first<<std::endl;
      OX<<Item.second<<std::endl;
    }
  return;
}

