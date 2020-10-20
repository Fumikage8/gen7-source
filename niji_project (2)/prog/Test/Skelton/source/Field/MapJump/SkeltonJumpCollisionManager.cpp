#if defined(GF_PLATFORM_WIN32)
//*************************************************************************************************
/**
 @file jump_collision_managera.cpp
 @brief 擬似マップジャンプの衝突管理
 @date 2015.01.22
 @author saita_kazuki
 */
//*************************************************************************************************


#include "Test/Skelton/include/Field/MapJump/SkeltonJumpCollisionManager.h"
#include "Test/Skelton/include/Field/MapJump/SkeltonJumpDataManager.h"

#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_math_control.h>
#include <Collision/include/gfl2_CollisionModel.h>

#include "Test/Skelton/include/Field/MapJump/SkeltonXmlTool.h"
#include "Test/Skelton/include/Field/MapJump/SkeltonFileReader.h"


#define NOT_ENABLE_COLLISION 1


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)


//=================================================================================================
// 定数
//=================================================================================================
const u32         CollisionDataManager::COLLISION_DATA_MAX = 128;
//const std::string CollisionDataManager::NAME_PREFIX = std::string( "AABB_");

// コンストラクタ
CollisionDataManager::CollisionDataManager() :
  m_pCollisionDataTable(),
  m_useDataNum( 0)
{
}

// デストラクタ
CollisionDataManager::~CollisionDataManager()
{
}

// 初期化
void CollisionDataManager::Initialize()
{
  m_pCollisionDataTable = new COLLISION_DATA[ COLLISION_DATA_MAX ];
  m_useDataNum = 0;
}

// 破棄
void CollisionDataManager::Terminate()
{
  GFL_SAFE_DELETE_ARRAY( m_pCollisionDataTable)
}

// データ読み込み
b32 CollisionDataManager::LoadData( const c8* pFilePath)
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
b32 CollisionDataManager::LoadData( const c16* pFilePath)
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

// 衝突したデータ名を取得
const c8* CollisionDataManager::GetHitName( gfl2::math::Vector4& pos, gfl2::collision::CollisionModel* pCollisionModel) const
{
  // @todo ひとまずコリジョンモデル優先

#if NOT_ENABLE_COLLISION

#else
  const c8* pHitName_CollisonModel = this->GetHitName_CollisionModel( pos, pCollisionModel);
  if( pHitName_CollisonModel)
  {
    return pHitName_CollisonModel;
  }
#endif

  const c8* pHitName_CollisonData = this->GetHitName_CollisionData( pos);
  if( pHitName_CollisonData)
  {
    return pHitName_CollisonData;
  }

  return NULL;
}


// データ取得
const COLLISION_DATA* CollisionDataManager::GetData()
{
  return m_pCollisionDataTable;
}

// データ数取得
u32 CollisionDataManager::GetDataNum()
{
  return m_useDataNum;
}


// 衝突したデータ名をコリジョンモデルから取得
const c8* CollisionDataManager::GetHitName_CollisionModel( gfl2::math::Vector4& pos, gfl2::collision::CollisionModel* pCollisionModel) const
{
  static   const u32 MAX_HIT_COUNT = 10;  // 衝突情報を格納する最大数

  // コリジョンライブラリ、コールバックインターフェイス
  class MapJumpCallbackable : public gfl2::collision::ICollisionModelCallbackable
  {
  public:

    // 衝突情報
    struct HITDATA
    {
      f32   height;
      const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
    };

    u32     m_HitCount;                         // 当たった数
    HITDATA m_HitDataTable[ MAX_HIT_COUNT ];    // 衝突情報バッファ

    // コンストラクタ
    MapJumpCallbackable() :
      m_HitCount( 0 )
    {
      for( u32 i = 0; i < MAX_HIT_COUNT; i++)
      {
        m_HitDataTable[i].height = 0.0f;
        m_HitDataTable[i].pTriangle = NULL;
      }
    }

    // デストラクタ
    virtual ~MapJumpCallbackable()
    {
    }

    // 当たり判定コールバック関数
    virtual bool CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height)
    {
      // 高さを記憶
      m_HitDataTable[m_HitCount].height = height;
      m_HitDataTable[m_HitCount].pTriangle = pTriangle;
      m_HitCount++;

      if( m_HitCount >= MAX_HIT_COUNT)
      {
        // 衝突検出終了
        return false;
      }
      else
      {
        // 衝突検出続行
        return true;
      }
    }

    // 当たったかどうか
    bool IsHit()
    {
      return ( m_HitCount != 0 );
    }

    // 指定の高さに一番近いコリジョンメッシュの名前を取得する
    const c8* GetNearHitName( f32 height)
    {
      // 衝突していなかったらNULLを返す
      if( m_HitCount == 0)
      {
        return NULL;
      }

#if NOT_ENABLE_COLLISION
      return NULL;
#else
      // 一番近い衝突メッシュを探す
      const c8* pResutName( m_HitDataTable[0].pTriangle->m_MeshName);
      f32 resultDiff = gfl2::math::Abs( m_HitDataTable[0].height - height);
      for( u32 i = 1; i < m_HitCount; i++)
      {
        f32 diff( gfl2::math::Abs( m_HitDataTable[i].height - height ));
        if( diff < resultDiff)
        {
          pResutName = m_HitDataTable[i].pTriangle->m_MeshName;
          resultDiff = diff;
        }
      }

      return pResutName;
#endif
    }

    // 再取得可能な状態にする
    void Clear( void)
    {
      m_HitCount = 0;
    }
  };

  // コリジョンメッシュとの当たり判定
  MapJumpCallbackable mapJumpCallBack;
  mapJumpCallBack.Clear();
  pCollisionModel->GetHeight(
    pos,
    NULL,
    &mapJumpCallBack
    );

  // 一番近い衝突メッシュ名を取得
  const c8* pHitName = mapJumpCallBack.GetNearHitName( pos.y);
  if( pHitName)
  {
    return pHitName;
  }
  return NULL;
}


// 衝突したデータ名をコリジョンデータから取得
const c8* CollisionDataManager::GetHitName_CollisionData( gfl2::math::Vector4& pos) const
{
  c8* retName = NULL;
  for( s32 i = 0; i < m_useDataNum; ++i)
  {
    if( m_pCollisionDataTable[i].aabb.IsContain( pos))
    {
      retName = m_pCollisionDataTable[i].name;
    }
  }

  if( retName)
  {
    return retName;
  }
  return NULL;
}

// コリジョンデータ更新
b32 CollisionDataManager::UpdateImpl( c8* pData )
{
  if( m_pCollisionDataTable == NULL )
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

  const rapidxml::xml_node<>* pCollisionDataListNode = doc.first_node("CollisionDataList");

  for( const rapidxml::xml_node<>* pCollisionDataNode = pCollisionDataListNode->first_node(); pCollisionDataNode; pCollisionDataNode = pCollisionDataNode->next_sibling() )
  {
    if( m_useDataNum >= COLLISION_DATA_MAX )
    {
      break;
    }

    m_pCollisionDataTable[ m_useDataNum ].id = m_useDataNum;
    if( this->LoadCollisionData( pCollisionDataNode, &m_pCollisionDataTable[ m_useDataNum ]) )
    {
      m_useDataNum++;
    }
  }

  return true;
}

// データ読み込み
b32 CollisionDataManager::LoadCollisionData( const rapidxml::xml_node<>* pCollisionDataNode, COLLISION_DATA* pCollisionData)
{
  // ヘッダー部分
  std::string meshName = pCollisionDataNode->first_attribute("Name")->value();
  strcpy_s( pCollisionData->name, 512, meshName.c_str() );

  // AABB
  gfl2::math::Vector4 min, max;
  GetVectorValue_Node( pCollisionDataNode->first_node("Min"), &min);
  GetVectorValue_Node( pCollisionDataNode->first_node("Max"), &max);
  pCollisionData->aabb.SetMinMax( min, max);

#if 0
  // プライオリティ設定
  const rapidxml::xml_node<>* pPriorityNode = pCollisionDataNode->first_node("Priority");
  pCollisionData->priority = 0;
  if( pPriorityNode )
  {
    pCollisionData->priority = atoi( pPriorityNode->value() );
  }
#endif

  return true;
}

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32
