
#if !defined(UPPERVIEW_MESSAGEBASE_H_INCLUDED)
#define UPPERVIEW_MESSAGEBASE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_MessageBase.h
 * @date   2015/07/31 Fri. 20:18:39
 * @author muto_yu
 * @brief  ふしぎなおくりもの 上画面 メッセージ描画
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"


/*
  forward declare
*/


namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  UpperView_MessageBase
  *  @brief  Draw: 上画面 メッセージ描画
  *  @date   2015.05.19 Tue.
  */
//==============================================================================
class UpperView_MessageBase
  : public NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView_MessageBase );

public:
  UpperView_MessageBase(GiftWork* pWork, const bool isLower = false);
  virtual ~UpperView_MessageBase();


  void  UpdateReceivedDataInfo(const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA* pReceiveData, const bool bEnableNGMessage = true);



  /*
    UIView
  */
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void  Update(void);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  /*
    end UIView
  */

protected:
  /**
    TextBox
  */
  enum  TextBoxID
  {
    TEXTBOX_Large,        /**<  大  */

    TEXTBOX_1of2,         /**<  2行表示1行目  */
    TEXTBOX_2of2,         /**<  2行表示2行目  */

    TEXTBOX_Serial_1of4,  /**<  シリアルコード 1/4  */
    TEXTBOX_Serial_2of4,  /**<  シリアルコード 2/4  */
    TEXTBOX_Serial_3of4,  /**<  シリアルコード 3/4  */
    TEXTBOX_Serial_4of4,  /**<  シリアルコード 4/4  */

    TEXTBOX_DataInfo_EventName,   /**<  受信データ情報：配信名       */
    TEXTBOX_DataInfo_Description, /**<  受信データ情報：説明         */
    TEXTBOX_DataInfo_Status,      /**<  受信データ情報：受け取り状態 */

    TEXTBOX_MAX
  };


  /**
    表示モード
  */
  enum  LayoutMode
  {
    LAYOUT_MessageLarge,    /**<  メッセージプレート大      */
    LAYOUT_MessageSmall,    /**<  メッセージプレート小      */
    LAYOUT_SerialCode,      /**<  シリアルコード            */
    LAYOUT_MessageWindow,   /**<  汎用メッセージウインドウ  */
    LAYOUT_ReceivedData,    /**<  受信データ詳細            */

    LAYOUT_
  };
  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  void  SetLayoutMode(const LayoutMode layoutMode);


  //------------------------------------------------------------------
  /**
    *  @func     SetText
    *  @brief    TextBoxのTextを設定
    */
  //------------------------------------------------------------------
  void  SetText(const TextBoxID textBoxID, const u32 messageID);


  //------------------------------------------------------------------
  /**
    *  @func     Set2LineText
    *  @brief    2行表示用プレート（VIEWMODE_MessageSmall）へのメッセージ設定
    */
  //------------------------------------------------------------------
  void  Set2LineText(const u32 messageID);

  //------------------------------------------------------------------
  /**
    *  @func     ShowMessageWindowText
    *  @brief    汎用メッセージウインドウの表示
    *  @param    messageID          [in]  表示するメッセージのID（UIView内）
    *  @param    isImmediateMessage [in]  メッセージ表示を瞬時に（ストリーム表示せず）行う
    */
  //------------------------------------------------------------------
  void  ShowMessageWindow(const u32 messageID, const bool isImmediateMessage = false, const bool isShowTimerIcon = false);


  //------------------------------------------------------------------
  /**
    *  @func     ShowTimerIcon
    *  @brief    タイマーアイコンの表示制御
    */
  //------------------------------------------------------------------
  void  ShowTimerIcon(const bool isVisible);

  /*
    members
  */
  GiftWork*                   m_pWork;
  gfl2::lyt::LytTextBox*      m_pTextBoxes[TEXTBOX_MAX];


  /*    */
  print::MessageWindow*                             m_p2LineMessageWindow;    /**<  2行表示用MessageWindow    */

  /*  汎用メッセージwindow  */
  gfl2::lyt::LytTextBox*                            m_pTextBoxForImmediate;   /**<  即時表示用TextBox         */
  gfl2::lyt::LytPane*                               m_pWindowRootPane;        /**<  MessageWindowの制御用Pane */
  gfl2::lyt::LytParts*                              m_pTimerIconPartsPane;    /**<  タイマーアイコン          */

};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(UPPERVIEW_MESSAGEBASE_H_INCLUDED)  */
