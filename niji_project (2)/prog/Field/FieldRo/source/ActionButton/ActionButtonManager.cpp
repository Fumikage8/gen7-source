/**
* @brief  アクションボタン管理するクラス
* @file   ActionButtonManager.cpp
*/

// field
#include "Field/FieldRo/include/ActionButton/ActionButtonManager.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include <niji_conv_header/field/FieldEffectTypes.h>
#include "Field/FieldRo/include/Collision/BaseActor.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectActionButton.h"
#include "Field/FieldRo/include/Fieldmap.h"
// Actor
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldBgEventActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMapJumpActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldTrainerActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldItemActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldBerryPointActor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldFishingSpotActor.h"

// GameData
#include "GameSys/include/GameData.h"

// arc
#include <arc_def_index/arc_def.h>

// camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"

#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( ActionButton );

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
ActionButtonManager::ActionButtonManager( void )
: m_isFocusTrainer    (false)
, m_isInterestNotClear(false)
, m_isInterestRequest (false)
, m_isFureaiPokemon (false)
{
  m_pActionButtonDatas = NULL;
  m_DataSize = 0;
  m_DataNum = 0;
  m_eventID = 0xffffffff;
  m_pEffect = NULL;
  m_State = IDLE;
  m_WaitCount = 0;
  m_NowWaitCount = 0;
}

/**
* @brief デストラクタ
*
* @return 無し
*/
ActionButtonManager::~ActionButtonManager( void )
{
}

/**
* @brief 初期化処理
*
* @param  rInitDesc   初期化設定
*
* @return 無し
*/
void ActionButtonManager::Initialize( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager )
{
	m_eventID = 0xffffffff;
	m_State = IDLE;
	m_WaitCount = 0;
	m_NowWaitCount = 0;
	m_pEffect = NULL;

	{
    //ロード直後にクローズするのでヒープの指定はすべてLowからとる
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_FOCUS_DATA;
    openReq.heapForFile = pHeap->GetLowerHandle();
    openReq.heapForArcSrc = pHeap->GetLowerHandle();
    openReq.heapForReq = pHeap->GetLowerHandle();
    pAsyncFileReadManager->SyncArcFileOpen( openReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_FOCUS_DATA;
    loadReq.datId = 0;
    loadReq.ppBuf = reinterpret_cast<void**>(&m_pActionButtonDatas);
    loadReq.heapForBuf = pHeap;
    loadReq.pBufSize = &m_DataSize;
    loadReq.align = 4;
    pAsyncFileReadManager->SyncArcFileLoadData( loadReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_FOCUS_DATA;
    closeReq.heapForReq = pHeap->GetLowerHandle();
    pAsyncFileReadManager->SyncArcFileClose( closeReq );
  }

	m_DataNum = m_DataSize / sizeof(ActionButtonData);
}

/**
* @brief 終了処理
*
* @return 無し
*/
void ActionButtonManager::Terminate( Effect::EffectManager *pEffectManager )
{
	if (m_pEffect != NULL)
	{
		pEffectManager->DeleteEffect( m_pEffect );
		m_pEffect = NULL;
	}
	GflHeapSafeFreeMemory(m_pActionButtonDatas);
}

/**
* @brief 更新処理
*
* @return 無し
*/
void ActionButtonManager::Update( BaseActor *pActor, MoveModel::FieldMoveModelManager* pFieldCharaModelManager, Effect::EffectManager *pEffectManager, b32 isInterestNotClear )
{
  // pActorがNULLでも注視をきらないかどうか
  m_isInterestNotClear = isInterestNotClear;

  m_isFureaiPokemon = false;

  // カメラがトレーナーにフォーカスしていればイベントそっちのけでトレーナーを見る
  Field::MoveModel::FieldMoveModel* pPlayer        = static_cast<Field::MoveModel::FieldMoveModel*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ) );
  Field::Camera::CameraManager*     pCameraManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetCameraManager();
  if( pPlayer && pCameraManager && pCameraManager->GetFocus() && pCameraManager->GetFocus()->IsInterest() )
  {
    gfl2::math::Vector3 position;
    pCameraManager->GetFocus()->GetInterestPosition( &position );
    pPlayer->EnableInterest( position );
    m_isFocusTrainer    = true;
    m_isInterestRequest = true;
  }
  else
  {
    m_isFocusTrainer = false;
  }

  MoveModel::FieldMoveModel* pMoveModel = pFieldCharaModelManager->GetInterestFureaiPokemon();

  // @fix GFMMcat[396]:ふれあいポケモンへの注視が不要に継続する、対処。スクリプトイベント中はふれあいの注視リクエストを無視するように修正
  if (pMoveModel && pPlayer && pPlayer->GetEventRequestCnt() == 0 ) // @fix cov_ctr[12609]:NULLチェック追加
  {
    gfl2::math::Vector3 interestPos(gfl2::math::Vector3::GetZero());
    MoveModel::FieldMoveCodeNpc::GetInterestPosition(pMoveModel, &interestPos);
    pPlayer->EnableInterest(interestPos);
    m_isFureaiPokemon = true;
    m_isInterestRequest = true; // @fix GFMMcat[396]:ふれあいポケモンへの注視が不要に継続する、対処。リクエストが途絶した際に主人公の注視を解除するように修正。
  }

	if (pActor == NULL)
	{
		ResetState( pEffectManager );
		return;
	}
	
	u32 target_event_id = 0xffffffff;
	u32 wait_count = 0;
	ICON_FRAME icon_frame = NONE;
	gfl2::math::Vector3 icon_pos(0,0,0);
	gfl2::math::Vector3 attention_pos(0,0,0);
	b32 attention = false;

  if( !this->GetActionButtonParam( pActor, &target_event_id, &wait_count, &icon_frame, &icon_pos, &attention_pos, &attention ) )
  {
		ResetState( pEffectManager );
		return;
  }

	switch (m_State)
	{
	case IDLE:
		//何かしがたのイベントを検出できた場合は準備ステートに移行する
		if ((target_event_id != 0xffffffff)&&(icon_frame != NONE))
		{
			m_eventID = target_event_id;
			m_NowWaitCount = 0;
			m_WaitCount = wait_count;
		  m_State = READY;
		}
		break;
	case READY:
		//準備中に異なるイベントIDを検出したときは、ステートをリセットする（アイドル状態に戻す）
		if (m_eventID != target_event_id)
		{
			ResetState(pEffectManager);
			return;
		}
		//表示までの時間計算
		if(++m_NowWaitCount >= m_WaitCount)
		{
			m_pEffect = pEffectManager->CreateEffect( Effect::EFFECT_TYPE_ACTION_BUTTON, icon_pos);
      if (m_pEffect != NULL)
      {
        Effect::EffectActionButton *eff = static_cast<Effect::EffectActionButton*>(m_pEffect);
        eff->SetAnimationFrame(icon_frame);
      }
			m_State = DISP;
		}
		break;
	case DISP:
		//表示中に異なるイベントIDを検出したときは、ステートをリセットする（アイドル状態に戻す）
		if (m_eventID != target_event_id)
		{
			ResetState(pEffectManager);
			return;
		}
		//座標更新
    if (m_pEffect != NULL)
    {
      Effect::EffectActionButton *eff = static_cast<Effect::EffectActionButton*>(m_pEffect);
		  eff->SetPosition(icon_pos);
    }
		break;
	}

  // カメラがトレーナーにフォーカスしていなければイベントの方を見る
  if( attention && pPlayer && !m_isFocusTrainer )
  {
    pPlayer->EnableInterest( attention_pos );
    m_isInterestRequest = true;
  }
  else
  {

  }
}

/**
* @brief 内部ステートのリセット
*
* @return 無し
*/
void ActionButtonManager::ResetState( Effect::EffectManager *pEffectManager )
{
	m_eventID = 0xffffffff;
	m_State = IDLE;
	m_WaitCount = 0;
	m_NowWaitCount = 0;
	if (m_pEffect != NULL)
	{
		pEffectManager->DeleteEffect( m_pEffect );
		m_pEffect = NULL;
	}

  // トレーナー注目をしていない状態で対象イベントがない場合は注目を終了
  const u32 DEFAULT_DISABLE_FRAME = 8;
  Field::MoveModel::FieldMoveModel* pPlayer = static_cast<Field::MoveModel::FieldMoveModel*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ) );
  if( pPlayer && !m_isFocusTrainer && !m_isInterestNotClear && m_isInterestRequest && !m_isFureaiPokemon )
  { 
    pPlayer->DisableInterest(DEFAULT_DISABLE_FRAME);
    m_isInterestRequest = false;
  }
}

/**
* @brief 指定フォーカスＩＤのデータは全方位許容型かを返す
*
* @return trueで全方位許容
*/
bool ActionButtonManager::IsAllRangeFocus(int focus_id)
{
  if ( (0 < focus_id) && (focus_id < m_DataNum) )
  {
    ActionButtonData *action_btn_data = &(m_pActionButtonDatas[focus_id]);
    if (action_btn_data->mAllRange != 0) return true;
  }
  return false;
}

/** 
 *  @brief BaseActorから情報を取得する。
 */
bool ActionButtonManager::GetActionButtonParam( BaseActor *pActor, u32 * p_target_event_id, u32* p_wait_count, ActionButtonManager::ICON_FRAME* p_icon_frame, gfl2::math::Vector3* p_icon_pos, gfl2::math::Vector3* p_attention_pos, b32* p_attention ) const
{
  IFieldActorBase *pBaseActor = reinterpret_cast<IFieldActorBase*>( pActor->GetUserData() );
  return GetActionButtonParam( pBaseActor, p_target_event_id, p_wait_count, p_icon_frame, p_icon_pos, p_attention_pos, p_attention );
}


bool ActionButtonManager::GetActionButtonParam( IFieldActorBase *pBaseActor, u32 * p_target_event_id, u32* p_wait_count, ActionButtonManager::ICON_FRAME* p_icon_frame, gfl2::math::Vector3* p_icon_pos, gfl2::math::Vector3* p_attention_pos, b32* p_attention ) const
{
  if( pBaseActor == NULL )  return false;

	//対象イベントIDをtarget_event_id にセットする
	FieldActorType type = pBaseActor->GetType();
  //
  /*
	if ((type == FIELD_ACTOR_TYPE_MOVEMODEL)||(type == FIELD_ACTOR_TYPE_TRAINER))
	{
    gfl2::math::Vector3 icon_ofs(0,0,0);
		u32 workIndex;
		u32 event_id = 0xffffffff;
		if (type == FIELD_ACTOR_TYPE_MOVEMODEL)
		{
			FieldMoveModelActor *pMoveModelActor = reinterpret_cast<FieldMoveModelActor*>( pBaseActor );
		  const FieldMoveModelAccessor::MoveModelData *data = pMoveModelActor->GetData();
		  workIndex                         = pFieldCharaModelManager->GetFieldMoveModelIndexFromEventId( data->eventID );
			event_id = data->eventID;
      icon_ofs = GetMoveModelIconOffset(pFieldCharaModelManager, data->characterId);
		}
		else //type == FIELD_ACTOR_TYPE_TRAINER
		{
			FieldTrainerActor *pTrainerActor = reinterpret_cast<FieldTrainerActor*>( pBaseActor );
			const FieldTrainerAccessor::TrainerData *data = pTrainerActor->GetData();
		  workIndex                         = pFieldCharaModelManager->GetFieldMoveModelIndexFromEventId( data->eventID );
			event_id = data->eventID;
      icon_ofs = GetMoveModelIconOffset(pFieldCharaModelManager, data->characterId);
		}

		MoveModel::FieldMoveModel* pMoveModel = pFieldCharaModelManager->GetFieldMoveModel( workIndex );
		if( pMoveModel )
    {
			s32 joint_idx = pMoveModel->GetCharaDrawInstance()->GetJointIndex("CamTgtHead");
			gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJoint = pMoveModel->GetCharaDrawInstance()->GetModelInstanceNode()->GetJointInstanceNode(joint_idx);
      if( pJoint )
      {
        gfl2::math::Matrix34 mat  ( pJoint->GetWorldMatrix() );
        icon_pos = mat.GetElemPosition() + icon_ofs;
				target_event_id = event_id;
				wait_count = ICON_DISP_WAIT;
				icon_frame = NORMAL;
      }
    }
	}
	else*/  //20150904　仕様変更 動作モデルにはアクションアイコンを出さない

  if ((type == FIELD_ACTOR_TYPE_MOVEMODEL))
  {
    gfl2::math::Vector3 icon_ofs(0, 0, 0);
    u32 workIndex;
    u32 event_id = 0xffffffff;
    FieldMoveModelActor *pMoveModelActor = reinterpret_cast<FieldMoveModelActor*>(pBaseActor);
    if (pMoveModelActor->IsContactPokemon())
    {
      MoveModel::FieldMoveModel* pMoveModel = pMoveModelActor->GetMoveModel();
      if (pMoveModel)
      {
        gfl2::math::Vector3 pos;
        MoveModel::FieldMoveCodeNpc::GetInterestPosition(pMoveModel, &pos);
        *p_icon_pos = pos;
        *p_attention_pos = pos;
        *p_target_event_id = event_id;
        *p_wait_count = 0;
        *p_icon_frame = NONE;
        *p_attention = true;

        return true;
      }
    }
  }
  else
	{
		gfl2::math::Quaternion quat;
		u32 event_id = 0xffffffff;
		int focus_id = 0;
		if(type == FIELD_ACTOR_TYPE_BG)
		{
			FieldBgEventActor *pBgEventActor = reinterpret_cast<FieldBgEventActor*>( pBaseActor );
		  const FieldBgEventAccessor::BgEventData *data = pBgEventActor->GetData();
		  focus_id = data->attentionID;
			event_id = data->eventID;
			quat.Set(data->quaternion);
		}
		else if (type == FIELD_ACTOR_TYPE_MAPJUMP)
		{
			FieldMapJumpActor *pMapJumpActor = reinterpret_cast<FieldMapJumpActor*>( pBaseActor );
		  const FieldMapJumpAccessor::MapJumpData *data = pMapJumpActor->GetData();
			focus_id = data->focus;
			event_id = data->eventID;
			quat.Set(data->quaternion);
		}
		else if(type == FIELD_ACTOR_TYPE_ITEM)
		{
			FieldItemActor *pItemActor = reinterpret_cast<FieldItemActor*>( pBaseActor );
		  const FieldItemAccessor::FieldItemData *data = pItemActor->GetData();
		  focus_id = data->attentionID;
			event_id = data->eventID;
			quat.Set(data->quaternion);
		}
    else if(type == FIELD_ACTOR_TYPE_BERRY_POINT)
    {
      FieldBerryPointActor* pBerryPointActor = reinterpret_cast<FieldBerryPointActor*>(pBaseActor);
      event_id = pBerryPointActor->GetEventID();
      focus_id = pBerryPointActor->GetFocusID();
      pBerryPointActor->GetRotation(&quat);
    }
    else if(type == FIELD_ACTOR_TYPE_FISHING_SPOT)
    {
      FieldFishingSpotActor* pFishingSpotActor = reinterpret_cast<FieldFishingSpotActor*>(pBaseActor);
      const FieldFishingSpotAccessor::Data* pData = pFishingSpotActor->GetData();

      event_id = pData->eventID;
      focus_id = pData->focusID;
      quat.Set(pData->quaternion[0], pData->quaternion[1], pData->quaternion[2], pData->quaternion[3]);
    }

		if ( (0 < focus_id) && (focus_id < m_DataNum) )
		{
			ActionButtonData *action_btn_data = &(m_pActionButtonDatas[focus_id]);
			gfl2::math::Vector3 ofs(action_btn_data->mIconPosX, action_btn_data->mIconPosY, action_btn_data->mIconPosZ);
			gfl2::math::Matrix34 mat;
			quat.QuaternionToMatrix(mat);
			ofs = mat * ofs;
			
      *p_icon_pos = pBaseActor->GetPosision();
			*p_attention_pos = pBaseActor->GetPosision();
			*p_target_event_id = event_id;
			*p_wait_count = action_btn_data->mWait;
			*p_icon_frame = static_cast<ICON_FRAME>(action_btn_data->mType);
			*p_icon_pos += ofs;
			*p_attention = GFL_BOOL_CAST(action_btn_data->mEyeValid);

      //@fix NMCat[1008] 主人公の家 自分の部屋：自機が地面に注目する場所がある
      gfl2::math::Vector3 eyeOfs(action_btn_data->mEyePosX, action_btn_data->mEyePosY, action_btn_data->mEyePosZ);
			eyeOfs = mat * eyeOfs;
			*p_attention_pos += eyeOfs;

      return true;
		}
	}

  return false;
}

/**
 * @brief 対象動作モデルのアイコンオフセット取得
 *
 * @return Vector3
 */
gfl2::math::Vector3 ActionButtonManager::GetMoveModelIconOffset(MoveModel::FieldMoveModelManager* pFieldCharaModelManager, const u32 characterId)
{
  gfl2::math::Vector3 iconOffset(0,0,0);
  // 共通のアプリケーションデータを取得
  const Field::MoveModel::ApplicationDataForCommon* pAppData = pFieldCharaModelManager->GetApplicationData( characterId );

  if (pAppData == NULL) return iconOffset;

  // npc
  if ( pAppData->type == Field::MoveModel::TYPE_NPC )
  {
    // コンバートリストからNPCアプリケーションデータを取得
    Field::MoveModel::ApplicationDataForNPC* pNpcData = (Field::MoveModel::ApplicationDataForNPC*)pAppData;
    const gfl2::math::Vector3 offset( (f32)pNpcData->iconOffsetX,(f32)pNpcData->iconOffsetY,(f32)pNpcData->iconOffsetZ );
    iconOffset = offset;
  }
  // pokemon
  else if( pAppData->type == Field::MoveModel::TYPE_POKEMON )
  {
    // コンバートリストからNPCアプリケーションデータを取得
    Field::MoveModel::ApplicationDataForPokemon* pPokemonData = (Field::MoveModel::ApplicationDataForPokemon*)pAppData;
    const gfl2::math::Vector3 offset( (f32)pPokemonData->iconOffsetX,(f32)pPokemonData->iconOffsetY,(f32)pPokemonData->iconOffsetZ );
    iconOffset = offset;
  }

  return iconOffset;

}

GFL_NAMESPACE_END( ActionButton );
GFL_NAMESPACE_END( Field );