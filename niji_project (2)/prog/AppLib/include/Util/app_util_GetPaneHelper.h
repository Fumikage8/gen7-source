
#if !defined(APP_UTIL_GETPANEHELPER_H_INCLUDED)
#define APP_UTIL_GETPANEHELPER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   app_util_GetPaneHelper.h
 * @date   2015/07/15 Wed. 15:00:24
 * @date   2015/08/03 Mon. 18:19:22 NetAppLibより移動
 * @author muto_yu
 * @brief  LayoutWorkから各種Paneを取得するためのヘルパ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <Layout/include/gfl2_LytSys.h>


namespace app   {
namespace util  {
  class G2DUtil;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  GetPaneHelper
  *  @brief  
  *  @code  
  *     //  使用例
  *     func()
  *     {
  *       GetPaneHelper             helper(pLayoutWork, pMultiResID);
  *       gfl2::lyt::LytTextBox*    pTextBox[3];
  *       gfl2::lyt::LytPicture*    pPicture;
  *
  *       pTextBox[0]  = helper.Push(PANEID_PARTS_00).Push(PANEID_PARTS_01).GetTextBoxPane(PANEID_TEXTBOX_00);
  *       pTextBox[1]  = helper                                            .GetTextBoxPane(PANEID_TEXTBOX_01);
  *       pTextBox[2]  = helper                                            .GetTextBoxPane(PANEID_TEXTBOX_02);
  *       pPicture     = helper                                            .GetPicturePane(PANEID_PICTURE);
  *  @endcode
  *  @date   2015/07/15 Wed. 15:01:25
  */
//==============================================================================
class GetPaneHelper
{
  GFL_FORBID_COPY_AND_ASSIGN(GetPaneHelper);
public:
  GetPaneHelper(gfl2::lyt::LytWk* pLayoutWork, gfl2::lyt::LytMultiResID* pMultiResID);
  GetPaneHelper(G2DUtil* pG2D, const u32 layoutWorkID = 0, const u32 resourceIndex = 0);
  virtual ~GetPaneHelper();


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  GetPaneHelper&  Clear(void);
  

  //------------------------------------------------------------------
  /**
    *  @brief    PartsPaneスタックの先頭に追加
    */
  //------------------------------------------------------------------
  GetPaneHelper&  Push(const gfl2::lyt::LytPaneIndex paneID);


  //------------------------------------------------------------------
  /**
    *  @brief    PartsPaneスタックの先頭を削除
    */
  //------------------------------------------------------------------
  GetPaneHelper&  Pop(void);

  //------------------------------------------------------------------
  /**
    *  @brief    PartsPaneスタックの先頭を返す
    */
  //------------------------------------------------------------------
  gfl2::lyt::LytParts*  Peek(void);


  /*
    各種Paneの取得
  */
  gfl2::lyt::LytPane*       GetPane(const gfl2::lyt::LytPaneIndex paneID);
  gfl2::lyt::LytPicture*    GetPicturePane(const gfl2::lyt::LytPaneIndex paneID);
  gfl2::lyt::LytTextBox*    GetTextBoxPane(const gfl2::lyt::LytPaneIndex paneID);
  gfl2::lyt::LytWindow*     GetWindowPane(const gfl2::lyt::LytPaneIndex paneID);
  gfl2::lyt::LytBounding*   GetBoundingPane(const gfl2::lyt::LytPaneIndex paneID);
  gfl2::lyt::LytParts*      GetPartsPane(const gfl2::lyt::LytPaneIndex paneID);

#if 0
  /*
    ここを有効にすればstreamのような記述が可能になります。
    
    GetPaneHelper helper(pLayoutWork, pMultiResID);
    gfl2::lyt::LytTextBox*  pTextBox[2];

    pTextBox[0]  = (helper.Clear() << PANE_PARTS_A_ID << PANE_PARTS_B_ID).GetTextBoxPane(PANE_TEXTBOX_00_ID);
    pTextBox[1]  = (helper                                              ).GetTextBoxPane(PANE_TEXTBOX_01_ID);
    
  */
  GetPaneHelper&  <<(const gfl2::lyt::LytPaneIndex paneID)
  {
    return Push(paneID);
  }
#endif


protected:
  template<class TargetPaneClass>
  TargetPaneClass*  GetPaneCore(const gfl2::lyt::LytPaneIndex paneID)
  {
    TargetPaneClass*  pTargetPane  = NULL;


    if(!m_StackDepth)
    {
      GetPane(&pTargetPane, paneID);
    }
    else
    {
      GetPane(&pTargetPane, m_pPartsStack[m_StackDepth-1], paneID);
    }

    return pTargetPane;
  }


  void   GetPane(gfl2::lyt::LytPane**      ppPane, const gfl2::lyt::LytPaneIndex paneID)     {*ppPane  = m_pLayout->GetPane(paneID);}
  void   GetPane(gfl2::lyt::LytPicture**   ppPane, const gfl2::lyt::LytPaneIndex paneID)     {*ppPane  = m_pLayout->GetPicturePane(paneID);}
  void   GetPane(gfl2::lyt::LytTextBox**   ppPane, const gfl2::lyt::LytPaneIndex paneID)     {*ppPane  = m_pLayout->GetTextBoxPane(paneID);}
  void   GetPane(gfl2::lyt::LytWindow**    ppPane, const gfl2::lyt::LytPaneIndex paneID)     {*ppPane  = m_pLayout->GetWindowPane(paneID);}
  void   GetPane(gfl2::lyt::LytBounding**  ppPane, const gfl2::lyt::LytPaneIndex paneID)     {*ppPane  = m_pLayout->GetBoundingPane(paneID);}
  void   GetPane(gfl2::lyt::LytParts**     ppPane, const gfl2::lyt::LytPaneIndex paneID)     {*ppPane  = m_pLayout->GetPartsPane(paneID);}

  void   GetPane(gfl2::lyt::LytPane**      ppPane, gfl2::lyt::LytParts* pParts, const gfl2::lyt::LytPaneIndex paneID)  {*ppPane  = m_pLayout->GetPane(pParts, paneID, m_pLayoutMultiResourceID);}
  void   GetPane(gfl2::lyt::LytPicture**   ppPane, gfl2::lyt::LytParts* pParts, const gfl2::lyt::LytPaneIndex paneID)  {*ppPane  = m_pLayout->GetPicturePane(pParts, paneID, m_pLayoutMultiResourceID);}
  void   GetPane(gfl2::lyt::LytTextBox**   ppPane, gfl2::lyt::LytParts* pParts, const gfl2::lyt::LytPaneIndex paneID)  {*ppPane  = m_pLayout->GetTextBoxPane(pParts, paneID, m_pLayoutMultiResourceID);}
  void   GetPane(gfl2::lyt::LytWindow**    ppPane, gfl2::lyt::LytParts* pParts, const gfl2::lyt::LytPaneIndex paneID)  {*ppPane  = m_pLayout->GetWindowPane(pParts, paneID, m_pLayoutMultiResourceID);}
  void   GetPane(gfl2::lyt::LytBounding**  ppPane, gfl2::lyt::LytParts* pParts, const gfl2::lyt::LytPaneIndex paneID)  {*ppPane  = m_pLayout->GetBoundingPane(pParts, paneID, m_pLayoutMultiResourceID);}
  void   GetPane(gfl2::lyt::LytParts**     ppPane, gfl2::lyt::LytParts* pParts, const gfl2::lyt::LytPaneIndex paneID)  {*ppPane  = m_pLayout->GetPartsPane(pParts, paneID, m_pLayoutMultiResourceID);}


  /*
    members
  */
  static const u32    PARTS_STACK_DEPTH_MAX  = 8;

  gfl2::lyt::LytWk*           m_pLayout;
  gfl2::lyt::LytMultiResID*   m_pLayoutMultiResourceID;
  u32                         m_StackDepth;
  gfl2::lyt::LytParts*        m_pPartsStack[PARTS_STACK_DEPTH_MAX];

};


} /*  namespace UI        */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(APP_UTIL_GETPANEHELPER_H_INCLUDED)  */
