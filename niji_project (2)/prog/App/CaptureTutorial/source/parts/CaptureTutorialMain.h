//============================================================================================
/**
 * @file    Main.h
 * @brief   捕獲デモのメインとなるScene
 * @author  ito_isao
 * @date    12.05.25
 */
//============================================================================================
#ifndef __APP_CAPTURE_TUTORIAL_MAIN_H__
#define __APP_CAPTURE_TUTORIAL_MAIN_H__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <ui/include/gfl2_UI.h>

//  nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>

#include "CaptureTutorialInterfaceScene.h"

#include "CaptureTutorialViewer.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( GameSys )
  class GameManager;
  class GameProc;
GFL_NAMESPACE_END( GameSys )

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)


  //------------------------------------------------------------------------------
  // 親となるクラス
  //------------------------------------------------------------------------------
  class CaptureTutorialMain : public CaptureTutorialInterfaceScene<CaptureTutorialMain>
  {
  public:
    static const s32 FADE_TIME;
    static const s32 RECORD_MAX;

  public:
    CaptureTutorialMain( GameSys::GameManager* pGameManager , GameSys::GameProc* pProc, CaptureTutorialViewer::TutorialType type );
    virtual ~CaptureTutorialMain();
  
    virtual void Enter();
    virtual bool Update();
    virtual void Exit();
  
    void PreDraw();
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    template<class T>
    void SetNext()
    {
      m_pNext = GFL_NEW( m_utilheap.GetDeviceHeap() ) T();
      m_pNext->SetOwner( this );
    }
  
    bool IsFinished() const { return m_finish; }
    void SetFinished() {m_finish = true; }
  
    //------------------------------------------------------------------
    // 取得系
    //------------------------------------------------------------------
    //gfl2::ui::Button*     GetButton()       { return m_pButton; }
    app::util::Heap*      GetHeap()         { return &m_utilheap; }
    GameSys::GameManager* GetGameManaager() { return m_pGameManager; }

    CaptureTutorialViewer* GetCore()        { return m_pCore; }

    CaptureTutorialViewer::TutorialType GetTutorialType(){ return m_TutorialType; }
  
    //========================================================================================
    // 各種シーンから呼び出されるであろう関数郡
    //========================================================================================
    //------------------------------------------------------------------
    // Viewer関連
    //------------------------------------------------------------------
    void CreateViewer();
    void DeleteViewer();

    //  Viewerを有効にするか
    void SetEnableDrawViewer( const bool bFlg )    { m_bViewerDraw = bFlg; }
    void SetEnableUpdateViewer( const bool bFlg )  { m_bViewerUpdate = bFlg; }

  private:
    void _Clear()
    {
      m_pCurrent        = NULL;
      m_pNext           = NULL;
      /*
      m_pDeviceManager  = NULL;
      m_pButton         = NULL;
      */
      m_pGameManager    = NULL;
      m_pCore           = NULL;

      m_finish        = false;
      m_bViewerDraw   = false;
      m_bViewerUpdate = false;

      m_TutorialType  = CaptureTutorialViewer::CAPTURE;
    }

    CaptureTutorialInterfaceScene<CaptureTutorialMain>* m_pCurrent;
    CaptureTutorialInterfaceScene<CaptureTutorialMain>* m_pNext;

    bool m_finish;
    bool m_bViewerDraw;
    bool m_bViewerUpdate;
  
    // ヒープ
    app::util::Heap m_utilheap;

    CaptureTutorialViewer::TutorialType m_TutorialType;

    /*
    // UI系
    gfl2::ui::DeviceManager*  m_pDeviceManager;
    gfl2::ui::Button*         m_pButton;
    */

    GameSys::GameManager*     m_pGameManager;
    GameSys::GameProc*        m_pProc; //ViewSystemに渡すため
    CaptureTutorialViewer*    m_pCore;
  
    // グラフィック系
  //  gfl::grp::GraphicsSystem* m_graphics;
  };

GFL_NAMESPACE_END(promotion)
GFL_NAMESPACE_END(app)

#endif //__APP_CAPTURE_TUTORIAL_MAIN_H__
