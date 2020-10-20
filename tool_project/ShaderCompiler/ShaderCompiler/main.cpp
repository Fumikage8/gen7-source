// ShaderCompiler.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <stdio.h>
#include <vector>
#include <string>
#include "ShaderCompiler.h"

#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <gfx/Sample/Framework/IApplication.h>
#include <gfx/include/gfl2_Surface.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
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

	virtual void* SystemMemoryMalloc(u32 size,u32 alignment){
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


#define			HELP_MAX		(2)

char *help[HELP_MAX] = {
	"ShaderCompiler		ver 1.0\n",
	"バイナリコードをアスキーコードに変換します。\n",
};

//-vsh "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shader/PDC_Body_Shadow.gfvsh" -fsh "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shader/PDC_Body_Shadow.gffsh" -o "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shader/PDC_Body_Shadow.3ds.gfvsh" -def HLSL -def 1
//-vsh "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shader/PDC_pm_keikouo_Neon.gfvsh" -fsh "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shader/PDC_pm_keikouo_Neon.gffsh" -o "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/Shader/PDC_pm_keikouo_Neon.3ds.gfvsh" -def HLSL -def 1

//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.wingl.gfvsh" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/Shader/Comb2_Manual.wingl.gffsh" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/EdgeMapVColAShader/Shader/Comb2_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/EdgeMapVColAShader/Shader/Comb2_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/EdgeMapVColAShader/Shader/Comb2_Manual.3ds.gfvsh" -def HLSL -def 1

//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/Shader/Comb3_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/Shader/Comb3_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/Shader/Comb3_Manual.wingl.gfvsh" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/Shader/Comb3_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/Shader/Comb3_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/Shader/Comb3_Manual.wingl.gffsh" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/EdgeMap/exports/Shader/Comb3_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/EdgeMap/exports/Shader/Comb3_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/EdgeMap/exports/Shader/Comb3_Manual.3ds.gfvsh" -def HLSL -def 1

//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb2_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb2_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb2_Manual.wingl.gfvsh" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb2_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb2_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb2_Manual.wingl.gffsh" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/IDEdgeMap/exports/Shader/Comb2_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/IDEdgeMap/exports/Shader/Comb2_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/IDEdgeMap/exports/Shader/Comb2_Manual.3ds.gfvsh" -def HLSL -def 1

//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb3_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb3_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb3_Manual.wingl.gfvsh" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb3_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb3_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/idedgemap/exports/Shader/Comb3_Manual.wingl.gffsh" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/IDEdgeMap/exports/Shader/Comb3_Manual.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/IDEdgeMap/exports/Shader/Comb3_Manual.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/IDEdgeMap/exports/Shader/Comb3_Manual.3ds.gfvsh" -def HLSL -def 1



//R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/EdgeMapVColAShader/Shader/

//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shader/PDC_pm_keikouo_Neon.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shader/PDC_pm_keikouo_Neon.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shader/PDC_pm_keikouo_Neon.gfvsh.wingl" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shader/PDC_pm_keikouo_Neon.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shader/PDC_pm_keikouo_Neon.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shader/PDC_pm_keikouo_Neon.gffsh.wingl" -def HLSL -def 1

//-vsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shader/PDC_pm_keikouo_Neon.gfvsh" -fsh "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shader/PDC_pm_keikouo_Neon.gffsh" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shader/PDC_pm_keikouo_Neon.gffsh.wingl" -def HLSL -def 1
//-vsh "R:/home/niji/git_program/tool_project/ShaderCompiler/PDC_Body_Shadow.gfvsh" -fsh "R:/home/niji/git_program/tool_project/ShaderCompiler/PDC_Body_Shadow.gfvsh" -o "R:/home/niji/git_program/tool_project/ShaderCompiler/PDC_Body_Shadow.3ds.gfvsh" -def HLSL -def 1

int _tmain(int argc, char* argv[])
{
	std::string										vshFilePath;
	std::string										fshFilePath;
	std::string										gshFilePath;
	std::string										outputFilePath;
	std::string										shaderTypeName;
	std::string										mainFunc;
	std::string										endian;
	std::vector<std::string>			argList;

	GLAllocator										m_Allocator;
	DisplayDeviceHandle						m_Handle;

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
		printf( "引数の数が半端です。名前と値の２つずつにして下さい。" );
		return 0;
	}

	if ( outputFilePath.find( ".gfvsh" ) != std::string::npos )		shaderTypeName = "vs";
	if ( outputFilePath.find( ".gfgsh" ) != std::string::npos )		shaderTypeName = "gs";
	if ( outputFilePath.find( ".gffsh" ) != std::string::npos )		shaderTypeName = "ps";

	ShaderCompiler								shaderCompiler;

	{//読み込み
		shaderCompiler.ReadFile( ShaderCompiler::Type::Vertex, vshFilePath );

		if ( gshFilePath.length() )
			shaderCompiler.ReadFile( ShaderCompiler::Type::Geometry, gshFilePath );

		shaderCompiler.ReadFile( ShaderCompiler::Type::Pixel, fshFilePath );
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
		else if ( outputFilePath.find( ".3ds" ) != std::string::npos )
		{
			shaderCompiler.CompileCTR( shaderTypeName, argList );
		}

		std::string			err = shaderCompiler.GetError();
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

