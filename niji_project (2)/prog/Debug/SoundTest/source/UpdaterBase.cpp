//======================================================================
/**
 * @file UpdaterBase.cpp
 * @date 2015/10/28 19:57:55
 * @author saito_nozomu
 * @brief 更新処理規定クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include "Debug/SoundTest/include/UpdaterBase.h"

// niji
#include "GameSys/include/GameManager.h"


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)



/**
 * @brief コンストラクタ
 * @param param セットアップパラメータ
 */
UpdaterBase::UpdaterBase(SetupParam &param)
{
  m_DevHeap = param.m_DevHeap;
  m_pLytSys = param.m_pLytSys;
  m_pStr = param.m_pStr;
  m_pSearchStr = param.m_pSearchStr;
  m_pMsg = param.m_pMsg;
  m_pDebugMsg = param.m_pDebugMsg;
  m_pWordSet = param.m_pWordSet;
  m_SoundIterator = param.m_SoundIteratorBegin;
  m_SoundIteratorBegin = param.m_SoundIteratorBegin;
  m_SoundIteratorEnd = param.m_SoundIteratorEnd;
  m_Page = param.m_Page;

  m_UseSoundIterator = true;
  m_Search = -1;
}

/*
 * @brief 表示情報のセット
 */
void UpdaterBase::SetDispInfo(void)
{
  ChangePageFunc();
}

/**
 * @brief 入力更新
 */
void UpdaterBase::UpdateInput(Page &page)
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  gfl2::ui::TouchPanel* pTouchPanel       = pDeviceManager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

  SearchAfter(page);

  if ( UpdateFunc() )
  {
    return;
  }

  //検索していた場合、検索結果の反映

  // Bボタン
  if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
  {
    FuncButtonB();
  }

  // 次移動
  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
	{
		int time = 1;
		if(pButton->IsHold(gfl2::ui::BUTTON_Y)){time = 10;}
		if(pButton->IsHold(gfl2::ui::BUTTON_X)){time = 100;}
		for(int i = 0; i < time; ++i)
		{
      if (m_UseSoundIterator)
      {
			  ++m_SoundIterator;
			  if(m_SoundIterator == m_SoundIteratorEnd)
			  {
				  m_SoundIterator = m_SoundIteratorBegin;
			  }
      }
      FuncKeyRight();
		}

    if (m_UseSoundIterator) NOZOMU_PRINT("info: iD=%d  msg=%d\n",m_SoundIterator->id, m_SoundIterator->msg_index);
	}
	// 前移動
	if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
	{
		int time = 1;
		if(pButton->IsHold(gfl2::ui::BUTTON_Y)){time = 10;}
		if(pButton->IsHold(gfl2::ui::BUTTON_X)){time = 100;}
		for(int i = 0; i < time; ++i)
		{
      if (m_UseSoundIterator)
      {
        if(m_SoundIterator == m_SoundIteratorBegin)
			  {
				  m_SoundIterator = m_SoundIteratorEnd;
          m_SoundIterator--;
			  }
			  else
			  {
				  --m_SoundIterator;
			  }
      }
      FuncKeyLeft();
		}

    if (m_UseSoundIterator) NOZOMU_PRINT("info: iD=%d  msg=%d\n",m_SoundIterator->id, m_SoundIterator->msg_index);
	}

  // 上移動
  if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
  {
    FuncKeyUp();
  }

  // 下移動
  if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
  {
    FuncKeyDown();
  }
	
	// 再生
	if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
	{
    if (m_UseSoundIterator) NOZOMU_PRINT("play: iD=%d  msg=%d\n",m_SoundIterator->id, m_SoundIterator->msg_index);
    FuncButtonA();
	}

  //タッチ
  if ( pTouchPanel->IsTouchTrigger() )
  {
    int x = pTouchPanel->GetX();
    int y = pTouchPanel->GetY();
    FuncTouch(x, y);
  }
}

void UpdaterBase::SearchAfterFunc(Page &page)
{
  for(SoundContainer::Iterator it = m_SoundIteratorBegin; it != m_SoundIteratorEnd; ++it)
  {
    if(it->msg_index == m_Search)
    {
      m_SoundIterator = it;
      page = GetPage();
			break;
    }
  }
}

void UpdaterBase::SearchAfter(Page &page)
{
  if (m_Search == -1) return;

  SearchAfterFunc(page);
  SetDispInfo();
  m_Search = -1;
}

GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif //PM_DEBUG
