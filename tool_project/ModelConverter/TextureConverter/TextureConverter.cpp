// TextureConverter.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <string>
#include <memory.h>
#include <iostream>
#include <types/include/gfl2_Typedef.h>

#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_ImageData.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include "TextureData.h"
#include "Nw4cImageData.h"
#include "stdafx.h"

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph::resource;

//-i "C:/home/xy_pokemon/rg/pm0025_00_pikachu/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa/exports/Textures/pm0025_00_BodyANor.tga" -o "C:/home/xy_pokemon/rg/pm0025_00_pikachu/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa/exports/Textures/pm0025_00_BodyANor.ctr.btex"
//-i "C:/home/xy_pokemon/rg/pm0025_00_pikachu/hoge/exports/Textures/pm0025_00_BodyBNor.tga" -o "C:/home/xy_pokemon/rg/pm0025_00_pikachu/hoge/exports/Textures/pm0025_00_BodyBNor.ctr.btex"

//-i "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Textures/pm0025_00_BodyANor.tga" -o "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Textures/pm0025_00_BodyANor.ctr.btex"

//-i "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Textures/DummyTexId.tga" -o "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Textures/DummyTexId.ctr.btex"
//-i "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Texture/NormalTexture.tga" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Texture/NormalTexture.3ds.tga"

//-i "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Texture/pm0025_00_BodyA1.tga" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Texture/pm0025_00_BodyA1.ctr.tex"
//-i "C:/home/xy_pokemon/bw/pm0497_00_jalorda/sourceimages/pm0497_00_Body1.tga" -o "C:/home/xy_pokemon/bw/pm0497_00_jalorda/sourceimages/pm0497_00_Body1.tex.3ds"
//-i "R:/home/niji/git_program/tool_project/ModelConverter/pm0497_00_Body1.nw4c.tga" -o "R:/home/niji/git_program/tool_project/ModelConverter/pm0497_00_Body1.tga.3ds"
//-i "R:/home/niji/git_program/tool_project/ModelConverter/pm0497_00_Body1.tga" -o "R:/home/niji/git_program/tool_project/ModelConverter/pm0497_00_Body1.tga.3ds"

//-i "R:/home/niji/git_program/tool_project/ModelConverter/pm0025_00_BodyA1.tga" -o "R:/home/niji/git_program/tool_project/ModelConverter/pm0025_00_BodyA1.3ds.tga"
//-i "R:/home/niji/git_program/tool_project/ModelConverter/pm0025_00_BodyANor.tga" -o "R:/home/niji/git_program/tool_project/ModelConverter/pm0025_00_BodyANor.3ds.tga"
//-i "R:/home/niji/git_program/tool_project/ModelConverter/DummyTex.tga" -o "R:/home/niji/git_program/tool_project/ModelConverter/DummyTex.3ds.tga"

class GLAllocator : public IGLAllocator
{
public:
  GLAllocator():IGLAllocator(){}
  virtual ~GLAllocator(){}

#if GFL_DEBUG
  virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
#else
  virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment)
#endif
  {
    return malloc(size);
  }

  virtual void SystemMemoryFree(void* const address)
  {
    free( address );
  }

  virtual void* LocalMemoryMalloc(u32 size, u32 alignment){
    return NULL;
  }

  virtual void LocalMemoryFree(void* const address)
  {

  }
};

class LocalFunc{
public:
  //-----------------------------------------------------------------------------
  /// @brief  リードファイル関数
  //-----------------------------------------------------------------------------
  static char* ReadFile( const char* filename, int* p_size, int align )
  {
    FILE *hFile;
    int file_size;
    char *read_buff;

    hFile = fopen( filename, "rb" );

    if ( hFile == NULL ){
      std::cerr << "Failed to open input file: " << filename << std::endl;
      return NULL;
    }

    fseek( hFile, 0, SEEK_END );
    file_size = ftell( hFile );
    *p_size = file_size;
    fseek( hFile, 0, SEEK_SET );

    file_size += align; // 必ずNULLが入るようにしたい。
    size_t buf_size = file_size + ((-file_size) & (align-1));
    {
      char mess[256];
      memset( mess, 0, 256 );
      sprintf( mess, "%d %d %d\n", buf_size, file_size, align );
    }

    read_buff = new char[buf_size];
    memset( read_buff, 0, buf_size );
    fread( read_buff, file_size, 1, hFile );
    fclose( hFile );


    return( read_buff );
  }

  //-----------------------------------------------------------------------------
  /// @brief  ファイル生成関数
  //-----------------------------------------------------------------------------
  static void CreateFile( const char* filename, const char* p_data, int size )
  {
    FILE *hFile;
    int file_size;
    char *read_buff;

    // ファイルオープンに失敗した場合は返り値1で終了
    if((hFile = fopen( filename, "wb" )) == NULL)
    {
      std::cerr << "Error: failed to open output file: "<< filename << std::endl;
      exit(1);
    }

    fwrite( p_data, 1, size, hFile );
    fclose( hFile );
  }

  //-----------------------------------------------------------------------------
  /// @brief  ファイルコピー関数
  //-----------------------------------------------------------------------------
  static void CopyFile(const char* src_filename, const char* dst_filename )
  {
    int size;
    char* pSrcData = ReadFile( src_filename, &size, 1 );

    CreateFile( dst_filename, pSrcData, size );

    delete pSrcData;
  }
};

int _tmain(int argc, _TCHAR* argv[])
{
  std::string inputFileName = "";
  std::string inputFilePath = "";
  std::string outputFilePath = "";
  std::string endian = "";
  std::string pratformType;
  std::string modulePath;
  b32 fOptimize = true;

  {
    char Path[ParserUtil::GFL_MAX_FILEPATH+1]; 

    GetModuleFileName( NULL, Path, ParserUtil::GFL_MAX_FILEPATH );
    char drive[ParserUtil::GFL_MAX_FILEPATH+1]
    ,dir  [ParserUtil::GFL_MAX_FILEPATH+1]
    ,fname[ParserUtil::GFL_MAX_FILEPATH+1]
    ,ext  [ParserUtil::GFL_MAX_FILEPATH+1];

    _splitpath(Path,drive,dir,fname,ext); // パス名を構成要素に分解します

    modulePath = drive;
    modulePath += dir;
    ParserUtil::ReplaceStr( modulePath, "\\", "/" );
  }

  for( int i = 0; i < argc; ++i )
  {
    if ( strcmp(argv[i], "-i") == 0 )
    {
      inputFilePath = argv[++i];
    }
    else if ( strcmp(argv[i], "-o") == 0 )
    {
      outputFilePath = argv[++i];
    }
    else if ( strcmp(argv[i], "-endian") == 0 )
    {
      endian = argv[++i];
    }
    else if ( strcmp(argv[i], "-notopt") == 0 )
    {
      fOptimize = false;
    }
  }

  inputFileName = ParserUtil::GetFileNameStr( inputFilePath.c_str() );

  GLAllocator m_Allocator;
  GFGL::Initialize( &m_Allocator );
  GLMemory::SetInstanceAllocator( &m_Allocator );

  DWORD processId = GetCurrentProcessId();

  char tempName[64];
  memset( tempName, 0, sizeof(tempName) );
  sprintf_s( tempName, "temp%d/", processId );

  const std::string tempDir = modulePath + tempName;
  const std::string tempFileName = tempDir + "temp.tga";
  const std::string tempNw4cFileName = tempDir + inputFileName;


  if ( outputFilePath.find( ".ctr" ) != std::string::npos )
  {
    Nw4cImageData image;
    image.ReadFromFile( inputFilePath.c_str(), ImageData::SwapType::CTR, true );

    // 入力画像の横解像度が2の冪乗でなければ返り値1で終了
    {
      u32 width = image.GetWidth();
      while(width > 1)
      {
        if(width % 2 != 0)
        {
          std::cerr << "Error: input image width is invalid." << std::endl;
          exit(1);
        }
        width /= 2;
      }
    }

    // 入力画像の縦解像度が2の冪乗でなければ返り値1で終了
    {
      u32 height = image.GetHeight();
      while(height > 1)
      {
        if(height % 2 != 0)
        {
          std::cerr << "Error: input image height is invalid." << std::endl;
          exit(1);
        }
        height /= 2;
      }
    }

    if ( image.IsNw4cTga() == false )
    {
      // Nw4cTga変換(ネイティブフォーマット化)
      std::string textureConverterPath = "NW4C_TextureConverter.exe";

      std::string option;

      option = inputFilePath;
      option += " ";
      option += "-o=";
      option += tempNw4cFileName;

      if ( fOptimize == false )
      {
        switch( image.GetFormat() ){
        case Format::R8G8B8:
          option += " -f=RGB8";
          break;
        default:
          option += " -f=RGBA8";
          break;
        }
      }

      std::string arg = textureConverterPath + " " + option;

      STARTUPINFO si; // スタートアップ情報
      PROCESS_INFORMATION pi; // プロセス情報

      ZeroMemory(&si,sizeof(si));
      si.cb=sizeof(si);

      CreateProcess( NULL, (char*)arg.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );

      CloseHandle(pi.hThread);

      WaitForSingleObject(pi.hProcess,INFINITE);

      CloseHandle(pi.hProcess);

      inputFilePath = tempNw4cFileName;
    }
  }

  TextureData textureData;

  if ( outputFilePath.find( ".ctr" ) != std::string::npos )
  {
    textureData.CreateBinary( inputFilePath, TextureData::Platform::CTR );
  }
  else if ( outputFilePath.find( ".wingl" ) != std::string::npos )
  {
    textureData.CreateBinary( inputFilePath, TextureData::Platform::WinDx );
  }

  textureData.WriteFile( outputFilePath );

  if ( outputFilePath.find( ".ctr" ) != std::string::npos )
  {
    ParserUtil::RemoveFile( tempFileName.c_str() );
    ParserUtil::RemoveFile( tempNw4cFileName.c_str() );
    RemoveDirectory( tempDir.c_str() );
  }

  return 0;
}

