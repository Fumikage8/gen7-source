//==============================================================================
/**
 *  @file    btl_pokeparam_contflag.h
 *  @brief   ポケモンXY バトルシステム  参加ポケモン戦闘用データ
 *  @author  taya
 *
 *  BTL_POKEPARAM の public な enum 定義を PAWN から読めるようにするため
 *  独立したヘッダに記述しています。
 *  このファイルのみ単体で include しないようにしてください。
 *                                                                       taya
 */
//==============================================================================
#if !defined  __BTL_POKEPARAM_CONTFLAG_H__
#define __BTL_POKEPARAM_CONTFLAG_H__

  /**
   * @brief 場にいるあいだ継続するフラグ
   *
   * @note  瀕死によりクリアされる
   * @note  ポケモン入れ替えによりクリアされる
   * @note  場に出る際にクリアされる
   */
  enum ContFlag
  {
    CONTFLG_ACTION_DONE,      ///< 行動した
    CONTFLG_CANT_CHANGE,      ///< にげる・入れ替えが出来なくなる
    CONTFLG_TAME,
    CONTFLG_SORAWOTOBU,
    CONTFLG_DIVING,
    CONTFLG_ANAWOHORU,
    CONTFLG_SHADOWDIVE,
    CONTFLG_MARUKUNARU,
    CONTFLG_TIISAKUNARU,
    CONTFLG_KIAIDAME,
    CONTFLG_POWERTRICK,
    CONTFLG_MIKURUNOMI,
    CONTFLG_CANT_ACTION,      ///< 反動で動けない
    CONTFLG_MORAIBI,
    CONTFLG_BATONTOUCH,
    CONTFLG_ITEM_LOSE,
    CONTFLG_ELECFIELD_GUARD,
    CONTFLG_MISTFIELD_GUARD,
    CONTFLG_ONNEN,            ///< 技「おんねん」発動中
    CONTFLG_MITIDURE,         ///< 技「みちづれ」発動中
    CONTFLG_JUDEN,            ///< 技「じゅうでん」発動中
    CONTFLG_GAMAN,            ///< 技「がまん」発動中
    CONTFLG_BODYPURGE,        ///< 技「ボディパージ」により体重が軽くなっている
    CONTFLG_MOETUKIRU,        ///< 技「もえつきる」を使用した

    CONTFLG_MAX,
    CONTFLG_NULL = CONTFLG_MAX,
  };


#endif  // #if !defined  __BTL_POKEPARAM_CONTFLAG_H__
