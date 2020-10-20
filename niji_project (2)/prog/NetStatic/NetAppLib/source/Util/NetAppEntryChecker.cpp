// ============================================================================
/*
 * @file NetAppEntryChecker.cpp
 * @brief 参加チェック系のユーティリティ
 * @date 2016.02.08
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/Util/NetAppEntryChecker.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


NetAppEntryChecker::NetAppEntryChecker()
{
}


NetAppEntryChecker::~NetAppEntryChecker()
{
}


bool NetAppEntryChecker::CheckPokemonCountForTrade( gfl2::heap::HeapBase* pHeap )
{
  bool bResult = false;

  pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

  Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

  u32 pokePartyPokemonCount = pPokeParty->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_NOT_EGG );

  u32 boxAllPokemonCount = pBoxPokemon->GetPokemonCountAll( pHeap, false );

  u32 allPokemonCount = pokePartyPokemonCount + boxAllPokemonCount;

  if( allPokemonCount >= 2 )
  {
    bResult = true;
  }

  return bResult;
}


bool NetAppEntryChecker::CheckPokemonCountForDoubleBattle( gfl2::heap::HeapBase* pHeap )
{
  bool bResult = false;

  pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

  Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

  u32 pokePartyPokemonCount = pPokeParty->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_NOT_EGG );

  u32 boxAllPokemonCount = pBoxPokemon->GetPokemonCountAll( pHeap, false );

  if( pokePartyPokemonCount >= 2 || boxAllPokemonCount >= 2 )
  {
    bResult = true;
  }

  return bResult;
}


bool NetAppEntryChecker::CheckTradePenalty()
{
  bool bResult = false;

  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  if( pJoinFestaDataSave->GetGradePenaltyCount() == 0 )
  {
    bResult = true;
  }

  return bResult;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief GTSが起動出来るかチェック
 *        
 *        GTSにあずけていて、手持ち・BOXがFULLだと起動できない        
 *        
 * @retval true:起動出来る
 * @retval false:起動出来ない
 */
/* ----------------------------------------------------------------------------------------- */
bool NetAppEntryChecker::CheckFreeSpaceForGtsPlay( gfl2::heap::HeapBase* pHeap )
{
  Savedata::GtsData* pGtsData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

  // 預けていないならそもそもチェックしない
  if( !pGtsData->GetDepositFlag() )
  {
    return true;
  }

  pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

  Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

  u32 pokePartyPokemonCount = pPokeParty->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_ALL );

  u32 boxAllSpaceCount = pBoxPokemon->GetSpaceCountAll( pHeap );

  if( pokePartyPokemonCount == pml::PokeParty::MAX_MEMBERS &&
      boxAllSpaceCount == 0 )
  {
    return false;
  }

  return true;
}

/* ----------------------------------------------------------------------------------------- */
/**
  * @brief GTSが起動出来るかチェック
  *        
  *        GTSにあずけていて、手持ち・BOXに１匹いれば起動できる
  *        
  * @retval true:起動出来る
  * @retval false:起動出来ない
  */
/* ----------------------------------------------------------------------------------------- */
bool NetAppEntryChecker::CheckPokemonCountForGtsPlay( gfl2::heap::HeapBase* pHeap )
{
  bool bResult = false;

  Savedata::GtsData* pGtsData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGtsData();

  // 預けている
  if( pGtsData->GetDepositFlag() )
  {
    pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

    Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

    u32 pokePartyPokemonCount = pPokeParty->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_NOT_EGG );

    u32 boxAllPokemonCount = pBoxPokemon->GetPokemonCountAll( pHeap, false );

    u32 allPokemonCount = pokePartyPokemonCount + boxAllPokemonCount;

    if( allPokemonCount >= 1 )
    {
      bResult = true;
    }
  }

  return bResult;
}

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)
