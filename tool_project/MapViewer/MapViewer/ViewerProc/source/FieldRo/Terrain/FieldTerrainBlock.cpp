/**
* @brief  フィールドの地形ブロッククラス
* @file   TerrainBlock.cpp
* @author ikuta_junya
* @data   2015.04.19
*/

// field
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainBlock.h"
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainFactory.h"
///#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodChangeStaticListener.h"
#include "ViewerProc/include/FieldStatic/Collision/CollisionSceneNone.h"
#include "ViewerProc/include/FieldStatic/Collision/StaticActor.h"
///#include "Field/FieldRo/include/KusaModel/FieldKusaModel.h"

// field debug
///#include "Field/FieldRo/include/Debug/FieldDebugTypes.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

const static u32 INVISIBLE_CULLING = 0x01;
const static u32 INVISIBLE_DEBUG_DRAW_SWITCH = 0x01 << 1;

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TerrainBlock::TerrainBlock( void ) :

  // 制御
  m_X( 0 )
  ,m_Z( 0 )
  ,m_Size( 0.0f )
  ,m_Pos( 0.0f ,0.0f, 0.0 )
  ///,m_RequestLodType( Block::LodType::MIDDLE )
  ///,m_LodType( Block::LodType::MIDDLE )
  ,m_InvisibleBit( 0 )
  ,m_IsLoading( false )
  ///,m_IsHighModel( false )

  // オブジェクト
  ,m_pCollisionModel( NULL )
  ,m_pStaticActor( NULL )
  ,m_pWallCollisionModel( NULL )
  ,m_pWallStaticActor( NULL )
  ///,m_ppLodChangeStaticListenerTable( NULL )
  ,m_LodChangeStaticListenerMax( 0 )
  
  // リソースデータ
  // 外部オブジェクト
  ,m_pTerrainFactory( NULL )
  ,m_pCollisionScene( NULL )
  ,m_pWallCollisionScene( NULL )
{
#if 0
  for( u32 i = 0; i < Block::LodType::MAX; i++ )
  {
    // m_pModelNodeTable[ i ] = NULL;
    m_pModelTable[ i ] = NULL;
    ///m_ppEncountGrassModelTable[i] = NULL;
  }
#endif //@saito_del
	m_pModelTable = NULL; //@satio_add
}

/**
* @brief デストラクタ
*
* @return 無し
*/
TerrainBlock::~TerrainBlock( void )
{
  Terminate();
}

/**
* @brief 初期化処理
*
* @param  rInitDesc   初期化設定
*
* @return 無し
*/
void TerrainBlock::Initialize( const InitializeDescription& rInitDesc )
{
  m_Pos = rInitDesc.m_Pos;
  m_X = rInitDesc.m_X;
  m_Z = rInitDesc.m_Z;
  m_Size = rInitDesc.m_Size;
  m_pTerrainFactory = rInitDesc.pTerrainFactory;
  m_pCollisionScene = rInitDesc.pCollisionScene;
  m_pWallCollisionScene = rInitDesc.m_pWallCollisionScene;

  ///m_IsHighModel = m_pTerrainFactory->CheckHighModelDataVailed( m_X, m_Z );

  // MiddleMode生成
  // m_pModelNodeTable[ Block::LodType::MIDDLE ] = m_pTerrainFactory->CreateMiddleModelNode( m_X, m_Z );
  ///m_pModelTable[ Block::LodType::MIDDLE ] = m_pTerrainFactory->CreateMiddleModel( m_X, m_Z );@saito_del
  m_pModelTable = m_pTerrainFactory->CreateMiddleModel( m_X, m_Z );//@saito_add
  /**
	// リスナー管理テーブル作成
  m_LodChangeStaticListenerMax = rInitDesc.m_LodChangeStaticListenerMax;
  if( m_LodChangeStaticListenerMax > 0 )
  {
    m_ppLodChangeStaticListenerTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) Block::LodChangeStaticListener* [ m_LodChangeStaticListenerMax ];
    for( u32 i = 0; i < m_LodChangeStaticListenerMax; i++ )
    {
      m_ppLodChangeStaticListenerTable[ i ] = NULL;
    }
  }
	*///@saito_del
}

/**
* @brief 終了処理
*
* @return 無し
*/
void TerrainBlock::Terminate( void )
{
  // リスナー管理テーブル破棄
  ///GFL_SAFE_DELETE_ARRAY( m_ppLodChangeStaticListenerTable );
  m_LodChangeStaticListenerMax = 0;

  // 壁コリジョン登録解除・破棄
  {
    if( m_pWallStaticActor )
    {
      m_pWallCollisionScene->UnRegistStaticActor( m_pWallStaticActor );
      GFL_SAFE_DELETE( m_pWallStaticActor );
    }
    GFL_SAFE_DELETE( m_pWallCollisionModel );
  }

  // 高さコリジョン登録解除・破棄
  {
    if( m_pStaticActor )
    {
      m_pCollisionScene->UnRegistStaticActor( m_pStaticActor );
      GFL_SAFE_DELETE( m_pStaticActor );
    }
    GFL_SAFE_DELETE( m_pCollisionModel );
  }
	/**
  // 草モデル
  {
    for( u32 i = 0; i < Block::LodType::MAX; i++ )
    {
      if( m_ppEncountGrassModelTable[ i ] ){
        for( int j=0; j<m_pTerrainFactory->GetEncountGrassModelTableNum(); ++j ){ GFL_SAFE_DELETE( m_ppEncountGrassModelTable[ i ][ j ] ); }
        GFL_SAFE_DELETE_ARRAY(m_ppEncountGrassModelTable[ i ]);
      }
    }
  }
	*///@saito_del
  // @todo 1フレーム遅れ開放
  // @todo ロード完了後開放
  // @todo コリジョンモデル開放
  /**
	for( u32 i = 0; i < Block::LodType::MAX; i++ )
  {
    // GFL_SAFE_DELETE( m_pModelNodeTable[ i ] );
    GFL_SAFE_DELETE( m_pModelTable[ i ] );

  }
	*/ //@saito_del
	GFL_SAFE_DELETE( m_pModelTable );//@saito_add


  if( m_pTerrainFactory->CheckHighModelUnitState( m_X, m_Z ) == TerrainFactory::UnitState::LOAD_COMPLETION )
  {
    m_IsLoading = false;
    m_pTerrainFactory->UnLoadHighModel( m_X, m_Z );
  }
}
#if 0
/**
* @brief 更新処理
*
* @return 無し
*/
void TerrainBlock::Update( void )
{
  // @todo 状態別処理を整理

  // 1フレーム遅れて、highモデルを開放する
#if 0
  if( m_pModelNodeTable[ Block::LodType::HIGH ] && m_LodType != Block::LodType::HIGH )
  {
    GFL_SAFE_DELETE( m_pModelNodeTable[ Block::LodType::HIGH ] );

    // 壁コリジョン登録解除・破棄
    {
      if( m_pWallStaticActor )
      {
        m_pWallCollisionScene->UnRegistStaticActor( m_pWallStaticActor );
        GFL_SAFE_DELETE( m_pWallStaticActor );
      }
      GFL_SAFE_DELETE( m_pWallCollisionModel );
    }

    // 高さコリジョン登録解除・破棄
    {
      if( m_pStaticActor )
      {
        m_pCollisionScene->UnRegistStaticActor( m_pStaticActor );
        GFL_SAFE_DELETE( m_pStaticActor );
      }
      GFL_SAFE_DELETE( m_pCollisionModel );
    }
    
    m_pTerrainFactory->UnLoadHighModel( m_X, m_Z );
  }
#endif

  if( m_pModelTable[ Block::LodType::HIGH ] && m_LodType != Block::LodType::HIGH )
  {
    GFL_SAFE_DELETE( m_pModelTable[ Block::LodType::HIGH ] );

    // エンカウント草の破棄
    {
      if( m_ppEncountGrassModelTable[ Block::LodType::HIGH ] ){
      
        for( int i=0; i<m_pTerrainFactory->GetEncountGrassModelTableNum(); ++i ){ GFL_SAFE_DELETE( m_ppEncountGrassModelTable[ Block::LodType::HIGH ][i] ); }
        GFL_SAFE_DELETE_ARRAY(m_ppEncountGrassModelTable[ Block::LodType::HIGH ]);
      }
    }

    // 壁コリジョン登録解除・破棄
    {
      if( m_pWallStaticActor )
      {
        m_pWallCollisionScene->UnRegistStaticActor( m_pWallStaticActor );
        GFL_SAFE_DELETE( m_pWallStaticActor );
      }
      GFL_SAFE_DELETE( m_pWallCollisionModel );
    }

    // 高さコリジョン登録解除・破棄
    {
      if( m_pStaticActor )
      {
        m_pCollisionScene->UnRegistStaticActor( m_pStaticActor );
        GFL_SAFE_DELETE( m_pStaticActor );
      }
      GFL_SAFE_DELETE( m_pCollisionModel );
    }
    
    m_pTerrainFactory->UnLoadHighModel( m_X, m_Z );
  }

  // highモデルロード中の処理
  if( m_IsLoading )
  {
    // 完了チェック
    if( m_pTerrainFactory->CheckHighModelUnitState( m_X, m_Z ) == TerrainFactory::UnitState::LOAD_COMPLETION )
    {
#if 0
      if( m_pModelNodeTable[ m_LodType ] )
      {
        m_pModelNodeTable[ m_LodType ]->SetVisible( false );
      }
      m_pModelNodeTable[ Block::LodType::HIGH ] = m_pTerrainFactory->CreateHighModelNode( m_X, m_Z );
#endif

      if( m_pModelTable[ m_LodType ] )
      {
        UpdateVisibleImpl( m_LodType, false );
      }
      m_pModelTable[ Block::LodType::HIGH ] = m_pTerrainFactory->CreateHighModel( m_X, m_Z );
      m_ppEncountGrassModelTable[ Block::LodType::HIGH ] = m_pTerrainFactory->CreateEncountGrassModelTable( m_X, m_Z );

      m_pCollisionModel = m_pTerrainFactory->CreateHeightCollisionModel( m_X, m_Z );
      m_LodType = Block::LodType::HIGH;
      this->ChangeLodNotificationListener( m_LodType );
      m_IsLoading = false;

      // 高さコリジョン生成・登録
      {
        // 内こともあるかも
        if( m_pCollisionModel )
        {
          m_pStaticActor = m_pCollisionScene->CreateStaticActor();
          m_pStaticActor->CreateMesh( gfl2::math::Vector3(0.0f,0.0f,0.0f), m_pCollisionModel );
        }

        if( m_pStaticActor )
        {
          m_pCollisionScene->RegistStaticActor( m_pStaticActor );
        }
      }

      // 壁コリジョン生成・登録
      {
        m_pWallCollisionModel = m_pTerrainFactory->CreateWallCollisionModel( m_X, m_Z );

        // 内こともあるかも
        if( m_pWallCollisionModel )
        {
          m_pWallStaticActor = m_pWallCollisionScene->CreateStaticActor();
          m_pWallStaticActor->CreateMesh( gfl2::math::Vector3(0.0f,0.0f,0.0f), m_pWallCollisionModel );
          m_pWallCollisionScene->RegistStaticActor( m_pWallStaticActor );
        }

      }
    }
    else
    {
      return;
    }
  }

  // 見た目切り替え処理
  if( m_RequestLodType != m_LodType )
  {
    // highモデルはロードが入るので特殊
    if( m_RequestLodType == Block::LodType::HIGH )
    {
      // ロードリクエストを投げる
      if( m_pTerrainFactory->GetHighModelUnitUnRunnigNum() > 0 )
      {
        m_pTerrainFactory->LoadHighModelAsync( m_X, m_Z );
        m_IsLoading = true;
      }
    }
    else
    {
#if 0
      if( m_pModelNodeTable[ m_LodType ] )
      {
        m_pModelNodeTable[ m_LodType ]->SetVisible( false );
      }
#endif
      if( m_pModelTable[ m_LodType ] )
      {
        UpdateVisibleImpl( m_LodType, false );
      }

      m_LodType = m_RequestLodType;
      this->ChangeLodNotificationListener( m_LodType );
    }
  }

#if 0
  // Visible更新処理
  if( m_pModelNodeTable[ m_LodType ] )
  {
    m_pModelNodeTable[ m_LodType ]->SetVisible( true );
    m_pModelNodeTable[ m_LodType ]->SetLocalTranslate( m_Pos );  // @todo 毎回やらなくする
  }
#endif

  // Visible制御
  {
#if PM_DEBUG
    // デバッグスイッチによる非表示
    b32 debugVisible = true;
    switch( m_LodType )    // ここでみるのは本当のState
    {
    case Block::LodType::MIDDLE: debugVisible = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_LOD ); break;
    case Block::LodType::LOW: debugVisible = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_LOD ); break;
    case Block::LodType::HIGH: debugVisible = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_HIGH ); break;
    }

    if( debugVisible )
    {
      m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_DEBUG_DRAW_SWITCH) );
    }
    else
    {
      m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_DEBUG_DRAW_SWITCH );
    }
#endif

    UpdateVisibleImpl( m_LodType, !m_InvisibleBit );
  }

  // Visible更新処理
  if( m_pModelTable[ m_LodType ] )
  {
    m_pModelTable[ m_LodType ]->SetPosition( m_Pos );  // @todo 毎回やらなくする
    m_pModelTable[ m_LodType ]->UpdateAnimation();
  }
}
#endif //@saito_del
#if 0
/**
* @brief LOD変更処理
*
* @param  lodLevel 変更したい状態
*
* @return 無し
*/
void TerrainBlock::ChangeLod( Block::LodType::Value lodLevel )
{
  if( lodLevel == Block::LodType::HIGH )
  {
    // return;
  }
  // Highモデル無い。。。
  if( m_IsHighModel == false && lodLevel == Block::LodType::HIGH )
  {
    return;
  }

  if( m_RequestLodType == lodLevel )
  {
    return;
  }

  m_RequestLodType = lodLevel;
}

/**
* @brief LOD変更通知を受け取るリスナーを登録
* @note  リスナーの寿命は登録側で管理してください
*
* @param  pListener  登録するリスナー
*
* @return 成否
*/
b32 TerrainBlock::RegistListener( Block::LodChangeStaticListener* pListener )
{
  // 空きスペースに設定
  Block::LodChangeStaticListener** ppWork = m_ppLodChangeStaticListenerTable;
  for( u32 i = 0; i < m_LodChangeStaticListenerMax; i++, ppWork++ )
  {
    if( *ppWork == NULL )
    {
      *ppWork = pListener;
      pListener->RegistAfter( m_LodType );
      return true;
    }

  }

  return false;
}

/**
* @brief LOD変更通知を受け取るリスナーを登録削除
* @note  リスナーの寿命は登録側で管理してください
*
* @param  pListener  登録削除するリスナー
*
* @return 成否
*/
b32 TerrainBlock::UnRegistListener( Block::LodChangeStaticListener* pListener )
{
  Block::LodChangeStaticListener** ppWork = m_ppLodChangeStaticListenerTable;
  for( u32 i = 0; i < m_LodChangeStaticListenerMax; i++, ppWork++ )
  {
    if( *ppWork == pListener )
    {
      *ppWork = NULL;
      return true;
    }

  }

  return false;
}
#endif //@saito_del
//
// private
//
#if 0
/**
* @brief LOD変更をリスナーに通知
*
* @param  lodType 変更後の状態
*
* @return 無し
*/
void TerrainBlock::ChangeLodNotificationListener( Block::LodType::Value lodType )
{
  Block::LodChangeStaticListener** ppWork = m_ppLodChangeStaticListenerTable;
  for( u32 i = 0; i < m_LodChangeStaticListenerMax; i++, ppWork++ )
  {
    if( *ppWork )
    {
      (*ppWork)->ChangeLod( lodType );
    }
  }
}

/**
* @brief 地形モデルの表示・非表示切り替え
*
* @param lodType  対象となるLODモデル
* @param visible  true 表示 false 非表示
*
* @return 無し
*/
void TerrainBlock::UpdateVisibleImpl( Block::LodType::Value lodType, b32 visible )
{
  if( m_pModelTable[ lodType ] == NULL )
  {
    return;
  }

  if( m_pModelTable[ lodType ]->IsVisible() == visible )
  {
    return;
  }

  m_pModelTable[ lodType ]->SetVisible( visible );
  for( int i=0; i<m_pTerrainFactory->GetEncountGrassModelTableNum(); ++i )
  {
    if(m_ppEncountGrassModelTable[ lodType ] && m_ppEncountGrassModelTable[ lodType ][i])
    {
      m_ppEncountGrassModelTable[ lodType ][i]->SetVisible( visible ); 
    }
  }

#if 0
  // @todo 重いかも？
  if( visible )
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pModelInstanceNodeTable[ lod ] );
  }
  else
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNodeTable[ lod ] );
  }
#endif
}
#endif //@saito_del

GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );