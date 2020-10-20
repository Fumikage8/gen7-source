// ============================================================================
/*
 * @file PokeTradeEvent.h
 * @brief ポケモン交換アプリのイベントです
 * @date 2015.11.25
 * @author endo_akira
 */
// ============================================================================
#if !defined( POKETRADEEVENT_H_INCLUDE )
#define POKETRADEEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"
#include "pml/include/pml_PokePara.h"
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "NetStatic/NetEvent/include/EvolutionEvent.h"
#include "System/include/GameStrLen.h"
#include "Savedata/include/MyStatus.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(AppDemo)
class AppDemoProc_MiracleTrade;
GFL_NAMESPACE_END(AppDemo)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


enum{
  STRLEN_POKETRADE_NPC_NAME = System::STRLEN_PLAYER_NAME + System::EOM_LEN,
};

typedef struct
{
  pml::pokepara::PokemonParam*    pRecvPokemon;   // もらうポケモン
  u32                             tray;           // トレイ番号(※手持ちの場合は App::Box::RETURN_TEMOTI を設定)
  u32                             pos;            // トレイ内の位置(手持ちの場合は手持ち内の位置)
  gfl2::str::STRCODE              npcName[ STRLEN_POKETRADE_NPC_NAME ];
} PokeTradeEventParam;


class PokeTradeEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeTradeEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, App::Event::PokeTradeEventParam* pPokeTradeEventParam );

  PokeTradeEvent( gfl2::heap::HeapBase* pHeap );
  virtual~PokeTradeEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

private:

  void setPokeTradeEventParam( App::Event::PokeTradeEventParam* pPokeTradeEventParam );

  void updateSaveData();

  void bootDemoProc();
  void bootZukanRegisterEvent( GameSys::GameManager* pGameManager );
  void bootEvolutionEvent( GameSys::GameManager* pGameManager );

private:
  
  enum Sequence
  {
    SEQUENCE_DEMO_START,
    SEQUENCE_DEMO_WAIT,
    SEQUENCE_ZUKAN_REGISTER,
    SEQUENCE_EVOLUTION
  };


  App::AppDemo::AppDemoProc_MiracleTrade*      m_pDemoProc;
  pml::pokepara::PokemonParam                  m_DemoSendPokemon;
  pml::pokepara::PokemonParam                  m_DemoRecvPokemon;
  u32                                          m_Tray;
  u32                                          m_Pos;
  bool                                         m_bInBox;
  bool                                         m_bCallZukanRegister;
  app::event::ZukanRegisterEventParam          m_ZukanRegisterEventParam;
  bool                                         m_bCallEvolution;
  NetEvent::Evolution::EvolutionEventParam     m_EvolutionEventParam;
  Savedata::MyStatus                           m_TradePersonStatus;

};


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // POKETRADEEVENT_H_INCLUDE
