// ============================================================================
/*
 * @file SuperTrainingFrame.h
 * @brief すごい特訓アプリのプロセスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIGSAMPLE_FRAME_H_INCLUDE )
#define SUPER_TRAINIGSAMPLE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  nijiのインクルード
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "App/SuperTraining/source/System/SuperTrainingWork.h"

//  Viewのインクルード
#include "App/SuperTraining/source/System/SuperTrainingViewSystemAccessor.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(Frame)

  class SuperTrainingFrame
   : public NetAppLib::System::NetAppFrameBase
   , public App::Tool::MessageMenuView::IEventHandler
  {
    GFL_FORBID_COPY_AND_ASSIGN( SuperTrainingFrame );
  public:
    SuperTrainingFrame(
      App::SuperTraining::System::SuperTrainingWork* pSuperTrainingWork );
    virtual~SuperTrainingFrame();

    virtual void  MessageMenuView_OnEvent(const EventCode event);

  private:

    // NetAppFrameBase
    virtual bool startup();
    virtual bool cleanup();
    virtual void setListener();
    virtual void removeListener();
    virtual void updateSequence();

  private:

    enum
    {
      SEQ_ITEM_SELECT = 0,
      SEQ_TRAINING,
      SEQ_MESSAGE,
    };

    //  チェック項目ボタンidからポケモンパラメータの能力idに変換
    pml::pokepara::PowerID  _GetPokeParaPowerIdFromChkButtonId( const App::SuperTraining::View::LowerItemListView::ButtonId buttonId );

    //  シーケンス設定
    u32 _SetSeq( const u32 newSeq );
    //  メッセージシーケンス設定
    u32 _SetMessageSeq( const u32 strId );

    void  _Clear()
    {
      m_pSuperTrainingWork    = NULL;
      m_messageMenuEventCode  = App::Tool::MessageMenuView::IEventHandler::EVENT_;
    }

    App::Tool::MessageMenuView::IEventHandler::EventCode              m_messageMenuEventCode;
    App::SuperTraining::System::SuperTrainingWork*                    m_pSuperTrainingWork;

    static const App::SuperTraining::View::LowerItemListView::ButtonId ms_aChkButtonIdList[];
  };


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

#endif // SUPER_TRAINIGSAMPLE_FRAME_H_INCLUDE
