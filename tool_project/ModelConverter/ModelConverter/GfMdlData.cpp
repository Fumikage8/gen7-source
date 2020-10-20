#if defined(WIN32)//Windowsでしか提供するつもりなし

//なのでSTL使っちゃう！わーい。けど、このcpp内にとどめる。
#include <vector>
#include <string>
#include <deque>



#include "GfMdlData.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")  

#include <d3d11.h>
#include <d3dcompiler.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNodeFactory.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
//#include <math/include/gfl2_math.h>

#include "CommandBuilder.h"
#include "gr_Combiner.h"
#include "gr_RenderState.h"
#include "gr_Texture.h"
#include "gr_LookUpTable.h"
//#include "Nw4cImageData.h"

#include<gfx/include/gfl2_ImageData.h>
#include<debug/include/gfl2_Assert.h>

#include <regex>  // 正規表現 @todo ikuta_junya追加

#include <iostream>
#include <fstream>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

using namespace gfl2;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;
using namespace gfl2::renderingengine::scenegraph::resource;

#define ERR_RETURN( check )       { if( check ) {}else{return false;} }
//#define ERR_RETURN( check )       { if( check ) {} }

namespace ModelConverter {

  class StringUtil{
  public:
    static std::wstring ShitJIStoUTF16( const std::string& str )
    {
      int len = ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, NULL, 0 );

      if ( len > 0 ) { 
        std::vector<wchar_t> utf16( len );

        if ( ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, &utf16[0], len ) ){
          return std::wstring( &utf16[0] );
        }
      }

      return NULL;
    }

    static std::string GetRelativePath( std::string filePath, std::string fileDir )
    {
      std::string fileName = ParserUtil::GetFileNameStr( filePath.c_str() );
      ParserUtil::ReplaceStr( filePath, "\\", "/" );
      std::vector<std::string> dirNames = ParserUtil::SplitStr( filePath, "/" );
      std::string textureDirName = "";

      if ( dirNames.size() >= 2 )
      {
        textureDirName = dirNames[ dirNames.size() - 2 ];
        textureDirName += "/";
      }

      filePath = fileDir + textureDirName + fileName;

      return filePath;
    }
  };

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  TextureData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  void TextureData::ReadTextureDataNode( const xml_node<>* top_node, std::string fileDir )
  {
    m_No = atoi( top_node->first_attribute("No")->value() );

    const xml_node<>* path_node = top_node->first_node("FilePath");

    m_FilePath = path_node->value();

    if ( fileDir.length())
      m_FilePath = StringUtil::GetRelativePath( m_FilePath, fileDir );
  }

  void TextureData::CreateBinary()
  {
    BinaryMaker binary;

    struct ImageHeader
    {//16バイトアラインサイズ
      s32 size;
      s32 dumy[3];
      c8  path[PATH_MAX];
    };

    ImageHeader header;

    memset( header.path, 0, sizeof(header.path) );
    ParserUtil::GetFileName( header.path, m_FilePath.c_str() );

    c8* pImageData = ParserUtil::ReadFile( m_FilePath.c_str(), &header.size );

    if ( pImageData == NULL )
    {
      ::OutputDebugStringA( "====================== renderingengine Warning： cannot find");
      ::OutputDebugStringA( m_FilePath.c_str() );
      ::OutputDebugStringA( ". ModelConverter assigns dummy texture. ======================\n" );

      GF_ALIGNED(0x80) static unsigned char  s_Dummy_tga[] = {
#include "R:/home/niji/git_program/gflib2/RenderingEngine/Resource/Dummy.tga.txt"
        0x00
      };

      pImageData = reinterpret_cast<c8*>(s_Dummy_tga);
      header.size = sizeof(s_Dummy_tga);
      binary.Write( &header, sizeof(header) );
      binary.Write( pImageData, header.size );

      m_pBinaryData = binary.CreateFixedBuffer();
      m_BinarySize = binary.GetBinarySize();
      m_Hash = util::FnvHash(reinterpret_cast<const u8*>(pImageData));
    }
    else
    {
      binary.Write( &header, sizeof(header) );
      binary.Write( pImageData, header.size );

      m_pBinaryData = binary.CreateFixedBuffer();
      m_BinarySize = binary.GetBinarySize();
      m_Hash = util::FnvHash(reinterpret_cast<const u8*>(pImageData));

      delete[] pImageData;
      pImageData = NULL;
    }

  }

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  ShaderData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  void ShaderData::ReadShaderDataNode( const xml_node<>* top_node, std::string fileDir )
  {
    m_No = atoi( top_node->first_attribute("No")->value() );
    {
      m_ShaderName = top_node->first_attribute("Name")->value();
      std::wstring utf16 = ParserUtil::UTF8toUTF16( m_ShaderName );
      m_ShaderName = ParserUtil::UTF16toShitJIS( utf16 );
    }

    {
      const xml_node<>* path_node = top_node->first_node("VshFilePath");

      if ( path_node )
      {
        m_VsFilePath = path_node->value();

        std::wstring utf16 = ParserUtil::UTF8toUTF16( m_VsFilePath );
        m_VsFilePath = ParserUtil::UTF16toShitJIS( utf16 );
        if ( fileDir.length())
          m_VsFilePath = StringUtil::GetRelativePath( m_VsFilePath, fileDir );
      }
    }

    {
      const xml_node<>* path_node = top_node->first_node("FshFilePath");

      if ( path_node )
      {
        m_FsFilePath = path_node->value();

        std::wstring utf16 = ParserUtil::UTF8toUTF16( m_FsFilePath );
        m_FsFilePath = ParserUtil::UTF16toShitJIS( utf16 );
        if ( fileDir.length())
          m_FsFilePath = StringUtil::GetRelativePath( m_FsFilePath, fileDir );
      }
    }

    {
      const xml_node<>* code_node = top_node->first_node("VshCode");

      if ( code_node )
      {
        m_VsCode = code_node->value();

        std::wstring utf16 = ParserUtil::UTF8toUTF16( m_VsCode );
        m_VsCode = ParserUtil::UTF16toShitJIS( utf16 );
      }
    }

    {
      const xml_node<>* code_node = top_node->first_node("FshCode");

      if ( code_node )
      {
        m_FsCode = code_node->value();

        std::wstring utf16 = ParserUtil::UTF8toUTF16( m_FsCode );
        m_FsCode = ParserUtil::UTF16toShitJIS( utf16 );
      }
    }

    //CreateBinary();
  }

  void ShaderData::CreateBinary()
  {
    m_pBinaryData = NULL;
    m_BinarySize = 0;

    if ( m_VsCode.length() == 0 ) return;
    if ( m_FsCode.length() == 0 ) return;

    std::string shaderFilePath = "../ShaderCompiler/";
    std::string vsFilePath = shaderFilePath + m_ShaderName + ".gfvsh";
    std::string fsFilePath = shaderFilePath + m_ShaderName + ".gffsh";
    std::string vsBinFilePath = vsFilePath + ".windx";
    std::string fsBinFilePath = fsFilePath + ".windx";

    ParserUtil::FileCreate( vsFilePath.c_str(), m_VsCode.c_str(), static_cast<s32>(m_VsCode.length()) );
    ParserUtil::FileCreate( fsFilePath.c_str(), m_FsCode.c_str(), static_cast<s32>(m_FsCode.length()) );

    std::string compilerPath = shaderFilePath + "ShaderCompiler.exe";

    {
      std::string option = "-vsh \"<VS>\" -fsh \"<FS>\" -o \"<OUT>\" -def HLSL -def 1";

      ParserUtil::ReplaceStr( option, "<VS>", vsFilePath );
      ParserUtil::ReplaceStr( option, "<FS>", fsFilePath );
      ParserUtil::ReplaceStr( option, "<OUT>", vsBinFilePath );

      std::string arg = compilerPath + " " + option;

      STARTUPINFO si;// スタートアップ情報
      PROCESS_INFORMATION pi;// プロセス情報

      ZeroMemory(&si,sizeof(si));
      si.cb=sizeof(si);

      CreateProcess( NULL, (char*)arg.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );

      CloseHandle(pi.hThread);

      WaitForSingleObject(pi.hProcess,INFINITE);

      CloseHandle(pi.hProcess);
    }

    {
      std::string option = "-vsh \"<VS>\" -fsh \"<FS>\" -o \"<OUT>\" -def HLSL -def 1";

      ParserUtil::ReplaceStr( option, "<VS>", vsFilePath );
      ParserUtil::ReplaceStr( option, "<FS>", fsFilePath );
      ParserUtil::ReplaceStr( option, "<OUT>", fsBinFilePath );

      std::string arg = compilerPath + " " + option;

      STARTUPINFO si;// スタートアップ情報
      PROCESS_INFORMATION pi;// プロセス情報

      ZeroMemory(&si,sizeof(si));
      si.cb=sizeof(si);

      CreateProcess( NULL, (char*)arg.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );

      CloseHandle(pi.hThread);

      WaitForSingleObject(pi.hProcess,INFINITE);

      CloseHandle(pi.hProcess);
    }

    BinaryMaker binary;

    {
      s32 size;
      c8* pData = ParserUtil::ReadFile( vsBinFilePath.c_str(), &size );
      s32 dataHeaderSize = sizeof(s32) + sizeof(s32);

      GfBinaryMdlData::ChunkHeader *pHeader = (GfBinaryMdlData::ChunkHeader*)&pData[dataHeaderSize];

      binary.Write( pHeader, pHeader->m_Size );
      delete[] pData;

      ParserUtil::RemoveFile( vsFilePath.c_str() );
      ParserUtil::RemoveFile( vsBinFilePath.c_str() );
    }

    {
      s32 size;
      c8* pData = ParserUtil::ReadFile( fsBinFilePath.c_str(), &size );
      s32 dataHeaderSize = sizeof(s32) + sizeof(s32);

      GfBinaryMdlData::ChunkHeader *pHeader = (GfBinaryMdlData::ChunkHeader*)&pData[dataHeaderSize];

      binary.Write( pHeader, pHeader->m_Size );
      delete[] pData;

      ParserUtil::RemoveFile( fsFilePath.c_str() );
      ParserUtil::RemoveFile( fsBinFilePath.c_str() );
    }

    m_pBinaryData = binary.CreateFixedBuffer();
    m_BinarySize = binary.GetBinarySize();
  }

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  ModelData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  ModelData::ModelData():
    m_pBinaryData(NULL)
  {
    memset(&m_ModelUserData, 0, sizeof(m_ModelUserData));
  }

  void ModelData::ReadModelDataNode( const xml_node<>* top_node )
  {
    for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
    {
      std::string nodeName = node->name();

      if ( nodeName == "ModelAABB" )
      {
        std::vector<math::Vector4> aabb;
        ParserUtil::ParseFloatArray( &aabb, node, 3, 0.0f );
        m_AABB.SetMin( aabb[0] );
        m_AABB.SetMax( aabb[1] );
      }
      else if ( nodeName == "ModelTransform" )
      {
        const xml_node<>* pGlobalMatrixNode = node->first_node("GlobalMatrix");

        if ( pGlobalMatrixNode )
        {
          std::vector<math::Vector4> mat;
          ParserUtil::ParseFloatArray( &mat, pGlobalMatrixNode, 4, 0.0f );

          for( int i = 0; i < 4; ++i )
            m_TransMat[i] = mat[i];
        }
      }
      else if ( nodeName == "UserData" )
      {
        xml_attribute<>* billboardType_attribute = node->first_attribute("BillboardType");
        if ( billboardType_attribute )
        {
          m_ModelUserData.m_BillboardType = static_cast<BillboardType::Enum>( std::atoi( billboardType_attribute->value() ) );
        }

        const xml_node<>* pKusaPositionNode = node->first_node("KusaPosition");

        if ( pKusaPositionNode )
        {
          ParserUtil::ParseFloatArray( &m_KusaPositionList, pKusaPositionNode, 3, 0.0f );
        }
      }
      else if ( nodeName == "TextureList" )
      {
        for( const xml_node<>* texture_node = node->first_node(); texture_node; texture_node = texture_node->next_sibling( ) )
        {
          TextureLinkData linkData;
          std::string textureNodeName = texture_node->name();

          linkData.m_No = std::atoi( texture_node->first_attribute( "No" )->value() );
          linkData.m_FilePath = texture_node->first_node("FilePath")->value();

          m_TextureList.push_back( linkData );
        }
      }
      if ( nodeName == "ShaderList" )
      {
        for( const xml_node<>* shader_node = node->first_node("Shader"); shader_node; shader_node = shader_node->next_sibling("Shader") )
        {
          std::string name = shader_node->first_attribute("Name")->value();
          std::wstring utf16 = ParserUtil::UTF8toUTF16( name );
          name = ParserUtil::UTF16toShitJIS( utf16 );
          m_ShaderList.push_back( name );
        }
      }
      else if ( nodeName == "Materials" )
      {
        for( const xml_node<>* material_node = node->first_node(); material_node; material_node = material_node->next_sibling( ) )
        {
          m_MaterialList.push_back( material_node->first_attribute( "Name" )->value() );
        }
      }
      else if ( nodeName == "Meshes" )
      {
        for( const xml_node<>* mesh_node = node->first_node(); mesh_node; mesh_node = mesh_node->next_sibling( ) )
        {
          std::string fullPath = mesh_node->first_attribute( "Name" )->value();
          u32 size = static_cast<u32>( fullPath.length() + 1 );
          c8 *pathBuffer = new c8[ size ];

          memset( pathBuffer, 0, size );
          ParserUtil::GetNodeName( pathBuffer, size, fullPath.c_str() );
          assert( strlen(pathBuffer) < PATH_MAX );

          m_MeshPathList.push_back( fullPath );
          m_MeshList.push_back( pathBuffer );

          delete[] pathBuffer;
        }
      }
    }
  }

  void ModelData::CreateBinary( const u8* pJointData, u32 jointDataSize )
  {
    BinaryMaker binary;

    {//Shaders情報
      u32 ShaderNum = static_cast<u32>( m_ShaderList.size() );
      binary.Write( &ShaderNum, sizeof( ShaderNum ) );

      for( u32 i = 0; i < m_ShaderList.size(); ++i )
      {
        util::FnvHash hash( reinterpret_cast<const u8*>(m_ShaderList[i].c_str()) );
        c8 *pathBuffer = new c8[ m_ShaderList[i].length() + 1];
        c8 nameBuffer[PATH_MAX];

        memset(pathBuffer,0,m_ShaderList[i].length() + 1);
        memset( nameBuffer, 0, sizeof(nameBuffer) );

        ParserUtil::GetFileName( pathBuffer, m_ShaderList[i].c_str() );
        assert( strlen(pathBuffer) < PATH_MAX );
        memcpy(nameBuffer,pathBuffer,strlen(pathBuffer)); // Garbage Data Fix

        binary.Write( &hash.value, sizeof( hash.value ) );
        binary.Write( nameBuffer, PATH_MAX );

        delete[] pathBuffer;
      }
    }

    {//TextureList情報
      u32 textureNum = static_cast<u32>( m_TextureList.size() );
      binary.Write( &textureNum, sizeof( textureNum ) );

      for( u32 i = 0; i < m_TextureList.size(); ++i )
      {
        util::FnvHash hash( reinterpret_cast<const u8*>(m_TextureList[i].m_FilePath.c_str()) );
        c8 *pathBuffer = new c8[ m_TextureList[i].m_FilePath.length() + 1];
        c8 nameBuffer[PATH_MAX];

        memset(pathBuffer,0,m_TextureList[i].m_FilePath.length() + 1);
        memset( nameBuffer, 0, sizeof(nameBuffer) );

        ParserUtil::GetFileName( pathBuffer, m_TextureList[i].m_FilePath.c_str() );
        assert( strlen(pathBuffer) < PATH_MAX );
        memcpy(nameBuffer,pathBuffer,strlen(pathBuffer)); // Garbage Data Fix


        //binary.Write( &m_TextureList[i].m_No, sizeof( m_TextureList[i].m_No ) );
        binary.Write( &hash.value, sizeof( hash.value ) );
        binary.Write( nameBuffer, PATH_MAX );

        delete[] pathBuffer;
      }
    }

    {//Materials情報
      u32 materialNum = static_cast<u32>( m_MaterialList.size() );
      binary.Write( &materialNum, sizeof( materialNum ) );

      for( u32 i = 0; i < m_MaterialList.size(); ++i )
      {
        util::FnvHash hash( reinterpret_cast<const u8*>(m_MaterialList[i].c_str()) );
        c8 *pathBuffer = new c8[ m_MaterialList[i].length() + 1];
        c8 nameBuffer[PATH_MAX];

        memset(pathBuffer,0,m_MaterialList[i].length() + 1);
        memset( nameBuffer, 0, sizeof(nameBuffer) );

        ParserUtil::GetFileName( pathBuffer, m_MaterialList[i].c_str() );
        assert( strlen(pathBuffer) < PATH_MAX );
        memcpy(nameBuffer,pathBuffer,strlen(pathBuffer)); // Garbage Data Fix


        binary.Write( &hash.value, sizeof( hash.value ) );
        binary.Write( nameBuffer, PATH_MAX );

        delete[] pathBuffer;
      }
    }

    {//Meshes情報
      u32 meshNum = static_cast<u32>( m_MeshList.size() );
      binary.Write( &meshNum, sizeof( meshNum ) );

      for( u32 i = 0; i < m_MeshList.size(); ++i )
      {
        util::FnvHash hash( reinterpret_cast<const u8*>(m_MeshList[i].c_str()) );
        c8 nameBuffer[PATH_MAX];

        memset( nameBuffer, 0, sizeof(nameBuffer) );
        memcpy(nameBuffer,m_MeshList[i].c_str(),strlen(m_MeshList[i].c_str())); // Garbage Data Fix


        binary.Write( &hash.value, sizeof( hash.value ) );
        binary.Write( nameBuffer, PATH_MAX );
      }
    }

    {//Model情報
      binary.Write( &m_AABB, sizeof( math::AABB ) );
      binary.Write( &m_TransMat, sizeof( math::Matrix ) );
    }

    {//ユーザーデータ情報
      GfMdlResourceNode::UserDataHeader userDataHeader;

      memset( &userDataHeader, 0, sizeof(userDataHeader) );

      userDataHeader.m_UserDataSize = m_KusaPositionList.size() * sizeof( float );
      userDataHeader.m_DefaultUserDataSize = sizeof(m_ModelUserData);

      binary.Write( &userDataHeader, sizeof( userDataHeader ) );

      if ( m_KusaPositionList.size() )
      {
        for( int i = 0; i < m_KusaPositionList.size(); ++i )
        {
          binary.Write( &m_KusaPositionList[i].y, sizeof( float ) );
        }
      }


      binary.Write( &m_ModelUserData, sizeof( m_ModelUserData ) );
    }

    if ( pJointData )
    {//Joint情報
      binary.Write( pJointData, jointDataSize );
    }

    m_pBinaryData = binary.CreateFixedBuffer();
    m_BinarySize = binary.GetBinarySize();
  }

  bool ModelData::CreateBinaryCTR( const u8* pJointData, u32 jointDataSize )
  {
    BinaryMaker binary;

    {//Shaders情報
      u32 ShaderNum = static_cast<u32>( m_ShaderList.size() );
      binary.Write( &ShaderNum, sizeof( ShaderNum ) );

      for( u32 i = 0; i < m_ShaderList.size(); ++i )
      {
        util::FnvHash hash( reinterpret_cast<const u8*>(m_ShaderList[i].c_str()) );
        c8 *pathBuffer = new c8[ m_ShaderList[i].length() + 1];
        c8 nameBuffer[PATH_MAX];

        memset( nameBuffer, 0, sizeof(nameBuffer) );

        ParserUtil::GetFileName( pathBuffer, m_ShaderList[i].c_str() );
        ERR_RETURN( strlen(pathBuffer) < PATH_MAX );
        strcat_s( nameBuffer, pathBuffer );

        binary.Write( &hash.value, sizeof( hash.value ) );

        delete[] pathBuffer;
      }
    }

    {//TextureList情報
      u32 textureNum = static_cast<u32>( m_TextureList.size() );
      binary.Write( &textureNum, sizeof( textureNum ) );

      for( u32 i = 0; i < m_TextureList.size(); ++i )
      {
        util::FnvHash hash( reinterpret_cast<const u8*>(m_TextureList[i].m_FilePath.c_str()) );
        c8 *pathBuffer = new c8[ m_TextureList[i].m_FilePath.length() + 1];
        c8 nameBuffer[PATH_MAX];

        memset( nameBuffer, 0, sizeof(nameBuffer) );

        ParserUtil::GetFileName( pathBuffer, m_TextureList[i].m_FilePath.c_str() );
        ERR_RETURN( strlen(pathBuffer) < PATH_MAX );
        strcat_s( nameBuffer, pathBuffer );

        binary.Write( &hash.value, sizeof( hash.value ) );

        delete[] pathBuffer;
      }
    }

    {//Materials情報
      u32 materialNum = static_cast<u32>( m_MaterialList.size() );
      binary.Write( &materialNum, sizeof( materialNum ) );

      for( u32 i = 0; i < m_MaterialList.size(); ++i )
      {
        util::FnvHash hash( reinterpret_cast<const u8*>(m_MaterialList[i].c_str()) );
        c8 *pathBuffer = new c8[ m_MaterialList[i].length() + 1];
        c8 nameBuffer[PATH_MAX];

        memset( nameBuffer, 0, sizeof(nameBuffer) );

        ParserUtil::GetFileName( pathBuffer, m_MaterialList[i].c_str() );
        ERR_RETURN( strlen(pathBuffer) < PATH_MAX );
        strcat_s( nameBuffer, pathBuffer );

        binary.Write( &hash.value, sizeof( hash.value ) );

        delete[] pathBuffer;
      }
    }

    {//Meshes情報
      u32 meshNum = static_cast<u32>( m_MeshList.size() );
      binary.Write( &meshNum, sizeof( meshNum ) );

      for( u32 i = 0; i < m_MeshList.size(); ++i )
      {
        util::FnvHash hash( reinterpret_cast<const u8*>(m_MeshList[i].c_str()) );
        c8 nameBuffer[PATH_MAX];

        memset( nameBuffer, 0, sizeof(nameBuffer) );
        strcat_s( nameBuffer, m_MeshList[i].c_str() );

        binary.Write( &hash.value, sizeof( hash.value ) );

      }
    }

    {//Model情報
      binary.Write( &m_AABB, sizeof( math::AABB ) );
    }

    if ( pJointData )
    {//Joint情報
      binary.Write( pJointData, jointDataSize );
    }

    {
      u32 paddingSize = 16 - ( binary.GetBinarySize() % 16 );
      u8 padding = 0;

      {
        char message[64] = {0};
        sprintf_s( message, "binary.GetBinarySize() = %d + %d\n", binary.GetBinarySize(), paddingSize );
        //OutputDebugStringA( message );
      }

      for( s32 i = 0; i < paddingSize; ++i )
        binary.Write( &padding, sizeof(padding) );
    }



    m_pBinaryData = binary.CreateFixedBuffer();
    m_BinarySize = binary.GetBinarySize();

    return true;
  }

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  MaterialData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  MaterialData::MaterialData()
  {
    m_Name = "";
    m_Type = "";
    m_TextureList.clear();
    memset( &m_Combiner, 0, sizeof(Combiner) );

    memset( &m_BlendTest, 0, sizeof(scenegraph_resource::MaterialResourceNode::BlendTest) );
    memset( &m_AttributeParam, 0, sizeof(scenegraph_resource::MaterialResourceNode::AttributeParam) );
    memset( &m_UserData, 0, sizeof(scenegraph_resource::MaterialResourceNode::UserData) );

    m_pBinaryData = NULL;
    m_BinarySize = 0;
    m_fUvSetAdjust = false;
  }

  inline bool  MaterialData::operator ==( const MaterialData& rhs ) const
  {
    if ( this->m_TextureList.size() != rhs.m_TextureList.size() ) return false;

    for( int i = 0; i < m_TextureList.size(); ++i )
    {
      if ( m_TextureList[i].m_SlotNo != rhs.m_TextureList[i].m_SlotNo ) return false;
    }

    if ( !(this->m_BlendTest == rhs.m_BlendTest) ) return false;

    {
      const u8* pSeek = reinterpret_cast<const u8*>( &m_AttributeParam );
      const u8* pSeekRhs = reinterpret_cast<const u8*>( &rhs.m_AttributeParam );

      for( int i = 0; i < sizeof(scenegraph_resource::MaterialResourceNode::AttributeParam); ++i )
      {
        if ( pSeek[i] != pSeekRhs[i] )
          return false;
      }
    }

    {
      const u8* pSeek = reinterpret_cast<const u8*>( &m_UserData );
      const u8* pSeekRhs = reinterpret_cast<const u8*>( &rhs.m_UserData );

      for( int i = 0; i < sizeof(scenegraph_resource::MaterialResourceNode::UserData); ++i )
      {
        if ( pSeek[i] != pSeekRhs[i] )
          return false;
      }
    }

    return true;
  }

  gfx::CompareFunc MaterialData::GetAlphaTestTypeFromAttributeName(const char* name)
  {
    const char* attributeNames[] = 
    {
      "LESS",
      "LEQUAL",
      "EQUAL",
      "GEQUAL",
      "GREATER",
      "NOTEQUAL",
      "ALWAYS",
      "NEVER",
    };
    for(u32 i = 0; i < gfx::CompareFunc::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<gfx::CompareFunc>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return gfx::CompareFunc::NumberOf;
  }

  gfx::CompareFunc MaterialData::GetDepthTestTypeFromAttributeName(const char* name)
  {
    const char* attributeNames[] = 
    {
      "LESS",
      "LEQUAL",
      "EQUAL",
      "GEQUAL",
      "GREATER",
      "NOTEQUAL",
      "ALWAYS",
      "NEVER",
    };
    for(u32 i = 0; i < gfx::CompareFunc::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<gfx::CompareFunc>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return gfx::CompareFunc::NumberOf;
  }

  gfx::CompareFunc MaterialData::GetStencilTestTypeFromAttributeName(const char* name)
  {
    const char* attributeNames[] = 
    {
      "LESS",
      "LEQUAL",
      "EQUAL",
      "GEQUAL",
      "GREATER",
      "NOTEQUAL",
      "ALWAYS",
      "NEVER",
    };
    for(u32 i = 0; i < gfx::CompareFunc::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<gfx::CompareFunc>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return gfx::CompareFunc::NumberOf;
  }

  gfx::StencilOp MaterialData::GetStencilOperationFromAttributeName(const char* name)
  {
    const char* attributeNames[] = 
    {
      "KEEP",
      "ZERO",
      "REPLACE",
      "INCR",
      "INCR_WRAP",
      "DECR",
      "DECR_WRAP",
      "INVERT",
    };
    for(u32 i = 0; i < gfx::StencilOp::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<gfx::StencilOp>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return gfx::StencilOp::NumberOf;
  }

  renderer::BlendMode MaterialData::GetBlendModeFromAttributeName(const char* name)
  {
    const char* attributeNames[] = 
    {
      "Blend",
      "Separate",
      "LogicOp",
      "None",
    };
    for(u32 i = 0; i < renderer::BlendMode::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<renderer::BlendMode>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return renderer::BlendMode::NumberOf;
  }

  gfx::BlendOp MaterialData::GetBlendFuncFromAttributeName(const char* name)
  {
    const char* attributeNames[] = 
    {
      "ADD",
      "SUBTRACT",
      "REVERSE_SUBTRACT",
      "MIN",
      "MAX",
    };
    for(u32 i = 0; i <  gfx::BlendOp::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast< gfx::BlendOp>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return  gfx::BlendOp::NumberOf;
  }

  renderer::BlendElement MaterialData::GetBlendElementFromAttributeName(const char* name)
  {
    const char* attributeNames[] = 
    {
      "Zero",
      "One",
      "SourceColor",
      "OneMinusSourceColor",
      "DestinationColor",
      "OneMinusDestinationColor",
      "SourceAlpha",
      "OneMinusSourceAlpha",
      "DestinationAlpha",
      "OneMinusDestinationAlpha",
      "ConstantColor",
      "OneMinusConstantColor",
      "ConstantAlpha",
      "OneMinusConstantAlpha",
      "SourceAlphaSaturate",
    };
    for(u32 i = 0; i < renderer::BlendElement::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<renderer::BlendElement>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return renderer::BlendElement::NumberOf;
  }

  renderer::LogicalOperation MaterialData::GetLogicalOperationFromAttributeName(const char* name)
  {
    const char* attributeNames[] = 
    {
      "CLEAR",
      "SET",
      "COPY",
      "NOOP",
      "COPY_INVERTED",
      "INVERT",
      "AND",
      "OR",
      "XOR",
      "AND_REVERSE",
      "OR_REVERSE",
      "AND_INVERTED",
      "OR_INVERTED",
      "NAND",
      "NOR",
      "EQUIV",
    };
    for(u32 i = 0; i < renderer::LogicalOperation::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<renderer::LogicalOperation>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return renderer::LogicalOperation::NumberOf;
  }

  LutInputAngleType MaterialData::GetLutInputAngleTypeFromAttributeName(const char* name)
  {
    const char* attributeNames[] =
    {
      "NH",
      "VH",
      "NV",
      "LN",
      "SP",
      "CP"
    };
    for(u32 i = 0; i < renderer::LogicalOperation::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<LutInputAngleType>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return LutInputAngleType::NumberOf;
  }

  gfl2::gfx::CullMode MaterialData::GetCullModeFromAttributeName( const char* name )
  {
    const char* attributeNames[] =
    {
      "Always",
      "Back",
      "Front"
    };
    for(u32 i = 0; i < gfl2::gfx::CullMode::NumberOf; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<gfl2::gfx::CullMode>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return gfl2::gfx::CullMode::NumberOf;
  }

  MathFormulaType MaterialData::GetMathFormulaTypeFromAttributeName( const char* name )
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
    // assert(0 && "アトリビュート名が不適切です");
    return MathFormulaType::NumberOf;
  }

  s16 MaterialData::GetCombinerInputSourceFromAttributeName( const char* name )
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
    // assert(0 && "アトリビュート名が不適切です");
    return length;
  }

  s16 MaterialData::GetCombinerOperandFromAttributeName( const char* name )
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
    // assert(0 && "アトリビュート名が不適切です");
    return length;
  }

  PokeNormalType GetPokeNormalTypeFromAttributeName( const char* name )
  {
    const char* attributeNames[] = {
      "None",
      "Dummy",
      "ObjectSpace",
    };

    s16 length = static_cast<s16>(sizeof(attributeNames) / sizeof(attributeNames[0]));
    for(s16 i = 0; i < length; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<PokeNormalType>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return static_cast<PokeNormalType>(length);
  }

  BlendType GetBlendTypeFromAttributeName( const char* name )
  {
    const char* attributeNames[] = {
      "None",
      "Nuki", //! 抜き設定
      "Liner", //! 線形補完
      "Add", //! 加算
      "Sub", //! 減算
      "Mul", //! 乗算
      "Inverse", //! ネガポジ反転
    };

    s16 length = static_cast<s16>(sizeof(attributeNames) / sizeof(attributeNames[0]));
    for(s16 i = 0; i < length; ++i)
    {
      if(strcmp(attributeNames[i], name) == 0)
      {
        return static_cast<BlendType>(i);
      }
    }
    // assert(0 && "アトリビュート名が不適切です");
    return static_cast<BlendType>(length);
  }

  void MaterialData::ReadMaterialDataNode( const xml_node<>* top_node, const VersionId versionId )
  {
    {
      m_Name = top_node->first_attribute( "Name" )->value();
      std::wstring utf16 = ParserUtil::UTF8toUTF16( m_Name );
      m_Name = ParserUtil::UTF16toShitJIS( utf16 );
    }

    m_Type = top_node->first_attribute( "Type" )->value();

    xml_attribute<>* priority_attribute = top_node->first_attribute("Priority");
    if (priority_attribute)
    {
      m_AttributeParam.m_Priority = atoi(priority_attribute->value());
    }

    {// <PsLightingEnable>などの先頭ノードの取得
      xml_node<>* pslightingenable_node = top_node->first_node( "PsLightingEnable" );
      m_AttributeParam.m_PsLightingEnable = (pslightingenable_node != NULL);
      if (m_AttributeParam.m_PsLightingEnable)
      {
        m_AttributeParam.m_PsLightingEnable = ( atoi( pslightingenable_node->first_attribute( "PsLightingEnable" )->value() ) != 0 );
      }
      xml_node<>* vslightingenable_node = top_node->first_node( "VsLightingEnable" );
      m_AttributeParam.m_VsLightingEnable = (vslightingenable_node != NULL);
      if (m_AttributeParam.m_VsLightingEnable)
      {
        m_AttributeParam.m_VsLightingEnable = ( atoi( vslightingenable_node->first_attribute( "VsLightingEnable" )->value() ) != 0 );
      }
      xml_node<>* lightsetno_node = top_node->first_node( "LightSetNo" );
      m_AttributeParam.m_LightSetNo = (lightsetno_node != NULL);
      if (m_AttributeParam.m_LightSetNo)
      {
        m_AttributeParam.m_LightSetNo = (s16)atoi( lightsetno_node->first_attribute( "LightSetNo" )->value() );
      }
      xml_node<>* fogenable_node = top_node->first_node( "FogEnable" );
      m_AttributeParam.m_FogEnable = (fogenable_node != NULL);
      if (m_AttributeParam.m_FogEnable)
      {
        m_AttributeParam.m_FogEnable = ( atoi( fogenable_node->first_attribute( "FogEnable" )->value() ) != 0 );
      }
      xml_node<>* fogno_node = top_node->first_node( "FogNo" );
      m_AttributeParam.m_FogNo = 0;
      if (fogno_node)
      {
        m_AttributeParam.m_FogNo = atoi( fogno_node->first_attribute( "FogNo" )->value() );
      }
      xml_node<>* cullmode_node = top_node->first_node( "CullMode" );
      m_AttributeParam.m_CullMode = gfl2::gfx::CullMode::CCW;
      if (cullmode_node)
      {
        m_AttributeParam.m_CullMode = MaterialData::GetCullModeFromAttributeName( cullmode_node->first_attribute( "CullMode" )->value() );
      }
    }

    {// カラーの取得
      xml_node<>* colors_node = top_node->first_node( "Colors" );
      if (colors_node)
      {
        xml_node<>* materialcolor_node = colors_node->first_node( "MaterialColor" );
        gfx::Color    f32Color;

        if (materialcolor_node)
        {
          xml_node<>* emission_node = materialcolor_node->first_node( "Emission" );
          f32Color.x = static_cast<float>( atof( emission_node->first_attribute( "R" )->value() ) );
          f32Color.y = static_cast<float>( atof( emission_node->first_attribute( "G" )->value() ) );
          f32Color.z = static_cast<float>( atof( emission_node->first_attribute( "B" )->value() ) );
          f32Color.w = static_cast<float>( atof( emission_node->first_attribute( "A" )->value() ) );
          m_AttributeParam.m_MaterialColor.m_Emission = f32Color;

          xml_node<>* ambient_node = materialcolor_node->first_node( "Ambient" );
          f32Color.x = static_cast<float>( atof( ambient_node->first_attribute( "R" )->value() ) );
          f32Color.y = static_cast<float>( atof( ambient_node->first_attribute( "G" )->value() ) );
          f32Color.z = static_cast<float>( atof( ambient_node->first_attribute( "B" )->value() ) );
          f32Color.w = static_cast<float>( atof( ambient_node->first_attribute( "A" )->value() ) );
          m_AttributeParam.m_MaterialColor.m_Ambient = f32Color;

          xml_node<>* diffuse_node = materialcolor_node->first_node( "Diffuse" );
          f32Color.x = static_cast<float>( atof( diffuse_node->first_attribute( "R" )->value() ) );
          f32Color.y = static_cast<float>( atof( diffuse_node->first_attribute( "G" )->value() ) );
          f32Color.z = static_cast<float>( atof( diffuse_node->first_attribute( "B" )->value() ) );
          f32Color.w = static_cast<float>( atof( diffuse_node->first_attribute( "A" )->value() ) );
          m_AttributeParam.m_MaterialColor.m_Diffuse = f32Color;

          xml_node<>* specular0_node = materialcolor_node->first_node( "Specular0" );
          f32Color.x = static_cast<float>( atof( specular0_node->first_attribute( "R" )->value() ) );
          f32Color.y = static_cast<float>( atof( specular0_node->first_attribute( "G" )->value() ) );
          f32Color.z = static_cast<float>( atof( specular0_node->first_attribute( "B" )->value() ) );
          f32Color.w = static_cast<float>( atof( specular0_node->first_attribute( "A" )->value() ) );
          m_AttributeParam.m_MaterialColor.m_Specular0 = f32Color;

          xml_node<>* specular1_node = materialcolor_node->first_node( "Specular1" );
          f32Color.x = static_cast<float>( atof( specular1_node->first_attribute( "R" )->value() ) );
          f32Color.y = static_cast<float>( atof( specular1_node->first_attribute( "G" )->value() ) );
          f32Color.z = static_cast<float>( atof( specular1_node->first_attribute( "B" )->value() ) );
          f32Color.w = static_cast<float>( atof( specular1_node->first_attribute( "A" )->value() ) );
          m_AttributeParam.m_MaterialColor.m_Specular1 = f32Color;
        }
        xml_node<>* constantcolor_node = colors_node->first_node( "ConstantColor" );
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
            if ( versionId.m_Value < VersionId(0, 9).m_Value )
            {// バージョン0.9以前は正確なテクスチャボーダーカラーが取得できていないので0, 0, 0, 1を設定
              f32Color.x = 0.0f;
              f32Color.y = 0.0f;
              f32Color.z = 0.0f;
              f32Color.w = 1.0f;
              m_AttributeParam.m_EtcColor.m_TextureBorderColor[i] = f32Color;
            }
            else
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
    }

    {// バンプ関連の情報
      xml_node<>* bumpmapno_node = top_node->first_node( "BumpMapNo" );
      m_AttributeParam.m_BumpMapNo = 0;
      if (bumpmapno_node)
      {
        m_AttributeParam.m_BumpMapNo = static_cast<s16>( atoi( bumpmapno_node->first_attribute( "BumpMapNo" )->value() ) );
      }
      xml_node<>* bumpmaptype_node = top_node->first_node( "BumpMapType" );
      m_AttributeParam.m_BumpMapType = 0;
      if (bumpmaptype_node)
      {
        m_AttributeParam.m_BumpMapType = static_cast<s16>( atoi( bumpmaptype_node->first_attribute( "BumpMapType" )->value() ) );
      }
      xml_node<>* zrecastenable_node = top_node->first_node( "ZRecastEnable" );
      m_AttributeParam.m_ZRecastEnable = false;
      if (zrecastenable_node)
      {
        m_AttributeParam.m_ZRecastEnable = static_cast<b32>( atoi( zrecastenable_node->first_attribute( "ZRecastEnable" )->value() ) != 0 );
      }
    }

    {// テクスチャの取得
      xml_node<>* textures_node = top_node->first_node( "Textures" );

      int cntTexture = 0;
      std::vector<std::string> uvSets;

      for( const xml_node<>* texture_node = textures_node->first_node("Texture");texture_node; texture_node = texture_node->next_sibling("Texture") )
      {
        Texture texture;
        xml_attribute<> *textureNoAttr = texture_node->first_attribute("Usage");

        if ( textureNoAttr == NULL )
        {
          textureNoAttr = texture_node->first_attribute("TextureNo");
        }

        texture.m_SlotNo = atoi( texture_node->first_attribute( "SlotNo" )->value() );
        texture.m_Usage = atoi( textureNoAttr->value() );
        texture.m_UvUsage = texture_node->first_attribute( "UvUsage" )->value();
        texture.m_UvUsageNo = 0;

        {
          static const c8* s_pUvUsageName[] = {
            "ColorLayer0",
            "ColorLayer1",
            "ColorLayer2",
            "Normal"
          };
          for( u32 no = 0; no < ARRAY_SIZE(s_pUvUsageName); ++no )
          {
            if ( texture.m_UvUsage == s_pUvUsageName[no] )
            {
              texture.m_UvUsageNo = no;
              break;
            }
          }
        }

        const xml_node<>* attribute_node = texture_node->first_node("Attribute");
        if ( attribute_node )
        {
          texture.m_Attribute.m_UvSetNo     = atoi( attribute_node->first_attribute( "UvSetNo" )->value() );
          texture.m_Attribute.m_MappintType = attribute_node->first_attribute( "MappintType" )->value();
          texture.m_Attribute.m_ScaleU      = static_cast<f32>( atof( attribute_node->first_attribute( "ScaleU" )->value() ) );
          texture.m_Attribute.m_ScaleV      = static_cast<f32>( atof( attribute_node->first_attribute( "ScaleV" )->value() ) );
          texture.m_Attribute.m_Rotate      = static_cast<f32>( atof( attribute_node->first_attribute( "Rotate" )->value() ) );
          texture.m_Attribute.m_TranslateU  = static_cast<f32>( atof( attribute_node->first_attribute( "TranslateU" )->value() ) );
          texture.m_Attribute.m_TranslateV  = static_cast<f32>( atof( attribute_node->first_attribute( "TranslateV" )->value() ) );
          texture.m_Attribute.m_RepeatU     = attribute_node->first_attribute( "RepeatU" )->value();
          texture.m_Attribute.m_RepeatV     = attribute_node->first_attribute( "RepeatV" )->value();
          texture.m_Attribute.m_MagFilter   = attribute_node->first_attribute( "MagFilter" )->value();
          texture.m_Attribute.m_MinFilter   = attribute_node->first_attribute( "MinFilter" )->value();

          xml_attribute<>* pMipBiasAttr = attribute_node->first_attribute( "MipBias" );
          texture.m_Attribute.m_MipBiasNo = (pMipBiasAttr) ? atof( pMipBiasAttr->value() ) : 0;

          if ( m_fUvSetAdjust )
          {
            xml_attribute<>* pUvSetName = attribute_node->first_attribute( "UvSetName" );
            if ( pUvSetName )
            {
              texture.m_Attribute.m_UvSetName = pUvSetName->value();
              texture.m_Attribute.m_UvSetNameList = ParserUtil::SplitStr( texture.m_Attribute.m_UvSetName, "|" );
            }

            if ( std::find( uvSets.begin(), uvSets.end(), texture.m_Attribute.m_UvSetName ) == uvSets.end() )
            {
              texture.m_Attribute.m_UvSetNo = uvSets.size();
              uvSets.push_back( texture.m_Attribute.m_UvSetName );
            }
            else
            {
              for( int setNo = 0; setNo < uvSets.size(); ++setNo )
              {
                if ( texture.m_Attribute.m_UvSetName == uvSets[setNo] )
                {
                  texture.m_Attribute.m_UvSetNo = setNo;
                  break;
                }
              }
            }
          }
        }

        {
          static const c8* s_pMappingTypeNames[] = {
            "UvMap",
            "CameraCubeEnvMap",
            "CameraSphereEnvMap",
            "ProjectionMap"
          };

          for( u32 no = 0; no < ARRAY_SIZE(s_pMappingTypeNames); ++no )
          {
            if ( texture.m_Attribute.m_MappintType == s_pMappingTypeNames[no] )
            {
              texture.m_Attribute.m_MappintTypeNo = no;
              break;
            }
          }
        }

        {
          // Maya上では, WRAP, MIRROR, CLAMP, BORDERの順だが,
          // Runtime上では, WRAP, MIRROR, BORDER, CLAMPなのでそれに合わせる
          static const c8 *pRepeatTypeName[] = {
            "WRAP", "MIRROR", "BORDER", "CLAMP"
          };

          for( u32 no = 0; no < ARRAY_SIZE(pRepeatTypeName); ++no )
          {
            if ( texture.m_Attribute.m_RepeatU == pRepeatTypeName[no] )
            {
              texture.m_Attribute.m_RepeatUNo = no;
            }
            if ( texture.m_Attribute.m_RepeatV == pRepeatTypeName[no] )
            {
              texture.m_Attribute.m_RepeatVNo = no;
            }
          }
        }

        {
          static const c8 *pFilterName[] = {
            "POINT", "LINEAR", "POINT_LINEAR", "LINEAR_LINEAR"
          };

          for( u32 no = 0; no < ARRAY_SIZE(pFilterName); ++no )
          {
            if ( texture.m_Attribute.m_MagFilter == pFilterName[no] ) texture.m_Attribute.m_MagFilterNo = no;
            if ( texture.m_Attribute.m_MinFilter == pFilterName[no] ) texture.m_Attribute.m_MinFilterNo = no;
          }

          {
            //POINT_LINEAR -> POINTにLINEAR_LINEAR -> LINEARに変換
            if ( texture.m_Attribute.m_MagFilterNo >= 2 ) texture.m_Attribute.m_MagFilterNo -= 2;
            if ( texture.m_Attribute.m_MinFilterNo >= 2 ) texture.m_Attribute.m_MinFilterNo -= 2;
          }
        }

        m_TextureList.push_back( texture );
        ++cntTexture;
      }
    }

    {// フラグメント情報の取得
      xml_node<>* fragments_node = top_node->first_node( "Fragments" );
      if( fragments_node )
      {
        // フラグメントライティングの項目の取得
        const xml_node<>* fragmentlighting_node = fragments_node->first_node( "FragmentLighting" );

        if (versionId.m_Value >= VersionId(0, 91).m_Value)
        {// バージョン0.91以降はフレネルの情報をfragmentlighting_nodeが持つ
          m_AttributeParam.m_PrimaryFresnelEnable = ( atoi( fragmentlighting_node->first_attribute( "PrimaryFresnelEnable" )->value() ) != 0 );
          m_AttributeParam.m_SecondaryFresnelEnable = ( atoi( fragmentlighting_node->first_attribute( "SecondaryFresnelEnable" )->value() ) != 0 );
          m_AttributeParam.m_SpecularColorClamp = ( atoi( fragmentlighting_node->first_attribute( "SpecularColorClamp" )->value() ) != 0 );
          m_AttributeParam.m_CookTorrance0Enable = ( atoi( fragmentlighting_node->first_attribute( "CookTorrance0Enable" )->value() ) != 0 );
          m_AttributeParam.m_CookTorrance1Enable = ( atoi( fragmentlighting_node->first_attribute( "CookTorrance1Enable" )->value() ) != 0 );
        }

        {
          const xml_node<>*	distribution0sampler_node = fragmentlighting_node->first_node( "Distribution0Sampler" );
          m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Distribution0] = ( distribution0sampler_node != NULL);
          if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Distribution0] )
          {
            m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution0].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(distribution0sampler_node->first_attribute( "LutInputAngleD0")->value() );
            m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution0].m_LutScale = static_cast<float>( atof(distribution0sampler_node->first_attribute( "LutScaleD0" )->value() ) );
            const xml_attribute<>* m_distribution0sampler_attribute = distribution0sampler_node->first_attribute( "TexSlotNo" );
          }

          const xml_node<>*	distribution1sampler_node = fragmentlighting_node->first_node( "Distribution1Sampler" );
          m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Distribution1] = ( distribution1sampler_node != NULL);
          if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Distribution1] )
          {
            m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution1].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(distribution1sampler_node->first_attribute( "LutInputAngleD1")->value() );
            m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution1].m_LutScale = static_cast<float>( atof(distribution1sampler_node->first_attribute( "LutScaleD1" )->value() ) );
            const xml_attribute<>* m_distribution1sampler_attribute = distribution1sampler_node->first_attribute( "TexSlotNo" );
          }

          const xml_node<>*	reflectancersampler_node = fragmentlighting_node->first_node( "ReflectanceRSampler" );
          m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionR] = ( reflectancersampler_node != NULL);
          if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionR] )
          {
            m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionR].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(reflectancersampler_node->first_attribute( "LutInputAngleRR")->value() );
            m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionR].m_LutScale = static_cast<float>( atof(reflectancersampler_node->first_attribute( "LutScaleRR" )->value() ) );
            const xml_attribute<>* m_reflectancersampler_attribute = reflectancersampler_node->first_attribute( "TexSlotNo" );
            m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionR].m_TextureSlotNo = atoi( m_reflectancersampler_attribute->value() );
          }

          const xml_node<>*	reflectancegsampler_node = fragmentlighting_node->first_node( "ReflectanceGSampler" );
          m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionG] = ( reflectancegsampler_node != NULL);
          if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionG] )
          {
            m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionG].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(reflectancegsampler_node->first_attribute( "LutInputAngleRG")->value() );
            m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionG].m_LutScale = static_cast<float>( atof(reflectancegsampler_node->first_attribute( "LutScaleRG" )->value() ) );
            const xml_attribute<>* m_reflectancegsampler_attribute = reflectancegsampler_node->first_attribute( "TexSlotNo" );
            m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionG].m_TextureSlotNo = atoi( m_reflectancegsampler_attribute->value() );
          }

          const xml_node<>*	reflectancebsampler_node = fragmentlighting_node->first_node( "ReflectanceBSampler" );
          m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionB] = ( reflectancebsampler_node != NULL);
          if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionB] )
          {
            m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionB].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(reflectancebsampler_node->first_attribute( "LutInputAngleRB")->value() );
            m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionB].m_LutScale = static_cast<float>( atof( reflectancebsampler_node->first_attribute( "LutScaleRB" )->value() ) );
            const xml_attribute<>* m_reflectancebsampler_attribute = reflectancebsampler_node->first_attribute( "TexSlotNo" );
            m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionB].m_TextureSlotNo = atoi( m_reflectancebsampler_attribute->value() );
          }

          const xml_node<>*	fresnelsampler_node = fragmentlighting_node->first_node( "FresnelSampler" );
          m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Fresnel] = (fresnelsampler_node != NULL);
          if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Fresnel] )
          {
            if (versionId.m_Value >= VersionId(0, 91).m_Value)
            {
              m_AttributeParam.m_LookUpTable[LookUpTableType::Fresnel].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(fresnelsampler_node->first_attribute( "LutInputAngleFR")->value() );
              m_AttributeParam.m_LookUpTable[LookUpTableType::Fresnel].m_LutScale = static_cast<float>( atof( fresnelsampler_node->first_attribute( "LutScaleFR" )->value() ) );
              const xml_attribute<>* m_fresnelsampler_attribute = fresnelsampler_node->first_attribute( "TexSlotNo" );
            }
            else
            {
              m_AttributeParam.m_PrimaryFresnelEnable = ( atoi( fresnelsampler_node->first_attribute( "PrimaryFresnelEnable" )->value() ) != 0 );
              m_AttributeParam.m_SecondaryFresnelEnable = ( atoi( fresnelsampler_node->first_attribute( "SecondaryFresnelEnable" )->value() ) != 0 );
              m_AttributeParam.m_SpecularColorClamp = ( atoi( fresnelsampler_node->first_attribute( "SpecularColorClamp" )->value() ) != 0 );
              m_AttributeParam.m_CookTorrance0Enable = ( atoi( fresnelsampler_node->first_attribute( "CookTorrance0Enable" )->value() ) != 0 );
              m_AttributeParam.m_CookTorrance1Enable = ( atoi( fresnelsampler_node->first_attribute( "CookTorrance1Enable" )->value() ) != 0 );
            }
          }
        }

        // テクスチャコンバイナの項目の取得
        const xml_node<>* texturecombiners_node = fragments_node->first_node( "TextureCombiners" );
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
      }
    }

    {// ブレンド情報の取得
      xml_node<>*		blendtest_node = top_node->first_node( "BlendTest" );
      if( blendtest_node )
      {
        m_BlendTest.m_DrawLayer = static_cast<scenegraph::resource::DrawLayer>(atoi( blendtest_node->first_attribute( "DrawLayer")->value() ));

        const xml_node<>* alphatest_node = blendtest_node->first_node( "AlphaTest" );
        m_BlendTest.m_AlphaTest.m_AlphaTestType = GetAlphaTestTypeFromAttributeName( alphatest_node->first_attribute( "AlphaTestType" )->value() );
        m_BlendTest.m_AlphaTest.m_AlphaTestRefValue = static_cast<float>( atof( alphatest_node->first_attribute( "AlphaTestRefValue" )->value() ) );

        const xml_node<>* depthtest_node = blendtest_node->first_node( "DepthTest" );
        m_BlendTest.m_DepthTest.m_DepthTestType = GetDepthTestTypeFromAttributeName( depthtest_node->first_attribute( "DepthTestType" )->value() );
        m_BlendTest.m_DepthTest.m_UpDateDepth = atoi( depthtest_node->first_attribute( "UpDateDepth" )->value() );
        m_BlendTest.m_DepthTest.m_PolygonOffset = atoi( depthtest_node->first_attribute( "PolygonOffSet" )->value() );

        const xml_node<>* stenciltest_node = blendtest_node->first_node( "StencilTest" );
        m_BlendTest.m_StencilTest.m_StencilTestEnable = static_cast<b32>( atoi( stenciltest_node->first_attribute( "StencilTestRefEnable" )->value() ) );
        m_BlendTest.m_StencilTest.m_StencilTestType = GetStencilTestTypeFromAttributeName( stenciltest_node->first_attribute( "StencilTestType" )->value() );
        m_BlendTest.m_StencilTest.m_StencilValue = static_cast<u8>( atoi( stenciltest_node->first_attribute( "StencilValue" )->value() ) );
        m_BlendTest.m_StencilTest.m_StencilMask = static_cast<u8>( atoi( stenciltest_node->first_attribute( "StencilMask" )->value() ) );
        m_BlendTest.m_StencilTest.m_StencilFailOp = GetStencilOperationFromAttributeName( stenciltest_node->first_attribute( "StencilFailOp" )->value() );
        m_BlendTest.m_StencilTest.m_StencilZFailOp = GetStencilOperationFromAttributeName( stenciltest_node->first_attribute( "StencilZFailOp" )->value() );
        m_BlendTest.m_StencilTest.m_StencilZpassOp = GetStencilOperationFromAttributeName( stenciltest_node->first_attribute( "StencilZpassOp" )->value() );

        const xml_node<>* blender_node = blendtest_node->first_node( "Blender" );
        m_BlendTest.m_Blender.m_BlendMode = GetBlendModeFromAttributeName( blender_node->first_attribute( "BlendMode" )->value() );
        m_BlendTest.m_Blender.m_LogicalOperation = GetLogicalOperationFromAttributeName( blender_node->first_attribute( "LogicalOperation" )->value() );
        {
          const xml_node<>* color_node = blender_node->first_node( "Color" );
          m_BlendTest.m_Blender.m_Color.m_BlendFunc = GetBlendFuncFromAttributeName( color_node->first_attribute( "BlendFunc" )->value() );
          m_BlendTest.m_Blender.m_Color.m_SrcElement = GetBlendElementFromAttributeName( color_node->first_attribute( "SrcElement" )->value() );
          m_BlendTest.m_Blender.m_Color.m_DstElement = GetBlendElementFromAttributeName( color_node->first_attribute( "DstElement" )->value() );

          const xml_node<>* alpha_node = blender_node->first_node( "Alpha" );
          m_BlendTest.m_Blender.m_Alpha.m_BlendFunc = GetBlendFuncFromAttributeName( alpha_node->first_attribute( "BlendFunc" )->value() );
          m_BlendTest.m_Blender.m_Alpha.m_SrcElement = GetBlendElementFromAttributeName( alpha_node->first_attribute( "SrcElement" )->value() );
          m_BlendTest.m_Blender.m_Alpha.m_DstElement = GetBlendElementFromAttributeName( alpha_node->first_attribute( "DstElement" )->value() );
        }
      }
    }

    // [UserData]の取得
    {
      xml_node<>* userdata_node = top_node->first_node( "UserData" );
      if (userdata_node)
      {
        m_UserData.m_EdgeType = static_cast<EdgeType>( atoi( userdata_node->first_attribute( "EdgeType" )->value()) );
        m_UserData.m_IDEdgeEnable = ( atoi( userdata_node->first_attribute( "IDEdgeEnable" )->value()) != 0);
        m_UserData.m_EdgeID = static_cast<u32>( atoi( userdata_node->first_attribute( "EdgeID" )->value()) );
        m_UserData.m_ProjectionType = static_cast<ProjectionType>( atoi( userdata_node->first_attribute( "ProjectionType" )->value()) );
        m_UserData.m_RimPow = static_cast<f32>( atof( userdata_node->first_attribute( "RimPow" )->value()) );
        m_UserData.m_RimScale = static_cast<f32>( atof( userdata_node->first_attribute( "RimScale" )->value()) );
        m_UserData.m_PhongPow = static_cast<f32>( atof( userdata_node->first_attribute( "PhongPow" )->value()) );
        m_UserData.m_PhongScale = static_cast<f32>( atof( userdata_node->first_attribute( "PhongScale" )->value()) );
        m_UserData.m_IDEdgeOffsetEnable = ( atoi( userdata_node->first_attribute( "IDEdgeOffsetEnable" )->value()) != 0);
        // todo: テクスチャの名前ではなく数値になっているのでプラグイン側の修正が必要(-1とかしたくない)
        m_UserData.m_EdgeMapAlphaMask = static_cast<CombinerAttribute::EdgeMapAlpha>( atoi( userdata_node->first_attribute( "EdgeMapAlphaMask" )->value()) - 1 );
        for ( u32 tex = CombinerAttribute::UvTextures::Texture0; tex < CombinerAttribute::UvTextures::NumberOf; ++tex )
        {
          c8 attr_name[64] = {0};
          sprintf_s(attr_name, "BakeTexture%d", tex);
          m_UserData.m_BakeTextures[tex] = static_cast<TextureBakeType>( atoi( userdata_node->first_attribute( attr_name )->value()) );
        }
        for ( u32 tex = LookUpTableType::Distribution0; tex < LookUpTableType::NumberOf; ++tex )
        {
          c8 attr_name[64] = {0};
          sprintf_s(attr_name, "BakeConstant%d", tex);
          m_UserData.m_BakeConstants[tex] = static_cast<TextureBakeType>( atoi( userdata_node->first_attribute( attr_name )->value()) );
        }
        xml_attribute<>* vertexshadertype_attribute = userdata_node->first_attribute( "VertexShaderType" );
        s32 vertexShaderType = static_cast<u32>(atoi( vertexshadertype_attribute->value() ) );
        if ( versionId.m_Value <= VersionId(0, 6).m_Value )
        {
          m_UserData.m_VertexShaderType = VertexShaderType::GetValueByIndex( vertexShaderType );
        }
        else
        {
          m_UserData.m_VertexShaderType = static_cast<VertexShaderType::Enum>( vertexShaderType );
        }
        m_UserData.m_ShaderParam0 = static_cast<f32>( atof( userdata_node->first_attribute( "ShaderParam0" )->value()) );
        m_UserData.m_ShaderParam1 = static_cast<f32>( atof( userdata_node->first_attribute( "ShaderParam1" )->value()) );
        m_UserData.m_ShaderParam2 = static_cast<f32>( atof( userdata_node->first_attribute( "ShaderParam2" )->value()) );
        m_UserData.m_ShaderParam3 = static_cast<f32>( atof( userdata_node->first_attribute( "ShaderParam3" )->value()) );

      }
    }

    // [特殊設定]の取得
    {
      xml_node<>* specialsetting_node = top_node->first_node( "SpecialSetting" );
      if ( specialsetting_node )
      {
        m_AttributeParam.m_IsPokemonShader = ( atoi( specialsetting_node->first_attribute( "IsPokemonShader" )->value()) != 0 );
        m_AttributeParam.m_UseShadowTexture = ( atoi( specialsetting_node->first_attribute( "UseShadowTexture" )->value()) != 0 );
        m_AttributeParam.m_UseObjectSpaceNormalMap = ( atoi( specialsetting_node->first_attribute( "UseObjectSpaceNormalMap" )->value()) != 0 );
        // ハイライトマップ, アウトラインカラーマップ, ブラースケールマップに関してはノードの有無で判断
        xml_attribute<>* highlighttextureenable_attribute = specialsetting_node->first_attribute( "HighLightTextureEnable" );
        m_AttributeParam.m_HighLightTextureEnable = (highlighttextureenable_attribute != NULL);

        if ( 0 )
        {
          xml_attribute<>* outlinecolortextureenable_attribute = specialsetting_node->first_attribute( "OutLineTextureEnable" );
          m_AttributeParam.m_OutLineColorTextureEnable = (outlinecolortextureenable_attribute != NULL);
          xml_attribute<>* blurscaletextureenable_attribute = specialsetting_node->first_attribute( "BlurScaleTextureEnable" );
          m_AttributeParam.m_HighLightTextureEnable = (blurscaletextureenable_attribute != NULL);
        }
        m_AttributeParam.m_PokeNormalType = GetPokeNormalTypeFromAttributeName( specialsetting_node->first_attribute( "PokeNormalType" )->value() );
        m_AttributeParam.m_BlendType =GetBlendTypeFromAttributeName( specialsetting_node->first_attribute( "BlendType" )->value() );
        m_AttributeParam.m_UpdateShaderParam = ( atoi( specialsetting_node->first_attribute( "UpdateShaderParam" )->value()) != 0 );
      }
    }

    {
      xml_node<>* combiner_node = top_node->first_node( "Combiner" );
      if (combiner_node)
      {
        m_Combiner.m_PresetName = combiner_node->first_attribute( "PresetName" )->value();
        std::wstring utf16 = ParserUtil::UTF8toUTF16( m_Combiner.m_PresetName );
        m_Combiner.m_PresetName = ParserUtil::UTF16toShitJIS( utf16 );
      }
    }
  }

  bool MaterialData::CreateBinary( std::vector<std::string> &rFilePathList, std::vector<ShaderData> &rShaderDataList, const bool enableLog )
  {
    BinaryMaker binary;

    char nameBuffer[NAME_DATA_MAX];

    {//マテリアル名出力
      util::FnvHash hash( reinterpret_cast<const u8*>(m_Name.c_str()) );

      assert( m_Name.length() < NAME_DATA_MAX );
      memset( nameBuffer, 0, sizeof(nameBuffer) );
      memcpy(nameBuffer,m_Name.c_str(),strlen(m_Name.c_str())); // Garbage Data Fix

      binary.Write( &hash.value, sizeof( hash.value ) );
      binary.Write( nameBuffer, sizeof( nameBuffer ) );
    }

    {//シェーダータイプ名出力
      util::FnvHash hash( reinterpret_cast<const u8*>(m_Type.c_str()) );

      assert( m_Type.length() < NAME_DATA_MAX );
      memset( nameBuffer, 0, sizeof(nameBuffer) );		
      memcpy(nameBuffer,m_Type.c_str(),strlen(m_Type.c_str())); // Garbage Data Fix

      binary.Write( &hash.value, sizeof( hash.value ) );
      binary.Write( nameBuffer, sizeof( nameBuffer ) );
    }

    for( int i = 0; i < rShaderDataList.size(); ++i )
    {//シェーダー名出力
      if ( rShaderDataList[i].m_ShaderName == m_Combiner.m_PresetName )
      {
        {
          std::string shaderName = ParserUtil::GetFileNameStr( rShaderDataList[i].m_VsFilePath.c_str() );
          std::vector<std::string> shaderNames = ParserUtil::SplitStr( shaderName, "." );
          util::FnvHash hash( reinterpret_cast<const u8*>(shaderNames[0].c_str()) );

          assert( shaderNames[0].length() < NAME_DATA_MAX );
          memset( nameBuffer, 0, sizeof(nameBuffer) );
          strcpy_s( nameBuffer, shaderNames[0].c_str() );

          binary.Write( &hash.value, sizeof( hash.value ) );
          binary.Write( nameBuffer, sizeof( nameBuffer ) );
        }

        {
          std::string shaderName = ParserUtil::GetFileNameStr( rShaderDataList[i].m_FsFilePath.c_str() );
          std::vector<std::string> shaderNames = ParserUtil::SplitStr( shaderName, "." );
          util::FnvHash hash( reinterpret_cast<const u8*>(shaderNames[0].c_str()) );

          assert( shaderNames[0].length() < NAME_DATA_MAX );
          memset( nameBuffer, 0, sizeof(nameBuffer) );				
          memcpy(nameBuffer,shaderNames[0].c_str(),strlen(shaderNames[0].c_str())); // Garbage Data Fix

          binary.Write( &hash.value, sizeof( hash.value ) );
          binary.Write( nameBuffer, sizeof( nameBuffer ) );
        }
        break;
      }
    }

    u32 textureNum = static_cast<u32>( m_TextureList.size() );
    binary.Write( &textureNum, sizeof( textureNum ) );

    for( u32 i = 0; i < textureNum; ++i )
    {
      const Texture &rTexture = m_TextureList[i];

      {
        c8 *pathBuffer = new c8[ rFilePathList[rTexture.m_Usage].length() + 1 ];
        c8 nameBuffer[NAME_DATA_MAX];

        memset( nameBuffer, 0, sizeof(nameBuffer) );

        ParserUtil::GetFileName( pathBuffer, rFilePathList[rTexture.m_Usage].c_str() );
        assert( strlen(pathBuffer) < NAME_DATA_MAX );
        strcat_s( nameBuffer, pathBuffer );

        util::FnvHash nameHash( reinterpret_cast<u8*>(nameBuffer) );
        u32 hash = nameHash.value;

        binary.Write( &hash, sizeof( u32 ) );
        binary.Write( nameBuffer, NAME_DATA_MAX );

        delete[] pathBuffer;
      }

      binary.Write( &rTexture.m_SlotNo, sizeof( rTexture.m_SlotNo ) );
      binary.Write( &rTexture.m_Usage, sizeof( rTexture.m_Usage ) );
      binary.Write( &rTexture.m_UvUsageNo, sizeof( rTexture.m_UvUsageNo ) );

      binary.Write( &rTexture.m_Attribute.m_UvSetNo, sizeof( rTexture.m_Attribute.m_UvSetNo ) );
      binary.Write( &rTexture.m_Attribute.m_MappintTypeNo, sizeof( rTexture.m_Attribute.m_MappintTypeNo ) );
      binary.Write( &rTexture.m_Attribute.m_ScaleU, sizeof( rTexture.m_Attribute.m_ScaleU ) );
      binary.Write( &rTexture.m_Attribute.m_ScaleV, sizeof( rTexture.m_Attribute.m_ScaleV ) );
      binary.Write( &rTexture.m_Attribute.m_Rotate, sizeof( rTexture.m_Attribute.m_Rotate ) );
      binary.Write( &rTexture.m_Attribute.m_TranslateU, sizeof( rTexture.m_Attribute.m_TranslateU ) );
      binary.Write( &rTexture.m_Attribute.m_TranslateV, sizeof( rTexture.m_Attribute.m_TranslateV ) );
      binary.Write( &rTexture.m_Attribute.m_RepeatUNo, sizeof( rTexture.m_Attribute.m_RepeatUNo ) );
      binary.Write( &rTexture.m_Attribute.m_RepeatVNo, sizeof( rTexture.m_Attribute.m_RepeatVNo ) );
      binary.Write( &rTexture.m_Attribute.m_MagFilterNo, sizeof( rTexture.m_Attribute.m_MagFilterNo ) );
      binary.Write( &rTexture.m_Attribute.m_MinFilterNo, sizeof( rTexture.m_Attribute.m_MinFilterNo ) );
      binary.Write( &rTexture.m_Attribute.m_MipBiasNo, sizeof( rTexture.m_Attribute.m_MipBiasNo ) );
    }

    {// マテリアル情報を書き込み
      scenegraph_resource::MaterialResourceNode::AttributeParam attributeParam;

      memset( &attributeParam, 0, sizeof(attributeParam) );//パディング部分があった場合のクリア
      attributeParam = m_AttributeParam;

      binary.Write( &attributeParam, sizeof( attributeParam ) );
    }

    {// ブレンド情報を書き込み
      scenegraph_resource::MaterialResourceNode::BlendTest blendTest;

      memset( &blendTest, 0, sizeof(blendTest) );//パディング部分があった場合のクリア
      blendTest = m_BlendTest;

      binary.Write( &blendTest, sizeof( blendTest ) );
    }

    {// UserData情報を書き込み
      scenegraph_resource::MaterialResourceNode::UserData userData;

      memset( &userData, 0, sizeof(userData) );//パディング部分があった場合のクリア
      userData = m_UserData;

      binary.Write( &userData, sizeof( userData ) );
    }

    m_pBinaryData = binary.CreateFixedBuffer();
    m_BinarySize = binary.GetBinarySize();

    return true;
  }

  std::vector<MaterialData::LookUpTableCommand> MaterialData::s_LookUpTableCommandList;

  bool MaterialData::CreateBinaryCTR( std::vector<std::string> &rFilePathList, std::vector<ShaderData> &rShaderDataList, const bool enableLog )
  {
    BinaryMaker binary;

    {//マテリアル名出力
      u8 nameLength = static_cast<u8>( m_Name.length() );
      util::FnvHash hash( reinterpret_cast<const u8*>(m_Name.c_str()) );

      binary.Write( &hash.value, sizeof( hash.value ) );
      binary.Write( &nameLength, sizeof(u8) );
      binary.Write( m_Name.c_str(), nameLength );

      {
        char message[128] = {0};
        sprintf_s( message, "materialName = %d : %d : %s\n", hash.value, nameLength, m_Name.c_str() );
        //OutputDebugStringA( message );
      }
    }

    {//シェーダー名出力
      u8 nameLength = static_cast<u8>( m_Combiner.m_PresetName.length() );
      util::FnvHash hash( reinterpret_cast<const u8*>(m_Combiner.m_PresetName.c_str()) );

      binary.Write( &hash.value, sizeof( hash.value ) );
      binary.Write( &nameLength, sizeof(u8) );
      binary.Write( m_Combiner.m_PresetName.c_str(), nameLength );
    }

    for( int i = 0; i < rShaderDataList.size(); ++i )
    {//シェーダー名出力
      if ( rShaderDataList[i].m_ShaderName == m_Combiner.m_PresetName )
      {
        {
          std::string shaderName = ParserUtil::GetFileNameStr( rShaderDataList[i].m_VsFilePath.c_str() );
          std::vector<std::string> shaderNames = ParserUtil::SplitStr( shaderName, "." );
          util::FnvHash hash( reinterpret_cast<const u8*>(shaderNames[0].c_str()) );
          u8 nameLength = static_cast<u8>( shaderNames[0].length() );

          binary.Write( &hash.value, sizeof( hash.value ) );
          binary.Write( &nameLength, sizeof(u8) );
          binary.Write( shaderNames[0].c_str(), nameLength );
        }

        {
          std::string shaderName = ParserUtil::GetFileNameStr( rShaderDataList[i].m_FsFilePath.c_str() );
          std::vector<std::string> shaderNames = ParserUtil::SplitStr( shaderName, "." );
          util::FnvHash hash( reinterpret_cast<const u8*>(shaderNames[0].c_str()) );
          u8 nameLength = static_cast<u8>( shaderNames[0].length() );

          binary.Write( &hash.value, sizeof( hash.value ) );
          binary.Write( &nameLength, sizeof(u8) );
          binary.Write( shaderNames[0].c_str(), nameLength );
        }
      }
    }

    static const s32 refTableNum = 3;
    u32 lutTextureNameHash[refTableNum] = {0, 0, 0};

    {//参照テーブル
      LookUpTableCommand lookUpTableCommand;
      nn::gr::CTR::LookUpTable lookUpTable;
      f32 table[ nn::gr::CTR::LookUpTable::LOOKUP_TABLE_ELEMENT_NUM ];
      f32 linearTable[ nn::gr::CTR::LookUpTable::LOOKUP_TABLE_ELEMENT_NUM ];

      struct LutPixel{
        u8 r;
        u8 g;
        u8 b;
        u8 a;
      };


      Texture *pRefTexture[refTableNum] = { NULL, NULL, NULL };
      u32 samplerTable[refTableNum] = { PICA_DATA_SAMPLER_RR, PICA_DATA_SAMPLER_RG, PICA_DATA_SAMPLER_RB };

      for( s32 i = 0; i < refTableNum; ++i )
      {
        u32 lutNo = LookUpTableType::ReflectionR + i;

        if ( m_AttributeParam.m_LookUpTableEnable[lutNo] )
        {
          for( s32 i2 = 0; i2 < m_TextureList.size(); ++i2 )
          {
            if ( m_TextureList[i2].m_SlotNo == m_AttributeParam.m_LookUpTable[lutNo].m_TextureSlotNo )
            {
              pRefTexture[i] = &m_TextureList[i2];
            }
          }
        }
      }

      for( s32 i = 0; i < refTableNum; ++i )
      {
        if ( pRefTexture[i] == NULL )
          continue;

        std::string lutFilePath = rFilePathList[ pRefTexture[i]->m_Usage ];
        
        c8 *pathBuffer = new c8[ lutFilePath.length() + 1 ];
        c8 nameBuffer[NAME_DATA_MAX];

        memset( nameBuffer, 0, sizeof(nameBuffer) );

        ParserUtil::GetFileName( pathBuffer, lutFilePath.c_str() );
        ERR_RETURN( strlen(pathBuffer) < NAME_DATA_MAX );
        strcat_s( nameBuffer, pathBuffer );

        std::string lutFileName = lutFilePath;
        lutFileName = nameBuffer;


        if ( m_UserData.m_VertexShaderType == VertexShaderType::Poke )
          lutFileName += ".hafl";//加工を示す名前

        util::FnvHash hash( reinterpret_cast<const u8*>(lutFileName.c_str()) );

        lutTextureNameHash[i] = hash.value;

        b32 fHit = false;
        for( int i2 = 0; i2 < s_LookUpTableCommandList.size(); ++i2 )
        {
          if ( s_LookUpTableCommandList[i2].m_TextureName == lutFileName )
          {
            fHit = true;
            break;
          }
        }

        if ( fHit )
          continue;

        {
          gfl2::gfx::ImageData image;
          b32                           result;

          result = image.ReadFromFile( lutFilePath.c_str() );

          if ( result == false )
          {
            //printf( "err : can not file open!! %s\n", lutFilePath.c_str() );
            return false;
          }

          LutPixel *pLutPixel = (LutPixel*)image.GetImage();
          bool fHalfNormal = false;

          if ( m_UserData.m_VertexShaderType == VertexShaderType::Poke )
            fHalfNormal = true;

          if ( m_UserData.m_VertexShaderType == VertexShaderType::BattleChar && this->m_AttributeParam.m_BumpMapNo == -1 )
          {
            fHalfNormal = true;
          }

          //@ todo 256サイズチェックが必要
          if ( fHalfNormal )
          {
            s32 readPos = 64;
            for( s32 i = 0; i < nn::gr::CTR::LookUpTable::LOOKUP_TABLE_ELEMENT_NUM; i += 2,readPos++ )
            {
              table[i] = static_cast<f32>( pLutPixel[readPos].r );
            }

            for( int i = 0; i < 254; i += 2 )
              table[i+1] = (table[i] + table[i+2]) / 2;//補完

            table[255] = table[254];
          }
          else
          {
            for( s32 i = 0; i < nn::gr::CTR::LookUpTable::LOOKUP_TABLE_ELEMENT_NUM; ++i )
              table[i] = static_cast<f32>( pLutPixel[i].r );
          }

          for( s32 i = 0; i < nn::gr::CTR::LookUpTable::LOOKUP_TABLE_ELEMENT_NUM; ++i )
            table[i] /= 255.0f;

          u32 samplingTail = nn::gr::CTR::LookUpTable::LOOKUP_TABLE_ELEMENT_NUM - 1;
          for( s32 i = 0; i < samplingTail; ++i )
          {
            linearTable[i] = (table[i] + table[i+1]) * 0.5f;
          }
          linearTable[samplingTail] = table[samplingTail];

          lookUpTable.SetTable( linearTable, 0.0f, false );
        }

        {
          memset( lookUpTableCommand.m_CommandBuffer, 0, sizeof(lookUpTableCommand.m_CommandBuffer) );

          u32 *pTail = lookUpTable.MakeCommand( lookUpTableCommand.m_CommandBuffer, (PicaDataFragLightSampler)samplerTable[i] );
          u32 length = (u32)pTail - (u32)lookUpTableCommand.m_CommandBuffer;
          u32 dummyCommand[] =  { 0x00000000, 0x00000000 };
          u32 returnCommand[] = { 0x00000001, 0x000f023d };

          if ( (length % 16) == 0 )
          {
            pTail[0] = dummyCommand[0];
            pTail[1] = dummyCommand[1];
            pTail[2] = returnCommand[0];
            pTail[3] = returnCommand[1];
            pTail += 4;
          }
          else
          {
            pTail[0] = returnCommand[0];
            pTail[1] = returnCommand[1];
            pTail += 2;
          }

          length = (u32)pTail - (u32)lookUpTableCommand.m_CommandBuffer;

          lookUpTableCommand.m_TextureName = lutFileName;
          lookUpTableCommand.m_TextureNameHash = lutTextureNameHash[i];
          lookUpTableCommand.m_CommandSize = length;

          s_LookUpTableCommandList.push_back( lookUpTableCommand );
        }
      }
    }

    {// マテリアル情報を書き込み
      scenegraph_resource::MaterialResourceNodeFactory::BinaryFileAccessor11::AttributeParamSlim attrParam;

      memset( &attrParam, 0, sizeof(attrParam) );

      {
        for( int stageNo = 0; stageNo < CombinerAttribute::CombinerMax; ++stageNo )
        {
          for( int colorType = 0; colorType <= CombinerColorType::A; ++colorType )
          {
            scenegraph_resource::MaterialResourceNode::TextureCombinerAttribute &rTextureCombinerAttribute = m_AttributeParam.m_TextureCombinerAttribute[colorType][stageNo];

            static const u32 srcNum[] =
            {
              1,//"Replace",
              2,//"Modulate",
              2,//"Add",
              2,//"AddSigned",
              3,//"Interpolate",
              2,//"Subtract",
              2,//"DotProduct3Rgb",
              2,//"DotProduct3Rgba",
              3,//"AddMult",
              3,//"MultAdd",
            };

            static const u32 s_PrimaryColorIndex = 5;//頂点カラー

            for( int srcNo = 0; srcNo < srcNum[rTextureCombinerAttribute.m_MathFormula]; ++srcNo )
            {
              //printf( "%s[%d][%d][%d] : %d\n", m_Name.c_str(), stageNo, colorType, srcNo, rTextureCombinerAttribute.m_InputSource[srcNo] );

              if ( rTextureCombinerAttribute.m_InputSource[srcNo] == s_PrimaryColorIndex )
              {
                switch( rTextureCombinerAttribute.m_Operand[srcNo] ){
                case 0://"Color",
                  attrParam.m_VertexColorEnable[0] = attrParam.m_VertexColorEnable[1] = attrParam.m_VertexColorEnable[2] = true;
                  break;
                case 1://"OneMinusColor",
                  attrParam.m_VertexColorEnable[0] = attrParam.m_VertexColorEnable[1] = attrParam.m_VertexColorEnable[2] = true;
                  break;
                case 2://"Alpha",
                  attrParam.m_VertexColorEnable[3] = true;
                  break;
                case 3://"OneMinusAlpha",
                  attrParam.m_VertexColorEnable[3] = true;
                  break;
                case 4://"Red",
                  attrParam.m_VertexColorEnable[0] = true;
                  break;
                case 5://"OneMinusRed",
                  attrParam.m_VertexColorEnable[0] = true;
                  break;
                case 6://"Green",
                  attrParam.m_VertexColorEnable[1] = true;
                  break;
                case 7://"OneMinusGreen",
                  attrParam.m_VertexColorEnable[1] = true;
                  break;
                case 8://"Blue",
                  attrParam.m_VertexColorEnable[2] = true;
                  break;
                case 9://"OneMinusBlue",
                  attrParam.m_VertexColorEnable[2] = true;
                  break;
                }
              }
            }
          }
        }
      }

      for( s32 i = 0; i < refTableNum; ++i )
        attrParam.m_LutTextureNameHash[i] = lutTextureNameHash[i];

      attrParam.m_BumpMapNo = m_AttributeParam.m_BumpMapNo;

      for( u32 stageNo = 0; stageNo < CombinerAttribute::CombinerMax; ++stageNo )
      {
        s8 constantColorIndex = m_AttributeParam.m_TextureCombinerConstant[stageNo];
        attrParam.m_ConstantColorIndex[stageNo] = constantColorIndex;
      }

      for( u32 stageNo = 0; stageNo < CombinerAttribute::CombinerMax; ++stageNo )
      {
        attrParam.m_ConstantColor[stageNo] = m_AttributeParam.m_ConstantColor[stageNo];
      }

      attrParam.m_LightSetNo = m_AttributeParam.m_LightSetNo;
      attrParam.m_VsLightingEnable = m_AttributeParam.m_VsLightingEnable;
      attrParam.m_PsLightingEnable = m_AttributeParam.m_PsLightingEnable;
      attrParam.m_FogEnable = m_AttributeParam.m_FogEnable;
      attrParam.m_FogNo = m_AttributeParam.m_FogNo;
      attrParam.m_UseShadowTexture = m_AttributeParam.m_UseShadowTexture;
      attrParam.m_EmissionColor = m_AttributeParam.m_MaterialColor.m_Emission;
      attrParam.m_AmbientColor = m_AttributeParam.m_MaterialColor.m_Ambient;
      attrParam.m_DiffuseColor = m_AttributeParam.m_MaterialColor.m_Diffuse;

      binary.Write( &attrParam, sizeof( attrParam ) );
    }

    if ( 0 )
    {// ブレンド情報を書き込み
      binary.Write( &m_BlendTest, sizeof( m_BlendTest ) );
    }

    {// UserData情報を書き込み
      binary.Write( &m_UserData, sizeof( m_UserData ) );

      {
        char message[64] = {0};
        sprintf_s( message, "m_UserData Size = %d\n", sizeof( m_UserData ) );
        //OutputDebugStringA( message );
      }
    }

    static PicaDataTextureMinFilter sc_minFilter[] =
    {
      PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_LINEAR,
      PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_LINEAR,
      PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_LINEAR,
      PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_LINEAR
    };

    static PicaDataTextureMagFilter sc_magFilter[] =
    {
      PICA_DATA_TEXTURE_MAG_FILTER_NEAREST,
      PICA_DATA_TEXTURE_MAG_FILTER_LINEAR,
      PICA_DATA_TEXTURE_MAG_FILTER_NEAREST,
      PICA_DATA_TEXTURE_MAG_FILTER_LINEAR
    };

    static const c8 *pFilterName[] = {
      "POINT", "LINEAR", "POINT_LINEAR", "LINEAR_LINEAR"
    };

    static PicaDataTextureWrap s_ctrTextureWrap[ gfl2::gfx::TextureAddress::NumberOf ] =
    {
      PICA_DATA_TEXTURE_WRAP_REPEAT, //  Wrap = 0,       //!< テクスチャサイズで割った余りを座標値としてとる。\nテクスチャを繰り返しパターンとして使用することになる
      PICA_DATA_TEXTURE_WRAP_MIRRORED_REPEAT,     //  Mirror,         //!< テクスチャサイズごとに折り返した座標値をとる。\nテクスチャを反転しながら繰り返すパターンとして使用することになる
      PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER,     //  Border,         //!< テクスチャ境界のテクセル値を読み出す
      PICA_DATA_TEXTURE_WRAP_CLAMP_TO_EDGE,       //  Clamp,          //!< [0, 1]の範囲にクランプした座標値をとる
    };

    {
      u32 textureNum = 0;

      for( u32 i = 0; i < m_TextureList.size(); ++i )
      {
        const Texture &rTexture = m_TextureList[i];
        if ( rTexture.m_SlotNo < 3 )
        {
          ++textureNum;
        }
      }

      binary.Write( &textureNum, sizeof( textureNum ) );

      for( u32 i = 0; i < m_TextureList.size(); ++i )
      {
        const Texture &rTexture = m_TextureList[i];

        if ( rTexture.m_SlotNo < 3 )
        {
          c8 *pathBuffer = new c8[ rFilePathList[rTexture.m_Usage].length() + 1 ];
          c8 nameBuffer[NAME_DATA_MAX];

          memset( nameBuffer, 0, sizeof(nameBuffer) );

          ParserUtil::GetFileName( pathBuffer, rFilePathList[rTexture.m_Usage].c_str() );
          ERR_RETURN( strlen(pathBuffer) < NAME_DATA_MAX );
          strcat_s( nameBuffer, pathBuffer );

          u8 nameLength = static_cast<u8>( strlen(pathBuffer) );
          util::FnvHash nameHash( reinterpret_cast<u8*>(nameBuffer) );
          u32 hash = nameHash.value;
          u8 slotNo = rTexture.m_SlotNo;
          u8 mappintTypeNo = rTexture.m_Attribute.m_MappintTypeNo;

          binary.Write( &hash, sizeof( u32 ) );
          binary.Write( &nameLength, sizeof(u8) );
          binary.Write( pathBuffer, nameLength );
          binary.Write( &slotNo, sizeof(u8) );
          binary.Write( &mappintTypeNo, sizeof(u8) );

          binary.Write( &rTexture.m_Attribute.m_ScaleU, sizeof( rTexture.m_Attribute.m_ScaleU ) );
          binary.Write( &rTexture.m_Attribute.m_ScaleV, sizeof( rTexture.m_Attribute.m_ScaleV ) );
          binary.Write( &rTexture.m_Attribute.m_Rotate, sizeof( rTexture.m_Attribute.m_Rotate ) );
          binary.Write( &rTexture.m_Attribute.m_TranslateU, sizeof( rTexture.m_Attribute.m_TranslateU ) );
          binary.Write( &rTexture.m_Attribute.m_TranslateV, sizeof( rTexture.m_Attribute.m_TranslateV ) );

          u32 repeatUNo = s_ctrTextureWrap[ rTexture.m_Attribute.m_RepeatUNo ];
          u32 repeatVNo = s_ctrTextureWrap[ rTexture.m_Attribute.m_RepeatVNo ];
          u32 magFilterNo;
          u32 minFilterNo;

          for( u32 no = 0; no < ARRAY_SIZE(pFilterName); ++no )
          {
            if ( rTexture.m_Attribute.m_MagFilter == pFilterName[no] )
              magFilterNo = sc_magFilter[no];
            if ( rTexture.m_Attribute.m_MinFilter == pFilterName[no] )
              minFilterNo = sc_minFilter[no];
          }

          binary.Write( &repeatUNo, sizeof( repeatUNo ) );
          binary.Write( &repeatVNo, sizeof( repeatVNo ) );
          binary.Write( &magFilterNo, sizeof( magFilterNo ) );
          binary.Write( &minFilterNo, sizeof( minFilterNo ) );
          binary.Write( &rTexture.m_Attribute.m_MipBiasNo, sizeof( rTexture.m_Attribute.m_MipBiasNo ) );

          delete[] pathBuffer;
        }
      }
    }

    if ( binary.GetBinarySize() % 16 )
    {
      u32 paddingSize = 16 - (binary.GetBinarySize() %16 );
      u8 padding = 0xFF;

      for( u32 i = 0; i < paddingSize; ++i )
        binary.Write( &padding, sizeof( u8 ) );
    }

    {
      char message[64] = {0};
      sprintf_s( message, "attr Size = %d\n", binary.GetBinarySize() );
      //OutputDebugStringA( message );
    }

    //16 Byte Align
    //--------以下コマンド---------
    BinaryMaker commandBinaryMaker;

    {//定数バッファ設定
      u32 commandBuffer[256];
      u32 *pTail = commandBuffer;
      u32 length;
      u32 registerNo;

      if ( 0 )
      {//lightPowScale
        math::Vector4 lightPowScale( m_UserData.m_RimPow, m_UserData.m_RimScale, m_UserData.m_PhongPow, m_UserData.m_PhongScale );
        registerNo = 22;
        pTail = Commandbuilder::MakeUniformCommandVS( commandBuffer, registerNo, lightPowScale );
        length = (u32)pTail - (u32)commandBuffer;
        commandBinaryMaker.Write( commandBuffer, length );
        ERR_RETURN( (commandBinaryMaker.GetBinarySize() % 8) == 0 );
      }

      if ( 0 )
      {//ポケモンシェーダーのパラメーター。ここはランタイムでの設定になるので、いずれ削除
        static const u32 initData[] = {
          0x80000017, 0x804f02c0,//c23
          0xbc708462, 0x3f800000,
          0x00000000, 0x00000000,
          0x80000018, 0x804f02c0,
          0x424d4b3f, 0x00000000,
          0x00000000, 0x00000000,
          0x80000055, 0x804f02c0,
          0x00000000, 0x00000000,
          0x00000000, 0x00000000
        };

        commandBinaryMaker.Write( initData, sizeof(initData) );
        ERR_RETURN( (commandBinaryMaker.GetBinarySize() % 8) == 0 );
      }

      if ( m_TextureList.size() )
      {
        u32 colorTextureCnt = 0;
        math::Matrix uvMatList[3];
        math::Vector4 textureMapNo(0, 0, 0, 0);
        for( u32 i = 0; i < m_TextureList.size(); ++i )
        {
          const Texture &rTexture = m_TextureList[i];

          if ( rTexture.m_SlotNo > 2 )
            continue;

          math::Matrix uvMat;
          math::Matrix centerMat = math::Matrix::GetTranslation( math::Vector4( -0.5f, -0.5f, 0.0f, 1.0f ) );
          math::Matrix rotMat = math::Matrix::GetRotationZ( rTexture.m_Attribute.m_Rotate );
          math::Matrix offSetMat = math::Matrix::GetTranslation( math::Vector4( 0.5f, 0.5f, 0.0f, 1.0f ) );
          math::Matrix scaleMat = math::Matrix::GetScaling( math::Vector4(rTexture.m_Attribute.m_ScaleU, rTexture.m_Attribute.m_ScaleV, 1.0f) );
          math::Matrix transMat = math::Matrix::GetTranslation( math::Vector4( -rTexture.m_Attribute.m_TranslateU, -rTexture.m_Attribute.m_TranslateV, 0.0f, 1.0f ) );//Mayaはテクスチャが移動するので逆スクロール


          uvMatList[colorTextureCnt] = centerMat * rotMat * offSetMat * scaleMat * transMat;
          uvMatList[colorTextureCnt] = uvMatList[colorTextureCnt].Transpose();

          if ( rTexture.m_Attribute.m_MappintTypeNo == 0 )
          {
            textureMapNo[rTexture.m_SlotNo] = static_cast<f32>( rTexture.m_Attribute.m_UvSetNo );
          }
          else
          {//0～2の場合はテクスチャの割り当て、3,4の場合はそれぞれCube,SphereMap
            textureMapNo[rTexture.m_SlotNo] = rTexture.m_Attribute.m_MappintTypeNo + 2;
          }

          ++colorTextureCnt;
        }

        if ( colorTextureCnt )
        {
          {//TexcMap.xyz c0 テクスチャ座標割当
            registerNo = 0;
            pTail = Commandbuilder::MakeUniformCommandVS( commandBuffer, registerNo, textureMapNo );
            length = (u32)pTail - (u32)commandBuffer;
            commandBinaryMaker.Write( commandBuffer, length );
            ERR_RETURN( (commandBinaryMaker.GetBinarySize() % 8) == 0 );
          }

          {//TextureMatrix
            registerNo = 1;
            pTail = Commandbuilder::MakeUniformCommandVSMat34( commandBuffer, registerNo, uvMatList, colorTextureCnt );
            length = (u32)pTail - (u32)commandBuffer;
            commandBinaryMaker.Write( commandBuffer, length );
            ERR_RETURN( (commandBinaryMaker.GetBinarySize() % 8) == 0 );
          }
        }
      }
    }

#if 0
    {//コンバイナ計算式書き込み
      u32 pixelShaderCommand[1024];
      nn::gr::CTR::Combiner combiner;
      b32 bufferOut[CombinerColorType::NumberOf] = { false, false };

      for( int stageNo = 0; stageNo < CombinerAttribute::CombinerMax; ++stageNo )
      {
        nn::gr::CTR::Combiner::Stage &rStage = combiner.stage[stageNo];

        scenegraph_resource::MaterialResourceNode::TextureCombinerAttribute &rTextureCombinerAttribute = m_AttributeParam.m_TextureCombinerAttribute[CombinerColorType::RGB][stageNo];

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
          PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
          PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1,
          PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2,
          PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE3,
          PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
          PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
          PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP,
          PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_SECONDARY_COLOR_DMP,
          PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
          PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP
        };

        static const PicaDataTexEnvScale s_ScaleTable[] = {
          PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_2, PICA_DATA_TEX_ENV_SCALE_2, PICA_DATA_TEX_ENV_SCALE_4
        };

        static const PicaDataTexEnvCombine s_CombineTable[] = {
          PICA_DATA_TEX_ENV_COMBINE_REPLACE,
          PICA_DATA_TEX_ENV_COMBINE_MODULATE,
          PICA_DATA_TEX_ENV_COMBINE_ADD,
          PICA_DATA_TEX_ENV_COMBINE_ADD_SIGNED,
          PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE,
          PICA_DATA_TEX_ENV_COMBINE_SUBTRACT,
          PICA_DATA_TEX_ENV_COMBINE_DOT3_RGB,
          PICA_DATA_TEX_ENV_COMBINE_DOT3_RGBA,
          PICA_DATA_TEX_ENV_COMBINE_ADD_MULT_DMP,
          PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP
        };

        rStage.rgb.combine       = s_CombineTable[ rTextureCombinerAttribute.m_MathFormula ];
        rStage.rgb.operand[0]    = s_OperandRGBTable[ rTextureCombinerAttribute.m_Operand[InputSourceType::A] ];
        rStage.rgb.operand[1]    = s_OperandRGBTable[ rTextureCombinerAttribute.m_Operand[InputSourceType::B] ];
        rStage.rgb.operand[2]    = s_OperandRGBTable[ rTextureCombinerAttribute.m_Operand[InputSourceType::C] ];
        rStage.rgb.source[0]     = s_SourceTable[ rTextureCombinerAttribute.m_InputSource[InputSourceType::A] ];
        rStage.rgb.source[1]     = s_SourceTable[ rTextureCombinerAttribute.m_InputSource[InputSourceType::B] ];
        rStage.rgb.source[2]     = s_SourceTable[ rTextureCombinerAttribute.m_InputSource[InputSourceType::C] ];
        rStage.rgb.scale         = s_ScaleTable[ static_cast<u32>(rTextureCombinerAttribute.m_Scale) ];
        rStage.rgb.bufferInput   = bufferOut[CombinerColorType::RGB] ? PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS : PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
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
      ERR_RETURN( (commandBinaryMaker.GetBinarySize() % 8) == 0 );
    }
#endif

    {//レンダーステート
      nn::gr::CTR::RenderState renderState;

      {//ブレンド
        static const PicaDataBlendEquation sc_eqList[] = {
          PICA_DATA_BLEND_EQUATION_ADD, PICA_DATA_BLEND_EQUATION_SUBTRACT, PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT,PICA_DATA_BLEND_EQUATION_MIN, PICA_DATA_BLEND_EQUATION_MAX,
        };

        static const PicaDataBlendFunc sc_funcList[] = {
          PICA_DATA_BLEND_FUNC_ZERO, PICA_DATA_BLEND_FUNC_ONE, PICA_DATA_BLEND_FUNC_SRC_COLOR, PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR, PICA_DATA_BLEND_FUNC_DST_COLOR,
          PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR, PICA_DATA_BLEND_FUNC_SRC_ALPHA, PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA, PICA_DATA_BLEND_FUNC_DST_ALPHA,
          PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA, PICA_DATA_BLEND_FUNC_CONSTANT_COLOR, PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_COLOR, PICA_DATA_BLEND_FUNC_CONSTANT_ALPHA,
          PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_ALPHA, PICA_DATA_BLEND_FUNC_SRC_ALPHA_SATURATE
        };

        renderState.blend.isEnable = (m_BlendTest.m_Blender.m_BlendMode != BlendModeType::None) ? true : false;

        // RGB
        renderState.blend.eqRgb = sc_eqList[ m_BlendTest.m_Blender.m_Color.m_BlendFunc ];
        renderState.blend.srcRgb = sc_funcList[m_BlendTest.m_Blender.m_Color.m_SrcElement];
        renderState.blend.dstRgb = sc_funcList[m_BlendTest.m_Blender.m_Color.m_DstElement];

        // Alpha
        renderState.blend.eqAlpha = sc_eqList[ m_BlendTest.m_Blender.m_Alpha.m_BlendFunc ];
        renderState.blend.srcAlpha = sc_funcList[m_BlendTest.m_Blender.m_Alpha.m_SrcElement];
        renderState.blend.dstAlpha = sc_funcList[m_BlendTest.m_Blender.m_Alpha.m_DstElement];

        {
          math::Vector4 blendColor = m_AttributeParam.m_EtcColor.m_BlendColor.GetColor();
          blendColor *= 255.0f;

          renderState.blend.colorR = static_cast<u32>( blendColor[0] + 0.5f );
          renderState.blend.colorG = static_cast<u32>( blendColor[1] + 0.5f );
          renderState.blend.colorB = static_cast<u32>( blendColor[2] + 0.5f );
          renderState.blend.colorA = static_cast<u32>( blendColor[3] + 0.5f );
        }
      }

      {//アルファ
        static const PicaDataAlphaTest sc_funcList[] =
        {
          PICA_DATA_ALPHA_TEST_LESS,      // Less, //!< リファレンス値より小さい場合にのみパスさせる
          PICA_DATA_ALPHA_TEST_LEQUAL,    // LessEqual, //!< リファレンス値と同じか小さい場合にパスさせる
          PICA_DATA_ALPHA_TEST_EQUAL,     // Equal, //!< リファレンス値と同じ場合にはパスさせる
          PICA_DATA_ALPHA_TEST_GEQUAL,    // GreaterEqual, //!< リファレンス値と同じか大きい場合にパスさせる
          PICA_DATA_ALPHA_TEST_GREATER,   // Greater, //!< リファレンス値よりも大きい場合にパスさせる
          PICA_DATA_ALPHA_TEST_NOTEQUAL,  // NotEqual, //!< リファレンス値と同じではない場合にパスさせる
          PICA_DATA_ALPHA_TEST_ALWAYS,    // Always, //!< 常にパスさせる
          PICA_DATA_ALPHA_TEST_NEVER,     // Never, //!< パスさせない
        };

        f32					value = m_BlendTest.m_AlphaTest.m_AlphaTestRefValue * 255.0f;
        renderState.alphaTest.isEnable = (m_BlendTest.m_AlphaTest.m_AlphaTestType == gfx::CompareFunc::Always ) ? false : true;
        renderState.alphaTest.func = sc_funcList[m_BlendTest.m_AlphaTest.m_AlphaTestType];
        renderState.alphaTest.refValue = static_cast<u32>( value + 0.5f );
      }

      {//デプス
        static const PicaDataDepthTest sc_funcList[] =
        {
          PICA_DATA_DEPTH_TEST_LESS,      // Less, //!< リファレンス値より小さい場合にのみパスさせる
          PICA_DATA_DEPTH_TEST_LEQUAL,    // LessEqual, //!< リファレンス値と同じか小さい場合にパスさせる
          PICA_DATA_DEPTH_TEST_EQUAL,     // Equal, //!< リファレンス値と同じ場合にはパスさせる
          PICA_DATA_DEPTH_TEST_GEQUAL,    // GreaterEqual, //!< リファレンス値と同じか大きい場合にパスさせる
          PICA_DATA_DEPTH_TEST_GREATER,   // Greater, //!< リファレンス値よりも大きい場合にパスさせる
          PICA_DATA_DEPTH_TEST_NOTEQUAL,  // NotEqual, //!< リファレンス値と同じではない場合にパスさせる
          PICA_DATA_DEPTH_TEST_ALWAYS,    // Always, //!< 常にパスさせる
          PICA_DATA_DEPTH_TEST_NEVER,     // Never, //!< パスさせない
        };

        renderState.depthTest.isEnable = (m_BlendTest.m_DepthTest.m_DepthTestType == gfx::CompareFunc::Always ) ? false : true;
        renderState.depthTest.func = sc_funcList[m_BlendTest.m_DepthTest.m_DepthTestType];
        renderState.depthTest.isEnableWrite = m_BlendTest.m_DepthTest.m_UpDateDepth;
      }

      {//ステンシル
        static const PicaDataStencilTest sc_funcList[] =
        {
          PICA_DATA_STENCIL_TEST_LESS,      // Less, //!< リファレンス値より小さい場合にのみパスさせる
          PICA_DATA_STENCIL_TEST_LEQUAL,    // LessEqual, //!< リファレンス値と同じか小さい場合にパスさせる
          PICA_DATA_STENCIL_TEST_EQUAL,     // Equal, //!< リファレンス値と同じ場合にはパスさせる
          PICA_DATA_STENCIL_TEST_GEQUAL,    // GreaterEqual, //!< リファレンス値と同じか大きい場合にパスさせる
          PICA_DATA_STENCIL_TEST_GREATER,   // Greater, //!< リファレンス値よりも大きい場合にパスさせる
          PICA_DATA_STENCIL_TEST_NOTEQUAL,  // NotEqual, //!< リファレンス値と同じではない場合にパスさせる
          PICA_DATA_STENCIL_TEST_ALWAYS,    // Always, //!< 常にパスさせる
          PICA_DATA_STENCIL_TEST_NEVER,     // Never, //!< パスさせない
        };

        static const PicaDataStencilOp sc_opList[] =
        {
          PICA_DATA_STENCIL_OP_KEEP,        // Keep = 0, //!< 更新しない
          PICA_DATA_STENCIL_OP_ZERO,        // Zero, //!< ０に設定
          PICA_DATA_STENCIL_OP_REPLACE,     // Replace, //!< 参照値で置き換える
          PICA_DATA_STENCIL_OP_INCR,        // Increment, //!< 最大値に達するまで増加
          PICA_DATA_STENCIL_OP_INCR_WRAP,   // IncrementWrap,//!< 増加させその値が最大値を超えると０に戻します。
          PICA_DATA_STENCIL_OP_DECR,        // Decrement, //!< ０になるまで減少
          PICA_DATA_STENCIL_OP_DECR_WRAP,   // DecrementWrap,//!< 減少させその値が０より小さくなった場合は最大値に戻します。
          PICA_DATA_STENCIL_OP_INVERT,      // Invert, //!< ビットを反転
        };

        renderState.stencilTest.isEnable = m_BlendTest.m_StencilTest.m_StencilTestEnable;
        renderState.stencilTest.maskOp = 0xFF;

        // 3DSは必ず両面同時設定しかできない
        // 表面に設定されている設定を裏面にも使うことにする
        u32 faceNo = 0;
        renderState.stencilTest.func = sc_funcList[ m_BlendTest.m_StencilTest.m_StencilTestType ];
        renderState.stencilTest.ref = m_BlendTest.m_StencilTest.m_StencilValue;
        renderState.stencilTest.mask = m_BlendTest.m_StencilTest.m_StencilMask;

        renderState.stencilTest.opFail = sc_opList[ m_BlendTest.m_StencilTest.m_StencilFailOp ];
        renderState.stencilTest.opZFail = sc_opList[ m_BlendTest.m_StencilTest.m_StencilZFailOp ];
        renderState.stencilTest.opZPass = sc_opList[ m_BlendTest.m_StencilTest.m_StencilZpassOp ];
      }

      {//カリング
        static nn::gr::CTR::RenderState::Culling::CullFace sc_cullFaceList[] =
        {
          nn::gr::CTR::RenderState::Culling::CULL_FACE_BACK,   // None = 0, //!< 背面カリングしない
          nn::gr::CTR::RenderState::Culling::CULL_FACE_BACK,   // CCW, //!< 背面を反時計回りでカリング
          nn::gr::CTR::RenderState::Culling::CULL_FACE_FRONT,  // CW, //!< 背面を時計回りでカリング
        };

        //m_AttributeParam.m_CullMode = gfx::CullMode::None;
        renderState.cullingTest.isEnable = (m_AttributeParam.m_CullMode == gfx::CullMode::None) ? false : true;
        renderState.cullingTest.cullFace = sc_cullFaceList[ m_AttributeParam.m_CullMode ];
      }

      {// ポリゴンオフセット
        renderState.wBuffer.isEnablePolygonOffset = (m_BlendTest.m_DepthTest.m_PolygonOffset != 0);
        renderState.wBuffer.polygonOffsetUnit = m_BlendTest.m_DepthTest.m_PolygonOffset;
      }

      //カラーマスク初期化
      renderState.colorMask = nn::gr::CTR::RenderState::ColorMask::COLOR_MASK_RGBA;

      {
        u32 commandBuffer[32*2];
        memset( commandBuffer, 0, sizeof(commandBuffer) );
        u32 *pTail = renderState.MakeCommand(commandBuffer);
        u32 length = (u32)pTail - (u32)commandBuffer;
        commandBinaryMaker.Write( commandBuffer, length );
        ERR_RETURN( (commandBinaryMaker.GetBinarySize() % 8) == 0 );
      }
    }

    {//テクスチャステート設定
      nn::gr::CTR::Texture texture;

      for( u32 texNo = 0; texNo < m_TextureList.size(); ++texNo )
      {
        const Texture &rTexture = m_TextureList[texNo];

        nn::gr::CTR::Texture::UnitBase *pUnit = NULL;

        switch( rTexture.m_SlotNo ){
        case 0:
          pUnit = &texture.unit0;
          texture.unit0.texType = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D;//@todo ２Ｄテクスチャのみ対応
          break;
        case 1:
          pUnit = &texture.unit1;
          texture.unit1.texType = PICA_DATA_TEXTURE1_SAMPLER_TYPE_TEXTURE_2D;
          break;
        case 2:
          pUnit = &texture.unit2;
          texture.unit2.texType = PICA_DATA_TEXTURE2_SAMPLER_TYPE_TEXTURE_2D;
          break;
        default:
          continue;
        }

        pUnit->physicalAddr = 0;
        pUnit->format = PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP;
        pUnit->width = 0;
        pUnit->height = 0;

        for( u32 no = 0; no < ARRAY_SIZE(pFilterName); ++no )
        {
          if ( rTexture.m_Attribute.m_MagFilter == pFilterName[no] )	pUnit->magFilter = sc_magFilter[no];
          if ( rTexture.m_Attribute.m_MinFilter == pFilterName[no] )	pUnit->minFilter = sc_minFilter[no];
        }

        pUnit->wrapS = s_ctrTextureWrap[ rTexture.m_Attribute.m_RepeatUNo ];
        pUnit->wrapT = s_ctrTextureWrap[ rTexture.m_Attribute.m_RepeatVNo ];

        {
          math::Vector4 borderColor = m_AttributeParam.m_EtcColor.m_TextureBorderColor[texNo].GetColor();
          borderColor *= 255.0f;

          pUnit->borderColorR = static_cast<u32>( borderColor[0] + 0.5f );
          pUnit->borderColorG = static_cast<u32>( borderColor[1] + 0.5f );
          pUnit->borderColorB = static_cast<u32>( borderColor[2] + 0.5f );
          pUnit->borderColorA = static_cast<u32>( borderColor[3] + 0.5f );
        }

        pUnit->lodBias = 0;//todo ミップ初期値は０とする。これはテクスチャコマンドに積む
        pUnit->minLodLevel = 0;
        pUnit->maxLodLevel = 0;
      }

      {
        u32 commandBuffer[64*2];
        memset( commandBuffer, 0, sizeof(commandBuffer) );
        u32 *pTail = texture.MakeCommand(commandBuffer);
        u32 length = (u32)pTail - (u32)commandBuffer;
        commandBinaryMaker.Write( commandBuffer, length );
      }

      ERR_RETURN( (commandBinaryMaker.GetBinarySize() % 8) == 0 );
    }

    {//参照テーブル
      u32 commandBuffer[1024];

      u32 nwCommand[] = {
        0x02220222, 0x000f01d0,//absLutInputD0
        0x02220000, 0x000f01d1,//lutInputD0
        0x00000000, 0x000f01d2,//lutScaleD0
      };

      nwCommand[0] = PICA_CMD_DATA_FRAG_LIGHT_ABSLUTINPUT( false, false, false, false, false, false, false );
      nwCommand[1] = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAG_LIGHT_ABSLUTINPUT );

      nwCommand[2] = PICA_CMD_DATA_FRAG_LIGHT_ENV_LUTINPUT( (u32)m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution0].m_LutInputAngle,
        (u32)m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution1].m_LutInputAngle,
        PICA_DATA_FRAG_LIGHT_ENV_NH_DMP,
        PICA_DATA_FRAG_LIGHT_ENV_NH_DMP,
        (u32)m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionB].m_LutInputAngle,
        (u32)m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionG].m_LutInputAngle,
        (u32)m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionR].m_LutInputAngle );
      nwCommand[3] = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAG_LIGHT_LUTINPUT );

      class LocalFunc{
      public:
        static u32 GetLutScaleType( f32 scale )
        {
          if ( scale == 0.25f ) return PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_0_25;
          if ( scale == 0.5f ) return PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_0_5;
          if ( scale == 0.1f ) return PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_1_0;
          if ( scale == 0.2f ) return PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_2_0;
          if ( scale == 0.4f ) return PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_4_0;
          if ( scale == 0.8f ) return PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_8_0;
          return PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_1_0;
        }
      };

      nwCommand[4] = PICA_CMD_DATA_FRAG_LIGHT_ENV_LUTSCALE( LocalFunc::GetLutScaleType( m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution0].m_LutScale ),
        LocalFunc::GetLutScaleType( m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution1].m_LutScale ),
        PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_1_0,
        PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_1_0,
        LocalFunc::GetLutScaleType( m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionB].m_LutScale ),
        LocalFunc::GetLutScaleType( m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionG].m_LutScale ),
        LocalFunc::GetLutScaleType( m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionR].m_LutScale ) );

      nwCommand[5] = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAG_LIGHT_LUTSCALE );

      commandBinaryMaker.Write( nwCommand, sizeof(nwCommand) );
    }

    {
      u32 dummyCommand[] =  { 0x00000000, 0x00000000 };
      u32 returnCommand[] = { 0x00000001, 0x000f023d };
      u32 commandHeader[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

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

        length = commandBinaryMaker.GetBinarySize();

        u8 *pOutData = commandBinaryMaker.CreateFixedBuffer();
        util::FnvHash24 materialHash( pOutData, length );

        commandHeader[0] = length;
        commandHeader[1] = m_AttributeParam.m_Priority;
        commandHeader[2] = materialHash.value;
        commandHeader[3] = m_BlendTest.m_DrawLayer;
        commandHeader[4] = lutTextureNameHash[0];
        commandHeader[5] = lutTextureNameHash[1];
        commandHeader[6] = lutTextureNameHash[2];
        commandHeader[7] = gfl2::util::ConstHash< 'C', 'o', 'm', 'b', 'C', 't', 'r'>::value;

        binary.Write( commandHeader, sizeof(commandHeader) );
        binary.Write( pOutData, length );
        delete [] pOutData;

        {
          char message[64] = {0};
          sprintf_s( message, "Command Size = %d\nHash = %08x\n", length, commandHeader[2] );
          //OutputDebugStringA( message );
        }

        /*u32 *pData = (u32*)commandBinaryMaker.CreateFixedBuffer();
        for( int i = 0; i < length / sizeof(u32); i+=2 )
        {
        char message[64] = {0};
        sprintf_s( message, "0x%08x, 0x%08x\n", pData[i], pData[i+1] );
        OutputDebugStringA( message );
        }*/
      }
    }

    m_pBinaryData = binary.CreateFixedBuffer();
    m_BinarySize = binary.GetBinarySize();

    return true;
  }

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  JointData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  void JointsData::ReadJointDataNode( const xml_node<>* jointsNode )
  {
    if ( jointsNode == NULL )
      return;

    s32 replaceCount = 0;
    for( const xml_node<>* jointNode = jointsNode->first_node("Joint"); jointNode; jointNode = jointNode->next_sibling("Joint") )
    {
      JointData data;
      std::string str;
      std::wstring utf16;

      memset(&data,0,sizeof(data));

      {
        str = jointNode->first_attribute( "Name" )->value();

        // @todo End～をはじく @todo ikuta_junya追加
        std::regex excludeName("^End.*");
        if( std::regex_match( str, excludeName ) )
        {
          //printf( "exclude jointName=%s\n", str.c_str() );
          m_JointNoReplaceList.push_back( -1 );
          continue;
        }
        else
        {
          m_JointNoReplaceList.push_back( replaceCount );
          replaceCount += 1;
        }

        utf16 = ParserUtil::UTF8toUTF16( str );
        str = ParserUtil::UTF16toShitJIS( utf16 );
        strcpy_s( data.m_Name, str.c_str() );
      }

      xml_attribute<> *parentNameAttr  = jointNode->first_attribute( "ParentName" );
      if ( parentNameAttr )
      {
        str = parentNameAttr->value();
        utf16 = ParserUtil::UTF8toUTF16( str );
        str = ParserUtil::UTF16toShitJIS( utf16 );
        strcpy_s( data.m_ParentName, str.c_str() ); 
      }

      {
        str = jointNode->first_attribute( "NeededRendering" )->value();
        utf16 = ParserUtil::UTF8toUTF16( str );
        str = ParserUtil::UTF16toShitJIS( utf16 );
        data.m_BitStatus.m_NeededRendering = ( str == "TRUE" ) ? true : false;
      }

      const xml_node<> *localMatrixNode = jointNode->first_node( "LocalMatrix");
      if ( localMatrixNode )
      {
        std::vector<math::Vector4> col;
        math::Matrix matrix;
        ParserUtil::ParseFloatArray( &col, localMatrixNode, 4, 0.0f );
        for( u32 i = 0; i < 4; ++i )
        {
          matrix[i] = col[i];
        }
        data.m_LocalMatrix = matrix;
      }

      const xml_node<> *globalMatrixNode = jointNode->first_node( "GlobalMatrix");
      if ( globalMatrixNode )
      {
        std::vector<math::Vector4> col;
        math::Matrix matrix;
        ParserUtil::ParseFloatArray( &col, globalMatrixNode, 4, 0.0f );
        for( u32 i = 0; i < 4; ++i )
        {
          matrix[i] = col[i];
        }
        data.m_GlobalMatrix = matrix;
      }

      const xml_node<>	*localScale = jointNode->first_node( "LocalScale");
      if ( localScale )
      {
        std::vector<math::Vector4> col;
        ParserUtil::ParseFloatArray(&col, localScale, 1, 0.0f);

        data.m_LocalScale[0] = col[0].x;
        data.m_LocalScale[1] = col[1].x;
        data.m_LocalScale[2] = col[2].x;
      }

      const xml_node<> *localRotation = jointNode->first_node( "LocalRotation");
      if ( localRotation )
      {
        std::vector<math::Vector4> col;
        ParserUtil::ParseFloatArray(&col, localRotation, 1, 0.0f);

        data.m_LocalRotation[0] = col[0].x;
        data.m_LocalRotation[1] = col[1].x;
        data.m_LocalRotation[2] = col[2].x;
      }

      const xml_node<> *localTranslate = jointNode->first_node( "LocalTransform");
      if ( localTranslate )
      {
        std::vector<math::Vector4> col;
        ParserUtil::ParseFloatArray(&col, localTranslate, 1, 0.0f);

        data.m_LocalTranslate[0] = col[0].x;
        data.m_LocalTranslate[1] = col[1].x;
        data.m_LocalTranslate[2] = col[2].x;
      }

      m_JointDataList.push_back( data );
    }
  }

  void JointsData::ReadSkeltonDataNode( const xml_node<>* skeltonNode )
  {
    if ( skeltonNode == NULL )
      return;

    s32 replaceCount = 0;
    for( const xml_node<>* node = skeltonNode->first_node("Node"); node; node = node->next_sibling("Node") )
    {
      JointData data;
      std::string str;
      std::wstring utf16;

      memset(&data,0,sizeof(data));

      {
        str = node->first_attribute( "Name" )->value();

        // @todo End～をはじく @todo ikuta_junya追加
        std::regex excludeName("^End.*");
        if( std::regex_match( str, excludeName ) )
        {
          //printf( "exclude jointName=%s\n", str.c_str() );
          m_JointNoReplaceList.push_back( -1 );
          continue;
        }
        else
        {
          m_JointNoReplaceList.push_back( replaceCount );
          // g_JointNoReplaceList.push_back( oriCount );
          replaceCount += 1;
        }

        utf16 = ParserUtil::UTF8toUTF16( str );
        str = ParserUtil::UTF16toShitJIS( utf16 );
        strcpy_s( data.m_Name, str.c_str() );
      }

      xml_attribute<> *parentNameAttr  = node->first_attribute( "ParentName" );
      if ( parentNameAttr )
      {
        str = parentNameAttr->value();
        utf16 = ParserUtil::UTF8toUTF16( str );
        str = ParserUtil::UTF16toShitJIS( utf16 );
        strcpy_s( data.m_ParentName, str.c_str() ); 
      }

      {
        str = node->first_attribute( "NeededRendering" )->value();
        utf16 = ParserUtil::UTF8toUTF16( str );
        str = ParserUtil::UTF16toShitJIS( utf16 );
        data.m_BitStatus.m_NeededRendering = ( str == "TRUE" ) ? true : false;
      }

      xml_attribute<> *billboardNameAttr  = node->first_attribute( "BillboardType" );
      if(billboardNameAttr)
      {
        str = billboardNameAttr->value();
        utf16 = ParserUtil::UTF8toUTF16( str );
        str = ParserUtil::UTF16toShitJIS( utf16 );

        if(strncmp(str.c_str(),"AXISY",strlen("AXISY")) == 0) data.m_BitStatus.m_BillboardType = 1;
        else if(strncmp(str.c_str(),"AXISXY",strlen("AXISXY")) == 0) data.m_BitStatus.m_BillboardType = 2;
        else data.m_BitStatus.m_BillboardType = 0;
      }

      xml_attribute<> *compensateNameAttr  = node->first_attribute( "SegmentScaleCompensate" );
      if(compensateNameAttr)
      {
        str = compensateNameAttr->value();
        utf16 = ParserUtil::UTF8toUTF16( str );
        str = ParserUtil::UTF16toShitJIS( utf16 );
        data.m_BitStatus.m_ScaleSegmentCompensate = ( str == "TRUE" ) ? false : true;//バイナリ的には意味合いを反転。runtimeではデフォtrueになっていたため。
      }

      const xml_node<> *localMatrixNode = node->first_node( "LocalMatrix");
      if ( localMatrixNode )
      {
        std::vector<math::Vector4> col;
        math::Matrix matrix;
        ParserUtil::ParseFloatArray( &col, localMatrixNode, 4, 0.0f );
        for( u32 i = 0; i < 4; ++i )
        {
          matrix[i] = col[i];
        }
        data.m_LocalMatrix = matrix;
      }

      const xml_node<> *globalMatrixNode = node->first_node( "GlobalMatrix");
      if ( globalMatrixNode )
      {
        std::vector<math::Vector4> col;
        math::Matrix matrix;
        ParserUtil::ParseFloatArray( &col, globalMatrixNode, 4, 0.0f );
        for( u32 i = 0; i < 4; ++i )
        {
          matrix[i] = col[i];
        }
        data.m_GlobalMatrix = matrix;
      }

      const xml_node<> *localScale = node->first_node( "LocalScale");
      if ( localScale )
      {
        std::vector<math::Vector4> col;
        ParserUtil::ParseFloatArray(&col, localScale, 1, 0.0f);

        data.m_LocalScale[0] = col[0].x;
        data.m_LocalScale[1] = col[1].x;
        data.m_LocalScale[2] = col[2].x;
      }

      const xml_node<> *localRotation = node->first_node( "LocalRotation");
      if ( localRotation )
      {
        std::vector<math::Vector4> col;
        ParserUtil::ParseFloatArray(&col, localRotation, 1, 0.0f);

        data.m_LocalRotation[0] = col[0].x;
        data.m_LocalRotation[1] = col[1].x;
        data.m_LocalRotation[2] = col[2].x;
      }

      const xml_node<> *localTranslate = node->first_node( "LocalTransform");
      if ( localTranslate )
      {
        std::vector<math::Vector4> col;
        ParserUtil::ParseFloatArray(&col, localTranslate, 1, 0.0f);

        data.m_LocalTranslate[0] = col[0].x;
        data.m_LocalTranslate[1] = col[1].x;
        data.m_LocalTranslate[2] = col[2].x;
      }

      m_JointDataList.push_back( data );
    }
  }

  void JointsData::CreateBinary()
  {
    BinaryMaker binary;

    struct JointsDataHeader//!@todo 適切な場所から参照
    {//16バイトアラインサイズ
      s32 size;
      s32 dumy[3];
    };

    JointsDataHeader header;
    memset(&header,0,sizeof(header));

    header.size = static_cast<u32>(m_JointDataList.size());

    binary.Write( &header, sizeof(header) );

    for( u32 i = 0; i < m_JointDataList.size(); ++i )
    {
      binary.Write( &m_JointDataList[i], sizeof(JointData) );
    }

    m_pBinaryData = binary.CreateFixedBuffer();
    m_BinarySize = binary.GetBinarySize();
  }

  void JointsData::CreateBinaryCTR()
  {
    BinaryMaker binary;

    struct JointsDataHeader//!@todo 適切な場所から参照
    {//16バイトアラインサイズ
      s32 size;
      s32 dumy[3];
    };

    JointsDataHeader header;
    memset(&header,0,sizeof(header));

    header.size = static_cast<u32>(m_JointDataList.size());
    binary.Write( &header, sizeof(header) );

    for( u32 i = 0; i < m_JointDataList.size(); ++i )
    {
      u8 length;
      length = strlen(m_JointDataList[i].m_Name);
      binary.Write( &length, sizeof(length) );
      binary.Write( m_JointDataList[i].m_Name, length );

      length = strlen(m_JointDataList[i].m_ParentName);
      binary.Write( &length, sizeof(length) );
      binary.Write( m_JointDataList[i].m_ParentName, length );

      u8 f = m_JointDataList[i].m_Status[0];
      binary.Write( &f, sizeof(f) );

      binary.Write( m_JointDataList[i].m_LocalScale, sizeof(m_JointDataList[i].m_LocalScale) );
      binary.Write( m_JointDataList[i].m_LocalRotation, sizeof(m_JointDataList[i].m_LocalRotation) );
      binary.Write( m_JointDataList[i].m_LocalTranslate, sizeof(m_JointDataList[i].m_LocalTranslate) );
    }

    m_pBinaryData = binary.CreateFixedBuffer();
    m_BinarySize = binary.GetBinarySize();
  }


  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  MeshData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  const MeshData::typeInfo MeshData::s_TypeInfos[ MeshData::VertexDataType::NumberOf ] = {
    { "Position", 4 },
    { "Normal", 4 },
    { "BiNormal", 4 },
    { "Tangent", 4 },
    { "Uv", 4 },
    { "Color", 4 },
    { "JointNo", 4 },
    { "JointWeight", 4 },
  };

  bool MeshData::ReadMeshDataNode( const xml_node<>* top_node, const JointsData& rJointsData, const std::vector<MaterialData> &rMaterialDataList )
  {
    m_Path = top_node->first_attribute( "Name" )->value();

    {
      u32 size = static_cast<u32>( m_Path.length() ) + 1;
      c8 *pathBuffer = new c8[ size ];

      memset( pathBuffer, 0, size );
      ParserUtil::GetNodeName( pathBuffer, size, m_Path.c_str() );
      assert( strlen(pathBuffer) < NAME_DATA_MAX );

      m_Name = pathBuffer;

      delete[] pathBuffer;
    }

    m_IsBlendShape = false;
    xml_attribute<> *isBlendShapeAttr  = top_node->first_attribute( "IsBlendShape" );
    if ( isBlendShapeAttr )
    {
      if ( strcmp( isBlendShapeAttr->value(), "TRUE" ) == 0 )
      {
        m_IsBlendShape = true;
      }
    }

    m_IsVisibilityAnimation = false;
    xml_attribute<> *isVisibilityAnimationAttr  = top_node->first_attribute( "IsVisibilityAnimation" );
    if ( isVisibilityAnimationAttr )
    {
      if ( strcmp( isVisibilityAnimationAttr->value(), "TRUE" ) == 0 )
      {
        m_IsVisibilityAnimation = true;
      }
    }

    {
      const xml_node<>* aabb_node = top_node->first_node("AABB");
      if ( aabb_node )
      {
        std::vector<math::Vector4> aabb;
        ParserUtil::ParseFloatArray( &aabb, aabb_node, 3, 0.0f );
        m_AABB.SetMin( aabb[0] );
        m_AABB.SetMax( aabb[1] );
      }
    }

    {
      const xml_node<>* vertexdata_node = top_node->first_node("VertexData");

      for( const xml_node<>* vector_node = vertexdata_node->first_node();vector_node; vector_node = vector_node->next_sibling() )
      {
        VertexData vertexData;

        std::string vectorType = vector_node->name();

        vertexData.m_Usage = vector_node->first_attribute( "Usage" )->value();

        if ( vertexData.m_Usage == "BiNormal" )
          continue;//BiNormalはシェーダー内で生成

        if ( m_fUvSetAdjust )
        {
          if ( vertexData.m_Usage == "Uv" )
          {
            xml_attribute<>* pUseTextureNoList = vector_node->first_attribute("UseTextureNoList");
            std::string useTextureNoListStr = pUseTextureNoList->value();

            if ( useTextureNoListStr.length() == 0 )
              continue;

            xml_attribute<>* pName = vector_node->first_attribute("Name");
            vertexData.m_UvSetName = pName->value();
          }
        }

        if ( vectorType == "Vector2" )
        {
          ParserUtil::ParseFloatArray( &vertexData.m_DataArray, vector_node, 2, 0.0f );
        }
        else if ( vectorType == "Vector3" )
        {
          ParserUtil::ParseFloatArray( &vertexData.m_DataArray, vector_node, 3, 1.0f );
        }
        else if ( vectorType == "Vector4" )
        {
          ParserUtil::ParseFloatArray( &vertexData.m_DataArray, vector_node, 4, 0.0f );
        }

        m_VertexDataList.push_back( vertexData );
      }
    }

    {
      int uvCnt = 0;
      for( int i = 0; i < m_VertexDataList.size(); ++i )
      {
        if ( m_VertexDataList[i].m_Usage == "Uv" )
          uvCnt ++;
      }

      m_IsMultiUv = false;
      if ( uvCnt > 1 )
        m_IsMultiUv = true;
    }

    {//m_VertexDataListを並べなおす
      u32 vertexDataNum = static_cast<u32>( m_VertexDataList.size() );

      for( u32 i = 0; i < vertexDataNum; ++i )
      {
        const VertexData &rVertexData = m_VertexDataList[i];

        u32 usageNo = 0;
        for( u32 no = 0; no < ARRAY_SIZE(s_TypeInfos); ++no )
        {
          if ( rVertexData.m_Usage == s_TypeInfos[no].pName )
          {
            usageNo = no;
            break;
          }
        }

        u32 arraySize = static_cast<u32>( rVertexData.m_DataArray.size() );
        u32 elemens = s_TypeInfos[usageNo].elements;

        m_pVertexDataList[ usageNo ].push_back( &m_VertexDataList[i] );
      }
    }

    for( int i = 0; i < m_pVertexDataList[VertexDataType::Color].size(); ++i )
    {
      VertexData* pColor = m_pVertexDataList[VertexDataType::Color][i];

      for( int i2 = 0; i2 < pColor->m_DataArray.size(); ++i2 )
      {
        for( u32 vpos = 0; vpos < 4; ++vpos )
        {
          if ( pColor->m_DataArray[i2][vpos] < 0.0f )
            pColor->m_DataArray[i2][vpos] = 0.0f;
        }
      }
    }

    //
    // JointNo置き換え @todo ikuta_junya追加
    //
    if ( m_pVertexDataList[VertexDataType::JointNo].size() )
    {
      VertexData* pJointNo = m_pVertexDataList[VertexDataType::JointNo][0];
      // printf("Joint Size=%d\n", pJointNo->m_DataArray.size() );
      for( u32 i = 0; i < pJointNo->m_DataArray.size(); i++ )
      {
        for( u32 vpos = 0; vpos < 4; ++vpos )
        {
          // printf("%f ", pJointNo->m_DataArray[i][vpos] );
          if( pJointNo->m_DataArray[i][vpos] != -1 )
          {
            u32 jointNo = pJointNo->m_DataArray[i][vpos];

            if( jointNo >= rJointsData.m_JointNoReplaceList.size() || rJointsData.m_JointNoReplaceList[jointNo] == -1.0f )
            {
              // 置き換え情報が不正
              ERR_RETURN( 0 );
            }
            else
            {
              //printf( "JointNo replace!! %d -> %d \n", jointNo, rJointsData.m_JointNoReplaceList[ jointNo ] );
              pJointNo->m_DataArray[i][vpos] = rJointsData.m_JointNoReplaceList[ jointNo ];
            }
          }

        } // for vpos
      } // for pJointNo->m_DataArray.size()
    } // if ( m_pVertexDataList[VertexDataType::JointNo].size() )


    {
      const xml_node<>* faces_node = top_node->first_node("Faces");
      u32         faceIndex = 0;

      for( const xml_node<>* face_node = faces_node->first_node("Face");face_node; face_node = face_node->next_sibling("Face") )
      {
        FaceData faceData;
        std::vector<IndexData>	indexDataArray;

        faceData.m_MeshName = m_Name;
        faceData.m_FaceIndex = faceIndex;
        faceData.m_Material = face_node->first_attribute( "Material" )->value();

        for( const xml_node<>* index_node = face_node->first_node("int");index_node; index_node = index_node->next_sibling("int") )
        {
          IndexData indexData;
          indexData.m_Usage = index_node->first_attribute( "Usage" )->value();

          if ( m_fUvSetAdjust )
          {
            if ( indexData.m_Usage == "UvIndex" )
            {
              xml_attribute<>* pUvSetName = index_node->first_attribute("Name");
              std::string uvSetNameStr = pUvSetName->value();
              bool fUvEnable = false;

              for( int i = 0; i < m_VertexDataList.size(); ++i )
              {
                if ( m_VertexDataList[i].m_UvSetName == uvSetNameStr )
                {
                  fUvEnable = true;
                  break;
                }
              }

              if ( fUvEnable == false )
                continue;//UvSet削除済み。インデックス配列も出力不要。
            }
          }

          ParserUtil::ParseFloatArray( &indexData.m_DataArray, index_node, 3, 0.0f );
          indexDataArray.push_back( indexData );
        }

        {
          u32 targetMaterial = 0;
          for( u32 matNo = 0; matNo < rMaterialDataList.size(); ++matNo )
          {
            if ( faceData.m_Material == rMaterialDataList[matNo].m_Name )
            {
              targetMaterial = matNo;
              break;
            }
          }
          ERR_RETURN( faceData.CreateVertexBuffer( indexDataArray, m_pVertexDataList, m_Name, rMaterialDataList[targetMaterial], m_fUvSetAdjust ) );

          for( u32 vNo = 0; vNo < faceData.m_FaceVertexBuffer.m_Position.size(); ++vNo )
          {
            faceData.m_FaceIndexBuffer.push_back( vNo );
          }
        }

        m_FaceDataList.push_back( faceData );

        ++faceIndex;
      }
    }

    return true;
  }

  void MeshData::CalcAABB()
  {
    math::AABB aabb;

    for( u32 fNo = 0; fNo < m_FaceDataList.size(); ++fNo )
    {
      FaceData &rFaceData = m_FaceDataList[fNo];

      for( u32 vNo = 0; vNo < rFaceData.m_FaceVertexBuffer.m_Position.size(); ++vNo )
      {
        aabb.Merge( rFaceData.m_FaceVertexBuffer.m_Position[vNo] );
      }
    }

    m_AABB = aabb;
  }

  void MeshData::DivJointSize( u32 jointStartRegNo, u32 jointMax )
  {
    std::vector<MeshData::FaceData> newFaceDataList;

    for( u32 faceNo = 0; faceNo < m_FaceDataList.size(); ++faceNo )
    {
      MeshData::FaceData &rFaceData = m_FaceDataList[faceNo];
      std::vector<MeshData::FaceData> divFaceDataList;

      divFaceDataList = rFaceData.DivJointSize( jointMax );

      for( u32 divNo = 0; divNo < divFaceDataList.size(); ++divNo )
        newFaceDataList.push_back( divFaceDataList[divNo] );
    }

    m_FaceDataList = newFaceDataList;
  }

  void MeshData::Optimize( const std::vector<MaterialData> &rMaterialDataList )
  {
    for( u32 faceNo = 0; faceNo < m_FaceDataList.size(); ++faceNo )
    {
      FaceData &rFaceData = m_FaceDataList[faceNo];
      VertexBuffer &vertexBuffer = rFaceData.m_FaceVertexBuffer;
      u32 vertexSize = static_cast<u32>( vertexBuffer.m_Position.size() );
      BinaryMaker vertexBufferMaker;
      BinaryMaker indexBufferMaker;

      const MaterialData *pMaterialData = NULL;

      for( u32 materilNo = 0; materilNo < rMaterialDataList.size(); ++materilNo )
      {
        if ( rMaterialDataList[materilNo].m_Name == rFaceData.m_Material )
        {
          pMaterialData = &rMaterialDataList[materilNo];
          break;
        }
      }

      if ( pMaterialData->m_UserData.m_VertexShaderType == VertexShaderType::PokeFire )
      {
        rFaceData.m_FaceIndexBuffer.clear();

        for( int vno = 0; vno < rFaceData.m_FaceVertexBuffer.m_Position.size(); ++vno )
        {
          rFaceData.m_FaceIndexBuffer.push_back(vno);
        }
      }
    }
  }

  void MeshData::DataCheck( u32 jointStartRegNo, u32 jointMax )
  {
    for( u32 faceNo = 0; faceNo < m_FaceDataList.size(); ++faceNo )
    {
      MeshData::FaceData &rFaceData = m_FaceDataList[faceNo];
      rFaceData.DataCheck( jointStartRegNo, jointMax);
    }
  }

  void MeshData::CreateBinary( b32 fSizeOpt, const bool enableLog )
  {    
    BinaryMaker binary;

    c8 nameBuffer[NAME_DATA_MAX];

    {//メッシュ情報出力
      util::FnvHash hash( reinterpret_cast<const u8*>(m_Name.c_str()) );

      assert( m_Name.length() < NAME_DATA_MAX );
      memset( nameBuffer, 0, sizeof(nameBuffer) );
      memcpy(nameBuffer,m_Name.c_str(),strlen(m_Name.c_str())); // Garbage Data Fix

      binary.Write( &hash.value, sizeof( hash.value ) );
      binary.Write( nameBuffer, sizeof( nameBuffer ) );
      binary.Write( &m_IsBlendShape, sizeof( m_IsBlendShape ) );
      binary.Write( &m_AABB, sizeof( m_AABB ) );

      //printf( "%s Mesh\n", nameBuffer );
    }

    {//頂点宣言情報出力
      if ( this->m_IsBlendShape == false )
      {
        static const u32 vertexDataNum = 9;
        binary.Write( &vertexDataNum, sizeof( vertexDataNum ) );

        gfx::StreamUsage streamUsages[ vertexDataNum ] = 
        {
          gfx::StreamUsage::Position,
          gfx::StreamUsage::Normal,
          gfx::StreamUsage::Tangent,
          gfx::StreamUsage::TexCoord,
          gfx::StreamUsage::TexCoord,
          gfx::StreamUsage::TexCoord,
          gfx::StreamUsage::Color,
          gfx::StreamUsage::BlendIndices,
          gfx::StreamUsage::BlendWeight,
        };

        for ( u32 i = 0; i < vertexDataNum; ++i )
        {
          u32 usageNo = streamUsages[i];
          binary.Write( &usageNo, sizeof( usageNo ) );
        }
      }
      else
      {
        static const u32 vertexDataNum = 1;
        binary.Write( &vertexDataNum, sizeof( vertexDataNum ) );

        u32 usageNo = gfx::StreamUsage::Position;
        binary.Write( &usageNo, sizeof( usageNo ) );
      }
    }

    {//メッシュ出力
      u32 faceDataNum = static_cast<u32>( m_FaceDataList.size() );
      binary.Write( &faceDataNum, sizeof( faceDataNum ) );

      for( u32 faceNo = 0; faceNo < faceDataNum; ++faceNo )
      {
        FaceData &rFaceData = m_FaceDataList[faceNo];

        //printf( "face[%d]\n", faceNo );

        {
          util::FnvHash hash( reinterpret_cast<const u8*>(rFaceData.m_Material.c_str()) );

          assert( rFaceData.m_Material.length() < NAME_DATA_MAX );
          memset( nameBuffer, 0, sizeof(nameBuffer) );
          memcpy(nameBuffer,rFaceData.m_Material.c_str(),strlen(rFaceData.m_Material.c_str())); // Garbage Data Fix

          binary.Write( &hash.value, sizeof( hash.value ) );
          binary.Write( nameBuffer, sizeof( nameBuffer ) );
        }

        {//ジョイントの並び出力
          u8 jointNum = rFaceData.m_JointList.size();

          binary.Write( &jointNum, sizeof( jointNum ) );

          for( u32 jointCnt = 0; jointCnt < jointNum; ++jointCnt )
          {
            u8 jointNo = static_cast<u8>( rFaceData.m_JointList[jointCnt] );
            binary.Write( &jointNo, sizeof( jointNo ) );
          }
        }

        VertexBuffer &vertexBuffer = rFaceData.m_FaceVertexBuffer;

        if ( this->m_IsBlendShape == false )
        {//現在のシェーダーのフォーマットに合わせつつ、インターリーブ配列化して出力
          u32 vertexSize = static_cast<u32>( vertexBuffer.m_Position.size() );

          binary.Write( &vertexSize, sizeof( vertexSize ) );

          //printf( "vertexSize = %d\n", vertexSize );

          if ( fSizeOpt )
          {
            for( u32 vno = 0; vno < vertexSize; ++vno )
            {
              math::Vector4 dummy;

              binary.Write( &vertexBuffer.m_Position[vno], sizeof( short ) * 3 );

              if ( vertexBuffer.m_Normal.size() )
                binary.Write( &vertexBuffer.m_Normal[vno], sizeof( short ) * 3 );
              else
                binary.Write( &dummy, sizeof( short ) * 3 );

              if ( vertexBuffer.m_Tangent.size() )
                binary.Write( &vertexBuffer.m_Tangent[vno], sizeof( short ) * 3 );
              else
                binary.Write( &dummy, sizeof( short ) * 3 );

              for( u32 uvNo = 0; uvNo < 3; ++uvNo )
              {
                if ( vertexBuffer.m_Uv[uvNo].size() )
                {
                  math::Vector4 uv = vertexBuffer.m_Uv[uvNo][vno];
                  binary.Write( &uv, sizeof( short ) * 2 );
                }
              }

              for( u32 colorNo = 0; colorNo < 1; ++colorNo )
              {
                u8 rgba8[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

                if ( vertexBuffer.m_Color[colorNo].size() )
                {
                  for( int elmNo = 0; elmNo < 4; ++elmNo )
                    rgba8[elmNo] = static_cast<u8>( vertexBuffer.m_Color[colorNo][vno][elmNo] * 255.0f );
                }

                binary.Write( &rgba8, sizeof( rgba8 ) );
              }

              {
                u8 jointNo[4] = { 0, 0, 0, 0 };

                if ( vertexBuffer.m_JointNo.size() )
                {
                  for( int elmNo = 0; elmNo < 4; ++elmNo )
                    jointNo[elmNo] = static_cast<u8>( vertexBuffer.m_JointNo[vno][elmNo] );
                }

                binary.Write( &jointNo, sizeof( jointNo ) );
              }

              if ( vertexBuffer.m_JointWeights.size() )
                binary.Write( &vertexBuffer.m_JointWeights[vno], sizeof( float ) * 4 );
              else
                binary.Write( &dummy, sizeof( float ) * 4 );
            }
          }
          else
          {
            for( u32 vno = 0; vno < vertexSize; ++vno )
            {
              math::Vector4 dummy;

              binary.Write( &vertexBuffer.m_Position[vno], sizeof( math::Vector4 ) );

              if ( vertexBuffer.m_Normal.size() )
                binary.Write( &vertexBuffer.m_Normal[vno], sizeof( math::Vector4 ) );
              else
                binary.Write( &dummy, sizeof( math::Vector4 ) );

              if ( vertexBuffer.m_Tangent.size() )
                binary.Write( &vertexBuffer.m_Tangent[vno], sizeof( math::Vector4 ) );
              else
                binary.Write( &dummy, sizeof( math::Vector4 ) );

              for( u32 uvNo = 0; uvNo < 3; ++uvNo )
              {
                if ( vertexBuffer.m_Uv[uvNo].size() )
                {
                  math::Vector4 uv = vertexBuffer.m_Uv[uvNo][vno];
                  binary.Write( &uv, sizeof( math::Vector4 ) );
                }
                else
                  binary.Write( &dummy, sizeof( math::Vector4 ) );
              }

              for( u32 colorNo = 0; colorNo < 1; ++colorNo )
              {
                if ( vertexBuffer.m_Color[colorNo].size() )
                  binary.Write( &vertexBuffer.m_Color[colorNo][vno], sizeof( math::Vector4 ) );
                else
                  binary.Write( &dummy, sizeof( math::Vector4 ) );
              }

              if ( vertexBuffer.m_JointNo.size() )
                binary.Write( &vertexBuffer.m_JointNo[vno], sizeof( math::Vector4 ) );
              else
                binary.Write( &dummy, sizeof( math::Vector4 ) );

              if ( vertexBuffer.m_JointWeights.size() )
                binary.Write( &vertexBuffer.m_JointWeights[vno], sizeof( math::Vector4 ) );
              else
                binary.Write( &dummy, sizeof( math::Vector4 ) );
            }
          }
        }
        else
        {
          u32 vertexSize = static_cast<u32>( vertexBuffer.m_Position.size() );

          binary.Write( &vertexSize, sizeof( vertexSize ) );

          if ( fSizeOpt )
          {
            for( u32 vno = 0; vno < vertexSize; ++vno )
            {
              binary.Write( &vertexBuffer.m_Position[vno], sizeof( float ) * 3 );
            }
          }
          else
          {
            for( u32 vno = 0; vno < vertexSize; ++vno )
            {
              binary.Write( &vertexBuffer.m_Position[vno], sizeof( math::Vector4 ) );
            }
          }
        }

        {
          u32 indexSize = static_cast<u32>( rFaceData.m_FaceIndexBuffer.size() );

          binary.Write( &indexSize, sizeof( indexSize ) );

          //printf( "indexSize = %d\n", indexSize );

          if ( fSizeOpt )
          {
            for( u32 vno = 0; vno < indexSize; ++vno )
            {
              u16 data = rFaceData.m_FaceIndexBuffer[vno];
              binary.Write( &data, sizeof( u16 ) );//16bitインデックス
            }
          }
          else
          {
            for( u32 vno = 0; vno < indexSize; ++vno )
            {
              u32 data = rFaceData.m_FaceIndexBuffer[vno];
              binary.Write( &data, sizeof( u32 ) );//32bitインデックス
            }
          }
        }
      }
    }

    m_pBinaryData = binary.CreateFixedBuffer();
    m_BinarySize = binary.GetBinarySize();
  }

  nn::gr::CTR::Vertex MeshData::CreateGrVertex( FaceData& rFaceData )
  {
    static const u32 s_SymbolMax = 12;

    struct BindSymbol{
      nn::gr::CTR::BindSymbolVSInput::ShaderType shaderType;
      nn::gr::CTR::BindSymbolVSInput::SymbolType symbolType;
      u8 start;
      u8 end;
      char *name;
      b32 enable;
      b32 isConst;
    };

    //Ｎｗの並び
    BindSymbol bindSymbolList[s_SymbolMax] = {
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 0, 0, "aPosition.xyz", true, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 1, 1, "aNormal.xyz", true, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 4, 4, "aTexCoord0.xy", true, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 5, 5, "aTexCoord1.xy", true, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 6, 6, "aTexCoord2.xy", true, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 2, 2, "aTangent.xyz", true, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 3, 3, "aColor", true, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 7, 7, "aBoneIndex", true, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 8, 8, "aBoneWeight", true, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 9, 9, "aUserAttribute0.xyz", false, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 10, 10, "aUserAttribute1.xyz", false, false },
      { nn::gr::CTR::BindSymbolVSInput::ShaderType::SHADER_TYPE_VERTEX, nn::gr::CTR::BindSymbolVSInput::BindSymbol::SYMBOL_TYPE_INPUT, 11, 11, "aUserAttribute2.xyz", false, false }
    };

    gfl2::gfx::VertexElement vertexElement[s_SymbolMax] = {
      { 0, gfl2::gfx::StreamUsage::Position, 0, gfl2::gfx::VertexType::Float, 3 },
      { 0, gfl2::gfx::StreamUsage::Normal, 0, gfl2::gfx::VertexType::Float, 3 },
      { 0, gfl2::gfx::StreamUsage::TexCoord, 0, gfl2::gfx::VertexType::Float, 2 },
      { 0, gfl2::gfx::StreamUsage::TexCoord, 1, gfl2::gfx::VertexType::Float, 2 },

      { 0, gfl2::gfx::StreamUsage::TexCoord, 2, gfl2::gfx::VertexType::Float, 2 },
      { 0, gfl2::gfx::StreamUsage::Tangent, 0, gfl2::gfx::VertexType::Float, 3 },
      { 0, gfl2::gfx::StreamUsage::Color, 0, gfl2::gfx::VertexType::UByte, 4 },
      { 0, gfl2::gfx::StreamUsage::BlendIndices, 0, gfl2::gfx::VertexType::UByte, 4 },

      { 0, gfl2::gfx::StreamUsage::BlendWeight, 0, gfl2::gfx::VertexType::UByte, 4 },
      { 0, gfl2::gfx::StreamUsage::Color, 1, gfl2::gfx::VertexType::UByte, 4 },
      { 0, gfl2::gfx::StreamUsage::Color, 2, gfl2::gfx::VertexType::UByte, 4 },
      { 0, gfl2::gfx::StreamUsage::Color, 3, gfl2::gfx::VertexType::UByte, 4 },
    };

    gfl2::math::Vector4			dummyValue[s_SymbolMax] = {
      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::Position, 0, gfl2::gfx::VertexType::Float, 3 },
      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::Normal, 0, gfl2::gfx::VertexType::Float, 3 },
      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::TexCoord, 0, gfl2::gfx::VertexType::Float, 2 },
      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::TexCoord, 1, gfl2::gfx::VertexType::Float, 2 },

      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::TexCoord, 2, gfl2::gfx::VertexType::Float, 2 },
      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::Tangent, 0, gfl2::gfx::VertexType::Float, 3 },
      gfl2::math::Vector4( 255, 255, 255, 255 ),//{ 0, gfl2::gfx::StreamUsage::Color, 0, gfl2::gfx::VertexType::SByte, 1 },
      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::BlendWeight, 0, gfl2::gfx::VertexType::UByte, 4 },

      gfl2::math::Vector4( 0, 1, 2, 3 ),//{ 0, gfl2::gfx::StreamUsage::BlendIndices, 0, gfl2::gfx::VertexType::SByte, 1 },
      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::Color, 1, gfl2::gfx::VertexType::Float, 4 },
      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::Color, 2, gfl2::gfx::VertexType::Float, 4 },
      gfl2::math::Vector4( 0, 0, 0, 0 ),//{ 0, gfl2::gfx::StreamUsage::Color, 3, gfl2::gfx::VertexType::Float, 4 },
    };

    static const s32 s_FormatTable[][gfl2::gfx::VertexType::NumberOf] =
    {
      // 要素１つ
      {
        -1,                            //!< 16bit符号付整数を[-1...1]に正規化した値
          PICA_DATA_SIZE_1_FLOAT, //!< 32bit浮動少数値
          -1,            //!< 16bit浮動少数値
          -1,                        //!< 8bit符号なし整数を[0...1]に正規化した値
          PICA_DATA_SIZE_1_SHORT, //!< 16bit符号付整数
          -1,                          //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
          PICA_DATA_SIZE_1_UNSIGNED_BYTE, //!< 8bit符号なし整数値
          -1,                        //!< 8bit固定小数値(小数部6bit)
          -1,                          //!< 16bit固定小数値(小数部12bit)
          PICA_DATA_SIZE_1_BYTE,          //!< 8bit符号付整数値
      },
      // 要素２つ
      {
        -1,                            //!< 16bit符号付整数を[-1...1]に正規化した値
          PICA_DATA_SIZE_2_FLOAT, //!< 32bit浮動少数値
          -1,            //!< 16bit浮動少数値
          -1,                        //!< 8bit符号なし整数を[0...1]に正規化した値
          PICA_DATA_SIZE_2_SHORT,					//!< 16bit符号付整数
          -1,                          //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
          PICA_DATA_SIZE_2_UNSIGNED_BYTE,	//!< 8bit符号なし整数値
          -1,                        //!< 8bit固定小数値(小数部6bit)
          -1,                          //!< 16bit固定小数値(小数部12bit)
          PICA_DATA_SIZE_2_BYTE,          //!< 8bit符号付整数値
        },
        // 要素３つ
        {
          -1,                            //!< 16bit符号付整数を[-1...1]に正規化した値
            PICA_DATA_SIZE_3_FLOAT, //!< 32bit浮動少数値
            -1,            //!< 16bit浮動少数値
            -1,                        //!< 8bit符号なし整数を[0...1]に正規化した値
            PICA_DATA_SIZE_3_SHORT, //!< 16bit符号付整数
            -1,                          //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
            PICA_DATA_SIZE_3_UNSIGNED_BYTE, //!< 8bit符号なし整数値
            -1,                        //!< 8bit固定小数値(小数部6bit)
            -1,                          //!< 16bit固定小数値(小数部12bit)
            PICA_DATA_SIZE_3_BYTE,          //!< 8bit符号付整数値
        },
        // 要素４つ
        {
          -1,                            //!< 16bit符号付整数を[-1...1]に正規化した値
            PICA_DATA_SIZE_4_FLOAT, //!< 32bit浮動少数値
            -1,            //!< 16bit浮動少数値
            -1,                        //!< 8bit符号なし整数を[0...1]に正規化した値
            PICA_DATA_SIZE_4_SHORT, //!< 16bit符号付整数
            -1,                          //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
            PICA_DATA_SIZE_4_UNSIGNED_BYTE, //!< 8bit符号なし整数値
            -1,                        //!< 8bit固定小数値(小数部6bit)
            -1,                          //!< 16bit固定小数値(小数部12bit)
            PICA_DATA_SIZE_4_BYTE,          //!< 8bit符号付整数値
          },
    };

    nn::gr::CTR::BindSymbolVSInput vsBindSymbolList[s_SymbolMax];
    nn::gr::CTR::Vertex::InterleaveInfo interleaveInfo;
    u32 inputCnt = 0;
    nn::gr::CTR::Vertex grVertex;

    if ( rFaceData.m_FaceVertexBuffer.m_Normal.size() )
    {
      b32 fSameData = true;
      for ( s32 i = 0; i < rFaceData.m_FaceVertexBuffer.m_Normal.size(); ++i )
      {
        if ( rFaceData.m_FaceVertexBuffer.m_Normal[0] != rFaceData.m_FaceVertexBuffer.m_Normal[i] )
        {
          fSameData = false;
          break;
        }
      }

      if ( fSameData )
      {
        dummyValue[1] = rFaceData.m_FaceVertexBuffer.m_Normal[0];
        rFaceData.m_FaceVertexBuffer.m_Normal.clear();
        OutputDebugStringA( "delete Normal\n" );
      }
      else
      {
        OutputDebugStringA( "not delete Normal\n" );
      }
    }

    if ( rFaceData.m_FaceVertexBuffer.m_Tangent.size() )
    {
      b32 fSameData = true;
      for ( s32 i = 0; i < rFaceData.m_FaceVertexBuffer.m_Tangent.size(); ++i )
      {
        if ( rFaceData.m_FaceVertexBuffer.m_Tangent[0] != rFaceData.m_FaceVertexBuffer.m_Tangent[i] )
        {
          fSameData = false;
          break;
        }
      }

      if ( fSameData )
      {
        dummyValue[5] = rFaceData.m_FaceVertexBuffer.m_Tangent[0];
        rFaceData.m_FaceVertexBuffer.m_Tangent.clear();
        //OutputDebugStringA( "delete Tangent\n" );
      }
    }

    if ( rFaceData.m_FaceVertexBuffer.m_Color[0].size() )
    {
      b32 fSameData = true;
      for ( s32 i = 0; i < rFaceData.m_FaceVertexBuffer.m_Color[0].size(); ++i )
      {
        if ( rFaceData.m_FaceVertexBuffer.m_Color[0][0] != rFaceData.m_FaceVertexBuffer.m_Color[0][i] )
        {
          fSameData = false;
          break;
        }
      }

      if ( fSameData )
      {
        dummyValue[6] = rFaceData.m_FaceVertexBuffer.m_Color[0][0] * 255.0f;
        rFaceData.m_FaceVertexBuffer.m_Color[0].clear();
        //OutputDebugStringA( "delete Color\n" );
      }
    }

    if ( rFaceData.m_FaceVertexBuffer.m_JointNo.size() )
    {
      b32 fSameData = true;
      for ( s32 i = 0; i < rFaceData.m_FaceVertexBuffer.m_JointNo.size(); ++i )
      {
        if ( rFaceData.m_FaceVertexBuffer.m_JointNo[0] != rFaceData.m_FaceVertexBuffer.m_JointNo[i] )
        {
          fSameData = false;
          break;
        }
      }

      if ( fSameData )
      {
        dummyValue[7] = rFaceData.m_FaceVertexBuffer.m_JointNo[0];
        rFaceData.m_FaceVertexBuffer.m_JointNo.clear();
        //OutputDebugStringA( "delete Tangent\n" );
      }
    }

    if ( rFaceData.m_FaceVertexBuffer.m_JointWeights.size() )
    {
      b32 fSameData = true;
      for ( s32 i = 0; i < rFaceData.m_FaceVertexBuffer.m_JointWeights.size(); ++i )
      {
        if ( rFaceData.m_FaceVertexBuffer.m_JointWeights[0] != rFaceData.m_FaceVertexBuffer.m_JointWeights[i] )
        {
          fSameData = false;
          break;
        }
      }

      if ( fSameData )
      {
        dummyValue[8] = rFaceData.m_FaceVertexBuffer.m_JointWeights[0] * 255.0f;
        rFaceData.m_FaceVertexBuffer.m_JointWeights.clear();
        //OutputDebugStringA( "delete Tangent\n" );
      }
    }

    if ( rFaceData.m_FaceVertexBuffer.m_Normal.size() == 0  )
      bindSymbolList[1].isConst = true;

    if ( rFaceData.m_FaceVertexBuffer.m_Uv[0].size() == 0 )
      bindSymbolList[2].isConst = true;//uv0

    if ( rFaceData.m_FaceVertexBuffer.m_Uv[1].size() == 0  )
      bindSymbolList[3].isConst = true;//uv1

    if ( rFaceData.m_FaceVertexBuffer.m_Uv[2].size() == 0  )
      bindSymbolList[4].isConst = true;//uv2

    if ( rFaceData.m_FaceVertexBuffer.m_Tangent.size() == 0  )
      bindSymbolList[5].isConst = true;

    if ( rFaceData.m_FaceVertexBuffer.m_Color[0].size() == 0  )
      bindSymbolList[6].isConst = true;

    if ( rFaceData.m_FaceVertexBuffer.m_JointNo.size() == 0  )
      bindSymbolList[7].isConst = true;

    if ( rFaceData.m_FaceVertexBuffer.m_JointWeights.size() == 0  )
      bindSymbolList[8].isConst = true;

    for( u32 i = 0; i < s_SymbolMax; ++i )
    {
      if ( bindSymbolList[i].enable == false  )
        continue;

      vsBindSymbolList[i].shaderType = bindSymbolList[i].shaderType;
      vsBindSymbolList[i].symbolType = bindSymbolList[i].symbolType;
      vsBindSymbolList[i].start = bindSymbolList[i].start;
      vsBindSymbolList[i].end = bindSymbolList[i].end;
      vsBindSymbolList[i].name = bindSymbolList[i].name;

      if ( bindSymbolList[i].isConst  )
      {
        grVertex.EnableAttrAsConst( vsBindSymbolList[i], vertexElement[i].Size, &dummyValue[i].x );
      }
      else
      {
        interleaveInfo.symbol[inputCnt] = &vsBindSymbolList[i];

        u8 formatIndex = vertexElement[i].Size - 1;
        u8 formatType = vertexElement[i].Type;
        s32 dataType = s_FormatTable[formatIndex][formatType];
        interleaveInfo.dataType[inputCnt] = static_cast<PicaDataVertexAttrType>(dataType);

        ++inputCnt;
      }
    }
    interleaveInfo.dataNum = inputCnt;

    grVertex.EnableInterleavedArray( interleaveInfo, 0x9999 );

    return grVertex;
  }

  bool MeshData::CreateBinaryCTR( b32 fSizeOpt, const std::vector<MaterialData> &rMaterialDataList, const bool enableLog )
  {
    BinaryMaker binary;

    std::vector<BinaryMaker> commandBinaryList;
    c8 nameBuffer[NAME_DATA_MAX];

    {//メッシュ情報出力
      util::FnvHash hash( reinterpret_cast<const u8*>(m_Name.c_str()) );
      u32 padding = 0;
      u32 faceNum = m_FaceDataList.size();
      u32 weightMax = 0;

      for( u32 faceNo = 0; faceNo < m_FaceDataList.size(); ++faceNo )
      {
        FaceData &rFaceData = m_FaceDataList[faceNo];
        u32 weightNum = rFaceData.JointWeightMax();

        if ( weightMax < weightNum )
          weightMax = weightNum;
      }

      assert( m_Name.length() < NAME_DATA_MAX );
      memset( nameBuffer, 0, sizeof(nameBuffer) );
      memcpy(nameBuffer,m_Name.c_str(),strlen(m_Name.c_str())); // Garbage Data Fix


      binary.Write( &hash.value, sizeof( hash.value ) );//4
      binary.Write( nameBuffer, sizeof( nameBuffer ) );//64
      binary.Write( &m_IsBlendShape, sizeof( m_IsBlendShape ) );//4
      binary.Write( &m_AABB, sizeof( m_AABB ) );//32
      binary.Write( &faceNum, sizeof( faceNum ) );
      binary.Write( &weightMax, sizeof( weightMax ) );

      //printf( "weightMax = %d\n", weightMax );
    }

    if ( binary.GetBinarySize() % 128 )
    {
      u32 paddingSize = 128 - (binary.GetBinarySize() % 128 );
      u8 padding = 0xFF;

      for( u32 i = 0; i < paddingSize; ++i )
        binary.Write( &padding, sizeof( u8 ) );
    }

    {//３Ｄコマンド生成
      u32 commandBuffer[64*2];

      for( u32 faceNo = 0; faceNo < m_FaceDataList.size(); ++faceNo )
      {
        FaceData &rFaceData = m_FaceDataList[faceNo];
        nn::gr::CTR::Vertex grVertex = MeshData::CreateGrVertex( rFaceData );

        {
          BinaryMaker commandBinaryMaker;

          memset( commandBuffer, 0, sizeof(commandBuffer) );

          u32 *pTail = grVertex.MakeEnableAttrCommand(commandBuffer);//runtimeでのアドレス調整が必要
          u32 length = (u32)pTail - (u32)commandBuffer;
          commandBinaryMaker.Write( commandBuffer, length );
          assert( (commandBinaryMaker.GetBinarySize() % 8) == 0 );

          {
            ///char message[64] = {0};
            ///sprintf_s( message, "Mesh Command A = %08x, %08x, %08x, %08x\n", commandBuffer[0], commandBuffer[1], commandBuffer[2], commandBuffer[3] );
            //OutputDebugStringA( message );
          }

          commandBinaryList.push_back( commandBinaryMaker );
        }

        {
          BinaryMaker commandBinaryMaker;

          memset( commandBuffer, 0, sizeof(commandBuffer) );

          u32 *pTail = grVertex.MakeDisableAttrCommand(commandBuffer);
          u32 length = (u32)pTail - (u32)commandBuffer;
          commandBinaryMaker.Write( commandBuffer, length );
          assert( (commandBinaryMaker.GetBinarySize() % 8) == 0 );

          commandBinaryList.push_back( commandBinaryMaker );
        }

        {
          BinaryMaker commandBinaryMaker;
          u32 vertexSize = static_cast<u32>( rFaceData.m_FaceIndexBuffer.size() );

          nn::gr::CTR::Vertex::IndexStream indexStream;
          indexStream.physicalAddr = 0x99999999;
          indexStream.drawVtxNum = vertexSize;

          {
            memset( commandBuffer, 0, sizeof(commandBuffer) );

            u32 *pTail = grVertex.MakeDrawCommand(commandBuffer, indexStream);//runtimeでのアドレス調整が必要
            u32 length = (u32)pTail - (u32)commandBuffer;
            commandBinaryMaker.Write( commandBuffer, length );
            assert( (commandBinaryMaker.GetBinarySize() % 8) == 0 );

            commandBinaryList.push_back( commandBinaryMaker );
          }
        }
      }
    }

    for( u32 type = 0; type < commandBinaryList.size(); ++type )
    {
      u32 dummyCommand[] =  { 0x00000000, 0x00000000 };
      u32 returnCommand[] = { 0x00000001, 0x000f023d };
      u32 commandHeader[4] = { 0, 0, 0, 0 };

      {
        u32 length = commandBinaryList[type].GetBinarySize();

        if ( (length % 16) == 0 )
        {
          commandBinaryList[type].Write( dummyCommand, sizeof(dummyCommand) );
          commandBinaryList[type].Write( returnCommand, sizeof(returnCommand) );
        }
        else
        {
          commandBinaryList[type].Write( returnCommand, sizeof(returnCommand) );
        }

        length = commandBinaryList[type].GetBinarySize();
        commandHeader[0] = length;
        commandHeader[1] = type;
        commandHeader[2] = commandBinaryList.size();
        binary.Write( commandHeader, sizeof(commandHeader) );
        u8	*pOutData = commandBinaryList[type].CreateFixedBuffer();
        binary.Write( pOutData, length );
        delete [] pOutData;

        {
          ///char message[64] = {0};
          ///sprintf_s( message, "Mesh Command Size[%d] = %d\n", type, length );
          //OutputDebugStringA( message );
        }

        /*u32 *pData = (u32*)commandBinaryList[type].CreateFixedBuffer();
        for( int i = 0; i < length / sizeof(u32); i+=2 )
        {
        char message[64] = {0};
        sprintf_s( message, "0x%08x, 0x%08x\n", pData[i], pData[i+1] );
        OutputDebugStringA( message );
        }*/
      }
    }

    {//メッシュ出力
      u32 faceDataNum = static_cast<u32>( m_FaceDataList.size() );

      std::vector<BinaryMaker> vertexBufferMakerList;
      std::vector<BinaryMaker> indexBufferMakerList;

      for( u32 faceNo = 0; faceNo < faceDataNum; ++faceNo )
      {
        FaceData &rFaceData = m_FaceDataList[faceNo];
        VertexBuffer &vertexBuffer = rFaceData.m_FaceVertexBuffer;
        u32 vertexSize = static_cast<u32>( vertexBuffer.m_Position.size() );
        BinaryMaker vertexBufferMaker;
        BinaryMaker indexBufferMaker;

        const MaterialData *pMaterialData = NULL;

        for( u32 materilNo = 0; materilNo < rMaterialDataList.size(); ++materilNo )
        {
          if ( rMaterialDataList[materilNo].m_Name == rFaceData.m_Material )
          {
            pMaterialData = &rMaterialDataList[materilNo];
            break;
          }
        }

        /*{
        char message[64] = {0};
        sprintf_s( message, "vertexBufferSize = %d\n", vertexSize );
        OutputDebugStringA( message );
        }*/

        if ( this->m_IsBlendShape == false )
        {//現在のシェーダーのフォーマットに合わせつつ、インターリーブ配列化して出力

          for( u32 vno = 0; vno < vertexSize; ++vno )
          {
            math::Vector4 dummy(0, 0, 0, 0);

            vertexBufferMaker.Write( &vertexBuffer.m_Position[vno], sizeof( f32 ) * 3 );

            if ( vertexBuffer.m_Normal.size() )
              vertexBufferMaker.Write( &vertexBuffer.m_Normal[vno], sizeof( f32 ) * 3 );

            for( u32 uvNo = 0; uvNo < 3; ++uvNo )
            {
              if ( vertexBuffer.m_Uv[uvNo].size() )
              {
                math::Vector4 uv = vertexBuffer.m_Uv[uvNo][vno];
                vertexBufferMaker.Write( &uv, sizeof( f32 ) * 2 );
              }
            }

            if ( vertexBuffer.m_Tangent.size() )
              vertexBufferMaker.Write( &vertexBuffer.m_Tangent[vno], sizeof( f32 ) * 3 );


            {
              math::Vector4 vColorScale( 255.0f, 255.0f, 255.0f, 255.0f);

              if ( pMaterialData->m_UserData.m_VertexShaderType == VertexShaderType::Kusa )
                vColorScale.Set( 1.0f, 1.0f, 1.0f, 1.0f );

              if ( pMaterialData->m_UserData.m_VertexShaderType == VertexShaderType::PokeFire || pMaterialData->m_UserData.m_VertexShaderType == VertexShaderType::PokeFireShadow )
                vColorScale.Set( 255.0f / 3.0f, 255.0f / 3.0f, 255.0f / 3.0f, 255.0f );//rgbは0.0～3.0であるため。

              for( u32 colorNo = 0; colorNo < 1; ++colorNo )
              {
                u8 rgba8[4] = { 255, 255, 255, 255 };
                if ( vertexBuffer.m_Color[colorNo].size() )
                {
                  for( u32 elm = 0; elm < 4; ++elm )
                  {
                    rgba8[elm] = static_cast<u8>( vertexBuffer.m_Color[colorNo][vno][elm] * vColorScale[elm] );//0.0f ～1.0f -> 0 ～ 255
                  }

                  vertexBufferMaker.Write( rgba8, sizeof( rgba8 ) );
                }
              }
            }

            {
              u8 no8[4] = { 0, 0, 0, 0 };

              if ( vertexBuffer.m_JointNo.size() )
              {
                for( u32 elm = 0; elm < 4; ++elm )
                  no8[elm] = static_cast<u8>( vertexBuffer.m_JointNo[vno][elm] );

                vertexBufferMaker.Write( no8, sizeof( no8 ) );
              }
            }

            {
              u8 weight8[4] = { 255, 0, 0, 0 };//Nwでは頂点シェーダー内で1/100スケールされる。

              if ( vertexBuffer.m_JointWeights.size() )
              {
                u32 weightMax = 0;
                u32 elm = 0;
                for( elm = 0; elm < 4; ++elm )
                {
                  weight8[elm] = static_cast<u8>( vertexBuffer.m_JointWeights[vno][elm] * 255.0f );//0.0f ～1.0f -> 0 ～ 255
                  weightMax += weight8[elm];
                }

                elm = 0;
                while( weightMax < 255 )
                {
                  weight8[elm] ++;
                  ++weightMax;
                  ++elm;
                  elm %= 4;
                }

                vertexBufferMaker.Write( weight8, sizeof( weight8 ) );
              }
            }
          }
        }
        else
        {
          for( u32 vno = 0; vno < vertexSize; ++vno )
          {
            vertexBufferMaker.Write( &vertexBuffer.m_Position[vno], sizeof( f32 ) * 3 );
          }
        }

        {
          u32 indexSize = static_cast<u32>( rFaceData.m_FaceIndexBuffer.size() );

          for( u32 id = 0; id < indexSize; ++id )
          {
            u16 data = rFaceData.m_FaceIndexBuffer[id];
            indexBufferMaker.Write( &data, sizeof( u16 ) );//16bitインデックス
          }
        }

        u32 paddingSize;
        u8 pad8 = 0;
        {
          paddingSize = vertexBufferMaker.GetBinarySize() % 16;
          for( u32 pad = 0; pad < paddingSize; ++pad )
          {
            vertexBufferMaker.Write( &pad8, sizeof(pad8) );
          }
        }

        {
          paddingSize = indexBufferMaker.GetBinarySize() % 16;
          for( u32 pad = 0; pad < paddingSize; ++pad )
          {
            indexBufferMaker.Write( &pad8, sizeof(pad8) );
          }
        }

        vertexBufferMakerList.push_back( vertexBufferMaker );
        indexBufferMakerList.push_back( indexBufferMaker );
      }

      {//ヘッダー情報出力
        u32 padding = 0;

        for( u32 faceNo = 0; faceNo < faceDataNum; ++faceNo )
        {
          FaceData &rFaceData = m_FaceDataList[faceNo];

          {
            util::FnvHash hash( reinterpret_cast<const u8*>(rFaceData.m_Material.c_str()) );
            u32 nameLength = rFaceData.m_Material.length();

            assert( rFaceData.m_Material.length() < NAME_DATA_MAX );
            memset( nameBuffer, 0, sizeof(nameBuffer) );
            memcpy( nameBuffer, rFaceData.m_Material.c_str(), rFaceData.m_Material.length() );

            if ( nameLength % 4 )
            {
              nameLength += 4 - (nameLength % 4);//4byte単位
            }

            binary.Write( &hash.value, sizeof( hash.value ) );
            binary.Write( &nameLength, sizeof(nameLength) );
            binary.Write( nameBuffer, nameLength );//4Byte単位文字列バッファ

            {
              char message[64] = {0};
              sprintf_s( message, "Face[%d/%d] = %s, ", faceNo, faceDataNum, nameBuffer );
              //OutputDebugStringA( message );
            }
          }

          {//ジョイントの並び出力(32Byte)
            u8 jointNum = rFaceData.m_JointList.size();
            u8 jointListData[31];

            memset( jointListData, 0, sizeof(jointListData) );

            binary.Write( &jointNum, sizeof( jointNum ) );

            for( u32 jointCnt = 0; jointCnt < jointNum; ++jointCnt )
            {
              jointListData[jointCnt] = static_cast<u8>( rFaceData.m_JointList[jointCnt] );
            }

            binary.Write( jointListData, sizeof( jointListData ) );

            {
              char message[64] = {0};
              sprintf_s( message, "(%d, %d) ", jointNum, sizeof( jointListData ) );
              //OutputDebugStringA( message );
            }
          }

          {
            VertexBuffer &vertexBuffer = rFaceData.m_FaceVertexBuffer;
            u32 vertexSize = static_cast<u32>( vertexBuffer.m_Position.size() );
            u32 indexSize = static_cast<u32>( rFaceData.m_FaceIndexBuffer.size() );
            u32 vertexBufferSize = static_cast<u32>( vertexBufferMakerList[faceNo].GetBinarySize() );
            u32 indexBufferSize = static_cast<u32>( indexBufferMakerList[faceNo].GetBinarySize() );

            binary.Write( &vertexSize, sizeof( vertexSize ) );//4byte
            binary.Write( &indexSize, sizeof( indexSize ) );//4byte
            binary.Write( &vertexBufferSize, sizeof( vertexBufferSize ) );//4byte
            binary.Write( &indexBufferSize, sizeof( indexBufferSize ) );//4byte

            {
              char message[64] = {0};
              sprintf_s( message, "%d, %d\n", vertexSize, indexSize );
              //OutputDebugStringA( message );
            }
          }
        }

        for( u32 faceNo = 0; faceNo < faceDataNum; ++faceNo )
        {
          u8 *pData = NULL;

          pData = vertexBufferMakerList[faceNo].CreateFixedBuffer();
          binary.Write( pData, vertexBufferMakerList[faceNo].GetBinarySize() );
          delete[] pData;

          pData = indexBufferMakerList[faceNo].CreateFixedBuffer();
          binary.Write( pData, indexBufferMakerList[faceNo].GetBinarySize() );
          delete[] pData;
        }
      }
    }

    m_pBinaryData = binary.CreateFixedBuffer();
    m_BinarySize = binary.GetBinarySize();

    return true;
  }

  u32 MeshData::GetVertexCount( void ) const
  {
    u32 vertexCount = 0;

    for( u32 i = 0; i < m_FaceDataList.size(); ++i )
    {
      vertexCount += m_FaceDataList[i].m_FaceVertexBuffer.m_Position.size();
    }

    return vertexCount;
  }

  u32 MeshData::GetPolygonCount( void ) const
  {
    u32 polygonCount = 0;

    for( u32 i = 0; i < m_FaceDataList.size(); ++i )
    {
      polygonCount += m_FaceDataList[i].m_FaceIndexBuffer.size() / 3;
    }

    return polygonCount;
  }

  std::string MeshData::GetFaceConvertLog( const int level ) const
  {
    std::stringstream ss;

    for(auto iter = m_FaceDataList.begin(); iter < m_FaceDataList.end(); ++iter)
    {
      ss << std::string(level, '\t');
      ss << "<Face";
      ss << " PolygonCount=\"" << iter->m_FaceIndexBuffer.size() / 3 << "\"";
      ss << " VertexCount=\"" << iter->m_FaceVertexBuffer.m_Position.size() << "\"";
      ss << "/>" << std::endl;
    }

    return ss.str();
  }

  //! 頂点バッファ作成
  bool MeshData::FaceData::CreateVertexBuffer( std::vector<IndexData> indexDataArray, std::vector<VertexData*> pVertexDataList[ MeshData::VertexDataType::NumberOf ], std::string meshName, const MaterialData &rMaterialData, bool fUvSetAdjust )
  {
    class LocalFunc{
    public:
      static int GetUvIndex( const MaterialData &rMaterialData, std::string uvSetName )
      {
        int uvIndex = -1;

        for( int texNo = 0; texNo < rMaterialData.m_TextureList.size(); ++texNo )
        {
          const MaterialData::Texture &rTexture = rMaterialData.m_TextureList[texNo];

          if ( rTexture.m_Attribute.m_UvSetNameList.size() == 0 )
          {//uvChuser未設定の場合。つまりデフォルトＵＶ
            if ( uvIndex == -1 )//まだ未設定の場合。
              uvIndex = rTexture.m_Attribute.m_UvSetNo;
          }
          else
          {
            for( int uvSetNo = 0; uvSetNo < rTexture.m_Attribute.m_UvSetNameList.size(); ++uvSetNo )
            {
              if ( uvSetName == rTexture.m_Attribute.m_UvSetNameList[uvSetNo] )
              {
                uvIndex = rTexture.m_Attribute.m_UvSetNo;
              }
            }
          }
        }

        return uvIndex;
      }
    };

    static const c8* s_pIndexUsageNames[] = {
      "PositionJointWeightIndex",
      "NormalIndex",
      "TangentBiNormalIndex",
      "UvIndex",
      "ColorIndex"
    };

    u32 indexDataNum = static_cast<u32>( indexDataArray.size() );
    u32 indexLength = static_cast<u32>( indexDataArray[0].m_DataArray.size() );//長さはみな同じ
    u32 uvCount = 0;
    u32 colorCount = 0;

    for( u32 idx = 0; idx < indexDataNum; ++idx )
    {
      IndexData &rIndexData = indexDataArray[idx];

      u32 usageTypeNo = 0;
      for( u32 no = 0; no < ARRAY_SIZE(s_pIndexUsageNames); ++no )
      {
        if ( rIndexData.m_Usage == s_pIndexUsageNames[no] )
        {
          usageTypeNo = no;
          break;
        }
      }

      switch( usageTypeNo ){
      case 0://"PositionJointWeightIndex",
        {
          if ( pVertexDataList[VertexDataType::Position].size() )
          {
            VertexData *pPosition = pVertexDataList[VertexDataType::Position][0];
            for( u32 no = 0; no < indexLength; ++no )
            {
              for( u32 vpos = 0; vpos < 3; ++vpos )
              {//３頂点ごと
                u32 index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
                m_FaceVertexBuffer.m_Position.push_back( pPosition->m_DataArray[index] );
              }
            }
          }

          if ( pVertexDataList[VertexDataType::JointNo].size() )
          {//二つはセット
            VertexData *pJointNo = pVertexDataList[VertexDataType::JointNo][0];
            VertexData *pJointWeight = pVertexDataList[VertexDataType::JointWeights][0];

            for( u32 no = 0; no < indexLength; ++no )
            {
              for( u32 vpos = 0; vpos < 3; ++vpos )
              {//３頂点ごと
                u32 index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
                m_FaceVertexBuffer.m_JointNo.push_back(pJointNo->m_DataArray[index]);
                m_FaceVertexBuffer.m_JointWeights.push_back(pJointWeight->m_DataArray[index]);
              }
            }
          }
        }
        break;
      case 1://"NormalIndex",
        if ( pVertexDataList[VertexDataType::Normal].size() )
        {
          VertexData *pNormal = pVertexDataList[VertexDataType::Normal][0];

          for( u32 no = 0; no < indexLength; ++no )
          {
            for( u32 vpos = 0; vpos < 3; ++vpos )
            {//３頂点ごと
              u32 index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
              m_FaceVertexBuffer.m_Normal.push_back(pNormal->m_DataArray[index]);
            }
          }
        }
        break;
      case 2://"TangentBiNormalIndex",
        {
          if ( pVertexDataList[VertexDataType::Tangent].size() )
          {
            VertexData *pTangent = pVertexDataList[VertexDataType::Tangent][0];

            for( u32 no = 0; no < indexLength; ++no )
            {
              for( u32 vpos = 0; vpos < 3; ++vpos )
              {//３頂点ごと
                u32 index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
                m_FaceVertexBuffer.m_Tangent.push_back(pTangent->m_DataArray[index]);
              }
            }
          }
        }
        break;
      case 3://"UvIndex",
        {
          ERR_RETURN( uvCount < pVertexDataList[VertexDataType::Uv].size() );
          VertexData *pUv = pVertexDataList[VertexDataType::Uv][uvCount];

          int outUvNo = uvCount;

          if ( fUvSetAdjust )
          {
            std::string uvSetName = meshName + " " + pUv->m_UvSetName;
            outUvNo = LocalFunc::GetUvIndex( rMaterialData, uvSetName );

            ERR_RETURN( outUvNo >= 0 );
            ERR_RETURN( outUvNo < pVertexDataList[VertexDataType::Uv].size() );
          }

          for( u32 no = 0; no < indexLength; ++no )
          {
            for( u32 vpos = 0; vpos < 3; ++vpos )
            {//３頂点ごと
              u32 index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
              m_FaceVertexBuffer.m_Uv[outUvNo].push_back(pUv->m_DataArray[index]);
            }
          }
          ++uvCount;
        }
        break;
      case 4://"ColorIndex"
        {
          VertexData *pColor = pVertexDataList[VertexDataType::Color][colorCount];

          for( u32 no = 0; no < indexLength; ++no )
          {
            for( u32 vpos = 0; vpos < 3; ++vpos )
            {//３頂点ごと
              u32 index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
              m_FaceVertexBuffer.m_Color[colorCount].push_back(pColor->m_DataArray[index]);
            }
          }
          ++colorCount;
        }
        break;
      }
    }

    return true;
  }

  b32 MeshData::FaceData::Merge( const MeshData::FaceData &rSrc )
  {
    b32 fHasJoints = m_FaceVertexBuffer.m_JointNo.size() ? true : false;
    b32 fSrcHasJoints = rSrc.m_FaceVertexBuffer.m_JointNo.size() ? true : false;

    if ( fHasJoints != fSrcHasJoints )
      return false;//フォーマットが違うときはマテリアルが違うはず。スキニングするしないは、頂点シェーダーで選択させよう。

    s32 indexOffSet = m_FaceVertexBuffer.m_Position.size();

    //頂点バッファ内容を後ろに追加
    for( u32 vNo = 0; vNo < rSrc.m_FaceVertexBuffer.m_Position.size(); ++vNo )
    {
      m_FaceVertexBuffer.m_Position.push_back( rSrc.m_FaceVertexBuffer.m_Position[vNo] );

      if ( m_FaceVertexBuffer.m_Normal.size() )
      {
        if ( rSrc.m_FaceVertexBuffer.m_Normal.size() )
          m_FaceVertexBuffer.m_Normal.push_back( rSrc.m_FaceVertexBuffer.m_Normal[vNo] );
        else
          m_FaceVertexBuffer.m_Normal.push_back( math::Vector4(0, 0, 1) );
      }

      if ( m_FaceVertexBuffer.m_Tangent.size() )
      {
        if ( rSrc.m_FaceVertexBuffer.m_Tangent.size() )
        {
          m_FaceVertexBuffer.m_Tangent.push_back( rSrc.m_FaceVertexBuffer.m_Tangent[vNo] );
        }
        else
        {
          m_FaceVertexBuffer.m_Tangent.push_back( math::Vector4(0, 0, 1) );
        }
      }

      for( u32 uvSetNo = 0; uvSetNo < UV_SET_MAX; ++uvSetNo )
      {
        if ( m_FaceVertexBuffer.m_Uv[uvSetNo].size() )
        {
          if ( rSrc.m_FaceVertexBuffer.m_Uv[uvSetNo].size() )
            m_FaceVertexBuffer.m_Uv[uvSetNo].push_back( rSrc.m_FaceVertexBuffer.m_Uv[uvSetNo][vNo] );
          else
            m_FaceVertexBuffer.m_Uv[uvSetNo].push_back( math::Vector4(0, 0, 0) );
        }
      }

      for( u32 uvSetNo = 0; uvSetNo < COLOR_SET_MAX; ++uvSetNo )
      {
        if ( m_FaceVertexBuffer.m_Color[uvSetNo].size() )
        {
          if ( rSrc.m_FaceVertexBuffer.m_Color[uvSetNo].size() )
            m_FaceVertexBuffer.m_Color[uvSetNo].push_back( rSrc.m_FaceVertexBuffer.m_Color[uvSetNo][vNo] );
          else
            m_FaceVertexBuffer.m_Color[uvSetNo].push_back( math::Vector4(0, 0, 0) );
        }
      }

      if ( m_FaceVertexBuffer.m_JointNo.size() && rSrc.m_FaceVertexBuffer.m_JointNo.size() )
      {
        m_FaceVertexBuffer.m_JointNo.push_back( rSrc.m_FaceVertexBuffer.m_JointNo[vNo] );
        m_FaceVertexBuffer.m_JointWeights.push_back( rSrc.m_FaceVertexBuffer.m_JointWeights[vNo] );
      }
    }

    //インデックスをずらして追加
    for( s32 i = 0; i < rSrc.m_FaceIndexBuffer.size(); ++i )
    {
      s32 index = indexOffSet + rSrc.m_FaceIndexBuffer[i];
      m_FaceIndexBuffer.push_back( index );
    }

    return true;
  }

  void MeshData::FaceData::VertexSizeOptimize(const MaterialData* pMaterialData)
  {
    static const u32 MaterialTextureMax = CombinerAttribute::UvTextures::NumberOf;
    b32 uvEnable[MaterialTextureMax] = { false, false, false };
    for( u32 texNo = 0; texNo < pMaterialData->m_TextureList.size(); ++texNo )
    {
      const MaterialData::Texture &rTexture = pMaterialData->m_TextureList[texNo];

      if ( rTexture.m_Attribute.m_UvSetNo < MaterialTextureMax )
        uvEnable[rTexture.m_Attribute.m_UvSetNo] = true;
    }

    for( u32 uvNo = 0; uvNo < MaterialTextureMax; ++uvNo )
    {
      if ( uvEnable[uvNo] == false )
      {
        m_FaceVertexBuffer.m_Uv[uvNo].clear();
      }
    }

    if ( pMaterialData->m_AttributeParam.m_BumpMapNo == -1 )
    {
      m_FaceVertexBuffer.m_Tangent.clear();
    }
  }

  void MeshData::FaceData::VertexMerge(const MaterialData* pMaterialData)
  {
    /// std::vector<VertexCacheData> vertexBuffer;
    VertexCacheData *vertexBuffer;
    int vertexBufferSize = m_FaceVertexBuffer.m_Position.size();
    vertexBuffer = new VertexCacheData[vertexBufferSize];

    /// std::vector<VertexCacheData> vertexBufferOpt;
    VertexCacheData *vertexBufferOpt = new VertexCacheData[vertexBufferSize];
    int vertexBufferOptSize = 0;


    for( u32 vno = 0; vno < m_FaceVertexBuffer.m_Position.size(); ++vno )
    {
      /// VertexCacheData vertex;

      vertexBuffer[vno].m_Position = m_FaceVertexBuffer.m_Position[vno];

      if (m_FaceVertexBuffer.m_Normal.size() )
        vertexBuffer[vno].m_Normal = m_FaceVertexBuffer.m_Normal[vno];

      if ( m_FaceVertexBuffer.m_Tangent.size() )
        vertexBuffer[vno].m_Tangent = m_FaceVertexBuffer.m_Tangent[vno];

      for( u32 i = 0; i < MeshData::UV_SET_MAX; ++i )
      {
        if( m_FaceVertexBuffer.m_Uv[i].size() )
          vertexBuffer[vno].m_Uv[i] = m_FaceVertexBuffer.m_Uv[i][vno];
      }

      for( u32 i = 0; i < MeshData::COLOR_SET_MAX; ++i )
      {
        if ( m_FaceVertexBuffer.m_Color[i].size() )
          vertexBuffer[vno].m_Color[i] = m_FaceVertexBuffer.m_Color[i][vno];
      }

      if ( m_FaceVertexBuffer.m_JointNo.size() )
      {
        vertexBuffer[vno].m_JointNo = m_FaceVertexBuffer.m_JointNo[vno];
        vertexBuffer[vno].m_JointWeights = m_FaceVertexBuffer.m_JointWeights[vno];
      }

      /// vertexBuffer.push_back( vertex );

    }

    for( u32 i = 0; i < vertexBufferSize; ++i )
    {
      b32 fHit = false;
      for( u32 i2 = 0; i2 < vertexBufferOptSize; ++i2 )
      {
        if ( vertexBuffer[i] == vertexBufferOpt[i2] )
        {
          fHit = true;
          break;
        }
      }

      if ( fHit == false )
      {
        ///	vertexBufferOpt.push_back( vertexBuffer[i] );
        memcpy(&vertexBufferOpt[vertexBufferOptSize],&vertexBuffer[i],sizeof(VertexCacheData));
        vertexBufferOptSize++;

      }
    }

    /// std::vector<s32> faceIndexBufferOpt;
    int size = this->m_FaceIndexBuffer.size();

    for( u32 i = 0; i < size; ++i )
    {
      s32 index = m_FaceIndexBuffer[i];
      s32 origIndex = index;
      /// VertexCacheData vertex = vertexBuffer[index];

      index = -1;
      for( u32 i2 = 0; i2 < vertexBufferOptSize; ++i2 )
      {
        if ( vertexBuffer[origIndex] == vertexBufferOpt[i2] )
        {
          index = i2;
          break;
        }
      }
      assert( index != -1 );

      /// faceIndexBufferOpt.push_back( index );
      // OPTIMIZE:
      m_FaceIndexBuffer[i] = index;
    }

    {
      char message[64] = {0};
      sprintf_s( message, "vertexBufferOptSize = %d\n", vertexBufferOptSize );
      //OutputDebugStringA( message );
    }

    //差し替え
    ///	m_FaceIndexBuffer = faceIndexBufferOpt;
    // REMOVED FOR OPTIMIZATION

    VertexBuffer faceVertexBufferOpt;

    for( s32 vno = 0; vno < vertexBufferOptSize; ++vno )
    {
      faceVertexBufferOpt.m_Position.push_back( vertexBufferOpt[vno].m_Position );

      if (m_FaceVertexBuffer.m_Normal.size() )
        faceVertexBufferOpt.m_Normal.push_back( vertexBufferOpt[vno].m_Normal );

      if (m_FaceVertexBuffer.m_Tangent.size() )
        faceVertexBufferOpt.m_Tangent.push_back( vertexBufferOpt[vno].m_Tangent );

      for( s32 i = 0; i < UV_SET_MAX; ++i )
      {
        if (m_FaceVertexBuffer.m_Uv[i].size() )
          faceVertexBufferOpt.m_Uv[i].push_back( vertexBufferOpt[vno].m_Uv[i] );
      }

      for( s32 i = 0; i < COLOR_SET_MAX; ++i )
      {
        if (m_FaceVertexBuffer.m_Color[i].size() )
          faceVertexBufferOpt.m_Color[i].push_back( vertexBufferOpt[vno].m_Color[i] );
      }

      if (m_FaceVertexBuffer.m_JointNo.size() )
      {
        faceVertexBufferOpt.m_JointNo.push_back( vertexBufferOpt[vno].m_JointNo );
        faceVertexBufferOpt.m_JointWeights.push_back( vertexBufferOpt[vno].m_JointWeights );
      }
    }

    m_FaceVertexBuffer.m_Position = faceVertexBufferOpt.m_Position;
    m_FaceVertexBuffer.m_Normal = faceVertexBufferOpt.m_Normal;
    m_FaceVertexBuffer.m_Tangent = faceVertexBufferOpt.m_Tangent;
    for( s32 i = 0; i < UV_SET_MAX; ++i )
      m_FaceVertexBuffer.m_Uv[i] = faceVertexBufferOpt.m_Uv[i];
    for( s32 i = 0; i < COLOR_SET_MAX; ++i )
      m_FaceVertexBuffer.m_Color[i] = faceVertexBufferOpt.m_Color[i];
    m_FaceVertexBuffer.m_JointNo = faceVertexBufferOpt.m_JointNo;
    m_FaceVertexBuffer.m_JointWeights = faceVertexBufferOpt.m_JointWeights;

    delete[] vertexBuffer;
    if(vertexBufferOptSize!=0) delete[] vertexBufferOpt;
  }

  u32 MeshData::FaceData::JointWeightMax()
  {
    u32 jointWeightCntMax = 0;
    b32 fHit = false;

    for( int i = 0; i < m_FaceVertexBuffer.m_JointWeights.size(); ++i )
    {
      u32 jointWeightCnt = 0;

      for( int i2 = 0; i2 < 4; ++i2 )
      {
        if ( m_FaceVertexBuffer.m_JointWeights[i][i2] > 0.0f )
        {
          if ( i2 && fHit == false )
          {
            //printf( "Warning: ModelConverter detected extra weight value setting.\n" );
          }

          fHit = true;
          ++jointWeightCnt;
        }
        else
        {
          fHit = false;
        }
      }

      if ( jointWeightCntMax < jointWeightCnt )
        jointWeightCntMax = jointWeightCnt;
    }
    return jointWeightCntMax;
  }

  void MeshData::FaceData::Optimize( u32 vertexCacheSize )
  {

    VertexCacheData *vertexCacheDataList = new VertexCacheData[m_FaceIndexBuffer.size()];
    int vertexCacheDataListSize = 0;

    std::vector<s32> indexList;

    for( u32 i = 0; i < m_FaceIndexBuffer.size(); ++i )
    {
      VertexCacheData check;
      u32 index = m_FaceIndexBuffer[i];

      check.m_Position = m_FaceVertexBuffer.m_Position[index];

      if (m_FaceVertexBuffer.m_Normal.size() )
        check.m_Normal = m_FaceVertexBuffer.m_Normal[index];

      if ( m_FaceVertexBuffer.m_Tangent.size() )
      {
        check.m_Tangent = m_FaceVertexBuffer.m_Tangent[index];
      }

      for( u32 i = 0; i < MeshData::UV_SET_MAX; ++i )
      {
        if( m_FaceVertexBuffer.m_Uv[i].size() )
          check.m_Uv[i] = m_FaceVertexBuffer.m_Uv[i][index];
      }

      for( u32 i = 0; i < MeshData::COLOR_SET_MAX; ++i )
      {
        if ( m_FaceVertexBuffer.m_Color[i].size() )
          check.m_Color[i] = m_FaceVertexBuffer.m_Color[i][index];
      }

      if ( m_FaceVertexBuffer.m_JointNo.size() )
      {
        check.m_JointNo = m_FaceVertexBuffer.m_JointNo[index];
        check.m_JointWeights = m_FaceVertexBuffer.m_JointWeights[index];
      }

      {
        b32 fHit = false;
        for( u32 cacheNo = 0; cacheNo < vertexCacheDataListSize; ++cacheNo )
        {
          if ( vertexCacheDataList[cacheNo] == check )
          {
            vertexCacheDataList[cacheNo].m_HitCount ++;
            indexList.push_back( cacheNo );
            fHit = true;
            break;
          }
        }

        if ( fHit == false )
        {
          check.m_HitCount = 1;
          vertexCacheDataList[vertexCacheDataListSize] = check;
          indexList.push_back( vertexCacheDataListSize );
          vertexCacheDataListSize++;
        }
      }
    }


    class LocalFuncOptimized{
    public:
      LocalFuncOptimized( u32 vertexCacheSize,std::vector<s32> &indexList,VertexCacheData *vertexCData,int vertexCDataSize )
      {
        m_vertexCacheDataList = vertexCData;
        m_vertexCacheDataListSize = vertexCDataSize;

        m_VertexCacheSize = vertexCacheSize;

        // Use double buffer:
        m_indexCacheData[0] = new s32[vertexCacheSize];
        m_indexCacheData[1] = new s32[vertexCacheSize];
        m_indexCacheAIndex = 0; m_indexCacheBIndex = 1;
        m_indexCache = m_indexCacheData[m_indexCacheAIndex];
        m_indexCacheSize = vertexCacheSize;

        for( u32 i = 0; i < vertexCacheSize; ++i )
        {
          m_indexCache[i] = -1;
        }

        // Create Index List:
        m_indexListSize = indexList.size();
        m_indexList = new s32[m_indexListSize];
        for(int n=0;n<m_indexListSize;n++) m_indexList[n] = indexList[n];

        m_cacheIndexList = new s32[m_indexListSize];
        m_cacheIndexListSize = 0;

      }

      ~LocalFuncOptimized()
      {
        delete[] m_indexCacheData[0];
        delete[] m_indexCacheData[1];
        delete[] m_indexList;
        delete[] m_cacheIndexList;
      }

      void Optimize()
      {
        do
        {
          u32 indexTop = GetCacheHitIndex();

          for( u32 i = indexTop; i < (indexTop+3); ++i )
          {
            b32 fHit = false;
            for( u32 cacheNo = 0; cacheNo < m_VertexCacheSize; ++cacheNo )
            {
              if ( m_indexCache[cacheNo] == m_indexList[i] )
              {
                fHit = true;
                break;
              }
            }

            if ( fHit == false )
            {

              // Insert first item:
              m_indexCacheData[m_indexCacheBIndex][0] = m_indexList[i];
              // Copy from current buffer to back buffer:
              memcpy(m_indexCacheData[m_indexCacheBIndex]+1,m_indexCacheData[m_indexCacheAIndex],sizeof(s32)*(m_indexCacheSize-1) );

              // Reverse buffers:
              m_indexCacheBIndex = 1 - m_indexCacheBIndex;
              m_indexCacheAIndex = 1 - m_indexCacheAIndex;

              // Fix IndexCache Ptr:
              m_indexCache = m_indexCacheData[m_indexCacheAIndex];
            }

            m_cacheIndexList[m_cacheIndexListSize++] = m_indexList[i];

            m_indexList[i] = -2;
          }

        }while( m_cacheIndexListSize < m_indexListSize );
      }

      inline u32 GetCacheHitIndex()
      {
        u32 maxHitCnt = 0;
        u32 index = 0;

        for( u32 i = 0; i < m_indexListSize; i += 3 )
        {
          if ( m_indexList[i] < 0 )
            continue; // Output Complete

          u32 cacheHitCnt = 0;

          for( u32 i2 = 0; i2 < 3; ++i2 )
          {
            for( u32 cacheNo = 0; cacheNo < m_VertexCacheSize; ++cacheNo )
            {
              if ( m_indexList[i+i2] == m_indexCache[cacheNo] )
              {
                cacheHitCnt++;
                break;
              }
            }
          }

          if ( maxHitCnt < cacheHitCnt )
          {
            maxHitCnt = cacheHitCnt;
            index = i;

            if ( cacheHitCnt == 3 )
              break;
          }
        }

        if ( maxHitCnt )
          return index;

        maxHitCnt = 0;
        index = 0;

        for( u32 i = 0; i < m_indexListSize; i += 3 )
        {
          if ( m_indexList[i] < 0 )
            continue; // Output Complete

          u32 hitCnt = 0;

          hitCnt += m_vertexCacheDataList[ m_indexList[i] ].m_HitCount;
          hitCnt += m_vertexCacheDataList[ m_indexList[i+1] ].m_HitCount;
          hitCnt += m_vertexCacheDataList[ m_indexList[i+2] ].m_HitCount;

          if ( maxHitCnt < hitCnt )
          {
            maxHitCnt = hitCnt;
            index = i;
          }
        }
        return index;
      }

      s32 *m_indexCache;
      s32 *m_indexCacheData[2];
      int m_indexCacheSize,m_indexCacheAIndex,m_indexCacheBIndex;

      u32 m_VertexCacheSize;
      VertexCacheData *m_vertexCacheDataList;
      int m_vertexCacheDataListSize;

      s32 *m_indexList;
      int m_indexListSize;

      s32 *m_cacheIndexList;
      int m_cacheIndexListSize;
    };

    {
      u32 hitCnt = 0;
      for( u32 i = 0; i < vertexCacheDataListSize; ++i )
      {
        if ( hitCnt < static_cast<u32>(vertexCacheDataList[i].m_HitCount) )
          hitCnt = static_cast<u32>(vertexCacheDataList[i].m_HitCount);
      }

      // OPTIMIZED
      LocalFuncOptimized cache(vertexCacheSize,indexList,vertexCacheDataList,vertexCacheDataListSize);		
      cache.Optimize();

      m_FaceIndexBuffer.reserve(cache.m_cacheIndexListSize);
      memcpy(&m_FaceIndexBuffer[0],cache.m_cacheIndexList,sizeof(s32)*cache.m_cacheIndexListSize);
    }

    {
      if ( m_FaceVertexBuffer.m_Position.size() )
      {
        m_FaceVertexBuffer.m_Position.clear();

        for( u32 i = 0; i < vertexCacheDataListSize; ++i )
        {
          m_FaceVertexBuffer.m_Position.push_back( vertexCacheDataList[i].m_Position );			
        }
      }

      if ( m_FaceVertexBuffer.m_Normal.size() )
      {
        m_FaceVertexBuffer.m_Normal.clear();
        for( u32 i = 0; i < vertexCacheDataListSize; ++i )
        {
          m_FaceVertexBuffer.m_Normal.push_back( vertexCacheDataList[i].m_Normal );
        }
      }

      if ( m_FaceVertexBuffer.m_Tangent.size() )
      {
        m_FaceVertexBuffer.m_Tangent.clear();
        for( u32 i = 0; i < vertexCacheDataListSize; ++i )
        {
          m_FaceVertexBuffer.m_Tangent.push_back( vertexCacheDataList[i].m_Tangent );
        }
      }

      for( u32 uvNo = 0; uvNo < MeshData::UV_SET_MAX; ++uvNo )
      {
        if( m_FaceVertexBuffer.m_Uv[uvNo].size() )
        {
          m_FaceVertexBuffer.m_Uv[uvNo].clear();
          for( u32 i = 0; i < vertexCacheDataListSize; ++i )
          {
            m_FaceVertexBuffer.m_Uv[uvNo].push_back( vertexCacheDataList[i].m_Uv[uvNo] );
          }
        }
      }

      for( u32 colNo = 0; colNo < MeshData::COLOR_SET_MAX; ++colNo )
      {
        if( m_FaceVertexBuffer.m_Color[colNo].size() )
        {
          m_FaceVertexBuffer.m_Color[colNo].clear();
          for( u32 i = 0; i < vertexCacheDataListSize; ++i )
          {
            m_FaceVertexBuffer.m_Color[colNo].push_back( vertexCacheDataList[i].m_Color[colNo] );
          }
        }
      }

      if ( m_FaceVertexBuffer.m_JointNo.size() )
      {
        m_FaceVertexBuffer.m_JointNo.clear();
        m_FaceVertexBuffer.m_JointWeights.clear();
        for( u32 i = 0; i < vertexCacheDataListSize; ++i )
        {
          m_FaceVertexBuffer.m_JointNo.push_back( vertexCacheDataList[i].m_JointNo );
          m_FaceVertexBuffer.m_JointWeights.push_back( vertexCacheDataList[i].m_JointWeights );
        }
      }
    }

    delete[] vertexCacheDataList;

  }

  std::vector<u32> MeshData::FaceData::GetJointNoList()
  {
    std::vector<u32> jointNoList;

    for( u32 i = 0; i < m_FaceVertexBuffer.m_JointNo.size(); ++i )
    {
      math::Vector4 &rJointNo = m_FaceVertexBuffer.m_JointNo[i];

      for( u32 i2 = 0; i2 < 4; ++i2 )
      {
        s32 no = static_cast<s32>( rJointNo[i2] );
        if ( no >= 0 )
          jointNoList.push_back( no );
      }
    }

    return jointNoList;
  }

  void MeshData::FaceData::DataCheck( u32 jointStartRegNo, u32 jointMax )
  {
    for( u32 i = 0; i < m_FaceVertexBuffer.m_JointNo.size(); ++i )
    {
      math::Vector4 &rJointNo = m_FaceVertexBuffer.m_JointNo[i];
      math::Vector4 &rJointWeights = m_FaceVertexBuffer.m_JointWeights[i];

      for( u32 i2 = 0; i2 < 4; ++i2 )
      {
        u32 jointNo = rJointNo[i2];
        if( JointNo >= jointMax )
          MessageBox( NULL, "joint no over", "err", MB_OK );

        rJointNo[i2] = 0;
      }

      rJointWeights[1] = 0;
      rJointWeights[2] = 0;
      rJointWeights[3] = 0;

      float weightMax = 0.0f;
      for( u32 i2 = 0; i2 < 4; ++i2 )
      {
        if( rJointWeights[i2] < 0.0f )
          MessageBox( NULL, "joint weight ress", "err", MB_OK );

        if( rJointWeights[i2] > 1.0f )
          MessageBox( NULL, "joint weight over", "err", MB_OK );

        weightMax += rJointWeights[i2];
      }

      rJointWeights *= (1.0f / weightMax);

      if ( rJointWeights[0] != 0.0f )
      {
        //printf("rJointWeights = %f\n", rJointWeights[0]);
        rJointWeights[0] = 1.0f;
      }

      /*weightMax = 1.0f - weightMax;
      if ( weightMax < 0.0f )
      weightMax *= -1.0f;

      if( weightMax > 0.01f )//誤差許容
      MessageBox( NULL, "joint max weight over", "err", MB_OK );*/
    }
  }

  std::vector<MeshData::FaceData> MeshData::FaceData::DivJointSize( u32 jointMax )
  {
    class LocalFunc{
    public:
      static std::vector<VertexCacheData> GetTriangleList( const std::vector<s32> &rFaceIndexBuffer, const VertexBuffer &rFaceVertexBuffer )
      {
        std::vector<VertexCacheData> triangleList(rFaceIndexBuffer.size());

        for( u32 i = 0; i < rFaceIndexBuffer.size(); ++i )
        {
          u32 vno = rFaceIndexBuffer[i];

          triangleList[i].m_Position = rFaceVertexBuffer.m_Position[vno];

          if (rFaceVertexBuffer.m_Normal.size() )
            triangleList[i].m_Normal = rFaceVertexBuffer.m_Normal[vno];

          if ( rFaceVertexBuffer.m_Tangent.size() )
            triangleList[i].m_Tangent = rFaceVertexBuffer.m_Tangent[vno];

          for( u32 i2 = 0; i2 < MeshData::UV_SET_MAX; ++i2 )
          {
            if( rFaceVertexBuffer.m_Uv[i2].size() )
              triangleList[i].m_Uv[i2] = rFaceVertexBuffer.m_Uv[i2][vno];
          }

          for( u32 i2 = 0; i2 < MeshData::COLOR_SET_MAX; ++i2 )
          {
            if ( rFaceVertexBuffer.m_Color[i2].size() )
              triangleList[i].m_Color[i2] = rFaceVertexBuffer.m_Color[i2][vno];
          }

          if ( rFaceVertexBuffer.m_JointNo.size() )
          {
            triangleList[i].m_JointNo = rFaceVertexBuffer.m_JointNo[vno];
            triangleList[i].m_JointWeights = rFaceVertexBuffer.m_JointWeights[vno];
          }

        }
        return triangleList;
      }

      static void GetUsedJointListAndMerge( const math::Vector4 &rJointNo,u32 *list,u32 &listSize )
      {
        for( s32 i = 0; i < 4; ++i )
        {
          s32 no = static_cast<s32>( rJointNo[i] );
          if ( no >= 0 )
          {
            bool hit = false;
            for(int n=0;n<listSize;n++)
            {
              // Already Exists:
              if(list[n]==no)
              {
                hit=true;
                break;
              }
            }
            if(!hit)
            {
              list[listSize++] = no;
            }
          }
        }
      }


      static math::Vector4 ReNumbering( const std::vector<u32> &rListA, math::Vector4 jointNo )
      {
        math::Vector4 jointReNo(-1, -1, -1, -1);

        for( s32 i = 0; i < 4; ++i )
        {
          s32 nowNo = jointNo[i];

          if ( nowNo < 0 )
            continue;

          for( s32 i2 = 0; i2 < rListA.size(); ++i2 )
          {
            if ( nowNo == rListA[i2] )
            {
              jointReNo[i] = i2;//rListAの並び基準に変更。runtimeではrListAの順序で頂点シェーダーに転送
              jointNo[i] = -1.0f;
              break;
            }
          }
        }

        for( s32 i = 0; i < 4; ++i )
        {
          assert( jointNo[i] == -1.0f );
        }

        return jointReNo;
      }

      static MeshData::FaceData GetNewFace( const std::vector<u32> &rJointList, const VertexCacheData *rTriangleList, u32 rTriangleListSize,const VertexBuffer &rOrigFaceVertexBuffer )
      {
        MeshData::FaceData newFace;

        newFace.m_JointList = rJointList;

        for( u32 i = 0; i < rTriangleListSize; ++i )
        {
          newFace.m_FaceIndexBuffer.push_back( i );

          newFace.m_FaceVertexBuffer.m_Position.push_back( rTriangleList[i].m_Position );

          if (rOrigFaceVertexBuffer.m_Normal.size() )
            newFace.m_FaceVertexBuffer.m_Normal.push_back( rTriangleList[i].m_Normal );

          if ( rOrigFaceVertexBuffer.m_Tangent.size() )
            newFace.m_FaceVertexBuffer.m_Tangent.push_back( rTriangleList[i].m_Tangent );

          for( u32 i2 = 0; i2 < MeshData::UV_SET_MAX; ++i2 )
          {
            if( rOrigFaceVertexBuffer.m_Uv[i2].size() )
              newFace.m_FaceVertexBuffer.m_Uv[i2].push_back( rTriangleList[i].m_Uv[i2] );
          }

          for( u32 i2 = 0; i2 < MeshData::COLOR_SET_MAX; ++i2 )
          {
            if ( rOrigFaceVertexBuffer.m_Color[i2].size() )
              newFace.m_FaceVertexBuffer.m_Color[i2].push_back( rTriangleList[i].m_Color[i2] );
          }

          if ( rOrigFaceVertexBuffer.m_JointNo.size() )
          {//ジョイント番号再設定
            newFace.m_FaceVertexBuffer.m_JointNo.push_back( LocalFunc::ReNumbering( newFace.m_JointList, rTriangleList[i].m_JointNo ) );
            //newFace.m_FaceVertexBuffer.m_JointNo.push_back( rTriangleList[i].m_JointNo );
            newFace.m_FaceVertexBuffer.m_JointWeights.push_back( rTriangleList[i].m_JointWeights );
          }
        }

        return newFace;
      }

    };

    std::vector<MeshData::FaceData> divFaceDataList;
    std::vector<VertexCacheData> triangleList;//VertexCacheData構造体を利用

    //一旦リストにばらす
    triangleList = LocalFunc::GetTriangleList( m_FaceIndexBuffer, m_FaceVertexBuffer );

    VertexBuffer newFaceVertexBuffer;
    std::vector<b32> checkList;
    u32 pushCnt = triangleList.size();

    checkList.resize( triangleList.size() );
    for( s32 i = 0; i < checkList.size(); ++i )
      checkList[i] = false;

    u32 *usedJointListAll = new u32[jointMax+12];
    u32 *usedJointListAllBackup = new u32[jointMax];
    u32 usedJointListAllSize = 0;
    u32 usedJointListAllSizeBackup = 0;

    VertexCacheData *divVettexList = new VertexCacheData[triangleList.size()]; //VertexCacheData構造体を利用
    u32 divVettexListSize = triangleList.size();

    while( pushCnt )
    {
      std::vector<u32> newJointList;

      usedJointListAllSize = 0;
      int count = 0;

      for( s32 i = 0; i < triangleList.size(); i += 3 )
      {
        if ( checkList[i] )
          continue;//すでに出力済

        usedJointListAllSizeBackup = usedJointListAllSize;
        if(usedJointListAllSize) memcpy(usedJointListAllBackup,usedJointListAll,sizeof(u32)*usedJointListAllSize);

        LocalFunc::GetUsedJointListAndMerge( triangleList[i].m_JointNo,usedJointListAll,usedJointListAllSize);
        LocalFunc::GetUsedJointListAndMerge( triangleList[i+1].m_JointNo,usedJointListAll,usedJointListAllSize);
        LocalFunc::GetUsedJointListAndMerge( triangleList[i+2].m_JointNo,usedJointListAll,usedJointListAllSize);

        if(usedJointListAllSize <= jointMax )
        {
          newJointList.resize(usedJointListAllSize);			
          memcpy(&newJointList[0],usedJointListAll,sizeof(u32)*usedJointListAllSize);

          checkList[i] = checkList[i+1] = checkList[i+2] = true;
          pushCnt -= 3;

          memcpy(&divVettexList[count],&triangleList[i],sizeof(VertexCacheData)*3);
          count+=3;
        }
        // Too big, restore backup:
        else
        {
          usedJointListAllSize = usedJointListAllSizeBackup;
        }
      }

      divVettexListSize = count;
      MeshData::FaceData		newFace = LocalFunc::GetNewFace( newJointList, divVettexList,divVettexListSize, m_FaceVertexBuffer );
      newFace.m_Material = this->m_Material;

      divFaceDataList.push_back( newFace );
    }

    delete[] divVettexList;

    delete[] usedJointListAll;
    delete[] usedJointListAllBackup;

    return divFaceDataList;
  }


  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  GfMdlDataクラス
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  GfMdlData::GfMdlData() : IResourceData(), rapidxml::xml_document<>()
  {
    m_MeshDataList.reserve(GfMdlData::MAX_MESH_DATA_LIST); // OPTIMIZE
  }

  GfMdlData::GfMdlData( c8* pData ) : IResourceData()
  {
    m_MeshDataList.reserve(GfMdlData::MAX_MESH_DATA_LIST); // OPTIMIZE
    parseData( pData );
  }

  GfMdlData::GfMdlData( std::string filePath )
  {
    m_MeshDataList.reserve(GfMdlData::MAX_MESH_DATA_LIST); // OPTIMIZE

    s32 size;
    c8* pData = ParserUtil::ReadFile( filePath.c_str(), &size );
    std::string fileDir = ParserUtil::GetFilePathStr( filePath.c_str() );

    assert(pData);
    m_FileDir = fileDir;
    parseData( pData, m_FileDir, m_FileDir);

    delete[] pData;
  }

  GfMdlData::GfMdlData( std::wstring wstrFilePath )
  {
    m_MeshDataList.reserve(GfMdlData::MAX_MESH_DATA_LIST); // OPTIMIZE

    std::string filePath = UTF16toUTF8(wstrFilePath);
    s32 size;
    c8* pData = ParserUtil::ReadFile( filePath.c_str(), &size );
    std::string fileDir = ParserUtil::GetFilePathStr( filePath.c_str() );

    assert(pData);
    m_FileDir = fileDir;
    parseData( pData, m_FileDir, m_FileDir);

    delete[] pData;
  }

  GfMdlData::~GfMdlData()
  {
    for ( u32 i = 0; i < m_TextureDataList.size(); ++i )
    {
      m_TextureDataList[i].destroy();
    }

    m_ModelData.destroy();

    for ( u32 i = 0; i < m_MaterialDataList.size(); ++i )
    {
      m_MaterialDataList[i].destroy();
    }

    for ( u32 i = 0; i < m_MeshDataList.size(); ++i )
    {
      m_MeshDataList[i].destroy();
    }

    for ( u32 i = 0; i < m_ShaderDataList.size(); ++i )
    {
      m_ShaderDataList[i].destroy();
    }

    m_JointData.destroy();
  }

  bool GfMdlData::ReadData( c8* pData )
  {
    if (pData == NULL)
    {
      return false;
    }
    return parseData( pData );
  }

  bool GfMdlData::ReadFile( std::string filePath, std::string textureDir, std::string shaderDir )
  {
    s32 size;
    c8* pData = ParserUtil::ReadFile( filePath.c_str(), &size );
    std::string fileDir = ParserUtil::GetFilePathStr( filePath.c_str() );

    if (pData == NULL)
    {
      return false;
    }
    if (textureDir.length() == 0)
    {
      textureDir = fileDir;
    }
    if (shaderDir.length() == 0)
    {
      shaderDir = fileDir;
    }
    bool result = parseData( pData, textureDir, shaderDir );

    delete[] pData;

    return result;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  bool GfMdlData::CreateBinary( b32 fSizeOpt, const std::string outputFilePath )
  {
    // ジョイント
    m_JointData.CreateBinary();

    // モデル
    m_ModelData.CreateBinary( m_JointData.m_pBinaryData, m_JointData.m_BinarySize );

    // テクスチャ
    for( u32 i = 0; i < m_TextureDataList.size(); ++i )
      m_TextureDataList[i].CreateBinary();

    // シェーダ
    for( u32 i = 0; i < m_ShaderDataList.size(); ++i )
      m_ShaderDataList[i].CreateBinary();

    {
      std::vector<std::string> filePathList;

      for( u32 i = 0; i < m_TextureDataList.size(); ++i )
      {
        filePathList.push_back( m_TextureDataList[i].m_FilePath );
      }

      for( u32 i = 0; i < m_MaterialDataList.size(); ++i )
        m_MaterialDataList[i].CreateBinary( filePathList, m_ShaderDataList, m_LogEnable );
    }

    // メッシュ
    for( u32 i = 0; i < m_MeshDataList.size(); ++i )
      m_MeshDataList[i].CreateBinary( fSizeOpt, m_LogEnable );

    if(m_LogEnable)
    {
      m_ConvertLog << this->GetModelConvertLog(outputFilePath, "Win32");
    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  bool GfMdlData::CreateBinaryCTR( const std::string outputFilePath )
  {
    bool err;

    // ジョイント
    m_JointData.CreateBinaryCTR();

    // モデル
    m_ModelData.CreateBinary( m_JointData.m_pBinaryData, m_JointData.m_BinarySize );

    // テクスチャ
    for( u32 i = 0; i < m_TextureDataList.size(); ++i )
      m_TextureDataList[i].CreateBinary();

    // シェーダ
    for( u32 i = 0; i < m_ShaderDataList.size(); ++i )
      m_ShaderDataList[i].CreateBinary();

    {
      std::vector<std::string> filePathList;

      for( u32 i = 0; i < m_TextureDataList.size(); ++i )
      {
        filePathList.push_back( m_TextureDataList[i].m_FilePath );
      }

      // マテリアル
      for( u32 i = 0; i < m_MaterialDataList.size(); ++i )
      {
        ERR_RETURN( m_MaterialDataList[i].CreateBinaryCTR( filePathList, m_ShaderDataList, m_LogEnable ) );
      }

      for( u32 i = 0; i < MaterialData::s_LookUpTableCommandList.size(); ++i )
      {
        std::string name = MaterialData::s_LookUpTableCommandList[i].m_TextureName;

        if ( name.length() )
        {
          char message[256] = {0};
          sprintf_s( message, "Lut Texture = %s\n", name.c_str() );
          //OutputDebugStringA( message );
        }
      }
    }

    // メッシュ
    for( u32 i = 0; i < m_MeshDataList.size(); ++i )
    {
      ERR_RETURN( m_MeshDataList[i].CreateBinaryCTR( true, m_MaterialDataList, m_LogEnable ) );
    }

    if(m_LogEnable)
    {
      m_ConvertLog << this->GetModelConvertLog(outputFilePath, "CTR");
    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  bool GfMdlData::WriteFile( std::string filePath )
  {
    BinaryMaker bm;
    //u64 chunkID;      2014.12.08時点で使用されていないのでコメントアウト

    static const u32 sc_Version = GfModelVersion::PaddingColorIsWhite;

    u32 dataCount = 1;//gfmodel
    u32 materialCount = static_cast<u32>(m_MaterialDataList.size());
    u32 meshCount = static_cast<u32>(m_MeshDataList.size());

    dataCount += (materialCount + meshCount);
    for( u32 i = 0; i < m_ShaderDataList.size(); ++i )
    {
      if ( m_ShaderDataList[i].m_BinarySize )
        dataCount ++;
    }

    // バージョン
    bm.Write(&sc_Version, sizeof(sc_Version));

    // データ数
    bm.Write(&dataCount, sizeof(dataCount));

    {
      u32 padding = 0;
      bm.Write(&padding, sizeof(padding));//ダミー
      bm.Write(&padding, sizeof(padding));
    }

    //シェーダーバイナリ埋め込み
    for( u32 i = 0; i < m_ShaderDataList.size(); ++i )
    {
      if ( m_ShaderDataList[i].m_BinarySize )
      {
        bm.Write(m_ShaderDataList[i].m_pBinaryData, m_ShaderDataList[i].m_BinarySize);
      }
    }

    BinaryMaker materialCommon;
    {//マテリアル共有情報
      u32 topHeader[4] = {0, 0, 0, 0};

      topHeader[0] = MaterialData::s_LookUpTableCommandList.size();

      if ( MaterialData::s_LookUpTableCommandList.size() )
        topHeader[1] = MaterialData::s_LookUpTableCommandList[0].m_CommandSize;//１つあたりのサイズ

      materialCommon.Write( topHeader, sizeof(topHeader) );

      for( u32 i = 0; i < MaterialData::s_LookUpTableCommandList.size(); ++i )
      {
        u32 lutHeader[4] = {0, 0, 0, 0};
        lutHeader[0] = MaterialData::s_LookUpTableCommandList[i].m_TextureNameHash;

        {
          char message[256] = {0};
          sprintf_s( message, "Lut Texture Hash = %08x\n", lutHeader[0] );
          //OutputDebugStringA( message );
        }

        materialCommon.Write( lutHeader, sizeof(lutHeader) );
        materialCommon.Write( MaterialData::s_LookUpTableCommandList[i].m_CommandBuffer, MaterialData::s_LookUpTableCommandList[i].m_CommandSize );
      }
    }

    {// モデル
      u32 paddingSize = 16 - ( m_ModelData.m_BinarySize % 16 );
      u8 padding = 0;

      if ( paddingSize == 16 )
        paddingSize = 0;

      GfBinaryMdlData::ChunkHeader modelHeader(IResourceData::Type("gfmodel").m_ID, m_ModelData.m_BinarySize + paddingSize + materialCommon.GetBinarySize() );
      bm.Write(&modelHeader, sizeof(modelHeader));
      bm.Write(m_ModelData.m_pBinaryData, m_ModelData.m_BinarySize );

      //if ( paddingSize % 16 )
      {
        for( s32 i = 0; i < paddingSize; ++i )
          bm.Write( &padding, sizeof(padding) );
      }

      u8* pCommonData = materialCommon.CreateFixedBuffer();
      bm.Write( pCommonData, materialCommon.GetBinarySize() );
      delete [] pCommonData;

      //printf( "m_ModelData.m_BinarySize = %08x\n", m_ModelData.m_BinarySize + paddingSize + materialCommon.GetBinarySize() );
    }

    u32 materialDataSize = 0;
    // マテリアル
    for (u32 i = 0; i < materialCount; ++i)
    {
      MaterialData& materialData = m_MaterialDataList[i];

      u32 paddingSize = 16 - ( materialData.m_BinarySize % 16 );
      u8 padding = 0;

      GfBinaryMdlData::ChunkHeader modelHeader(IResourceData::Type("material").m_ID, materialData.m_BinarySize + paddingSize);
      bm.Write(&modelHeader, sizeof(modelHeader));
      bm.Write(materialData.m_pBinaryData, materialData.m_BinarySize);

      for( s32 i = 0; i < paddingSize; ++i )
        bm.Write( &padding, sizeof(padding) );

      materialDataSize += materialData.m_BinarySize + paddingSize;
    }

    //printf( "%d MaterialDataSize = %08x\n", materialCount, materialDataSize );

    // メッシュ
    for (u32 i = 0; i < meshCount; ++i)
    {
      MeshData& meshData = m_MeshDataList[i];

      u32 paddingSize = 16 - ( meshData.m_BinarySize % 16 );
      u8 padding = 0;

      GfBinaryMdlData::ChunkHeader modelHeader(IResourceData::Type("mesh").m_ID, meshData.m_BinarySize + paddingSize );
      bm.Write(&modelHeader, sizeof(modelHeader));
      bm.Write(meshData.m_pBinaryData, meshData.m_BinarySize);

      for( s32 i = 0; i < paddingSize; ++i )
        bm.Write( &padding, sizeof(padding) );
    }

    // キャッシュ作成
    u8* buffer = bm.CreateFixedBuffer();
    u32 bufferSize = bm.GetBinarySize();
    ParserUtil::FileCreate( filePath.c_str(), reinterpret_cast<char*>(buffer), bufferSize );
    delete[] buffer;

    if ( m_LogEnable )
    {
      filePath += ".log";
      std::ofstream logFile;
      logFile.open(filePath);
      logFile << m_ConvertLog.str();
      logFile.close();
    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  bool GfMdlData::WriteFileCTR( std::string filePath )
  {
    BinaryMaker bm;
    //u64 chunkID;      2014.12.08時点で使用されていないのでコメントアウト

    u32 materialCount = static_cast<u32>(m_MaterialDataList.size());
    u32 meshCount = static_cast<u32>(m_MeshDataList.size());

    // モデル
    GfBinaryMdlData::ChunkHeader modelHeader(IResourceData::Type("gfmodel").m_ID, m_ModelData.m_BinarySize);
    bm.Write(&modelHeader, sizeof(modelHeader));
    bm.Write(m_ModelData.m_pBinaryData, m_ModelData.m_BinarySize);

    {
      char message[64] = {0};
      sprintf_s( message, "m_ModelData.m_BinarySize = %d\n", m_ModelData.m_BinarySize );
      //OutputDebugStringA( message );
    }

    for (u32 i = 0; i < materialCount; ++i)
    {
      MaterialData& materialData = m_MaterialDataList[i];

      GfBinaryMdlData::ChunkHeader modelHeader(IResourceData::Type("material").m_ID, materialData.m_BinarySize);
      bm.Write(&modelHeader, sizeof(modelHeader));

      bm.Write(materialData.m_pBinaryData, materialData.m_BinarySize);
    }

    for (u32 i = 0; i < meshCount; ++i)
    {
      MeshData& meshData = m_MeshDataList[i];

      GfBinaryMdlData::ChunkHeader modelHeader(IResourceData::Type("mesh").m_ID, meshData.m_BinarySize);
      bm.Write(&modelHeader, sizeof(modelHeader));

      bm.Write(meshData.m_pBinaryData, meshData.m_BinarySize);
    }

    {
      char message[64] = {0};
      sprintf_s( message, "%d + %d\n", materialCount, meshCount );
      //OutputDebugStringA( message );
    }

    // キャッシュ作成
    u8* buffer = bm.CreateFixedBuffer();
    u32 bufferSize = bm.GetBinarySize();
    ParserUtil::FileCreate( filePath.c_str(), reinterpret_cast<char*>(buffer), bufferSize );
    delete[] buffer;

    if ( m_LogEnable )
    {
      filePath += ".log";
      std::ofstream logFile;
      logFile.open(filePath);
      logFile << m_ConvertLog.str();
      logFile.close();
    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  void GfMdlData::MaterialMerge()
  {
    std::vector<MaterialData> optimizeList;

    for (u32 i = 0; i < m_MaterialDataList.size(); ++i)
    {
      MaterialData& materialData = m_MaterialDataList[i];

      b32 fHit = false;

      if ( materialData.m_Name.find( "NotOpt" ) != std::string::npos )
      {
        for (u32 i2 = 0; i2 < optimizeList.size(); ++i2)
        {
          MaterialData& optMaterialData = optimizeList[i2];

          if ( materialData == optMaterialData )
          {
            fHit = true;

            for( int meshNo = 0; meshNo < m_MeshDataList.size(); ++meshNo )
            {
              MeshData &rMeshData = m_MeshDataList[meshNo];

              for( int faceNo = 0; faceNo < rMeshData.m_FaceDataList.size(); ++faceNo )
              {
                MeshData::FaceData &rFaceData = rMeshData.m_FaceDataList[faceNo];
                if ( rFaceData.m_Material == materialData.m_Name )
                {//切り捨てられるので置き換え
                  rFaceData.m_Material = optMaterialData.m_Name;
                }
              }
            }
          }
        }
      }

      if ( fHit == false )
      {
        optimizeList.push_back( materialData );
      }

    }

    m_MaterialDataList.clear();
    m_MaterialDataList = optimizeList;

    {
      m_ModelData.m_MaterialList.clear();
      for( u32 i = 0; i < m_MaterialDataList.size(); ++i )
      {
        m_ModelData.m_MaterialList.push_back( m_MaterialDataList[i].m_Name );
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  void GfMdlData::VertexSizeOptimize()
  {
    std::vector<MeshData::VertexCacheData> vertexCacheDataList;

    for( u32 meshNo = 0; meshNo < m_MeshDataList.size(); ++meshNo )
    {
      MeshData &rMeshData = m_MeshDataList[meshNo];

      for( u32 faceNo = 0; faceNo < rMeshData.m_FaceDataList.size(); ++faceNo )
      {
        MeshData::FaceData &rFaceData = rMeshData.m_FaceDataList[faceNo];
        const MaterialData *pMaterialData = NULL;

        for( u32 materialNo = 0; materialNo < m_MaterialDataList.size(); ++materialNo )
        {
          if ( m_MaterialDataList[materialNo].m_Name == rFaceData.m_Material )
          {
            pMaterialData = &m_MaterialDataList[materialNo];
          }
        }

        rFaceData.VertexSizeOptimize( pMaterialData );
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  void GfMdlData::VertexMerge()
  {
    std::vector<MeshData::VertexCacheData> vertexCacheDataList;
    u32 allVertexSize = 0;

    for( u32 meshNo = 0; meshNo < m_MeshDataList.size(); ++meshNo )
    {
      MeshData &rMeshData = m_MeshDataList[meshNo];

      for( u32 faceNo = 0; faceNo < rMeshData.m_FaceDataList.size(); ++faceNo )
      {
        MeshData::FaceData &rFaceData = rMeshData.m_FaceDataList[faceNo];
        const MaterialData *pMaterialData = NULL;

        for( u32 materialNo = 0; materialNo < m_MaterialDataList.size(); ++materialNo )
        {
          if ( m_MaterialDataList[materialNo].m_Name == rFaceData.m_Material )
          {
            pMaterialData = &m_MaterialDataList[materialNo];
          }
        }

        rFaceData.VertexMerge( pMaterialData );

        allVertexSize += rFaceData.m_FaceVertexBuffer.m_Position.size();
      }
    }

    {
      char message[64] = {0};
      sprintf_s( message, "allVertexSize = %d\n", allVertexSize );
      OutputDebugStringA( message );
    }
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  void GfMdlData::MeshMerge()
  {
    std::vector<MeshData> optimizeMeshList;

    for( u32 meshNo = 0; meshNo < m_MeshDataList.size(); ++meshNo )
    {
      MeshData &rMeshData = m_MeshDataList[meshNo];

      if ( rMeshData.m_IsBlendShape )
        continue;

      if ( rMeshData.m_IsVisibilityAnimation || rMeshData.m_IsMultiUv )
      {
        if ( m_LogEnable )
        {
          std::cout << rMeshData.m_Name;
          std::cout << "は、";

          if ( rMeshData.m_IsVisibilityAnimation )
            std::cout << "ビジビリティアニメが有効、";

          if ( rMeshData.m_IsMultiUv )
            std::cout << "マルチＵＶを使用している";

          std::cout << "為、メッシュ結合対象外です。\n";
        }

        optimizeMeshList.push_back( rMeshData );//最適化できないものを先に積んでおく
      }
    }

    if ( m_LogEnable )
      std::cout << "\n";

    for (u32 materialNo = 0; materialNo < m_MaterialDataList.size(); ++materialNo)
    {
      MaterialData &rMaterialData = m_MaterialDataList[materialNo];
      std::vector<MeshData::FaceData> optimizeFaceList;

      for( u32 meshNo = 0; meshNo < m_MeshDataList.size(); ++meshNo )
      {
        MeshData &rMeshData = m_MeshDataList[meshNo];

        if ( rMeshData.m_IsBlendShape )
          continue;

        if ( rMeshData.m_IsVisibilityAnimation || rMeshData.m_IsMultiUv )
          continue;

        for( u32 faceNo = 0; faceNo < rMeshData.m_FaceDataList.size(); ++faceNo )
        {
          MeshData::FaceData &rFaceData = rMeshData.m_FaceDataList[faceNo];

          if( rFaceData.m_Material != rMaterialData.m_Name )
            continue;

          optimizeFaceList.push_back( rFaceData );
        }
      }

      if ( optimizeFaceList.size() )
      {
        MeshData newMeshData;
        MeshData::FaceData	optFace = optimizeFaceList[0];
        char      logMessage[256];
        if ( m_LogEnable )
        {
          std::cout << rMaterialData.m_Name;
          std::cout << "マテリアルを使用しているメッシュを結合します。\n";
          memset( logMessage, 0, sizeof(logMessage) );
          sprintf_s( logMessage, "\t%sメッシュ%d番目ファイスを基準に結合します。\n", optFace.m_MeshName.c_str(), optFace.m_FaceIndex );
          std::cout << logMessage;
        }

        newMeshData.m_Path = newMeshData.m_Name = rMaterialData.m_Name + "_OptMesh";

        for( u32 faceNo = 1; faceNo < optimizeFaceList.size(); ++faceNo )
        {
          if ( m_LogEnable )
          {
            memset( logMessage, 0, sizeof(logMessage) );
            sprintf_s( logMessage, "\t\t%sメッシュ%d番目ファイスは、", optimizeFaceList[faceNo].m_MeshName.c_str(), optimizeFaceList[faceNo].m_FaceIndex );
            std::cout << logMessage;
          }

          if ( optFace.Merge( optimizeFaceList[faceNo] ) == false )
          {
            if ( m_LogEnable )
            {
              if ( optimizeFaceList[faceNo].m_FaceVertexBuffer.m_JointNo.size() )
                std::cout << "スキンメッシュのため";
              else
                std::cout << "スキンメッシュでないため";

              std::cout << "結合できませんでした。\n";
            }
            
            //マージできないのでm_MeshDataListは更新しない。
            return;
          }

          if ( m_LogEnable )
            std::cout << "結合できました。\n";
        }

        newMeshData.m_FaceDataList.push_back( optFace );
        newMeshData.CalcAABB();

        optimizeMeshList.push_back( newMeshData );
      }
    }

    m_MeshDataList = optimizeMeshList;

    {
      m_ModelData.m_MeshList.clear();
      m_ModelData.m_MeshPathList.clear();
      for( u32 i = 0; i < m_MeshDataList.size(); ++i )
      {
        m_ModelData.m_MeshList.push_back( m_MeshDataList[i].m_Name );
        m_ModelData.m_MeshPathList.push_back( m_MeshDataList[i].m_Path );
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  void GfMdlData::MeshDivJointSize( u32 jointStartRegNo, u32 jointMax )
  {
    for( u32 meshNo = 0; meshNo < m_MeshDataList.size(); ++meshNo )
    {
      m_MeshDataList[meshNo].DivJointSize( jointStartRegNo, jointMax );
    }
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  void GfMdlData::MeshOptimize( u32 vertexCacheSize )
  {
    for( u32 meshNo = 0; meshNo < m_MeshDataList.size(); ++meshNo )
    {
      MeshData &rMeshData = m_MeshDataList[meshNo];

      for( u32 faceNo = 0; faceNo < rMeshData.m_FaceDataList.size(); ++faceNo )
      {
        MeshData::FaceData &rFaceData = rMeshData.m_FaceDataList[faceNo];

        rFaceData.Optimize(vertexCacheSize);
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  void GfMdlData::MeshOptimizeCTR()
  {
    for( u32 i = 0; i < m_MeshDataList.size(); ++i )
    {
      m_MeshDataList[i].Optimize( m_MaterialDataList );
    }
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  void GfMdlData::DataCheck( u32 jointStartRegNo, u32 jointMax )
  {
    for( u32 meshNo = 0; meshNo < m_MeshDataList.size(); ++meshNo )
    {
      m_MeshDataList[meshNo].DataCheck( jointStartRegNo, jointMax );
    }
  }

  std::string GfMdlData::GetModelConvertLog( const std::string outputFilePath,  const std::string platform ) const
  {
    std::stringstream ss;

    ss << "<?xml version='1.0' encoding='utf-8' ?>" << std::endl;
    ss << "<GfbMdlDescriptor>" << std::endl;
    ss << "\t<Name>" << outputFilePath << "</Name>" << std::endl;
    ss << "\t<Platform>" << platform << "</Platform>" << std::endl;
    ss << "\t<MaterialCount>" << m_MaterialDataList.size() << "</MaterialCount>" << std::endl;
    ss << "\t<MeshCount>" << m_MeshDataList.size() << "</MeshCount>" << std::endl;

    {
      u32 TotalFaceCount = 0;
      for ( auto iter = m_MeshDataList.begin(); iter < m_MeshDataList.end(); ++iter )
      {
        TotalFaceCount += iter->m_FaceDataList.size();
      }
      ss << "\t<TotalFaceCount>" << TotalFaceCount << "</TotalFaceCount>" << std::endl;
    }

    {
      u32 totalVertexCount = 0;
      for( auto iter = m_MeshDataList.begin(); iter < m_MeshDataList.end(); ++iter )
      {
        totalVertexCount += iter->GetVertexCount();
      }
      ss << "\t<TotalVertexCount>" << totalVertexCount << "</TotalVertexCount>" << std::endl;
    }

    {
      u32 totalPolygonCount = 0;
      for( auto iter = m_MeshDataList.begin(); iter < m_MeshDataList.end(); ++iter )
      {
        totalPolygonCount += iter->GetPolygonCount();
      }
      ss << "\t<TotalPolygonCount>" << totalPolygonCount << "</TotalPolygonCount>" << std::endl;
    }

    {
      ss << "\t<MaterialBinaries>" << std::endl;

      ss << this->GetMaterialConvertLog(2);

      ss << "\t</MaterialBinaries>" << std::endl;
    }

    {
      ss << "\t<MesheBinaries>" << std::endl;

      ss << this->GetMeshConvertLog(2);

      ss << "\t</MesheBinaries>" << std::endl;
    }

    ss << "</GfbMdlDescriptor>" << std::endl;

    return ss.str();
  }

  std::string GfMdlData::GetMaterialConvertLog( const int level ) const
  {
    std::stringstream ss;

    for( auto iter = m_MaterialDataList.begin(); iter < m_MaterialDataList.end(); ++iter )
    {
      ss << std::string(level, '\t');
      ss << "<Material";
      ss << " Name=\"" << iter->m_Name << "\"";
      ss << " BinarySize=\"" << iter->m_BinarySize << "\"";
      ss << "/>" << std::endl;
    }

    return ss.str();
  }

  std::string GfMdlData::GetMeshConvertLog( const int level ) const
  {
    std::stringstream ss;

    for( auto iter = m_MeshDataList.begin(); iter < m_MeshDataList.end(); ++iter )
    {
      ss << std::string(level, '\t');
      ss << "<Mesh";
      ss << " Name=\"" << iter->m_Name << "\"";
      ss << " BinarySize=\"" << iter->m_BinarySize << "\"";
      ss << " FaceNum=\"" << iter->m_FaceDataList.size() << "\"";
      ss << " PolygonCount=\"" << iter->GetPolygonCount() << "\"";
      ss << " VertexCount=\"" << iter->GetVertexCount() << "\"";
      ss << ">" << std::endl;

      ss << iter->GetFaceConvertLog( level + 1 );

      ss << "\t\t</Mesh>" << std::endl;
    }

    return ss.str();
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  std::string GfMdlData::UTF16toUTF8( const std::wstring& str )
  {
    int len = ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL );

    if ( len > 0 ) { 
      std::vector<char> utf8( len );

      if ( ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, &utf8[0], len, NULL, NULL ) ){
        return std::string( &utf8[0] );
      }
    }

    return NULL;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  bool GfMdlData::parseData( c8* pData, std::string textureDir, std::string shaderDir )
  {
    s32 size = 0;

    {
      //this->parse< parse_no_utf8 | parse_no_entity_translation >( pData );
      this->parse< parse_no_utf8 >( pData );

      const xml_node<>* top_node = first_node("GfModel");
      c8* strVersionId = top_node->first_attribute("version")->value();
      char* strtokTmp = NULL;
      m_VersionId.m_Version.m_Major = atoi(strtok_s(strVersionId, ".", &strtokTmp));
      m_VersionId.m_Version.m_Minor = atoi(strtok_s(NULL, ".", &strtokTmp));
      if (m_VersionId.m_Version.m_Minor / 10 == 0)
      {// マイナーバージョンは必ず2桁で統一
        m_VersionId.m_Version.m_Minor *= 10;
      }

      if (top_node == NULL)
      {
        return false;
      }

      {
        m_fUvSetAdjust = false;

        const xml_node<> *pExportOptionNode = top_node->first_node( "ExportOption");

        if ( pExportOptionNode )
        {
          std::string uvSetAdjust = pExportOptionNode->first_attribute("UvSetAdjust")->value();
          if ( uvSetAdjust == "Enable")
            m_fUvSetAdjust = true;
        }
      }

      {
        const xml_node<> *jointsNode = top_node->first_node( "Joints");//旧フォーマット対応
        const xml_node<> *skeltonNode = top_node->first_node( "Skelton");

        if ( jointsNode )
        {//旧フォーマット対応
          m_JointData.ReadJointDataNode( jointsNode );
        }
        else if ( skeltonNode )
        {
          m_JointData.ReadSkeltonDataNode( skeltonNode );
        }

        m_ModelData.ReadModelDataNode( top_node );
      }

      for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
      {
        std::string nodeName = node->name();
        if ( nodeName == "TextureList" )
        {
          for( const xml_node<>* texture_node = node->first_node("Texture"); texture_node; texture_node = texture_node->next_sibling("Texture") )
          {
            TextureData textureData;

            textureData.ReadTextureDataNode( texture_node, textureDir );
            this->m_TextureDataList.push_back( textureData );
          }
        }
      }

      for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
      {
        std::string nodeName = node->name();
        if ( nodeName == "ShaderList" )
        {
          for( const xml_node<>* shader_node = node->first_node("Shader"); shader_node; shader_node = shader_node->next_sibling("Shader") )
          {
            ShaderData shaderData;
            shaderData.ReadShaderDataNode( shader_node, shaderDir );
            this->m_ShaderDataList.push_back( shaderData );
          }
        }
      }

      {
        for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
        {
          std::string nodeName = node->name();
          if ( nodeName == "Materials" )
          {
            for( const xml_node<>* material_node = node->first_node("Material"); material_node; material_node = material_node->next_sibling("Material") )
            {
              MaterialData materialData;

              materialData.m_fUvSetAdjust = m_fUvSetAdjust;

              materialData.ReadMaterialDataNode( material_node, m_VersionId );
              this->m_MaterialDataList.push_back( materialData );
            }
          }
        }
      }

      for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
      {
        std::string nodeName = node->name();
        if ( nodeName == "Meshes" )
        {
          for( const xml_node<>* mesh_node = node->first_node("Mesh"); mesh_node; mesh_node = mesh_node->next_sibling("Mesh") )
          {
            MeshData meshData;

            meshData.m_fUvSetAdjust = m_fUvSetAdjust;

            ERR_RETURN( meshData.ReadMeshDataNode( mesh_node, m_JointData, m_MaterialDataList ) );
            this->m_MeshDataList.push_back( meshData );
          }
        }
      }

    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  s32 GfMdlData::GetSize( void )
  {
    s32 size = 0;
    size += 1;//m_ModelData
    size += static_cast<u32>( m_MaterialDataList.size() );
    size += static_cast<u32>( m_MeshDataList.size() );
    size += static_cast<u32>( m_TextureDataList.size() );
    size += static_cast<u32>( m_ShaderDataList.size() );
    return size;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  const void* GfMdlData::GetData( u32 index )
  {
    if ( index == 0 )
      return m_ModelData.m_pBinaryData;
    --index;

    if ( index < m_MaterialDataList.size() )
    {
      return m_MaterialDataList[index].m_pBinaryData;
    }
    index -= static_cast<u32>(m_MaterialDataList.size());

    if ( index < m_MeshDataList.size() )
    {
      return m_MeshDataList[index].m_pBinaryData;
    }
    index -= static_cast<u32>(m_MeshDataList.size());

    if ( index < m_TextureDataList.size() )
    {
      return m_TextureDataList[index].m_pBinaryData;
    }
    index -= static_cast<u32>(m_TextureDataList.size());

    if ( index < m_ShaderDataList.size() )
    {
      return m_ShaderDataList[index].m_pBinaryData;
    }
    //index -= static_cast<u32>(m_ShaderDataList.size());

    return NULL;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  u32 GfMdlData::GetDataSize( u32 index )
  {
    if ( index == 0 )
      return m_ModelData.m_BinarySize;
    --index;

    if ( index < m_MaterialDataList.size() )
    {
      return m_MaterialDataList[index].m_BinarySize;
    }
    index -= static_cast<u32>( m_MaterialDataList.size() );

    if ( index < m_MeshDataList.size() )
    {
      return m_MeshDataList[index].m_BinarySize;
    }
    index -= static_cast<u32>( m_MeshDataList.size() );

    if ( index < m_TextureDataList.size() )
    {
      return m_TextureDataList[index].m_BinarySize;
    }
    index -= static_cast<u32>( m_TextureDataList.size() );

    if ( index < m_ShaderDataList.size() )
    {
      return m_ShaderDataList[index].m_BinarySize;
    }
    //index -= static_cast<u32>( m_ShaderDataList.size() );

    return 0;
  }

  //----------------------------------------------------------------------------
  /**
   *   @brief 
   */
  //-----------------------------------------------------------------------------
  IResourceData::Type GfMdlData::GetType( u32 index )
  {
    if ( index == 0 )
      return IResourceData::Type("gfmodel");
    --index;

    if ( index < m_MaterialDataList.size() )
    {
      return IResourceData::Type("material");
    }
    index -= static_cast<u32>( m_MaterialDataList.size() );

    if ( index < m_MeshDataList.size() )
    {
      return IResourceData::Type("mesh");
    }
    index -= static_cast<u32>( m_MeshDataList.size() );

    if ( index < m_TextureDataList.size() )
    {
      return IResourceData::Type("texture");
    }
    index -= static_cast<u32>( m_TextureDataList.size() );

    if ( index < m_ShaderDataList.size() )
    {
      return IResourceData::Type("shader");
    }
    index -= static_cast<u32>( m_ShaderDataList.size() );

    return IResourceData::Type("none");
  }

}

#endif
