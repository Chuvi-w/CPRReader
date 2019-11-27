#include <string>
#include "CPRRiff.h"

void ReadCPR(const std::string &sFileName)
{
   std::string sFullName=std::string("..\\CprFiles\\")+sFileName+".cpr";
   CRiffFile Riff;
   Riff.SetOutID(sFileName);
   Riff.LoadFile(sFullName);

      return ;


}
int main()
{
   ReadCPR("vame01");
//    ReadCPR("vame03");
//    ReadCPR("vame05");
//    ReadCPR("vame08");
//    ReadCPR("vame09");
//    ReadCPR("vame11");
//    ReadCPR("vame13");
//    ReadCPR("vame14");
   return 0;
}

