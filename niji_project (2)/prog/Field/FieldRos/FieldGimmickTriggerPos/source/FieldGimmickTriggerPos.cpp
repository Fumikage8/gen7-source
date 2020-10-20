//=================================================================================================
/**
*  @file FieldGimmickTriggerPos.cpp
*  @brief フィールドギミック 電気試練
*  @author ikuta_junya
*  @date 2016.02.17
*/
//=================================================================================================

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

// field
#include "Field/FieldRos/FieldGimmickTriggerPos/include/FieldGimmickTriggerPos.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "arc_def_index/arc_def.h"
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"

#include "FieldScript/include/EventScriptCall.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"

// conv_header
#include <niji_conv_header/field/static_model/static_model_id.h>
#include <niji_conv_header/field/script/inc/field_event.inc>

// reference
//#include <niji_reference_files/script/FieldPawnTypes.h>


// @todo ヒープ分割を行う

GFL_NAMESPACE_BEGIN(Field)

struct FieldGimmickTriggerPos::EVENT_DATA
{
  b32 isUse;
  b32 isActive;
  b32 isHold;
  b32 isTrigger;
  gfl2::math::Vector3 pos;
  f32                 r;
  s32 eventID;
  s32 scriptID;
};

/**
 *  @brief コンストラクタ
 *  @param pHeap ヒープ
 *  @param pFieldmap フィールドマップ
 */
FieldGimmickTriggerPos::FieldGimmickTriggerPos(gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap)
: FieldGimmick(pFieldmap), m_InitializeState(0), m_FinalizeState(0), m_pFieldDeviceHeap(pHeap), m_pEventDataTable(NULL), m_EventDataTableSize( 0 )
{
}

/**
 *  @brief デストラクタ
 */
FieldGimmickTriggerPos::~FieldGimmickTriggerPos()
{
}

/**
 *  @brief  生成
 */
void FieldGimmickTriggerPos::Create(void)
{
  m_InitializeState = 0;

}
void FieldGimmickTriggerPos::UpdateCreate(void)
{
  switch (m_InitializeState)
  {
  case 0:
  {
    m_pEventDataTable = GFL_NEW(m_pFieldDeviceHeap) EVENT_DATA[24];
    m_EventDataTableSize = 24;

    for (u32 i = 0; i < m_EventDataTableSize; i++)
    {
      m_pEventDataTable[i].isUse = false;
    }

    s32 dataCount = 0;
    Field::StaticModel::StaticModelManager* pStaticModelManager = m_pFieldmap->GetStaticModelManager();
    for (s32 i = 0; i < StaticModel::StaticModelManager::STATIC_MODEL_MAX; ++i)
    {
      StaticModel::StaticModel* pStaticModel = pStaticModelManager->GetStaticModel(i);

      if (pStaticModel && pStaticModel->IsEmpty() == false)
      {
        switch (pStaticModel->GetStaticModelID())
        {
        case STATIC_MODEL_ID_W_F0801D0101_BM_WARP01:
        case STATIC_MODEL_ID_W_F0801D0101_BM_WARP02:
        case STATIC_MODEL_ID_W_F0801D0101_BM_SWITCH_R:
        case STATIC_MODEL_ID_W_F0801D0101_BM_SWITCH_G:
        case STATIC_MODEL_ID_W_F0801D0101_BM_SWITCH_B:
        case STATIC_MODEL_ID_W_F0801D0101_BM_SWITCH_Y:
        {
          m_pEventDataTable[dataCount].isUse = true;
          m_pEventDataTable[dataCount].isActive = true;
          m_pEventDataTable[dataCount].eventID = pStaticModel->GetEventID();
          m_pEventDataTable[dataCount].scriptID = SCRID_FLD_TRIGGER_POS;
          m_pEventDataTable[dataCount].pos = pStaticModel->GetLocalSRT().translate;
          m_pEventDataTable[dataCount].r = 80.0f;
          m_pEventDataTable[dataCount].isHold = false;
          m_pEventDataTable[dataCount].isTrigger = false;
          dataCount++;
          break;
        }
        }
      }
    }

    m_InitializeState = 1;
  } // case 0
  }
}
bool FieldGimmickTriggerPos::IsCreated(void)
{
  return ( m_InitializeState == 1);
}

/**
*  @brief  Ready中1度だけ呼び出し
*/
void FieldGimmickTriggerPos::Ready(void)
{
  // ヒットチェック
  Field::MoveModel::FieldMoveModelPlayer* pPlayer(m_pFieldmap->GetPlayerCharacter());
  gfl2::math::Vector3 pos = pPlayer->GetLocalSRT().translate;
  HitCheck( true, pos);
}

/**
 *  @biref 破棄更新
 *  @return 破棄が完了したかどうか
 *
 *  破棄が完了するまでマイフレーム呼出がなされる関数
 */
bool FieldGimmickTriggerPos::Finalize(void)
{
	// @fix cov_ctr[12582]:不要ケース削除
  switch(m_FinalizeState)
  {
    case 0:
    {
      m_FinalizeState = 1;
      GFL_SAFE_DELETE_ARRAY(m_pEventDataTable);
      return true;
    }
    case 1:
    {
		  return true;
    }
  }

  return false;
}

/**
 *  @brief 更新
 *  Fieldmap::Updateにて呼出
 *  UpdateNormalが終了してから呼び出される
 */
void FieldGimmickTriggerPos::UpdateLast(void)
{
  // イベント中なら処理を中断
  Field::MoveModel::FieldMoveModelPlayer* pPlayer(m_pFieldmap->GetPlayerCharacter());
  if (!pPlayer)
  {
    for (u32 i = 0; i < m_EventDataTableSize; i++)
    {
      EVENT_DATA* pData = &m_pEventDataTable[i];
      if (pData->isUse)
      {
        pData->isHold = false;
        pData->isTrigger = false;
        continue;
      }
    }
    return;
  }

  // ヒットチェック
  gfl2::math::Vector3 pos = pPlayer->GetLocalSRT().translate;
  HitCheck( ( pPlayer->GetEventRequestCnt() != 0 ),  pos );
}

/**
 *  @brief  イベント起動チェック
 */
bool FieldGimmickTriggerPos::EventRequestCheck(const EventCheckRequest *cpReq)
{
  // フィールドがないなら処理を中断
  if (!m_pFieldmap)
  {
    return false;
  }

  // イベント中なら処理を中断
  Field::MoveModel::FieldMoveModelPlayer* pPlayer(m_pFieldmap->GetPlayerCharacter());
  if (!pPlayer || pPlayer->GetEventRequestCnt() )
  {
    return false;
  }


  for (u32 i = 0; i < m_EventDataTableSize; i++)
  {
    EVENT_DATA* pData = &m_pEventDataTable[i];
    if (pData->isUse == false)
    {
      continue;
    }
    
    if (pData->isTrigger && pData->isActive )
    {
      Field::EventScriptCall::CallScript( cpReq->p_gameman,  pData->scriptID, NULL, NULL, pData->eventID );
      return true;
    }
  }
  
  return false;
}

/**
 *	@brief  トリガーPOSの有効フラグを設定。
 *  @note   トリガーPOSギミックが適用されていないマップで使用すると、アサート
  *	@note   対象のイベントが保持している、当たり判定情報を必ずクリアします。対象POSが反応する状態で実行しないでください。
 *
 *	@param	event_id イベントID
 *  @param  flag　　 true:有効 false:無効
 */
void FieldGimmickTriggerPos::SetActiveFlag(u32 event_id, b32 flag)
{
  for (u32 i = 0; i < m_EventDataTableSize; i++)
  {
    EVENT_DATA* pData = &m_pEventDataTable[i];
    if (pData->isUse == false)
    {
      continue;
    }

    if (pData->eventID == event_id)
    {
      pData->isActive = flag;
      pData->isHold = false;
      pData->isTrigger = false;
    }
  }
}

void FieldGimmickTriggerPos::HitCheck(b32 isEvent, const gfl2::math::Vector3& rPos)
{
  for (u32 i = 0; i < m_EventDataTableSize; i++)
  {
    EVENT_DATA* pData = &m_pEventDataTable[i];
    if (pData->isUse == false)
    {
      continue;
    }

    if (pData->isActive == false)
    {
      pData->isHold = false;
      pData->isTrigger = false;
      continue;
    }

    f32 distanceSq = (rPos - pData->pos).LengthSq();
    if (distanceSq >= pData->r * pData->r)
    {
      pData->isHold = false;
      pData->isTrigger = false;
      continue;
    }

    // hit
    if (isEvent)
    {
      pData->isHold = true;
      pData->isTrigger = false;
    }
    else
    {
      if (pData->isHold)
      {
        pData->isTrigger = false;
      }
      else
      {
        pData->isTrigger = true;
      }
    }
  }
}

/**
 *  @brief コンストラクタ
 *  @param pHeap ヒープ
 *  @param pFieldmap フィールドマップ
 */
FieldGimmickUb01::FieldGimmickUb01(gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap)
: FieldGimmick(pFieldmap), m_InitializeState(0), m_FinalizeState(0), m_pFieldDeviceHeap(pHeap), m_pFileReadManager(NULL)
{
  m_pAllocator = GFL_NEW(pHeap) System::nijiAllocator(pHeap);
}

/**
 *  @brief デストラクタ
 */
FieldGimmickUb01::~FieldGimmickUb01()
{
  GFL_SAFE_DELETE(m_pAllocator);
}

/**
 *  @brief  生成
 */
void FieldGimmickUb01::Create(void)
{
  // エフェクト
  {
    Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
    pEffectManager->LoadDynamicEffectResource(Field::Effect::EFFECT_TYPE_EF_PH0301_DEN, m_pFieldDeviceHeap );
  }

  m_InitializeState = 0;

}
void FieldGimmickUb01::UpdateCreate(void)
{
  switch (m_InitializeState)
  {
  case 0:
  {
    // エフェクトセットアップ
    {
      Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
      if (!pEffectManager->IsLoadedEffectResource(Field::Effect::EFFECT_TYPE_EF_PH0301_DEN))
      {
        break;
      }
    }

    // エフェクトセットアップ
    {
      Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
      pEffectManager->SetupDynamicEffectResource(Field::Effect::EFFECT_TYPE_EF_PH0301_DEN, m_pFieldDeviceHeap);
    }

    m_InitializeState = 1;
  } // case 0
  }
}
bool FieldGimmickUb01::IsCreated(void)
{
  return ( m_InitializeState == 1);
}

/**
*  @brief  Ready中1度だけ呼び出し
*/
void FieldGimmickUb01::Ready(void)
{
}

/**
 *  @biref 破棄更新
 *  @return 破棄が完了したかどうか
 *
 *  破棄が完了するまでマイフレーム呼出がなされる関数
 */
bool FieldGimmickUb01::Finalize(void)
{
	// @fix cov_ctr[12592]:不要ケース削除
#if 0
  switch(m_FinalizeState)
  {
    case 0:
    {
      m_FinalizeState = 1;
    }
    case 1:
    {
      m_FinalizeState = 2;
    }
   
    case 2:
    {
      return true;
    }
  }
#endif

  return true;
}

/**
 *  @brief 更新
 *  Fieldmap::Updateにて呼出
 *  UpdateNormalが終了してから呼び出される
 */
void FieldGimmickUb01::UpdateLast(void)
{
}


GFL_NAMESPACE_END(Field)