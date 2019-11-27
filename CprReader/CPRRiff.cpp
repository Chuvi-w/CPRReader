
#include <intrin.h>
#include "CPRRiff.h"
#include "MemFile.h"

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
   char ChWord[1024];

   if(Chunk.Header.Id==FCC_ROOT)
   {
      ChunkOffset=0;
      int nRootID=0;
      do
      {
         memcpy(&DataSize, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ChunkOffset, sizeof(DataSize));
         DataSize=bswap(DataSize);

         memset(ChWord, 0, sizeof(ChWord));
         if(ChunkOffset>Chunk.Header.Size-DataSize)
         {
            printf("Read Root %i error. Size=%i. Remain %i\n", Chunk.Offset, DataSize, (Chunk.Header.Size-ChunkOffset));
            break;
         }
         memcpy(ChWord, m_vData.data()+Chunk.Offset+sizeof(Chunk.Header)+ChunkOffset+sizeof(DataSize), DataSize);
         printf("ROOT<%i>:%s\n", nRootID++, ChWord);
         ChunkOffset+=DataSize+sizeof(DataSize);

      }
      while(ChunkOffset<Chunk.Header.Size);
   }
}

void CRiffFile::ReadARCHChunk(const RiffChunk_t &Chunk)
{
   void *pChankData=nullptr;
   int ChunkType=0;
   uint32_t DataSize;
   size_t ReadOffset=0;

   char ChWord[1024];
   uint16_t AddData16=0, AddData16sw=0;
   uint32_t AddData32=0, AddData32sw=0;

   std::string sFileName=m_sID+"_chunk_"+std::to_string(Chunk.Offset)+"_"+std::to_string(Chunk.Header.Size)+".bin";

#if 0
   FILE *fOUT=fopen(sFileName.c_str(), "wb");
   if(fOUT)
   {
      printf("Write %s\n", sFileName.c_str());
      fwrite(m_vData.data()+Chunk.Offset+sizeof(Chunk.Header), 1, Chunk.Header.Size, fOUT);
      fclose(fOUT);
   }
#endif
   if(2321447==Chunk.Offset&&Chunk.Header.Id==FCC_ARCH)
   {
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
