#include <vector>
#include <stdint.h>
#include "FSHeader.h"


typedef struct RiffHeader_s
{
   uint32_t	chunkId;
   uint32_t	chunkSize;
   uint32_t	format;
}RiffHeader_t;

typedef struct RiffChunkHeader_s
{
   uint32_t	Id;
   uint32_t	Size;
}RiffChunkHeader_t;

typedef struct RiffChunk_s
{
   RiffChunkHeader_t Header;
   uint32_t Offset;
}RiffChunk_t;

class CRiffFile
{
public:
   CRiffFile();
   ~CRiffFile();
   bool LoadFile(const fs::path &File);
   bool LoadFile(const std::vector<uint8_t> &vData);
   void ReadROOTChunk(const RiffChunk_t &Chunk);
   void ReadARCHChunk(const RiffChunk_t &Chunk);
   void SetOutID(const std::string &sID);
private:
   std::vector <uint8_t> m_vData;
   std::vector <RiffChunk_t> m_vChunks;
   std::string m_sID;

   std::vector<std::string> m_RootChunks;
};