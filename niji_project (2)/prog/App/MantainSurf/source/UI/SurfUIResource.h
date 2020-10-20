#if !defined(SURFUIRESOURCE_H_INCLUDED)
#define SURFUIRESOURCE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SurfUIResource.h
 * @date   2016/11/14 Mon. 17:40:17
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <fs/include/gfl2_Fs.h>

#include  <AppLib/include/Tool/GeneralView/Module/ResourceProvider.h>
#include  <NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h>

/*  message  */
#include  <arc_index/message.gaix>
#include  <niji_conv_header/message/msg_mantainsurf.h>

/*  arc */
#include  <arc_def_index/arc_def.h>

/*  layout  */
#include  <arc_index/SurfUI.gaix>
#include  <niji_conv_header/app/surf/SurfUI.h>
#include  <niji_conv_header/app/surf/SurfUI_anim_list.h>
#include  <niji_conv_header/app/surf/SurfUI_pane.h>

/*  surfSound  */
#include  "../SurfSound/SurfSoundManager.h"

namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
static const u32  SURFUI_MESSAGE_ARCDAT = GARC_message_mantainsurf_DAT;


//==============================================================================
/**
  *  @brief  
  *  @date   2017/01/19 Thu. 12:21:55
  */
//==============================================================================
class SurfUIResourceProvider
  : public  app::tool::ResourceProvider
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfUIResourceProvider);
public:
  explicit  SurfUIResourceProvider(app::util::Heap* pUtilHeap=NULL)
              : app::tool::ResourceProvider(pUtilHeap)
              , m_pMessageUtil(NULL)
            {
            }

  virtual   ~SurfUIResourceProvider()
            {
              CleanupMessage();
            }

  /*  message  */
  void  SetupMessage(const u32 dataNum)
        {
          GFL_ASSERT(m_pUtilHeap);
          GFL_ASSERT(!m_pMessageUtil);
          gfl2::heap::HeapBase* pHeapBase = m_pUtilHeap->GetDeviceHeap();
          m_pMessageUtil  = GFL_NEW(pHeapBase) NetAppLib::Message::MessageUtility(pHeapBase, dataNum);
        }

  void  CleanupMessage(void)
        {
          GFL_SAFE_DELETE(m_pMessageUtil);
        }

  NetAppLib::Message::MessageUtility*  GetMessageUtil(void) const  {return m_pMessageUtil;}

protected:
  NetAppLib::Message::MessageUtility*  m_pMessageUtil;
};




} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(SURFUIRESOURCE_H_INCLUDED)  */
