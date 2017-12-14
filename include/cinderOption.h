/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   workInc/cinderOption.h
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
#ifndef cinderOption_h
#define cinderOption_h

/*!
  \class cinderOption
  \brief General information from file
  \version 1.1
  \date August 2016
  \author S. Ansell
*/

class cinderOption
{
 private:

  double tst;         ///< time step [minimum]
  double signif;      ///< significance
  double epsm;        ///< exp(-epsm) and below set to 1.0
  double epsn;        ///< accuracy to 10^epsn from computer
  double expomax;     ///< max exponent
  int kchn;           ///< chaings in output [default no]
  int klib;           ///< library check [no]
  int nfe;            ///< Fussion type [def 3 : High energy]
  int nlintl;         ///< lines of title info
  int nosame;         ///< Skip return to earlier nuclied [def = yes]
  int russ;           ///< null lines  [def exclude (1)]

 public:
 
  cinderOption();
  cinderOption(const cinderOption&);
  cinderOption& operator=(const cinderOption&);
  virtual ~cinderOption();

  void addString(const std::string&,std::string);  
  void write(std::ostream&,const int,const double,const double) const;

};
 
#endif
