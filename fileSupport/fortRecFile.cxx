#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h" 
#include "fortRecFile.h"

namespace RawFile
{

fortRecFile::fortRecFile() :
  bSize(1024),Buffer(new char[1024]),
  curLeng(0),recLeng(0),bPos(0)
  /*!
    Constructor
  */
{}


fortRecFile::fortRecFile(const std::string& FName) :
  bSize(1024),Buffer(new char[1024]),
  curLeng(0),recLeng(0),bPos(0)
  /*!
    Constructor
    \param FName :: file name
  */
{
  openFile(FName);
}

fortRecFile::~fortRecFile()
  /*! 
    Destructor
  */
{
  delete [] Buffer;
}

void
fortRecFile::resize()
  /*!
    Resize the buffer
  */
{
  if (recLeng>bSize)
    {
      delete [] Buffer;
      Buffer=new char[recLeng+32];          // Space for extra
      bSize=recLeng;
    }
  return;
}

int
fortRecFile::openFile(const std::string& FName) 
  /*!
    Opens the file for reading
    \param FName :: file name
    \return 0 on success / -1 on error
   */
{
  ELog::RegMethod RegA("fortRecFile","openFile");
  if (!FName.empty())
    {
      fortIX.clear();
      fortIX.open(FName.c_str(),std::ios::binary|std::ios::in);
      if(nextRecord())
	{
	  ELog::EM<<"Successful read:"<<FName<<ELog::endTrace;
	  return 0;
	}
    }
  ELog::EM<<"Failed to open file :"<<FName<<ELog::endErr;
  return -1;
}

int
fortRecFile::endRecord()
  /*!
    Advance to the end of the current record
    \return Number of points to use
  */
{
  if (bPos)
    return nextRecord();
  return curLeng;
}

int
fortRecFile::nextRecord()
  /*!
    Gets the next record into the buffer
    - Each record is Length:Stuff:Length
    Note: that these have been changed to long int
    \return 0 on failure / length of success
  */
{
  ELog::RegMethod RegA("fortRecFile","nextRecord");
  
  if (fortIX.good())
    {
      fortIX.read(reinterpret_cast<char*>(&recLeng),
		  sizeof(recLeng));
      ELog::EM<<"REC VALUE == "<<recLeng<<ELog::endDiag;
      resize();
      if (recLeng)
	{
	  curLeng=recLeng;
	  // Plus end terminator
	  fortIX.read(Buffer,recLeng+
		      static_cast<long int>(sizeof(recLeng)));
	  
	  if (*reinterpret_cast<int*>(Buffer+recLeng)!=curLeng)
	    ELog::EM<<"Failed at point "<<curLeng<<" "
		    <<*reinterpret_cast<int*>(Buffer+recLeng)
					       <<ELog::endErr;
	  bPos=0;
	}
    }
  else
    {
      curLeng=0;
      recLeng=0;
    }
  return curLeng;
}

template<typename T>
T
fortRecFile::getItem()
  /*!
    Get a simple item and convert.
    Also consume buffer
    \return type
   */
{
  const int sizeItem(sizeof(T));
  if (sizeItem+bPos>curLeng)
    {
      ELog::EM<<"Failed on "<<bPos<<" "<<curLeng
	      <<" "<<sizeItem<<ELog::endErr;
  
      throw ColErr::RangeError<long int>(sizeItem,bPos,curLeng,
					 "fortRecFile::getItem");
    }
  const T out(*(reinterpret_cast<const T*>(Buffer+bPos)));
  bPos+=sizeItem;
  if (bPos==curLeng) nextRecord();
  return out;
}


std::string
fortRecFile::getString(const int leng)
  /*!
    Get a string and consume the buffer.
    \param leng :: leng of string to get
    \return String type
   */
{
  if (leng+bPos>curLeng)
    throw ColErr::RangeError<long int>(leng,bPos,curLeng,
				  "fortRecFile::getString");
  const std::string Out(Buffer+bPos,static_cast<size_t>(leng));
  bPos+=leng;       
  if (bPos==curLeng) nextRecord();

  return Out;
}


template double fortRecFile::getItem();
template float fortRecFile::getItem();
template int fortRecFile::getItem();
template unsigned int fortRecFile::getItem();
template long int fortRecFile::getItem();
template short int fortRecFile::getItem();
template char fortRecFile::getItem();


} // NAMESPACE RawFile
