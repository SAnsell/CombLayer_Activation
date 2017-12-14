/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   System/glob/Glob.cxx
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
#include <fnmatch.h>
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
#include "Glob.h"

namespace glob
{
  
Glob::Glob(const std::string& PMatch) :
  dirPtr(0),dirEntry(0)
   /*!
     Constructor
     \param PMatch :: Pattern to match 
   */						
{
  ELog::RegMethod RegA("Glob","Constructor");
  
  std::pair<std::string, std::string>
    dirMask = StrFunc::pairSplit(PMatch,"/",1);

  if (dirMask.first.empty())
    dirMask.first="./";

  dirPtr = opendir(dirMask.first.c_str());
  if (!dirPtr)
    ELog::EM<<"No pointer "<<ELog::endErr;
  pattern = dirMask.second;  
  if (!next())
    throw ColErr::FileError
      (0,PMatch,"Failed to find file pattern");
}

Glob::~Glob()
  /*!
    Deletion operator
  */
{
  if (dirPtr)
    {
      closedir(dirPtr);
      dirPtr=0;
    }
}

bool
Glob::next()
  /*!
    Advance to next item in the string
    \return true if item exists
   */
{
  if (dirPtr)
    {
      while ((dirEntry = readdir(dirPtr)) != 0)
	{
	  if (!fnmatch(pattern.c_str(),dirEntry->d_name,
		       FNM_CASEFOLD | FNM_NOESCAPE | FNM_PERIOD))
	    return true;
	}
    }
  return false;
}

} // namespace glob
