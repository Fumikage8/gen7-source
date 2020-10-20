
#if !defined(EVOLUTIONWORK_H_INCLUDED)
#define EVOLUTIONWORK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionWork.h
 * @date   2015/08/28 Fri. 18:20:07
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

#include  "../../include/EvolutionProcParam.h"
#include  "../Views/EvolutionViewContainer.h"


namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  */
//==============================================================================
class EvolutionWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( EvolutionWork );

public:
  EvolutionWork(void);
  virtual ~EvolutionWork();

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void                    SetViewContainer(EvolutionViewContainer* pViewContainer)  {m_pEvolutionViewContainer = pViewContainer;}
  EvolutionViewContainer* GetViewContainer(void)                                    {return m_pEvolutionViewContainer;}


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
  EvolutionViewContainer*             m_pEvolutionViewContainer;
};



} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONWORK_H_INCLUDED)  */

