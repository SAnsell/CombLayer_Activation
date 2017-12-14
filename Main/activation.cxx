/********************************************************************* 
  CombLayer : MCNP(X) SSW/SSR Reader
 
 * File:   Main/activation.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <memory>
#include <array>
#include <endian.h>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h" 
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "InputControl.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "inputParam.h"
#include "MainProcess.h"
#include "neutron.h"
#include "BUnit.h"
#include "Boundary.h"
#include "WorkData.h"
#include "cinderOption.h"
#include "cinderHistory.h"
#include "htapeProcess.h"
#include "tallyProcess.h"
#include "materialProcess.h"
#include "Control.h"

MTRand RNG(12345UL);

namespace ELog 
{
  ELog::OutputLog<EReport> EM;
}

int 
main(int argc,char* argv[])
{
  ELog::RegMethod RControl("","main");
  mainSystem::activateLogging(RControl);

  std::vector<std::string> Names;  
  std::map<std::string,std::string> Values;  
  std::map<std::string,double> IterVal;           // Variable to iterate 

  // PROCESS INPUT:
  InputControl::mainVector(argc,argv,Names);
  const std::string IName=InputControl::getFileName(Names);
  
  Control mainProcess;
  
  mainProcess.readControlFile(IName);
  mainProcess.runHTape();
  mainProcess.readFluxes();
  mainProcess.readMaterials();

  mainProcess.writeCinderInput();
  
  return 0;

}
