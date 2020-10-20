//==============================================================================
/**
 * @file   NetAppPokePartyUtility.cpp
 * @date   2015/10/15 Thu. 19:02:02
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/UI/UIView.h"

#include  "NetStatic/NetAppLib/Include/System/ApplicationWorkBase.h"
#include  "NetStatic/NetAppLib/Include/UI/NetAppPokePartyUtility.h"


namespace NetAppLib {
namespace UI        {
// =============================================================================
/**/
//==============================================================================
NetAppPokePartyUtility::NetAppPokePartyUtility(System::ApplicationWorkBase* pWorkBase)
  : NetAppPokeparaUtility(pWorkBase)
  , m_PartyCapacity(0)

{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
NetAppPokePartyUtility::~NetAppPokePartyUtility()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokePartyUtility::Setup(const u32 partyCapacity, app::ui::UIView*  pTargetView, const u32 layoutID)
{
  const u32 iconCapacity  = pml::PokeParty::MAX_MEMBERS * partyCapacity;

  m_PartyCapacity = iconCapacity;
  NetAppPokeparaUtility::Setup(iconCapacity, pTargetView, layoutID);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  NetAppPokePartyUtility::Cleanup(void)
{
  m_PartyCapacity = 0;
  return NetAppPokeparaUtility::Cleanup();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokePartyUtility::SetPokeParty(const u32 partyIndex, const pml::PokeParty* pPokeParty)
{
  if(!m_PartyCapacity)              return;
  if(partyIndex >= m_PartyCapacity) return;
  if(!pPokeParty)                   return;

  const u32 membersMax      = pml::PokeParty::MAX_MEMBERS;
  const u32 iconIndexStart  = partyIndex  * membersMax;

  for(u32 memberIndex = 0; memberIndex < membersMax; ++memberIndex)
  {
    SetCoreParam(iconIndexStart + memberIndex, pPokeParty->GetMemberPointerConst(memberIndex));
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokePartyUtility::RegisterStatusResource(const u32  partyIndex, const u32 iconIndex, const RegisterInfo& rRegisterInfo)
{
  if(!m_PartyCapacity)              return;
  if(partyIndex >= m_PartyCapacity) return;

  NetAppPokeparaUtility::RegisterStatusResource(partyIndex * pml::PokeParty::MAX_MEMBERS + iconIndex, rRegisterInfo);
}



} /*  namespace UI        */
} /*  namespace NetAppLib */
