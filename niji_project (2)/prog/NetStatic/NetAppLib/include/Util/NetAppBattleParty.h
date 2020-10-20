#if !defined(NETAPPBATTLEPARTY_H_INCLUDED)
#define NETAPPBATTLEPARTY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppBattleParty.h
 * @date   2015/12/14 Mon. 19:02:19
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "PokeTool/include/PokeToolBattleParty.h"


namespace NetAppLib {
namespace Util      {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/12/14 Mon. 19:03:37
  */
//==============================================================================
class NetAppBattleParty
  : public  PokeTool::BattleParty
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppBattleParty);
public:
  NetAppBattleParty(gfl2::heap::HeapBase* pHeapBase)
    : PokeTool::BattleParty(pHeapBase)
  {}
  virtual ~NetAppBattleParty(){}


  void  Clear(void)
  {
    GetPokeParty()->Clear();
  }


  int RecreateBattleParty(const Savedata::BOX* pBox, Savedata::BoxPokemon* pBoxPoke, const int teamNo, gfl2::heap::HeapBase* pHeapBase)
  {
    Clear();
    return PokeTool::BattleParty::CreateBattleParty(pBox, pBoxPoke, teamNo, pHeapBase);
  }
};




} /*  namespace Util      */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPBATTLEPARTY_H_INCLUDED)  */
