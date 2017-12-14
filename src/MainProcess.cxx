/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/MainProcess.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <string>
#include <iterator>
#include <memory>

#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "support.h"
#include "masterWrite.h"
#include "MainProcess.h"

namespace mainSystem
{

void
activateLogging(ELog::RegMethod& RControl)
  /*!
    Sets up the Main flags for logging
    \param RControl :: Any RegMethod 
  */
{  // Set up output information:
  ELog::EM.setNBasePtr(RControl.getBasePtr());
  ELog::EM.setTypeFlag(0);
  //  ELog::EM.setActive(255 ^ ELog::debug); // No debug
  ELog::EM.setActive(255);
  ELog::EM.setDebug(ELog::debug);
  ELog::EM.setAction(ELog::error);       // Exit on Error
  ELog::EM.setColour();

  return;
}

}  // NAMESPACE mainSystem

