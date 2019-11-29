
#include <intrin.h>
#include "CPRRiff.h"
#include "MemFile.h"
#include <iostream>
#include <sstream>

#define __builtin_bswap16 _byteswap_ushort
#define __builtin_bswap32 _byteswap_ulong
#define __builtin_bswap64 _byteswap_uint64


template <typename T>
inline T bswap(T s)
{
   switch(sizeof(T))
   {
      case 2: { auto res=__builtin_bswap16(*(uint16_t *)&s); return *(T *)&res; }
      case 4: { auto res=__builtin_bswap32(*(uint32_t *)&s); return *(T *)&res; }
      case 8: { auto res=__builtin_bswap64(*(uint64_t *)&s); return *(T *)&res; }
      default: return s;
   }
}


template <char a, char b, char c, char d>  struct  FourCC
{
   static const uint32_t value=(((((d<<8)|c)<<8)|b)<<8)|a;
};

#define FourCC_ID(id) uint32_t FCC_##id(FourCC<#id[0],#id[1],#id[2],#id[3]>::value);//Not works
FourCC_ID(RIFF);
FourCC_ID(NUND);
FourCC_ID(ROOT);
FourCC_ID(ARCH);

CRiffFile::CRiffFile()
{

}

CRiffFile::~CRiffFile()
{

}

bool CRiffFile::LoadFile(const fs::path &File)
{
   if(fs::is_regular_file(File))
   {
      auto FileData=ReadFileInMemory(File.string().c_str());
      return LoadFile(FileData);
   }
}

bool CRiffFile::LoadFile(const std::vector<uint8_t> &vData)
{
   if(vData.size()<sizeof(RiffHeader_t))
   {
      return false;
   }
   RiffHeader_t Header;
   RiffChunk_t Chunk;

   m_vData.clear();
   m_vChunks.clear();
   size_t nReadPos=0;
   memcpy(&Header, vData.data()+nReadPos, sizeof(Header));
   Header.chunkSize=bswap(Header.chunkSize);
   char RiffID[5];
  
   if(Header.chunkId==FCC_RIFF&&Header.format==FCC_NUND&&vData.size()>=(Header.chunkSize+sizeof(Header)))
   {
      m_vData=vData;
      nReadPos+=sizeof(Header);
      do 
      {
         memcpy(&Chunk.Header, vData.data()+nReadPos, sizeof(Chunk.Header));
         Chunk.Header.Size=bswap(Chunk.Header.Size);
         memcpy(RiffID, &Chunk.Header.Id, 4);
         RiffID[4]=0;

         if(nReadPos+sizeof(Chunk.Header)+Chunk.Header.Size<=vData.size())
         {
            Chunk.Offset=nReadPos; 
            m_vChunks.push_back(Chunk);
         }
         else
         {
            printf("Read chank error\n");
            m_vChunks.clear();
            break;;
         }
         printf("%s %i\n", RiffID, vData.size()-(nReadPos+sizeof(Chunk.Header)+Chunk.Header.Size));
         if(Chunk.Header.Id==FCC_ROOT)
         {
            ReadROOTChunk(Chunk);
         }

         if(Chunk.Header.Id==FCC_ARCH)
         {
            ReadARCHChunk(Chunk);
           // return false;
         }
         nReadPos+=(sizeof(Chunk.Header)+Chunk.Header.Size);
      } while (nReadPos<vData.size());
     
      printf("\r\n\r\n");
      return !m_vChunks.empty()&&!m_vData.empty();
   }
   return false;
}

void CRiffFile::ReadROOTChunk(const RiffChunk_t &Chunk)
{
   uint32_t DataSize;
   size_t ChunkOffset=0;
   std::string sChunkWord;
   size_t OldPos=0;
   if(Chunk.Header.Id==FCC_ROOT)
   {
      ChunkOffset=0;
      do
      {
         memcpy(&DataSize, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ChunkOffset, sizeof(DataSize));
         DataSize=bswap(DataSize);
         if(!sChunkWord.empty())
         {
            sChunkWord.push_back('_');
         }
         OldPos=sChunkWord.size();
         sChunkWord.resize(sChunkWord.size()+DataSize, '\0');
         //sChunkWord.clear();
         if(ChunkOffset>Chunk.Header.Size-DataSize)
         {
            printf("Read Root %i error. Size=%i. Remain %i\n", Chunk.Offset, DataSize, (Chunk.Header.Size-ChunkOffset));
            break;
         }
         memcpy(&sChunkWord[OldPos], m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ChunkOffset+sizeof(DataSize), DataSize);
        
         ChunkOffset+=DataSize+sizeof(DataSize);
      

      }
      while(ChunkOffset<Chunk.Header.Size);
   }

   m_RootChunks.push_back(sChunkWord);
  
}

void CRiffFile::ReadARCHChunk(const RiffChunk_t &Chunk)
{
   void *pChankData=nullptr;
   int ChunkType=0;
   uint32_t DataSize;
   size_t ReadOffset=0;
   //std::queue
   char ChWord[1024];
   uint16_t AddData16=0, AddData16sw=0;
   uint32_t AddData32=0, AddData32sw=0;
   std::string sFileName=m_RootChunks.back()+"_"+m_sID;
   //std::string sFileName=m_sID+"_chunk_"+std::to_string(Chunk.Offset)+"_"+std::to_string(Chunk.Header.Size)+".bin";
   std::cout<<sFileName<<std::endl;
#if 1
   FILE *fOUT=fopen((sFileName+".bin").c_str(), "wb");
   if(fOUT)
   {
      printf("Write %s\n", sFileName.c_str());
      fwrite(m_vData.data()+Chunk.Offset+sizeof(Chunk.Header), 1, Chunk.Header.Size, fOUT);
      fclose(fOUT);
   }
  // return;
#endif
   if(Chunk.Header.Id==FCC_ARCH)
   {
#if 0
      bool bPrintable=false;
      do 
      {
         
         memcpy(&DataSize, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset, sizeof(DataSize));
         DataSize=bswap(DataSize);
         auto c1=(char*)(m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset+sizeof(DataSize));
        
         if(DataSize&&DataSize>1&&DataSize<200&&c1[DataSize-1]=='\0')
         {
            bPrintable=true;
            for(uint32_t i=0; i<DataSize-1; i++)
            {
               if(c1[i]<0||!isprint(c1[i])||c1[i]==0)
               {
                  bPrintable=false;
                  break;
               }
            }
            if(bPrintable)
            {
               printf("\t\t<%05u>%s\n",ReadOffset,c1);
            }
            else
            {
              // printf("<%05u>::\n",ReadOffset);
            }
           
         }
         ReadOffset+=1;
      } while (ReadOffset<Chunk.Header.Size);
      return;
#endif

      std::stringstream sChunkText;
#if 1
      bool bPrintable=false;
      std::vector<std::pair<size_t,size_t>> vBlockEnds;
      size_t BlockEnd;
      vBlockEnds.push_back(std::make_pair((size_t)0, (size_t)Chunk.Header.Size));
      bool bHaveChanges=false;
      size_t ReadPos;
      size_t nBlocks=0;

      auto PrintSpaces=[&vBlockEnds, &sChunkText]()
      {
         for(int i=0; i<vBlockEnds.size(); i++)
         {
            sChunkText<<" ";
         }
      };

      do
      {
         ReadPos=0;
         memcpy(&ChunkType, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset+ReadPos, sizeof(ChunkType));
         ChunkType=bswap(ChunkType);
         ReadPos+=sizeof(ChunkType);
         memcpy(&DataSize, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset+ReadPos, sizeof(DataSize));
         DataSize=bswap(DataSize);
         ReadPos+=sizeof(DataSize);
         auto c1=(char*)(m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset+ReadPos);
         ReadPos+=DataSize;
         if(
            DataSize&&
            DataSize>1&&
            DataSize<200&&
            Chunk.Offset+sizeof(Chunk.Header)+ReadOffset+ReadPos+DataSize<m_vData.size()&&
            c1[DataSize-1]=='\0')
         {
            bPrintable=true;
            for(uint32_t i=0; i<DataSize-1; i++)
            {
               if(c1[i]<0||!isprint(c1[i])||c1[i]==0)
               {
                  bPrintable=false;
                  break;
               }
            }
            if(bPrintable)
            {
               if(ChunkType==-1||ChunkType==-2)
               {
                  memcpy(&AddData16, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset+ReadPos, sizeof(AddData16));
                  AddData16=bswap(AddData16);
                  ReadPos+=sizeof(AddData16);
                  AddData32=0;
                  if(ChunkType==-1)
                  {
                     memcpy(&AddData32, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset+ReadPos, sizeof(AddData32));
                     AddData32=bswap(AddData32);
                     ReadPos+=sizeof(AddData32);
                  }


                  BlockEnd=ReadOffset+ReadPos+AddData32;

                  if(BlockEnd<=vBlockEnds.back().second||ChunkType==-2)
                  {

                     //printf("<%i><%05u><%05u><%u>%s\n", ChunkType,ReadOffset, AddData32, AddData16, c1);
                     PrintSpaces();
                     sChunkText<<"<"<<ReadOffset-vBlockEnds.back().first<<"> "<<c1<<" {"<<AddData16<<"}";
                     if(ChunkType==-1)
                     {
                        sChunkText<<" ["<<AddData32<<"]"<<std::endl;
                        PrintSpaces();
                        sChunkText<<"{"<<std::endl;

                        vBlockEnds.push_back(std::make_pair(ReadOffset, BlockEnd));
                     }
                     else
                     {
                        sChunkText<<std::endl;
                     }
                  }
                  else
                  {
                     PrintSpaces();
                     sChunkText<<"BAD <"<<ReadOffset-vBlockEnds.back().first<<"> "<<c1<<" {"<<AddData16<<"} ["<<AddData32<<"]"<<std::endl;                    
                  }
               }
               else
               {
                  PrintSpaces();
                  sChunkText<<"Text:<"<<ReadOffset-vBlockEnds.back().first<<"> "<<c1<<std::endl;
               }

            }

         }

         ReadOffset+=1;
         do
         {
            bHaveChanges=false;
            if(!vBlockEnds.empty()&&vBlockEnds.back().second<=ReadOffset)
            {
               bHaveChanges=true;

               vBlockEnds.pop_back();
               PrintSpaces();
               sChunkText<<"}"<<std::endl;
            }
         }
         while(bHaveChanges);

      }
      while(ReadOffset<Chunk.Header.Size);
      auto sStr=sChunkText.str();
      fOUT=fopen((sFileName+".txt").c_str(), "wb");
      if(fOUT)
      {
         printf("Write %s\n", sFileName.c_str());
         fwrite(sStr.c_str(),sStr.length(),1,fOUT);
         fclose(fOUT);
      }
      return;
#endif
      do
      {
         memcpy(&ChunkType, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset, sizeof(ChunkType));
         ReadOffset+=sizeof(ChunkType);
         ChunkType=bswap(ChunkType);
         if(ChunkType!=-2)
         {
            printf("t=%i\n",ChunkType);
         }
         if(ChunkType<0)
         {
            memcpy(&DataSize, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset, sizeof(DataSize));
            DataSize=bswap(DataSize);
            ReadOffset+=sizeof(DataSize);
         }
         else
         {
            DataSize=ChunkType;
         }
         memset(ChWord, 0, sizeof(ChWord));
         
         memcpy(ChWord, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset, DataSize);
         ReadOffset+=DataSize;

         pChankData=(m_vData.data()+sizeof(Chunk.Header)+Chunk.Offset);
         ChWord[DataSize]=0;
        
         if(ChunkType==-2)
         {
            memcpy(&AddData16, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset, sizeof(AddData16));
            AddData16sw=bswap(AddData16);
            printf("AddData16sw=%u\n", AddData16sw);
            ReadOffset+=sizeof(AddData16);
            if(AddData16!=0)
            {
               printf("");
            }
         }
        
         if(ChunkType==-1)
         {
            memcpy(&AddData16, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset, sizeof(AddData16));
            AddData16sw=bswap(AddData16);
            printf("AddData16sw=%u\n", AddData16sw);
            ReadOffset+=sizeof(AddData16);

            memcpy(&AddData32, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ReadOffset, sizeof(AddData32));
            AddData32sw=bswap(AddData32);
            printf("AddData32sw=%u. BE=%u\n", AddData32sw, ReadOffset+sizeof(AddData32)+AddData32sw);
            ReadOffset+=sizeof(AddData32);
           // if(AddData16==5||AddData16==)
            ReadOffset+=26;

         }
         
         
         printf("type=%i [%s]\n", ChunkType, ChWord);
      }
      while(ReadOffset<Chunk.Header.Size);
   }
}

void CRiffFile::SetOutID(const std::string &sID)
{
   m_sID=sID;
}
