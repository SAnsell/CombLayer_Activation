#ifndef GLOB_H
#define GLOB_H

namespace glob
{
  /*!
    \class Glob
    \version 1.0
    \brief holding system
   */

  
class Glob
{
 private:
  
  std::string pattern;  ///< Pattern to match
  DIR* dirPtr;          ///< Directory pointer
  dirent* dirEntry;     ///< Place in directory

  Glob(const Glob&);
  void operator=(const Glob&);

 public:

  Glob(const std::string&);
  ~Glob();

  /// Get the next file
  std::string getFileName() const
    { return dirEntry->d_name; }

  /// status 
  operator bool() const
  { return (dirEntry != 0);  }

  bool next();

};

} // namespace glob

#endif
