/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/Control.h
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
#ifndef Control_h
#define Control_h

/*!
  \class Control
  \brief General information from file
  \version 1.1
  \date August 2016
  \author S. Ansell
*/

class Control
{
 private:

  std::string libraryPath;               ///< Path to main library
  std::string matFile;                   ///< Material info file

  std::vector<std::string> mcnpOFiles;   ///< MCNP output files
  std::vector<std::string> mcnpHFiles;   ///< MCNP htape files
  
  std::string outDirBase;         ///< Output directory header

  
  cinderOption COpt;              ///< Cinder options
  double htapeNorm;               ///< htape normalization [if different]
  double srcNorm;                 ///< source normalization

  std::map<int,std::string> VolName;    ///< Cell names
  std::map<int,double> Vols;            ///< Cell volumes
  std::map<int,int> MatNumber;          ///< Cell materials

  std::map<int,int> CellReMap;          ///< Decide if cell needs remapping
  cinderHistory history;                ///< history set

  htapeProcess HT;                      ///< Htape (spallation)
  tallyProcess fluxes;                  ///< Neutron fluxes [input]
  materialProcess matCards;             ///< Material cards

  void addMCNPOutFiles(std::string&);
  void addMCNPHistpFiles(std::string&);
  void setLibrary(const std::string&);

  void setBaseName(const std::string&);
  void procFiles(const std::string&,std::string);
  void procNormalization(const std::string&,std::string);
  void procCellList(const std::string&,std::string);    
  void procCellReMap(const std::string&,std::string);
  
  std::string getOutDir(const int) const;
  
  void writeLibrary(const std::string&) const;
  void writeInput(const std::string&,const int,const double) const;
  void addTallyCells(const std::string&,const int);
  
 public:
 
  Control();
  Control(const Control&);
  Control& operator=(const Control&);
  virtual ~Control();

  int getCellMat(const int) const;
  
  void readControlFile(const std::string&);
  void runHTape();
  void readFluxes();
  void readMaterials();
  void writeCinderInput() const;
  
};
 
#endif
