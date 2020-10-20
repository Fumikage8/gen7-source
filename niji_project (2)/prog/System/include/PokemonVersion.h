#if !defined( PM_VERSION_H_INCLUDED )
#define PM_VERSION_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#if USE_PAWN == 0
GFL_NAMESPACE_BEGIN(System)
#endif

//=========================================================================
/**
 * @file  PokemonVersion.h
 * @brief ポケモンROMや国コードの定義
 * @date  2005.11.25
 * @author  GAME FREAK inc.
 *
 * AGBポケモンの定義ファイルをそのまま移行した
 * POKEMON_PLの定義ファイルをそのまま移行した
 * swanからkujiraへ引継ぎ  2011.3.6 k.ohno
 */
//=========================================================================


//=========================================================================
//
//
//  ソースのバージョン指定関連ファイル
//
//
//=========================================================================


//=========================================================================
//  定義
//=========================================================================

//　●色違いバージョンによって処理を分岐する場合は
//  　PM_VERSION　によって記述をかえる
//
//  例）
//  #if ( PM_VERSION == VERSION_RUBY )
//    ルビーバージョンの場合の処理を記述
//
//  #elif ( PM_VERSION == VERSION_SAPPHIRE )
//    サファイアバージョンの場合の処理を記述
//
//  #endif
//

//------------------------------------------------------------------
/**
 * @brief カセットバージョン
 *
 * @note  28,29を使用すると、X/Y/AS/ORでカロス地方とみなされる
 */
//------------------------------------------------------------------
#define VERSION_SAPPHIRE        1   ///< AGB サファイア
#define VERSION_RUBY            2   ///< AGB ルビー
#define VERSION_EMERALD         3   ///< AGB エメラルド
#define VERSION_RED             4   ///< AGB ファイアレッド
#define VERSION_GREEN           5   ///< AGB リーフグリーン
#define VERSION_GOLD            7   ///< DS ハートゴールド
#define VERSION_SILVER          8   ///< DS ソウルシルバー
#define VERSION_DIAMOND         10  ///< DS ダイヤモンド
#define VERSION_PEARL           11  ///< DS パール
#define VERSION_PLATINUM        12  ///< DS プラチナ
#define VERSION_COLOSSEUM       15  ///< GC コロシアム
#define VERSION_WHITE           20  ///< DS ホワイト
#define VERSION_BLACK           21  ///< DS ブラック
#define VERSION_W2              22  ///< DS ホワイト２
#define VERSION_B2              23  ///< DS ブラック２
#define VERSION_X               24  ///< 3DS X
#define VERSION_Y               25  ///< 3DS Y
#define VERSION_CTR_SAPPHIRE    26  ///< 3DS アルファサファイア
#define VERSION_CTR_RUBY        27  ///< 3DS オメガルビー
#define VERSION_CALOS_RESERVE3  28  ///< ※カロス地方予約領域
#define VERSION_CALOS_RESERVE4  29  ///< ※カロス地方予約領域
#define VERSION_SUN             30  ///< 3DS SUN
#define VERSION_MOON            31  ///< 3DS MOON
#define VERSION_SUN2            32  ///< 3DS momiji SUN
#define VERSION_MOON2           33  ///< 3DS momiji MOON
#define VERSION_HOLOHOLO        34  ///< Pokemon HOLOHOLO
#define VERSION_VC_RED          35  ///< VC 赤
#define VERSION_VC_GREEN        36  ///< VC 緑
#define VERSION_VC_BLUE         37  ///< VC 青
#define VERSION_VC_PIKACHU      38  ///< VC ぴかちゅう
#define VERSION_VC_GOLD         39  ///< VC 金
#define VERSION_VC_SILVER       40  ///< VC 銀
#define VERSION_VC_CRISTAL      41  ///< VC クリスタル



///< カロス地方範囲
#define VERSION_CALOS_MIN  (VERSION_X)
#define VERSION_CALOS_MAX  (VERSION_CALOS_RESERVE4)




//------------------------------------------------------------------
//  言語コード指定
//------------------------------------------------------------------
#define POKEMON_LANG_NULL          0  ///<  言語コード：無効な値
#define POKEMON_LANG_JAPAN         1  ///<  言語コード：日本        JPN
#define POKEMON_LANG_ENGLISH       2  ///<  言語コード：英語        USA
#define POKEMON_LANG_FRANCE        3  ///<  言語コード：フランス語  FRA
#define POKEMON_LANG_ITALY         4  ///<  言語コード：イタリア語  ITA
#define POKEMON_LANG_GERMANY       5  ///<  言語コード：ドイツ語    DEU
#define POKEMON_LANG_SPAIN         7  ///<  言語コード：スペイン語  ESP
#define POKEMON_LANG_KOREA         8  ///<  言語コード：韓国語      KOR
#define POKEMON_LANG_CHINA         9  ///<  言語コード：簡体字（中国）      CHN
#define POKEMON_LANG_TAIWAN        10 ///<  言語コード：繁体字（台湾、香港）    TWN



//------------------------------------------------------------------
//  性別コード指定
//------------------------------------------------------------------
#define   PM_MALE     0   ///< 性別コード：男性
#define   PM_FEMALE   1   ///< 性別コード：女性
#define   PM_NEUTRAL    2   ///< 性別コード：中性

//=========================================================================
//  変数
//=========================================================================


#if USE_PAWN == 0

//=========================================================================
//  関数プロトタイプ
//=========================================================================

//------------------------------------------------------------------
/**
* @brief 指定のROMバージョンがアッパーバージョンか判定
*
* @param version     ROMバージョン
*
* @return アッパーバージョンROMならtrueを返却
*/
//------------------------------------------------------------------
// @note niji未対応 ( 2016/01/22の時点で使用してないのでコメントアウト )
//extern bool IsUpperVersion( u8 version );


extern u8 GetVersion( void );

#if !defined(PM_VERSION)
error PM_VERSION_UndefinedError //PM_VERSIONプリプロセッサの未定義を検出するために、わざとビルドエラーになる行を仕込んでいます
#endif

#if  PM_DEBUG
extern void SetVersion( u8 version );
#define GET_VERSION()  (System::GetVersion())
#else
#define GET_VERSION()  PM_VERSION
#endif

//------------------------------------------------------------------
/**
 * @brief 言語IDを取得する
 * @return 言語ID
 *
 * xy::Languageクラスが保持する言語IDをグローバル参照するための関数
 */
//------------------------------------------------------------------
extern u8 GetLang( void );

#endif


#if USE_PAWN == 0
GFL_NAMESPACE_END(System)
#endif

#endif  // PM_VERSION_H_INCLUDED
