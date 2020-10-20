//======================================================================
/**
 * @file UpdaterPVZukan.cpp
 * @date 2015/10/29 12:01:03
 * @author saito_nozomu
 * @brief 更新処理クラス（図鑑順ポケモン鳴き声用）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>
#include "Debug/DebugKeyboard/include/DebugKeyboard.h"
#include "Sound/include/Sound.h"
#include "Debug/SoundTest/include/UpdaterBase.h"
#include "Debug/SoundTest/include/UpdaterPVZukan.h"

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/DllProc.h"

#include "arc_index/message.gaix"

// pml
#include <pml/include/pmlib.h>

#include "Debug/DebugNumInput/include/DebugNumInput.h"

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)


const TouchInfo s_touch[TOUCH_MAX] = 
{
	{
		160-100, 120-20,
		200, 40,
	}
	,
	{
		160-100, 120+20,
		200, 40,
	}
	,
};


/**
 * @brief コンストラクタ
 * @param param セットアップパラメータ
 * @param pContainer サウンドコンテナーへのポインタ
 */
UpdaterPVZukan::UpdaterPVZukan(SetupParam &param, SoundContainer *pContainer)
: UpdaterBase(param)
{
  m_UseSoundIterator = false;
  m_pContainer = pContainer;
  m_PokeNo = 1;

  m_pNumInput = NULL;

  m_pInfoStrTop = GFL_NEW(m_DevHeap) gfl2::str::StrBuf(STRLEN_LABEL_BUFFER, m_DevHeap);
  m_pInfoStrBottomZukan = GFL_NEW(m_DevHeap) gfl2::str::StrBuf(STRLEN_LABEL_BUFFER, m_DevHeap);
  m_pInfoStrBottomName = GFL_NEW(m_DevHeap) gfl2::str::StrBuf(STRLEN_LABEL_BUFFER, m_DevHeap);

  for(int i=0;i<POKE_VOICE_NUM;i++)
  {
    m_pInfoStrTopVoice[i] = GFL_NEW(m_DevHeap) gfl2::str::StrBuf(STRLEN_LABEL_BUFFER, m_DevHeap);
  }
}

/**
 * @brief デストラクタ
 */
UpdaterPVZukan::~UpdaterPVZukan()
{
  for(int i=0;i<POKE_VOICE_NUM;i++)
  {
    GFL_DELETE m_pInfoStrTopVoice[i];
  }

  GFL_DELETE m_pInfoStrBottomName;
  GFL_DELETE m_pInfoStrBottomZukan;
  GFL_DELETE m_pInfoStrTop;
}

bool UpdaterPVZukan::UpdateFunc(void)
{
  if (m_InputDel)
  {
    m_InputDel = false;
    if(m_pNumInput != NULL)
	  {
      GFL_DELETE m_pNumInput;
      m_pNumInput = NULL;
    }
    return true;
  }

  if(m_pNumInput != NULL)
	{
		// 数値入力
		debug::NumInput::Result ret = m_pNumInput->Update();
		switch(ret)
		{
		case debug::NumInput::RES_CONT:
			return true;
		case debug::NumInput::RES_DECIDE:
      m_PokeNo = m_pNumInput->GetValue();
      ChangeDispInfo();
      m_InputDel = true;
			return true;
		case debug::NumInput::RES_CANCEL:
      m_InputDel = true;
			return true;
		}

    return true;
	}

  return false;
}

/**
 * @brief Aボタン押下時の処理
 */
void UpdaterPVZukan::FuncButtonA(void)
{
  SoundContainer::Iterator it = m_pContainer->PVBegin(m_PokeNo, true);
  for(int i = 0; i < m_PokeCursor; ++i)
  {
    ++it;
	}
  NOZOMU_PRINT("play_voice %d msg %d\n", it->id, it->msg_index);
  Sound::PlayVoice(0, (VoiceID)it->id);
}

/**
 * @brief 右キー押下時の処理
 */
void UpdaterPVZukan::FuncKeyRight(void)
{
  ++m_PokeNo;
	if(m_PokeNo > m_pContainer->GetMaxPokeNumber())
	{
    m_PokeNo = 1;
	}
	if(m_PokeCursor >= m_pContainer->PVSize(m_PokeNo, true))
  {
    m_PokeCursor = 0;
  }

  if(m_PokeCursor < 0)
  {
    m_PokeCursor = m_pContainer->PVSize(m_PokeNo, true)-1;
  }

  ChangeDispInfo();

  NOZOMU_PRINT("info: Cur=%d  poke_no=%d\n",m_PokeCursor, m_PokeNo);
}

/**
 * @brief 左キー押下時の処理
 */
void UpdaterPVZukan::FuncKeyLeft(void)
{
  --m_PokeNo;
  if(m_PokeNo < 1)
  {
    m_PokeNo = m_pContainer->GetMaxPokeNumber();
  }
  if(m_PokeCursor >= m_pContainer->PVSize(m_PokeNo, true))
  {
    m_PokeCursor = 0;
  }

  if(m_PokeCursor < 0)
  {
    m_PokeCursor = m_pContainer->PVSize(m_PokeNo, true)-1;
  }

  ChangeDispInfo();

  NOZOMU_PRINT("info: Cur=%d  poke_no=%d\n",m_PokeCursor, m_PokeNo);

}

/**
 * @brief 上キー押下時の処理
 */
void UpdaterPVZukan::FuncKeyUp(void)
{
  --m_PokeCursor;
  if(m_PokeCursor < 0)
	{
    m_PokeCursor = m_pContainer->PVSize(m_PokeNo, true)-1;
	}

  NOZOMU_PRINT("info: Cur=%d  poke_no=%d\n",m_PokeCursor, m_PokeNo);
}

/**
 * @brief 下キー押下時の処理
 */
void UpdaterPVZukan::FuncKeyDown(void)
{
  ++m_PokeCursor;
  if(m_PokeCursor >= m_pContainer->PVSize(m_PokeNo, true))
  {
    m_PokeCursor = 0;
	}

  NOZOMU_PRINT("info: Cur=%d  poke_no=%d\n",m_PokeCursor, m_PokeNo);
}

/**
 * @brief タッチ時の処理
 * @param x Ｘ座標
 * @param y Ｙ座標
 */
void UpdaterPVZukan::FuncTouch(int x, int y)
{
  for(int i = 0; i < TOUCH_MAX; ++i)
	{
		if(s_touch[i].x < x && x < s_touch[i].x + s_touch[i].width &&
		   s_touch[i].y < y && y < s_touch[i].y + s_touch[i].height)
		{
			Touch(i);
			break;
		}
	}
}

/*
 * @brief ページ変更時処理
 */
void UpdaterPVZukan::ChangePageFunc(void)
{
  m_pDebugMsg->GetString(	msg_dmenu_sound_test_PVZukan, *m_pStr );
  m_pDebugMsg->GetString(	msg_dmenu_sound_test_search_zukanno, *m_pInfoStrBottomZukan );
  m_pDebugMsg->GetString(	msg_dmenu_sound_test_search_monsname, *m_pInfoStrBottomName );
  ChangeDispInfo();
}

/**
 * @brief 上画面描画
 * @param pDrawUtilText  テキストユーティリティクラスへのポインタ
 */
void UpdaterPVZukan::DrawTop(gfl2::util::DrawUtilText* pDrawUtilText)
{
  pDrawUtilText->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  pDrawUtilText->SetTextScale( 1.0f, 1.0f );

#if defined(GF_PLATFORM_WIN32)
  pDrawUtilText->DrawTextW( 10, 10, m_pStr->GetPtr() );
  pDrawUtilText->DrawTextW( 10, 25, m_pInfoStrTop->GetPtr() );
#else
  pDrawUtilText->DrawText( 10, 10, m_pStr->GetPtr() );
  pDrawUtilText->DrawText( 10, 25, m_pInfoStrTop->GetPtr() );
#endif

  for( int count=0;count<m_VoiceCount;count++)
	{
		if(count == m_PokeCursor)
		{
			// 選択中なので赤色に
      pDrawUtilText->SetTextColor( gfl2::math::Vector4( 1.0f, 0.3f, 0.3f, 1.0f ) );
		}
		else
		{
			// 見選択アイテムなので白色に
      pDrawUtilText->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
		}
		
    if(count < 9)
		{
#if defined(GF_PLATFORM_WIN32)
      pDrawUtilText->DrawTextW( 0, 50+20*count, m_pInfoStrTopVoice[count]->GetPtr() );
#else
      pDrawUtilText->DrawText( 0, 50+20*count, m_pInfoStrTopVoice[count]->GetPtr() );
#endif
		}
	  else
	  {
#if defined(GF_PLATFORM_WIN32)
      pDrawUtilText->DrawTextW( 0, 50+20*(count-8), m_pInfoStrTopVoice[count]->GetPtr() );
#else
      pDrawUtilText->DrawText( 0, 50+20*(count-8), m_pInfoStrTopVoice[count]->GetPtr() );
#endif
	  }
	}
}

/**
 * @brief 下画面描画
 * @param pDrawUtilText  テキストユーティリティクラスへのポインタ
 */
void UpdaterPVZukan::DrawBottom(gfl2::util::DrawUtilText* pDrawUtilText)
{
  if( !m_InputDel && (m_pNumInput != NULL) )
	{
    m_pNumInput->Draw();
  }
  else
  {
    // 白色に
    pDrawUtilText->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
	  // 通常のタッチ部分描画
    
#if defined(GF_PLATFORM_WIN32)
    pDrawUtilText->DrawTextW( s_touch[0].x+5, s_touch[0].y+5, m_pInfoStrBottomZukan->GetPtr());
    pDrawUtilText->DrawTextW( s_touch[1].x+5, s_touch[1].y+5, m_pInfoStrBottomName->GetPtr());
#else
    pDrawUtilText->DrawText( s_touch[0].x+5, s_touch[0].y+5, m_pInfoStrBottomZukan->GetPtr());
    pDrawUtilText->DrawText( s_touch[1].x+5, s_touch[1].y+5, m_pInfoStrBottomName->GetPtr());
#endif
  }
}

/**
 * @brief 検索後処理
 * @param page ページ
 */
void UpdaterPVZukan::SearchAfterFunc(Page &page)
{
  m_PokeNo = MonsNo2GraNo(m_Search);
}

/**
 * @brief ページ切り替え可能か
 * @return bool true で可能
 */
bool UpdaterPVZukan::CanPageChange(void)
{
  return (m_pNumInput == NULL);
}

/**
 * @brief タッチ時の処理
 * @param id
 */
void UpdaterPVZukan::Touch(int id)
{
  switch(id)
	{
	case TOUCH_SEARCH_NO:
		this->SearchByNo();
		break;
	case TOUCH_SEARCH_NAME:
		this->SearchByName();
		break;
	}
}

/**
 * @brief 名前で検索
 */
void UpdaterPVZukan::SearchByName(void)
{
  debug::KeyboardProc* proc = GameSys::CallDebugKeyboardProc();
  proc->SetupParam( m_DevHeap, m_DevHeap, m_pSearchStr, 256, 0 );
  proc->SetDefaultInputMode( debug::KeyboardProc::INPUT_KATAKANA );
  proc->SetLayoutSystem( m_pLytSys );
  proc->SetSearchWords(GARC_message_monsname_DAT, &m_Search, false);

	m_Search = -1;
}

/**
 * @brief ナンバーで検索
 */
void UpdaterPVZukan::SearchByNo(void)
{
	int init = m_PokeNo;
  gfl2::str::StrBuf* title = GFL_NEW_LOW( m_DevHeap ) gfl2::str::StrBuf( STRLEN_LABEL_BUFFER, m_DevHeap );
  m_pDebugMsg->GetString( msg_dmenu_sound_test_input_title, *title );

  m_pNumInput = GFL_NEW(m_DevHeap) debug::NumInput( m_DevHeap, m_DevHeap, m_pLytSys );
  m_pNumInput->Start( title->GetPtr(), 1, 1, 1000 );

  GFL_SAFE_DELETE( title );
}

/**
 * @brief 表示情報の変更
 */
void UpdaterPVZukan::ChangeDispInfo(void)
{
  int msg_index = m_SoundIterator->msg_index;
	int cs_id = m_SoundIterator->csid;
	
  gfl2::str::StrBuf* temp = GFL_NEW_LOW( m_DevHeap ) gfl2::str::StrBuf( STRLEN_LABEL_BUFFER, m_DevHeap );

  u16 monsno = m_PokeNo;
  if ( monsno > MONSNO_END ) monsno = 0;
  m_pDebugMsg->GetString( msg_dmenu_sound_test_zukan_dev, *temp );
  m_pWordSet->RegisterNumber( 0, m_PokeNo, 3 , print::NUM_DISP_LEFT , print::NUM_CODE_HANKAKU);
  m_pWordSet->RegisterNumber( 1, MonsNo2GraNo(monsno), 3 , print::NUM_DISP_LEFT , print::NUM_CODE_HANKAKU);
  m_pWordSet->RegisterPokeMonsNameNo(2,(MonsNo)monsno);
  m_pWordSet->Expand( m_pInfoStrTop, temp );

  int count = 0;
  for( SoundContainer::Iterator it = m_pContainer->PVBegin(m_PokeNo, false);
		 it != m_pContainer->PVEnd(m_PokeNo, false);
		 ++it )
	{
		int msg_index = it->msg_index;

    m_pDebugMsg->GetString( msg_dmenu_sound_test_label, *temp );
    m_pWordSet->RegisterWord( 0, m_pMsg, msg_index);
    m_pWordSet->Expand( m_pInfoStrTopVoice[count], temp );
		++count;
	}
  m_VoiceCount = count;

  GFL_SAFE_DELETE( temp );
}

GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif //PM_DEBUG