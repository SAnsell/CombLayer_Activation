/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/htapeProcess.h
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
#ifndef htapeProcess_h
#define htapeProcess_h

class cellProduction;

/*!
  \class htapeProcess
  \brief General information from file
  \version 1.1
  \date August 2016
  \author S. Ansell
*/

class htapeProcess
{
 private:

  /// type of cellProduction storage
  typedef std::map<int,cellProduction> CTYPE;

  long int nps;     ///< number of points for cell production
  CTYPE cellProd;   ///< Cells  [master production]

  
  static void readZaid(const size_t,cellProduction&,std::istream&);

  long int readHeader(const size_t prodType,std::istream&,
		      CTYPE&) const;
  void processHTape(const std::map<int,double>&);
  long int procProduction(CTYPE&);
  void procGas(CTYPE&);
  void procDestruction(CTYPE&);

  cellProduction* findCellProd(const int);
  cellProduction* findCellProd(CTYPE&,const int) const;


  void addCells(const long int,const CTYPE&);
  
 public:
 
  htapeProcess();
  htapeProcess(const htapeProcess&);
  htapeProcess& operator=(const htapeProcess&);
  virtual ~htapeProcess();


  void scale(const double);
  void addSProdFile(const std::string&,
		const std::map<int,double>&);

  void writeSprods(const std::string&,const int,const double) const;
  void write(std::ostream&) const;
  
};

std::ostream&
operator<<(std::ostream&,const htapeProcess&);

#endif
