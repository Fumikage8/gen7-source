//============================================================================================
/**
 * @file		BoxPokepara.h
 * @brief		ボックス画面 ポケモンパラメータ関連
 * @author	Hiroyuki Nakamura
 * @date		12.03.30
 */
//============================================================================================
#if !defined( __BOX_POKEPARA_H__ )
#define __BOX_POKEPARA_H__

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"
#include "AppLib/include/Util/app_util_heap.h"

#include "App/Box/include/BoxAppParam.h"

#include "AppLib/include/Util/AppRenderingManager.h"
#include "Savedata/include/BoxPokemonSave.h"
#include "Savedata/include/BoxSave.h"


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
 * @brief	ポケモンパラメータ
 */
//--------------------------------------------------------------------------------------------
class PokePara {
public:
  
  enum{
    POKE_TRAY_MAX      = Savedata::BoxPokemon::TRAY_POKE_MAX,	//!< １トレイのポケモン数
    POKE_PARTY_MAX     = pml::PokeParty::MAX_MEMBERS,					//!< 手持ちポケモン数
    POKE_SELECT_MAX    = POKE_TRAY_MAX + POKE_PARTY_MAX,			//!< ポケモン選択数
  };

  //! 逃がすチェック
  enum {
    FREE_TRUE = 0,          //!< 逃がせる
    FREE_ERR_TAMAGO,        //!< タマゴ
    FREE_ERR_UNION_POKE,    //!< 合体ポケモン
    FREE_ERR_BATTLE_ONCE,   //!< 戦えるポケモンがいなくなる
    FREE_ERR_BB_LOCK,       //!< バトルボックスがロックされている
  };

  static const u32 PUT_POS_TRAY_SPACE = 0xff;   //!< 配置位置がトレイの空き位置

	static const u32 FORM_CHANGE_POS_NONE = 0xffffffff;		//!< フォルムチェンジの必要なし

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		コンストラクタ
	 */
	//--------------------------------------------------------------------------------------------
  PokePara(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		デストラクタ
	 */
	//--------------------------------------------------------------------------------------------
  ~PokePara();

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		イニシャライズ
	 *
	 * @param		party				手持ちポケモン
	 * @param		box_poke		ボックスのポケモン
	 * @param		heap				ヒープ
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
  void Initialize(  Savedata::BoxPokemon * box_poke, gfl2::heap::HeapBase * heap );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		手持ちポケモン数取得
	 *
	 * @param		none
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
 // u32 GetPokePartyCount(void);

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
//  void GetCoreParam( pml::pokepara::CoreParam * cp, u32 tray, u32 pos );

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
 // void PokeDataClear( pml::pokepara::CoreParam * cp, u32 tray, u32 pos );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		トレイのポケモン数取得
	 *
	 * @param		cp		展開用ポケモンパラメータ
	 * @param		tray	トレイ番号
	 *
	 * @return	none
	 *
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 */
	//--------------------------------------------------------------------------------------------
  u32 GetTrayCount( pml::pokepara::CoreParam * cp, u32 tray );

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
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 */
	//--------------------------------------------------------------------------------------------
  bool CheckTraySpace( pml::pokepara::CoreParam * cp, u32 tray, u32 cnt );

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
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 */
	//--------------------------------------------------------------------------------------------
	u32 GetTraySpace( pml::pokepara::CoreParam * cp, u32 tray );

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
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
 // u32 GetAreaPokeCount( pml::pokepara::CoreParam * cp, u32 tray, u32 pos, u32 width, u32 height );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		ポケモンデータ移動
	 *
	 * @param		cp				展開用ポケモンパラメータ
	 * @param		get_tray	取得トレイ
	 * @param		get_pos		取得位置
	 * @param		put_tray	配置トレイ
	 * @param		put_pos		配置位置
	 *
	 * @return	none
	 *
	 * @li	"位置 >= POKE_TRAY_MAX" : 手持ちのポケモン ( "位置 - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
  void MovePokeData( pml::pokepara::CoreParam * cp, u32 get_tray, u32 get_pos, u32 put_tray, u32 put_pos );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		範囲ポケモンデータ移動
	 *
	 * @param		cp					展開用ポケモンパラメータ
	 * @param		get_tray		取得トレイ
	 * @param		get_pos			取得位置
	 * @param		get_width		取得幅
	 * @param		get_height	取得高さ
	 * @param		put_tray		配置トレイ
	 * @param		put_pos			配置位置
	 *
	 * @return	none
	 *
	 * @li	"位置 >= POKE_TRAY_MAX" : 手持ちのポケモン ( "位置 - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
  void MoveMultiPokeData(
        pml::pokepara::CoreParam * cp, u32 get_tray, u32 get_pos, u32 get_width, u32 get_height, u32 put_tray, u32 put_pos );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		ポケモンデータ移動（空き位置に配置）
	 *
	 * @param		cp					展開用ポケモンパラメータ
	 * @param		get_tray		取得トレイ
	 * @param		get_pos			取得位置
	 * @param		get_width		取得幅
	 * @param		get_height	取得高さ
	 * @param		put_tray		配置トレイ
	 * @param		put_pos			配置位置
	 *
	 * @return	none
	 *
	 * @li	"トレイ = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 * @li	"位置 >= POKE_TRAY_MAX" : 手持ちのポケモン ( "位置 - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
  void MovePokeDataSpace(
        pml::pokepara::CoreParam * cp, u32 get_tray, u32 get_pos, u32 get_width, u32 get_height, u32 put_tray, u32 put_pos );

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
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
//	bool UpdateItem( pml::pokepara::CoreParam * cp, u32 tray, u32 pos, u16 item, Savedata::ZukanData * sv_zukan );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		マーキング変更
	 *
	 * @param		cp			展開用ポケモンパラメータ
	 * @param		tray		トレイ
	 * @param		pos			位置
	 * @param		mark		マーク
	 *
	 * @return	none
	 *
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
  void UpdateBoxMark( pml::pokepara::CoreParam * cp, u32 tray, u32 pos, u32 mark );

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
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
//  u32 CheckPokeFree( pml::pokepara::CoreParam * cp, u32 tray, u32 pos );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		覚えている秘伝技を取得
	 *
	 * @param		cp			展開用ポケモンパラメータ
	 * @param		tray		トレイ
	 * @param		pos			位置
	 *
	 * @retval	"0 = 覚えていない"
	 * @retval	"0 != それ以外"
	 *
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
  u32 GetExWaza( pml::pokepara::CoreParam * cp, u32 tray, u32 pos );

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
//  bool CheckBattlePokemon( u32 bit );
  //--------------------------------------------------------------------------------------------
  /**
   * @brief		戦えるポケモンがいるか
   * @param		検査用ポケパーティー
   * @retval	"true = いる"
   * @retval	"false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool CheckBattlePokemon( pml::PokeParty* pPP );
  //--------------------------------------------------------------------------------------------
  /**
   * @brief		戦えるポケモンの数
   * @param		検査用ポケパーティー
   * @retval	数
   */
  //--------------------------------------------------------------------------------------------
  int CountBattlePokemon( pml::PokeParty* pPP );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		スペシャルポケモンか
   * @param		cp		ポケモンパラメータ
   * @retval	"true = スペシャルです"
   * @retval	"false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool CheckSpecialPokemon( pml::pokepara::CoreParam * cp );
	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		戦えるポケモンがいるか（範囲版）
	 *
	 * @param		gp	チェックに含まない範囲開始位置
	 * @param		gw	幅
	 * @param		gh	高さ
	 *
	 * @retval	"true = いる"
	 * @retval	"false = それ以外"
	 */
	//--------------------------------------------------------------------------------------------
  bool CheckBattlePokemon( u32 gp, u32 gw, u32 gh );

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
	 * @li	"gt = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 * @li	"gp >= POKE_TRAY_MAX" : 手持ちのポケモン ( "gp - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
//  bool CheckEgg( pml::pokepara::CoreParam * tcp, u32 gt, u32 gp, u32 gw, u32 gh );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		イベント配布ポケモンかをチェック（交換に出せるか）
	 *
	 * @param		cp		ポケモンパラメータ
	 * @retval	"true = 出せる"
	 * @retval	"false = それ以外"
	 *
	 */
	//--------------------------------------------------------------------------------------------
	bool CheckEventHaifuPokemon( pml::pokepara::CoreParam * cp );

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
  bool CheckTradeCondition( pml::pokepara::CoreParam * cp, TradeData * trade );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		フォルムチェンジするぽけもんかどうか チェンジする場合元に戻す
   *
   * @return	いたらtrue
   *
   */
  //--------------------------------------------------------------------------------------------
  bool CheckAndChangeFormChangePokemon(pml::pokepara::CoreParam* cp,Savedata::ZukanData* pZukanData);
  
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
  bool CheckItem( u32 itemno );
  
private:
	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		各配置場所の幅取得
	 *
	 * @param		tray		トレイ番号
	 * @param		pos			位置
	 *
	 * @return	幅
	 *
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
  u32 GetAreaWidth( u32 tray, u32 pos );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		各配置場所の高さ取得
	 *
	 * @param		tray		トレイ番号
	 * @param		pos			位置
	 *
	 * @return	高さ
	 *
	 * @li	"tray = savedata::BOX::BATTLE_BOX_TRAY_NUMBER" : バトルボックスのポケモン
	 * @li	"pos >= POKE_TRAY_MAX" : 手持ちのポケモン ( "pos - POKE_TRAY_MAX"が位置 )
	 */
	//--------------------------------------------------------------------------------------------
  u32 GetAreaHeight( u32 tray, u32 pos );

	public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief		フォルムチェンジするぽけもんかどうか アイテムを確認して変更
   * @return	none
   */
  //--------------------------------------------------------------------------------------------
  bool CheckAndChangeFormChangePokemonItem(pml::pokepara::CoreParam* cp, int itemno,Savedata::ZukanData* pZukanData);


private:
//  pml::PokeParty * pPokeParty;					//!< 手持ちポケモン
  Savedata::BoxPokemon * pBoxPokemon;		//!< ボックスポケモン
  gfl2::heap::HeapBase * pHeap;					//!< ヒープ
};


}   // box
}   // app

#endif // __BOX_POKEPARA_H__
