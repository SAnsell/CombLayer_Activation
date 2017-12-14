#ifndef RawFile_FileBlock_h
#define RawFile_FileBlock_h

namespace RawFile 
{

/*!
  \union FileBlock
  \author S. Ansell
  \date August 2005
  \version 1.0
  \brief Union to allow char reading from raw files
*/
union FileBlock
{
  int I[2];         ///< Integer
  char C[8];        ///< Character block
  float F[2];       ///< Float
  double D;         ///< double
};

} 

#endif
