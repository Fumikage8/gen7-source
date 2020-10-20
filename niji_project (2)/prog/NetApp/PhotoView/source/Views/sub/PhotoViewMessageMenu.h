#if !defined(PHOTOVIEWMESSAGEMENU_H_INCLUDED)
#define PHOTOVIEWMESSAGEMENU_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewMessageMenu.h
 * @date   2017/02/07 Tue. 15:42:47
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


namespace NetApp   {
namespace PhotoView {
  class PhotoViewWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/20 Wed. 17:47:35
  */
//==============================================================================
class MessageMenu
  : public  NetAppLib::UI::NetAppMessageMenu
{
  GFL_FORBID_COPY_AND_ASSIGN(MessageMenu);
public:

  /*  ctor/dtor  */
  MessageMenu(PhotoViewWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler = NULL);
  virtual ~MessageMenu();

  enum  Mode
  {
    MODE_None,

    MODE_ConfirmUGC,    /**<  UGC閲覧に関する確認  */

    MODE_MAX
  };

  void  Show(const Mode mode);
  void  Hide(void)  {HideMessage();}


protected:
  void  PreShow(void);
  void  ShowYN(const u32 messageID, const bool isImmediate);

  /*
    members
  */
  PhotoViewWork*  m_pWork;
  Mode            m_Mode;
};



} /*  namespace PhotoView */
} /*  namespace NetApp   */
#endif  /*  #if !defined(PHOTOVIEWMESSAGEMENU_H_INCLUDED)  */
