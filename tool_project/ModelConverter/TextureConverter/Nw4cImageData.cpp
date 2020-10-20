#include "Nw4cImageData.h"

using namespace gfl2;
using namespace gfl2::gfx;

b32 Nw4cImageData::IsNw4cTga()
{
  const TargaHeader* const pHeader = reinterpret_cast< const TargaHeader* >( m_pDataTop );
  u32 isNw4c = true;

  if ( pHeader->idLength == 0 )
    return false;

  {
    const char *nw4cHeader = "NW4C_Tga Ver1.0";
    char* nw4cCheck = (char*)(pHeader + 1);

    for( u32 i = 0; i < strlen(nw4cHeader); ++i )
    {
      if ( nw4cCheck[i] != nw4cHeader[i] )
      {
        isNw4c = false;
        break;
      }
    }
  }

  return isNw4c;
}

std::string Nw4cImageData::GetNw4cTfm()
{
  std::string formatName = "";
  u32 offSet;
  const u8* pSeek = reinterpret_cast< const u8* >( m_pDataTop );
  pSeek += sizeof(TargaHeader);
  pSeek += 16; // "NW4C_Tga Ver1.0"
  memcpy( &offSet, pSeek, sizeof(u32) );

  pSeek = reinterpret_cast< const u8* >( m_pDataTop );
  pSeek += offSet;

  const DataBlockHeader *pDataBlockHeader = reinterpret_cast<const DataBlockHeader*>(pSeek);

  while( 1 )
  {
    char name[16];
    memcpy( name, pDataBlockHeader->idName, 8 );
    name[8] = 0;

    if ( strcmp( name, "nw4c_end" ) == 0 )
    {
      break;
    }
    else if ( strcmp( name, "nw4c_tfm" ) == 0 )
    {
      u32 length = pDataBlockHeader->size - sizeof(DataBlockHeader);
      pSeek += sizeof(DataBlockHeader);
      memcpy( name, pSeek, length );
      name[length] = 0;

      formatName = name;
      break;
    }

    pSeek += pDataBlockHeader->size;
    pDataBlockHeader = reinterpret_cast<const DataBlockHeader*>(pSeek);
  }

  return formatName;
}

#include <nn/gx/CTR/gx_MacroTexture.h>

s32 Nw4cImageData::GetNw4cFormat()
{
  static const u32 listMax = 14;
  static const std::string nameList[listMax] = {
    "L4", // ： ルミナンス 4 bit
    "L8", // ： ルミナンス 8 bit
    "A4", // ： アルファ 4 bit
    "A8", // ： アルファ 8 bit
    "La4", // ： ルミナンス 4 bit + アルファ 4 bit
    "La8", // ： ルミナンス 8 bit + アルファ 8 bit
    "Hilo8", // ： R 8 bit + G 8 bit
    "Rgb565", // ： R 5 bit + G 6 bit + B 5 bit
    "Rgb8", // ： RGB 各 8 bit
    "Rgb5_a1", // ： RGB 各 5 bit + アルファ 1 bit
    "Rgba4", // ： RGBA 各 4 bit
    "Rgba8", // ： RGBA 各 8 bit
    "Etc1", // ： RGB 圧縮
    "Etc1_a4" // ： RGB 圧縮 + アルファ 4 bit
  };

  static const s32 formatType[listMax] = {
    Format::L4,
    Format::L8,
    Format::A4,
    Format::A8,
    Format::L4A4,
    Format::L8A8,
    Format::H8L8,
    Format::R5G6B5,
    Format::X8R8G8B8,
    Format::R5G5B5A1,
    Format::A4R4G4B4,
    Format::A8R8G8B8,
    Format::ETC1,
    Format::ETC1_A4
  };

  std::string formatName = GetNw4cTfm();

  for( u32 i = 0; i < listMax; ++i )
  {
    if ( formatName == nameList[i] )
    {
      return formatType[i];
    }
  }
  return -1;
}

s32 Nw4cImageData::GetNw4cNativeFormat()
{
  static const u32 listMax = 14;
  static const std::string nameList[listMax] = {
    "L4", // ： ルミナンス 4 bit
    "L8", // ： ルミナンス 8 bit
    "A4", // ： アルファ 4 bit
    "A8", // ： アルファ 8 bit
    "La4", // ： ルミナンス 4 bit + アルファ 4 bit
    "La8", // ： ルミナンス 8 bit + アルファ 8 bit
    "Hilo8", // ： R 8 bit + G 8 bit
    "Rgb565", // ： R 5 bit + G 6 bit + B 5 bit
    "Rgb8", // ： RGB 各 8 bit
    "Rgb5_a1", // ： RGB 各 5 bit + アルファ 1 bit
    "Rgba4", // ： RGBA 各 4 bit
    "Rgba8", // ： RGBA 各 8 bit
    "Etc1", // ： RGB 圧縮
    "Etc1_a4" // ： RGB 圧縮 + アルファ 4 bit
  };

  static const s32 formatType[listMax] = {
    PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_4BITS_DMP, // ： ルミナンス 4 bit
    PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_BYTE, // ： ルミナンス 8 bit
    PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_4BITS_DMP, // ： アルファ 4 bit
    PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_BYTE, // ： アルファ 8 bit
    PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE_4_4_DMP, // ： ルミナンス 4 bit + アルファ 4 bit
    PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE, // ： ルミナンス 8 bit + アルファ 8 bit
    PICA_DATA_TEXTURE_FORMAT_HILO8_DMP_UNSIGNED_BYTE, // ： R 8 bit + G 8 bit
    PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_SHORT_5_6_5, // ： R 5 bit + G 6 bit + B 5 bit
    PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_BYTE, // ： RGB 各 8 bit
    PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_5_5_5_1, // ： RGB 各 5 bit + アルファ 1 bit
    PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_4_4_4_4, // ： RGBA 各 4 bit
    PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_BYTE, // ： RGBA 各 8 bit
    PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP, // ： RGB 圧縮
    PICA_DATA_TEXTURE_FORMAT_ETC1_ALPHA_RGB8_A4_NATIVE_DMP // ： RGB 圧縮 + アルファ 4 bit
  };

  std::string formatName = GetNw4cTfm();

  for( u32 i = 0; i < listMax; ++i )
  {
    if ( formatName == nameList[i] )
    {
      return formatType[i];
    }
  }
  return -1;
}

u32 Nw4cImageData::GetNw4cTxdSize()
{
  u32 dataSize = 0;
  u32 offSet;
  const u8* pSeek = reinterpret_cast< const u8* >( m_pDataTop );
  pSeek += sizeof(TargaHeader);
  pSeek += 16; // "NW4C_Tga Ver1.0"
  memcpy( &offSet, pSeek, sizeof(u32) );

  pSeek = reinterpret_cast< const u8* >( m_pDataTop );
  pSeek += offSet;

  const DataBlockHeader *pDataBlockHeader = reinterpret_cast<const DataBlockHeader*>(pSeek);

  while( 1 )
  {
    char name[16];
    memcpy( name, pDataBlockHeader->idName, 8 );
    name[8] = 0;

    if ( strcmp( name, "nw4c_end" ) == 0 )
    {
      break;
    }
    else if ( strcmp( name, "nw4c_txd" ) == 0 )
    {
      dataSize = pDataBlockHeader->size - sizeof(DataBlockHeader);
      break;
    }

    pSeek += pDataBlockHeader->size;
    pDataBlockHeader = reinterpret_cast<const DataBlockHeader*>(pSeek);
  }

  return dataSize;
}

u8* Nw4cImageData::GetNw4cTxd()
{
  u8* pNativeData = NULL;
  u32 offSet;
  const u8* pSeek = reinterpret_cast< const u8* >( m_pDataTop );
  pSeek += sizeof(TargaHeader);
  pSeek += 16; // "NW4C_Tga Ver1.0"
  memcpy( &offSet, pSeek, sizeof(u32) );

  pSeek = reinterpret_cast< const u8* >( m_pDataTop );
  pSeek += offSet;

  const DataBlockHeader *pDataBlockHeader = reinterpret_cast<const DataBlockHeader*>(pSeek);

  while( 1 )
  {
    char name[16];
    memcpy( name, pDataBlockHeader->idName, 8 );
    name[8] = 0;

    if ( strcmp( name, "nw4c_end" ) == 0 )
    {
      break;
    }
    else if ( strcmp( name, "nw4c_txd" ) == 0 )
    {
      pSeek += sizeof(DataBlockHeader);
      pNativeData = (u8*)pSeek;
      break;
    }

    pSeek += pDataBlockHeader->size;
    pDataBlockHeader = reinterpret_cast<const DataBlockHeader*>(pSeek);
  }

  return pNativeData;
}

s8 Nw4cImageData::GetNw4cMipLevel()
{
  s8 mipLevel = 1;
  u32 offSet;
  const u8* pSeek = reinterpret_cast< const u8* >( m_pDataTop );
  pSeek += sizeof(TargaHeader);
  pSeek += 16; // "NW4C_Tga Ver1.0"
  memcpy( &offSet, pSeek, sizeof(u32) );

  pSeek = reinterpret_cast< const u8* >( m_pDataTop );
  pSeek += offSet;

  const DataBlockHeader *pDataBlockHeader = reinterpret_cast<const DataBlockHeader*>(pSeek);

  while( 1 )
  {
    char name[16];
    memcpy( name, pDataBlockHeader->idName, 8 );
    name[8] = 0;

    if ( strcmp( name, "nw4c_end" ) == 0 )
    {
      break;
    }
    else if ( strcmp( name, "nw4c_mps" ) == 0 )
    {
      pSeek += sizeof(DataBlockHeader);
      mipLevel = *pSeek;
      mipLevel -= 0x30;
      break;
    }

    pSeek += pDataBlockHeader->size;
    pDataBlockHeader = reinterpret_cast<const DataBlockHeader*>(pSeek);
  }

  return mipLevel;
}
