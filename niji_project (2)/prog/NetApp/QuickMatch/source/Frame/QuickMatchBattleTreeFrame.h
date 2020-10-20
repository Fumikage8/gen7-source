// ============================================================================
/*
 * @file QuickMatchBattleTreeFrame.h
 * @brief クイックマッチアプリの初期化プロセスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCHBATTLETREE_FRAME_H_INCLUDE )
#define QUICKMATCHBATTLETREE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"

#include "App/FieldMenu/source/QuickMatchDraw.h"
#include "App/FieldMenu/include/QuickMatchDrawListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(Frame)


class QuickMatchBattleTreeFrame : public NetAppLib::System::NetAppFrameBase , public App::FieldMenu::QuickMatchDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchBattleTreeFrame );
public:
  QuickMatchBattleTreeFrame(
    NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork );
  virtual~QuickMatchBattleTreeFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual bool cleanup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  virtual void OnQuickMatchDrawExit(void);
  virtual void OnQuickMatchDrawNext(void);

private:

  NetApp::QuickMatch::System::QuickMatchWork*                    m_pQuickMatchWork;
  App::FieldMenu::QuickMatchDraw* m_pQuickMatchDraw;
  bool m_isEnd;
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCHINIT_FRAME_H_INCLUDE
