#if !defined(EVOLUTIONMESSAGEMENU_H_INCLUDED)
#define EVOLUTIONMESSAGEMENU_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionMessageMenu.h
 * @date   2015/08/28 Fri. 18:29:43
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h>


namespace NetApp  {
namespace Evolution {
  class EvolutionWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  EvolutionMessageMenu
  */
//==============================================================================
class EvolutionMessageMenu
  : public NetAppLib::UI::NetAppMessageMenu
{
  GFL_FORBID_COPY_AND_ASSIGN(EvolutionMessageMenu);
public:
  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  EvolutionMessageMenu(EvolutionWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler = NULL);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual ~EvolutionMessageMenu();



  enum  Mode
  {
    /*  main  */

    MODE_
  };


  void  Show(const Mode mode);                                /**<  汎用（パラメータなし）   */

protected:
  //------------------------------------------------------------------
  /**
    *  @brief     CommonMessageWindowViewを生成する
    */
  //------------------------------------------------------------------
//  virtual App::Tool::CommonMessageWindowView*  CreateCommonMessageWindowView(const App::Tool::CommonMessageWindowView::PARAM& rParam, App::Tool::CommonMessageWindowView::IEventHandler* pHandler);


  void  PreShow(void);                                        /**<  showの前に共通でやっておくこと  */
  void  ShowYN(const u32 messageID, const bool isImmediate);  /**<  Y/N  */

  EvolutionWork*                      m_pWork;
//  EvolutionCommonMessageWindowView*   m_pEvolutionCommonMessageWindowView;
//  app::ui::UIView*                    m_pCommonMessageWindowTargetView;
};



} /*  namespace Evolution   */
} /*  namespace NetApp      */
#endif  /*  #if !defined(EVOLUTIONMESSAGEMENU_H_INCLUDED)  */
