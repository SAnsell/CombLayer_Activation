/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/CellInfo.h
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
#ifndef CellInfo_h
#define CellInfo_h

/*!
  \class CellInfo
  \brief General information from file
  \version 1.1
  \date August 2016
  \author S. Ansell
*/

struct CellInfo
{
  std::string VolName;       ///< Main name 
     
  std::vector<int> cellN;         ///< Cells making up volume
  std::vector<double> cellV;      ///< cell volumes
  
 
  CellInfo(const std::string&);
  CellInfo(const CellInfo&);
  CellInfo& operator=(const CellInfo&);
  virtual ~CellInfo();

  void addCell(const int,const double);
};
 
#endif
