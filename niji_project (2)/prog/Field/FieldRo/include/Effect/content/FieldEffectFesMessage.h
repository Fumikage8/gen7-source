#if !defined(FIELDEFFECTFESMESSAGE_H_INCLUDED)
#define FIELDEFFECTFESMESSAGE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   FieldEffectFesMessage.h
 * @date   2015/10/20 Tue. 18:07:56
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <str/include/gfl2_str.h>

#include  "Print/include/WordSet.h"
#include  "AppLib/include/Util/app_util_Heap.h"
#include  "../FieldEffectBase.h"


namespace Field   {
  namespace FieldWindow {class  FieldWindowManager;}

namespace Effect  {
  class EffectLayout;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/20 Tue. 18:09:35
  */
//==============================================================================
class EffectFesMessage
  : public  IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectFesMessage);
public:
  EffectFesMessage(void);
  virtual ~EffectFesMessage();


  struct  SetupData
  {
    void*                             pResource;
    gfl2::heap::HeapBase*             pHeapBase;
    FieldWindow::FieldWindowManager*  pFieldWindowManager;
  };
  void  Setup(const SetupData& rSetupData);


  enum  ColorMode
  {
    COLMODE_Normal,
    COLMODE_Rare,
    COLMODE_Event,

    COLMODE_
  };
  void  Show(const gfl2::str::STRCODE* pName, const gfl2::str::STRCODE* pMessage, const bool isExpand = false, const bool isImmediate = false, const ColorMode colMode = COLMODE_Normal);
  void  Hide(const bool isImmediate = false);

  void SetRegisterMonsNameNo( u32 bufID, MonsNo monsno );


  /*  FieldEffectBase I/F  */
//  virtual void  Initialize( void* in_pResource, gfl2::heap::HeapBase* in_pHeap, Field::IField3DObjectNode*  in_pParent );
//  virtual void  Initialize( void* in_pResource, gfl2::heap::HeapBase* in_pHeap, const gfl2::math::Vector3&  in_vPosition );
  virtual bool  Terminate( void );
//  virtual void  Update( void );
  virtual void  Delete( void );
//  virtual bool  IsAnimationLastFrame(void);



protected:
  enum  TextBoxID
  {
    TEXTBOX_Name,
    TEXTBOX_Message,
    
    TEXTBOX_MAX
  };
  enum  PaneID
  {
    PANE_NamePlateControl,
    
    PANE_MAX
  };


  /*  FieldEffectBase I/F  */
//  virtual void  updateNormal( void );

  SetupData                         m_SetupData;
  app::util::Heap                   m_LocalHeap;
  EffectLayout*                     m_pEffectLayout;
  print::WordSet*                   m_pWordSet;
  gfl2::str::StrBuf*                m_pStrBuf;
  gfl2::str::StrBuf*                m_pTmpBuf;
  /*  panes  */
  gfl2::lyt::LytTextBox*            m_pTextBoxes[TEXTBOX_MAX];
  gfl2::lyt::LytPane*               m_pPanes[PANE_MAX];
};




} /*  namespace Effect  */
} /*  namespace Field   */
#endif  /*  #if !defined(FIELDEFFECTFESMESSAGE_H_INCLUDED)  */
