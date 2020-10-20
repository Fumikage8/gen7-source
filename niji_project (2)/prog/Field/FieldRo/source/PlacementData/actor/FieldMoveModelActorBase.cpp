//======================================================================
/**
 * @file FieldMoveModelActorBase.cpp
 * @date 2015/08/19 15:51:36
 * @author saita_kazuki
 * @brief 動作モデルアクター基底クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActorBase.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

#include "niji_conv_header/field/FieldPlacementMoveCode.h"
#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

GFL_NAMESPACE_BEGIN(Field)

/**
 *  @brief  コンストラクタ
 */
FieldMoveModelActorBase::FieldMoveModelActorBase( void )
  : IFieldActorBase()
  , m_pMoveModelManager( NULL )
  , m_pTerrainManager( NULL )
  , m_pRenderingPipeLine( NULL )
  , m_ModelIndex( Field::MoveModel::FIELD_MOVE_MODEL_MAX )
  , m_MoveModelHeaderWork()
  , m_MoveModelHeaderResource()
  , m_bIsCulling( false )
  , m_bIsCullingRegist( false )
{
}

/**
 *  @brief  デストラクタ
 */
FieldMoveModelActorBase::~FieldMoveModelActorBase( void )
{
}

/**
 *  @brief  完全破棄
 */
void FieldMoveModelActorBase::Terminate( void )
{
  if( terminate() == false)
  {
    return;
  }

  m_pMoveModelManager->TerminateMoveModelWorkResource( static_cast<Field::MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
  m_ModelIndex = Field::MoveModel::FIELD_MOVE_MODEL_MAX;
}

/**
 *  @brief  リソース、コリジョンの破棄(モデルワークは残す）
 */
void FieldMoveModelActorBase::TerminateResource( void )
{
  terminate();
}

/**
 *  @brief  追加
 */
void FieldMoveModelActorBase::RegistActor( void )
{
  m_bIsRegist = true;
  registActor();
}

/**
 *  @brief  削除
 */
void FieldMoveModelActorBase::UnregistActor( void )
{
  m_bIsRegist = false;
  unregistActor();
}

/**
 *  @brief  動作モデル取得
 */
MoveModel::FieldMoveModel *FieldMoveModelActorBase::GetMoveModel( void ) const
{
  return m_pMoveModelManager->GetFieldMoveModel( static_cast<Field::MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
}

/**
 *  @brief 配置情報内のMoveCodeを、Runtime上のMoveCodeに変換
 */
MoveModel::FIELD_MOVE_CODE_ID FieldMoveModelActorBase::GetPlacementMoveCodeToMoveCode( u32 placement_movecode )
{
  static const MoveModel::FIELD_MOVE_CODE_ID sc_PlacementMoveCodeToMoveCode[] = 
  {
    MoveModel::FIELD_MOVE_CODE_NONE,
    MoveModel::FIELD_MOVE_CODE_NPC_RETURN          ,       ///< NPC挙動   注目[無] 回転[無] 初期配置戻り[有]

    MoveModel::FIELD_MOVE_CODE_NPC_MEET            ,       ///< NPC挙動   注目[有] 回転[有] 初期配置戻り[無]
    MoveModel::FIELD_MOVE_CODE_NPC_MEET_RETURN     ,       ///< NPC挙動   注目[有] 回転[有] 初期配置戻り[有]
    MoveModel::FIELD_MOVE_CODE_NPC_LOOK            ,       ///< NPC挙動   注目[有] 回転[無] 初期配置戻り[無]
    MoveModel::FIELD_MOVE_CODE_NPC_LOOK_RETURN     ,       ///< NPC挙動   注目[有] 回転[無] 初期配置戻り[無]

    MoveModel::FIELD_MOVE_CODE_NPC_MEET_SHORT,             ///< NPC挙動   注目[有] 回転[有] 初期配置戻り[無] 注目距離縮小
    MoveModel::FIELD_MOVE_CODE_NPC_MEET_SHORT_RETURN,      ///< NPC挙動   注目[有] 回転[有] 初期配置戻り[有] 注目距離縮小
    MoveModel::FIELD_MOVE_CODE_NPC_LOOK_SHORT,             ///< NPC挙動   注目[有] 回転[無] 初期配置戻り[無] 注目距離縮小
    MoveModel::FIELD_MOVE_CODE_NPC_LOOK_SHORT_RETURN,      ///< NPC挙動   注目[有] 回転[無] 初期配置戻り[無] 注目距離縮
    
    // @fix NMcat[1248]:タマゴが発生した際に、一定ラインを超えないとNPC動作が発生しない、対処。NPC動作制御をintrPosから動作コードに変更
    MoveModel::FIELD_MOVE_CODE_NPC_AZUKARIYA,
    MoveModel::FIELD_MOVE_CODE_FUREAI_A,                   ///< ふれあい挙動Aタイプ
  };

  // @caution 動作コードを新規追加する際は無効化する必要がある
  GFL_ASSERT( GFL_NELEMS(sc_PlacementMoveCodeToMoveCode) ==  PLACEMENT_MOVE_CODE_MAX );

  if( placement_movecode < PLACEMENT_MOVE_CODE_MAX )
  {
    return sc_PlacementMoveCodeToMoveCode[placement_movecode];
  }
  else
  {
    // @caution 動作コードを新規追加する際は無効化する必要がある
    GFL_ASSERT(placement_movecode < PLACEMENT_MOVE_CODE_MAX);
    return MoveModel::FIELD_MOVE_CODE_NONE;
  }
}

/**
 *  @brief  動作モデルワークの初期化
 */
void FieldMoveModelActorBase::initializeMoveModelWork()
{
  if( isMoveModelWorkCreated( m_ModelIndex))
  {
    // この場合は復帰処理なのでワークを初期化しない
    return;
  }

  // 空きを取得
  m_ModelIndex = m_pMoveModelManager->GetFieldMoveModelIndexFromFreeSpace();
  if( m_ModelIndex >= MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    GFL_ASSERT_MSG( false, "MoveModel free work none" );
    return;
  }

  // ワークの確保
  m_pMoveModelManager->InitializeMoveModelWork( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ), &m_MoveModelHeaderWork );
}

/**
 *  @brief  動作モデル番号の取得
 *  @param eventID このインスタンスを識別するためのユニークなID
 *  @return 動作モデル番号。MoveModel::FIELD_MOVE_MODEL_MAX以外が返ってきたら復帰処理
 */
u32 FieldMoveModelActorBase::getMoveModelIndex( u32 eventID) const
{
  return m_pMoveModelManager->GetFieldMoveModelIndexFromEventId( eventID );
}

/**
 *  @brief  動作モデルリソースの初期化
 *  @param  pDressUpModelResourceManager    着せ替えリソースマネージャ（通常はNULLでOK。ジョインフェスタのみ渡す。)
 */
void FieldMoveModelActorBase::initializeMoveModelResource( poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager )
{
  m_pMoveModelManager->InitializeMoveModelResource( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ), &m_MoveModelHeaderResource, pDressUpModelResourceManager );
}

/**
 *  @brief  コリジョンデータ取得
 *  @param pOutColData コリジョンデータ格納先
 *  @param collisionAccessor コリジョンデータアクセサー
 *  @param index コリジョンデータ番号
 */
bool FieldMoveModelActorBase::getCollisionData(
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
void FieldMoveModelActorBase::setupCollision(
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
 *  @brief  MoveModelWorkが既に作られているか
 *  @retval true 作られている
 *  @retval true 作られていない
 */
bool FieldMoveModelActorBase::isMoveModelWorkCreated( u32 modelIndex)
{
  if( modelIndex < MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    return true;
  }
  return false;
}

/**
 *  @brief  破棄共通処理
 *  @retval true 破棄処理を行った
 *  @retval false 破棄処理を行っていない
 */
bool FieldMoveModelActorBase::terminate()
{
  if( m_ModelIndex >= Field::MoveModel::FIELD_MOVE_MODEL_MAX ){ return false; }

  Field::MoveModel::FieldMoveModel *pCharaModel = m_pMoveModelManager->GetFieldMoveModel( static_cast<Field::MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );

  if( !pCharaModel ){ return false; }
  if( !pCharaModel->IsSetupWork() ){ return false; }

  if( pCharaModel->IsSetupResource() )
  {
    UnregistActor(); // リソースが存在しているならばUnregistする。
  }


  m_pTerrainManager = NULL;
  m_pRenderingPipeLine = NULL;

  m_bIsCulling = false;

  return true;
}

/**
 *  @brief  カリング状態ON
 */
void FieldMoveModelActorBase::onCulling()
{
  m_bIsCulling = true;
  unregistActor();
}

/**
 *  @brief  カリング状態OFF
 */
void FieldMoveModelActorBase::offCulling()
{
  m_bIsCulling = false;
  registActor();
}

/**
 *  @brief  アクターを登録できるか
 *  @retval true できる
 *  @retval false できない
 */
bool FieldMoveModelActorBase::isRegistActor( void )
{
#if PM_DEBUG
  return ( m_bIsRegist && !m_bIsCulling && ( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_MOVEMODEL )!=0 ) );
#endif

  return ( m_bIsRegist && !m_bIsCulling );
}

/**
 *  @brief  アクターを削除できるか
 *  @retval true できる
 *  @retval false できない
 */
bool FieldMoveModelActorBase::isUnRegistActor( void )
{
#if PM_DEBUG
  return ( !m_bIsRegist || m_bIsCulling || ( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_MOVEMODEL )==0 ) );
#endif

  return ( !m_bIsRegist || m_bIsCulling );
}

/**
 *  @brief  動作モデルの登録を行う
 */
void FieldMoveModelActorBase::registMoveModel( void )
{
  Field::MoveModel::FieldMoveModel *pCharaModel = m_pMoveModelManager->GetFieldMoveModel( static_cast<Field::MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
  if( pCharaModel == NULL )
  {
    return;
  }

  pCharaModel->SetVisible( true );
}

/**
 *  @brief  動作モデルの登録解除を行う
 */
void FieldMoveModelActorBase::unregistMoveModel( void )
{
  Field::MoveModel::FieldMoveModel *pCharaModel = m_pMoveModelManager->GetFieldMoveModel( static_cast<Field::MoveModel::FIELD_MOVE_MODEL_ID>( m_ModelIndex ) );
  if( pCharaModel == NULL )
  {
    return;
  }

  pCharaModel->SetVisible( false );
}

#if PM_DEBUG
/**
 *  @brief  デバッグ用：描画切替
 */
void FieldMoveModelActorBase::DebugFunc_ChangeVisible( void )
{
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_MOVEMODEL ) != 0 )
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
