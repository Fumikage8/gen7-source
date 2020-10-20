//============================================================================================
/**
 * @file   btl_fieldEff_def.h
 * @brief  btl::FieldStatus クラスが持つ public な enum 定義だが、PAWNから参照するため記述ファイルを独立させている
 * @author taya
 * @date   12.11.18
 */
//============================================================================================
#if !defined __BTL_FIELDEFF_DEF_H__
#define __BTL_FIELDEFF_DEF_H__

  /**
   *  バトル独自のフィールド状態
   */
  enum EffectType {

    EFF_WEATHER,     ///< 天候各種
    EFF_TRICKROOM,   ///< トリックルーム
    EFF_JURYOKU,     ///< じゅうりょく
    EFF_FUIN,        ///< ふういん
    EFF_MIZUASOBI,   ///< みずあそび
    EFF_DOROASOBI,   ///< どろあそび
    EFF_WONDERROOM,  ///< ワンダールーム
    EFF_MAGICROOM,   ///< マジックルーム
    EFF_PLASMASHOWER,///< プラズマシャワー
    EFF_FAIRY_LOCK,  ///< フェアリーロック
    EFF_GROUND,      ///< グラウンド状態

    EFF_MAX,
    EFF_NULL = EFF_MAX,

  };


#endif
