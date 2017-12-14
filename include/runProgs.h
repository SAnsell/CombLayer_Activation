/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/runProgs.h
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
#ifndef runProgs_h
#define runProgs_h

class runProgs
{
 private:

  std::string cinderCMD;            ///< Cinder command/path
  std::string htapeCMD;             ///< HTape command/path
  std::string tabcodeCMD;           ///< tabcode command/path

  runProgs();  

  int runCode(const std::string&);
  
 public:
 
  static runProgs& Instance();
  virtual ~runProgs();

  void setHTapeEXE(const std::string&);
  void setCinderEXE(const std::string&);
  void setTabcodeEXE(const std::string&);
  
  int runHTape(const std::string&,const std::string&);
  int runCinder(const std::string&,const std::string&);
  int runTabCode(const std::string&,const std::string&);
  
};
 
#endif
