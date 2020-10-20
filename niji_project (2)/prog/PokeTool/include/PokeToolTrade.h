//=============================================================================
/**
 * @file        PokeToolTrade.h
 * @author  tamada
 * @brief   交換関連のポケモンツール
 * @date    2012.12.15
 */
//=============================================================================
#if !defined( __POKE_TOOL_TRADE_H__ )
#define __POKE_TOOL_TRADE_H__

namespace Savedata { class MyStatus; }
namespace Savedata {
  class ZukanData;
  class Record;
}

GFL_NAMESPACE_BEGIN(PokeTool)

//------------------------------------------------------------------------
/**
 * @brief 交換後のポケモンパラメータ更新処理(通信・フィールド兼用）
 * @param[in/out] pp        交換したポケモン
 * @param[in]     MyStatus  新しくそのポケモンを受け取った人の情報
 *
 * @note  親履歴、ポケモンの思い出、なつき度などにかかわる情報を更新する
 *        この関数はNPC交換が使用中。
 *        通信交換はUpdatePokemonAfterCommTradeを使用すること
 */
//------------------------------------------------------------------------
extern void UpdatePokeParamAfterTrade(
    pml::pokepara::CoreParam * pp, const Savedata::MyStatus * myStatus );


//------------------------------------------------------------------------
/**
 * @brief ユンゲラーをフーディンにする 図鑑には２体登録する そうでないポケモンは図鑑登録
 * @param[in/out] pp        交換したポケモン
 * @param[in]     図鑑
 *
 * @note  交換処理と進化処理を一度に処理できない場合に使用する関数
 */
//------------------------------------------------------------------------
extern void HuudhinEvolveZukanSet(
    pml::pokepara::CoreParam * pp, Savedata::ZukanData* pZukan );

//------------------------------------------------------------------------
/**
 * @brief                   ミラクル交換用強制進化
 * @param[in/out] pp        交換したポケモン
 * @param[in/] pmy        自分が渡したポケモン
 * @param[in]     　　ZukanData 図鑑データ
 * @param[in]         Record    レコード
 */
//------------------------------------------------------------------------
extern void MiracleEvolveZukanSet( pml::pokepara::CoreParam * pp,  pml::pokepara::CoreParam * pmy , Savedata::ZukanData* pZukan, Savedata::Record* pRecord );

//------------------------------------------------------------------------
/**
 * @brief フォルムチェック＆リセット（シェイミ・トリミアン用）
 *
 * @param   poke    pml::pokepara::CoreParam
 *
 */
//------------------------------------------------------------------------
extern void CheckTradePokemonForm( pml::pokepara::CoreParam *poke );

//------------------------------------------------------------------------
/**
 * @brief   通信交換後のポケモンパラメータ更新処理
 *
 * @param   poke      pml::pokepara::CoreParam
 * @param   pMyStatus Savedata::MyStatus
 *
 */
//------------------------------------------------------------------------
extern void UpdatePokemonAfterCommTrade( pml::pokepara::CoreParam *poke, 
                                         Savedata::MyStatus *pMyStatus );


GFL_NAMESPACE_END(PokeTool);

#endif  // __POKE_TOOL_TRADE_H__
