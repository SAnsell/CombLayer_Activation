/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/Score.h
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
#ifndef Score_h
#define Score_h

namespace MonteCarlo
{
  class neutron;
}
 
/*!
  \class Score
  \brief Basic Score output file set
  \version 1.1
  \date September 2015
  \author S. Ansell

  Simple Score output
 */

class Score 
{
 private:

  const size_t maxCol;  ///< Max number of collisions
  size_t NCol;                      ///< Number of collisions
  std::vector<double> collisionWt;  ///< Collision count
  std::vector<size_t> collisionN;  ///< Collision count
  
  double Plus;        // good  [make vector???]

  long int pCnt;      ///< Cnt
  
 public:
  
  Score(const size_t);
  Score(const Score&);
  Score& operator=(const Score&);
  ~Score();

  void addNeutron(const MonteCarlo::neutron&);
  double getTrans() const
    { return Plus/static_cast<double>(pCnt); }
  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const Score&);


#endif

