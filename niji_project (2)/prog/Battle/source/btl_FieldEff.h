#if !defined( __BTL_FIELDEFF_H__ )
#define __BTL_FIELDEFF_H__
//=============================================================================================
/**
 * @file  btl_FieldEff.h
 * @brief ポケモンXY バトル -システム-  フィールドエフェクト種類等
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================

/**
 *  フィールドエフェクト種類
 */
enum BtlFieldEffect{

  BTL_FLDEFF_WEATHER,     ///< 各種天候
  BTL_FLDEFF_TRICKROOM,   ///< トリックルーム
  BTL_FLDEFF_JURYOKU,     ///< じゅうりょく
  BTL_FLDEFF_FUIN,        ///< ふういん
  BTL_FLDEFF_MIZUASOBI,   ///< みずあそび
  BTL_FLDEFF_DOROASOBI,   ///< どろあそび
  BTL_FLDEFF_WONDERROOM,  ///< ワンダールーム
  BTL_FLDEFF_MAGICROOM,   ///< マジックルーム
  BTL_FLDEFF_PLASMASHOWER,

  BTL_FLDEFF_MAX,
  BTL_FLDEFF_NULL = BTL_FLDEFF_MAX,

};


#endif // __BTL_FIELDEFF_H__
