// ============================================================================
/*
 * @file NetAppCommonTradeUtility.h
 * @brief 通信アプリの交換に関する共通処理を行うクラスです。
 * @date 2015.09.07
 * @author endo_akira
 */
// ============================================================================
#if !defined( NETAPP_COMMON_TRADE_UTILITY_H_INCLUDE )
#define NETAPP_COMMON_TRADE_UTILITY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "pml/include/pokepara/pml_PokemonCoreParam.h"
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "NetStatic/NetEvent/include/EvolutionEvent.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)

enum TradeID
{
  TRADE_ID_MIRACLE,
  TRADE_ID_GTS_SEARCH,
  TRADE_ID_GTS_DOWNLOAD,
  TRADE_ID_P2P,
  TRADE_ID_NPC
};

class NetAppCommonTradeUtility
{
public:

  /**
   * @brief 交換成立後の同期セーブ前のセーブデータの更新処理を行います
   * @param pHeap                                 [in]ヒープ
   * @param eTradeID                              [in]トレードID
   * @param bInBox                                [in]ボックス内のポケモンか手持ちのポケモンか[ true : ボックス内, false : 手持ち ]
   * @param tray                                  [in]トレイ番号
   * @param pos                                   [in]トレイ内の位置
   * @param pRecvPokemonCoreParam                 [in]相手から受け取ったポケモン
   * @param pSendPokemonCoreParam                 [in]自分が送ったポケモン
   * @param bOutCallZukanRegister                 [out]図鑑登録アプリを呼ぶか[ true : 呼ぶ, false : 呼ばない ]
   * @param outZukanRegisterEventParam            [out]図鑑登録アプリを呼び出す際に必要なパラメータ
   * @param bOutCallEvolution                     [out]進化アプリを呼ぶか[ true : 呼ぶ, false : 呼ばない ]
   * @param outEvolutionEventParam                [out]進化アプリを呼び出す際に必要なパラメータ
   *
   * @note  この関数を呼び出すと GameSys::GameData の pml::PokeParty、もしくは Savedata::BoxPokemon に、
   *        相手から受け取ったポケモンを設定しています。
   */
  static void UpdateSaveData(
    gfl2::heap::HeapBase*                         pHeap,
    NetAppLib::Util::TradeID                      eTradeID,
    bool                                          bInBox,
    u32                                           tray,
    u32                                           pos,
    pml::pokepara::CoreParam*                     pRecvPokemonCoreParam,
    pml::pokepara::CoreParam*                     pSendPokemonCoreParam,
    bool&                                         bOutCallZukanRegister,
    app::event::ZukanRegisterEventParam&          outZukanRegisterEventParam,
    bool&                                         bOutCallEvolution,
    NetEvent::Evolution::EvolutionEventParam&     outEvolutionEventParam );

};


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPP_COMMON_TRADE_UTILITY_H_INCLUDE
