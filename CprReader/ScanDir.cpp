#include "ScanDir.h"

std::map<fs::path, std::vector<fs::path>> ListDirectory(const fs::path &dir)
{
   std::map<fs::path,std::vector<fs::path>> vFiles;
   fs::path FileExt;
   for(auto& p:fs::recursive_directory_iterator(dir))
   {
      try
      {
         if(fs::is_regular_file(p.path()))
         {
            if(!p.path().has_extension())
            {
               FileExt.clear();       
            }
            else
            {
               FileExt=p.path().extension();
            }
            if(vFiles.find(FileExt)==vFiles.end())
            {
               vFiles[FileExt]=std::vector<fs::path>();
            }

            vFiles[FileExt].push_back(p.path());
         }
      }
      catch(...)
      {

      }
   }
   return vFiles;
}


