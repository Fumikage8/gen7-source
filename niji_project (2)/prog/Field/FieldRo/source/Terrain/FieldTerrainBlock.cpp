/**
* @brief  フィールドの地形ブロッククラス
* @file   TerrainBlock.cpp
* @author ikuta_junya
* @data   2015.04.19
*/

// gfl2
#include <core/include/gfl2_Utility.h>

// field
#include "Field/FieldRo/include/Terrain/FieldTerrainBlock.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainFactory.h"
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodChangeStaticListener.h"
#include "Field/FieldRo/include/Collision/CollisionSceneNone.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/KusaModel/FieldKusaModel.h"
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockTypes.h"
#include "Field/FieldRo/include/Camera/FieldCameraHitObject.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// CTRプロファリング用
#include "Field/FieldRo/include/Fieldmap.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

const static u32 INVISIBLE_CULLING = 0x01;
const static u32 INVISIBLE_DEBUG_DRAW_SWITCH = 0x01 << 1;
const static u32 INVISIBLE_MIDDLE_DISTANCE = 0x01 << 2;     // 距離によるmiddleモデルの表示制限

// @note エンカウント草を後から描画する事により負荷が軽くなる見込みだったが、逆に重くなってしまったので一旦デフォルトプライオリティを指定しています
static u32 ENCOUNT_GRASS_MODEL_PRIORITY( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::s_DefaultPriority );  // エンカウント草の不透明モデル内での描画の優先度(0～255、0から順に描画)

//
// カメラ接触時に非表示にするメッシュの命名規則
// 1. 
// ・cc_名前
// 2. 透過モード
// ・cc_a_名前   透過前のメッシュ
// ・cc_b_名前   透過後のメッシュ(半透明設定、コンバイナシェーダーでコンスタントカラー5のアルファ値を参照、マテリアルを別にする)
//
// @note 下記の理由から、プログラムで半透明オブジェクトにする(マテリアル書き換え)のをあきらめ、モデルにあらかじめ透過前と透過後の2つのメッシュを配置していただく形式を取った
//       1. ctr版のブレンドパラメーターの上書き機能が未実装（実装すると追加描画コマンド分だけメモリ使用量が増えてしまい、処理負荷が上がる）
//       2. 全マップ共通で成り立つような描画プライオリティの設定が、今からでは困難
//       3. 実装時期がデバッグ開始直前
struct CameraCollisionMeshType
{
  enum Value
  {
    NOT_SUBJECT,  // 対象外
    VISIBLE,      // Type[VISIBLE]のメッシュ
    ALPHA_BASE,   // Type[ALPHA]
    ALPHA_AFTER,
  };
};

// 指定メッシュがカメラコリジョン(非表示メッシュ)対象かチェック
CameraCollisionMeshType::Value CheckCameraEyeCollisionTarget( const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMesh, b32 checkVisible = true )
{
  if( checkVisible && !pMesh->IsVisible() )
  {
    return CameraCollisionMeshType::NOT_SUBJECT;
  }

  if( gfl2::core::Utility::Strncmp( pMesh->GetName(), "cc_", 3 ) == 0 )
  {
    if( gfl2::core::Utility::Strncmp( pMesh->GetName() + 3, "a_", 2 ) == 0 )
    {
      return CameraCollisionMeshType::ALPHA_BASE;
    }

    if( gfl2::core::Utility::Strncmp( pMesh->GetName() + 3, "b_", 2 ) == 0 )
    {
      return CameraCollisionMeshType::ALPHA_AFTER;
    }

    return CameraCollisionMeshType::VISIBLE;
  }

  return CameraCollisionMeshType::NOT_SUBJECT;
}

// 透過前のメッシュから、透過後のメッシュのindexを検索
s32 SearchCameraEyeCollisionTargetAlphaAfter( const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pBaseMesh, const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode )
{
  c8 nameWork[ gfl2::renderingengine::scenegraph::NodeName::NAME_LENGTH ];
  gfl2::core::Utility::Memcpy( nameWork, pBaseMesh->GetName(), sizeof( nameWork ) );

  //
  // 透過前のメッシュ名から透過後のメッシュ名のハッシュ値を生成
  // 
  
  // cc_a_** -> cc_b_**
  nameWork[ 3 ] = 'b';
  gfl2::util::FnvHash nameHash( reinterpret_cast<const u8*>( &nameWork[0] ) ); 
  u32 nameHashValue = nameHash.value;

  //
  // 透過後のメッシュを検索
  //

  u32 meshNum = pModelInstanceNode->GetMeshNum();
  for( s32 i = 0; i < meshNum; i++ )
  {
    const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLink = pModelInstanceNode->GetMeshLinkData( i );

#if 0
    // 運用が面倒なので、プログラムでfalseにする事にした
    // Afterは非表示になっている
    if( pMeshLink->IsVisible() )
    {
      continue;
    }
#endif

    if( pMeshLink->GetNameHash() == nameHashValue )
    {
      return i;
    }
  }

  return -1;
}




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
  ,m_RequestLodType( Block::LodType::MIDDLE )
  ,m_LodType( Block::LodType::MIDDLE )
  ,m_LodTypePrev( m_LodType )
  ,m_InvisibleBit( 0 )
  ,m_InvisibleBitForEncountGrass( 0 )
  ,m_IsLoading( false  )
  ,m_IsHighModel( false )
  ,m_ReplaceNo( 0 )
  ,m_FactoryUnitNo( -1 )
  ,m_EncountGrassModelPriority( 0 )
  ,m_pEncountGrassCullingInfoTable( NULL )

  // オブジェクト
  ,m_pCollisionModel( NULL )
  ,m_pStaticActor( NULL )
  ,m_pStaticLodListenerList( NULL )
  ,m_pStaticActorListForCameraEye( NULL )
  
  // リソースデータ
  // 外部オブジェクト
  ,m_pTerrainFactory( NULL )
  ,m_pCollisionScene( NULL )
  ,m_ppWallCollisionSceneTable( NULL )
  ,m_ppWallCollisionModelTable( NULL )
  ,m_ppWallStaticActorTable( NULL )
  ,m_pCameraEyeCollsionScene( NULL )
  ,m_IsHideMeshSystemOff( false )
{
  for( u32 i = 0; i < Block::LodType::MAX; i++ )
  {
    // m_pModelNodeTable[ i ] = NULL;
    m_pModelTable[ i ] = NULL;
    m_ppEncountGrassModelTable[i] = NULL;
  }
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
  m_ReplaceNo = rInitDesc.m_ReplaceNo;
  m_Size = rInitDesc.m_Size;
  m_pTerrainFactory = rInitDesc.pTerrainFactory;
  m_pCollisionScene = rInitDesc.pCollisionScene;
  m_ppWallCollisionSceneTable = rInitDesc.m_ppWallCollisionSceneTable;
  m_pCameraEyeCollsionScene = rInitDesc.m_pCameraEyeCollsionScene;


  // MiddleMode生成
  // m_pModelNodeTable[ Block::LodType::MIDDLE ] = m_pTerrainFactory->CreateMiddleModelNode( m_X, m_Z );
  m_pModelTable[ Block::LodType::MIDDLE ] = m_pTerrainFactory->CreateMiddleModel( m_X, m_Z, m_ReplaceNo );

  // リスナー管理リスト作成
  m_pStaticLodListenerList = GFL_NEW( rInitDesc.m_pSystemHeap ) StaticLodListenerList( rInitDesc.m_pSystemHeap );


  // モデルインスタンスに座標を設定
  for( u32 i = 0; i < Block::LodType::MAX; i++ )
  {
    if( m_pModelTable[i] )
    {
      m_pModelTable[ i ]->SetPosition( m_Pos );
    }
  }

  // 壁用のリソーステーブル生成
  m_ppWallCollisionModelTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) gfl2::collision::CollisionModel* [ WallBinIndex::MAX ];
  for( u32 i = 0; i < WallBinIndex::MAX; i++ )
  {
    m_ppWallCollisionModelTable[ i ] = NULL;
  }

  m_ppWallStaticActorTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) StaticActor* [ WallIndex::MAX ];
  for( u32 i = 0; i < WallIndex::MAX; i++ )
  {
    m_ppWallStaticActorTable[ i ] = NULL;
  }


  // エンカウント草のカリング制御用情報
  m_pEncountGrassCullingInfoTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) ENCOUNT_GRASS_CULLING_INFO [ m_pTerrainFactory->GetEncountGrassModelTableNum() ];
  for( u32 i = 0; i < m_pTerrainFactory->GetEncountGrassModelTableNum(); i++ )
  {
    m_pEncountGrassCullingInfoTable[ i ].isCulling = false;
    m_pEncountGrassCullingInfoTable[ i ].isOverTerrainModel = false;
  }
}

/**
* @brief 地形ブロックのコントロール開始
* @note  フィールド初期化の流れで実行される
*
* @return 無し
*/
void TerrainBlock::ControlStart( void )
{
  // Highモデルが有効かチェック、ファクトリーの初期化後に行う必要があるため、このタイミングで行う
  m_IsHighModel = m_pTerrainFactory->CheckHighModelDataVailed( m_X, m_Z, m_ReplaceNo );
}

/**
* @brief 終了処理
*
* @return 無し
*/
void TerrainBlock::Terminate( void )
{
  GFL_SAFE_DELETE( m_pEncountGrassCullingInfoTable );

  // @note ここにはフィールド破棄時にしか来ない、必ず開放できる状態
  if( m_pStaticActorListForCameraEye )
  {
    DeleteAndUnResistCollisionCameraEye( m_pStaticActorListForCameraEye ); 
    GFL_SAFE_DELETE( m_pStaticActorListForCameraEye );
  }

  // リスナー管理テーブル破棄
  GFL_SAFE_DELETE( m_pStaticLodListenerList );

  // 壁コリジョン登録解除・破棄
  if( m_ppWallStaticActorTable )
  {
    for( u32 i = 0; i < WallIndex::MAX; i++ )
    {
      if( m_ppWallStaticActorTable[ i ] )
      {
        if( m_ppWallCollisionSceneTable && m_ppWallCollisionSceneTable[ i ] )
        {
          m_ppWallCollisionSceneTable[ i ]->UnRegistStaticActor( m_ppWallStaticActorTable[i] );
        }
        GFL_SAFE_DELETE( m_ppWallStaticActorTable[i] );
      }
    }
    GFL_SAFE_DELETE_ARRAY( m_ppWallStaticActorTable );
  }

  m_ppWallCollisionSceneTable = NULL;

  if( m_ppWallCollisionModelTable )
  {
    for( u32 i = 0; i < WallBinIndex::MAX; i++ )
    {
      if( m_ppWallCollisionModelTable[ i ] )
      {
        GFL_SAFE_DELETE( m_ppWallCollisionModelTable[i] );
      }
    }
    GFL_SAFE_DELETE_ARRAY( m_ppWallCollisionModelTable );
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

  // 草モデル
  {
    for( u32 i = 0; i < Block::LodType::MAX; i++ )
    {
      if( m_ppEncountGrassModelTable[ i ] )
      {
        m_pTerrainFactory->DeleteEncountGrassModelTable( &m_ppEncountGrassModelTable[i] );
      }
    }
  }

  // 見た目
  for( u32 i = 0; i < Block::LodType::MAX; i++ )
  {
    m_pTerrainFactory->DeleteModel( &m_pModelTable[ i ] );
  }

  if( m_pTerrainFactory->CheckHighModelUnitState( m_X, m_Z ) == TerrainFactory::UnitState::LOAD_COMPLETION )
  {
    m_IsLoading = false;
    m_FactoryUnitNo = -1;
    m_pTerrainFactory->UnLoadHighModel( m_X, m_Z );
  }
}

/**
* @brief 更新処理
*
* @param localAnimationFrame;      Localアニメの現在フレーム
* @param oneDayAnimationFrame;     1日アニメの現在フレーム
*
* @return 無し
*/
void TerrainBlock::Update( f32 localAnimationFrame, f32 oneDayAnimationFrame, const gfl2::math::Matrix44* pCullingMatrixTable )
{
  // LOD制御(モデルの変更リクエスト、Highモデルの生成・破棄)
  this->UpdateLodControl();

#if PM_DEBUG
  // エンカウント草の描画プライオリティのデバッグ変更
  {
    if( m_EncountGrassModelPriority != ENCOUNT_GRASS_MODEL_PRIORITY )
    {
      SetPriorityToEncountGrassModel( ENCOUNT_GRASS_MODEL_PRIORITY, false );
    }
  }
#endif

  // Visible制御
  {
#if PM_DEBUG
    // デバッグスイッチによる非表示
    b32 debugVisible = false;

    if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::TERRAIN ) )
    {
      switch( m_LodType )    // ここでみるのは本当のState
      {
      case Block::LodType::MIDDLE: debugVisible = FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::TERRAIN_LOD ); break;
      case Block::LodType::LOW: debugVisible = FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::TERRAIN_LOD ); break;
      case Block::LodType::HIGH: debugVisible = FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::TERRAIN_HIGH ); break;
      }
    }

    if( debugVisible )
    {
      m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_DEBUG_DRAW_SWITCH) );
      m_InvisibleBitForEncountGrass = ( m_InvisibleBitForEncountGrass & (~INVISIBLE_DEBUG_DRAW_SWITCH) );
    }
    else
    {
      m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_DEBUG_DRAW_SWITCH );
      m_InvisibleBitForEncountGrass = ( m_InvisibleBitForEncountGrass | INVISIBLE_DEBUG_DRAW_SWITCH );
    }
#endif

    // 視錐台カリングによる非表示 ( 関数化 )
    b32 cullingUpdateEnable = true;
    if( pCullingMatrixTable == NULL || m_pModelTable[ m_LodType ] == NULL ) // GetCullingAABBと同期する必要あり
    {
      cullingUpdateEnable = false;
    }

#if PM_DEBUG
    if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_TERRAIN_CULLING ) == 0 )
    {
      cullingUpdateEnable = false;
    }
#endif

    if( cullingUpdateEnable )
    {
      gfl2::math::AABB aabb;
      GetCullingAABB( &aabb );

      // ＡＡＢＢをカリング判定する。
      if ( gfl2::math::AABB::Util::IsIntersectFrustum( pCullingMatrixTable, aabb ) )
      {
        // 内側
        m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_CULLING) );
      }
      else
      {
        // 外側
        m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_CULLING );
      }
    }
    else
    {
      m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_CULLING) );
    }

    // 表示するべきモデルがない場合はカリング扱いにする
    if( m_pModelTable[ m_LodType ] == NULL )
    {
      m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_CULLING );
    }

    // Visible更新実行
    if( m_LodType == Block::LodType::MIDDLE )
    {
      UpdateVisibleImpl( m_LodType, !m_InvisibleBit );
    }
    else
    {
      UpdateVisibleImpl( m_LodType, !( m_InvisibleBit & (~INVISIBLE_MIDDLE_DISTANCE) ) );
    }

    // エンカウント草のVisible更新実行
    UpdateEncountGrassVisible( pCullingMatrixTable );
  }


  // モデル更新処理 
  if( m_pModelTable[ m_LodType ] && m_InvisibleBit == false )
  {
    m_pModelTable[ m_LodType ]->SetAnimationFrame( localAnimationFrame, Block::AnimationSlotNo::LOCAL );
    m_pModelTable[ m_LodType ]->SetAnimationFrame( oneDayAnimationFrame, Block::AnimationSlotNo::ONE_DAY );
    m_pModelTable[ m_LodType ]->UpdateAnimation();
  }

  // @note niji仕様、エンカウント草はアニメ不要

}

/**
* @brief フィールド開始用更新処理
*
* @return true Setup継続 false 完了
*/
b32 TerrainBlock::UpdateStartSetup( void )
{
  // LOD制御(モデルの変更リクエスト、Highモデルの生成・破棄)
  this->UpdateLodControl();

  return ( m_IsLoading );
}

/**
* @brief フィールド終了用更新処理
*
* @return true Setup継続 false 完了
*/
b32 TerrainBlock::UpdateEndSetup( void )
{
  // 強制middle
  m_RequestLodType = Block::LodType::MIDDLE;

  // LOD制御(モデルの変更リクエスト、Highモデルの生成・破棄)
  this->UpdateLodControl();

  return ( m_IsLoading );  // 読み込みがとまっていればOK
}

/**
* @brief カメラコリジョン前更新処理
*/
void TerrainBlock::UpdateCameraCollisonPrev( void )
{
  if( m_pStaticActorListForCameraEye == NULL )
  {
    return;
  }

  Field::DynamicList<StaticActor*>::Iterator it = m_pStaticActorListForCameraEye->Begin();
  Field::DynamicList<StaticActor*>::Iterator end = m_pStaticActorListForCameraEye->End();

  for( ; it != end; it++ )
  {
    Field::Camera::CameraHitObject* pHitObject = static_cast<Field::Camera::CameraHitObject*>( (*it)->GetUserData() );
    pHitObject->PreUpdate();
  }
}

/**
* @brief カメラコリジョン後更新処理
*/
void TerrainBlock::UpdateCameraCollisonAfter( void )
{
  if( m_pStaticActorListForCameraEye == NULL )
  {
    return;
  }

  Field::DynamicList<StaticActor*>::Iterator it = m_pStaticActorListForCameraEye->Begin();
  Field::DynamicList<StaticActor*>::Iterator end = m_pStaticActorListForCameraEye->End();

  for( ; it != end; it++ )
  {
    Field::Camera::CameraHitObject* pHitObject = static_cast<Field::Camera::CameraHitObject*>( (*it)->GetUserData() );
    pHitObject->Update();
  }
}

/**
* @brief LODが変更中か取得
*
* @return true 変更中 false 変更中ではない
*/
b32 TerrainBlock::IsLodChanging( void ) const
{
  return ( m_RequestLodType != m_LodType || m_IsLoading ); // @note  m_RequestLodType == m_LodType == Middleで、m_IsLoadingがtrueなことがある
}

/**
* @brief LOD変更処理
*
* @param  lodLevel 変更したい状態
*
* @return 無し
*/
void TerrainBlock::ChangeLod( Block::LodType::Value lodLevel )
{
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
* @param  pListHeap  リスト追加に使用するヒープ( ※最大登録すう分のヒープサイズしか確保されていません、その他の用途には使用しないでください)
*
* @return 成否
*/
b32 TerrainBlock::RegistListener( Block::LodChangeStaticListener* pListener, gfl2::heap::HeapBase* pHeap )
{
  if( pListener == NULL || m_pStaticLodListenerList == NULL )
  {
    return false;
  }

  // リスナーをリストに追加
  m_pStaticLodListenerList->PushBack( pListener, pHeap );
  
  // リスナーに登録時の情報を通知
  pListener->registBlock( m_X, m_Z );
  pListener->RegistAfter( m_LodType );

  return true;
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
  if( pListener == NULL || m_pStaticLodListenerList == NULL )
  {
    return false;
  }

  // @note [m_pStaticLodListenerList->Size()]の検索が走る
  if( m_pStaticLodListenerList->Remove( pListener ) != 0 )
  {
    pListener->unRegistBlock();
    return true;
  }
  else
  {
    return false;
  }
}

/**
 *  @brief  草モデルを取得する。
 *  @param  index   何段目の草モデルを取得するか。
 *
 *  @retval NULL  なし
 *  @retval ptr   草モデル
 */
KusaModel::FieldKusaModel* TerrainBlock::GetKusaModel( u32 index )
{
  if( m_ppEncountGrassModelTable[ Block::LodType::HIGH ] )
  {
    if(m_pTerrainFactory->GetEncountGrassModelTableNum() > index)
    {
      return m_ppEncountGrassModelTable[ Block::LodType::HIGH ][ index ];
    }
  }

  return NULL;
}

/**
*  @brief 現在モデルが表示されているか取得する
*/
b32 TerrainBlock::IsVisible( void ) const
{
  if( m_pModelTable[ m_LodType ] == NULL )
  {
    return false;
  }

  return m_pModelTable[ m_LodType ]->IsVisible();
}

/**
*  @brief 現在モデルのDrawTag数を取得
*/
s32 TerrainBlock::GetDrawTagNum( void ) const
{
  if( m_pModelTable[ m_LodType ] == NULL )
  {
    return 0;
  }

  return m_pModelTable[ m_LodType ]->GetModelInstanceNode()->GetDrawTagNum();
}

/**
*  @brief 距離によるmiddleモデルの非表示設定
*/
void TerrainBlock::SetMiddleModelInvisibleForDistance( b32 invisible )
{
  if( invisible )
  {
    m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_MIDDLE_DISTANCE);
  }
  else
  {
    m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_MIDDLE_DISTANCE) );
  }
}

/**
* @brief カメラ接触でメッシュの非表示にする機能を有効化する
*
* @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
*/
void TerrainBlock::HideMeshSystemOn( b32 oneTime )
{
  m_IsHideMeshSystemOff = false;

  if( m_pStaticActorListForCameraEye == NULL )
  {
    return;
  }

  Field::DynamicList<StaticActor*>::Iterator it = m_pStaticActorListForCameraEye->Begin();
  Field::DynamicList<StaticActor*>::Iterator end = m_pStaticActorListForCameraEye->End();

  for( ; it != end; it++ )
  {
    Field::Camera::CameraHitObject* pHitObject = static_cast<Field::Camera::CameraHitObject*>( (*it)->GetUserData() );
    pHitObject->HideOn( oneTime );
  }
}

/**
* @brief カメラ接触でメッシュの非表示にする機能を無効化する
*
* @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
*/
void TerrainBlock::HideMeshSystemOff( b32 oneTime )
{
  m_IsHideMeshSystemOff = true;
  
  if( m_pStaticActorListForCameraEye == NULL )
  {
    return;
  }

  Field::DynamicList<StaticActor*>::Iterator it = m_pStaticActorListForCameraEye->Begin();
  Field::DynamicList<StaticActor*>::Iterator end = m_pStaticActorListForCameraEye->End();

  for( ; it != end; it++ )
  {
    Field::Camera::CameraHitObject* pHitObject = static_cast<Field::Camera::CameraHitObject*>( (*it)->GetUserData() );
    pHitObject->HideOff( oneTime );
  }
}

#if PM_DEBUG
void TerrainBlock::SetDebugInvisibleBit( u32 bitMask, b32 flag )
{
  if( flag )
  {
    m_InvisibleBit = ( m_InvisibleBit | bitMask);
    m_InvisibleBitForEncountGrass = ( m_InvisibleBitForEncountGrass | bitMask);
  }
  else
  {
    m_InvisibleBit = ( m_InvisibleBit & (~bitMask) );
    m_InvisibleBitForEncountGrass = ( m_InvisibleBitForEncountGrass | bitMask);
  }
}

u32* TerrainBlock::GetEncountGrassModelPriorityDefaultSettingPtr( void )
{
  return &ENCOUNT_GRASS_MODEL_PRIORITY;
}

#endif // PM_DEBUG


//
// private
//

/**
*  @brief  LOD制御更新
*
*  @return 無し
*/
void TerrainBlock::UpdateLodControl( void )
{
  m_LodTypePrev = m_LodType;

  if( m_pModelTable[ Block::LodType::HIGH ] && m_LodType != Block::LodType::HIGH )
  {
    // 1フレーム遅れ確認をし、highモデルを開放する
    DeleteHighModel( false );
  }

  // highモデルロード中の処理
  if( m_IsLoading )
  {
    // 完了チェック
    if( m_pTerrainFactory->CheckHighModelUnitState( m_X, m_Z ) == TerrainFactory::UnitState::LOAD_COMPLETION )
    {
#if PM_DEBUG
      if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_DUMP_BLOCK_INFO ) )
      {  
        m_pTerrainFactory->DebugDumpBlockBinSize( m_X, m_Z );
      }
#endif 

      //
      // highブロックに紐付くオブジェクトを生成
      //
      
      // ブロック生成で使ってよい動的ヒープを生成工場から借りる
      gfl2::heap::HeapBase* pUnitDeviceHeap = m_pTerrainFactory->GetUnitDeviceHeap( m_FactoryUnitNo );
      System::nijiAllocator* pUnitAllocator = m_pTerrainFactory->GetUnitAllocator( m_FactoryUnitNo );

      if( m_pModelTable[ m_LodType ] )
      {
        UpdateVisibleImpl( m_LodType, false );
        SetEncountGrassVisible( m_LodType, false );
      }
      m_pModelTable[ Block::LodType::HIGH ] = m_pTerrainFactory->CreateHighModel( m_X, m_Z );
      m_pModelTable[ Block::LodType::HIGH ]->SetPosition( m_Pos );
      m_ppEncountGrassModelTable[ Block::LodType::HIGH ] = m_pTerrainFactory->CreateEncountGrassModelTable( m_X, m_Z );
      SetPriorityToEncountGrassModel( ENCOUNT_GRASS_MODEL_PRIORITY, true );

      m_pCollisionModel = m_pTerrainFactory->CreateHeightCollisionModel( m_X, m_Z );
      m_LodType = Block::LodType::HIGH;
      this->ChangeLodNotificationListener( m_LodType );
      m_IsLoading = false;

      // 高さコリジョン生成・登録
      {
        // 内こともあるかも
        if( m_pCollisionModel )
        {
          m_pStaticActor = m_pCollisionScene->CreateStaticActor( pUnitAllocator, pUnitDeviceHeap );
          m_pStaticActor->CreateMesh( gfl2::math::Vector3(0.0f,0.0f,0.0f), m_pCollisionModel );
        }

        if( m_pStaticActor )
        {
          m_pCollisionScene->RegistStaticActor( m_pStaticActor );
        }
      }

      // 壁コリジョン生成・登録
      {
        gfl2::collision::CollisionModel* workTable[ WallBinIndex::MAX ] = { NULL };
        if( m_pTerrainFactory->CreateWallCollisionModel( m_X, m_Z, workTable ) )
        {
          // コリジョンモデルを記憶
          for( u32 i = 0; i < WallBinIndex::MAX; i++ )
          {
            m_ppWallCollisionModelTable[i] = workTable[ i ];
          }

          // アクター生成とシーン登録
          for( u32 i = 0; i < WallIndex::MAX; i++ )
          {
            // 対応するリソースを取得
            WallBinIndex::Value colModelDataindex = sc_WallIndexToBinIndex[ i ];

            // リソース or 登録シーンが無い
            if( m_ppWallCollisionModelTable[ colModelDataindex ] == NULL || m_ppWallCollisionSceneTable[i] == NULL )
            {
              continue;
            }

            // 対応するリソースがあったので、アクターの生成とシーン登録を実行
            gfl2::collision::CollisionModel* pCollisionModel = m_ppWallCollisionModelTable[ colModelDataindex ];
            BaseCollisionScene* pScene = m_ppWallCollisionSceneTable[ i ];
            StaticActor* pActor = pScene->CreateStaticActor( pUnitAllocator, pUnitDeviceHeap );

            pActor->CreateMesh( gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), pCollisionModel );
            pActor->CreateCookingData();
            pScene->RegistStaticActor( pActor );

            m_ppWallStaticActorTable[ i ] = pActor;
          }
        }

        // 

      } 

      // カメラコリジョンで消すメッシュのアクター生成とシーン登録
      {
        m_pStaticActorListForCameraEye = GFL_NEW( pUnitDeviceHeap ) Field::DynamicList<StaticActor*>( pUnitDeviceHeap );
        CreateAndResistCollisionCameraEye( pUnitAllocator, pUnitDeviceHeap, m_pModelTable[ Block::LodType::HIGH ], m_pStaticActorListForCameraEye );


#if PM_DEBUG
        if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::TERRAIN_DATA_CHECK ) )
        {
          // コリジョン透過メッシュのデータチェック
          DebugDataCheckCollisionMesh( m_pModelTable[ Block::LodType::HIGH ] );
        }
#endif
      }

      // エンカウント草のカリング制御をセットアップ
      {
        SetupEncountGrassCullingInfo();
      }

#if PM_DEBUG
      if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_DUMP_BLOCK_INFO ) )
      {
        m_pTerrainFactory->DebugDumpBlockInstanceSize( m_X, m_Z );
      }

      // スクリプト解析用Dump
      if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_DUMP_BLOCK_INFO_FOR_SCRIPT ) )
      {
        m_pTerrainFactory->DebugDumpBlockForScript( m_X, m_Z );
      }
#endif 

#if FIELD_CTR_PROFILING_MAPBLOCKCHANGE
      FIELD_CTR_PROFILING_END
#endif
    } // 完了チェック
    else
    {
      return;
    }
  } //   if( m_IsLoading )

  // 見た目切り替え処理
  if( m_RequestLodType != m_LodType )
  {
    // highモデルはロードが入るので特殊
    if( m_RequestLodType == Block::LodType::HIGH )
    {
      // ロードリクエストを投げる、Highモデルは同時に存在できる数が限られている、他のブロックが空くまで待つ
      m_FactoryUnitNo = m_pTerrainFactory->LoadHighModelAsync( m_X, m_Z, m_ReplaceNo );
      if( m_FactoryUnitNo != -1 )
      {
        m_IsLoading = true;

#if FIELD_CTR_PROFILING_MAPBLOCKCHANGE
        FIELD_CTR_PROFILING_START
#endif
      }
    }
    else
    {
      if( m_pModelTable[ m_LodType ] )
      {
        UpdateVisibleImpl( m_LodType, false );
        SetEncountGrassVisible( m_LodType, false );
      }

      m_LodType = m_RequestLodType;
      this->ChangeLodNotificationListener( m_LodType );
    }
  }
}

/**
* @brief LOD変更をリスナーに通知
*
* @param  lodType 変更後の状態
*
* @return 無し
*/
void TerrainBlock::ChangeLodNotificationListener( Block::LodType::Value lodType )
{
  if( m_pStaticLodListenerList == NULL )
  {
    return;
  }

  StaticLodListenerListIterator it( m_pStaticLodListenerList->Begin() );
  StaticLodListenerListIterator end( m_pStaticLodListenerList->End() );
  while( it != end )
  {
    (*it)->ChangeLod( lodType );
    it++;
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
}

/**
* @brief カリングに使用するAABBを取得する
*
* @param pOut 格納先
*
* @return 無し
*/
b32 TerrainBlock::GetCullingAABB( gfl2::math::AABB* pOut ) const
{
  if( m_pModelTable[ m_LodType ] == NULL )
  {
    return false;
  }

  // 地形モデルのAABB
  *pOut = m_pModelTable[ m_LodType ]->GetModelInstanceNode()->GetWorldAABB();
  return true;
}

/**
* @brief highモデル削除
* @note  1フレーム遅れになってない場合は削除しません
*
* @param force  trueのとき、1フレーム遅れになってない場合も削除
*
* @return true 削除された or 削除済み
*/
b32 TerrainBlock::DeleteHighModel( b32 force )
{
  if( m_pModelTable[ Block::LodType::HIGH ] == NULL )
  {
    return true;
  }

  if( force )
  {
    if( m_pModelTable[ Block::LodType::HIGH ] )
    {
      m_pModelTable[ Block::LodType::HIGH ]->SetDestroyCheckEnable( false );
    }

    if( m_ppEncountGrassModelTable[ Block::LodType::HIGH ] )
    {
      for( u32 i=0; i < m_pTerrainFactory->GetEncountGrassModelTableNum(); ++i )
      {
        if( m_ppEncountGrassModelTable[Block::LodType::HIGH][i] )
        {
          m_ppEncountGrassModelTable[Block::LodType::HIGH][i]->SetDrawCheckListener( false );
        }
      }
    }
  }
  else
  {
    // 開放できるかチェック
    if( m_pModelTable[ Block::LodType::HIGH ] && m_pModelTable[ Block::LodType::HIGH ]->CanDestroy() == false )
    {
      return false;
    }

    if( m_ppEncountGrassModelTable[ Block::LodType::HIGH ] )
    {
      for( u32 i=0; i < m_pTerrainFactory->GetEncountGrassModelTableNum(); ++i )
      {
        if( m_ppEncountGrassModelTable[Block::LodType::HIGH][i] && m_ppEncountGrassModelTable[Block::LodType::HIGH][i]->GetReferenceCnt() != 0 )
        {
          return false;
        }
      }
    }
  }

  //
  // 破棄実行
  //
 
  if( m_pStaticActorListForCameraEye )
  {
    DeleteAndUnResistCollisionCameraEye( m_pStaticActorListForCameraEye ); 
    GFL_SAFE_DELETE( m_pStaticActorListForCameraEye );
  }

  m_pTerrainFactory->DeleteModel( &m_pModelTable[ Block::LodType::HIGH ] );

  // エンカウント草の破棄
  {
    if( m_ppEncountGrassModelTable[ Block::LodType::HIGH ] )
    {
      m_pTerrainFactory->DeleteEncountGrassModelTable( &m_ppEncountGrassModelTable[ Block::LodType::HIGH ] );
    }
  }

  // 壁コリジョン登録解除・破棄
  if( m_ppWallStaticActorTable )
  {
    for( u32 i = 0; i < WallIndex::MAX; i++ )
    {
      if( m_ppWallStaticActorTable[ i ] )
      {
        if( m_ppWallCollisionSceneTable && m_ppWallCollisionSceneTable[ i ] )
        {
          m_ppWallCollisionSceneTable[ i ]->UnRegistStaticActor( m_ppWallStaticActorTable[i] );
        }
        GFL_SAFE_DELETE( m_ppWallStaticActorTable[i] );
      }
    }
  }

  if( m_ppWallCollisionModelTable )
  {
    for( u32 i = 0; i < WallBinIndex::MAX; i++ )
    {
      if( m_ppWallCollisionModelTable[ i ] )
      {
        GFL_SAFE_DELETE( m_ppWallCollisionModelTable[i] );
      }
    }
  }

  // 高さコリジョン登録解除・破棄
  if( m_pStaticActor )
  {
    m_pCollisionScene->UnRegistStaticActor( m_pStaticActor );
    GFL_SAFE_DELETE( m_pStaticActor );
  }
  GFL_SAFE_DELETE( m_pCollisionModel );

  // リソース破棄
  m_pTerrainFactory->UnLoadHighModel( m_X, m_Z );
  m_FactoryUnitNo = -1;
  return true;
}

/**
* @brief 指定モデルのカメラコリジョン(非表示メッシュ)アクターを生成し、シーンに登録する
*
* @param pAllocator       アクター生成に使うアロケーター
* @param pAllocator       アクター生成と生成したアクターのリスト管理に使用するメモリ
* @param pBaseModel       対象モデル
* @param pStaticActorList 生成したアクターを保持する動的リスト
*
* @return 無し
*/
void TerrainBlock::CreateAndResistCollisionCameraEye( System::nijiAllocator* pAllocator, gfl2::heap::HeapBase* pHeap, poke_3d::model::BaseModel* pBaseModel, Field::DynamicList<StaticActor*>* pStaticActorList )
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = pBaseModel->GetModelInstanceNode();
  u32 meshNum = pModelInstanceNode->GetMeshNum();
  for( u32 i = 0; i < meshNum; i++ )
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLink = pModelInstanceNode->GetMeshLinkData( i );
    CameraCollisionMeshType::Value meshType = CheckCameraEyeCollisionTarget( pMeshLink );
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pAlphaAfter = NULL;
    b32 isAlphaMode = false;

    // @fix GFMcat[1963]:カメラ接触非表示オブジェクトの表示がおかしくなるときがある、関連不具合。アルファ透過後のメッシュをMode::VISIBLEとして多重登録していたので、無視するように修正
    switch( meshType )
    {
    case CameraCollisionMeshType::ALPHA_AFTER:
    case CameraCollisionMeshType::NOT_SUBJECT:
      {
        continue;
      }

    case CameraCollisionMeshType::ALPHA_BASE:
      {
        // 相方を検索
        s32 afterIndex = SearchCameraEyeCollisionTargetAlphaAfter( pMeshLink, pModelInstanceNode );
        if( afterIndex == -1 )
        {
          GFL_ASSERT( 0 ); // 相方が見当たらない
          continue;
        }

        pAlphaAfter = pModelInstanceNode->GetMeshLinkData( afterIndex );
        isAlphaMode = true;
      }
    }

    // アクター生成
    StaticActor* pActor = m_pCameraEyeCollsionScene->CreateStaticActor( pAllocator, pHeap );

    gfl2::math::AABB aabb = pMeshLink->GetAABB();
    gfl2::math::Vector4 diff = aabb.GetDifference();

    // 形状を設定
    pActor->CreateObb(
      gfl2::math::Vector3( aabb.GetCenter().x, aabb.GetCenter().y, aabb.GetCenter().z ),
      gfl2::math::Vector3( 1.0f, 0.0f, 0.0f ),
      gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ),
      gfl2::math::Vector3( 0.0f, 0.0f, 1.0f ),
      diff.x * 0.5f,
      diff.y * 0.5f,
      diff.z * 0.5f
      );

    // メッシュを記憶
    Field::Camera::CameraHitObject* pHitObject = GFL_NEW( pHeap ) Field::Camera::CameraHitObject();
    Field::Camera::CameraHitObject::InitializeDescription desc;
    desc.m_pHeap = pHeap;
    desc.m_pMeshLink = pMeshLink;
    desc.m_pModelInstanceNode = pModelInstanceNode;

    if( isAlphaMode )
    {
      desc.m_Mode = Field::Camera::CameraHitObject::Mode::ALPHA;
      desc.m_pMeshLinkAfter = pAlphaAfter;
    }
    else
    {
      desc.m_Mode = Field::Camera::CameraHitObject::Mode::VISIBLE;
    }

    pHitObject->Initialize( desc );
    // @fix GFMcat[4831]イベント中に透過メッシュが不要に消える、対処。透過機能をOFFする機能を追加。生成時にOFFの場合は生成直後に通知
    if( m_IsHideMeshSystemOff )
    {
      pHitObject->HideOff( true );
    }

    pActor->SetUserData( pHitObject );

    // シーンに追加
    m_pCameraEyeCollsionScene->RegistStaticActor( pActor );

    // 記憶
    pStaticActorList->PushBack( pActor, pHeap );

    // GFL_PRINT("mesh name = %s \n", pMeshLink->GetName() );
  }
}

/**
* @brief 指定のリストで管理されているカメラコリジョン(非表示メッシュ)アクターを登録解除し、破棄する
*
* @param pStaticActorList 指定リスト
*
* @return 無し
*/
void TerrainBlock::DeleteAndUnResistCollisionCameraEye( Field::DynamicList<StaticActor*>* pStaticActorList )
{
  Field::DynamicList<StaticActor*>::Iterator it = pStaticActorList->Begin();
  Field::DynamicList<StaticActor*>::Iterator end = pStaticActorList->End();

  while( it != end )
  {
    // シーンから破棄
    m_pCameraEyeCollsionScene->UnRegistStaticActor( *it );

    Field::Camera::CameraHitObject* pHitObject = static_cast<Field::Camera::CameraHitObject*>( (*it)->GetUserData() );
    GFL_SAFE_DELETE( pHitObject );
    (*it)->SetUserData( NULL );

    // アクター削除
    GFL_SAFE_DELETE( *it );

    // リストから破棄
    it = pStaticActorList->Erase( it );
  }
}

/**
* @brief エンカウント草モデルの描画優先度を設定
*
* @param priority 描画の優先度(描画ソートで使用, 0〜3の値しか取らない)
* @param force    true 必ず更新
*
* @return 無し
*/
void TerrainBlock::SetPriorityToEncountGrassModel( u32 priority, b32 force )
{
  if( m_ppEncountGrassModelTable[ Block::LodType::HIGH ] == NULL )
  {
    return;
  }

  // 初回実行時まではm_EncountGrassModelPriorityが不正、最初は必ずforce

  if( force == false && m_EncountGrassModelPriority == priority )
  {
    return;
  }

  m_EncountGrassModelPriority = priority;

  for( u32 e=0; e < m_pTerrainFactory->GetEncountGrassModelTableNum(); ++e )
  {
    if( m_ppEncountGrassModelTable[Block::LodType::HIGH][e] == NULL )
    {
      continue;
    }

    // プライオリティを書き換え
    m_ppEncountGrassModelTable[Block::LodType::HIGH][e]->SetSortPriority( m_EncountGrassModelPriority );
  }
}

/**
* @brief エンカウント草モデルのカリング情報を初期化
*
* @return 無し
*/
void TerrainBlock::SetupEncountGrassCullingInfo( void )
{
  for( u32 i = 0; i < m_pTerrainFactory->GetEncountGrassModelTableNum(); i++ )
  {
    m_pEncountGrassCullingInfoTable[ i ].isCulling = false;
    m_pEncountGrassCullingInfoTable[ i ].isOverTerrainModel = false;

    if( m_ppEncountGrassModelTable[Block::LodType::HIGH][i] == NULL )
    {
      continue;
    }

    if( m_pModelTable[ Block::LodType::HIGH ] )
    {
      gfl2::math::AABB modelAABB = m_pModelTable[ Block::LodType::HIGH ]->GetModelInstanceNode()->GetWorldAABB();
      gfl2::math::AABB encAABB = m_ppEncountGrassModelTable[Block::LodType::HIGH][i]->GetWorldAABB();

      gfl2::math::Vector4 modelMin = modelAABB.GetMin();
      gfl2::math::Vector4 modelMax = modelAABB.GetMax();
      gfl2::math::Vector4 encMin = encAABB.GetMin();
      gfl2::math::Vector4 encMax = encAABB.GetMax();
      
      if(
        encMax.x <= modelMax.x &&
        encMin.x >= modelMin.x &&
        encMax.y <= modelMax.y &&
        encMin.y >= modelMin.y &&
        encMax.z <= modelMax.z &&
        encMin.z >= modelMin.z 
        )
      {
        // 見た目モデルにないほうされている
        m_pEncountGrassCullingInfoTable[ i ].isOverTerrainModel = false;
      }
      else
      {
        // 見た目モデルからはみ出ている
        m_pEncountGrassCullingInfoTable[ i ].isOverTerrainModel = true;
      }
    }
    else
    {
      // 見た目モデルがない場合ははみ出ている扱いにしておく。現在はHighモデル生成後にしか実行されないので、ありえないが念のため
      m_pEncountGrassCullingInfoTable[ i ].isOverTerrainModel = true;
    }
  }
}

/**
* @brief エンカウント草モデルの表示を更新
*
* @return 無し
*/
void TerrainBlock::UpdateEncountGrassVisible( const gfl2::math::Matrix44* pCullingMatrixTable )
{
  if( m_LodType != Block::LodType::HIGH )
  {
    return;
  }

  for( u32 i = 0; i < m_pTerrainFactory->GetEncountGrassModelTableNum(); i++ )
  {
    if( m_ppEncountGrassModelTable[m_LodType][i] == NULL )
    {
      m_pEncountGrassCullingInfoTable[i].isCulling = true;
      continue;
    }

    // 地形に収まっていて、地形が表示OFFなら同時に消す
    if( !m_pEncountGrassCullingInfoTable[i].isOverTerrainModel && ( m_InvisibleBit & (~INVISIBLE_MIDDLE_DISTANCE)) )
    {
      m_pEncountGrassCullingInfoTable[i].isCulling = true;
    }
    else
    {
      // 自身のAABBで比較
      gfl2::math::AABB aabb = m_ppEncountGrassModelTable[m_LodType][i]->GetAABB();
      if( gfl2::math::AABB::Util::IsIntersectFrustum( pCullingMatrixTable, aabb ) )
      {
        // 内側
        m_pEncountGrassCullingInfoTable[i].isCulling = false;
      }
      else
      {
        // 外側
        m_pEncountGrassCullingInfoTable[i].isCulling = true;
      }
    }

    // @note エンカウント草は複数あるので、bit側のカリングは未使用

    // モデルに適用
    b32 isVisible = !(m_pEncountGrassCullingInfoTable[i].isCulling || m_InvisibleBitForEncountGrass );
    if( m_ppEncountGrassModelTable[m_LodType][i]->IsVisible() != isVisible )
    {
      m_ppEncountGrassModelTable[m_LodType][i]->SetVisible( isVisible );
    }
  }
}

/**
* @brief エンカウント草モデルの表示を設定
*
* @param lodType  対象となるLODモデル
* @param visible  true 表示 false 非表示
*
* @return 無し
*/
void TerrainBlock::SetEncountGrassVisible( Block::LodType::Value lodType, b32 visible )
{
  for( u32 i=0; i<m_pTerrainFactory->GetEncountGrassModelTableNum(); ++i )
  {
    if( m_ppEncountGrassModelTable[ lodType ] && m_ppEncountGrassModelTable[ lodType ][i])
    {
      if( m_ppEncountGrassModelTable[ lodType ][i]->IsVisible() != visible )
      {
        m_ppEncountGrassModelTable[ lodType ][i]->SetVisible( visible ); 
      }
    }
  }
}

#if PM_DEBUG

//
// データチェック
// 

// コリジョン透過メッシュに不備があればアサートを出す
void TerrainBlock::DebugDataCheckCollisionMesh( poke_3d::model::BaseModel* pBaseModel )
{
  //
  // チェック1 ： 透過後のメッシュが使用しているマテリアルが他のメッシュに使われていないか？
  //

  // @note GFNMcat[1963]カメラ接触非表示オブジェクトの表示がおかしくなるときがある、洗い出し機能
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = pBaseModel->GetModelInstanceNode();
  u32 meshNum = pModelInstanceNode->GetMeshNum();
  for( u32 i = 0; i < meshNum; i++ )
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLink = pModelInstanceNode->GetMeshLinkData( i );
    CameraCollisionMeshType::Value meshType = CheckCameraEyeCollisionTarget( pMeshLink, false );

    if( meshType != CameraCollisionMeshType::ALPHA_AFTER )
    {
      continue;
    }

    // 全マテリアルチェック
    for( u32 f = 0; f < pMeshLink->GetFaceNum(); f++ )
    {
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMat = pModelInstanceNode->GetMaterialInstanceNodeByNameHash( pMeshLink->GetFaceData( f )->m_MaterialNameHash );
      DebugDataCheckCollisionMeshMaterial( pBaseModel, pMeshLink, pMat );
    } // for face

  } // for mesh

}

// コリジョン透過メッシュのマテリアル重複チェック
void TerrainBlock::DebugDataCheckCollisionMeshMaterial( poke_3d::model::BaseModel* pBaseModel, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pSrcMesh, gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pSrcMat )
{
  // 指定マテリアルが指定メッシュ以外で使われていたらアサート
  // その場合、大量に出る可能性があるため1回度アサートがでたら打ち切り

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = pBaseModel->GetModelInstanceNode();
  u32 meshNum = pModelInstanceNode->GetMeshNum();

  for( u32 targetIndex = 0; targetIndex < meshNum; targetIndex++ )
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pTargetMeshLink = pModelInstanceNode->GetMeshLinkData( targetIndex );
    if( pSrcMesh->GetNameHash() == pTargetMeshLink->GetNameHash() )
    {
      continue;
    }

    for( u32 targetFace = 0; targetFace < pTargetMeshLink->GetFaceNum(); targetFace++ )
    {
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pTargetMat = pModelInstanceNode->GetMaterialInstanceNodeByNameHash( pTargetMeshLink->GetFaceData( targetFace )->m_MaterialNameHash );
      
      // 同名マテリアルが使用されている
      if( pSrcMat->GetNameHash() == pTargetMat->GetNameHash() )
      {
        GFL_ASSERT_MSG( 0, "透過メッシュデータNG：Model[%d,%d]-MeshName[%s]-MaterialName[%s]がMeshName[%s]から参照されています。マテリアルを切り出してください", 
          m_X, m_Z, pSrcMesh->GetName(), pSrcMat->GetName(), pTargetMeshLink->GetName() );

        // 打ち切り
        return;
      } 

    }// for targetFace
  } // for targetMesh
}

#endif

GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );