//======================================================================
/**
 * @file FieldStaticModelActorBase.cpp
 * @date 2015/08/19 15:51:36
 * @author saita_kazuki
 * @brief 配置モデルアクター基底クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/PlacementData/actor/FieldStaticModelActorBase.h"
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModelDefine.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

#include "GameSys/include/GameManager.h"
#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

GFL_NAMESPACE_BEGIN(Field)

/**
 *  @brief  コンストラクタ
 */
FieldStaticModelActorBase::FieldStaticModelActorBase( void )
  : IFieldActorBase()
  , m_pStaticModel( NULL )
  , m_pStaticModelManager( NULL )
  , m_pTerrainManager( NULL )
  , m_pRenderingPipeLine( NULL )
  , m_bIsCulling( true )
  , m_LODChangeListener()
  , m_bIsLODChangeRegist( false )
{
}

/**
 *  @brief  デストラクタ
 */
FieldStaticModelActorBase::~FieldStaticModelActorBase( void )
{
}

/**
 *  @brief  1フレ遅れ解放リクエスト
 */
void FieldStaticModelActorBase::RequestDelayTerminate( void )
{
  this->UnregistActor();

  // ☆配置モデル破棄リクエスト
  m_pStaticModel->RequestDelayTerminate( StaticModel::MODEL_TYPE_ALL);
}

/**
 *  @brief  完全破棄
 */
void FieldStaticModelActorBase::Terminate( void )
{
  if( terminate() == false)
  {
    return;
  }

  IFieldActorBase::Terminate();
}

/**
 *  @brief  追加
 */
void FieldStaticModelActorBase::RegistActor( void )
{
  m_bIsRegist = true;

  // registActorだけではm_bIsCullingがfalseにならず、
  // UnRegistする必要がないと誤認識されるのでoffCullingを呼ぶ
  offCulling();

  registLODChangeListener();
  // 地形カリングは適用しない
  //registCullingListener();
}

/**
 *  @brief  削除
 */
void FieldStaticModelActorBase::UnregistActor( void )
{
  // 地形カリングは適用しない
  //unregistCullingListener();
  unregistLODChangeListener();

  // unregistActorだけではm_bIsCullingがtrueに初期化されないので、
  // onCullingを呼ぶように修正した。
  onCulling();

  m_bIsRegist = false;
}

/**
 *  @brief  コリジョンデータ取得
 *  @param pOutColData コリジョンデータ格納先
 *  @param collisionAccessor コリジョンデータアクセサー
 *  @param index コリジョンデータ番号
 */
bool FieldStaticModelActorBase::getCollisionData(
  FieldCollisionAccessor::CollisionData* pOutColData,
  const FieldCollisionAccessor& collisionAccessor,
  u32 index
  )
{
  if( collisionAccessor.GetDataMax() > 0 && collisionAccessor.GetData( index, pOutColData ) )
  {
    return true;
  }
  return false;
}

/**
 *  @brief  コリジョンのセットアップ
 *  @param collisionData CollisionAccessorから取得したバイナリデータ
 *  @param parentPos アクターの位置
 *  @param pBaseActor コリジョン生成するActor
 *  @param pUserData Actorに登録するユーザー任意データ(NULLなら登録しない)
 */
void FieldStaticModelActorBase::setupCollision(
  const FieldCollisionAccessor::CollisionData& colData,
  gfl2::math::Vector3& parentPos,
  BaseActor* pCollisionActor,
  void* pUserData
  )
{
  if( pCollisionActor == NULL )
  {
    return;
  }

  FieldCollisionAccessor::CreateActor( pCollisionActor, colData, parentPos );

  if( pUserData )
  {
    pCollisionActor->SetUserData( pUserData );
  }
}

/**
 *  @brief  LOD通知リスナーのセットアップ
 */
b32 FieldStaticModelActorBase::registLODChangeListener( void )
{
  m_LODChangeListener.Setup( this);

  if( m_pTerrainManager == NULL )
  {
    return false;
  }

  m_bIsLODChangeRegist = m_pTerrainManager->RegistBlockListener( &m_LODChangeListener );
  GFL_ASSERT_MSG( m_bIsLODChangeRegist, "TerrainManager::RegistBlockLODListener() Failed.\n");

  return m_bIsLODChangeRegist;
}

/**
 *  @brief  LOD通知リスナーの登録解除
 */
void FieldStaticModelActorBase::unregistLODChangeListener( void )
{
  // LOD通知リスナーからの削除
  if( m_pTerrainManager && m_bIsLODChangeRegist )
  {
    m_pTerrainManager->UnRegistBlockListener( &m_LODChangeListener );
    m_bIsLODChangeRegist = false;
  }
}

/**
 *  @brief  破棄共通処理
 *  @retval true 破棄処理を行った
 *  @retval false 破棄処理を行っていない
 */
bool FieldStaticModelActorBase::terminate( void )
{
  m_pStaticModel->Terminate();

  m_pTerrainManager = NULL;
  m_pRenderingPipeLine = NULL;

  m_bIsCulling = true;

  m_bIsLODChangeRegist = false;

  return true;
}

/**
 *  @brief  カリング状態ON
 */
void FieldStaticModelActorBase::onCulling( void )
{
  unregistActor();
  m_bIsCulling = true;
}

/**
 *  @brief  カリング状態OFF
 */
void FieldStaticModelActorBase::offCulling( void )
{
  registActor();
  m_bIsCulling = false;
}

/**
 *  @brief  LOD変更可能かどうか
 *  @retval true 可能
 *  @retval true 不可能
 */
bool FieldStaticModelActorBase::isChangeLOD( void )
{
  // ユーザー指定のRegist:false または Culling:true なら変更しない
  if( !m_bIsRegist || m_bIsCulling )
  {
    return false;
  }
  return true;
}

/**
 *  @brief  LOD変更
 */
void FieldStaticModelActorBase::changeLOD( Field::Terrain::Block::LodType::Value lodType )
{
  if( m_pStaticModel)
  {
    StaticModel::ModelType old  = m_pStaticModel->GetNowModelType();
    StaticModel::ModelType next = this->getModelTypeByTerrainLodType( lodType );

    switch( m_pStaticModel->GetLODType())
    {
    case StaticModel::LOD_TYPE_VISIBLE_OFF:
      this->changeLOD_VisibleOff( old, next);
      break;

    case StaticModel::LOD_TYPE_VISIBLE_KEEP:
      this->changeLOD_VisibleKeep( old, next);
      break;

    default:
      GFL_ASSERT( 0);
      break;
    }
  }
}

/**
 *  @brief  LOD変更 ミドル以下は表示しない
 *  @param changeLODからしか呼ばれない想定
 *  @retval true モデルタイプを変更した
 *  @retval false モデルタイプを変更していない
 */
bool FieldStaticModelActorBase::changeLOD_VisibleOff( StaticModel::ModelType old, StaticModel::ModelType next)
{
  GFL_ASSERT_STOP( m_pStaticModel); // もし他で使用された場合に開発中に気付かせるため

  bool isChangeLOD = false;
  isChangeLOD = m_pStaticModel->ChangeLOD_VisibleOff( next);

  // タイプが同じで変更が必要がないときはfalseが返ってくる
  if( isChangeLOD == false)
  {
    return false;
  }

  // 変更後がハイモデル( = 前回がなし )なら、描画パイプライン、コリジョンシーンに登録する
  if( next == StaticModel::MODEL_TYPE_HIGH )
  {
    this->registRenderPipeLine( StaticModel::MODEL_TYPE_HIGH);
    this->registCollisionScene();
  }
  // 変更前がハイモデル( = 今はなし )なら、描画パイプライン、コリジョンシーンから外す
  else if( old == StaticModel::MODEL_TYPE_HIGH)
  {
    this->unregistRenderPipeLine( StaticModel::MODEL_TYPE_HIGH);
    this->unregistCollisionScene();
  }

  return true;
}

/**
 *  @brief  LOD変更 ミドル以下でも表示維持。モデルを切り替えるかは管理表への登録次第
 *  @param changeLODからしか呼ばれない想定
 *  @retval true モデルタイプを変更した
 *  @retval false モデルタイプを変更していない
 */
bool FieldStaticModelActorBase::changeLOD_VisibleKeep( StaticModel::ModelType old, StaticModel::ModelType next)
{
  GFL_ASSERT_STOP( m_pStaticModel); // もし他で使用された場合に開発中に気付かせるため

  bool isChange = false;
  isChange = m_pStaticModel->ChangeLOD( next);

  // 渡したLODタイプが同じで変更の必要がなかった場合、
  // モデルが登録されていない場合、はfalseが返ってくる
  if( isChange == false)
  {
    return false;
  }

  this->unregistRenderPipeLine( old);
  this->registRenderPipeLine( next);

  // 変更前がハイモデル( = 今はミドルかローモデル )ならコリジョンシーンからunregist
  if( old == StaticModel::MODEL_TYPE_HIGH)
  {
    this->unregistCollisionScene();
  }
  // 変更前がミドルかローモデル( = 今はハイモデル )ならコリジョンシーンにregist
  else if( next == StaticModel::MODEL_TYPE_HIGH)
  {
    this->registCollisionScene();
  }

  return true;
}

/**
 *  @brief  アクターを登録する必要があるか
 *  @retval true できる
 *  @retval false できない
 */
bool FieldStaticModelActorBase::isNeedRegistActor( void )
{
  bool result = ( m_bIsRegist && m_bIsCulling );
#if PM_DEBUG
  // デバッグ用に上書き
  result = m_bIsRegist && ( m_bIsCulling || ( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_STATICMODEL ) != 0 ) );
#endif
  return result;
}

/**
 *  @brief  アクターを削除する必要があるか
 *  @retval true できる
 *  @retval false できない
 */
bool FieldStaticModelActorBase::isNeedUnRegistActor( void )
{
  bool result = ( m_bIsRegist && !m_bIsCulling );
#if PM_DEBUG
  result |= ( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_STATICMODEL ) == 0 ) && m_bIsRegist;
#endif
  return result;
}

/**
 *  @brief  コリジョンシーンへの登録
 */
void FieldStaticModelActorBase::registCollisionScene( void )
{
  if( m_pCollisionScene )
  {
    for( u32 i = 0; i < m_Size; ++ i )
    {
      if( !m_ppActor[ i ] ){ continue; }
      m_pCollisionScene->RegistStaticActor( m_ppActor[ i ] );
    }

    StaticActor* pActor = m_pStaticModel->GetStaticActor();
    if( pActor)
    {
      pActor->SetActive( true);
    }
  }
}

/**
 *  @brief  コリジョンシーンからの削除
 */
void FieldStaticModelActorBase::unregistCollisionScene( void )
{
  if( m_pCollisionScene )
  {
    for( u32 i = 0; i < m_Size; ++ i )
    {
      if( !m_ppActor[ i ] ){ continue; }
      if( m_ppActor[ i ]->GetUserData() != NULL )
      {
        m_pCollisionScene->UnRegistStaticActor( m_ppActor[ i ] );
      }
    }

    StaticActor* pActor = m_pStaticModel->GetStaticActor();
    if( pActor)
    {
      pActor->SetActive( false);
    }
  }
}

/**
 *  @brief  配置モデルの表示ON
 */
void FieldStaticModelActorBase::visibleStaticModel( void )
{
  if( m_pStaticModel )
  {
    u32 now = m_pStaticModel->GetNowModelType();
    poke_3d::model::BaseModel* pBaseModel = m_pStaticModel->GetBaseModel( now);
    if( pBaseModel)
    {
      m_pRenderingPipeLine->AddEdgeRenderingTarget( pBaseModel->GetModelInstanceNode() );
    }

    m_pStaticModel->SetUpdateEnable( true);
    m_pStaticModel->SetVisible( true );
  }
}

/**
 *  @brief  配置モデルの表示OFF
 */
void FieldStaticModelActorBase::invisibleStaticModel( void )
{
  if( m_pStaticModel )
  {
    m_pStaticModel->SetVisible( false );
    m_pStaticModel->SetUpdateEnable( false);

    u32 now = m_pStaticModel->GetNowModelType();
    poke_3d::model::BaseModel* pBaseModel = m_pStaticModel->GetBaseModel( now);
    if( pBaseModel)
    {
      m_pRenderingPipeLine->RemoveEdgeRenderingTarget( pBaseModel->GetModelInstanceNode() );
    }
  }
}

/**
 *  @brief  配置モデルの描画パイプラインへの登録
 */
void FieldStaticModelActorBase::registRenderPipeLine( u32 modelType )
{
  if( modelType == StaticModel::MODEL_TYPE_NONE)
  {
    return;
  }

  if( m_pStaticModel )
  {
    poke_3d::model::BaseModel* pBaseModel = m_pStaticModel->GetBaseModel( modelType);
    if( pBaseModel)
    {
      m_pRenderingPipeLine->AddEdgeRenderingTarget( pBaseModel->GetModelInstanceNode() );
    }
  }
}

/**
 *  @brief  配置モデルの描画パイプラインからの削除
 */
void FieldStaticModelActorBase::unregistRenderPipeLine( u32 modelType )
{
  if( modelType == StaticModel::MODEL_TYPE_NONE)
  {
    return;
  }

  if( m_pStaticModel )
  {
    poke_3d::model::BaseModel* pBaseModel = m_pStaticModel->GetBaseModel( modelType);
    if( pBaseModel)
    {
      m_pRenderingPipeLine->RemoveEdgeRenderingTarget( pBaseModel->GetModelInstanceNode() );
    }
  }
}

/**
 *  @brief  地形LODタイプから配置モデルのモデルタイプ取得
 */
StaticModel::ModelType FieldStaticModelActorBase::getModelTypeByTerrainLodType( Terrain::Block::LodType::Value lodType )
{
  switch( lodType )
  {
  case Terrain::Block::LodType::HIGH:
    return StaticModel::MODEL_TYPE_HIGH;

  case Terrain::Block::LodType::MIDDLE:
    return StaticModel::MODEL_TYPE_MIDDLE;

  case Terrain::Block::LodType::LOW:
    return StaticModel::MODEL_TYPE_LOW;

  default:
    GFL_ASSERT_STOP( 0 );
    return StaticModel::MODEL_TYPE_HIGH;
  }
}

//==========================================================================
//  LODChangeListener
//==========================================================================

/**
* @brief 地形ブロックに登録された瞬間に実行される関数
*
* @param  lodType 登録時の地形のLOD状態
*
* @return 無し
*/
void FieldStaticModelActorBase::LodChangeStaticListener::RegistAfter( Field::Terrain::Block::LodType::Value lodType )
{
  if( m_pActor->isChangeLOD())
  {
    m_pActor->changeLOD( lodType);
  }
}

/**
* @brief 地形ブロックのLODが切り替わった瞬間に実行される関数
*
* @param  lodType 切り替え後のLOD状態
*
* @return 無し
*/
void FieldStaticModelActorBase::LodChangeStaticListener::ChangeLod( Field::Terrain::Block::LodType::Value lodType )
{
  if( m_pActor->isChangeLOD())
  {
    m_pActor->changeLOD( lodType);
  }
}

/**
* @brief 所属する地形ブロックを決定する座標を取得する
*
* @param  pOut　座標格納先  
*
* @return 成否
*/
b32 FieldStaticModelActorBase::LodChangeStaticListener::GetControlPosition( gfl2::math::Vector3* pOut ) const
{
  StaticModel::StaticModel* pModel = m_pActor->m_pStaticModel;
  *pOut = pModel->GetLocalSRT().translate;
  return true;
}

/**
 *  @brief  監視するアクターを設定
 */
void FieldStaticModelActorBase::LodChangeStaticListener::Setup( FieldStaticModelActorBase * pActor )
{
  m_pActor = pActor;
}

#if PM_DEBUG
/**
 *  @brief  デバッグ用：描画切替
 */
void FieldStaticModelActorBase::DebugFunc_ChangeVisible( void )
{
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_STATICMODEL ) != 0 )
  {
    registActor();
  }
  else
  {
    unregistActor();
  }
}
#endif

GFL_NAMESPACE_END(Field)
