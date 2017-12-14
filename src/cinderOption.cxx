/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   work/cinderOption.cxx
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
#include "cinderOption.h"

cinderOption::cinderOption() : 
  tst(1e-5),signif(1e-16),epsm(0.0),epsn(0.0),
  expomax(0.0),kchn(0),klib(0),
  nfe(3),nlintl(2),nosame(0),russ(1)
  /*!
    Constructor
  */
{}

cinderOption::cinderOption(const cinderOption& A) : 
  tst(A.tst),signif(A.signif),epsm(A.epsm),epsn(A.epsn),
  expomax(A.expomax),kchn(A.kchn),klib(A.klib),
  nfe(A.nfe),nlintl(A.nlintl),nosame(A.nosame),
  russ(A.russ)
  /*!
    Copy constructor
    \param A :: cinderOption to copy
  */
{}

cinderOption&
cinderOption::operator=(const cinderOption& A)
  /*!
    Assignment operator
    \param A :: cinderOption to copy
    \return *this
  */
{
  if (this!=&A)
    {
      tst=A.tst;
      signif=A.signif;
      epsm=A.epsm;
      epsn=A.epsn;
      expomax=A.expomax;
      kchn=A.kchn;
      klib=A.klib;
      nfe=A.nfe;
      nlintl=A.nlintl;
      nosame=A.nosame;
      russ=A.russ;
    }
  return *this;
}

cinderOption::~cinderOption()
  /*!
    Destructor
   */
{}

void
cinderOption::addString(const std::string& tag,
			std::string line) 
  /*!
    process a line
    \param tag :: identifier name 
    \param line :: line 
  */
{
  ELog::RegMethod RegA("cinderOption","addString");

  double V;
  if (StrFunc::section(line,V))
    {
      if (tag=="tst")
        tst=V;
      else if (tag=="signif")
        signif=V;
      else if (tag=="kchn")
        kchn=0;
      else if (tag=="klib")
        kchn=static_cast<int>(V);
      else if (tag=="nfe")
        nfe=static_cast<int>(V);
      else if (tag=="nlintl")
        nlintl=static_cast<int>(V);
      else if (tag=="nosame")
        nosame=static_cast<int>(V);
      else if (tag=="russ")
        russ=static_cast<int>(V);
      else
	{
	  ELog::EM<<"Failed to understand :"<<tag<<" : "<<V<<ELog::endErr;
	}
    }
      
  return;
}

void
cinderOption::write(std::ostream& OX,
		    const int matN,
		    const double totalVolume,
		    const double norm) const

  /*!
    Writes out information about the DataMap
    \param OX :: output file 
  */
{
  ELog::RegMethod RegA("cinderOption","write");
  
  boost::format longFMT("%12.5e");
  boost::format shortFMT("%10.3e");
  boost::format integerFMT("%d");
  boost::format matFMT("%3d");  

  
  OX<< (longFMT % totalVolume)<<",";
  OX<< (longFMT % norm)<<",";
  OX<< (shortFMT % tst)<<",";
  OX<< (shortFMT % signif)<<",";
  OX<< ((epsm<1e-31) ? "," : (shortFMT % epsm).str()+",");
  OX<< ((epsn<1e-31) ? "," : (shortFMT % epsn).str()+",");
  OX<< ((expomax<1e-31) ? "," : (shortFMT % expomax ).str()+",");
  OX<< "0,";
  OX<< (integerFMT % kchn)<<",";
  OX<< (integerFMT % klib)<<",";
  OX<< (integerFMT % nfe)<<",";
  OX<<"-1,";
  OX<< (integerFMT % nlintl)<<",";
  OX<< (integerFMT % nosame)<<",";
  OX<< (integerFMT % russ)
    <<"/"<<std::endl;
      
  OX<<" test targets"<<std::endl;
  OX<<"   Beamline"<<std::endl;
  OX<<"Input Deck for CINDER'90"<<std::endl;
  OX<<"tally  4"<<std::endl;;
  OX<<"mat"<<(matFMT % matN)<<std::endl;
  return;
}

