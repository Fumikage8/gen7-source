/**
* @brief  ポケモンふれあい用関数
* @file   FieldPokemonFureaiTool.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiTool.h"
#include <prog/GameSys/include/GameManager.h>
#include <prog/GameSys/include/GameData.h>
#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFureaiIconBase.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiDataAccessor.h"

#include "Sound/include/Sound.h"

#include "niji_conv_header/field/flagwork/WorkDefine.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"


#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( PokemonFureai );

const u32 NICKNAME_KNOW_BIT = 0x01 << 8;
const u32 NADENADE_END_BIT = 0x01 << 9;

s32 GetFureaiNo(Field::MoveModel::FieldMoveModel* pFieldMoveModel)
{
  // 個別データ
	Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pFieldMoveModel->GetActorUserWork());
  const FieldContactPokemonAccessor::ContactPokemonData* pContactPokemonData = pFieldMoveModelActor->GetContactPokemonData();
	return pContactPokemonData->fureaiNo;
}

s32 GetSaveWorkIndex(s32 fureai_no)
{
	if (fureai_no >= FPF_FUREAI_NO_NUM)
	{
		GFL_ASSERT( 0 );
		return WK_EVENT_FUREAI_01;
	}

  u16 workIndex = WK_EVENT_FUREAI_01 + fureai_no;
  return workIndex;
}

s32 GetTimeFlagIndex(s32 fureai_no)
{
	if (fureai_no >= FPF_FUREAI_NO_NUM)
	{
		GFL_ASSERT(0);
		return TMFLG_FUREAI_00;
	}


	u16 workIndex = TMFLG_FUREAI_00 + fureai_no;
  return workIndex;
}

s32 GetPoint(const FieldContactPokemonAccessor::ContactPokemonData* pData)
{
  u16 workIndex = GetSaveWorkIndex( pData->fureaiNo );
  
  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  u16 value = svEventWork->GetEventWork( workIndex ); 
  return ( 0x00FF & value);
}

s32 GetRank(Field::MoveModel::FieldMoveModel* pFieldMoveModel)
{
  FieldMoveModelActor* pPokemonActor(
    reinterpret_cast<Field::FieldMoveModelActor*>(pFieldMoveModel->GetActorUserWork()));

  s32 point = GetPoint( pPokemonActor->GetContactPokemonData() );
  if (point == 0)
  {
    return FPF_RANK_UNKNOWN;
  }
  else if (point < pPokemonActor->GetFureaiPokemonSettingData()->fureaiPointBorder )
  {
    return FPF_RANK_LIKE;
  }
  else
  {
    return FPF_RANK_LOVE;
  }
}

s32 AddPoint(const FieldContactPokemonAccessor::ContactPokemonData* pData, s32 add_value )
{
  u16 point = GetPoint( pData ); 
  point += add_value;
  if (point > 250)
  {
    point = 250;
  }

  SetPoint( pData->fureaiNo, point );
  return 0;
}

s32 AddPointByFieldIcon(Field::MoveModel::FieldMoveModel* pFieldMoveModel, s32 iconType)
{
	switch (iconType)
	{
	// case Effect::EFFECT_TYPE_FUREAI_ICON_HEART1:
	case Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1: AddPoint(pFieldMoveModel, FPF_ADD_POINT_FIELD_01); break;
	case Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1: AddPoint(pFieldMoveModel, FPF_ADD_POINT_FIELD_02); break;
	}

	return 0;
}

s32 AddPoint(Field::MoveModel::FieldMoveModel* pFieldMoveModel , s32 add_value)
{
  FieldMoveModelActor* pPokemonActor(
    reinterpret_cast<Field::FieldMoveModelActor*>(pFieldMoveModel->GetActorUserWork()));
  return AddPoint( pPokemonActor->GetContactPokemonData(), add_value ); 
}

void SetPoint(s32 fureai_no, b32 point)
{
  u16 workIndex = GetSaveWorkIndex( fureai_no );
  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  u16 value = svEventWork->GetEventWork( workIndex ); 
  value = ( value & 0xFF00 );
  value = ( value | point);
  svEventWork->SetEventWork( workIndex, value );
}

b32 IsAddPoint(const FieldContactPokemonAccessor::ContactPokemonData* pData)
{
  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  return !svEventWork->CheckEventFlag( pData->reserve1 );
}

// なでなで
b32 IsNadeNadeRequestEventEnd(s32 fureai_no)
{
  u16 workIndex = GetSaveWorkIndex(fureai_no);

  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  u16 value = svEventWork->GetEventWork(workIndex);
  return (value & NADENADE_END_BIT);
}

void SetNadeNadeRequestEventEnd(s32 fureai_no, b32 flag)
{
  u16 workIndex = GetSaveWorkIndex(fureai_no);
  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  u16 value = svEventWork->GetEventWork(workIndex);

  if (flag)
  {
    value |= NADENADE_END_BIT;
    svEventWork->SetEventWork(workIndex, value);
  }
  else
  {
    value &= ~NADENADE_END_BIT;
    svEventWork->SetEventWork(workIndex, value);
  }
}

// 睡眠
void SetSleepEnd(s32 fureai_no, b32 flag)
{
	s32 index = GetTimeFlagIndex( fureai_no );
	Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();

	if (flag)
	{
		svEventWork->SetEventFlag(index);
	}
	else
	{
		svEventWork->ResetEventFlag(index);
	}
}

b32 IsSleepEnd(s32 fureai_no)
{
	s32 index = GetTimeFlagIndex(fureai_no);
	Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
	return svEventWork->CheckEventFlag(index);
}

b32 IsNickNameKnow(const FieldContactPokemonAccessor::ContactPokemonData* pData)
{
  u16 workIndex = GetSaveWorkIndex( pData->fureaiNo );
  return IsNickNameKnow( pData->fureaiNo );
}

void SetNickNameKnow(const FieldContactPokemonAccessor::ContactPokemonData* pData, b32 flag)
{
  u16 workIndex = GetSaveWorkIndex( pData->fureaiNo );
  SetNickNameKnow( pData->fureaiNo, flag );
}

b32 IsNickNameKnow(s32 fureai_no)
{
  u16 workIndex = GetSaveWorkIndex( fureai_no );

  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  u16 value = svEventWork->GetEventWork(workIndex);
  return (value & NICKNAME_KNOW_BIT);
}

void SetNickNameKnow(s32 fureai_no, b32 flag)
{
  u16 workIndex = GetSaveWorkIndex( fureai_no );
  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  u16 value = svEventWork->GetEventWork( workIndex );

  if (flag)
  {
    value |= NICKNAME_KNOW_BIT;
    svEventWork->SetEventWork( workIndex, value);
  }
  else
  {
    value &= ~NICKNAME_KNOW_BIT;
    svEventWork->SetEventWork( workIndex, value);
  }
}

static const u32 FUREAI_SE_CONTROL_ID( 3 );

Field::Effect::IEffectBase* GetEffect(Field::MoveModel::FieldMoveModel* pFieldMoveModel)
{
  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  
  // 今でているアイコンを検索
  for (u32 i = 0; i < Field::Effect::EffectManager::MAX_EFFECT_NUM; ++i)
  {
    // @note ふれあいアイコンは同時に1個しか存在しない
    Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(i);
    if (pEffect && pEffect->IsExec() && pEffect->IsSameParent(pFieldMoveModel))
    {
      switch (pEffect->GetType())
      {
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2:
      case Effect::EFFECT_TYPE_FUREAI_ICON_HEART1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_TEN1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1:
      {
        return pEffect;
      }
      }
    }
  }

  return NULL;

}


b32 CloseEffect( Field::MoveModel::FieldMoveModel* pFieldMoveModel )
{
  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  // 今でているアイコンをclose
  for (u32 i = 0; i < Field::Effect::EffectManager::MAX_EFFECT_NUM; ++i)
  {
    Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(i);
    if (pEffect && pEffect->IsExec() && pEffect->IsSameParent(pFieldMoveModel))
    {
      switch (pEffect->GetType())
      {
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2:
      case Effect::EFFECT_TYPE_FUREAI_ICON_HEART1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_TEN1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1:
      {
        Field::Effect::EffectFureaiIconBase *pIconEffct = reinterpret_cast<Field::Effect::EffectFureaiIconBase*>(pEffect);
        pIconEffct->RequestClose();
        return true;
      }
      }
    }
  }

  return false;
}

b32 CreateEffect(Field::Effect::Type in_eType, Field::MoveModel::FieldMoveModel* pFieldMoveModel, bool in_bPlaySe, b32 voice_play, b32 is_keep, s32* pDataIndex)
{
  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();

  // 今でているアイコンをclose
  for (u32 i = 0; i < Field::Effect::EffectManager::MAX_EFFECT_NUM; ++i)
  {
    Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(i);
    if (pEffect && pEffect->IsExec() && pEffect->IsSameParent(pFieldMoveModel))
    {
      switch (pEffect->GetType())
      {
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2:
      case Effect::EFFECT_TYPE_FUREAI_ICON_HEART1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_TEN1:
      case Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1:
      {
        Field::Effect::EffectFureaiIconBase *pIconEffct = reinterpret_cast<Field::Effect::EffectFureaiIconBase*>(pEffect);
        pIconEffct->RequestClose();
        break;
      }
      }
    }
  }

  if (in_bPlaySe)
  {
    u32 controlID = pFieldMoveModel->GetEventId();

    switch (in_eType)
    {
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1: Sound::PlaySE(SEQ_SE_FLD_0428, 0, controlID ); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2: Sound::PlaySE(SEQ_SE_FLD_0428, 0, controlID ); break; // @note 確認
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_HEART1: Sound::PlaySE(SEQ_SE_FLD_0430, 0, controlID ); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1: Sound::PlaySE(SEQ_SE_FLD_0443, 0, controlID ); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1: Sound::PlaySE(SEQ_SE_FLD_215, 0, controlID ); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1: Sound::PlaySE(SEQ_SE_FLD_0429, 0, controlID ); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_TEN1: Sound::PlaySE(SEQ_SE_FLD_0427, 0, controlID ); break;
    case Field::Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1: Sound::PlaySE(SEQ_SE_FLD_0431, 0, controlID ); break;
    }

    in_bPlaySe = false;
  }

  // in_bPlaySe = false;
  Field::Effect::EffectFureaiIconBase *pIconEffct = reinterpret_cast<Field::Effect::EffectFureaiIconBase*>(pEffectManager->CreateEffect(in_eType, pFieldMoveModel, in_bPlaySe, Field::Effect::EffectManager::WORK_TYPE_SYS, pDataIndex));
  if (pIconEffct)
  {
    FieldMoveModelActor* pPokemonActor(
      reinterpret_cast<Field::FieldMoveModelActor*>(pFieldMoveModel->GetActorUserWork()));
    pIconEffct->SetScale(pPokemonActor->GetFureaiPokemonSettingData()->icon_scale);
  }

  if (voice_play)
  {
    switch (in_eType)
    {
    case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1: PlayVoice(pFieldMoveModel, FPF_VOICE_PLAY_ICON_EX1 ); break;
    case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2: PlayVoice(pFieldMoveModel, FPF_VOICE_PLAY_ICON_EX2 ); break;
    case Effect::EFFECT_TYPE_FUREAI_ICON_HEART1: PlayVoice(pFieldMoveModel, FPF_VOICE_PLAY_ICON_HEART ); break;
    case Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1: PlayVoice(pFieldMoveModel, FPF_VOICE_PLAY_ICON_NOTE ); break;
    case Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1: PlayVoice(pFieldMoveModel, FPF_VOICE_PLAY_ICON_Q ); break;
    case Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1: PlayVoice(pFieldMoveModel, FPF_VOICE_PLAY_ICON_SMILE ); break;
    case Effect::EFFECT_TYPE_FUREAI_ICON_TEN1: PlayVoice(pFieldMoveModel, FPF_VOICE_PLAY_ICON_TEN ); break;
    case Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1: PlayVoice(pFieldMoveModel, FPF_VOICE_PLAY_ICON_ZZ ); break;
    }
  }

  if (pIconEffct && is_keep)
  {
    pIconEffct->SetMode(  Field::Effect::EffectFureaiIconBase::MODE_KEEP );
  }

  return ( pIconEffct != NULL);
}

b32 HaveEffect(Field::MoveModel::FieldMoveModel* pFieldMoveModel)
{
	Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();

	// 今でているアイコンをclose
	for (u32 i = 0; i < Field::Effect::EffectManager::MAX_EFFECT_NUM; ++i)
	{
		Field::Effect::IEffectBase *pEffect = pEffectManager->GetEffect(i);
		if (pEffect && pEffect->IsExec() && pEffect->IsSameParent(pFieldMoveModel))
		{
			switch (pEffect->GetType())
			{
			case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2:
			case Effect::EFFECT_TYPE_FUREAI_ICON_HEART1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_TEN1:
			case Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1:
			{
				return true;
			}
			}
		}
	}

	return false;
}


b32 PlayVoice(Field::MoveModel::FieldMoveModel* pFieldMoveModel, s32 type)
{
  if (type == FPF_VOICE_TYPE_NONE)
  {
    return false;
  }

  s8 slot = 0;
  if (type == FPF_VOICE_TYPE_NORMAL)
  {
    Field::MoveModel::ApplicationDataForCommon* pAppData = (Field::MoveModel::ApplicationDataForCommon*)GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetMoveModelManager()->GetApplicationData(
      pFieldMoveModel->GetCharacterId());
    if (pAppData && pAppData->type == Field::MoveModel::TYPE_POKEMON)
    {
			// @fix cov_ctr[12628]:ポケモンでない場合はボイス再生コードに入らないように修正
      Field::MoveModel::ApplicationDataForPokemon* pNpcData = (Field::MoveModel::ApplicationDataForPokemon*)pAppData;
      s32 monsNo = pNpcData->monsNo;
			f32 volume = 1.0f;
			f32 pan = 0.0f;
			f32 pitch = 1.0f;
			Sound::VoiceType voiceType = Sound::VOICE_TYPE_DEFAULT;
      Sound::PlayVoice(slot, static_cast<MonsNo>(monsNo), static_cast<FormNo>(0), voiceType, false, pFieldMoveModel->GetEventId() );
			Sound::ChangeVoiceVolume(slot, volume);
			Sound::ChangeVoicePan(slot, pan);
			Sound::ChangeVoicePitch(slot, pitch);
    }
		else
		{
			GFL_ASSERT( 0 );
		}
  }
  else
  {
    // ふれあい設定を参照
    Field::FieldResident* pFieldResident = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident();
    Field::PokemonFureai::DataAccessor dataAccessor(pFieldResident->GetResource(BL_IDX_FIELD_RESIDENT__POKEMON_FUREAI_BIN));
    f32 pitch = 1.0f;
    
    // ピカチュウ @fix MMcat[619]:ピカチュウを撫でた際に不自然な鳴き声が再生される対処。ピカチュウはピッチを弄らず再生ラベルを変更
    if (pFieldMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0025_00 || pFieldMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0025_01)
    {
      VoiceID voiceId = VOICE_PV001;

      if (type == FPF_VOICE_TYPE_SHORT_02 )
      {
        voiceId = VOICE_PV_PIKA_KW_05;
      }
      else if (type == FPF_VOICE_TYPE_YOROKOBI_02)
      {
        if (pFieldMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0025_00)
        {
          voiceId = VOICE_PV_PIKA_KW_09;
        }
        else
        {
          voiceId = VOICE_PV_PIKA_KW_10;
        }
      }
      else
      {
        voiceId = static_cast<VoiceID>(dataAccessor.GetVoiceID(pFieldMoveModel->GetCharacterId(), type));
      }

      Sound::PlayVoice(slot, voiceId, false, pFieldMoveModel->GetEventId());
      Sound::ChangeVoicePitch(slot, pitch);
    }
    // ピカチュウ以外
    else
    {
      if (type == FPF_VOICE_TYPE_YOROKOBI_02)
      {
        type = FPF_VOICE_TYPE_YOROKOBI;
        pitch = 0.5f;
      }

      if (type == FPF_VOICE_TYPE_SHORT_02)
      {
        type = FPF_VOICE_TYPE_SHORT;
        pitch = 1.1f;
      }

      VoiceID voiceId = static_cast<VoiceID>(dataAccessor.GetVoiceID(pFieldMoveModel->GetCharacterId(), type));
      Sound::PlayVoice(slot, voiceId, false, pFieldMoveModel->GetEventId());
      Sound::ChangeVoicePitch(slot, pitch);
    }
  }
  return true;
}

b32 IsStartSleep(Field::MoveModel::FieldMoveModel* pFieldMoveModel)
{
  // 未知の場合は寝ない
  if (GetRank(pFieldMoveModel) == FPF_RANK_UNKNOWN)
  {
    return false;
  }

  // 起こしている
	if (IsSleepEnd(GetFureaiNo(pFieldMoveModel)))
	{
		return false;
	}

  GameSys::GameData* p_gdata = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  s32 timeZone = EvTime::GetAlolaTimeZone( p_gdata );

  FieldMoveModelActor* pPokemonActor(
    reinterpret_cast<Field::FieldMoveModelActor*>(pFieldMoveModel->GetActorUserWork()));

  u32 sleepType = pPokemonActor->GetContactPokemonData()->reserve2;
  switch (sleepType)
  {
  case FPF_SLEEP_TYPE_NIGHT:
  {
    switch (timeZone)
    {
    case GameSys::TimeZone::NIGHT:
    case GameSys::TimeZone::NIGHT_TO_MIDNIGHT:
    case GameSys::TimeZone::MIDNIGHT:
    {
      return true;
    }
    }
    break;
  }

  case FPF_SLEEP_TYPE_NOON:
  {
    switch (timeZone)
    {
    case GameSys::TimeZone::MORNING:
    case GameSys::TimeZone::MORNING_TO_NOON:
    case GameSys::TimeZone::NOON:
    {
      return true;
    }
    }
    break;
  }
  }
  return false;
}

// 親から子を取得(いない場合はNULL)
MoveModel::FieldMoveModel* GetChildPokemon(MoveModel::FieldMoveModel* pParent)
{
  Field::PlacementDataManager* pPlacementDataManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident()->GetPlacementDataManager();
  
  // サーチ
  for (u32 i = 0; i < PlacementDataManager::MOVEMODEL_EVENT_ACTOR_MAX; i++)
  {
    FieldMoveModelActor* pWorkActor(
      reinterpret_cast<Field::FieldMoveModelActor*>(pPlacementDataManager->GetMoveModelActor(i)));

    if (pWorkActor == NULL || !pWorkActor->IsSetupMoveModel()) // @note IsRegistでは、非表示状態の動作モデルが省かれてしまう
    {
      continue;
    }

    if (pWorkActor->IsContactPokemon() )
    {
      if (pParent->GetEventId() == pWorkActor->GetContactPokemonData()->npcEventID)
      {
        return pWorkActor->GetMoveModel();
      }
    }
  }

  return NULL;
}

// 子から親を取得(いない場合はNULL)
MoveModel::FieldMoveModel* GetParentNpc(MoveModel::FieldMoveModel* pPokemon)
{
  Field::PlacementDataManager* pPlacementDataManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident()->GetPlacementDataManager();
  FieldMoveModelActor* pPokemonActor(
    reinterpret_cast<Field::FieldMoveModelActor*>( pPokemon->GetActorUserWork() ));

  // サーチ
  for (u32 i = 0; i < PlacementDataManager::MOVEMODEL_EVENT_ACTOR_MAX; i++)
  {
    FieldMoveModelActor* pWorkActor(
      reinterpret_cast<Field::FieldMoveModelActor*>(pPlacementDataManager->GetMoveModelActor(i)));

    if (pWorkActor == NULL || !pWorkActor->IsSetupMoveModel()) // @note IsRegistでは、非表示状態の動作モデルが省かれてしまう
    {
      continue;
    }

    if ( pPokemonActor->GetContactPokemonData()->npcEventID == pWorkActor->GetMoveModel()->GetEventId() )
    {
      return pWorkActor->GetMoveModel();
    }
  }

  return NULL;
}


static const f32 PAN_CONTROL_START_COS = 10.0f;
static const f32 PAN_CONTROL_END_COS = 170.0f;
static const f32 PAN_CONTROL_POWER = 0.90f;

/**
*  @brief  cosの値から、パンの数値を決定する。
*/
f32 CalcCosToPan(f32 cos)
{
  f32 start_cos = gfl2::math::CosDeg(PAN_CONTROL_START_COS);
  f32 end_cos = gfl2::math::CosDeg(PAN_CONTROL_END_COS);
  f32 length_max = gfl2::math::Abs(end_cos - start_cos);

  if ((start_cos >= cos) && (end_cos <= cos))
  {
    f32 cos_len = gfl2::math::Abs(cos - start_cos);
    return gfl2::math::SinDeg((cos_len / length_max) * 180.0f);
  }
  else
  {
    return 0.0f;
  }
}

// 指定ポケモンの現在の音量、水平パンを取得する
void GetSoundParam(MoveModel::FieldMoveModel* pMoveModel, f32* pVolume, f32* pPan)
{
  // 主人公
  Field::MoveModel::FieldMoveModelPlayer* pMoveModelPlayer = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetPlayerCharacter();
  gfl2::math::Vector3 playerPos = pMoveModelPlayer->GetLocalSRT().translate;


  // カメラ位置
  //gfl2::math::Vector3  controlPos = m_playerPosition;
  //gfl2::math::Quaternion controlRotate = m_playerQuaternion;
  gfl2::math::Quaternion rotate_camera = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetCameraManager()->GetMainViewCamera()->GetBaseCamera()->GetRotationQuat();
  gfl2::math::Vector3 pokemonPos = pMoveModel->GetLocalSRT().translate;

  // Y軸180度回転
  gfl2::math::Quaternion rotate = rotate_camera * gfl2::math::Quaternion(gfl2::math::Vector3(0, 1.0f, 0), gfl2::math::ConvDegToRad(180.0f));

  gfl2::math::Vector3 distance = pokemonPos - playerPos;


  // 中心方向のベクトルと、プレイヤーかカメラ向きの角度と外積を求める。
  // どちらも平面で行なう。
  gfl2::math::Vector3 player_rot(0.0f, 0.0f, 1.0f);
  gfl2::math::Matrix34 rotMtx;
  rotate.QuaternionToMatrix(rotMtx);
  player_rot = rotMtx * player_rot;
  player_rot.SetY(0.0f);
  if (player_rot.LengthSq() > 0.0f) // Y軸に値が入っているか。
  {
    player_rot = player_rot.Normalize();
  }
  else
  {
    player_rot.Set(0, 0, 1.0f);
  }

  f32 dir_cos;
  f32 dir_cross_y;
  f32 length = distance.Length();
  if (length > 0.001f)
  {
    dir_cos = distance.Dot(player_rot);
    dir_cos /= length;
    gfl2::math::Vector3 cross = distance.Cross(player_rot);
    dir_cross_y = cross.GetY();
  }
  else
  {
    // プレイヤーと音源位置が一緒
    dir_cos = 1.0f;
    dir_cross_y = 1.0f;
  }

  // パンの値を求める。
  f32 pan;
  if (dir_cross_y > 0.0f)
  {
    pan = CalcCosToPan(dir_cos) * PAN_CONTROL_POWER;
  }
  else
  {
    pan = -CalcCosToPan(dir_cos) * PAN_CONTROL_POWER;
  }

  pan = gfl2::math::Clamp(pan, -1.0f, 1.0f);

  // ボリューム
  Field::MoveModel::FieldMoveCodeFureaiTypeA::SETTING* pSetting = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetSettingData();

  // 厚みを取得
  f32 playerBodyWidth = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetBodyWidth(pMoveModelPlayer, true);
  f32 npcBodyWidth = Field::MoveModel::FieldMoveCodeFureaiTypeA::GetBodyWidth(pMoveModel, false);
  
  f32 min_volume = pSetting->sound.min_volume;
  f32 max_volume = pSetting->sound.max_volume;
  f32 min_length = pSetting->sound.min_length;
  f32 max_length = pSetting->sound.max_length;
  length = length - playerBodyWidth - npcBodyWidth;

  f32 volume = 1.0f;
  if (length >= min_length)
  {
    volume = min_volume;
  }
  else if (length <= max_length)
  {
    volume = max_volume;
  }
  else
  {
    f32 per = (length - max_length) / (min_length - max_length);
    per = gfl2::math::Easing::GetValue(static_cast<gfl2::math::Easing::EaseFunc>(pSetting->sound.ease_func), per);
    volume = max_volume + (min_volume - max_volume ) * per;
  }

  volume = gfl2::math::Clamp(volume, min_volume, max_volume);

  *pVolume = volume;
  *pPan = pan;
}

void VoiceUpdate(void)
{
  // 監視対象SE
  static const u32 sc_FureaiSETable[] =
  {
    SEQ_SE_FLD_0428,
    SEQ_SE_FLD_0430,
    SEQ_SE_FLD_0443,
    SEQ_SE_FLD_215,
    SEQ_SE_FLD_0429,
    SEQ_SE_FLD_0427,
    SEQ_SE_FLD_0431
  };

  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetMoveModelManager();

  for (u32 i = 0; i<Field::MoveModel::FIELD_MOVE_MODEL_MAX; i++)
  {
    Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel(i);
    if (pMoveModel->IsSetupWork() && pMoveModel->IsSetupResource())
    {
      if (pMoveModel->GetMoveCodeId() != MoveModel::FIELD_MOVE_CODE_FUREAI_A)
      {
        continue;
      }

      f32 pan = 1.0f;
      f32 volume = 1.0f;
      GetSoundParam(pMoveModel, &volume, &pan);

      // SE
      for (u32 i = 0; i < GFL_NELEMS(sc_FureaiSETable); i++)
      {
        if (Sound::IsSEPlaying(sc_FureaiSETable[i], pMoveModel->GetEventId()))
        {
          Sound::ChangeSEVolume(sc_FureaiSETable[i], volume, 0, pMoveModel->GetEventId());
          Sound::ChangeSEPan(sc_FureaiSETable[i], pan, pMoveModel->GetEventId());
        }
      }

      // ボイス @note 現状はスロット0だけ
      for (u32 i = 0; i < 1; i++ )
      {
        if (!Sound::IsVoiceFinished(i) && Sound::GetVoiceUserId(i) == pMoveModel->GetEventId() )
        {
          Sound::ChangeVoicePan(i, pan);
          Sound::ChangeVoiceVolume(i, volume);
        }
      }
    }
  }
}

GFL_NAMESPACE_END( PokemonFureai );
GFL_NAMESPACE_END( Field );