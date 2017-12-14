/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   work/cellProduction.cxx
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
#include "cellProduction.h"


std::ostream&
operator<<(std::ostream& OX,const cellProduction& CP)
  /*!
    Write out to standard stream
    \param OX :: Output stream
    \param cP :: htapeProcess to write
    \return output stream 
  */
{
  CP.write(OX);
  return OX;
}

cellProduction::cellProduction() 
  /*!
    Constructor
  */
{}

cellProduction::cellProduction(const cellProduction& A) : 
  elmProd(A.elmProd),
  elmLoss(A.elmLoss),elmTotal(A.elmTotal)
  /*!
    Copy constructor
    \param A :: cellProduction to copy
  */
{}

cellProduction&
cellProduction::operator=(const cellProduction& A)
  /*!
    Assignment operator
    \param A :: cellProduction to copy
    \return *this
  */
{
  if (this!=&A)
    {
      elmProd=A.elmProd;
      elmLoss=A.elmLoss;
      elmTotal=A.elmTotal;
    }
  return *this;
}

cellProduction::~cellProduction()
  /*!
    Destructor
  */
{}



void
cellProduction::cellIndexProd(const size_t index,
                              const int z,const int n,
                              const int meta,const double& frac,
                              const double& fracErr)
  /*!
    Add a production item 
    \param index :: index number [0/1/2]
    \param z :: Atomic number
    \param n :: neutron number
    \parma meta :: gamma active level
    \param frac :: fraction changed
    \param fracErr :: error on value
  */
{
  ELog::RegMethod RegA("cellProduction","cellIndexProd");

  switch(index)
    {
    case 0:
    case 1:
      addProd(z,n,meta,frac,fracErr);
      addTotal(z,n,meta,frac,fracErr);
      break;
    case 2:
      addDestruct(z,n,frac,fracErr);
      addTotal(z,n,0,-frac,fracErr);
      break;
    default:
      throw ColErr::IndexError<size_t>(index,3,"Prod index");
    }
  
  return;
}

void
cellProduction::addTotal(const int z,const int n,const int meta,
                         const double& frac,const double& fracErr)
  /*!
    Add to total item +/-
    \param z :: Atomic number
    \param n :: neutron number
    \param meta :: Meta value
    \param f :: fraction changed
    \param fracErr :: error on value
   */
{
  ELog::RegMethod RegA("cellProduction","addTotal");

  if (z<1 || n<0)
    ELog::EM<<"Zaid unknown "<<z<<" "<<n<<" "<<meta<<ELog::endErr;

  const int zaid=z*10000+(n+z)*10+meta;
  const DError::doubleErr V(frac,fracErr*std::abs(frac));
  std::map<int,DError::doubleErr>::iterator mc=
    elmTotal.find(zaid);
  if (mc!=elmTotal.end())
    {
      mc->second+=V;
    }
  else
    {
      elmTotal.emplace(zaid,V);
    }

  return;
}


void
cellProduction::addProd(const int z,const int n,
                        const int meta,const double& frac,
                        const double& fracErr)
  /*!
    Add a production item
    \param z :: Atomic number
    \param n :: neutron number
    \param meta :: metastable
    \param frac :: fraction changed
    \param fracErr :: error on value
  */
{
  ELog::RegMethod RegA("cellProduction","addProd");

  if (z<1 || n<0)
    ELog::EM<<"Zaid unknown "<<z<<" "<<n<<ELog::endErr;

  const int zaid=z*10000+(n+z)*10+meta;
  const DError::doubleErr V(frac,fracErr*frac);
  std::map<int,DError::doubleErr>::iterator mc=
    elmProd.find(zaid);
  if (mc!=elmProd.end())
    mc->second+=V;
  else
    elmProd.emplace(zaid,V);

  return;
}

void
cellProduction::addDestruct(const int z,const int n,
			    const double& frac,const double& fracErr)
  /*!
    Add a destroyed items
    \param z :: Atomic number
    \param n :: neutron number
    \param f :: fraction changed
    \param fracErr :: error on value
  */
{
  ELog::RegMethod RegA("cellProduction","addDestruct");

  
  if (z<1 || n<0)
    ELog::EM<<"Zaid unknown "<<z<<" "<<n<<ELog::endErr;

  const int zaid=z*10000+(n+z)*10;
  const DError::doubleErr V(frac,fracErr*frac);
  std::map<int,DError::doubleErr>::iterator mc=
    elmLoss.find(zaid);
  if (mc!=elmLoss.end())
    {
      mc->second+=V;
    }
  else
    {
      elmLoss.emplace(zaid,V);
    }
  return;
}

DError::doubleErr
cellProduction::getTotal() const
  /*!
    Calculate the total
    \return total of all destroyed / created
  */
{
  ELog::RegMethod RegA("cellProduction","getTotal");

  DError::doubleErr Out;
  for(const CTYPE::value_type& CV : elmTotal)
    Out+=CV.second;

  return Out;
}


cellProduction&
cellProduction::scale(const double V)
  /*!
    Scale the production in all the cells
    \param V :: Value to scale by
   */
{
  ELog::RegMethod RegA("cellProduction","writeSprods");

  for(CTYPE::value_type& CV : elmProd)
    CV.second*=V;
  for(CTYPE::value_type& CV : elmLoss)
    CV.second*=V;
  for(CTYPE::value_type& CV : elmTotal)
    CV.second*=V;
  
  return *this;
}

void
cellProduction::scaleSum(CTYPE& AUnit,const CTYPE& BUnit,
                         const long int oldNPS,
                         const long int newNPS)
  /*!
    Adds a scaled component set
    \param AUnit :: components to be added to 
    \param BUnit :: extra set to add
  */
{
  ELog::RegMethod RegA("cellProduction","scaleSum");

  // Items just to be scaled:
  for(CTYPE::value_type& CV : AUnit)
    {
      CTYPE::const_iterator mc=BUnit.find(CV.first);
      if (mc==BUnit.end())
        CV.second*=static_cast<double>(oldNPS);
      else
        {
          CV.second*=static_cast<double>(oldNPS);
          CV.second+=mc->second*static_cast<double>(newNPS);
        }
    }

  // Items that need to be included:
  for(const CTYPE::value_type& CV : BUnit)
    {
      CTYPE::iterator mc=AUnit.find(CV.first); 
      if (mc==AUnit.end())                  // simple insert
        AUnit.emplace(CV.first,CV.second*static_cast<double>(newNPS));
    }

  
  // Global re-scaling
  const double NSum(static_cast<double>(newNPS+oldNPS));
  for(CTYPE::value_type& CV : AUnit)
    CV.second/=NSum;

  return;
}



cellProduction&
cellProduction::addComponent(const cellProduction& A,
			     const long int oldNPS,
                             const long int newNPS)
  /*!
    Scale the production in all the cells
    \param A :: cellProuctiono to add
    \param oldNPS :: Value to scale by old
    \param newNPS :: Value to scale by for newItem
   */
{
  ELog::RegMethod RegA("cellProduction","addComponent");

  scaleSum(elmProd,A.elmProd,oldNPS,newNPS);
  scaleSum(elmLoss,A.elmLoss,oldNPS,newNPS);
  scaleSum(elmTotal,A.elmTotal,oldNPS,newNPS);
  
  
  return *this;
}

void
cellProduction::writeSprods(std::ostream& OX) const
  /*!
    Write out the Splodprod file
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("cellProduction","writeSprods");

  boost::format FMT("  %6i     %10.4e");
  for(const CTYPE::value_type& CV : elmTotal)
    {
      if (CV.second>1e-20)
	OX<<(FMT % CV.first % CV.second.getVal())<<std::endl;
    }
  return;
}

void
cellProduction::write(std::ostream& OX) const 
  /*!
    Write out debug info
    \param OX :: Output stream
   */
{
  OX<<"  production:\n";
  for(const CTYPE::value_type& PI : elmProd)
    OX<<"    "<<PI.first<<" "<<PI.second<<std::endl;

  OX<<"  destruction:\n";
  for(const CTYPE::value_type& PI : elmLoss)
    OX<<"    "<<PI.first<<" "<<PI.second<<std::endl;
    
  return;
}

