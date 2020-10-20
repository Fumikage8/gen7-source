#ifndef TEXTURE_DATA_H_INCLUDED
#define TEXTURE_DATA_H_INCLUDED

#include <stdio.h>
#include <string>
#include <memory.h>
#include <types/include/gfl2_Typedef.h>

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  TextureData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct TextureData
{
  TextureData():m_pBinaryData(NULL){}
  ~TextureData(){}

  void destroy()
  {
    if ( m_pBinaryData )
      delete[] m_pBinaryData;
  }

  static const u32 PATH_MAX = 64;//!@todo 適切な場所から参照

  u32 m_No;
  std::string m_FilePath;

  u32 m_BinarySize;
  u8 *m_pBinaryData;

  enum Platform{
    WinDx,
    CTR
  };

  void CreateBinary( std::string path, Platform type );
  bool WriteFile( std::string filePath );

  Platform m_Type;
};

#endif
