/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   work/cinderHistory.cxx
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
#include <map>
#include <algorithm>
#include <functional>
#include <iterator>

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
#include "doubleErr.h"
#include "mathSupport.h"
#include "cinderHistory.h"

cinderHistory::cinderHistory() 
  /*!
    Constructor
  */
{}

cinderHistory::cinderHistory(const cinderHistory& A) : 
  current(A.current),time(A.time),timeUnit(A.timeUnit)
  /*!
    Copy constructor
    \param A :: cinderHistory to copy
  */
{}

cinderHistory&
cinderHistory::operator=(const cinderHistory& A)
  /*!
    Assignment operator
    \param A :: cinderHistory to copy
    \return *this
  */
{
  if (this!=&A)
    {
      current=A.current;
      time=A.time;
      timeUnit=A.timeUnit;
    }
  return *this;
}

cinderHistory::~cinderHistory()
  /*!
    Destructor
   */
{}


char
cinderHistory::convertTimeUnit(const std::string& TType)
  /*!
    Convert string type of time to a time unit letter
    \param TType :: name of time
    \return time unit
   */
{
  ELog::RegMethod RegA("cinderHistory","convertTime");
  
  if (TType=="s" || TType=="sec")
    return 's';
  if (TType=="m" || TType=="min")
    return 'm';
  if (TType=="d" || TType=="day")
    return 'd';
  if (TType=="h" || TType=="hour")
    return 'h';
  if (TType=="y" || TType=="year")
    return 'y';

  throw ColErr::InContainerError<std::string>(TType,"TType");
  
}

double
cinderHistory::convertTime(const std::string& TType)
  /*!
    Convert string type of time to number of seconds
    \param TType :: name of time
    \return numer of second
   */
{
  ELog::RegMethod RegA("cinderHistory","convertTime");
  
  if (TType=="s" || TType=="sec")
    return 1.0;
  if (TType=="m" || TType=="min")
    return 60.0;
  if (TType=="d" || TType=="day")
    return 24*3600.0;
  if (TType=="h" || TType=="hour")
    return 3600.0;
  if (TType=="y" || TType=="year")
    return 24*3600*365.25;

  throw ColErr::InContainerError<std::string>(TType,"TType");
}

void
cinderHistory::addLine(const std::string& AItem,
		       std::string Line)
  /*!
    Add a history line
    \param Line :: to process
  */
{
  ELog::RegMethod RegA("cinderHistory","addLine");


  size_t N;
  double C;
  if (StrFunc::convert(AItem,N) &&
      StrFunc::section(Line,C))
    {
      char tUnit;
      double L;
      std::string timeType;
      while(StrFunc::section(Line,L),
	    StrFunc::section(Line,timeType))
	{
	  current.push_back(C);
	  tUnit=convertTimeUnit(timeType);
	  time.push_back(L);
	  timeUnit.push_back(tUnit);
	}
    }
  
  return;
}



void
cinderHistory::write(std::ostream& OX) const
  /*!
    Writes out information about the DataMap
    \param OX :: output file 
  */
{
  boost::format FMT("%d   %8.2e");
  boost::format timeFMT("    %8.2e%|16t|\'%c\'");

  if (!current.empty())
    {
      size_t index(0);
      size_t begIndex(0);
      double C;
      
      while(index<current.size())
	{
	  C=current[index];
	  while(index<current.size() && std::abs(C-current[index])<1e-6)
	    {
	      index++;
	    }
	  // write
	  OX<<(FMT % (index-begIndex) % C)<<std::endl;
	  for(size_t j=begIndex;j<index;j++)
	    OX<<(timeFMT % time[j] % timeUnit[j])<<std::endl;
	  begIndex=index;

	}
    }
  return;
}

