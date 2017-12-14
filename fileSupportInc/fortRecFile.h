#ifndef RawFile_fortRecFile_h
#define RawFile_fortRecFile_h

namespace RawFile 
{

/*!
  \class fortRecFile

  Using int because fortran records are in lengt
*/
class fortRecFile
{
 private:

  std::ifstream fortIX;       ///< Fortran file buffer
  int bSize;                  ///< Buffer size
  char* Buffer;               ///< Buffer for data

  int curLeng;           ///< Length of current record
  int recLeng;           ///< Length of next record
  int bPos;                   ///< Sub-buffer postion

  ///\cond SINGLETON
  // This should not happend because of the stream
  fortRecFile(const fortRecFile&);
  fortRecFile& operator=(const fortRecFile&);
  ///\endcond SINGLETON

  void resize();

 public:

  fortRecFile();  
  fortRecFile(const std::string&);  
  ~fortRecFile();

  int openFile(const std::string&);
  int nextRecord();
  int endRecord();

   const char* getFB() const { return Buffer; }
   
   template<typename T> T getItem();
   std::string getString(const int);

};

} 

#endif
