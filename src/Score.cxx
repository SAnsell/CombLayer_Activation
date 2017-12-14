/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/Score.cxx
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
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "neutron.h"
#include "support.h"
#include "Score.h"

std::ostream&
operator<<(std::ostream& OX,const Score& A)
  /*!
    Write output
   */
{
  A.write(OX);
  return OX;
}

Score::Score(const size_t M) :
  maxCol(M),NCol(0),collisionWt(maxCol),
  collisionN(maxCol),
  Plus(0.0),pCnt(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{
  std::fill(collisionWt.begin(),collisionWt.end(),0.0);
  std::fill(collisionN.begin(),collisionN.end(),0UL);
}

Score::Score(const Score& A) : 
  maxCol(A.maxCol),NCol(A.NCol),collisionWt(A.collisionWt),
  collisionN(A.collisionN),Plus(A.Plus),pCnt(A.pCnt)
  /*!
    Copy constructor
    \param A :: Score to copy
  */
{}

Score&
Score::operator=(const Score& A)
  /*!
    Assignment operator
    \param A :: Score to copy
    \return *this
  */
{
  if (this!=&A)
    {
      NCol=A.NCol;
      collisionWt=A.collisionWt;
      collisionN=A.collisionN;
      Plus=A.Plus;
      pCnt=A.pCnt;
    }
  return *this;
}




Score::~Score() 
  /*!
    Destructor
  */
{}

void
Score::addNeutron(const MonteCarlo::neutron& N)
{
  Plus+=N.weight;
  if (N.nCollision<maxCol)
    {
      collisionWt[N.nCollision]+=N.weight;
      collisionN[N.nCollision]++;
    }
  pCnt++;

  return;
}

void
Score::write(std::ostream& OX) const
  /*!
   */
{
  OX<<"Plus == "<<std::scientific<<Plus/pCnt<<" "<<Plus<<" "<<pCnt<<"\n";
  for(size_t i=0;i<maxCol;i++)
    OX<<"Col "<<i<<" "<<collisionWt[i]<<" "<<collisionN[i]<<"\n";

  return;
}
  



  


