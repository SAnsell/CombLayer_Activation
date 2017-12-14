/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   workInc/cinderHistory.h
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
#ifndef cinderHistory_h
#define cinderHistory_h

/*!
  \class cinderHistory
  \brief General information from file
  \version 1.1
  \date August 2016
  \author S. Ansell
*/

class cinderHistory
{
 private:

  std::vector<double> current;     ///< Current (1/0)
  std::vector<double> time;        ///< time +/- value
  std::vector<char> timeUnit;      ///< time unit

  static double convertTime(const std::string&);
  static char convertTimeUnit(const std::string&);
  
 public:
 
  cinderHistory();
  cinderHistory(const cinderHistory&);
  cinderHistory& operator=(const cinderHistory&);
  virtual ~cinderHistory();

  void addLine(const std::string&,std::string);  
  void write(std::ostream&) const;

};
 
#endif
