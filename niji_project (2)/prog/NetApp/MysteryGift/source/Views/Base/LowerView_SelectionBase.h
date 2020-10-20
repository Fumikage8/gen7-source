



#if !defined(LOWERVIEW_SELECTIONBASE_H_INCLUDED)
#define LOWERVIEW_SELECTIONBASE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_SelectionBase.h
 * @date   2015/07/31 Fri. 16:04:20
 * @author muto_yu
 * @brief  Gift_SLC_LOW_000.flytを扱うViewの基底
 *         共通のPaneをButtonで扱う場合とPaneListで扱う場合があるため、機能を派生側で分ける
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <NetStatic/NetAppLib/include/UI/NetAppCursorView.h>


/*
  forward declare
*/

namespace NetApp  {
namespace Gift {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.25 Thu.
  */
//==============================================================================
class LowerView_SelectionBase
  : public NetAppLib::UI::NetAppCursorView
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_SelectionBase );
public:
  //------------------------------------------------------------------
  /**
    *  @func     LowerView_SelectionBase
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  LowerView_SelectionBase(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~LowerView_SelectionBase
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~LowerView_SelectionBase();


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
  /** layout work  */
  enum  {
    LAYOUTWORK_SelectionLow,

    LAYOUTWORK_MAX
  };
  /**
    TextBox
  */
  enum  TextBoxID
  {
    TEXTBOX_Large1of3,        /**<  大3択  */
    TEXTBOX_Large2of3,        /**<  大3択  */
    TEXTBOX_Large3of3,        /**<  大3択  */

    TEXTBOX_Small1of9,        /**<  小 1/9  */
    TEXTBOX_Small2of9,        /**<  小 2/9  */
    TEXTBOX_Small3of9,        /**<  小 3/9  */
    TEXTBOX_Small4of9,        /**<  小 4/9  */
    TEXTBOX_Small5of9,        /**<  小 5/9  */
    TEXTBOX_Small6of9,        /**<  小 6/9  */
    TEXTBOX_Small7of9,        /**<  小 7/9  */
    TEXTBOX_Small8of9,        /**<  小 8/9  */
    TEXTBOX_Small9of9,        /**<  小 9/9  */

    TEXTBOX_EventInformation, /**<  イベント説明  */

    TEXTBOX_NUM
  };


  GiftWork*               m_pWork;
  gfl2::lyt::LytTextBox*  m_pTextBoxes[TEXTBOX_NUM];

};


} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LOWERVIEW_SELECTIONBASE_H_INCLUDED)  */
