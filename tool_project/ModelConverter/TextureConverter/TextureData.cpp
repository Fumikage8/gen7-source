#include <nn/gx/CTR/gx_MacroTexture.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include "TextureData.h"
#include "Nw4cImageData.h"

using namespace gfl2::renderingengine::scenegraph::resource;

void TextureData::CreateBinary( std::string path, Platform type )
{
  BinaryMaker binary;

  struct ImageHeader
  {
    // 16バイトアラインサイズ
    s32 size;
    s32 dumy[3];
    c8 path[PATH_MAX];
  };

  struct FormatHeader
  {
    // 16バイトアラインサイズ
    u16 width;
    u16 height;
    u16 format;
    u16 mipLevel;
  };

  ImageHeader header;

  m_FilePath = path;

  header.dumy[0] = 0;
  header.dumy[1] = 0;
  header.dumy[2] = 0;
  memset( header.path, 0, sizeof(header.path) );
  ParserUtil::GetFileName( header.path, m_FilePath.c_str() );

  m_Type = type;

  switch( m_Type ){
  case Platform::WinDx:
    {
      c8* pImageData = ParserUtil::ReadFile( m_FilePath.c_str(), &header.size );

      binary.Write( &header, sizeof(header) );
      binary.Write( pImageData, header.size );

      m_pBinaryData = binary.CreateFixedBuffer();
      m_BinarySize = binary.GetBinarySize();

      delete[] pImageData;
      pImageData = NULL;
    }
    break;
  case Platform::CTR:
    {
      Nw4cImageData image;
      u32 commandBuffer[3*2];
      u32 *command = commandBuffer;
      FormatHeader fheader;

      image.ReadFromFile( m_FilePath.c_str() );

      header.size = image.GetNw4cTxdSize();
      fheader.width = image.GetWidth();
      fheader.height = image.GetHeight();
      fheader.format = image.GetNw4cFormat();
      fheader.mipLevel = image.GetNw4cMipLevel();

#if 0
      // デフォルトではTexture0に設定するコマンド
      // 0x082
      *command++ = PICA_CMD_DATA_TEXTURE_SIZE( image.GetWidth(), image.GetHeight() );
      *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_SIZE ); // <- runtimeで設定

      // 0x085
      *command++ = PICA_CMD_DATA_TEXTURE_ADDR( 0 ); // <- runtimeで設定
      *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR1 ); // <- runtimeで設定

      // 0x08e
      u32 format = image.GetNw4cFormat();
      *command++ = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( format );
      *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_FORMAT ); // <- runtimeで設定
#endif

      // チャンクヘッダー24byte
      binary.Write( &header, sizeof(header) ); // 80
      binary.Write( &fheader, sizeof(fheader) ); // 8byte

      {
        // 128バイトアラインキープ
        u32 binSize = 24 + binary.GetBinarySize();
        u32 paddingSize = 128 - ( binSize % 128 );
        u8 padding = 0xFF;

        for( s32 i = 0; i < paddingSize; ++i )
          binary.Write( &padding, sizeof(padding) );
      }

      binary.Write( image.GetNw4cTxd(), image.GetNw4cTxdSize() ); // 128バイトアラインキープ

      m_pBinaryData = binary.CreateFixedBuffer();
      m_BinarySize = binary.GetBinarySize();
    }
    break;
  }
}

bool TextureData::WriteFile( std::string filePath )
{
  {
    BinaryMaker bm;
    u64 chunkID;

    static const u32 sc_Version = GfModelVersion::PokeShaderLutLightingFix;
    u32 dataCount = 1;

    // バージョン
    bm.Write(&sc_Version, sizeof(sc_Version)); // 4

    // データ数
    bm.Write(&dataCount, sizeof(dataCount)); // 8

    GfBinaryMdlData::ChunkHeader modelHeader(IResourceData::Type("texture").m_ID, m_BinarySize);
    bm.Write(&modelHeader, sizeof(modelHeader)); // 24
    bm.Write(m_pBinaryData, m_BinarySize);

    // キャッシュ作成
    u8* buffer = bm.CreateFixedBuffer();
    u32 bufferSize = bm.GetBinarySize();
    ParserUtil::FileCreate( filePath.c_str(), reinterpret_cast<char*>(buffer), bufferSize );
    delete[] buffer;
  }

  return true;
}
