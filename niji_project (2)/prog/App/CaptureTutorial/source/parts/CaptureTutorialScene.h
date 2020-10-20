//============================================================================================
/**
 * @file    Scene.h
 * @brief   捕獲デモでの各種Scene
 * @author  ito_isao
 * @date    12.05.25
 */
//============================================================================================
#ifndef __APP_CAPTURE_TUTORIAL_SCENE_H__
#define __APP_CAPTURE_TUTORIAL_SCENE_H__
#pragma once

#include "CaptureTutorialInterfaceScene.h"
#include "CaptureTutorialMain.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

  //  前方宣言
  class CaptureTutorialViewer;

  //------------------------------------------------------------------------------
  // 初めてプロックに入ってきた時の、フェードイン
  //------------------------------------------------------------------------------
  class CaptureTutorialInitScene : public CaptureTutorialInterfaceScene<CaptureTutorialMain>
  {
  public:
    virtual void Enter();
    virtual bool Update();
    virtual void Exit();
  private:
    u32 m_seq;
  };

  //------------------------------------------------------------------------------
  // メイン
  //------------------------------------------------------------------------------
  class CaptureTutorialMainScene : public CaptureTutorialInterfaceScene<CaptureTutorialMain>
  {
  public:
    virtual void Enter();
    virtual bool Update();
    virtual void Exit();
  
  private:
    //  ゲーム内のフレーム数を取得
    u32  _GetFrameRate();

    void _Clear()
    {
      m_seq              = 0;
      m_frameRate        = 0;
      m_time             = 0;
      m_is_capture       = false;
      m_down_atk         = false;
      m_pEventScriptExe  = NULL;
    }

    u32 m_seq;
    u32 m_frameRate;
    u32 m_time;

    bool m_is_capture;
    bool m_down_atk;

    class CaptureTutorialEventScriptExecution*  m_pEventScriptExe;
  };

  //------------------------------------------------------------------------------
  // プロックから抜ける前の、フェードアウト
  //------------------------------------------------------------------------------
  class CaptureTutorialExitScene : public CaptureTutorialInterfaceScene<CaptureTutorialMain>
  {
  public:
    virtual void Enter();
    virtual bool Update();
    virtual void Exit() {}

  private:
    u32 m_seq;
  };

GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)

#endif //__APP_CAPTURE_TUTORIAL_SCENE_H__
