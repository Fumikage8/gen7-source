#if !defined( __BTL_SICKCONT_H__ )
#define __BTL_SICKCONT_H__
//=============================================================================================
/**
 * @file  btl_sickcont.h
 * @brief ポケモンXY バトルシステム 状態異常などの継続パラメータ
 * @author  taya
 *
 * @date  2010.12.18  作成
 */
//=============================================================================================

#pragma once

GFL_NAMESPACE_BEGIN(btl)

//===================================================================
// 状態異常等の継続パラメータ
//===================================================================

//--------------------------------------------------------------
/**
 *  状態異常継続パラメータ
 */
//--------------------------------------------------------------
typedef struct  {

  union {
    u64    raw;
    struct {
      u64  type        : 3;
      u64  causePokeID : 5;  ///< 状態異常の原因となったポケモンのID
      u64  _0          : 56;
    };
    struct {
      u64  type_turn   : 3;   ///< ターン数型
      u64  causePokeID : 5;   ///< 状態異常の原因となったポケモンのID
      u64  count       : 6;   ///< 有効ターン数
      u64  param       : 16;  ///< 汎用パラメータ16bit
      u64  flag        : 1;   ///< 汎用フラグ 1bit
      u64  _1          : 33;
    }turn;
    struct {
      u64  type_poke   : 3;   ///< ポケ依存型
      u64  causePokeID : 5;   ///< 状態異常の原因となったポケモンのID
      u64  ID          : 6;   ///< 対象ポケID
      u64  param       : 16;  ///< 汎用パラメータ16bit
      u64  flag        : 1;   ///< 汎用フラグ 1bit
      u64  _2          : 33;
    }poke;
    struct {
      u64  type_perm   : 3;   ///< 永続型
      u64  causePokeID : 5;   ///< 状態異常の原因となったポケモンのID
      u64  count_max   : 6;   ///< ターンカウントアップ最大数
      u64  param       : 16;  ///< 汎用パラメータ16bit
      u64  flag        : 1;   ///< 汎用フラグ 1bit
      u64  _3          : 33;
    }permanent;
    struct {
      u64  type_poketurn : 3;  ///< ターン数・ポケ依存混合型
      u64  causePokeID   : 5;  ///< 状態異常の原因となったポケモンのID
      u64  count         : 6;  ///< 有効ターン数
      u64  pokeID        : 6;  ///< 対象ポケID
      u64  param         : 16; ///< 汎用パラメータ16bit
      u64  flag          : 1;  ///< 汎用フラグ 1bit
      u64  _4            : 27;
    }poketurn;
  };

}BTL_SICKCONT;

/**
 *  NULL型 作成
 */
extern BTL_SICKCONT  SICKCONT_MakeNull( void );

/**
 *  NULL型かどうか判別
 */
extern bool  SICKCONT_IsNull( const BTL_SICKCONT& cont );

/*
 *  ターン数型 作成
 *  ※ターン経過による効果切れを実現するためには、tables::GetTurnCheckWazaSickByOrder も処理追加の必要がある
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param turn         継続ターン数
 */
extern BTL_SICKCONT  SICKCONT_MakeTurn( u8 causePokeID, u8 turns );

/*
 *  ターン数型（パラメータ付き）作成
 *  ※ターン経過による効果切れを実現するためには、tables::GetTurnCheckWazaSickByOrder も処理追加の必要がある
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param turn         継続ターン数
 * @param param        パラメータ
 */
extern BTL_SICKCONT  SICKCONT_MakeTurnParam( u8 causePokeID, u8 turns, u16 param );

/*
 *  ポケ依存型作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param pokeID       依存対象ポケモンのID
 */
extern BTL_SICKCONT  SICKCONT_MakePoke( u8 causePokeID, u8 pokeID );

/*
 *  永続型作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 */
extern BTL_SICKCONT  SICKCONT_MakePermanent( u8 causePokeID );

/*
 *  永続型（ターン数カウントアップ）作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param count_max    ターンカウントの最大値
 */
extern BTL_SICKCONT  SICKCONT_MakePermanentInc( u8 causePokeID, u8 count_max );

/*
 *  永続型（パラメータ付き）作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param param        パラメータ
 */
extern BTL_SICKCONT  SICKCONT_MakePermanentParam( u8 causePokeID, u16 param );

/*
 *  永続型（ターン数カウントアップ＆パラメータ付き）作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param count_max    ターンカウントの最大値
 * @param param        パラメータ
 */
extern BTL_SICKCONT  SICKCONT_MakePermanentIncParam( u8 causePokeID, u8 count_max, u16 param );

/*
 *  ターン数・ポケ依存混合型作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param pokeID       依存対象ポケモンのID
 * @param turn         継続ターン数
 */
extern BTL_SICKCONT  SICKCONT_MakePokeTurn( u8 causePokeID, u8 pokeID, u8 turns );

/*
 *  ターン数・ポケ依存混合型（パラメータ付き）作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 * @param pokeID       依存対象ポケモンのID
 * @param turn         継続ターン数
 * @param param        パラメータ
 */
extern BTL_SICKCONT  SICKCONT_MakePokeTurnParam( u8 causePokeID, u8 pokeID, u8 turns, u16 param );

/**
 * もうどく用型作成
 * @param causePokeID  状態異常の原因となったポケモンのID
 */
extern BTL_SICKCONT  SICKCONT_MakeMoudokuCont( u8 causePokeID );

/**
 * もうどく用型かどうか判別
 */
extern bool  SICKCONT_IsMoudokuCont( const BTL_SICKCONT& cont );

/*
 *  依存ポケIDを取得（ポケ依存でなければ BTL_POKEID_NULL）
 */
extern u8  SICKCONT_GetPokeID( const BTL_SICKCONT& cont );

/*
 *  依存ポケIDを書き換え（ポケ依存でなければ何もしないL）
 */
extern void  SICKCONT_SetPokeID( BTL_SICKCONT* cont, u8 pokeID );


/*
 *  有効ターン数を取得（ターン数制限がなければ 0）
 */
extern u8  SICCONT_GetTurnMax( const BTL_SICKCONT& cont );

/*
 *  汎用パラメータを設定
 */
extern void  SICKCONT_AddParam( BTL_SICKCONT* cont, u16 param );

/*
 *  設定した汎用パラメータを取得
 */
extern u16  SICKCONT_GetParam( const BTL_SICKCONT& cont );

/*
 *  汎用フラグセット
 */
extern void  SICKCONT_SetFlag( BTL_SICKCONT* cont, bool flag );

/*
 *  汎用フラグ取得
 */
extern bool  SICKCONT_GetFlag( const BTL_SICKCONT& cont );

/*
 *  継続ターン数増加
 */
extern void  SICKCONT_IncTurn( BTL_SICKCONT* cont, u8 inc );

/*
 *  継続ターン数セット
 */
extern void  SICKCONT_SetTurn( BTL_SICKCONT* cont, u8 turn );

/**
 * @brief 状態異常の原因となったポケモンのIDを取得する
 * @retval [0, 31]          原因となったポケモンが存在する場合
 * @retval BTL_POKEID_NULL  原因となったポケモンが存在しない場合
 */
extern u8 SICKCONT_GetCausePokeID( const BTL_SICKCONT& cont );

/**
 * @brief 状態異常の原因となったポケモンのIDを設定する
 * @param[in]  pokeID   原因ポケモンID
 */
extern void SICKCONT_SetCausePokeID( BTL_SICKCONT* cont, u8 pokeID );


/**
 * @brief 32ビット値に分割する
 * @param[in]  cont   分割対象の BTL_SICKCONT
 * @param[out] high   上位32ビットの格納先
 * @param[out] low    下位32ビットの格納先
 */
extern void SICKCONT_Split32bit( const BTL_SICKCONT& cont, u32* high, u32* low );

/**
 * @brief 32ビット値を結合する
 * @param[out] cont   結合した値の格納先
 * @param[in]  high   上位32ビット
 * @param[in]  low    下位32ビット
 */
extern void SICKCONT_Unite32bit( BTL_SICKCONT* cont, u32 high, u32 low );


GFL_NAMESPACE_END(btl)


#endif // __BTL_SICKCONT_H__
