/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/SimpleAxis.h
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
#ifndef attachSystem_SimpleAxis_h
#define attachSystem_SimpleAxis_h

namespace attachSystem
{
/*!
  \class SimpleAxis
  \version 1.0
  \author S. Ansell
  \date March 2010
  \brief Component that is at a fixed position
*/

class SimpleAxis  
{
 protected:
  
  const std::string keyName;       ///< Key Name

  Geometry::Vec3D X;            ///< X-coordinate [shutter x]
  Geometry::Vec3D Y;            ///< Y-coordinate [shutter y]
  Geometry::Vec3D Z;            ///< Z-coordinate 
  Geometry::Vec3D Origin;       ///< Origin  


 public:

  SimpleAxis(const std::string&);
  SimpleAxis(const std::string&,const Geometry::Vec3D&);
  SimpleAxis(const std::string&,
	    const Geometry::Vec3D&,const Geometry::Vec3D&,
	    const Geometry::Vec3D&,const Geometry::Vec3D&);
  SimpleAxis(const SimpleAxis&);
  SimpleAxis& operator=(const SimpleAxis&);
  virtual ~SimpleAxis() {}     ///< Destructor


  // Operator Set:
  void createUnitVector();
  void createUnitVector(const SimpleAxis&);
  void createUnitVector(const SimpleAxis&,const Geometry::Vec3D&);
  void createUnitVector(const Geometry::Vec3D&,const Geometry::Vec3D&,
			  const Geometry::Vec3D&);


  void setCentre(const Geometry::Vec3D&);
  void applyShift(const double,const double,const double);
  void applyAngleRotate(const double,const double);
  void applyAngleRotate(const double,const double,const double);
  void applyFullRotate(const double,const double,
		       const Geometry::Vec3D&);
  void reverseZ();
  

  /// Get keyname
  const std::string& getKeyName() const 
    { return keyName; }
  /// Access X
  const Geometry::Vec3D& getX() const 
    { return X; }  
  /// Access Y direction
  const Geometry::Vec3D& getY() const 
    { return Y; }  
  /// Access Z direction
  const Geometry::Vec3D& getZ() const 
    { return Z; }
  /// Access centre
  virtual const Geometry::Vec3D& getCentre() const 
    { return Origin; }  


  void applyRotation(const Geometry::Vec3D&,const double);

};

}

#endif
 
