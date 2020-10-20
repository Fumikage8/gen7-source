//=================================================================================================
/**
*  @file   FieldMoveModelFacialAnime.cpp
*  @brief  フィールド用 動作モデル表情制御マネージャー 
*  @author Miyuki Iwasawa@gamefreak.co.jp
*  @date   2015.10.30
*/
//=================================================================================================
#include <debug/include/gfl2_DebugPrint.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h" 
#include "Field/FieldStatic/include/FieldNode.h"
#include "Field/FieldStatic/include/MoveModel/FieldFacialAnime.h"
#include <model/include/gfl2_CharaModelFactory.h>

// RTTIデバッグシステム
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldFacialAnimeBase::FieldFacialAnimeBase()
:m_pCharaDrawInstance()
,m_state(ANIME_STATE_NONE)
,m_seq(0)
,m_frame(0)
,m_comNum(0)
{
  gfl2::std::MemClear( m_comTable, sizeof(CommandData)*COMMAND_TABLE_SIZE);
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ 
*/
//-----------------------------------------------------------------------------
FieldFacialAnimeBase::~FieldFacialAnimeBase()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  アニメコマンド登録初期化
*/
//-----------------------------------------------------------------------------
bool FieldFacialAnimeBase::AnimeCommandInit()
{
  if( m_state == ANIME_STATE_PLAY ){
    GFL_ASSERT_MSG(0,"Error AnimeCommandPlaying\n");
    return false;
  }
  m_state = ANIME_STATE_INIT;
  m_comNum = 0;
  m_seq = 0;
  m_frame = 0;
  gfl2::std::MemClear( m_comTable, sizeof(CommandData)*COMMAND_TABLE_SIZE);
  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アニメコマンド登録
*/
//-----------------------------------------------------------------------------
bool FieldFacialAnimeBase::AnimeCommandEntry( s32 anime_id, s32 frame )
{
  if( m_state != ANIME_STATE_INIT ){
    GFL_ASSERT_MSG(0,"Error AnimeCommand Not Initialize\n");
    return false;
  }
  if( m_comNum >= COMMAND_TABLE_SIZE ){
    GFL_ASSERT_MSG(0,"Error AnimeCommand SizeOver\n");
    return false;
  }
  m_comTable[m_comNum].anime_id = anime_id;
  m_comTable[m_comNum].frame = frame;
  m_comNum++;
  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アニメコマンドプレイ
*/
//-----------------------------------------------------------------------------
bool FieldFacialAnimeBase::AnimeCommandPlay()
{
  if( m_state != ANIME_STATE_INIT ){
    GFL_ASSERT_MSG(0,"Error AnimeCommand Not Initialize\n");
    return false;
  }
  if( m_comNum == 0 ){
    GFL_ASSERT_MSG(0,"Error AnimeCommand Zero Entry\n");
    m_state = ANIME_STATE_END;
    return false;
  }
  this->SetAutoAnime(false);
  m_state = ANIME_STATE_PLAY;
  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アニメコマンドUpdate
*/
//-----------------------------------------------------------------------------
s32 FieldFacialAnimeBase::AnimeCommandUpdate()
{
  if( m_state != ANIME_STATE_PLAY ){
    return -1;  //変化無し
  }
  s32 id = m_comTable[m_seq].anime_id;
  u32 frame = m_comTable[m_seq].frame;

  if( ++m_frame < frame ){
    if( m_frame == 1 ){
      return id;
    }
    return -1;
  }
  m_frame = 0;
  
  if( ++m_seq >= m_comNum ){
    m_state = ANIME_STATE_END;
  }
  return id;
}

//-----------------------------------------------------------------------------
/**
*  @brief  アニメコマンド再生中チェック
*/
//-----------------------------------------------------------------------------
bool FieldFacialAnimeBase::AnimeCommandPlayCheck()
{
  if( m_state == ANIME_STATE_PLAY ){
    return true;
  }
  return false;
}
//-----------------------------------------------------------------------------
/**
*  @brief  アニメコマンド初期化済みチェック
*/
//-----------------------------------------------------------------------------
bool FieldFacialAnimeBase::AnimeCommandInitCheck()
{
  if( m_state == ANIME_STATE_INIT ){
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  EyeAnime コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldFacialAnimeEye::FieldFacialAnimeEye()
:FieldFacialAnimeBase()
{

}

void FieldFacialAnimeEye::Initialize( poke_3d::model::CharaModel* pCharaDrawInstance )
{
  m_pCharaDrawInstance = pCharaDrawInstance;
  SetAutoAnime(true);
}


//-----------------------------------------------------------------------------
/**
*  @brief  EyeAnime Update
*/
//-----------------------------------------------------------------------------
void FieldFacialAnimeEye::PrevUpdate()
{
  //コマンド更新
  if( AnimeCommandPlayCheck() ){
    s32 id = AnimeCommandUpdate();
    if( id >= 0 ){
      m_pCharaDrawInstance->SetEyeIndex(static_cast<poke_3d::model::CharaModel::EyeIndex>(id));  
    }
  }
  //更新
  m_pCharaDrawInstance->UpdateEye();  //自動アニメもこの中でコントロールされている 
}

//-----------------------------------------------------------------------------
/**
*  @brief  EyeAnime AutoAnime
*/
//-----------------------------------------------------------------------------
bool FieldFacialAnimeEye::CheckAutoAnime()
{
  if( m_pCharaDrawInstance->GetAutoBlinkMode() != poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE ){
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  EyeAnime AutoAnime
*/
//-----------------------------------------------------------------------------
void FieldFacialAnimeEye::SetAutoAnime( bool set )
{
  if( AnimeCommandPlayCheck() ){
    GFL_ASSERT_MSG(0,"error AnimeCommandPlaying\n");  //アニメコマンド中はチェンジ不可
    return;
  }
  poke_3d::model::CharaModel::AutoBlinkMode now = m_pCharaDrawInstance->GetAutoBlinkMode();
  if( set ){
    if( now == poke_3d::model::CharaModel::AUTO_BLINK_MODE_FIELD  ) return;

    m_pCharaDrawInstance->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_FIELD);
    m_state = ANIME_STATE_AUTO;
  }else{
    if( now == poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE  ) return;

    m_pCharaDrawInstance->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE);
    m_state = ANIME_STATE_NONE;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  EyeAnime Change
*/
//-----------------------------------------------------------------------------
void FieldFacialAnimeEye::ChangeAnime( s32 eyeIndex )
{
  poke_3d::model::CharaModel::EyeIndex index =
    static_cast<poke_3d::model::CharaModel::EyeIndex>(eyeIndex);

  if( AnimeCommandPlayCheck() ){
    GFL_ASSERT_MSG(0,"error AnimeCommandPlaying\n");  //アニメコマンド中はチェンジ不可
    return;
  }
  this->SetAutoAnime(false);
  m_pCharaDrawInstance->SetEyeIndex(index);
  m_state = ANIME_STATE_CHANGE; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  EyeAnime ResetAnime
*/
//-----------------------------------------------------------------------------
void FieldFacialAnimeEye::ResetAnime(void)
{
  m_pCharaDrawInstance->SetEyeIndex(poke_3d::model::CharaModel::EYE_INDEX_NONE);  
  this->SetAutoAnime(true);
  m_state = ANIME_STATE_NONE; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  MouthAnime コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldFacialAnimeMouth::FieldFacialAnimeMouth()
:FieldFacialAnimeBase()
 ,m_isAutoAnime(false)
 ,m_autoAnimeSequence(0)
 ,m_autoAnimeFrame(0)
 ,m_autoAnimeInterval(0)
{
}


void FieldFacialAnimeMouth::Initialize( poke_3d::model::CharaModel* pCharaDrawInstance )
{
  m_pCharaDrawInstance = pCharaDrawInstance;
  SetAutoAnime(false);
}

//-----------------------------------------------------------------------------
/**
*  @brief  MouthAnime Update
*/
//-----------------------------------------------------------------------------
void FieldFacialAnimeMouth::PrevUpdate( void )
{
  //コマンド更新
  if( AnimeCommandPlayCheck() ){
    s32 id = AnimeCommandUpdate();
    if( id >= 0 ){
      m_pCharaDrawInstance->SetMouthIndex(static_cast<poke_3d::model::CharaModel::MouthIndex>(id));  
    }
  }else if( m_isAutoAnime ){
    AutoAnimeUpdate();
  }
  //更新
  m_pCharaDrawInstance->UpdateMouth();
}

//-----------------------------------------------------------------------------
/**
*  @brief  EyeAnime AutoAnime
*/
//-----------------------------------------------------------------------------
bool FieldFacialAnimeMouth::CheckAutoAnime()
{
  return m_isAutoAnime;
}

//-----------------------------------------------------------------------------
/**
*  @brief  MouthAnime AutoAnime
*/
//-----------------------------------------------------------------------------
void FieldFacialAnimeMouth::SetAutoAnime( bool set )
{
  if( AnimeCommandPlayCheck() ){
    GFL_ASSERT_MSG(0,"error AnimeCommandPlaying\n");  //アニメコマンド中はチェンジ不可
    return;
  }
  if( set ){
    if( m_isAutoAnime ) return;

    m_pCharaDrawInstance->SetMouthIndex(poke_3d::model::CharaModel::MOUTH_INDEX_OPEN);  
    m_state = ANIME_STATE_AUTO;
  }else{
    if( !m_isAutoAnime ) return;

    m_pCharaDrawInstance->SetMouthIndex(poke_3d::model::CharaModel::MOUTH_INDEX_CLOSE);
    m_state = ANIME_STATE_CHANGE;
  }
  m_isAutoAnime = set;
  m_autoAnimeSequence = 0;
  m_autoAnimeFrame = 0;
  m_autoAnimeInterval = 0;
}

//-----------------------------------------------------------------------------
/**
*  @brief  MouthAnime Change
*/
//-----------------------------------------------------------------------------
void FieldFacialAnimeMouth::ChangeAnime( s32 mouthIndex )
{
  poke_3d::model::CharaModel::MouthIndex index =
    static_cast<poke_3d::model::CharaModel::MouthIndex>(mouthIndex);

  if( AnimeCommandPlayCheck() ){
    GFL_ASSERT_MSG(0,"error AnimeCommandPlaying\n");  //アニメコマンド中はチェンジ不可
    return;
  }
  this->SetAutoAnime(false);
  m_pCharaDrawInstance->SetMouthIndex(index);
  m_state = ANIME_STATE_CHANGE; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  MouthAnime ResetAnime
*/
//-----------------------------------------------------------------------------
void FieldFacialAnimeMouth::ResetAnime(void)
{
  m_pCharaDrawInstance->SetMouthIndex(poke_3d::model::CharaModel::MOUTH_INDEX_NONE);  
  this->SetAutoAnime(false);
  m_state = ANIME_STATE_NONE; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  MouthAnime AutoAnimeUpdate
*/
//-----------------------------------------------------------------------------
void FieldFacialAnimeMouth::AutoAnimeUpdate( void )
{
  const u32 ONE_STATE_FRAME = 2;
  const u32 INTERVAL_FRAME = 4;

  m_autoAnimeFrame++;
  
  switch (m_autoAnimeSequence)
  {
  // 開ける
  case 0:

    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_OPEN );

    ++m_autoAnimeSequence;
    m_autoAnimeFrame = 0;

    if ( m_pCharaDrawInstance->isSetupRandomFunc() ){
      m_autoAnimeInterval = m_pCharaDrawInstance->GetRandomValue(INTERVAL_FRAME)+ONE_STATE_FRAME;
    }else{
      m_autoAnimeInterval = INTERVAL_FRAME;
    }
    //break throw
    
  // 開ける
  case 1:

    // 次へ
    if ( m_autoAnimeFrame > m_autoAnimeInterval )
    {
      ++m_autoAnimeSequence;
      m_autoAnimeFrame = 0;
    }
    break;

  // 半開け
  case 2:

    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_HALF_OPEN );

    // 次へ
    if( m_autoAnimeFrame > ONE_STATE_FRAME )
    {
      ++m_autoAnimeSequence;
      m_autoAnimeFrame = 0;
    }
    break;

  // 閉じる 
  case 3:

    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_CLOSE );

    // 次へ
    if( m_autoAnimeFrame > ONE_STATE_FRAME )
    {
      ++m_autoAnimeSequence;
      m_autoAnimeFrame = 0;
    }
    break;

  // 半開け
  case 4:

    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_HALF_OPEN );

    // 最初へ
    if( m_autoAnimeFrame > ONE_STATE_FRAME )
    {
      m_autoAnimeSequence = 0;
      m_autoAnimeFrame = 0;
    }
    break;
  }
}


}; //end of namespace MoveModel
}; //end of namespace Field
