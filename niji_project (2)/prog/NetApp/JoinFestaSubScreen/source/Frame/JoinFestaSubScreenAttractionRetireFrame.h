// ============================================================================
/*
 * @file JoinFestaSubScreenAttractionRetireFrame.h
 * @brief ジョインフェスタ下画面のアトラクションリタイア専用フレームです。
 * @date 2015.12.04
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_ATTRACTION_R_FRAME_H_INCLUDE )
#define JOINFESTASUBSCREEN_ATTRACTION_R_FRAME_H_INCLUDE
#pragma once

// FrameBase
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameBase.h"

// TimeLimit
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSubScreenAttractionRetireFrame : public JoinFestaSubScreenFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenAttractionRetireFrame );
public:
  JoinFestaSubScreenAttractionRetireFrame(
    NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
    NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView );
  virtual~JoinFestaSubScreenAttractionRetireFrame();

private:

  //-----------------------------------------------------------------------------
  // NetAppFrameBase
  //-----------------------------------------------------------------------------
  //! このフレームの起動処理を記述します。true で終了
  virtual bool startup();
  //! このフレームのシーケンス制御処理を記述します。
  virtual void updateSequence();

private:

  //! 最低ウェイト（ms）
  static const u32 WAIT_TIME = 1500; // 1.5秒
  // 制限時間
  NetAppLib::Util::NetAppTimeout  m_waitTimeout;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_ATTRACTION_R_FRAME_H_INCLUDE
