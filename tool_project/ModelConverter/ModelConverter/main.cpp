#include <tchar.h>
#include <string>
#include <conio.h>
#include "GfMdlData.h"
#include "CommandBuilder.h"

#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>


#include "gr_Combiner.h"

#include <nn/gx/CTR/gx_MacroShader.h>
#include <nn/gx/CTR/gx_MacroTexture.h>
#include "util_Float24.h"

//-i "R:/home/niji/resource/field_graphic/map/island01/w_f0102/exports/graphics/w_f0102_01_03.gfmdl" -o "R:/home/niji/resource/field_graphic/map/island01/w_f0102/exports/graphics/w_f0102_01_03.ctr.gfbmdl"

//-i "R:/home/niji/resource/field_graphic/map/island01/w_f0102/exports/graphics/w_f0102_01_01.gfmdl" -o "R:/home/niji/resource/field_graphic/map/island01/w_f0102/exports/graphics/w_f0102_01_01.ctr.gfbmdl"

//-i "R:/home/niji/resource/field_graphic/map/island01/w_f0102/exports/graphics/w_f0102_05_03.gfmdl" -o "R:/home/niji/resource/field_graphic/map/island01/w_f0102/exports/graphics/w_f0102_05_03.ctr.gfbmdl" -logEnable

//-i "C:/home/xy_pokemon/rg/pm0088_00_betbeter/exports/pm0088_00.gfmdl" -o "C:/home/xy_pokemon/rg/pm0088_00_betbeter/exports/pm0088_00.wingl.gfbmdl"


//-i "C:/home/xy_pokemon/rg/pm0092_00_ghos/exports/pm0092_00.gfmdl" -o "C:/home/xy_pokemon/rg/pm0092_00_ghos/exports/pm0092_00.ctr.gfbmdl"


//-i "C:/home/xy_pokemon/dp/pm0402_00_korotock/exports/pm0402_00.gfmdl" -o "C:/home/xy_pokemon/dp/pm0402_00_korotock/exports/pm0402_00.ctr.gfbmdl"

//-i "C:/datatest/plan/w_f0101_.gfmdl" -o "C:/datatest/out.ctr.gfbmdl"

// FOR PROFILING:
//-i "C:/datatest/graphics/w_f0101_07_04.gfmdl" -o "C:/datatest/out.ctr.gfbmdl"

//-i "R:/home/niji/resource/field_graphic/map/island01/w_f01r0102/export/a_f01r0102_00_00.gfmdl" -o "R:/home/niji/resource/field_graphic/map/island01/w_f01r0102/export/a_f01r0102_00_00.ctr.gfbmdl"

//-i "R:/home/niji/resource/effect/battle/graphic/model/ew399/exports/ew399_aura/ew399_aura.gfmdl" -o "R:/home/niji/resource/effect/battle/graphic/model/ew399/exports/ew399_aura/ew399_aura.ctr.gfbmdl"

//-i "C:/mayadata/animationtestdata/test_joint_anime/test_joint_anime.gfmdl" -o "C:/mayadata/animationtestdata/test_joint_anime/test_joint_anime.wingl.gfbmdl"
//-i "C:/mayadata/animationtestdata/test_joint_anime/test_joint_anime.gfmdl" -o "C:/mayadata/animationtestdata/test_joint_anime/test_joint_anime.ctr.gfbmdl"


//-i "R:/home/niji/workspace/04_field/graphic/ibe_mana/exports/f1.gfmdl" -o "R:/home/niji/workspace/04_field/graphic/ibe_mana/exports/f1.ctr.gfbmdl" -meshopt disable

//-i "C:/home/niji/chara/temp/Cutin_Test/exports/bt9999_00.gfmdl" -o "C:/home/niji/chara/temp/Cutin_Test/exports/bt9999_00.ctr.gfbmdl"

//-i "C:/home/xy_pokemon/bw/pm0494_00_victini/exports/pm0494_00.gfmdl" -o "C:/home/xy_pokemon/bw/pm0494_00_victini/exports/pm0494_00.ctr.gfbmdl"

//-i "C:/home/xy_pokemon/bw/pm0577_00_uniran/exports/pm0577_00.gfmdl" -o "C:/home/xy_pokemon/bw/pm0577_00_uniran/exports/pm0577_00.ctr.gfbmdl"

//-i "R:/home/niji/git_program/tool_project/CTRViewer/CTRViewer/romfiles/testdata/exports/pSphere1.gfmdl" -o "R:/home/niji/git_program/tool_project/CTRViewer/CTRViewer/romfiles/testdata/exports/pSphere1.ctr.gfbmdl"

//-i "C:/home/niji/chara/temp/capacity_test/shadow/ts0001_00/scenes/vcolor/ts0001_00.gfmdl" -o "C:/home/niji/chara/temp/capacity_test/shadow/ts0001_00/scenes/vcolor/ts0001_00.ctr.gfbmdl"

//-i "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/検証/gfmdl/com_block_ikuta_150227.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/検証/gfmdl/com_block_ikuta_150227.wingl.gfbmdl"

//-i "C:/Users/ishiguro_masateru/Desktop/test_data/world03_03_06/world03_03_06.gfmdl" -o "C:/Users/ishiguro_masateru/Desktop/test_data/world03_03_06/world03_03_06.ctr.gfbmdl"
//-i "C:/Users/ishiguro_masateru/Desktop/test_data/map_exports/map00_00_01.gfmdl" -o "C:/Users/ishiguro_masateru/Desktop/test_data/map_exports/map00_00_01.ctr.gfbmdl"
//-i "C:/Users/ishiguro_masateru/Desktop/test_data/encgrass02.gfmdl" -o "C:/Users/ishiguro_masateru/Desktop/test_data/encgrass02.ctr.gfbmdl"

//-i "R:/home/niji/exec/data_ctr/tr0001/tr0001_00.gfmdl" -o "R:/home/niji/exec/data_ctr/tr0001/tr0001_00.ctr.gfbmdl"

//-i "R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/IDEdgeMap.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/IDEdgeMap/exports/IDEdgeMap.ctr.gfbmdl"
//-i "R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/IDEdgeMap.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/IDEdgeMap/exports/IDEdgeMap.wingl.gfbmdl"

//-i "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/EdgeMapShader.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/EdgeMapShader.3ds.gfbmdl"
//-i "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/EdgeMapShader.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/EdgeMapShader.wingl.gfbmdl"

//-i "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMap/exports/EdgeMapShader.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/EdgeMap/exports/EdgeMapShader.ctr.gfbmdl"

//-i "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/EdgeMapVColA.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/StereoDraw/romfiles/EdgeMapVColAShader/EdgeMapVColA.ctr.gfbmdl"
//-i "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/EdgeMapVColA.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/EdgeMapVColA.wingl.gfbmdl"






//-i "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/pm0025_00.gfmdl" -o "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/pm0025_00.ctr.gfbmdl"
//-i "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/pm0025_00.gfmdl" -o "R:/home/niji/git_program/tool_project/CTRViewer/CTRViewer/romfiles/exports/pm0025_00.ctr.gfbmdl"
//-i "pm0025_00.gfmdl" -o "pm0025_00.ctr.gfbmdl"

//R:/home/niji/git_program/gflib2/RenderingEngine/resource/EdgeMapVColAShader/exports/EdgeMapVColA.gfmdl

//-i "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/pm0025_00.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/pm0025_00.wingl.gfbmdl"
//-i "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/pm0025_00inc.gfmdl" -o "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/pm0025_00inc.gfbmdl.windx"
//-i "R:/home/niji/git_program/tool_project/ModelConverter/ModelConverter/pm0025_00.gfmdl" -o "R:/home/niji/git_program/tool_project/ModelConverter/ModelConverter/pm0025_00.3ds.gfmdl"

//-i "C:/home/xy_pokemon/rg/pm0006_00_lizardon/exports/pm0006_00.mb.gfmdl" -o "C:/home/xy_pokemon/rg/pm0006_00_lizardon/exports/pm0006_00.mb.3ds.gfmdl"


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

  virtual void* LocalMemoryMalloc(u32 size, u32 alignment){
    return NULL;
  }

  virtual void LocalMemoryFree(void* const address)
  {

  }
};

int _tmain(int argc, _TCHAR* argv[])
{
  std::string inputFilePath = "";
  std::string outputFilePath = "";
  std::string endian = "";
  std::string meshOpt = "Enable";
  std::string pratformType;
  std::string textureDir;
  std::string shaderDir;
  b32         logEnable = false;

  GLAllocator m_Allocator;
  GFGL::Initialize( &m_Allocator );
  GLMemory::SetInstanceAllocator( &m_Allocator );

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
    else if ( strcmp(argv[i], "-meshopt") == 0 )
    {
      meshOpt = argv[++i];
    }
    else if ( strcmp(argv[i], "-textureDir") == 0 )
    {
      ++i;
      if ( strlen( argv[i] ) )
      {
        textureDir = argv[i];
        textureDir += "/";
      }
    }
    else if ( strcmp(argv[i], "-shaderDir") == 0 )
    {
      ++i;
      if ( strlen( argv[i] ) )
      {
        shaderDir = argv[i];
        shaderDir += "/";
      }
    }
    else if ( strcmp(argv[i], "-logEnable") == 0 )
    {
      logEnable = true;
    }
  }


  ModelConverter::GfMdlData gfMdlData;
  b32 fResult;

  gfMdlData.LogEnable( logEnable );

  fResult = gfMdlData.ReadFile( inputFilePath.c_str(), textureDir, shaderDir );
  if ( fResult == false )
  {
    printf( "Error: Failed to open input file: %s\n", inputFilePath.c_str() );
    return -1;
  }

  gfMdlData.MaterialMerge();

  gfMdlData.VertexMerge();
  gfMdlData.VertexSizeOptimize();

  gfMdlData.MeshMerge();
  gfMdlData.MeshDivJointSize( 0, 20 );//必ず実行

  long start = GetTickCount();

  if ( meshOpt == "Enable" )
  {
    gfMdlData.MeshOptimize();

    if ( outputFilePath.find( ".ctr" ) != std::string::npos )
      gfMdlData.MeshOptimizeCTR();
  }

  long end = GetTickCount(); long dif = end - start;
  int mil = (dif % 1000)%100; int sec = (dif / 1000) % 60; int min = (dif / (1000*60)) % 60;
  // printf("ExecuteTime: %02i:%02i:%02i\n",min,sec,mil);
  // getch();

  //gfMdlData.DataCheck( 0, 20 );

  if ( outputFilePath.find( ".wingl" ) != std::string::npos )
  {
    gfMdlData.CreateBinary( false, outputFilePath );
    gfMdlData.WriteFile( outputFilePath.c_str() );
  }
  else if ( outputFilePath.find( ".windx" ) != std::string::npos )
  {
    gfMdlData.CreateBinary( false, outputFilePath );
    gfMdlData.WriteFile( outputFilePath.c_str() );
  }
  else if ( outputFilePath.find( ".ctr" ) != std::string::npos )
  {
    fResult = gfMdlData.CreateBinaryCTR( outputFilePath );
    if ( fResult == false )
    {
      printf( "Error: failed to create CTR binary file.\n" );
      return -1;
    }

    fResult = gfMdlData.WriteFile( outputFilePath.c_str() );
    if ( fResult == false )
    {
      printf( "Error: failed to open output file: %s\n", outputFilePath.c_str() );
      return -1;
    }
  }

  return 0;
}

//copy $(SolutionDir)$(Configuration)\ModelConverter.exe C:\home\niji\tool\ModelConverter\ModelConverter.exe
