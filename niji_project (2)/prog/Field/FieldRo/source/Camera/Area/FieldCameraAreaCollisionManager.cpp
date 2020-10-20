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
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaDataManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaCollisionManager.h"

#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderManager.h"
#endif  //#if PM_DEBUG
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

// カメラエリアデータ、コリジョンルール
// 1. 自機の足元座標を基準に、Y座標を無限にヒットチェック
// 2. ヒットチェックに使用するレイはボリューム(太さ)を持たない
// 3. 複数ヒットした場合はプライオリティ値が高いデータを使用
// 4. プライオリティ値が同じ場合は「自機の足元→コリジョンの交点」の距離が短いデータを使用　(距離も同じ場合はデータ順で先勝ち)

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
    f32 centerRatio;  /// 中央比率
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

}; // class PrimitiveCollisionDataAccessor


// 自由形状Hitデータへのアクセッサー
class MeshHitData
{
public:

  MeshHitData( void ) : 
    m_Intersection( 0.0f, 0.0f, 0.0f )
    ,m_pTriangle( NULL )
  {
    m_HitResult.Clear();
  }

  virtual ~MeshHitData( void )
  {
  }

  // 内部データを無効化
  void Clear( void )
  {
    m_HitResult.Clear();
    m_Intersection = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
    m_pTriangle = NULL;
  }

  // プライオリティ判定、引数が勝つ場合はtrueが返る
  b32 CheckPriority( const gfl2::math::Vector3& rPos, s32 priority, f32 height ) const
  {
    return m_HitResult.CheckPriority( rPos, priority, height );
  }

  // Hit結果を取得
  void GetResult( AreaCollisionManager::HitResult* pOut ) const
  {
    *pOut = m_HitResult;

    if( m_HitResult.m_IsHit )
    {
      pOut->m_Ratio = GetRatio();
    }
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
    m_HitResult = rSrc.m_HitResult;
    m_Intersection = rSrc.m_Intersection;
    m_pTriangle = rSrc.m_pTriangle;
  }

  // Hitデータを設定する(パラメータ指定版)
  void Set( 
    u32 areaNo,
    s32 priority,
    const gfl2::math::Vector3& rIntersection,
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle
    )
  {
    m_HitResult.Set( areaNo, priority, rIntersection.y );
    m_Intersection = rIntersection;
    m_pTriangle = pTriangle;
  }


private:
  
  /**
  * @brief 各頂点の頂点カラーから、交点の値を取得する
  *
  * @return 値[0.0..1.0]
  */
  f32 GetRatio( void ) const
  {
    gfl2::math::Vector3 positions[3];
    for( u32 i = 0; i < 3; i++ )
    {
      positions[i].x = m_pTriangle->m_Positions[i].x;
      positions[i].y = m_pTriangle->m_Positions[i].y;
      positions[i].z = m_pTriangle->m_Positions[i].z;
    }

    // 必要となるベクトル
    gfl2::math::Vector3 vec0to2( positions[2] - positions[0] );
    gfl2::math::Vector3 vec0to1( positions[1] - positions[0] );
    gfl2::math::Vector3 vec1to2( positions[2] - positions[1] );
    gfl2::math::Vector3 vec0toIntersection( m_Intersection - positions[0] );
    gfl2::math::Vector3 vec1toIntersection( m_Intersection - positions[1] );

    // 面積を計算
    f32 sizeFor01 = ( vec0to1.Cross( vec0toIntersection ) ).Length() * 0.5f;
    f32 sizeFor12 = ( vec1to2.Cross( vec1toIntersection ) ).Length() * 0.5f;
    f32 sizeFor02 = ( vec0to2.Cross( vec0toIntersection ) ).Length() * 0.5f;

    // 面積の比率から、交点のレートを計算
    f32 ratio = ( 
      m_pTriangle->m_vertexColorG[ 0 ] * sizeFor12 + 
      m_pTriangle->m_vertexColorG[ 1 ] * sizeFor02 + 
      m_pTriangle->m_vertexColorG[ 2 ] * sizeFor01 
      ) / ( sizeFor12 + sizeFor02 + sizeFor01 ) / 255;

    return gfl2::math::Clamp( ratio, 0.0f, 1.0f );
  }

  AreaCollisionManager::HitResult m_HitResult;
  gfl2::math::Vector3             m_Intersection;                                       // 交点
  const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* m_pTriangle;    // Hitした3角ポリゴン

}; // class MeshHitData

// カメラエリアコリジョンモデル用、レイキャストコールバック
class RaycastCustomCallback : public gfl2::collision::ICollisionModelCallbackable
{
public:

  /**
  * @brief コンストラクタ
  */
  RaycastCustomCallback( const Area::AreaDataManager* pAreaDataManager, const gfl2::math::Vector3& rControlPositon ) : 
    m_ControlPositon( rControlPositon ),
    m_pAreaDataManager( pAreaDataManager )
  {
    m_HitData.Clear();
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
  virtual bool CollisionModelHeightCallback(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height )
  {
    s32 attribute = pTriangle->m_GroundAttribute;






    // 結びついているデータが見つからない場合はヒットしていない扱いにする
    // @note gfcolのコンバート時にアトリビュート名がデータXMLから取得できなかった場合は-1になっている
    if( attribute < 0 || m_pAreaDataManager->IsAreaDataNoValid( static_cast<u32>( attribute ) ) == false )
    {
      // GFL_ASSERT( 0 );  // 開発中に気づくためのアサート
      return true;
    }

    // 既にヒットしているならプライオリティをチェック
    s32 priority = m_pAreaDataManager->GetPriority( attribute );

    if( m_HitData.CheckPriority( m_ControlPositon, priority, height ) )
    {
      // 今回のデータを使用

    }
    else
    {
      // プライオリティ負け
      return true;
    }

    // 更新
    m_HitData.Set
      (
      attribute,
      priority,
      gfl2::math::Vector3( m_ControlPositon.x, height, m_ControlPositon.z ),
      pTriangle
      );
      
    // 継続　@note プライオリティチェックをするので打ち切れない
    return true;
  }

  /**
  * @brief 衝突データを取得する
  *
  * @param pOut 格納先
  *
  * @return 無し
  */
  void GetHitData( Area::MeshHitData* pOut ) const
  {
    pOut->Set( m_HitData );
  }

private:

  gfl2::math::Vector3             m_ControlPositon;
  Area::MeshHitData               m_HitData;
  
  // 外部オブジェクト
  const Area::AreaDataManager* m_pAreaDataManager; // 設定データ管理、プライオリティ取得に使用する

}; // class RaycastCustomCallback



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
    m_pModelClollisionDataResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( rInitDesc.m_pDeviceAllocator, &resourceData );

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

  PrimitiveCollisionDataAccessor::Circle* pData = reinterpret_cast< PrimitiveCollisionDataAccessor::Circle* >( m_pPrimitiveCollisionDataAccessor->GetDataStartAddr( PrimitiveCollisionDataAccessor::CIRCLE ) );
  for( u32 i = 0; i < num; i++, pData++ )
  {
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
    s32 priorityWork = rAreaDataManager.GetPriority( pData->attribute );
    if( result.CheckPriority( rPos, priorityWork, pData->center[ 1 ] ) )
    {
      // 中心-外周間のどの位置か、0..1.0で設定する
      f32 ratioWork = gfl2::math::FSqrt( squareX + squareZ ) / gfl2::math::FSqrt( squareRadius );
      ratioWork = gfl2::math::Clamp( ratioWork, 0.0f, 1.0f );
      

      if( pData->centerRatio >= 1.0f )
      {
        // 必ず0.0fになる
        ratioWork = 0.0f;
      }
      else if( pData->centerRatio == 0.0f )
      {
        // 何もしない
      }
      else if( ratioWork <= pData->centerRatio )
      {
        // 中央比率計算(0.0以下切捨て)
        ratioWork = 0.0f;
      }
      else
      {
        // 中央比率計算
        ratioWork = ( ratioWork - pData->centerRatio ) / ( 1.0f - pData->centerRatio );
      }

      // 今回のデータを採用
      result.Set(
        pData->attribute,
        priorityWork,
        pData->center[ 1 ],
        ratioWork
        );
    }
    else
    {
      continue;
    }
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

  RaycastCustomCallback callback( &rAreaDataManager, rPos );
  gfl2::math::Vector4 pos( rPos );

  m_pModelClollisionData->GetHeight( pos, NULL, &callback );

  Area::MeshHitData hitData;
  HitResult resultWork;
  callback.GetHitData( &hitData );
  hitData.GetResult( &resultWork );

  // HitしているならHitデータ更新
  if( resultWork.IsHit() )
  {
    // 他形状の結果とのプライオリティチェック
    if( result.CheckPriority( rPos, resultWork ) )
    {
      result = resultWork;
    }
  }

  *pOut = result;
}


#if PM_DEBUG
/**
*  @brief  カメラエリアをデバッグ表示へ登録
*
*  @param  color  コリジョン表示色
*  @param  onlyActiveFlag true:アクティブなコリジョンのみ表示する false：全て表示する
*/
void AreaCollisionManager::DebugRegistRenderCollisionArea( gfl2::math::Vector4 color, s32 hitAttr, b32 onlyActiveFlag )
{
  Debug::DebugCollisionRenderManager* pManager = GFL_SINGLETON_INSTANCE( Debug::DebugCollisionRenderManager );
  if( ( pManager != NULL ) && ( m_pModelClollisionData != NULL ) )
  {
    pManager->RegistRenderCollisionModel( m_pModelClollisionData, color, true, hitAttr);
  }
#if 0 //  GetData()がASSERTするのでパス
  if( ( pManager != NULL ) && ( m_pPrimitiveCollisionDataAccessor != NULL ) )
  {
    PrimitiveCollisionDataAccessor::CollisionData data;
    m_pPrimitiveCollisionDataAccessor->GetData(PrimitiveCollisionDataAccessor::Type::CIRCLE, 0, &data);
    pManager->RegistRenderCollisionCylinder( gfl2::math::Vector3(data.circle.center[0], data.circle.center[1], data.circle.center[2]), data.circle.radius, 0.0f, color );
  }
#endif  // #if 0
};
#endif  //#if PM_DEBUG


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );
