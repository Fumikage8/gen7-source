#include "ShaderCompiler.h"
#include<RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>

#include <util/include/gfl2_FnvHash.h>
#include "gr_ShaderLite.h"

#include <windows.h>
#include <iostream>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")  

#include "gr_Combiner.h"
//#include "gr_RenderState.h"
//#include "gr_Texture.h"
//#include "gr_Vertex.h"
//#include "gr_LookUpTable.h"

using namespace rapidxml;

using namespace gfl2;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph::resource;

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ShaderCompiler::ShaderCompiler()
{

}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ShaderCompiler::~ShaderCompiler()
{

}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::SetSource( Type type, const char* pSrc )
{
  m_Source[type] = pSrc;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::CompileHLSL( std::string shaderTypeName, std::vector<std::string> argList )
{
  /*HRESULT					hr;
  DWORD						dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
  ID3DBlob				*pBlob = NULL;
  ID3DBlob				*pErrorBlob = NULL;
  Type						type;
  std::string			shaderFuncName = "main";

  if ( shaderTypeName == "vs" )
  {
  type = Type::Vertex;
  shaderTypeName = "vs_4_0";
  }
  else if ( shaderTypeName == "gs" )
  {
  type = Type::Geometry;
  shaderTypeName = "gs_4_0";
  }
  else if ( shaderTypeName == "ps" )
  {
  type = Type::Pixel;
  shaderTypeName = "ps_4_0";
  }

  D3D_SHADER_MACRO		*pDefList;
  int									macroNum = 0;

  if ( argList.size() )
  {
  macroNum = (argList.size() / 2) + 1;
  pDefList = new D3D_SHADER_MACRO[ macroNum ];
  pDefList[ macroNum - 1 ].Name = NULL;
  pDefList[ macroNum - 1 ].Definition = NULL;
  }

  for( unsigned int i = 0; i < argList.size(); i += 2 )
  {
  pDefList[i/2].Name = argList[i].c_str();
  pDefList[i/2].Definition = argList[i+1].c_str();
  }

  hr = D3DCompile( m_Source[type].c_str(), m_Source[type].length(), NULL, pDefList, NULL, shaderFuncName.c_str(), shaderTypeName.c_str(), dwShaderFlags, 0, &pBlob, &pErrorBlob );

  if( FAILED(hr) )
  {

  if ( shaderTypeName == "vs_4_0" )
  {
  shaderFuncName = "VsMain";
  }
  else if ( shaderTypeName == "gs_4_0" )
  {
  shaderFuncName = "GsMain";
  }
  else if ( shaderTypeName == "ps_4_0" )
  {
  shaderFuncName = "PsMain";
  }

  hr = D3DCompile( m_Source[type].c_str(), m_Source[type].length(), NULL, pDefList, NULL, shaderFuncName.c_str(), shaderTypeName.c_str(), dwShaderFlags, 0, &pBlob, &pErrorBlob );

  if( FAILED(hr) )
  {
  if( pErrorBlob != NULL )
  {
  m_ErrorCode = (char*)pErrorBlob->GetBufferPointer();
  if ( pDefList )				delete[] pDefList;
  if ( pBlob )					pBlob->Release();
  if ( pErrorBlob )			pErrorBlob->Release();
  return;
  }
  }
  }

  {
  FileHeader				fileHeader;


  memset( fileHeader.shaderName, 0, sizeof(fileHeader.shaderName) );
  strcat_s( fileHeader.shaderName, m_ShaderName.c_str() );

  util::FnvHash				hash( reinterpret_cast<const u8*>(m_ShaderName.c_str()) );
  fileHeader.shaderNameHash = hash.value;

  m_BinaryMaker.Write( &fileHeader, sizeof(fileHeader) );
  }

  {
  int					shaderNum = 1;
  m_BinaryMaker.Write( &shaderNum, sizeof(shaderNum) );
  }

  ShaderHeader		header;

  header.size = pBlob->GetBufferSize();
  header.shaderType = type;
  memset( header.path, 0, sizeof(header.path) );
  ParserUtil::GetFileName( header.path, m_FileName.c_str() );

  m_BinaryMaker.Write( &header, sizeof(header) );
  m_BinaryMaker.Write(pBlob->GetBufferPointer(), pBlob->GetBufferSize());

  if ( pDefList )			delete[] pDefList;
  if ( pBlob )				pBlob->Release();
  if ( pErrorBlob )		pErrorBlob->Release();*/
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::CompileGLSL( std::string shaderTypeName, std::vector<std::string> argList )
{
  Type type;

  if ( shaderTypeName == "vs" )
  {
    type = Type::Vertex;
  }
  else if ( shaderTypeName == "gs" )
  {
    type = Type::Geometry;
  }
  else if ( shaderTypeName == "ps" )
  {
    type = Type::Pixel;
  }

  {
    FileHeader fileHeader;

    memset( &fileHeader, 0, sizeof(fileHeader) );
    strcat_s( fileHeader.shaderName, m_ShaderName[type].c_str() );

    util::FnvHash hash( reinterpret_cast<const u8*>(m_ShaderName[type].c_str()) );
    fileHeader.shaderNameHash = hash.value;
    fileHeader.shaderNum = 1;

    m_BinaryMaker.Write( &fileHeader, sizeof(fileHeader) );
  }

  {
    int shaderNum = 1;
    m_BinaryMaker.Write( &shaderNum, sizeof(shaderNum) );
  }

  ShaderHeader header;
  c8 nullStr = 0;

  memset( &header, 0, sizeof(header) );

  header.size = m_Source[type].length() + sizeof(nullStr);
  header.shaderType = type;

  ParserUtil::GetFileName( header.path, m_FileName.c_str() );

  m_BinaryMaker.Write( &header, sizeof(header) );
  m_BinaryMaker.Write( m_Source[type].c_str(), m_Source[type].length() );
  m_BinaryMaker.Write( &nullStr, sizeof(nullStr) );
}

void ShaderCompiler::CompileCTR(std::string shaderTypeName, std::vector<std::string> argList )
{
  BinaryMaker commandBinaryMaker;
  int shaderNum = 1;

  Type type;

  if ( shaderTypeName == "vs" )
  {
    type = Type::Vertex;
  }
  else if ( shaderTypeName == "gs" )
  {
    type = Type::Geometry;
  }
  else if ( shaderTypeName == "ps" )
  {
    type = Type::Pixel;
  }

  std::string modulePath;

  {
    char Path[MAX_PATH+1]; 

    GetModuleFileName( NULL, Path, MAX_PATH );
    char drive[MAX_PATH+1]
    ,dir  [MAX_PATH+1]
    ,fname[MAX_PATH+1]
    ,ext  [MAX_PATH+1];

    _splitpath(Path,drive,dir,fname,ext);//パス名を構成要素に分解します

    modulePath = drive;
    modulePath += dir;
    //modulePath += "temp/";
    ParserUtil::ReplaceStr( modulePath, "\\", "/" );
  }

  class LocalFunc{
  public:
    static void CreateCtrShaderFile( std::string source, std::string tempFileName )
    {//コードを抜き出しvshファイル作成
      std::vector<std::string> codeList;
      codeList = ParserUtil::SplitStr( source, "//<ctr_code_start>" );
      codeList = ParserUtil::SplitStr( codeList[1], "//<ctr_code_end>" );

      std::string shCode = codeList[0];

      ParserUtil::ReplaceStr( shCode, "/<ctr_code_start>", "" );

      ParserUtil::FileCreate( tempFileName.c_str(), shCode.c_str(), shCode.length() );
    }

    static void ShaderAssembler( std::string tempFileName, std::string tempObjFileName )
    {//アセンブル
      std::string assemblerPath = "ctr_VertexShaderAssembler32.exe";

      STARTUPINFO si;// スタートアップ情報
      PROCESS_INFORMATION pi;// プロセス情報

      ZeroMemory(&si,sizeof(si));
      si.cb=sizeof(si);

      std::string arg = assemblerPath;
      arg += " " + tempFileName + " -O" +tempObjFileName;

      CreateProcess( NULL, (char*)arg.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );

      CloseHandle(pi.hThread);

      WaitForSingleObject(pi.hProcess,INFINITE);

      CloseHandle(pi.hProcess);
    }
  };

  if ( shaderTypeName == "vs" )
  {
    DWORD processId = GetCurrentProcessId();

    char tempName[64];
    memset( tempName, 0, sizeof(tempName) );
    sprintf_s( tempName, "temp%d/", processId );

    std::string tempDir = modulePath + tempName;
    std::string tempVsFileName = tempDir + "temp.vsh";
    std::string tempVsObjFileName = tempDir + "temp.vsh.o";
    std::string tempVsBinFileName = tempDir + "temp.vsh.shbin";
    std::string tempGsFileName = tempDir + "temp.gsh";
    std::string tempGsObjFileName = tempDir + "temp.gsh.o";

    {//コンパイル
      LocalFunc::CreateCtrShaderFile( m_Source[Type::Vertex], tempVsFileName );
      LocalFunc::ShaderAssembler( tempVsFileName, tempVsObjFileName );

      if ( m_Source[Type::Geometry].length() )
      {
        LocalFunc::CreateCtrShaderFile( m_Source[Type::Geometry], tempGsFileName );
        LocalFunc::ShaderAssembler( tempGsFileName, tempGsObjFileName );
      }

      {//リンク
        std::string linkerPath = "ctr_VertexShaderLinker32.exe";
        STARTUPINFO si;// スタートアップ情報
        PROCESS_INFORMATION pi;// プロセス情報

        ZeroMemory(&si,sizeof(si));
        si.cb=sizeof(si);

        std::string arg = linkerPath;
        arg += " " + tempVsObjFileName;
        if ( m_Source[Type::Geometry].length() )
          arg += " " + tempGsObjFileName;
        arg += " -O" +tempVsBinFileName;

        CreateProcess( NULL, (char*)arg.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );

        CloseHandle(pi.hThread);

        WaitForSingleObject(pi.hProcess,INFINITE);

        CloseHandle(pi.hProcess);
      }
    }

    {//コマンド作成
      nn::gr::CTR::ShaderLite shaderLite;
      u32 command[1024*10];
      s32 size;
      s32 geometryShaderIndex = ( m_Source[Type::Geometry].length() ) ? 1 : -1;

      char *pShaderBin = ShaderCompiler::ReadFile( tempVsBinFileName.c_str(), &size, 1 );

      if ( m_Source[Type::Geometry].length() )
        shaderLite.SetDrawMode( PICA_DATA_DRAW_TRIANGLE_STRIP );

      u32 workSize = shaderLite.GetRequiredBufferSize( pShaderBin );
      u32* pWork = new u32[workSize];

      memset( command, 0, sizeof(command) );
      memset( pWork, 0, sizeof(u32) * workSize );

      shaderLite.SetupBinary( pShaderBin, 0, geometryShaderIndex, pWork );

      u32 *pTail;
      u32 length;

      u32 dummyCommand[] =  { 0x00000000, 0x00000000 };
      u32 returnCommand[] = { 0x00000001, 0x000f023d };
      u32 commandHeader[4] = { 0, 0, 0, 0 };

      {
        pTail = shaderLite.MakeFullCommand( command );
        length = (u32)pTail - (u32)command;

        if ( (length % 16) == 0 )
        {
          commandHeader[0] = length + 16;

          commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
          commandBinaryMaker.Write( command, length );

          commandBinaryMaker.Write( dummyCommand, sizeof(dummyCommand) );
          commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
        }
        else
        {
          commandHeader[0] = length + 8;

          commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
          commandBinaryMaker.Write( command, length );

          commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
        }
      }

      {
        pTail = shaderLite.MakeShaderCommand( command, true );
        length = (u32)pTail - (u32)command;

        if ( (length % 16) == 0 )
        {
          commandHeader[0] = length + 16;

          commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
          commandBinaryMaker.Write( command, length );

          commandBinaryMaker.Write( dummyCommand, sizeof(dummyCommand) );
          commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
        }
        else
        {
          commandHeader[0] = length + 8;

          commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
          commandBinaryMaker.Write( command, length );

          commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
        }
      }

      {
        pTail = shaderLite.MakeShaderCommand( command, false );
        length = (u32)pTail - (u32)command;

        if ( (length % 16) == 0 )
        {
          commandHeader[0] = length + 16;

          commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
          commandBinaryMaker.Write( command, length );

          commandBinaryMaker.Write( dummyCommand, sizeof(dummyCommand) );
          commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
        }
        else
        {
          commandHeader[0] = length + 8;

          commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
          commandBinaryMaker.Write( command, length );

          commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
        }
      }

      delete[] pShaderBin;
      delete[] pWork;
    }

    ParserUtil::RemoveFile( tempVsFileName.c_str() );
    ParserUtil::RemoveFile( tempVsObjFileName.c_str() );
    ParserUtil::RemoveFile( tempVsBinFileName.c_str() );
    RemoveDirectory( tempDir.c_str() );
  }
  else
  {
    std::string fshCode;

    {//コードを抜き出し
      std::vector<std::string> codeList;
      codeList = ParserUtil::SplitStr( m_Source[Type::Pixel], "//<ctr_code_start>" );
      codeList = ParserUtil::SplitStr( codeList[1], "//<ctr_code_end>" );

      fshCode = codeList[0];

      ParserUtil::ReplaceStr( fshCode, "/<ctr_code_start>", "" );
    }

    this->parse< parse_no_utf8 >( (char*)fshCode.c_str() );

    const xml_node<>* top_node = first_node("GfModel");

    scenegraph::resource::MaterialResourceNode::AttributeParam	m_AttributeParam;

    {// カラーの取得
      xml_node<>* colors_node = first_node( "Colors" );
      if (colors_node)
      {
        xml_node<>* constantcolor_node = colors_node->first_node( "ConstantColor" );
        gfx::Color f32Color;

        if (constantcolor_node)
        {
          for(s32 i = 0; i < CombinerAttribute::CombinerMax; ++i)
          {
            const char* attr_name = "ConstantColor";
            char attr_buff[64];
            sprintf_s( attr_buff, "%s%d", attr_name, i );
            xml_node<>* emission_node = constantcolor_node->first_node( attr_buff );
            f32Color.x = static_cast<float>( atof( emission_node->first_attribute( "R" )->value() ) );
            f32Color.y = static_cast<float>( atof( emission_node->first_attribute( "G" )->value() ) );
            f32Color.z = static_cast<float>( atof( emission_node->first_attribute( "B" )->value() ) );
            f32Color.w = static_cast<float>( atof( emission_node->first_attribute( "A" )->value() ) );
            m_AttributeParam.m_ConstantColor[i] = f32Color;
          }
        }
        xml_node<>* etccolor_node = colors_node->first_node( "EtcColor" );
        if (etccolor_node)
        {
          xml_node<>* blendcolor_node = etccolor_node->first_node( "BlendColor" );
          f32Color.x = static_cast<float>( atof( blendcolor_node->first_attribute( "R" )->value() ) );
          f32Color.y = static_cast<float>( atof( blendcolor_node->first_attribute( "G" )->value() ) );
          f32Color.z = static_cast<float>( atof( blendcolor_node->first_attribute( "B" )->value() ) );
          f32Color.w = static_cast<float>( atof( blendcolor_node->first_attribute( "A" )->value() ) );
          m_AttributeParam.m_EtcColor.m_BlendColor = f32Color;

          xml_node<>* buffercolor_node = etccolor_node->first_node( "BufferColor" );
          f32Color.x = static_cast<float>( atof( buffercolor_node->first_attribute( "R" )->value() ) );
          f32Color.y = static_cast<float>( atof( buffercolor_node->first_attribute( "G" )->value() ) );
          f32Color.z = static_cast<float>( atof( buffercolor_node->first_attribute( "B" )->value() ) );
          f32Color.w = static_cast<float>( atof( buffercolor_node->first_attribute( "A" )->value() ) );
          m_AttributeParam.m_EtcColor.m_BufferColor = f32Color;

          for( s32 i = 0; i < CombinerAttribute::TextureMax; ++i )
          {
            char node_name[64] = {0};
            sprintf_s( node_name, "TextureBorderColor%d", i );
            xml_node<>* texturebordercolor_node = etccolor_node->first_node( node_name );
            f32Color.x = static_cast<float>( atof( texturebordercolor_node->first_attribute( "R" )->value() ) );
            f32Color.y = static_cast<float>( atof( texturebordercolor_node->first_attribute( "G" )->value() ) );
            f32Color.z = static_cast<float>( atof( texturebordercolor_node->first_attribute( "B" )->value() ) );
            f32Color.w = static_cast<float>( atof( texturebordercolor_node->first_attribute( "A" )->value() ) );
            m_AttributeParam.m_EtcColor.m_TextureBorderColor[i] = f32Color;
          }
        }
      }
    }

    // テクスチャコンバイナの項目の取得
    const xml_node<>* texturecombiners_node = first_node( "TextureCombiners" );
    if (texturecombiners_node)
    {
      for ( s32 i = 0; i < CombinerAttribute::CombinerMax; ++i)
      {
        char node_name[64] = {0};
        sprintf_s( node_name, "TextureCombiner%d", i );
        const xml_node<>* texturecombiner_node = texturecombiners_node->first_node( node_name );
        m_AttributeParam.m_TextureCombinerConstant[i] = static_cast<s16>( atoi( texturecombiner_node->first_attribute( "ConstantColorNo" )->value() ) );
        const char*	child_node_name[CombinerColorType::NumberOf] = { "Color", "Alpha" };
        for ( s32 j = 0; j < CombinerColorType::NumberOf; ++j )
        {
          const xml_node<>* combiner_node = texturecombiner_node->first_node( child_node_name[j] );
          m_AttributeParam.m_TextureCombinerAttribute[j][i].m_MathFormula = GetMathFormulaTypeFromAttributeName(combiner_node->first_attribute( "MathFormula" )->value() );
          m_AttributeParam.m_TextureCombinerAttribute[j][i].m_Scale = static_cast<float>( atof( combiner_node->first_attribute( "Scale" )->value() ) );
          for ( s32 type = 0; type < InputSourceType::NumberOf; ++type )
          {
            char source_attribute[64] = {0};
            char operand_attribute[64] = {0};
            sprintf_s(source_attribute, "Source%c", 'A' + type);
            sprintf_s(operand_attribute, "Operand%c", 'A' + type);
            m_AttributeParam.m_TextureCombinerAttribute[j][i].m_InputSource[type] = GetCombinerInputSourceFromAttributeName( combiner_node->first_attribute( source_attribute )->value() );
            m_AttributeParam.m_TextureCombinerAttribute[j][i].m_Operand[type] = GetCombinerOperandFromAttributeName( combiner_node->first_attribute( operand_attribute )->value() );
          }
          m_AttributeParam.m_TextureCombinerAttribute[j][i].m_OutPutBuffer = ( atoi( combiner_node->first_attribute( "OutputBuffer" )->value() ) != 0 );
        }
      }
    }

    {//コンバイナ計算式書き込み
      u32 pixelShaderCommand[1024];
      nn::gr::CTR::Combiner combiner;
      b32 bufferOut[CombinerColorType::NumberOf] = { false, false };

      for( int stageNo = 0; stageNo < CombinerAttribute::CombinerMax; ++stageNo )
      {
        nn::gr::CTR::Combiner::Stage &rStage = combiner.stage[stageNo];

        scenegraph::resource::MaterialResourceNode::TextureCombinerAttribute &rTextureCombinerAttribute = m_AttributeParam.m_TextureCombinerAttribute[CombinerColorType::RGB][stageNo];

        static const PicaDataTexEnvOperand s_OperandRGBTable[] = {
          PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_ONE_MINUS_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_ALPHA, PICA_DATA_OPE_RGB_ONE_MINUS_SRC_ALPHA, PICA_DATA_OPE_RGB_SRC_R_DMP,
          PICA_DATA_OPE_RGB_ONE_MINUS_SRC_R_DMP, PICA_DATA_OPE_RGB_SRC_G_DMP, PICA_DATA_OPE_RGB_ONE_MINUS_SRC_G_DMP, PICA_DATA_OPE_RGB_SRC_B_DMP, PICA_DATA_OPE_RGB_ONE_MINUS_SRC_B_DMP,
        };

        static const PicaDataTexEnvOperand s_OperandATable[] = {
          PICA_DATA_OPE_RGB_SRC_COLOR,//選択されることはない。
          PICA_DATA_OPE_RGB_ONE_MINUS_SRC_COLOR,//選択されることはない。
          PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_R_DMP, PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_R_DMP,
          PICA_DATA_OPE_ALPHA_SRC_G_DMP, PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_G_DMP, PICA_DATA_OPE_ALPHA_SRC_B_DMP, PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_B_DMP
        };

        static const PicaDataTexEnvSrc s_SourceTable[] = {
          PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1, PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2, PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE3,
          PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP,
          PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_SECONDARY_COLOR_DMP, PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS, PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP
        };

        static const PicaDataTexEnvScale s_ScaleTable[] = {
          PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_2, PICA_DATA_TEX_ENV_SCALE_2, PICA_DATA_TEX_ENV_SCALE_4
        };

        static const PicaDataTexEnvCombine s_CombineTable[] = {
          PICA_DATA_TEX_ENV_COMBINE_REPLACE, PICA_DATA_TEX_ENV_COMBINE_MODULATE, PICA_DATA_TEX_ENV_COMBINE_ADD, PICA_DATA_TEX_ENV_COMBINE_ADD_SIGNED, PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE,
          PICA_DATA_TEX_ENV_COMBINE_SUBTRACT, PICA_DATA_TEX_ENV_COMBINE_DOT3_RGB, PICA_DATA_TEX_ENV_COMBINE_DOT3_RGBA, PICA_DATA_TEX_ENV_COMBINE_ADD_MULT_DMP, PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP
        };

        rStage.rgb.combine     = s_CombineTable[ rTextureCombinerAttribute.m_MathFormula ];
        rStage.rgb.operand[0]  = s_OperandRGBTable[ rTextureCombinerAttribute.m_Operand[InputSourceType::A] ];
        rStage.rgb.operand[1]  = s_OperandRGBTable[ rTextureCombinerAttribute.m_Operand[InputSourceType::B] ];
        rStage.rgb.operand[2]  = s_OperandRGBTable[ rTextureCombinerAttribute.m_Operand[InputSourceType::C] ];
        rStage.rgb.source[0]   = s_SourceTable[ rTextureCombinerAttribute.m_InputSource[InputSourceType::A] ];
        rStage.rgb.source[1]   = s_SourceTable[ rTextureCombinerAttribute.m_InputSource[InputSourceType::B] ];
        rStage.rgb.source[2]   = s_SourceTable[ rTextureCombinerAttribute.m_InputSource[InputSourceType::C] ];
        rStage.rgb.scale       = s_ScaleTable[ static_cast<u32>(rTextureCombinerAttribute.m_Scale) ];
        rStage.rgb.bufferInput = bufferOut[CombinerColorType::RGB] ? PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS : PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
        bufferOut[CombinerColorType::RGB] = rTextureCombinerAttribute.m_OutPutBuffer;

        rTextureCombinerAttribute = m_AttributeParam.m_TextureCombinerAttribute[CombinerColorType::A][stageNo];

        rStage.alpha.combine     = s_CombineTable[ rTextureCombinerAttribute.m_MathFormula ];
        rStage.alpha.operand[0]  = s_OperandATable[ rTextureCombinerAttribute.m_Operand[InputSourceType::A] ];
        rStage.alpha.operand[1]  = s_OperandATable[ rTextureCombinerAttribute.m_Operand[InputSourceType::B] ];
        rStage.alpha.operand[2]  = s_OperandATable[ rTextureCombinerAttribute.m_Operand[InputSourceType::C] ];
        rStage.alpha.source[0]   = s_SourceTable[ rTextureCombinerAttribute.m_InputSource[InputSourceType::A] ];
        rStage.alpha.source[1]   = s_SourceTable[ rTextureCombinerAttribute.m_InputSource[InputSourceType::B] ];
        rStage.alpha.source[2]   = s_SourceTable[ rTextureCombinerAttribute.m_InputSource[InputSourceType::C] ];
        rStage.alpha.scale       = s_ScaleTable[ static_cast<u32>(rTextureCombinerAttribute.m_Scale) ];
        rStage.alpha.bufferInput = bufferOut[CombinerColorType::A] ? PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS : PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
        bufferOut[CombinerColorType::A] = rTextureCombinerAttribute.m_OutPutBuffer;

        {
          s16 constantColorIndex = m_AttributeParam.m_TextureCombinerConstant[stageNo];
          math::Vector4 constantColor = m_AttributeParam.m_ConstantColor[constantColorIndex].GetColor();
          constantColor *= 255.0f;

          rStage.constColorR = static_cast<u32>( constantColor[0] + 0.5f );
          rStage.constColorG = static_cast<u32>( constantColor[1] + 0.5f );
          rStage.constColorB = static_cast<u32>( constantColor[2] + 0.5f );
          rStage.constColorA = static_cast<u32>( constantColor[3] + 0.5f );
        }
      }

      {
        math::Vector4 bufferColor = m_AttributeParam.m_EtcColor.m_BufferColor.GetColor();
        bufferColor *= 255.0f;

        combiner.bufferColorR = static_cast<u32>( bufferColor[0] + 0.5f );
        combiner.bufferColorG = static_cast<u32>( bufferColor[1] + 0.5f );
        combiner.bufferColorB = static_cast<u32>( bufferColor[2] + 0.5f );
        combiner.bufferColorA = static_cast<u32>( bufferColor[3] + 0.5f );
      }

      u32 *pTail = combiner.MakeCommand(pixelShaderCommand);
      u32 length = (u32)pTail - (u32)pixelShaderCommand;
      commandBinaryMaker.Write( pixelShaderCommand, length );
      assert( (commandBinaryMaker.GetBinarySize() % 8) == 0 );

      u32 dummyCommand[] =  { 0x00000000, 0x00000000 };
      u32 returnCommand[] = { 0x00000001, 0x000f023d };

      {
        u32 length = commandBinaryMaker.GetBinarySize();

        if ( (length % 16) == 0 )
        {
          commandBinaryMaker.Write( dummyCommand, sizeof(dummyCommand) );
          commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
        }
        else
        {
          commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
        }
      }
    }
  }

  {//----バイナリ生成-----
    {//ヘッダー
      FileHeader fileHeader;

      memset( &fileHeader, 0, sizeof(fileHeader) );

      memset( fileHeader.shaderName, 0, sizeof(fileHeader.shaderName) );
      strcat_s( fileHeader.shaderName, m_ShaderName[type].c_str() );

      util::FnvHash hash( reinterpret_cast<const u8*>(m_ShaderName[type].c_str()) );
      fileHeader.shaderNameHash = hash.value;
      fileHeader.shaderNum = shaderNum;

      m_BinaryMaker.Write( &fileHeader, sizeof(fileHeader) );
    }

    for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
    {
      u8* pData = commandBinaryMaker.CreateFixedBuffer();

      {
        ShaderHeader header;

        memset( &header, 0, sizeof(header) );

        header.size = commandBinaryMaker.GetBinarySize();

        util::FnvHash hash( pData, header.size );

        if ( shaderTypeName == "vs" )
          header.shaderType = Type::Vertex;
        else
          header.shaderType = Type::Pixel;

        header.dataHash = hash.value;
        memset( header.path, 0, sizeof(header.path) );
        ParserUtil::GetFileName( header.path, m_FileName.c_str() );

        m_BinaryMaker.Write( &header, sizeof(header) );
      }

      m_BinaryMaker.Write(pData, commandBinaryMaker.GetBinarySize());
      delete [] pData;
    }
  }
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::ReadFile( Type type, std::string filePath )
{
  int size;
  char* pSrc = ShaderCompiler::ReadFile( filePath.c_str(), &size, 1 );
  m_FileName = filePath;
  m_ShaderName[type] = ParserUtil::GetFileNameStr( filePath.c_str() );

  m_ShaderName[type] = ParserUtil::SplitStr( m_ShaderName[type], "." )[0];

  if ( pSrc )
  {
    SetSource( type, pSrc );
    delete[] pSrc;
  }
  else
  {
    std::cerr << "Error: failed to open input file: " << filePath << std::endl;
    exit(1);
  }
}

//-----------------------------------------------------------------------------
/// @brief	リードファイル関数
//-----------------------------------------------------------------------------
char* ShaderCompiler::ReadFile( const char* filename, int* p_size, int align )
{
  FILE *hFile;
  int file_size;
  char *read_buff;

  fopen_s( &hFile, filename, "rb" );

  if ( hFile == NULL ){
    return NULL;
  }

  fseek( hFile, 0, SEEK_END );
  file_size = ftell( hFile );
  *p_size = file_size;
  fseek( hFile, 0, SEEK_SET );

  file_size += align;//必ずNULLが入るようにしたい。
  int buf_size = file_size + ((-file_size) & (align-1));

  read_buff = new char[buf_size];
  memset( read_buff, 0, buf_size );
  fread( read_buff, file_size, 1, hFile );
  fclose( hFile );

  return( read_buff );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::WriteFile( std::string filePath )
{
  u8* pData = m_BinaryMaker.CreateFixedBuffer();
  u32 binarySize = m_BinaryMaker.GetBinarySize();

  static const u32 sc_Version = GfModelVersion::PokeShaderLutLightingFix;
  u32 dataCount = 1;

  {
    BinaryMaker bm;

    // バージョン
    bm.Write(&sc_Version, sizeof(sc_Version));

    // データ数
    bm.Write(&dataCount, sizeof(dataCount));

    //16バイトアライン
    {
      u32 padding[2] = {0, 0};
      bm.Write( &padding, sizeof(padding) );
    }

    //
    GfBinaryMdlData::ChunkHeader modelHeader(IResourceData::Type("shader").m_ID, binarySize);
    bm.Write(&modelHeader, sizeof(modelHeader));

    bm.Write(pData, binarySize);

    u8* pOutData = bm.CreateFixedBuffer();
    u32 outSize = bm.GetBinarySize();

    ShaderCompiler::WriteFile( filePath.c_str(), pOutData, outSize );

    delete [] pOutData;
  }

  delete [] pData;
}

//-----------------------------------------------------------------------------
/// @brief	ファイル生成関数
//-----------------------------------------------------------------------------
void ShaderCompiler::WriteFile( const char* filename, const void* p_data, int size )
{
  FILE *hFile;

  char filePath[ParserUtil::GFL_MAX_FILEPATH];

  ShaderCompiler::GetFilePath( filePath, filename );

  if(fopen_s( &hFile, filename, "wb" ) != 0)
  {
    std::cerr << "Error: failed to open output file: " << filename << std::endl;
    exit(1);
  }
  fwrite( p_data, 1, size, hFile );
  fclose( hFile );
}

//-----------------------------------------------------------------------------
/// @brief	ファイルパスの取得
//-----------------------------------------------------------------------------
void ShaderCompiler::GetFilePath( char* p_out, const char* filename )
{
  int size;

  size = (int)strlen( filename );

  while( size ){
    if ( filename[size] == '/' )  break;
    if ( filename[size] == '\\' ) break;
    --size;
  }

  if ( size )
  {
    memcpy( p_out, filename, size + 1 );
    p_out[ size + 1 ] = 0;//term code
  }
}

MathFormulaType ShaderCompiler::GetMathFormulaTypeFromAttributeName( const char* name )
{
  const char* attributeNames[] =
  {
    "Replace",
    "Modulate",
    "Add",
    "AddSigned",
    "Interpolate",
    "Subtract",
    "DotProduct3Rgb",
    "DotProduct3Rgba",
    "AddMult",
    "MultAdd",
    "NumberOf"
  };

  for(u32 i = 0; i < MathFormulaType::NumberOf; ++i)
  {
    if(strcmp(attributeNames[i], name) == 0)
    {
      return static_cast<MathFormulaType>(i);
    }
  }
  assert(0 && "Invalid attribute.");
  return MathFormulaType::NumberOf;
}

s16 ShaderCompiler::GetCombinerInputSourceFromAttributeName( const char* name )
{
  const char* attributeNames[] = {
    "Texture0",
    "Texture1",
    "Texture2",
    "Texture3", // !< 未使用(プロシージャル用)
    "Constant",
    "PrimaryColor",
    "FragmentPrimaryColor",
    "FragmentSecondaryColor",
    "Previous",
    "PreviousBuffer",
  };

  s16 length = static_cast<s16>(sizeof(attributeNames) / sizeof(attributeNames[0]));
  for(s16 i = 0; i < length; ++i)
  {
    if(strcmp(attributeNames[i], name) == 0)
    {
      return i;
    }
  }
  assert(0 && "Invalid attribute.");
  return length;
}

s16 ShaderCompiler::GetCombinerOperandFromAttributeName( const char* name )
{
  const char* attributeNames[] = {
    "Color",
    "OneMinusColor",
    "Alpha",
    "OneMinusAlpha",
    "Red",
    "OneMinusRed",
    "Green",
    "OneMinusGreen",
    "Blue",
    "OneMinusBlue",
  };

  s16 length = static_cast<s16>(sizeof(attributeNames) / sizeof(attributeNames[0]));
  for(s16 i = 0; i < length; ++i)
  {
    if(strcmp(attributeNames[i], name) == 0)
    {
      return i;
    }
  }
  assert(0 && "Invalid attribute.");
  return length;
}
