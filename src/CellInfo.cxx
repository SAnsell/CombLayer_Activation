/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   work/CellInfo.cxx
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
#include "CellInfo.h"

CellInfo::CellInfo(const std::string& VName) :
  VolName(VName)
  /*!
    Constructor
    \param VName :: Volume name
  */
{}

CellInfo::CellInfo(const CellInfo& A) :
  VolName(A.VolName),cellN(A.cellN),
  cellV(A.cellV)
  /*!
    Copy constructor
    \param A :: CellInfo to copy
  */
{}

CellInfo&
CellInfo::operator=(const CellInfo& A)
  /*!
    Assignment operator
    \param A :: CellInfo to copy
    \return *this
  */
{
  if (this!=&A)
    {
      VolName=A.VolName;
      cellN=A.cellN;
      cellV=A.cellV;
    }
  return *this;
}

CellInfo::~CellInfo()
 /*!
  Destructor
  */
{}

void
CellInfo::addCell(const int CN,const double CV)
  /*!
    Add a cellnumber and volume
    \param CN :: Cell number
    \param CV :: Cell volume
  */
{
  cellN.push_back(CN);
  cellV.push_back(CV);
  return;
}
