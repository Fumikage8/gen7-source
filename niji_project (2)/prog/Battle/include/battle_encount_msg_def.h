//=========================================================================
/**
 * @file   battle_encount_msg_def.h
 * @brief  バトル演出 ポケモン戦闘エンカウントメッセージパターンIDヘッダ
 *
 * @note   このファイルに定義されたメッセージパターンIDと描画される文字列紐付は手動です
 *         btl_std.mstxtに定義された BTL_STRID_STD_Encount_XXXXとバトル内部ルーチンで紐づけられています
 *
 *         デフォルト値の指定はBattleEffectパターンの1パラメータとして管理され
 *         シンボルエンカウントフラグなどの外部条件により上書きされるケースがあります。
 */
//=========================================================================
#if !defined( __BATTLE_ENCOUNT_MSG_DEF_H__ )
#define __BATTLE_ENCOUNT_MSG_DEF_H__


enum BattleEncountMsgId{
  BATTLE_ENCOUNT_MSG_NONE,           ///<
  BATTLE_ENCOUNT_MSG_WILD,           ///<野生シングル「あっ！野生の○○が飛び出してきた
  BATTLE_ENCOUNT_MSG_WILD_S,         ///<野生飛出し「うわっ！野生の○○が飛びかかってきた
  BATTLE_ENCOUNT_MSG_WILD_MOVE,      ///<移動ポケモン「あっ！野生の○○が現れた
  BATTLE_ENCOUNT_MSG_WILD_TALK,      ///<シンボル「野生の○○が現れた
  BATTLE_ENCOUNT_MSG_WILD_RIDE,      ///<ポケモン(サイホーン/マンムー)に乗った状態「うわっ！踏みつけた○○が飛び出してきた
  BATTLE_ENCOUNT_MSG_WILD_LEGEND,    ///<伝説シンボル「○○が現れた
  BATTLE_ENCOUNT_MSG_WILD_BAND,      ///<群れ「うわっ！○○の群れに遭遇した
  BATTLE_ENCOUNT_MSG_WILD_S_GRASS,   ///<飛出し草「うわっ！野生の○○が草の中から出てきた
  BATTLE_ENCOUNT_MSG_WILD_S_GROUND,  ///<飛出し土「うわっ！野生の○○が土の中から出てきた
  BATTLE_ENCOUNT_MSG_WILD_S_FLY,     ///<飛出し飛来「うわっ！野生の○○が空から飛んできた
  BATTLE_ENCOUNT_MSG_WILD_S_FALT,    ///<飛出し落下「うわっ！野生の○○が天井から落ちてきた
  BATTLE_ENCOUNT_MSG_WILD_S_TRASH,   ///<ゴミ箱「うわっ！野生の○○がゴミ箱から出てきた
  BATTLE_ENCOUNT_MSG_MAX,   ///<MAX値定義
};

#endif //!defined( __BATTLE_ENCOUNT_MSG_DEF_H__ )

