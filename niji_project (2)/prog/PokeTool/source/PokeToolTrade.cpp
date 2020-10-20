//=============================================================================
/**
 * @file  poke_tool_Trade.cpp
 * @author  tamada
 * @brief   交換関連のポケモンツール
 * @date    2012.12.15
 */
//=============================================================================
#include "PokeTool/include/NakayoshiManager.h"
#include "Savedata/include/ZukanSave.h"
#include "Savedata/include/Record.h"
#include "PokeTool/include/PokeToolForm.h"
#include "PokeTool/include/PokeToolTrade.h"
#include "PokeTool/include/PokeMemo.h"


GFL_NAMESPACE_BEGIN(PokeTool)

//------------------------------------------------------------------------
/**
 * @brief 交換後のポケモンパラメータ更新処理(通信・フィールド兼用）
 * @param[in/out] pp        交換したポケモン
 * @param[in]     MyStatus  新しくそのポケモンを受け取った人の情報
 *
 * @note  親履歴、ポケモンの思い出、なつき度、仲良し度などにかかわる情報を更新する
 *        この関数はNPC交換が使用中。
 *        通信交換は↓のUpdatePokemonAfterCommTradeを使用すること
 */
//------------------------------------------------------------------------
void UpdatePokeParamAfterTrade(
    pml::pokepara::CoreParam * pp, const Savedata::MyStatus * myStatus )
{
  //タマゴの場合は何も書き換えない
  if ( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) return;

  //親情報を更新する
  pml::pokepara::OwnerInfo ownerInfo;
  myStatus->CreateOwnerInfo( ownerInfo );
  pp->UpdateOwnerInfo( &ownerInfo );
}


//------------------------------------------------------------------------
/**
 * @brief ユンゲラーをフーディンにする 図鑑には２体登録する そうでないポケモンはなにもしない
 * @param[in/out] pp        交換したポケモン
 * @param[in]     
 *
 * @note  この関数を呼んでいるものはいないので、Savedata::ZukanDataのIncrementPokeSeeAmountを追加しませんでした。
 */
//------------------------------------------------------------------------
void HuudhinEvolveZukanSet(
    pml::pokepara::CoreParam * pp, Savedata::ZukanData* pZukan )
{

  if(pp->GetMonsNo() == MONSNO_YUNGERAA){
    // 図鑑登録
    pZukan->SetPokeSee( *pp );
    pZukan->SetPokeGet( *pp );
    //    進化
    {
      MonsNo mons;
      u32 rootnum;
      pp->CanEvolveByTrade(NULL, &mons, &rootnum);
      pp->Evolve(mons, rootnum);
    }
    // 図鑑登録
    pZukan->SetPokeSee( *pp );
    pZukan->SetPokeGet( *pp );
  }
}




//------------------------------------------------------------------------
/**
 * @brief                   ミラクル交換用強制進化
 * @param[in/out] pp        交換したポケモン
 * @param[in/] pmy        自分が渡したポケモン
 * @param[in]     　　ZukanData 図鑑データ
 * @param[in]         Record    レコード

 * @note  この関数を呼んでいるのは
 *            prog/src/netapp/gts/gts_enter.cppのEnterMainSystem::StoreYungeraaRegistHuudhin 
 *            prog/src/netapp/randomtrade/RandomTrade.cppのProc::Save
 *        です。
 */
//------------------------------------------------------------------------
void MiracleEvolveZukanSet(
    pml::pokepara::CoreParam * pp,  pml::pokepara::CoreParam * pmy , Savedata::ZukanData* pZukan, Savedata::Record* pRecord )
{
  // 図鑑登録
  pZukan->SetPokeSee( *pp );
  pZukan->SetPokeGet( *pp );
  pZukan->IncrementPokeSeeAmount(pp->GetMonsNo());

  {
    MonsNo  next_monsno;
    u32     evo_root;
    if(pp->CanEvolveByTrade( pmy, &next_monsno, &evo_root ) == true) {
      pp->Evolve(next_monsno, evo_root);

      // 進化したときだけ行う出会った回数インクリメント
      pZukan->IncrementPokeSeeAmount(pp->GetMonsNo());
      //レコード+
      Savedata::IncRecord( Savedata::Record::RECID_POKE_EVOLUTION );   // 進化させた数
      Savedata::IncRecord( Savedata::Record::RECID_DAYCNT_EVOLUTION ); // 一日に進化させた数
    }
    // 図鑑登録
    pZukan->SetPokeSee( *pp );
    pZukan->SetPokeGet( *pp );
  }
}


//------------------------------------------------------------------------
/**
 * @brief シェイミ・トリミアン・フーパは交換成立時にフォルムを０にリセットする
 *
 * @param   poke    pml::pokepara::PokemonParam
 */
//------------------------------------------------------------------------
void CheckTradePokemonForm( pml::pokepara::CoreParam *poke )
{
  // シェイミチェック
  if(poke->GetMonsNo()==MONSNO_SHEIMI){
    if(poke->GetFormNo()!=FORMNO_SHEIMI_LAND){
      poke->ChangeFormNo(FORMNO_SHEIMI_LAND); // ランドフォルムに戻す
    }
  }

  // トリミアンチェック
  if(poke->GetMonsNo()==MONSNO_TORIMIAN){
    if(poke->GetFormNo()!=FORMNO_TORIMIAN_NORMAL){
      PokeTool::form::SetPoodleForm( poke,FORMNO_TORIMIAN_NORMAL); // ノーマルフォルムに戻す
    }
  }

  // フーパチェック
  if(poke->GetMonsNo()==MONSNO_HUUPA ){
    if(poke->GetFormNo()!=FORMNO_HUUPA_NORMAL){
      PokeTool::form::SetHuupaForm( poke,FORMNO_HUUPA_NORMAL); // ノーマルフォルムに戻す
    }
  }
}

//------------------------------------------------------------------------
/**
 * @brief   通信交換後のポケモンパラメータ更新処理
 *
 * @param   poke       pml::pokepara::CoreParam
 * @param   pMyStatus  Savedata::MyStatus
 *
 */
//------------------------------------------------------------------------
void UpdatePokemonAfterCommTrade( pml::pokepara::CoreParam *poke, Savedata::MyStatus *pMyStatus )
{
  // 親情報の更新
  PokeTool::UpdatePokeParamAfterTrade( poke, pMyStatus );

  // フォルムチェック＆リセット
  PokeTool::CheckTradePokemonForm( poke );

  // 回復
  poke->RecoverAll();

  GFL_PRINT("ポケメモ・思い出登録・フォルムリセット・親更新・トレーニング・回復\n");

}




GFL_NAMESPACE_END(PokeTool)
