//============================================================================================
/**
 * @file		box_pokepara.cpp
 * @brief		ボックス画面 ポケモンパラメータ関連
 * @author	Hiroyuki Nakamura
 * @author	k.ohno
 * @date		12.03.30
 */
//============================================================================================
#include "BoxPokepara.h"
#include "PokeTool/include/PokeToolPresentPokemon.h"
#include "PokeTool/include/PokeToolForm.h"
#include "App/Box/include/BoxAppParam.h"
#include "Savedata/include/ZukanSave.h"
#include <pml/include/pmlib.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::box
 * @brief			ボックス画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace App {
namespace Box {

//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 */
//--------------------------------------------------------------------------------------------
PokePara::PokePara(void)
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
PokePara::~PokePara()
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		イニシャライズ
 *
 * @param		party				手持ちポケモン
 * @param		box_poke		ボックスのポケモン
 * @param		battle_box	バトルボックスのポケモン
 * @param		heap				ヒープ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PokePara::Initialize( Savedata::BoxPokemon * box_poke, gfl2::heap::HeapBase * heap )
{
//	pPokeParty  = party;
	pBoxPokemon = box_poke;
	pHeap       = heap;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモン数取得
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//u32 PokePara::GetPokePartyCount(void)
//{
//	return pPokeParty->GetMemberCount();
//}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンパラメータ取得
 *
 * @param		cp		ポケモンパラメータ格納場所
 * @param		tray	トレイ番号
 * @param		pos		位置
 *
 * @return	none
 *
 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
 */
//--------------------------------------------------------------------------------------------
/*
void PokePara::GetCoreParam( pml::pokepara::CoreParam * cp, u32 tray, u32 pos )
{
	// 範囲外
	if( pos >= POKE_SELECT_MAX ){
		cp->Clear();
	// 手持ち
	}else if( pos >= POKE_TRAY_MAX ){
		pos -= POKE_TRAY_MAX;
		if( pos >= pPokeParty->GetMemberCount() ){
			cp->Clear();
		}else{
			pml::pokepara::PokemonParam * pp = pPokeParty->GetMemberPointer( pos );
			u8 * tmp = reinterpret_cast<u8 *>( GflHeapAllocMemoryLow(pHeap,pml::pokepara::CoreParam::DATASIZE) );
			pp->Serialize_Core( tmp );
			cp->Deserialize_Core( tmp );
			GflHeapFreeMemory( tmp );
		}
	}
  else{
	// ボックス
		pBoxPokemon->GetPokemon( cp, tray, pos );
	}
}
   */

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンデータ消去
 *
 * @param		cp		展開用ポケモンパラメータ
 * @param		tray	トレイ番号
 * @param		pos		位置
 *
 * @return	none
 *
 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
 */
//--------------------------------------------------------------------------------------------
/*
void PokePara::PokeDataClear( pml::pokepara::CoreParam * cp, u32 tray, u32 pos )
{
	// 手持ち
	if( pos >= POKE_TRAY_MAX ){
		pPokeParty->RemoveMember( pos-POKE_TRAY_MAX );
	}
  else{
	// トレイ
		pBoxPokemon->ClearPokemon( tray, pos, cp );
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイのポケモン数取得
 *
 * @param		cp		展開用ポケモンパラメータ
 * @param		tray	トレイ番号
 *
 * @return	none
 *
 */
//--------------------------------------------------------------------------------------------
u32 PokePara::GetTrayCount( pml::pokepara::CoreParam * cp, u32 tray )
{
	return pBoxPokemon->GetPokemonCount( tray, cp );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイに空きがあるか
 *
 * @param		cp		展開用ポケモンパラメータ
 * @param		tray	トレイ番号
 * @param		cnt		必要な空き数
 *
 * @retval	"true = あり"
 * @retval	"false = なし"
 *
 */
//--------------------------------------------------------------------------------------------
bool PokePara::CheckTraySpace( pml::pokepara::CoreParam * cp, u32 tray, u32 cnt )
{
	// トレイ
	if( pBoxPokemon->GetSpaceCount( tray, cp ) < cnt ){
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイの空き位置を取得
 *
 * @param		cp		展開用ポケモンパラメータ
 * @param		tray	トレイ番号
 *
 * @retval	"POKE_TRAY_MAX = 空きなし"
 * @retval	"POKE_TRAY_MAX != 空き位置"
 *
 */
//--------------------------------------------------------------------------------------------
u32 PokePara::GetTraySpace( pml::pokepara::CoreParam * cp, u32 tray )
{
  for( int i=0; i<POKE_TRAY_MAX; i++ ){
    if( pBoxPokemon->CheckPokemon( tray, i, cp ) == false ){
      return i;
    }
	}
	return POKE_TRAY_MAX;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		範囲内のポケモン数取得
 *
 * @param		cp			展開用ポケモンパラメータ
 * @param		tray		トレイ番号
 * @param		pos			開始位置
 * @param		width		選択幅
 * @param		height	選択高さ
 *
 * @return	ポケモン数
 *
 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
 */
//--------------------------------------------------------------------------------------------
/*
u32 PokePara::GetAreaPokeCount( pml::pokepara::CoreParam * cp, u32 tray, u32 pos, u32 width, u32 height )
{
	u32	cnt = 0;

	for( u32 i=0; i<height; i++ ){
		for( u32 j=0; j<width; j++ ){
			GetCoreParam( cp, tray, pos + i * 6 + j );
			if( cp->IsNull() == false ){
				cnt++;
			}
		}
	}

	return cnt;
}

*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		持ち物変更
 *
 * @param		cp				展開用ポケモンパラメータ
 * @param		tray			トレイ
 * @param		pos				位置
 * @param		item			アイテム番号
 * @param		sv_zukan	図鑑セーブデータ
 *
 * @return	none
 *
 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
 */
//--------------------------------------------------------------------------------------------
/*
bool PokePara::UpdateItem( pml::pokepara::CoreParam * cp, u32 tray, u32 pos, u16 item, Savedata::ZukanData * sv_zukan )
{
	bool	ret = false;

	// 手持ち
	if( pos >= POKE_TRAY_MAX ){
		pml::pokepara::PokemonParam * pp = pPokeParty->GetMemberPointer( pos-POKE_TRAY_MAX );
		pp->SetItem( item );
		FormNo form = pp->GetNextFormNoFromHoldItem( item );
		if( pp->GetFormNo() != form ){
			pp->ChangeFormNo( form );
			sv_zukan->SetPokeSee( *pp );
			ret = true;
		}
	}else{
		GetCoreParam( cp, tray, pos );
		cp->SetItem( item );
		FormNo form = cp->GetNextFormNoFromHoldItem( item );
		if( cp->GetFormNo() != form ){
			cp->ChangeFormNo( form );
			sv_zukan->SetPokeSee( *cp );
			ret = true;
		}
		// トレイ
		{
			pBoxPokemon->UpdatePokemon( cp, tray, pos );
		}
	}

	return ret;
}
*/
//--------------------------------------------------------------------------------------------
/**
 * @brief		逃がせるかをチェック
 *
 * @param		cp			展開用ポケモンパラメータ
 * @param		tray		トレイ
 * @param		pos			位置
 *
 * @retval	"FREE_TRUE = 逃がせる"
 * @retval	"FREE_ERR_TAMAGO = タマゴ"
 * @retval	"FREE_ERR_UNION_POKE = 合体ポケモン"
 * @retval	"FREE_ERR_BATTLE_ONCE = 戦えるポケモンがいなくなる"
 * @retval	"FREE_ERR_BB_LOCK = バトルボックスがロックされている"
 *
 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
 */
//--------------------------------------------------------------------------------------------
/*
u32 PokePara::CheckPokeFree( pml::pokepara::CoreParam * cp, u32 tray, u32 pos )
{
	GetCoreParam( cp, tray, pos );

	// タマゴ
	if( cp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == true ){
		return FREE_ERR_TAMAGO;
	}
	// 合体ポケモン
	if( cp->GetMonsNo() == MONSNO_KYUREMU && cp->GetFormNo() != FORMNO_KYUREMU_A ){
		return FREE_ERR_UNION_POKE;
	}
	// 手持ち
	if( pos >= POKE_TRAY_MAX ){
		if( CheckBattlePokemon( (1<<(pos-POKE_TRAY_MAX)) ) == true ){
			return FREE_TRUE;
		}
		return FREE_ERR_BATTLE_ONCE;
	}

	return FREE_TRUE;
}
*/
//--------------------------------------------------------------------------------------------
/**
 * @brief		戦えるポケモンがいるか
 *
 * @param		bit		チェックに含まない位置のビットテーブル
 *
 * @retval	"true = いる"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
/*
bool PokePara::CheckBattlePokemon( u32 bit )
{
	for( u32 i=0; i<pPokeParty->GetMemberCount(); i++ ){
		if( ( bit & (1<<i) ) == 0 ){
			pml::pokepara::PokemonParam * pp = pPokeParty->GetMemberPointer( i );
			if( pp->GetHp() != 0 && pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false ){
				return true;
			}
		}
	}
	return false;
}

*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		戦えるポケモンがいるか
 * @param		検査用ポケパーティー
 * @retval	"true = いる"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool PokePara::CheckBattlePokemon( pml::PokeParty* pPP )
{
	for( u32 i=0; i<pPP->GetMemberCount(); i++ ){
    pml::pokepara::PokemonParam * pp = pPP->GetMemberPointer( i );
    if( pp->GetHp() != 0 && pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false ){
      return true;
		}
	}
	return false;
}



//--------------------------------------------------------------------------------------------
/**
 * @brief		戦えるポケモンの数
 * @param		検査用ポケパーティー
 * @retval	数
 */
//--------------------------------------------------------------------------------------------
int PokePara::CountBattlePokemon( pml::PokeParty* pPP )
{
  u32 i,j=0;
	for( i=0; i<pPP->GetMemberCount(); i++ ){
    pml::pokepara::PokemonParam * pp = pPP->GetMemberPointer( i );
    if( pp->GetHp() != 0 && pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false ){
      j++;
		}
	}
	return j;
}






//--------------------------------------------------------------------------------------------
/**
 * @brief		タマゴチェック
 *
 * @param		tcp		展開用ポケモンパラメータ
 * @param		gt		トレイ
 * @param		gp		チェック範囲開始位置
 * @param		gw		幅
 * @param		gh		高さ
 *
 * @retval	"true = いる"
 * @retval	"false = それ以外"
 *
 * @li	"gp >= POKE_TRAY_MAX" : 手持ちのポケモン ( "gp - POKE_TRAY_MAX"が位置 )
 */
//--------------------------------------------------------------------------------------------
/*
bool PokePara::CheckEgg( pml::pokepara::CoreParam * tcp, u32 gt, u32 gp, u32 gw, u32 gh )
{

	for( u32 i=0; i<gh; i++ ){
		for( u32 j=0; j<gw; j++ ){
			GetCoreParam( tcp, gt, gp + i * 6 + j );
			if( tcp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == true ){
				return true;
			}
		}
	}
	return false;
}
*/
//--------------------------------------------------------------------------------------------
/**
 * @brief		イベント配布ポケモンかをチェック
 *
 * @param		cp		ポケモンパラメータ
 *
 * @retval	"true = はいふです
 * @retval	"false = それ以外"
 *
 */
//--------------------------------------------------------------------------------------------
bool PokePara::CheckEventHaifuPokemon( pml::pokepara::CoreParam * cp )
{
	if( !PokeTool::PresentPokemon::IsTradeEntry( cp ) ){
		return true;
	}
	return false;
}




//--------------------------------------------------------------------------------------------
/**
 * @brief		スペシャルポケモンか
 *
 * @param		cp		ポケモンパラメータ
 *
 * @retval	"true = スペシャルです"
 * @retval	"false = それ以外"
 *
 */
//--------------------------------------------------------------------------------------------
bool PokePara::CheckSpecialPokemon( pml::pokepara::CoreParam * cp )
{
  MonsNo mons = cp->GetMonsNo();
  FormNo form = cp->GetFormNo();

  // 合体ポケモン
  // キュレム
  if( mons == MONSNO_KYUREMU && form != FORMNO_KYUREMU_A )
  {
    return true;
  }
  // ネクロズマ
  if( mons == MONSNO_PURIZUMU && ( form == FORMNO_PURIZUMU_FORM_S || form == FORMNO_PURIZUMU_FORM_L ) )
  {
    return true;
  }

  return false;
}




//--------------------------------------------------------------------------------------------
/**
 * @brief		フォルムチェンジするぽけもんかどうか チェンジする場合元に戻す
 *
 * @return	none
 *
 */
//--------------------------------------------------------------------------------------------

bool PokePara::CheckAndChangeFormChangePokemon(pml::pokepara::CoreParam* cp,Savedata::ZukanData* pZukanData)
{
  bool forceset = PokeTool::form::UpdateFormForBox(cp);

  if(forceset){
    pZukanData->SetPokeSee( *cp );   // 図鑑みたフラグセット //cross check [8025]
  }
  return forceset;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンと交換条件を比較
 *
 * @param		cp			ポケモンパラメータ
 * @param		trade		交換条件
 *
 * @retval	"true = 出せる"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool PokePara::CheckTradeCondition( pml::pokepara::CoreParam * cp, TradeData * pTrade )
{
	bool fast = cp->StartFastMode();
	bool ret  = true;
	bool egg = false;


  if( cp->IsNull() == true )
  {
		ret = false;
	}
  else
  {
		egg = cp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
	}

	// ポケモン番号
	if( ret == true )
  {
		if( pTrade->MonsNo != MONSNO_NULL )
    {
			if( egg == true )
      {
				ret = false;
			}
      else if( pTrade->MonsNo != cp->GetMonsNo() )
      {
				ret = false;
			}
		}
	}
	// 性別
	if( ret == true ){
		if( pTrade->Gender != pml::SEX_NUM ){
			if( egg == true ){
				ret = false;
			}else if( pTrade->Gender != cp->GetSex() ){
				ret = false;
			}
		}
	}
	// レベル
	if( ret == true ){
    if( egg == true )
    {
      ret = false;
    }
    else if(cp->GetLevel()==0){
      ret = false;
    }
    else if( pTrade->Level != 0 ){
      u8 tLevel = pTrade->Level - 1;
      tLevel = tLevel / 10;
      u8 pokeLevel = cp->GetLevel() - 1 ;
      pokeLevel = pokeLevel / 10;
      if( pokeLevel != tLevel ){
        ret = false;
      }
		}
	}

	cp->EndFastMode( fast );

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		交換してはいけないアイテムか
 *
 * @retval	"true = いけません"
 * @retval	"false = 大丈夫"

・「ポケモン通信交換」「ＧＴＳ」「ミラクル交換」「ゲーム内ポケモン交換」では、																															
 　道具番号６５６～６８５（未定）までを所持している場合（※メガシンカ用のキーストーンを所持）、																															
　下記メッセージを表示します。交換不可　→Ａ－１へ																															
	とくべつな　どうぐを　もっているので																														
	こうかんに　だすことが　できません																														
																															


 */
//--------------------------------------------------------------------------------------------
bool PokePara::CheckItem( u32 itemno )
{
  if((ITEM_BENIIRONOTAMA==itemno)||	(ITEM_AIIRONOTAMA==itemno)){
		return true;
	}

  
  return item::ITEM_CheckMegaStone(itemno);
}



//--------------------------------------------------------------------------------------------
/**
 * @brief		フォルムチェンジするぽけもんかどうか アイテムを確認して変更
 * @return	none
 */
//--------------------------------------------------------------------------------------------

bool PokePara::CheckAndChangeFormChangePokemonItem(pml::pokepara::CoreParam* cp, int itemno,Savedata::ZukanData* pZukanData)
{
  MonsNo mons = cp->GetMonsNo();
	pml::FormNo fno = cp->GetFormNo();
	pml::FormNo itemfno = cp->GetNextFormNoFromHoldItem(itemno);

  if(fno != itemfno){
		cp->ChangeFormNo(itemfno);
    pZukanData->SetPokeSee( *cp );   // 図鑑みたフラグセット //GFNMCat[4178]
    return true;
	}
  return false;
}





}		// box
}		// app
