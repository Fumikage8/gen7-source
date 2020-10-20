//=================================================================================================
/**
*  @file FieldGimmickLift.h
*  @brief フィールドギミック 昇降機
*  @author saita_kazuki
*  @date 2016.02.17
*/
//=================================================================================================

#include "Field/FieldRos/FieldGimmickLift/include/FieldGimmickLift.h"

// gflib
#include <util/include/gfl2_std_string.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// gamesys
#include "GameSys/include/GameManager.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelShadowManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectShadow.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectShadowFoot.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @brief コンストラクタ
 *  @param pHeap ヒープ
 *  @param pFieldmap フィールドマップ
 *  @param zoneId ゾーンID
 *  @param pPlacementDataManager 配置情報管理クラス
 */
FieldGimmickLift::FieldGimmickLift( gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap, u32 zoneId, PlacementDataManager* pPlacementDataManager ) 
  : FieldGimmick( pFieldmap)
  , m_pHeap( pHeap)
  , m_pPlacementDataManager( pPlacementDataManager)
  , m_zoneID( zoneId)
{
  this->Clear();
}

/**
 *  @brief デストラクタ
 */
FieldGimmickLift::~FieldGimmickLift()
{
}

/**
 *  @biref 破棄更新
 *  @return 破棄が完了したかどうか
 *
 *  破棄が完了するまでマイフレーム呼出がなされる関数
 */
bool FieldGimmickLift::Finalize( void )
{
  this->Clear();

  return true;
}

/**
 *  @brief 更新
 *  Fieldmap::Updateにて呼出
 *  UpdateNormalが終了してから呼び出される
 */
void FieldGimmickLift::UpdateLast( void )
{
  switch( m_state)
  {
  case STATE_NONE:
    this->UpdateStateNone();
    break;

  case STATE_RUNNIG:
    this->UpdateStateRunning();
    break;

  case STATE_END:
    this->UpdateStateEnd();
    break;

  default:
    GFL_ASSERT_MSG( 0, "Invalid state\n");
    m_state = STATE_NONE;
    break;
  }
}

/**
 *  @brief 登録
 *  @param param 登録パラメーター
 *  @note この関数を呼んだ後からギミックが動作します
 */
void FieldGimmickLift::Regist( const FieldGimmickLift::RegistParam& param)
{
  if( m_state != STATE_NONE)
  {
    GFL_ASSERT_MSG( 0, "Not call UnRegist : state is %d\n", m_state);
    return;
  }

  // クリアしておく
  this->Clear();

  // モード設定
  m_mode = param.mode;

  // 配置モデルの登録
  StaticModel::StaticModelManager*  pStaticModelManager = m_pFieldmap->GetStaticModelManager();
  m_pStaticModel = pStaticModelManager->GetEventIDStaticModel( param.staticModelEventID);
  if( m_pStaticModel == NULL)
  {
    GFL_ASSERT_MSG( 0, "Not found static model : eventID %d\n", param.staticModelEventID);
    this->Clear();
    return;
  }

  // 配置モデルカリングを無効にする
  m_pStaticModel->SetUpdateCullingEnable( false);
  poke_3d::model::BaseModel* pModel = this->GetBaseModelByStaticModel( m_pStaticModel);
  pModel->SetVisible( true);
  pModel->SetAnimationCalculateEnable( true);

  // ジョイント番号の取得
  m_staticModelJointIndex = this->GetBaseModelByStaticModel( m_pStaticModel)->GetJointIndex( "LiftPos");
  if( m_staticModelJointIndex == -1)
  {
    GFL_ASSERT_MSG( 0, "Not found joint index\n");
    this->Clear();
    return;
  }

  gfl2::math::Vector3 jointPos = this->GetStaticModelJointWorldPosition( m_pStaticModel, m_staticModelJointIndex);

  // 動作モデルの登録
  MoveModel::FieldMoveModelManager* pMoveModelManager = m_pFieldmap->GetMoveModelManager();
  for( u32 i = 0; i < REGIST_MOVE_MODEL_MAX; ++i)
  {
    if( param.moveModelEventID[i] != FIELD_EVENTID_NULL)
    {
      MoveModel::FIELD_MOVE_MODEL_ID moveModelIndex = static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( pMoveModelManager->GetFieldMoveModelIndexFromEventId( param.moveModelEventID[i]));
      if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX)
      {
        // 無効値でないイベントIDで検索したのに失敗した
        GFL_ASSERT_MSG( 0, "Not found move model : eventID %d\n", param.moveModelEventID[i]);
        continue;
      }

      m_pMoveModel[i] = pMoveModelManager->GetFieldMoveModel( moveModelIndex);

      // 影を地形ではなく、動作モデルの足元に出すようShadowManagerに誤認させるため、地面コリジョンを退避する
      m_pCollisionSceneStash[i] = m_pMoveModel[i]->GetCollisionSceneContainerForGround();
      m_pMoveModel[i]->SetCollisionSceneContainerForGround( NULL);

      // 地形とのコリジョンを無効化
      m_pMoveModel[i]->GetDynamicActor( MoveModel::COLLISION_TYPE_TERRAIN)->SetActive( false);

      // オフセット取得
      if( m_mode == GIMMICK_LIFT_MODE_XYZ)
      {
        gfl2::math::Vector3 modelPos = m_pMoveModel[i]->GetCharaDrawInstance()->GetPosition();
        m_jointOffset[i] = modelPos - jointPos;
      }
    }
  }

  // 動作開始
  m_state = STATE_RUNNIG;
}

/**
 *  @brief 登録解除
 */
void FieldGimmickLift::UnRegist()
{
  if( m_state != STATE_RUNNIG)
  {
    GFL_ASSERT_MSG( 0, "Not call Regist or Can not UnRegist state : state is %d\n", m_state); // 開発中に気付かせるアサート
    return;
  }

  m_state = STATE_END;
}

/**
 *  @brief クリア処理
 */
void FieldGimmickLift::Clear()
{
  m_state                 = STATE_NONE;
  m_mode                  = GIMMICK_LIFT_MODE_Y;
  m_pStaticModel          = NULL;
  m_staticModelJointIndex = -1;

  for( u32 i = 0; i < REGIST_MOVE_MODEL_MAX; ++i)
  {
    m_pMoveModel[i] = NULL;
    m_pCollisionSceneStash[i] = NULL;
    m_jointOffset[i] = gfl2::math::Vector3::GetZero();
  }
}

/**
 *  @brief 登録した動作モデルのみトラバースする
 */
void FieldGimmickLift::TraverseRegistMoveModel()
{
  for( u32 i = 0; i < REGIST_MOVE_MODEL_MAX; ++i)
  {
    if( m_pMoveModel[i])
    {
      gfl2::renderingengine::scenegraph::DagNode* pDagNode = m_pMoveModel[i]->GetCharaDrawInstance()->GetModelInstanceNode();
      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast( pDagNode);
    }
  }

  // 影管理インスタンス取得
  MoveModel::FieldMoveModelShadowManager* pShadowManager = m_pFieldmap->GetMoveModelShadowManager();
  pShadowManager->UpdateShadow();

  // 影のトラバース
  for( u32 i = 0; i < MoveModel::FieldMoveModelShadowManager::SHADOW_MAX; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = pShadowManager->GetEffectShadow( i)->GetBaseModel()->GetModelInstanceNode();
    for( u32 i = 0; i < pModelInstanceNode->GetJointNum(); ++i)
    {
      gfl2::renderingengine::scenegraph::DagNode* pDagNode = pModelInstanceNode->GetJointInstanceNode( i);
      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast( pDagNode);
    }
  }

  // 足跡影のトラバース
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = pShadowManager->GetEffectShadowFoot()->GetBaseModel()->GetModelInstanceNode();
    for( u32 i = 0; i < pModelInstanceNode->GetJointNum(); ++i)
    {
      gfl2::renderingengine::scenegraph::DagNode* pDagNode = pModelInstanceNode->GetJointInstanceNode( i);
      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast( pDagNode);
    }
  }
}

/**
 *  @brief 配置モデルの指定ジョイント番号のワールド座標を取得
 *  @param pStaticModel 配置モデル
 *  @param jointIndex ジョイント番号
 *  @return 指定ジョイントのワールド座標
 *  @note 引数が不正な場合は座標(0,0,0)が返る
 */
gfl2::math::Vector3 FieldGimmickLift::GetStaticModelJointWorldPosition( StaticModel::StaticModel* pStaticModel, s32 jointIndex)
{
  if( pStaticModel == NULL)
  {
    GFL_ASSERT_MSG( 0, "Static model is NULL\n");
    return gfl2::math::Vector3::GetZero();
  }
  if( jointIndex == -1)
  {
    GFL_ASSERT_MSG( 0, "Joint index is -1\n");
    return gfl2::math::Vector3::GetZero();
  }
  return this->GetBaseModelByStaticModel( pStaticModel)->GetJointWorldPosition( m_staticModelJointIndex);
}

/**
 *  @brief 配置モデルのBaseModelを取得
 *  @param pStaticModel 配置モデル
 *  @note 引数が不正な場合はNULLが返る
 */
poke_3d::model::BaseModel* FieldGimmickLift::GetBaseModelByStaticModel( StaticModel::StaticModel* pStaticModel)
{
  if( pStaticModel == NULL)
  {
    GFL_ASSERT_MSG( 0, "Static model is NULL\n");
    return NULL;
  }
  return pStaticModel->GetBaseModel( StaticModel::MODEL_TYPE_HIGH);
}

/**
 *  @brief ギミックモードに応じたセット座標に変換する
 */
void FieldGimmickLift::ConvertModeSetPosition( gfl2::math::Vector3* pOutPos, const gfl2::math::Vector3& srcPos, poke_3d::model::BaseModel* pModel, const gfl2::math::Vector3& offset)
{
  switch( m_mode)
  {
  case GIMMICK_LIFT_MODE_Y:
    {
      // Y座標だけ変更
      gfl2::math::Vector3 modelPos = pModel->GetPosition();
      pOutPos->SetX( modelPos.GetX());
      pOutPos->SetY( srcPos.GetY());
      pOutPos->SetZ( modelPos.GetZ());
    }
    break;

  case GIMMICK_LIFT_MODE_XYZ:
    {
      // XYZ座標すべて変更
      pOutPos->Set( srcPos + offset);
    }
    break;

  default:
    {
      GFL_ASSERT_MSG( 0, "Invalid mode\n");
      pOutPos->Set( srcPos);
    }
    break;
  }
}

/**
 *  @brief 状態 : なし 更新
 */
void FieldGimmickLift::UpdateStateNone()
{
  // 何もしない
}

/**
 *  @brief 状態 : 動作中 更新
 */
void FieldGimmickLift::UpdateStateRunning()
{
  // 昇降機の座標を取得
  gfl2::math::Vector3 srcPos = this->GetStaticModelJointWorldPosition( m_pStaticModel, m_staticModelJointIndex);

  // 動作モデルにセットする
  for( u32 i = 0; i < REGIST_MOVE_MODEL_MAX; ++i)
  {
    if( m_pMoveModel[i] == NULL)
    {
      continue;
    }

    // モードごとにセットする座標が変わる
    gfl2::math::Vector3 fixPos;
    poke_3d::model::BaseModel* pModel = m_pMoveModel[i]->GetCharaDrawInstance();
    this->ConvertModeSetPosition( &fixPos, srcPos, pModel, m_jointOffset[i]);

    pModel->SetPosition( fixPos);
  }

  // トラバース
  this->TraverseRegistMoveModel();
}

/**
 *  @brief 状態 : 終了 更新
 */
void FieldGimmickLift::UpdateStateEnd()
{
  for( u32 i = 0; i < REGIST_MOVE_MODEL_MAX; ++i)
  {
    if( m_pMoveModel[i] == NULL)
    {
      continue;
    }

    // 地形とのコリジョンを有効化
    m_pMoveModel[i]->GetDynamicActor( MoveModel::COLLISION_TYPE_TERRAIN)->SetActive( true);

    // 地面コリジョンを戻す
    m_pMoveModel[i]->SetCollisionSceneContainerForGround( m_pCollisionSceneStash[i]);

    // 地面に吸着させる
    m_pMoveModel[i]->AdjustGround();
  }

  // 配置モデルカリングを有効にする
  if( m_pStaticModel)
  {
    m_pStaticModel->SetUpdateCullingEnable( true);
  }

  // 動作終了
  this->Clear();
}

GFL_NAMESPACE_END( Field )
