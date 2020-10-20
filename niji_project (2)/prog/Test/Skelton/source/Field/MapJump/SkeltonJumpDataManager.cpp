#if defined(GF_PLATFORM_WIN32)
//*************************************************************************************************
/**
 @file jump_data_managera.cpp
 @brief 擬似マップジャンプのデータ管理
 @date 2015.01.22
 @author saita_kazuki
 */
//*************************************************************************************************


#include "Test/Skelton/include/Field/MapJump/SkeltonJumpDataManager.h"

#include "Test/Skelton/include/Field/MapJump/SkeltonXmlTool.h"
#include "Test/Skelton/include/Field/MapJump/SkeltonFileReader.h"

// ファイル操作系
#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <Windows.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)


//=================================================================================================
// 定数
//=================================================================================================
const u32   JumpDataManager::JUMP_DATA_MAX   = 128;
const f32   JumpDataManager::JUMP_DATA_ANGLE_NONE = FLT_MAX;

// コンストラクタ
JumpDataManager::JumpDataManager() :
  m_pJumpDataTable(),
  m_useDataNum( 0),
  m_isLock( false)
{
}

// デストラクタ
JumpDataManager::~JumpDataManager()
{
}

// 初期化
void JumpDataManager::Initialize()
{
  m_pJumpDataTable = new JUMP_DATA[ JUMP_DATA_MAX ];
  m_useDataNum = 0;
  m_isLock = false;
}

// 破棄
void JumpDataManager::Terminate()
{
  if( m_pJumpDataTable )
  {
    delete[] m_pJumpDataTable;
    m_pJumpDataTable = NULL;
  }
}

// データ読み込み
b32 JumpDataManager::LoadData( const c8* pFilePath)
{
  s32 size;
  FileReader reader;
  c8* pFileData = reader.ReadFile( pFilePath, &size);

  if( pFileData )
  {
    b32 result = UpdateImpl( pFileData );
    delete pFileData;
    return result;
  }
  else
  {
    // ファイルのロードに失敗した
    return false;
  }
}

// データ読み込み
b32 JumpDataManager::LoadData( const c16* pFilePath)
{
  s32 size;
  FileReader reader;
  c8* pFileData = reader.ReadFile( pFilePath, &size);

  if( pFileData)
  {
    b32 result = UpdateImpl( pFileData);
    delete pFileData;
    return result;
  }
  else
  {
    // ファイルのロードに失敗した
    return false;
  }
}

void JumpDataManager::CreateXml( const c16* pFilePath )
{
  std::ofstream ofs( pFilePath );
  ofs << "<?xml version=\"1.0\" encoding=\"Shift-JIS\"?>" << std::endl;
  ofs << "<ROOT>" << std::endl;

  for( u32 i = 0; i < m_useDataNum; i++ )
  {
    const JUMP_DATA* pData = &m_pJumpDataTable[i];
    ofs << "  <" << pData->name << "/>" << std::endl;
  }

  ofs << "</ROOT>" << std::endl;
}


// データ取得
const JUMP_DATA* JumpDataManager::GetData( const c8* pName, bool isForce) const
{
  // データ取得不可ならNULLを返す
  if( m_isLock && !isForce)
  {
    return NULL;
  }

  std::string name( pName );

  // gfmdlのメッシュ名の変更により修正
  // gfmdl version 2.1以前 <Mesh Name="|camera|camera_01|camera_01Shape" ～
  // ↓
  // gfmdl version 2.1以降 <Mesh Name="|camera|camera_01" ～
#if 0
  // "|camera_02Shape"を消す
  std::string::size_type t = name.find_last_of( "|" );
  if( t != std::string::npos )
  {
    name.erase( t );

    // "|areacamera|"を消す
    t = name.find_last_of( "|" );
    if( t != std::string::npos )
    {
      name.erase( 0, t + 1 );
    }  
  }
#else
  // gfmdl version 2.1以前のモデル用の処理
  std::string::size_type t = name.rfind( "Shape" );
  if( t != std::string::npos)
  {
    t = name.find_last_of( "|" );
    if( t != std::string::npos )
    {
      name.erase( t );
    }
  }

  t = name.find_last_of( "|" );
  if( t != std::string::npos )
  {
    name.erase( 0, t + 1 );
  }
#endif

  for( u32 i = 0; i < m_useDataNum; i++ )
  {
    // 格納データの名前と一致すればデータを返す
    const JUMP_DATA* pData = &m_pJumpDataTable[i];
    if( strcmp( name.c_str(), pData->name ) == 0 )
    {
      return pData;
    }
  }

  return NULL;
}

const JUMP_DATA* JumpDataManager::GetDataByIndex( s32 index, bool isForce ) const
{
    // データ取得不可ならNULLを返す
  if( m_isLock && !isForce)
  {
    return NULL;
  }

  if( index < 0 || index >= m_useDataNum )
  {
    return NULL;
  }

  return &m_pJumpDataTable[index];
}

// データ取得不可
void JumpDataManager::Lock()
{
  m_isLock = true;
}

// データ取得可
void JumpDataManager::Unlock()
{
  m_isLock = false;
}

// データ更新
b32 JumpDataManager::UpdateImpl( c8* pData )
{
  if( m_pJumpDataTable == NULL )
  {
    return false;
  }

  rapidxml::xml_document<> doc;

  try
  {
    doc.parse<0>( pData );
  }
  catch( rapidxml::parse_error &e )
  {
    MessageBoxA( NULL, "XMLの構文が間違っています", "エラー", MB_ICONERROR );
    return false;
  }

  const rapidxml::xml_node<>* pJumpDataListNode = doc.first_node("JumpDataList");

  for( const rapidxml::xml_node<>* pJumpDataNode = pJumpDataListNode->first_node(); pJumpDataNode; pJumpDataNode = pJumpDataNode->next_sibling() )
  {
    if( m_useDataNum >= JUMP_DATA_MAX )
    {
      break;
    }

    m_pJumpDataTable[ m_useDataNum ].id = m_useDataNum;
    if( this->LoadJumpData( pJumpDataNode, &m_pJumpDataTable[ m_useDataNum ]) )
    {
      m_useDataNum++;
    }
  }

  return true;
}

// データ読み込み
b32 JumpDataManager::LoadJumpData( const rapidxml::xml_node<>* pJumpDataNode, JUMP_DATA* pJumpData)
{
  // ヘッダー部分
  std::string meshName = pJumpDataNode->first_attribute("MeshName")->value();
  strcpy_s( pJumpData->name, 512, meshName.c_str() );

  // ジャンプ位置
  GetVectorValue_Node( pJumpDataNode->first_node("position"), &pJumpData->pos);

  // プライオリティ
  pJumpData->priority = 0;
  const rapidxml::xml_node<>* pPriorityFrameNode = pJumpDataNode->first_node("Priority");
  if( pPriorityFrameNode )
  {
    pJumpData->priority = atoi( pPriorityFrameNode->value() );
  }

  // フェードフレーム設定
  pJumpData->fadeFrame = 0;
  const rapidxml::xml_node<>* pFadeFrameNode = pJumpDataNode->first_node("FadeFrame");
  if( pFadeFrameNode )
  {
    pJumpData->fadeFrame = atoi( pFadeFrameNode->value() );
  }

  // ジャンプ後向き設定
  pJumpData->angle = JUMP_DATA_ANGLE_NONE;
  const rapidxml::xml_node<>* pAngleNode = pJumpDataNode->first_node("angle");
  if( pAngleNode )
  {
    c8* type = pAngleNode->first_attribute("type")->value();
    if( type)
    {
      if( strcmp( type, "degree") == 0)
      {
        pJumpData->angle = gfl2::math::ConvDegToRad( atoi( pAngleNode->value()));
      }
      else if( strcmp( type, "radian") == 0)
      {
        pJumpData->angle = static_cast<f32>( atof( pAngleNode->value()));
      }
    }
    else
    {
      pJumpData->angle = gfl2::math::ConvDegToRad( atoi( pAngleNode->value()));
    }
  }

  return true;
}

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32
