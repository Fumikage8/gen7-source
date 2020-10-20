#if !defined(UPPERVIEW_MESSAGE_H_INCLUDED)
#define UPPERVIEW_MESSAGE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Message.h
 * @date   2015/07/13 Mon. 19:51:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"


namespace NetApp  {
namespace Live    {
  class LiveWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  UpperView_Message
  *  @brief  上画面 メッセージ
  *  @date   2015/07/13 Mon. 19:54:16
  */
//==============================================================================
class UpperView_Message
  : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView_Message );
public:
  UpperView_Message(LiveWork* pWork);
  virtual ~UpperView_Message();


  /*  implement UIView I/F  */
//  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
   /*  endimplement UIView I/F  */

protected:
  /*
    members
  */
  LiveWork*     m_pWork;


  enum  TextBoxID
  {
    TEXTBOX_Title,          /**<  上部タイトル  */
    TEXTBOX_Information,    /**<  情報          */
    
    TEXTBOX_MAX
  };
  gfl2::lyt::LytTextBox*    m_TextBoxes[TEXTBOX_MAX];
};




} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(UPPERVIEW_MESSAGE_H_INCLUDED)  */
