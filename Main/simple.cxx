/********************************************************************* 
  CombLayer : MCNP(X) SSW/SSR Reader
 
 * File:   Main/simple.cxx
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
#include "inputParam.h"
#include "MainProcess.h"
#include "fortRecFile.h"


MTRand RNG(12345UL);

namespace ELog 
{
  ELog::OutputLog<EReport> EM;
}

int 
main(int argc,char* argv[])
{
  int exitFlag(0);                // Value on exit
  ELog::RegMethod RControl("","main");
  mainSystem::activateLogging(RControl);

  std::vector<std::string> Names;  
  std::map<std::string,std::string> Values;  
  std::map<std::string,double> IterVal;           // Variable to iterate 
  std::string Fname;
  // PROCESS INPUT:
  InputControl::mainVector(argc,argv,Names);
  ELog::EM<<"Size of Names == "<<Names.size()<<ELog::endDiag;
  try
    {
      if (Names.empty() || Names[0].empty()) return -1;
      RawFile::fortRecFile DB(Names[0]);         // Data

      std::string from=DB.getString(8);

      std::string kod=DB.getString(8);
      std::string ver=DB.getString(5);
      std::string loddat=DB.getString(8);
      std::string idtm=DB.getString(19);  // current date and time
      std::string probid=DB.getString(19);  // problem identification string
      std::string aid=DB.getString(80);  // problem identification string
      int knod=DB.getItem<int>();  // dump number [2]
      //      unsigned int knod=DB.getItem<unsigned int>();  // dump number [2]
      
      ELog::EM<<"Ha[8] ==="<<kod<<"==="<<ELog::endDiag;
      ELog::EM<<"Hb[5] ==="<<ver<<"==="<<ELog::endDiag;
      ELog::EM<<"Hc[8] ==="<<loddat<<"==="<<ELog::endDiag;
      ELog::EM<<"Hd[19] ==="<<idtm<<"==="<<ELog::endDiag;
      ELog::EM<<"He[19] ==="<<probid<<"==="<<ELog::endDiag;
      ELog::EM<<"He[80] ==="<<aid<<"==="<<ELog::endDiag;
      ELog::EM<<"He[x] ==="<<knod<<"==="<<ELog::endDiag;
      ELog::EM<<"--------------------------------------"<<ELog::endDiag;
      //      std::string np=DB.getString(8);  // dump number [2]
      long int np=DB.getItem<long int>();  // dump number [2]
      long int nrsw=DB.getItem<long int>();  // dump number [2]
      int ncp=DB.getItem<int>();  // dump number [2]
      int njss=DB.getItem<int>();  // dump number [2]
      long int nqsw=DB.getItem<long int>();  // dump number [2]

      ELog::EM<<"A ==="<<np<<"==="<<ELog::endDiag;
      ELog::EM<<"A ==="<<nrsw<<"==="<<ELog::endDiag;
      ELog::EM<<"A ==="<<ncp<<"==="<<ELog::endDiag;
      ELog::EM<<"A ==="<<njss<<"==="<<ELog::endDiag;
      ELog::EM<<"A ==="<<nqsw<<"==="<<ELog::endDiag;
      ELog::EM<<"--------------------------------------"<<ELog::endDiag;
      int nilw=DB.getItem<int>();  
      int mipt=DB.getItem<int>();  // 37
      int kq=DB.getItem<int>();  // 37
      ELog::EM<<"B ==="<<nilw<<"==="<<ELog::endDiag;
      ELog::EM<<"B ==="<<mipt<<"==="<<ELog::endDiag;
      ELog::EM<<"B ==="<<kq<<"==="<<ELog::endDiag;
      for(size_t i=0;i<17;i++)
	{
	  int junk=DB.getItem<int>();  // 0
	  if (junk!=0)
	    ELog::EM<<"FAIL ON "<<i<<" "<<junk<<ELog::endCrit;
	}
      ELog::EM<<"--------------------------------------"<<ELog::endDiag;
      int jss=DB.getItem<int>();
      int kst=DB.getItem<int>();
      int n=DB.getItem<int>();
      double scf=DB.getItem<double>();

      ELog::EM<<"C ==="<<jss<<"==="<<ELog::endDiag;
      ELog::EM<<"C ==="<<kst<<"==="<<ELog::endDiag;
      ELog::EM<<"C ==="<<n<<"==="<<ELog::endDiag;
      ELog::EM<<"C ==="<<scf<<"==="<<ELog::endDiag;
      ELog::EM<<"--------------------------------------"<<ELog::endDiag;
      ELog::EM<<" PARTICES STUFF crossing per surface "<<ELog::endDiag;
      ELog::EM<<"A  == "<<DB.getItem<double>()<<ELog::endDiag;
      for(size_t i=0;i<37*4+2;i++)
	ELog::EM<<"P["<<i<<"] == "<<DB.getItem<int>()<<ELog::endDiag;
      for(int j=0;j<100;j++)
	{
	  ELog::EM<<"--------------------------------------"<<ELog::endDiag;      
	  double Res[11];
	  for(size_t i=0;i<11;i++)
	    {
	      Res[i]=DB.getItem<double>();
	      ELog::EM<<"Res[<<"<<i<<"] "<<Res[i]<<ELog::endDiag;
	    }
	}
    }
  catch (ColErr::ExitAbort& EA)
    {
      if (!EA.pathFlag())
	ELog::EM<<"Exiting from "<<EA.what()<<ELog::endCrit;
      exitFlag= -2;
    }
  catch (ColErr::ExBase& A)
    {
      ELog::EM<<"\nEXCEPTION FAILURE :: "
	      <<A.what()<<ELog::endCrit;
      exitFlag = -1;
    }

  return exitFlag;

}
