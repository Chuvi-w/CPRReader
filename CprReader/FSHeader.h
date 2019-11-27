#ifndef FSHeader_h__
#define FSHeader_h__

#if defined (__linux__)
#include <experimental/filesystem>
namespace fs=std::experimental::filesystem;
#elif (defined(__MINGW32__)||defined(__MINGW32__))
#include <experimental/filesystem>
namespace fs=std::experimental::filesystem;
#elif defined (WIN32) ||defined (WIN64)
   #include <filesystem>
   #if (_MSC_VER/100==18)
      namespace fs=std::tr2::sys;
   #elif (_MSC_VER/100==19)
      namespace fs=std::experimental::filesystem;
   #endif
#else
#error UNKNOWN FS Header platform
#endif





#endif

