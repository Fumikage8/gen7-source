// ShaderCompiler.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <stdio.h>
#include <vector>
#include <string>

#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <gfx/include/gfl2_Surface.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>

#include "ShaderCompiler.h"
#include "gr_ShaderLite.h"

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph::resource;

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

  virtual void* LocalMemoryMalloc(u32 size,u32 alignment){
    return NULL;
  }

  virtual void LocalMemoryFree(void* const address)
  {

  }
};

#define HELP_MAX (2)

char *help[HELP_MAX] = {
  "ShaderCompiler ver 1.0\n",
  "Convert binary code to ascii code.\n",
};

//-vsh "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shaders/PokeFire.gfvsh" -gsh "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shaders/PokeFire.gfgsh" -fsh "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shaders/BodyAVco_Manual.gffsh" -o "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shaders/PokeFire.ctr.gfbvsh" -def HLSL -def 1

int _tmain(int argc, char* argv[])
{
  std::string vshFilePath;
  std::string fshFilePath;
  std::string gshFilePath;
  std::string outputFilePath;
  std::string shaderTypeName;
  std::string mainFunc;
  std::string endian;
  std::vector<std::string> argList;

  GLAllocator m_Allocator;

  for( int i = 0; i < argc; ++i )
  {
    if ( strcmp(argv[i], "-vsh") == 0 )
    {
      vshFilePath = argv[++i];
    }
    else if ( strcmp(argv[i], "-gsh") == 0 )
    {
      gshFilePath = argv[++i];
    }
    else if ( strcmp(argv[i], "-fsh") == 0 )
    {
      fshFilePath = argv[++i];
    }
    else if ( strcmp(argv[i], "-o") == 0 )
    {
      outputFilePath = argv[++i];
    }
    else if ( strcmp(argv[i], "-endian") == 0 )
    {
      endian = argv[++i];
    }
    else if ( strcmp(argv[i], "-def") == 0 )
    {
      argList.push_back( argv[++i] );
    }
  }

  if ( argc == 1 )
  {
    for( int i = 0; i < HELP_MAX; ++i )
    {
      printf( help[i] );
    }

    return 0;
  }

  if ( argList.size() % 2 )
  {
    printf( "Error: failed to parse command line arguments. One option must have one argument." );
    return 0;
  }

  if ( outputFilePath.find( ".gfbvsh" ) != std::string::npos ) shaderTypeName = "vs";
  if ( outputFilePath.find( ".gfbgsh" ) != std::string::npos ) shaderTypeName = "gs";
  if ( outputFilePath.find( ".gfbfsh" ) != std::string::npos ) shaderTypeName = "ps";

  ShaderCompiler shaderCompiler;

  {//読み込み
    if(vshFilePath.length() && vshFilePath != "dummy")
    {
      shaderCompiler.ReadFile( ShaderCompiler::Type::Vertex, vshFilePath );
    }

    if(gshFilePath.length() && gshFilePath != "dummy")
    {
      shaderCompiler.ReadFile( ShaderCompiler::Type::Geometry, gshFilePath );
    }

    if(fshFilePath.length() && fshFilePath != "dummy")
    {
      shaderCompiler.ReadFile( ShaderCompiler::Type::Pixel, fshFilePath );
    }
  }

  {//コンパイル
    if ( outputFilePath.find( ".wingl" ) != std::string::npos )
    {
      shaderCompiler.CompileGLSL( shaderTypeName, argList );
    }
    else if ( outputFilePath.find( ".windx" ) != std::string::npos )
    {
      shaderCompiler.CompileHLSL( shaderTypeName, argList );
    }
    else if ( outputFilePath.find( ".ctr" ) != std::string::npos )
    {
      shaderCompiler.CompileCTR( shaderTypeName, argList );
    }

    std::string err = shaderCompiler.GetError();
    if ( err.length() )
    {
      printf( "err\n" );
      printf( err.c_str() );
      return 0;
    }
  }

  shaderCompiler.WriteFile( outputFilePath );

  return 0;
}

