//======================================================================
/**
* @file FieldCameraAreaCollisionManager.cpp
* @date 2015/07/01 18:29:27
* @author ikuta_junya
* @brief フィールドカメラエリアのコリジョンデータを管理するクラス
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

// gfl2
#include <debug/include/gfl2_Assert.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>

// field
#include "ViewerProc/include/FieldRo/Camera/Area/FieldCameraAreaDataManager.h"
#include "ViewerProc/include/FieldRo/Camera/Area/FieldCameraAreaCollisionManager.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

static const f32 Y_HIT_VOLUME_BOTTOM( -150.0f ); // Y軸コリジョン判定範囲、下方向。@todo 作業者と相談し調整する予定
static const f32 Y_HIT_VOLUME_TOP( 150.0f ); // Y軸コリジョン判定範囲、上方向。@todo 作業者と相談し調整する予定


// 基本形状コリジョンデータへのアクセッサー
class PrimitiveCollisionDataAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( PrimitiveCollisionDataAccessor );

public:

  // ランタイムコードとデータフォーマットを合わせる為のバージョン
  enum 
  {
    VERSION_150702 = 0,   // 機能追加時


    // 現在のデータバージョン
    LATEST_VERSION = VERSION_150702
  };

  // 形状の種類
  enum Type
  {
    CIRCLE,


    // @note この並びを変更する場合はバイナリデータのパック順と、SetupDataStartAddrTableのアドレス計算を変更する必要がある
    TYPE_MAX,
  };

  // Circle
  struct Circle
  {
    f32 center[3];    /// 中心座標
    f32 radius;       /// 半径
    s32 attribute;    /// 保持アトリビュート
  };

  struct CollisionData
  {
    u32 type;
    union
    {
      Circle circle;
    };
  };

GFL_WARNING_PUSH
GFL_WARNING_WIN32_IGNORE(4200) //C4200警告を抑止
  // カメラエリア基本形状コリジョン配置情報ヘッダー (エリア単位）
  struct CollisionDataHeader
  {
    u32 version;          // データフォーマットのバージョン
    u32 totalNum;         // データ総数
    u32 num[ TYPE_MAX ];  // 形状別データ数
    u32 type[];
  };
GFL_WARNING_POP

public:

  /**
   *  @biref  コンストラクタ
   */
  PrimitiveCollisionDataAccessor() : m_pData( NULL )
  {
    for( u32 i = 0; i < TYPE_MAX; i++ )
    {
      m_DataStartAddrTable[ i ] = 0;
    }
  }

  /**
   *  @brief  デストラクタ
   */
  ~PrimitiveCollisionDataAccessor()
  {

  }

  /**
   *  @brief  初期化
   */
  void Initialize( const void * pData )
  {
    m_pData = reinterpret_cast<const CollisionDataHeader*>( pData );
    GFL_ASSERT( m_pData ); // 開発時に気がつくためのアサート
    GFL_ASSERT_STOP_MSG(  m_pData->version == LATEST_VERSION, "data version missing %d != %d\n", m_pData->version, LATEST_VERSION );
  
    // 各形状別のデータ開始アドレスを記憶したテーブルを生成
    SetupDataStartAddrTable();
  }

  /**
   *  @brief  データ総数
   */
  u32 GetTotalDataMax() const
  {
    GFL_ASSERT( m_pData ); // 開発時に気がつくためのアサート
    return m_pData->totalNum;
  }

  /**
  *  @brief  指定形状のデータ数を取得
  */
  u32 GetDataMax( Type type ) const
  {
    GFL_ASSERT( type < TYPE_MAX ); // 開発時に気がつくためのアサート
    return m_pData->num[ type ];
  }

  /**
  *  @brief 定形状のデータの情報取得
  */
  b32 GetData( Type type, u32 idx, CollisionData* p_out ) const
  {
    GFL_ASSERT( idx < GetDataMax( type ) ); // 開発時に気がつくためのアサート
    uptr addr = GetDataStartAddr( type );

    p_out->type = type;
    switch( type ){
    case CIRCLE:
      {
        Circle * p_data = reinterpret_cast<Circle*>(addr);
        p_data += idx;
        p_out->circle  = *p_data;
        break;
      }

    default:
      {
        GFL_ASSERT_MSG( 0, "type not found %d", type );
        return false;
      }
    }

    return true;
  }

  /**
  *  @brief 各形状別のデータ開始アドレスを取得
  */
  uptr GetDataStartAddr( Type type ) const
  {
    GFL_ASSERT( type < TYPE_MAX ); // 開発時に気がつくためのアサート
    return m_DataStartAddrTable[ type ];
  }

private:

  /**
  *  @brief 各形状別のデータ開始アドレスを記憶したテーブルを生成
  */
  void SetupDataStartAddrTable( void )
  {
    GFL_ASSERT( m_pData ); // 開発時に気がつくためのアサート
    size_t sizeTable[] =
    {
      sizeof( Circle )
    };

    uptr addr = reinterpret_cast<uptr>(m_pData->type);

    for( u32 i = 0; i < TYPE_MAX; i++ )
    {
      m_DataStartAddrTable[ i ] = addr;
      addr += sizeTable[ i ] * m_pData->num[ i ];
    }
  }


private:

  const CollisionDataHeader * m_pData;
  uptr  m_DataStartAddrTable[ TYPE_MAX ];

};


// 自由形状Hitデータへのアクセッサー
class MeshHitData
{
public:

  MeshHitData( void ) : 
    intersection( 0.0f, 0.0f, 0.0f )
    ,pTriangle( NULL )
    ,priority( 0 )
  {

  }

  virtual ~MeshHitData( void )
  {
  }

  /**
  * @brief エリア番号が有効値か取得する
  * @note 結びついているエリア番号が実在するかはここではチェックしていない、現状ではコリジョン側ではじいている
  * @note gfcolのコンバート時にアトリビュート名がデータXMLから取得できなかったマテリアルに対してHitしている場合にfalseが返る
  *
  * @return true 有効 false 無効
  */
  b32 IsAreaNoValid( void ) const
  {
    return ( pTriangle->m_GroundAttribute != -1 );
  }

  /**
  * @brief エリア番号を取得する
  * @note 結びついているエリア番号が実在するかはここではチェックしていない、現状ではコリジョン側ではじいている
  *
  * @return エリア番号
  * @retval Area::AreaNo::INVALID 無効値
  */
  s32 GetAreaNo( void ) const
  {
    if( IsAreaNoValid() == false )
    {
      return Area::AreaDataNo::INVALID;
    }

    return pTriangle->m_GroundAttribute;
  }

  /**
  * @brief 各頂点の頂点カラーから、交点の値を取得する
  *
  * @return 値[0.0..1.0]
  */
  f32 GetRatio( void ) const
  {
    // @todo Vector型からVector3型に置き換え
    gfl2::math::Vector3 positions[3];
    for( u32 i = 0; i < 3; i++ )
    {
      positions[i].x = pTriangle->m_Positions[i].x;
      positions[i].y = pTriangle->m_Positions[i].y;
      positions[i].z = pTriangle->m_Positions[i].z;
    }

    // 必要となるベクトル
    gfl2::math::Vector3 vec0to2( positions[2] - positions[0] );
    gfl2::math::Vector3 vec0to1( positions[1] - positions[0] );
    gfl2::math::Vector3 vec1to2( positions[2] - positions[1] );
    gfl2::math::Vector3 vec0toIntersection( intersection - positions[0] );
    gfl2::math::Vector3 vec1toIntersection( intersection - positions[1] );

    // 面積を計算
    f32 sizeFor01 = ( vec0to1.Cross( vec0toIntersection ) ).Length() * 0.5f;
    f32 sizeFor12 = ( vec1to2.Cross( vec1toIntersection ) ).Length() * 0.5f;
    f32 sizeFor02 = ( vec0to2.Cross( vec0toIntersection ) ).Length() * 0.5f;

    // 面積の比率から、交点のレートを計算
    f32 ratio = ( 
      pTriangle->m_vertexColorG[ 0 ] * sizeFor12 + 
      pTriangle->m_vertexColorG[ 1 ] * sizeFor02 + 
      pTriangle->m_vertexColorG[ 2 ] * sizeFor01 
      ) / ( sizeFor12 + sizeFor02 + sizeFor01 ) / 255;

    return gfl2::math::Clamp( ratio, 0.0f, 1.0f );
  }

  /**
  * @brief Hitデータを設定する
  *
  * @param  rSrc  元データ
  *
  * @return 無し
  */
  void Set( const MeshHitData& rSrc )
  {
    intersection = rSrc.intersection;
    pTriangle = rSrc.pTriangle;
    priority = rSrc.priority;
  }


  gfl2::math::Vector3 intersection;                                                   // 交点
  const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;    // Hitした3角ポリゴン
  s32  priority;                                                                      // むし

};

// カメラエリアコリジョンモデル用、レイキャストコールバック
class RaycastCustomCallback : public gfl2::collision::ICollisionModelIntersectionCallbackable
{
public:

  /**
  * @brief コンストラクタ
  */
  RaycastCustomCallback( const Area::AreaDataManager* pAreaDataManager ) : m_IsHit( false ), m_pAreaDataManager( pAreaDataManager )
  {
  }

  /**
  * @brief デストラクタ
  */
  virtual ~RaycastCustomCallback()
  {

  }

  /**
  * @brief 当たり判定コールバック関数
  */
  virtual bool CollisionModelIntersectionCallback(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector4& intersection)
  {
    s32 attribute = pTriangle->m_GroundAttribute;

    // 結びついているデータが見つからない場合はヒットしていない扱いにする
    if( m_pAreaDataManager->IsAreaDataNoValid( attribute ) == false )
    {
      GFL_ASSERT( 0 );  // 開発中に気づくためのアサート
      return true;
    }

    // 既にヒットしているならプライオリティをチェック
    s32 priority = m_pAreaDataManager->GetPriority( attribute );
    if( m_IsHit )
    {
      // 高い方を優先
      if( priority < m_HitData.priority )
      {
        return true;
      }

    }
    else
    {
      m_IsHit = true;
    }

    // 更新
    m_HitData.intersection.x = intersection.x;
    m_HitData.intersection.y = intersection.y;
    m_HitData.intersection.z = intersection.z;
    m_HitData.priority = priority;
    m_HitData.pTriangle = pTriangle;


    // 継続　@note プライオリティチェックをするので打ち切れない
    return true;
  }

  /**
  * @brief レイの一番近い交点を取得する
  * @return 衝突データ
  */
  b32 GetHitData( Area::MeshHitData* pOut ) const
  {
    if( IsHit() == false )
    {
      return false;
    }

    pOut->Set( m_HitData );
    return true;
  }

  /**
  * @brief 1度でも当たったかを返す
  * @retval true 当たった
  * @retval false 当たっていない
  */
  b32 IsHit(void) const
  {
    return m_IsHit;
  }

private:

  b32                  m_IsHit;
  Area::MeshHitData    m_HitData;
  
  // 外部オブジェクト
  const Area::AreaDataManager* m_pAreaDataManager;

};



/**
* @brief コンストラクタ
*
* @param rInitDesc  初期化設定
*
* @return 無し
*/ 
AreaCollisionManager::AreaCollisionManager( const InitializeDescription& rInitDesc ) : 
  m_pPrimitiveCollisionDataAccessor( NULL )
  ,m_pModelClollisionData( NULL )
  ,m_pModelClollisionDataResource( NULL )

{
  // 基本形状、無いときもヘッダー情報だけはある
  {
    m_pPrimitiveCollisionDataAccessor = GFL_NEW( rInitDesc.m_pSystemHeap ) PrimitiveCollisionDataAccessor;
    m_pPrimitiveCollisionDataAccessor->Initialize( rInitDesc.m_pPrimitiveData );
  }

  // 自由形状、無いときはNULL
  if( rInitDesc.m_pModelData )
  {
    s32 size = 0; // 使っていない
    gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
    resourceData.SetModelData( static_cast<c8*>( rInitDesc.m_pModelData ), size );
    m_pModelClollisionDataResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeTest( rInitDesc.m_pDeviceAllocator, &resourceData );

    m_pModelClollisionData = GFL_NEW( rInitDesc.m_pSystemHeap ) gfl2::collision::CollisionModel();
    m_pModelClollisionData->Initialize( m_pModelClollisionDataResource );
  }

}

/**
* @brief デストラクタ
*
* @return 無し
*/
AreaCollisionManager::~AreaCollisionManager( void )
{
  GFL_SAFE_DELETE( m_pModelClollisionData );
  GFL_SAFE_DELETE( m_pModelClollisionDataResource );
  GFL_SAFE_DELETE( m_pPrimitiveCollisionDataAccessor );
}

/**
* @brief 指定座標の判定を行う
*
* @param  rPos 指定座標
* @param  rAreaDataManager エリアデータ(forプライオリティ)
* @param  pOut 結果格納先
*
* @return 無し
*/
void AreaCollisionManager::Check( const gfl2::math::Vector3& rPos, const AreaDataManager& rAreaDataManager, HitResult* pOut ) const
{
  HitResult result;

  result.Clear();
  pOut->Clear();

  // Circle
  CheckCircle( rPos, rAreaDataManager, result, &result );

  // 自由形状
  CheckMesh( rPos, rAreaDataManager, result, &result );


  *pOut = result;
}

//
// private
// 

/**
* @brief 指定座標の判定を行う( 円形状 )
*
* @param  rPos 指定座標
* @param  rAreaDataManager エリアデータ(forプライオリティ)
* @param  rPresent         現状のHit状態、他の形状の結果が格納されている( pOut と同じインスタンスでも正常動作する )
* @param  pOut             結果格納先
*
* @return 無し
*/
void AreaCollisionManager::CheckCircle( const gfl2::math::Vector3& rPos, const AreaDataManager& rAreaDataManager, const HitResult& rPresent, HitResult* pOut ) const
{
  HitResult result;
  result = rPresent;

  u32 num = m_pPrimitiveCollisionDataAccessor->GetDataMax( PrimitiveCollisionDataAccessor::CIRCLE );
  if( num < 0 )
  {
    *pOut = result;
    return;
  }
  
  f32 heightCheckTop( rPos.y + Y_HIT_VOLUME_TOP );
  f32 heightCheckBottom( rPos.y + Y_HIT_VOLUME_BOTTOM );

  PrimitiveCollisionDataAccessor::Circle* pData = reinterpret_cast< PrimitiveCollisionDataAccessor::Circle* >( m_pPrimitiveCollisionDataAccessor->GetDataStartAddr( PrimitiveCollisionDataAccessor::CIRCLE ) );
  for( u32 i = 0; i < num; i++, pData++ )
  {
    // 高さ範囲ないか？
    if( pData->center[ 1 ] > heightCheckTop || pData->center[ 1 ] < heightCheckBottom )
    {
      continue;
    }

    // 半径範囲ないか?
    f32 squareX = rPos.x - pData->center[ 0 ];
    squareX *= squareX;
    f32 squareZ = rPos.z - pData->center[ 2 ];
    squareZ *= squareZ;
    f32 squareRadius = pData->radius * pData->radius;

    if( squareX + squareZ > squareRadius )
    {
      continue;
    }

    // Hit済みならプライオリティチェック
    if( result.m_IsHit )
    {
      s32 priorityWork = rAreaDataManager.GetPriority( pData->attribute );

      if( priorityWork > result.m_Priority )
      {
        // 今回のデータを採用
        result.m_Priority = priorityWork;
      }
      else
      {
        continue;
      }
    }
    else
    {
      result.m_IsHit = true;
      result.m_Priority = rAreaDataManager.GetPriority( pData->attribute );
    }

    // データ番号を結果に設定
    result.m_AreaDataNo = pData->attribute;

    // 中心-外周間のどの位置か、0..1.0で設定する
    f32 ratioWork = gfl2::math::FSqrt( squareX + squareZ ) / gfl2::math::FSqrt( squareRadius );
    result.m_Ratio = gfl2::math::Clamp( ratioWork, 0.0f, 1.0f );

  }

  *pOut = result;
}

/**
* @brief 指定座標の判定を行う( 自由形状 )
*
* @param  rPos 指定座標
* @param  rAreaDataManager エリアデータ(forプライオリティ)
* @param  rPresent         現状のHit状態、他の形状の結果が格納されている( pOut と同じインスタンスでも正常動作する )
* @param  pOut             結果格納先
*
* @return 無し
*/
void AreaCollisionManager::CheckMesh( const gfl2::math::Vector3& rPos, const AreaDataManager& rAreaDataManager, const HitResult& rPresent, HitResult* pOut ) const
{
  HitResult result;
  result = rPresent;

  if( m_pModelClollisionData == NULL )
  {
    *pOut = result;
    return;
  }

  f32 heightCheckTop( rPos.y + Y_HIT_VOLUME_TOP );
  f32 heightCheckBottom( rPos.y + Y_HIT_VOLUME_BOTTOM );

  RaycastCustomCallback callback( &rAreaDataManager );
  gfl2::math::Vector4 posStart; // @todo Vectror3に置き換える
  gfl2::math::Vector4 posEnd; // @todo Vectror3に置き換える
  posStart.x = rPos.x;
  posStart.y = 0.0f;
  posStart.z = rPos.z;
  posEnd = posStart;

  posStart.y = heightCheckTop;
  posEnd.y = heightCheckBottom;

  m_pModelClollisionData->GetIntersection( posStart, posEnd, NULL, &callback );

  if( callback.IsHit() )
  {
    Area::MeshHitData hitData;
    callback.GetHitData( &hitData );

    if( hitData.IsAreaNoValid() )
    {
      // Hit済みならプライオリティチェック
      if( result.m_IsHit == false || hitData.priority > result.m_Priority )
      {
        result.m_AreaDataNo = hitData.GetAreaNo();
        result.m_Ratio = hitData.GetRatio();
        result.m_IsHit = true;
      }
    }
  } // IsHit()

  *pOut = result;
}


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );