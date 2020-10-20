//========================================================================================
/**
 * @brief 技による状態異常コード  - Pawn等のスクリプトから参照できるように単体ヘッダとしている -
 * @file wazasick_def.h
 * @author taya
 * @date 2011.11.01
 */
//========================================================================================
#if !defined __WAZASICKDEF_H__
#define __WAZASICKDEF_H__
#pragma once


#if !defined PAWN_COMPILER
namespace pml{
  /**
   * @namespace pml::wazadata
   */
  namespace wazadata {
#endif

    //------------------------------------------------------------------------------------
    /**
     * @brief ワザによって引き起こされる状態異常。
     * @note PokeSickをバトル用に拡張したもの。
     */
    //------------------------------------------------------------------------------------
    enum WazaSick {
      WAZASICK_NULL = 0,

      // 以下、マヒ〜どくは PokemonParam からも参照されている。値および並び順は変更しないこと！
      WAZASICK_MAHI,
      WAZASICK_NEMURI,
      WAZASICK_KOORI,
      WAZASICK_YAKEDO,
      WAZASICK_DOKU,

      WAZASICK_KONRAN,
      WAZASICK_MEROMERO,

      WAZASICK_BIND,
      WAZASICK_AKUMU,
      WAZASICK_NOROI,
      WAZASICK_TYOUHATSU,
      WAZASICK_ICHAMON,
      WAZASICK_KANASIBARI,
      WAZASICK_AKUBI,
      WAZASICK_KAIHUKUHUUJI,
      WAZASICK_IEKI,            ///< とくせい無効化
      WAZASICK_MIYABURU,        ///< みやぶられて相性ゼロでも等倍で殴られる＆回避率上昇が無効
      WAZASICK_YADORIGI,        ///< やどりぎのたね
      WAZASICK_SASIOSAE,        ///< さしおさえ
      WAZASICK_HOROBINOUTA,     ///< ほろびのうた
      WAZASICK_NEWOHARU,        ///< ねをはる
      WAZASICK_TOOSENBOU,       ///< にげる・いれかえが出来ない
      WAZASICK_ENCORE,          ///< 直前に使ったワザしか出せない
      WAZASICK_ZIGOKUDUKI,      ///< じごくづき( 音技を出せない )
      WAZASICK_HANEYASUME,      ///< はねやすめ（飛行タイプ->無属性化）

      WAZASICK_WAZALOCK,        ///< 直前に使ったワザしか出せない上、行動も選択できない
      WAZASICK_TAMELOCK,        ///< 直前に使ったワザしか出せない上、行動も選択できない（溜めワザ専用）
      WAZASICK_KODAWARI,        ///< 最初に使ったワザしか選択できない
      WAZASICK_MUSTHIT,         ///< ワザ必中状態
      WAZASICK_MUSTHIT_TARGET,  ///< 特定相手に対してのみ必中状態
      WAZASICK_FLYING,          ///< でんじふゆう
      WAZASICK_FLYING_CANCEL,   ///< ふゆう状態になれない
      WAZASICK_TELEKINESIS,     ///< テレキネシス
      WAZASICK_FREEFALL,        ///< フリーフォール
      WAZASICK_HITRATIO_UP,     ///< 命中率上昇
      WAZASICK_AQUARING,        ///< アクアリング
      WAZASICK_FORCE_WAZATYPE,  ///< 攻撃ワザタイプ強制書き換え（とりあえず“そうでん”に使用）
      WAZASICK_FUNJIN,          ///< ふんじん
      WAZASICK_SAWAGU,          ///< さわぐ
      WAZASICK_ABARERU,         ///< あばれる・はなびらのまい・げきりん
      WAZASICK_TOGISUMASU,      ///< とぎすます

      WAZASICK_MAX,
      WAZASICK_STD_MAX = WAZASICK_MEROMERO+1, ///< 標準状態異常の数（メロメロまで）

      WAZASICK_SPECIAL_CODE = 0xffff, ///< 特殊処理コード
    };

#if !defined PAWN_COMPILER
  }
}
#endif


#endif  // __WAZASICKDEF_H__
