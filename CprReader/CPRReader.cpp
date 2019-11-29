#include <string>
#include "CPRRiff.h"
#include "ScanDir.h"
#include <iostream>

void ReadCPR(const fs::path &fFile)
{
   std::string sIDName=fFile.stem().string();
   CRiffFile Riff;
   Riff.SetOutID(sIDName);
   Riff.LoadFile(fFile);

   return;
}


int main()
{
  auto vFiles= ListDirectory(R"(D:\ComplNumb\Merm_2032)");
  std::list<fs::path> vCPRList;
  std::vector<fs::path> vCPRVect;
  if(vFiles.find(".cpr")!=vFiles.end())
  {
     vCPRVect=vFiles[".cpr"];
     std::copy(vCPRVect.begin(), vCPRVect.end(), std::back_inserter(vCPRList));
     vCPRList.sort([](const fs::path &p1, const fs::path &p2)
     {
        return fs::file_size(p1)<fs::file_size(p2);
     });
     
     for(auto f:vCPRList)
     {
        ReadCPR(f);
     }
     //ReadCPR(vCPRList.back());
     ;
  }


   //ReadCPR("vame01");
//    ReadCPR("vame03");
//    ReadCPR("vame05");
//    ReadCPR("vame08");
//    ReadCPR("vame09");
//    ReadCPR("vame11");
//    ReadCPR("vame13");
//    ReadCPR("vame14");
   return 0;
}

