/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   work/runProgs.cxx
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
#include <wait.h>
#include <unistd.h>
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
#include "cinderOption.h"
#include "cinderHistory.h"
#include "runProgs.h"

runProgs::runProgs() :
  cinderCMD("/home/ansell/cinder-1.05/src/cinder"),
  htapeCMD("/home/ansell/BinV270/bin/htape3x")
  /*!
    Constructor
  */
{}



runProgs::~runProgs()
  /*!
    Destructor
  */
{}



runProgs&
runProgs::Instance()
  /*!
    Effective this object			
    \return runProgs object
  */
{
  static runProgs A;
  return A;
}

void
runProgs::setCinderEXE(const std::string& prog)
  /*!
    Set the cinder exectuable
    \param prog :: program
   */
{
  cinderCMD=prog;
  return;
}

void
runProgs::setTabcodeEXE(const std::string& prog)
  /*!
    Set the tabcode exectuable
    \param prog :: program
   */
{
  tabcodeCMD=prog;
  return;
}

void
runProgs::setHTapeEXE(const std::string& prog)
  /*!
    Set the htape exectuable
    \param prog :: program
   */
{
  htapeCMD=prog;
  return;
}

int
runProgs::runCode(const std::string& CMD)
  /*!
    Run a piece of code
    \param CMD :: Command to run
    \return status code
   */
{
  ELog::RegMethod RegA("runProgs","runCode");
  return system(CMD.c_str());
}

int
runProgs::runHTape(const std::string& ARGS,
                   const std::string& outFile)
  /*!
    Run a piece of code in htape
    \param ARGS :: Argunments 
    \param outFile :: file to write output to [if not empty]
    \return return code
   */
{
  ELog::RegMethod RegA("runProgs","runHTape");

  if (outFile.empty())
    return runCode(htapeCMD+" "+ARGS);

  return runCode(htapeCMD+" "+ARGS+" > "+outFile);
}

int
runProgs::runCinder(const std::string& ARGS,
                    const std::string& outFile)
  /*!
    Run a piece of code in htape
    \param ARGS :: Argunments 
    \param outFile :: file to write output to [if not empty]
    \return return code
   */
{
  ELog::RegMethod RegA("runProgs","runCinder");

  if (outFile.empty())
    return runCode(cinderCMD+" "+ARGS);

  return runCode(cinderCMD+" "+ARGS+" > "+outFile);
}

int
runProgs::runTabCode(const std::string& ARGS,
                     const std::string& outFile)
  /*!
    Run a piece of code in htape
    \param ARGS :: Argunments 
    \param outFile :: file to write output to [if not empty]
    \return return code
   */
{
  ELog::RegMethod RegA("runProgs","runTabCode");

  if (outFile.empty())
    return runCode(tabcodeCMD+" "+ARGS);

  return runCode(tabcodeCMD+" "+ARGS+" > "+outFile);
}





/*		  /
  pid_t pid = fork(); // create child proces

  char** ARGV=
  switch (pid)
    {
    case -1:
      ELog::EM<<"ERROR Fork failed"<<ELog::endErr;
      return;
    case 0: 
      ELog::EM<<"Calling CHILD PROCESS:"<<CMD<<ELog::endDiag;
      execv(CMD.c_str(), ); // Execute the program
      ELog::EM<<"Failed to start process "<<ELog::endErr;
      return;
    default:
      ELog::EM << "Process created with pid " << pid << ELog::endDiag;;
      int status;
      
      while (!WIFEXITED(status))
	{
	  waitpid(pid, status, 0); // Wait for process
	}
      
      ELog::EM << "Process exited  " << WEXITSTATUS(status) << ELog::endDiag;
    }
		  
*/
