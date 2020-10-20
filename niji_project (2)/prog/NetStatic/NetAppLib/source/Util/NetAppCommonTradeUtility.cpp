// ============================================================================
/*
 * @file NetAppCommonTradeUtility.cpp
 * @brief 通信アプリの交換に関する共通処理を行うクラスです。
 * @date 2015.09.07
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/Util/NetAppCommonTradeUtility.h"
#include "PokeTool/include/PokeToolTrade.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "PokeTool/include/PokeMemo.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


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
void NetAppCommonTradeUtility::UpdateSaveData(
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
  NetEvent::Evolution::EvolutionEventParam&     outEvolutionEventParam )
{
  GFL_PRINT( "----------------------------------------------------------------------------------\n" );
  GFL_PRINT( "--------------------NetAppCommonTradeUtility::UpdateSaveData----------------------\n" );

  GFL_PRINT( "eTradeID = %d\n", eTradeID );
  GFL_PRINT( "bInBox   = %d\n", bInBox );
  GFL_PRINT( "tray     = %d\n", tray );
  GFL_PRINT( "pos      = %d\n", pos );


  GameSys::GameData*      pGameData   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Savedata::Record*       pRecord     = pGameData->GetRecordSaveData();
  Savedata::MyStatus*     pMyStatus   = pGameData->GetPlayerStatus();
  Savedata::ZukanData*    pZukanData  = pGameData->GetZukanData();
  Savedata::BoxPokemon*   pBoxPokemon = pGameData->GetBoxPokemon();
  pml::PokeParty*         pPokeParty  = pGameData->GetPlayerParty();

  PokeTool::PokeGetUtil::PutPlace::Tag    zukanRegisterParamPutPlace = PokeTool::PokeGetUtil::PutPlace::NONE;
  PokeTool::PokeGetUtil::PutSpace::Tag    zukanRegisterParamPutSpace = PokeTool::PokeGetUtil::PutSpace::NONE;
  u32                                     zukanRegisterParamPos0     = 0;
  u32                                     zukanRegisterParamPos1     = 0;

  // レコードの加算
  {
    switch( eTradeID )
    {
      case TRADE_ID_MIRACLE:
      {
        Savedata::IncRecord( Savedata::Record::RECID_MIRACLE_TRADE_COUNT );
        PokeDiary::Set( PDID_COUNT_MTRADE, pSendPokemonCoreParam->GetMonsNo(), pRecvPokemonCoreParam->GetMonsNo() );
      }
      break;
      
      case TRADE_ID_GTS_SEARCH:
      case TRADE_ID_GTS_DOWNLOAD:
      {
        Savedata::IncRecord( Savedata::Record::RECID_GTS_TRADE_COUNT );
        PokeDiary::Set( PDID_COUNT_GTS_POKETRADE, pSendPokemonCoreParam->GetMonsNo(), pRecvPokemonCoreParam->GetMonsNo() );
      }
      break;
            
      case TRADE_ID_P2P:
      {
        Savedata::IncRecord( Savedata::Record::RECID_COMM_TRADE );

        // タマゴの場合は、MONSNO_TAMAGOを入れる
        u32 sendMonsNo = (pSendPokemonCoreParam->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == true ) ? MONSNO_TAMAGO : pSendPokemonCoreParam->GetMonsNo(); 
        u32 recvMonsNo = (pRecvPokemonCoreParam->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == true ) ? MONSNO_TAMAGO : pRecvPokemonCoreParam->GetMonsNo(); 
        PokeDiary::Set( PDID_COUNT_P2PTRADE, sendMonsNo, recvMonsNo );
      }
      break;
    }
  }

  switch( eTradeID )
  {
    case TRADE_ID_MIRACLE:
    case TRADE_ID_GTS_SEARCH:
    case TRADE_ID_GTS_DOWNLOAD:
    case TRADE_ID_P2P:
    {
      // 通信交換後のポケモンパラメータ更新処理
      PokeTool::UpdatePokemonAfterCommTrade( pRecvPokemonCoreParam, pMyStatus );

      if( pRecvPokemonCoreParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
      {
        outZukanRegisterEventParam.callerType = PokeTool::PokeGetUtil::CallerType::KOUKAN_TAMAGO_NET;
      }
      else
      {
        outZukanRegisterEventParam.callerType = PokeTool::PokeGetUtil::CallerType::KOUKAN_NET;
      }
    }
    break;

    case TRADE_ID_NPC:
    {
      if( pRecvPokemonCoreParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
      {
        outZukanRegisterEventParam.callerType = PokeTool::PokeGetUtil::CallerType::KOUKAN_TAMAGO;
      }
      else
      {
        outZukanRegisterEventParam.callerType = PokeTool::PokeGetUtil::CallerType::KOUKAN;
      }
    }
    break;
  }


  // ポケモンデータを書き換える処理
  {
    pml::pokepara::PokemonParam pokemonParam( pHeap, *pRecvPokemonCoreParam );
    outZukanRegisterEventParam.pp = &pokemonParam;
    PokeTool::PokeGetUtil::ReturnType::Tag result;

    // GTSのダウンロード時
    if( eTradeID == TRADE_ID_GTS_DOWNLOAD )
    {
      if( pHeap )
      {
        // Boxに追加する時
        if( bInBox )
        {
          u32 spaceTray = tray;
          u32 spacePos = pos;

          bool bBoxSpaceFlag = false;

          u32 spaceCount = pBoxPokemon->GetSpaceCountAll( pHeap );

          if( spaceCount >= 1 )
          {
            bBoxSpaceFlag = pBoxPokemon->GetSpacePos( spaceTray, spacePos, pHeap );
          }

          // Boxに空きがある
          if( bBoxSpaceFlag )
          {
            zukanRegisterParamPutPlace = PokeTool::PokeGetUtil::PutPlace::BOX;
            zukanRegisterParamPutSpace = PokeTool::PokeGetUtil::PutSpace::FIX;
            zukanRegisterParamPos0     = spaceTray;
            zukanRegisterParamPos1     = spacePos;
          }
          // Boxに空きがない
          else
          {
            // 手持ちに余裕がある時
            if( pPokeParty->GetMemberCount() < pml::PokeParty::MAX_MEMBERS )
            {
              zukanRegisterParamPutPlace = PokeTool::PokeGetUtil::PutPlace::PARTY;
              zukanRegisterParamPutSpace = PokeTool::PokeGetUtil::PutSpace::FREE;
              zukanRegisterParamPos0     = 0;
              zukanRegisterParamPos1     = 0;
            }
            // 手持ちに余裕がない時
            else
            {
              GFL_ASSERT_MSG( NULL, "Error !! : Failed to add in the box and party.\n" );
            }
          }
        }
        // 手持ちに追加する時
        else
        {
          // 手持ちに余裕がある時
          if( pPokeParty->GetMemberCount() < pml::PokeParty::MAX_MEMBERS )
          {
            zukanRegisterParamPutPlace = PokeTool::PokeGetUtil::PutPlace::PARTY;
            zukanRegisterParamPutSpace = PokeTool::PokeGetUtil::PutSpace::FREE;
            zukanRegisterParamPos0     = 0;
            zukanRegisterParamPos1     = 0;
          }
          // 手持ちに余裕がない時
          else
          {
            u32 spaceTray = 0;
            u32 spacePos = 0;

            bool bBoxSpaceFlag = false;

            u32 spaceCount = pBoxPokemon->GetSpaceCountAll( pHeap );

            if( spaceCount >= 1 )
            {
              bBoxSpaceFlag = pBoxPokemon->GetSpacePos( spaceTray, spacePos, pHeap );
            }

            // Boxに空きがある
            if( bBoxSpaceFlag )
            {
              zukanRegisterParamPutPlace = PokeTool::PokeGetUtil::PutPlace::BOX;
              zukanRegisterParamPutSpace = PokeTool::PokeGetUtil::PutSpace::FIX;
              zukanRegisterParamPos0     = spaceTray;
              zukanRegisterParamPos1     = spacePos;
            }
            // Boxに空きがない
            else
            {
              GFL_ASSERT_MSG( NULL, "Error !! : Failed to add in the party and box.\n" );
            }
          }
        }
      }
    }
    // GTSのダウンロード以外の時(ミラクル交換、GTSのサーチ、P2P交換)
    else
    {
      zukanRegisterParamPutPlace = ( bInBox ) ? PokeTool::PokeGetUtil::PutPlace::BOX : PokeTool::PokeGetUtil::PutPlace::PARTY;
      zukanRegisterParamPutSpace = PokeTool::PokeGetUtil::PutSpace::FIX;
      zukanRegisterParamPos0     = ( bInBox ) ? tray : pos;
      zukanRegisterParamPos1     = pos;
    }

    // 進化前
    {
      // 図鑑登録アプリを呼ぶか呼ばないかの判定フラグ
      {
        result = PokeTool::PokeGetUtil::UpdatePokeAtPokeGet( pHeap, &pokemonParam, outZukanRegisterEventParam.callerType, &outZukanRegisterEventParam.returnParam );

        bOutCallZukanRegister = ( result == PokeTool::PokeGetUtil::ReturnType::CALL_ZUKAN_REGISTER ) ? true : false;
      }
    
      // 進化前のポケモンを図鑑登録
      PokeTool::PokeGetUtil::PutPoke( pHeap, &pokemonParam, zukanRegisterParamPutPlace, zukanRegisterParamPutSpace, zukanRegisterParamPos0, zukanRegisterParamPos1, &outZukanRegisterEventParam.returnParam );
    }

    // 進化後
    {
      // 進化アプリを呼ぶか呼ばないかの判定フラグ
      {
        bOutCallEvolution = pokemonParam.CanEvolveByTrade( pSendPokemonCoreParam, &outEvolutionEventParam.m_MonsNoEvolve, &outEvolutionEventParam.m_EvolveRootNum );
      }

      // 進化する時
      if( bOutCallEvolution )
      {
        // MCat #434, #890 の対応
        // 進化したポケモンは、進化前に配置したポケモンと入れ替える(置き換える)

        GFL_PRINT( "Pokemon evolution.\n" );

        // 進化させる
        pokemonParam.Evolve( outEvolutionEventParam.m_MonsNoEvolve, outEvolutionEventParam.m_EvolveRootNum );

        u32 pos0 = outZukanRegisterEventParam.returnParam.pos0;
        u32 pos1 = outZukanRegisterEventParam.returnParam.pos1;

        GFL_PRINT( "pos0                       = %d\n", pos0 );
        GFL_PRINT( "pos1                       = %d\n", pos1 );
        GFL_PRINT( "zukanRegisterParamPutPlace = %d\n", zukanRegisterParamPutPlace );
        GFL_PRINT( "zukanRegisterParamPutSpace = %d\n", zukanRegisterParamPutSpace );
        GFL_PRINT( "zukanRegisterParamPos0     = %d\n", zukanRegisterParamPos0 );
        GFL_PRINT( "zukanRegisterParamPos1     = %d\n", zukanRegisterParamPos1 );

        // 進化後のポケモンを図鑑登録
        PokeTool::PokeGetUtil::PutPoke( pHeap, &pokemonParam, zukanRegisterParamPutPlace, PokeTool::PokeGetUtil::PutSpace::FIX, pos0, pos1, &outZukanRegisterEventParam.returnParam );
      }
    }
  }

  outZukanRegisterEventParam.pp = NULL;
}


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)
