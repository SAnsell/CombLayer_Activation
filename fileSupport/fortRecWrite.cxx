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
#include "fortRecWrite.h"

namespace RawFile
{

fortRecWrite::fortRecWrite() :
  recLeng(0)
  /*!
    Constructor
  */
{}

fortRecWrite::~fortRecWrite()
  /*! 
    Destructor
  */
{}


int
fortRecWrite::openFile(const std::string& FName) 
  /*!
    Opens the file for reading
    \param FName :: file name
    \return 0 on success / -1 on error
   */
{
  ELog::RegMethod RegA("fortRecWrite","openFile");

  if (!FName.empty())
    {
      fortIX.clear();
      fortIX.close();
      fortIX.open(FName.c_str(),std::ios::binary|std::ios::out);
      return 0;
    }
  return -1;
}

void
fortRecWrite::closeFile()
  /*!
    Close file
   */
{
  writeRecord();
  fortIX.close();
  
  return;
}

int
fortRecWrite::writeRecord()
  /*!
    Gets the next record into the buffer
    - Each record is Length:Stuff:Length
    Note: that these have been changed to long int
    \return 0 on failure / 1 on success
  */
{
  ELog::RegMethod RegA("fortRecWrite","nextRecord");
  
  if (fortIX.good())
    {
      fortIX.write(reinterpret_cast<char*>(&recLeng),
		   sizeof(recLeng));
      fortIX.write(buffer.c_str(),recLeng);
      fortIX.write(reinterpret_cast<char*>(&recLeng),
		   sizeof(recLeng));
      buffer="";
      recLeng=0;
    }      
  return (fortIX.good()) ? 1 : 0;
}


void
fortRecWrite::addString(const std::string& SItem)
  /*!
    ADd a simple item to buffer
    \param SItem :: Inpt stream
    \return type
   */
{
  ELog::RegMethod RegA("fortRecWrite","addString");

  buffer+=SItem;
  recLeng+=SItem.size();
  
  return;
}

void
fortRecWrite::addString(const std::string& SItem,const size_t len)
  /*!
    ADd a simple item to buffer
    \param SItem :: Inpt stream
    \param len :: Length to cut in  (space padded)
    \return type
   */
{
  ELog::RegMethod RegA("fortRecWrite","addString");
  if (SItem.size()==len)
    {
      buffer+=SItem;
    }
  else if (SItem.size()>len)
    {
      buffer+=SItem.substr(0,len);
    }
  else
    {
      buffer+=SItem;
      buffer+=std::string(len-SItem.size(),' ');
    }
  recLeng+=len;
  return;
}

template<typename T>
void
fortRecWrite::addItem(const T& value)
  /*!
    Get a simple item and convert.
    Also consume buffer
    \return type
   */
{
  ELog::RegMethod RegA("fortRecWrite","addItem");
  union charLink
  {
    char CVal[sizeof(T)];
    T Item;
  };

  charLink A;
  A.Item=value;
  for(size_t i=0;i<sizeof(T);i++)
    buffer+=A.CVal[i];

  recLeng+=sizeof(T);
  
  return;
}

template void fortRecWrite::addItem(const int&);
template void fortRecWrite::addItem(const long int&);
template void fortRecWrite::addItem(const double&);
template void fortRecWrite::addItem(const float&);

  
} // NAMESPACE RawFile
