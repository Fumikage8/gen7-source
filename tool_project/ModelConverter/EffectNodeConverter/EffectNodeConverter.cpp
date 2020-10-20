// EffectNodeConverter.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <string>
#include <memory.h>
#include <types/include/gfl2_Typedef.h>

#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_ImageData.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfLocData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryLocData.h>

#include "EffectNodeData.h"

#include "stdafx.h"

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph::resource;


//-i "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Textures/pm0025_00_BodyANor.tga" -o "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Textures/pm0025_00_BodyANor.ctr.btex"

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


int _tmain(int argc, _TCHAR* argv[])
{
  std::string					inputFileName = "";
  std::string					inputFilePath = "";
  std::string					outputFilePath = "";

  bool inputFile,outputFile;

  inputFile = outputFile = false;

  for( int i = 0; i < argc; ++i )
  {
    if ( strcmp(argv[i], "-i") == 0 )
    {
      inputFilePath = argv[++i];
      inputFile = true;
    }
    else if ( strcmp(argv[i], "-o") == 0 )
    {
      outputFilePath = argv[++i];
      outputFile = true;
    }		
  }

  if(!inputFile || !outputFile)
  {
    printf("\nUsage: effectnodeconverter -i inputFile -o outputFile\n");
    return 0;
  }

  inputFileName = ParserUtil::GetFileNameStr( inputFilePath.c_str() );

  GLAllocator				m_Allocator;
  GFGL::Initialize( &m_Allocator );

  printf("InputFile: %s \nOutputFile:%s\n",inputFilePath.c_str(),outputFilePath.c_str());

  // Convert Locator File:
  {
    s32					size;			
    c8*					pGflocData;

    pGflocData = ParserUtil::ReadFile( inputFilePath.c_str(), &size );
    scenegraph::resource::GfEffectNodeData locatorResourceData( pGflocData );

    std::string filePathStr(outputFilePath.c_str());
    locatorResourceData.WriteFile(filePathStr);

    delete pGflocData;

  }

  return 0;
}

