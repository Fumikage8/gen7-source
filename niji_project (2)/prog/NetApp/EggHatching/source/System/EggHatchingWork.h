
#if !defined(EGGHATCHINGWORK_H_INCLUDED)
#define EGGHATCHINGWORK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingWork.h
 * @date   2015/11/24 Tue. 12:40:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <str/include/gfl2_Str.h>
#include  <arc_index/message.gaix>
#include  <NetStatic/NetAppLib/include/System/ApplicationWorkBase.h>
#include  <NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h>

#include  "../../include/EggHatchingProcParam.h"
#include  "../Views/EggHatchingViewContainer.h"


namespace NetApp    {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  */
//==============================================================================
class EggHatchingWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( EggHatchingWork );

public:
  EggHatchingWork(void);
  virtual ~EggHatchingWork();

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void                      SetViewContainer(EggHatchingViewContainer* pViewContainer)  {m_pViewContainer = pViewContainer;}
  EggHatchingViewContainer* GetViewContainer(void)                                      {return m_pViewContainer;}


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  NetAppLib::Message::MessageUtility&  GetMessageUtility(void)   {return m_MessageUtility;}




  /*
    members
  */
  APP_PARAM*    m_pAppParam;
  bool          m_bDemoStarted;

private:
  /*  message  */
  NetAppLib::Message::MessageUtility  m_MessageUtility;
  EggHatchingViewContainer*           m_pViewContainer;
};



} /*  namespace EggHatching */
} /*  namespace NetApp      */
#endif  /*  #if !defined(EGGHATCHINGWORK_H_INCLUDED)  */
