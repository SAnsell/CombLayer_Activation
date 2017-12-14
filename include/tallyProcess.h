/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/tallyProcess.h
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
#ifndef tallyProcess_h
#define tallyProcess_h

class cellFlux;

/*!
  \class tallyProcess
  \brief General information from file
  \version 1.1
  \date August 2016
  \author S. Ansell
*/

class tallyProcess
{
 private:

  /// type of cellProduction storage
  typedef std::map<int,WorkData> CTYPE;

  long int nps;                            ///< Current nps
  CTYPE cellFlux;                          ///< Fluxes
 
  int find1Tally(std::istream&,int&,long int&);
  void getFluxTally(std::istream&,const long int);
  void readWorkEnergy(std::istream&,const long int,
		      const std::vector<int>&);

  
  void addFlux(const int,const WorkData&);

  void cinderRebin(std::vector<WorkData>&);
  
 public:
 
  tallyProcess();
  tallyProcess(const tallyProcess&);
  tallyProcess& operator=(const tallyProcess&);
  virtual ~tallyProcess();

  const WorkData& getWorkData(const int) const;

  void readMCNP(const std::string&);

  bool isValid(const int,const double) const;
  void writeFluxes(const std::string&,const int) const;
  void write(std::ostream&) const;
  
};

std::ostream&
operator<<(std::ostream&,const tallyProcess&);

#endif
