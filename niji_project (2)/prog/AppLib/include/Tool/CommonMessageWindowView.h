#if !defined(COMMONMESSAGEWINDOWVIEW_H_INCLUDED)
#define COMMONMESSAGEWINDOWVIEW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   CommonMessageWindowView.h
 * @date   2015/07/03 Fri. 14:23:31
 * @author muto_yu
 * @brief  共通メッセージウインドウのLayoutを使用したメッセージウインドウ
 * @note   NetAppLibより移動
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <Print/include/WordSet.h>
#include  <Savedata/include/ConfigSave.h>

#include  "AppLib/include/Tool/CommonMessageWindowManipulator.h"
#include  "AppLib/include/Ui/UIView.h"


namespace App  {
namespace Tool  {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  CommonMessageWindowView
  *  @brief  共通メッセージウインドウのLayoutを使用したメッセージウインドウ
  *  @date   2015/07/03 Fri. 14:40:01
  */
//==============================================================================
class CommonMessageWindowView
  : public app::ui::UIView
  , public CommonMessageWindowManipulator::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( CommonMessageWindowView );

public:
  /**
    動作に必要なもの
    内部でShallowCopyする
  */
  typedef struct PARAM
  {
    app::util::Heap*                  pHeap;
    app::util::AppRenderingManager*   pRenderingManager;
    
    /*  ctor  */
    PARAM(app::util::Heap* _pHeap, app::util::AppRenderingManager* _pRenderingManager)
      : pHeap(_pHeap)
      , pRenderingManager(_pRenderingManager)
    {
    }
  }PARAM;


  /**
    イベントハンドラ
  */
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    enum  EventCode
    {
      EVENT_OnEOM,        /**<  メッセージを終端まで表示した                                  */
      EVENT_OnFinish,     /**<  （ユーザ入力待ちなどの）Finishとなった（Noneでは発生しない）  */
      
      EVENT_
    };
    virtual void  CommonMessageWindowView_OnEvent(const EventCode event) = 0;
  };


  //------------------------------------------------------------------
  /**
    *  @func     CommonMessageWindowView
    *  @brief    ctor
    *  @param    pLayoutBuffer  [in]  Layoutリソースデータへのポインタ。通常ResourceManagerから取得する。
    */
  //------------------------------------------------------------------
  CommonMessageWindowView(const PARAM& rParam, IEventHandler* pHandler = NULL);


  //--------------------------------------------------------------------------------------------
  /**
    * @brief    dtor
    */
  //--------------------------------------------------------------------------------------------
  virtual ~CommonMessageWindowView();



  //------------------------------------------------------------------
  /**
    *  @note    内部でG2DUtilのCreateMessageWindow,CreateMsgCursorなどを行う
    *  @param   [in]  pLayoutBuffer   layoutリソースデータ
    *  @note    LYTRES_MENUWINDOW_MENU_MIN_LOW_000_BFLYT 以外のLayoutリソースを使って
    *           CommonMessageWindowManipulatorを初期化する場合はOverrideする
    *           自身でCreate2Dしない場合はDrawもOverrideする（Draw内でGetG2DUtil()しているため）
    */
  //------------------------------------------------------------------
  virtual void  Setup(void* pLayoutBuffer);


  //------------------------------------------------------------------
  /**
    *  @func     SetMessageSpeed
    *  @brief    メッセージスピードを設定する
    */
  //------------------------------------------------------------------
  void  SetMessageSpeed(Savedata::ConfigSave::MSG_SPEED spd);


  //--------------------------------------------------------------------------------------------
  /**
    * @brief     メッセージ表示
    */
  //--------------------------------------------------------------------------------------------
  void  ShowMessage(const gfl2::str::MsgData& rMessageData, const u32 messageID, const bool isImmediateMessage = false, const bool isShowTimerIcon = false, const bool isShowBG = false);
  void  ShowMessage(const gfl2::str::StrBuf* pStrBuf, const bool isImmediateMessage = false, const bool isShowTimerIcon = false, const bool isShowBG = false);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  HideMessage(void);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief キー入力可否設定
   * @note  trueで一時キー入力無効
   */
  /* -------------------------------------------------------------------------*/
  void SetSuspendKey( bool flag );


  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  void  SetFinishMode_None(void)                              {m_MessageWindowManipulator.SetFinishMode_None();}                          /**<  Finishなし              */
  void  SetFinishMode_UserInput(void)                         {m_MessageWindowManipulator.SetFinishMode_UserInput();}                     /**<  Userの入力によって終了  */
  void  SetFinishMode_Timer(const u8 timeoutFrame)            {m_MessageWindowManipulator.SetFinishMode_Timer(timeoutFrame);}             /**<  一定時間後に終了        */
  void  SetFinishMode_UserInputOrTimer(const u8 timeoutFrame) {m_MessageWindowManipulator.SetFinishMode_UserInputOrTimer(timeoutFrame);}  /**<  User/TimerのHybrid      */


  /*
    自身でCreate2Dしていない場合はDrawをOverrideする（Draw内部でGetG2DUtilしているため）
  */
  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);    /*    */
  /*  end implement UIView I/F  */


  //------------------------------------------------------------------
  /**
    *  @brief    m_MessageWindowManipulatorが操作対象にしているG2DUtilを返す
    */
  //------------------------------------------------------------------
  app::util::G2DUtil* GetMessageWindowManipulatorG2DUtil(void) const { return m_MessageWindowManipulator.GetG2DUtil(); }


public:
  static const u8 MESSAGE_WINDOW_TEXT_BOX_PANE_NUM = 2;  //!< メッセージウィンドウに使っていテキストペインの数


protected:
  /*  implement CommonMessageWindowManipulator::IEventHandler  */
  virtual void  CommonMessageWindowManipulator_OnEvent(const CommonMessageWindowManipulator::EventID event);
  /*  end       CommonMessageWindowManipulator::IEventHandler  */


  static const u32                 STR_MAX = 512;

  PARAM                            m_Param;
  CommonMessageWindowManipulator   m_MessageWindowManipulator;
  IEventHandler*                   m_pHandler;
  gfl2::lyt::LytPicture*           m_pPictureBG;                 /**<  BGPicturePane  */
  gfl2::str::StrBuf                m_StrBuf;
  print::WordSet                   m_WordSet;                    //!< 使わないけど必要
};





} /*  namespace Tool  */
} /*  namespace App   */
#endif  /*  #if !defined(COMMONMESSAGEWINDOWVIEW_H_INCLUDED)  */
