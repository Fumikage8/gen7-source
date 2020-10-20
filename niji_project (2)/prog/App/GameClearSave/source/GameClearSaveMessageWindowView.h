//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		GameClearSaveMessageWindowView.h
 *	@brief  ゲームクリアー時のセーブ画面のメッセージウィンドウ
 *	@author	koji_kawada@gamefreak.co.jp
 *	@date		2016.01.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __APP_GAMECLEARSAVE_MESSAGEWINDOWVIEW_H__)
#define __APP_GAMECLEARSAVE_MESSAGEWINDOWVIEW_H__


#include <AppLib/include/Tool/CommonMessageWindowView.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(game_clear_save)


class MessageWindowView : public App::Tool::CommonMessageWindowView
{
  GFL_FORBID_COPY_AND_ASSIGN(MessageWindowView);

public:
  MessageWindowView(const PARAM& rParam, IEventHandler* pHandler = NULL)
    : App::Tool::CommonMessageWindowView(rParam, pHandler)
  {}
  virtual ~MessageWindowView()
  {}

  virtual void Setup(void* pLayoutBuffer);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);
};


GFL_NAMESPACE_END(game_clear_save)
GFL_NAMESPACE_END(app)


#endif		// __APP_GAMECLEARSAVE_MESSAGEWINDOWVIEW_H__

