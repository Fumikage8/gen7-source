//==============================================================================
/**
 * @file        ScriptFuncSetSound.cpp
 * @brief       ScriptFuncSetCommon系 サウンド系統
    
 * @author      Miyuki Iwasawa
 * @date        2015/12/23
 */
//==============================================================================

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/SaveData.h"
#include "Sound/include/Sound.h"

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"

#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )

//------------------------------------------------------------------------------
/**
 * @func    SEPlay 
 * @brief   SE再生 
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SEPlay( AMX* amx, const cell* ptr )
{
  u32 soundID = ptr[1];
  f32 volume  = amx_ctof(ptr[2]);
  f32 pan     = amx_ctof(ptr[3]);
  f32 pitch   = amx_ctof(ptr[4]);

  Sound::PlaySE(soundID);
  Sound::ChangeSEVolume(soundID,volume);
  Sound::ChangeSEPan(soundID,pan);
  Sound::ChangeSEPitch(soundID,pitch);

  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func    SEChangeVolume
 * @brief   SE再生 
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SEChangeVolume(AMX* amx, const cell* ptr)
{
  u32 soundID = ptr[1];
  f32 volume = amx_ctof(ptr[2]);
  u32 change_frame = ptr[3];
  Sound::ChangeSEVolume(soundID, volume, change_frame );
  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func    SEIsPlaying
 * @brief   指定SEが再生中か？
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SEIsPlaying( AMX* amx, const cell* ptr )
{
  u32 soundID = ptr[1];
  
  return Sound::IsSEPlaying(soundID);
}
//------------------------------------------------------------------------------
/**
 * @func    SEStop 
 * @brief   SE再生終了
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SEStop( AMX* amx, const cell* ptr )
{
  u32 soundID = ptr[1];
  
  Sound::StopSE(soundID);
  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func    MEPlay 
 * @brief   ME再生
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MEPlay( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  
  u32 soundID = ptr[1];
  p_gman->GetGameData()->GetFieldSound()->PlayME(soundID);
  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func    MEIsPlaying
 * @brief   ME再生中チェック
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MEIsPlaying( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  
  u32 soundID = ptr[1];
  
  bool isMEPlaying = p_gman->GetGameData()->GetFieldSound()->IsMEPlaying(soundID);
  GFL_PRINT("isMEPlaying %d\n",isMEPlaying);
  return isMEPlaying;
}
//------------------------------------------------------------------------------
/**
 * @func    MEReturnBGM
 * @brief   ME再生後、通常BGM再生を再開
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MEReturnBGM( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  u32 soundID = ptr[1];
  
  if( soundID == 0 ){
    p_gman->GetGameData()->GetFieldSound()->RecoverBGMFromME();    
  }else{
    p_gman->GetGameData()->GetFieldSound()->RecoverBGMFromME(soundID);  
  }
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func    BGMChangeNowMap
 * @brief   BGM再生(現在のゾーン)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BGMChangeNowMap( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  Sound::FadeFrame fadeOutFrame = static_cast<Sound::FadeFrame>(ptr[1]);
  Sound::FadeFrame fadeInFrame = static_cast<Sound::FadeFrame>(ptr[2]);

  p_gman->GetGameData()->GetFieldSound()->ChangeFieldBGMNowMap(fadeOutFrame,fadeInFrame);
  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func    BGMStartEvent
 * @brief   イベントBGM再生
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BGMStartEvent( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  u32 soundID = ptr[1];
  Sound::FadeFrame fadeOutFrame = static_cast<Sound::FadeFrame>(ptr[2]);
  Sound::FadeFrame fadeInFrame = static_cast<Sound::FadeFrame>(ptr[3]);

  p_gman->GetGameData()->GetFieldSound()->StartEventBGM(soundID,fadeOutFrame,fadeInFrame);
  
  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func    BGMChangeEvent
 * @brief   イベントBGM切り替え
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BGMChangeEvent( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  u32 soundID = ptr[1];
  Sound::FadeFrame fadeOutFrame = static_cast<Sound::FadeFrame>(ptr[2]);
  Sound::FadeFrame fadeInFrame = static_cast<Sound::FadeFrame>(ptr[3]);

  p_gman->GetGameData()->GetFieldSound()->ChangeEventBGM(soundID,fadeOutFrame,fadeInFrame);
  
  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func    BGMEndEvent
 * @brief   イベントBGM再生終了
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BGMEndEvent( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  Sound::FadeFrame fadeOutFrame = static_cast<Sound::FadeFrame>(ptr[1]);
  Sound::FadeFrame fadeInFrame = static_cast<Sound::FadeFrame>(ptr[2]);
  
  p_gman->GetGameData()->GetFieldSound()->EndEventBGM(fadeOutFrame,fadeInFrame);
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func    BGMStartBattleEvent
 * @brief   バトルイベントBGM再生
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BGMStartBattleEvent( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  u32 soundID = ptr[1];
  Sound::FadeFrame fadeOutFrame = static_cast<Sound::FadeFrame>(ptr[2]);
  Sound::FadeFrame fadeInFrame = static_cast<Sound::FadeFrame>(ptr[3]);

  p_gman->GetGameData()->GetFieldSound()->StartBattleBGM(soundID,fadeOutFrame,fadeInFrame);
  
  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func    BGMChangeBattleEvent
 * @brief   バトルイベントBGM切り替え
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BGMChangeBattleEvent( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  u32 soundID = ptr[1];
  Sound::FadeFrame fadeOutFrame = static_cast<Sound::FadeFrame>(ptr[2]);
  Sound::FadeFrame fadeInFrame = static_cast<Sound::FadeFrame>(ptr[3]);

  p_gman->GetGameData()->GetFieldSound()->ChangeBattleBGM(soundID,fadeOutFrame,fadeInFrame);
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func    BGMIsPlaying
 * @brief   BGMの再生中チェック
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BGMIsPlaying( AMX* amx, const cell* ptr )
{
  u32 soundID = ptr[1];

  if( soundID == 0 ){
    return Sound::IsBGMPlaying();
  }else{
    return Sound::IsBGMPlaying(soundID);  
  }
  
}
//------------------------------------------------------------------------------
/**
 * @func    BGMVolumeChange
 * @brief   BGMボリューム変更
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BGMVolumeChange( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  
  f32 volume = amx_ctof( ptr[1] );
  Sound::FadeFrame fadeFrame = static_cast<Sound::FadeFrame>(ptr[2]);

  p_gman->GetGameData()->GetFieldSound()->ChangeBGMVolume(volume,fadeFrame);

  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func    BGMVolumeChange
 * @brief   BGMボリュームを元に戻す
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BGMVolumeRecover( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  Sound::FadeFrame fadeFrame = static_cast<Sound::FadeFrame>(ptr[1]);

  p_gman->GetGameData()->GetFieldSound()->ChangeBGMVolume(1.0f,fadeFrame);

  return NULL;
}

cell AMX_NATIVE_CALL FuncSetCommon::BGMPause( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  
  Sound::FadeFrame fadeFrame = static_cast<Sound::FadeFrame>(ptr[1]);
  Sound::FadeOutBGMReq( fadeFrame );
  return NULL;
}
cell AMX_NATIVE_CALL FuncSetCommon::BGMPauseRelease( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  Sound::FadeFrame fadeFrame = static_cast<Sound::FadeFrame>(ptr[1]);
  Sound::FadeInBGMReq( fadeFrame );
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func    VoicePlay 
 * @brief   ポケモンボイス再生(再生スロット0固定)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::VoicePlay_( AMX* amx, const cell* ptr )
{
  u32 monsno  = ptr[1];
  u32 formno  = ptr[2];
  u32 type    = ptr[3];
  f32 volume  = amx_ctof(ptr[4]);
  f32 pan     = amx_ctof(ptr[5]);
  f32 pitch   = amx_ctof(ptr[6]);
  u8  slot    = ptr[7];

  Sound::VoiceType voiceType = Sound::VOICE_TYPE_DEFAULT;

  switch( type ){
  case SCR_PM_VOICE_TYPE_NORMAL:
    voiceType =  Sound::VOICE_TYPE_DEFAULT;
    break;
  case SCR_PM_VOICE_TYPE_BATTLE:
    voiceType =  Sound::VOICE_TYPE_BATTLE;
    break;
  case SCR_PM_VOICE_TYPE_DOWN:
    voiceType =  Sound::VOICE_TYPE_DOWN;
    break;
  case SCR_PM_VOICE_TYPE_SPECIAL:
    voiceType =  Sound::VOICE_TYPE_SP00;
    break;
  default:
    break;
  }

  //スロットの有効数はライブラリ側でケアしている
  Sound::PlayVoice( slot, static_cast<MonsNo>(monsno), static_cast<FormNo>(formno), voiceType, false );
  Sound::ChangeVoiceVolume( slot, volume );
  Sound::ChangeVoicePan( slot, pan );
  Sound::ChangeVoicePitch( slot, pitch );

  return NULL;
}
cell AMX_NATIVE_CALL FuncSetCommon::VoicePlay( AMX* amx, const cell* ptr )
{
  u32 monsno  = ptr[1];
  u32 formno  = ptr[2];
  u32 type    = ptr[3];
  f32 volume  = amx_ctof(ptr[4]);
  f32 pan     = amx_ctof(ptr[5]);
  f32 pitch   = amx_ctof(ptr[6]);
  u8  slot    = ptr[7];

  Sound::VoiceType voiceType = Sound::VOICE_TYPE_DEFAULT;

  switch( type ){
  case SCR_PM_VOICE_TYPE_NORMAL:
    voiceType =  Sound::VOICE_TYPE_DEFAULT;
    break;
  case SCR_PM_VOICE_TYPE_BATTLE:
    voiceType =  Sound::VOICE_TYPE_BATTLE;
    break;
  case SCR_PM_VOICE_TYPE_DOWN:
    voiceType =  Sound::VOICE_TYPE_DOWN;
    break;
  case SCR_PM_VOICE_TYPE_SPECIAL:
    voiceType =  Sound::VOICE_TYPE_SP00;
    break;
  default:
    break;
  }

  //スロットの有効数はライブラリ側でケアしている
  Sound::PlayVoice( slot, static_cast<MonsNo>(monsno), static_cast<FormNo>(formno), voiceType, false );
  Sound::ChangeVoiceVolume( slot, volume );
  Sound::ChangeVoicePan( slot, pan );
  Sound::ChangeVoicePitch( slot, pitch );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func    VoiceIsPlaying
 * @brief   ポケモンボイス再生中チェック
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::VoiceIsPlaying( AMX* amx, const cell* ptr )
{
  u8 slot    = ptr[1];

  //スロットの有効数はライブラリ側でケアしている
  return !Sound::IsVoiceFinished(slot);
}


//------------------------------------------------------------------------------
/**
 * @func        GetFieldBGM
 * @brief       FieldBGMの取得.
 * @author      N.Takeda
 * @date        2016/03/22(火) 11:15:34
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetFieldBGM( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  
  s32 iZoneID = ptr[1];
  s32 priority_ride_bgm = GFL_BOOL_CAST(ptr[2]);
  s32 iBgm;
  
  if( iZoneID == -1 ){
    iBgm = p_gman->GetGameData()->GetFieldSound()->GetFieldBGM();
  }else{
    iBgm = p_gman->GetGameData()->GetFieldSound()->GetFieldBGM( static_cast<ZoneID>(iZoneID), p_gman->GetGameData()->GetFieldSound()->GetPlayerForm(), priority_ride_bgm );
  }
  return iBgm;
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
