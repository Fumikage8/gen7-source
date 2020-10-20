//======================================================================
/**
 * @file UpdaterBGM.cpp
 * @date 2015/10/29 12:01:03
 * @author saito_nozomu
 * @brief 更新処理クラス（ＢＧＭ用）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_DebugWinSystem.h>
#include "Debug/DebugKeyboard/include/DebugKeyboard.h"
#include "Sound/include/Sound.h"
#include "Debug/SoundTest/include/UpdaterBase.h"
#include "Debug/SoundTest/include/UpdaterBGM.h"

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/DllProc.h"

#include  "Print/include/wordset.h"

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)

/**
 * @brief コンストラクタ
 * @param param セットアップパラメータ
 */
UpdaterBGM::UpdaterBGM(SetupParam &param)
: UpdaterBase(param)
{
  m_pInfoStrTop = GFL_NEW(m_DevHeap) gfl2::str::StrBuf(STRLEN_LABEL_BUFFER, m_DevHeap);
  m_pInfoStrBottom = GFL_NEW(m_DevHeap) gfl2::str::StrBuf(STRLEN_LABEL_BUFFER, m_DevHeap);
}

/**
 * @brief デストラクタ
 */
UpdaterBGM::~UpdaterBGM()
{
  GFL_DELETE m_pInfoStrBottom;
  GFL_DELETE m_pInfoStrTop;
}

/**
 * @brief Aボタン押下時の処理
 */
void UpdaterBGM::FuncButtonA(void)
{
  Sound::StopBGM();
	
  Sound::StartBGMReq( m_SoundIterator->id );
	
  //m_playing_bgm = m_iterator[m_page]->id;
	//m_playing_bgm_name = m_iterator[m_page]->msg_index;
}

/**
 * @brief Bボタン押下時の処理
 */
void UpdaterBGM::FuncButtonB(void)
{
  Sound::StopBGM();
}

/**
 * @brief 右キー押下時の処理
 */
void UpdaterBGM::FuncKeyRight(void)
{
  ChangeDispInfo();
}

/**
 * @brief 左キー押下時の処理
 */
void UpdaterBGM::FuncKeyLeft(void)
{
  ChangeDispInfo();
}

/**
 * @brief タッチ時の処理
 * @param x Ｘ座標
 * @param y Ｙ座標
 */
void UpdaterBGM::FuncTouch(int x, int y)
{
  debug::KeyboardProc* proc = GameSys::CallDebugKeyboardProc();
  proc->SetupParam( m_DevHeap, m_DevHeap, m_pSearchStr, 256, 0 );
  proc->SetDefaultInputMode( debug::KeyboardProc::INPUT_ALPHA );
  proc->SetLayoutSystem( m_pLytSys );
  proc->SetSearchWords(m_pMsg, &m_Search, true);

  m_Search = -1;
}

/*
 * @brief ページ変更時処理
 */
void UpdaterBGM::ChangePageFunc(void)
{
  m_pDebugMsg->GetString(	msg_dmenu_sound_test_BGM, *m_pStr );
  m_pDebugMsg->GetString(	msg_dmenu_sound_test_search_id, *m_pInfoStrBottom );
  ChangeDispInfo();
}

/**
 * @brief 上画面描画
 * @param pDrawUtilText  テキストユーティリティクラスへのポインタ
 */
void UpdaterBGM::DrawTop(gfl2::util::DrawUtilText* pDrawUtilText)
{
  pDrawUtilText->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  pDrawUtilText->SetTextScale( 1.0f, 1.0f );

#if defined(GF_PLATFORM_WIN32)
  pDrawUtilText->DrawTextW( 10, 10, m_pStr->GetPtr() );
  pDrawUtilText->DrawTextW( 10, 30, m_pInfoStrTop->GetPtr());
#else
  pDrawUtilText->DrawText( 10, 10, m_pStr->GetPtr() );
  pDrawUtilText->DrawText( 10, 30, m_pInfoStrTop->GetPtr());
#endif
  
}

/**
 * @brief 下画面描画
 * @param pDrawUtilText  テキストユーティリティクラスへのポインタ
 */
void UpdaterBGM::DrawBottom(gfl2::util::DrawUtilText* pDrawUtilText)
{
  pDrawUtilText->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  pDrawUtilText->SetTextScale( 1.0, 1.0f );

#if defined(GF_PLATFORM_WIN32)
  pDrawUtilText->DrawTextW( 10, 10, m_pInfoStrBottom->GetPtr() );
#else
  pDrawUtilText->DrawText( 10, 10, m_pInfoStrBottom->GetPtr() );
#endif

}

/**
 * @brief 表示情報の変更
 */
void UpdaterBGM::ChangeDispInfo(void)
{
  int msg_index = m_SoundIterator->msg_index;
	int cs_id = m_SoundIterator->csid;
	
  gfl2::str::StrBuf* temp = GFL_NEW_LOW( m_DevHeap ) gfl2::str::StrBuf( STRLEN_LABEL_BUFFER, m_DevHeap );

  m_pDebugMsg->GetString( msg_dmenu_sound_test_id_label, *temp );
  m_pWordSet->RegisterNumber( 0, cs_id, 4 , print::NUM_DISP_LEFT , print::NUM_CODE_HANKAKU);
  m_pWordSet->RegisterWord( 1, m_pMsg, msg_index);
  m_pWordSet->Expand( m_pInfoStrTop, temp );

  GFL_SAFE_DELETE( temp );
}



GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif //PM_DEBUG