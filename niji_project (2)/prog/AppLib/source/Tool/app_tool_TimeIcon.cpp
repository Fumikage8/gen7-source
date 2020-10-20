//=============================================================================
/**
 * @file    app_tool_TimeIcon.cpp
 * @brief   タイマーアイコン
 * @author  Hiroyuki Nakamura
 * @date    12.04.18
 */
//=============================================================================

// module
#include "AppLib/include/Tool/app_tool_TimeIcon.h"
// niji
#include "System/include/GflUse.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//-----------------------------------------------------------------------------
/**
 * @func    StartAnime
 * @brief   アニメ再生
 * @date    2015.06.25
 *
 * @param   lytwk   レイアウトワーク
 * @param   anime   アニメ番号
 * @param   frame   フレームレート
 */
//-----------------------------------------------------------------------------
void TimeIcon::StartAnime( gfl2::lyt::LytWk * lytwk, u32 anime, GameSys::FrameMode frame )
{
  static const u32 ANIME_START_PATTERN = 17;  // アニメ開始パターン数

	f32 frm = lytwk->GetAnimeMaxFrame( anime );
  u32	rnd = System::GflUse::GetPublicRand( ANIME_START_PATTERN );

  if( lytwk->IsBindAnime( anime ) == false )
  {
    lytwk->BindAnime( anime );
  }
  if( frame == GameSys::FRAMEMODE_60 )
  {
    lytwk->SetAutoAnimeSpeed( anime, 0.5f );
  }
  else
  {
    lytwk->SetAutoAnimeSpeed( anime, 1.0f );
  }

  lytwk->SetAnimeFrame( anime, frm/ANIME_START_PATTERN*rnd );
  lytwk->SetAutoAnime( anime, true );
}


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
