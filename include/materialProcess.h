/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/materialProcess.h
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
#ifndef materialProcess_h
#define materialProcess_h


/*!
  \class materialProcess
  \brief General information from file
  \version 1.1
  \date August 2016
  \author S. Ansell
*/

class materialProcess
{
 private:

  /// type of cellProduction storage
  typedef std::map<int,MonteCarlo::Material> MTYPE;
  MTYPE matStore;                          ///< Materials 

  int findMaterialCards(std::istream&) const;
  int findCellCards(std::istream&) const;
  
  void processMaterialCards(std::istream&);
  void processCellCards(std::istream&,std::map<int,int>&) const;
  void procMaterial(const int,std::string);
  
 public:
 
  materialProcess();
  materialProcess(const materialProcess&);
  materialProcess& operator=(const materialProcess&);
  virtual ~materialProcess();


  void readMCNP(const std::string&,std::map<int,int>&);

  void writeMaterials(const std::string&) const;
  void write(std::ostream&) const;
  
};

std::ostream&
operator<<(std::ostream&,const materialProcess&);

#endif
