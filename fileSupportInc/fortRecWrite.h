#ifndef RawFile_fortRecWrite_h
#define RawFile_fortRecWrite_h

namespace RawFile 
{

class fortRecWrite
{
 private:

  std::ofstream fortIX;       ///< Fortran file buffer

  std::string buffer;

  int recLeng;           ///< Length of current record

  ///\cond SINGLETON
  fortRecWrite(const fortRecWrite&);
  fortRecWrite& operator=(const fortRecWrite&);

  ///\endcond SINGLETON


 public:

  fortRecWrite();  
  ~fortRecWrite();

  int openFile(const std::string&);
  int writeRecord();

  void addString(const std::string&);
  void addString(const std::string&,const size_t);
  template<typename T> void addItem(const T&);

  void closeFile();
};

} 

#endif
