#ifndef Nw4cImageData_H_INCLUDED
#define Nw4cImageData_H_INCLUDED

#include <string>

#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_ImageData.h>

class Nw4cImageData : public gfl2::gfx::ImageData
{
public:
  Nw4cImageData():ImageData(){}
  virtual ~Nw4cImageData(){}

  struct DataBlockHeader{
    char idName[8];
    u32 size;
  };

  b32 IsNw4cTga();
  std::string GetNw4cTfm();
  s32 GetNw4cNativeFormat();
  s32 GetNw4cFormat();

  u32 GetNw4cTxdSize();
  u8* GetNw4cTxd();
  s8 GetNw4cMipLevel();
};

#endif
