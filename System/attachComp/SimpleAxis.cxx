/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/SimpleAxis.cxx
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
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "SimpleAxis.h"

namespace attachSystem
{

SimpleAxis::SimpleAxis(const std::string& KN) :
  keyName(KN),X(Geometry::Vec3D(1,0,0)),Y(Geometry::Vec3D(0,1,0)),
  Z(Geometry::Vec3D(0,0,1))
 /*!
    Constructor 
    \param KN :: KeyName
    \param NL :: Number of links
  */
{}

SimpleAxis::SimpleAxis(const std::string& KN,
		       const Geometry::Vec3D& O) :
  keyName(KN),X(Geometry::Vec3D(1,0,0)),Y(Geometry::Vec3D(0,1,0)),
  Z(Geometry::Vec3D(0,0,1)),Origin(O)
  /*!
    Constructor 
    \param KN :: KeyName
    \param NL :: Number of links
    \param O :: Origin Point
  */
{}

SimpleAxis::SimpleAxis(const std::string& KN,
		     const Geometry::Vec3D& O,
		     const Geometry::Vec3D& xV,
		     const Geometry::Vec3D& yV,
		     const Geometry::Vec3D& zV) :
  keyName(KN),X(xV.unit()),Y(yV.unit()),Z(zV.unit()),
  Origin(O)
  /*!
    Constructor 
    \param KN :: KeyName
    \param O :: Origin Point
  */
{}

SimpleAxis::SimpleAxis(const SimpleAxis& A) : 
  keyName(A.keyName),X(A.X),Y(A.Y),Z(A.Z),
  Origin(A.Origin)
  /*!
    Copy constructor
    \param A :: SimpleAxis to copy
  */
{}

SimpleAxis&
SimpleAxis::operator=(const SimpleAxis& A)
  /*!
    Assignment operator
    \param A :: SimpleAxis to copy
    \return *this
  */
{
  if (this!=&A)
    {

      X=A.X;
      Y=A.Y;
      Z=A.Z;
      Origin=A.Origin;
    }
  return *this;
}

void
SimpleAxis::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("SimpleAxis","createUnitVector");
  
  Z=Geometry::Vec3D(0,0,1);          // Gravity axis [up]
  Y=Geometry::Vec3D(0,1,0);
  X=Geometry::Vec3D(1,0,0);
  return;
}

void
SimpleAxis::createUnitVector(const SimpleAxis& FC)
  /*!
    Create the unit vectors
    \param FC :: Relative to another FC
  */
{
  ELog::RegMethod RegA("SimpleAxis","createUnitVector(SimpleAxis)");

  Z=FC.Z;
  Y=FC.Y;
  X=FC.X;
  Origin=FC.Origin;

  return;
}

void
SimpleAxis::createUnitVector(const SimpleAxis& FC,
			    const Geometry::Vec3D& POrigin)
  /*!
    Create the unit vectors
    \param FC :: Relative to another FC
    \param POrigin :: New Origin
  */
{
  ELog::RegMethod RegA("SimpleAxis","createUnitVector(SimpleAxis,Vec3D)");

  Z=FC.Z;
  Y=FC.Y;
  X=FC.X;
  Origin=POrigin;
  return;
}

  
void
SimpleAxis::createUnitVector(const Geometry::Vec3D& OG,
			    const Geometry::Vec3D& BeamAxis,
			    const Geometry::Vec3D& ZAxis)
  /*!
    Create the unit vectors [using beam directions]
    \param OG :: Origin
    \param BeamAxis :: Beamline axis line
    \param ZAxis :: Direction for Z
  */
{
  ELog::RegMethod RegA("SimpleAxis","createUnitVector(Vec3D,Vec3D,Vec3D))");

  //Geometry::Vec3D(-1,0,0);          // Gravity axis [up]
  Z=ZAxis;
  Y=BeamAxis.unit();
  X=(Y*Z);                            // horrizontal axis [across]

  Origin=OG;
  return;
}

void
SimpleAxis::applyShift(const double xStep,
		      const double yStep,
		      const double zStep)
  /*!
    Create the unit vectors
    \param xStep :: XStep [of X vector]
    \param yStep :: YStep [of Y vector]
    \param zStep :: ZStep [of Z vector]
  */
{
  Origin+=X*xStep+Y*yStep+Z*zStep;
  return;
}


void
SimpleAxis::applyAngleRotate(const double xAngle,
			     const double yAngle,
			     const double zAngle)
 /*!
    Create the unit vectors
    \param xAngle :: x Rotation [first]
    \param yAngle :: Y Rotation [second]
    \param zAngle :: Z Rotation [first]
  */
{
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,Z);
  const Geometry::Quaternion Qy=
    Geometry::Quaternion::calcQRotDeg(yAngle,Y);
  const Geometry::Quaternion Qx=
    Geometry::Quaternion::calcQRotDeg(xAngle,X);
  Qz.rotate(Y);
  Qz.rotate(X);
  Qy.rotate(Y);
  Qy.rotate(X);
  Qy.rotate(Z);
  
  Qx.rotate(Y);
  Qx.rotate(X);
  Qx.rotate(Z);
  return;
}
void
SimpleAxis::applyAngleRotate(const double xyAngle,
			    const double zAngle)
 /*!
    Create the unit vectors
    \param xyAngle :: XY Rotation [second]
    \param zAngle :: Z Rotation [first]
  */
{
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Qz.rotate(Y);
  Qz.rotate(Z);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);
  return;
}

void
SimpleAxis::applyFullRotate(const double xyAngle,
			   const double zAngle,
			   const Geometry::Vec3D& RotCent)
  /*!
    Create the unit vectors
    \param xyAngle :: XY Rotation [second]
    \param zAngle :: Z Rotation [first]
    \param RotCentre :: Z Rotation [first]
  */
{
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Qz.rotate(Y);
  Qz.rotate(Z);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);

  Origin-=RotCent;
  Qz.rotate(Origin);
  Qxy.rotate(Origin);
  Origin+=RotCent;

  return;
}

void
SimpleAxis::reverseZ()
  /*!
    Flip the Z axis keeping Y Fixed
    (could generalize but ...)
  */
{
  Z*= -1.0;
  X*= -1.0;
  return;
}

void 
SimpleAxis::setCentre(const Geometry::Vec3D& C)
  /*!
    User Interface to LU[1] to set Point + Axis
    \param C :: Centre point
  */
{
  ELog::RegMethod RegA("SimpleAxis","setCentre");
  Origin=C;
  return;
}

void
SimpleAxis::applyRotation(const Geometry::Vec3D& Axis,
			 const double Angle)
  /*!
    Apply a rotation to the basis set
    \param Axis :: rotation axis 
    \param Angle :: rotation angle
  */
{
  ELog::RegMethod RegA("SimpleAxis","applyRotation");

  const Geometry::Quaternion Qrot=
    Geometry::Quaternion::calcQRotDeg(Angle,Axis.unit());
  
  Qrot.rotate(X);
  Qrot.rotate(Y);
  Qrot.rotate(Z);
  return;
}


  

}  // NAMESPACE attachSystem
