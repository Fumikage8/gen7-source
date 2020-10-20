//=============================================================================
/**
 *
 *	@file		battle_ResultData.h
 *	@brief  バトル結果データ定義
 *	@author hosaka genya
 *	@data		2012.10.10
 *
 */
//=============================================================================
#if !defined( __BATTLE_RESULTDATA_H__ )
#define __BATTLE_RESULTDATA_H__

//--------------------------------------------------------------
/**
 *  勝敗コード
 */
//--------------------------------------------------------------
#if USE_PAWN == 0
typedef enum {
#else
  enum { 
#endif // USE_PAWN

  BTL_RESULT_LOSE,        ///< 負けた
  BTL_RESULT_WIN,         ///< 勝った
  BTL_RESULT_DRAW,        ///< ひきわけ
  BTL_RESULT_RUN,         ///< 逃げた
  BTL_RESULT_RUN_ENEMY,   ///< 相手が逃げた
  BTL_RESULT_CAPTURE,     ///< 捕まえた（野生のみ）
  BTL_RESULT_COMM_ERROR,  ///< 通信エラーによる

  BTL_RESULT_MAX, ///<(retult < BTL_RESULT_MAX)で結果コードの正常範囲を取得している場所があるので挿入時は注意
  
  /*
   * BTS3364,GFBTS1030 対策 130330
   * スカイバトル専用特殊結果コード(バトルから返される値ではない)
   *
   * スカイバトルでは勝負に負けても、不参加ポケモンが生き残っている場合があり
   * この場合はゲームオーバーにならない。
   * しかし、勝負には負けているので勝つまで戦える仕組みを実装するためにEventBattleCallで
   * 勝負後の状態を判定し、結果を上書きする場合の特殊コードを作成
   */
  BTL_RESULT_LOSE_NOT_GAMEOVER_FOR_SKY,  ///< スカイバトルでゲームオーバーにならない「負け」の場合の特殊コード

#if USE_PAWN == 0
}BtlResult;
#else
};
#endif // USE_PAWN

#if USE_PAWN == 0
typedef enum {
#else
  enum { 
#endif // USE_PAWN

  BTL_RESULT_WILD_LOSE,        ///< 全滅(負けた/ひきわけ)
  BTL_RESULT_WILD_WIN,         ///< 勝った
  BTL_RESULT_WILD_RUN,         ///< 逃げた
  BTL_RESULT_WILD_RUN_ENEMY,   ///< 相手が逃げた
  BTL_RESULT_WILD_CAPTURE,     ///< 捕まえた
  BTL_RESULT_WILD_MAX,

#if USE_PAWN == 0
}BtlResultWild;
#else
};
#endif // USE_PAWN

#if USE_PAWN == 0
typedef enum {
#else
  enum { 
#endif // USE_PAWN

  BTL_RESULT_SKYBTL_LOSE,       ///< 負けて全滅した
  BTL_RESULT_SKYBTL_LOSE_NOT_GAMEOVER,  ///< 勝負に負けたが全滅はしていない
  BTL_RESULT_SKYBTL_WIN,       ///< 勝った
  BTL_RESULT_SKYBTL_MAX,

#if USE_PAWN == 0
}BtlResultSky;
#else
};
#endif // USE_PAWN

#if USE_PAWN == 0

//@バトル変更箇所
//sangoテレビナビ用データ格納
//※シナリオ上でメガシンカしたか？の評価にも使うので、すべての戦闘で必ず記録される。
typedef struct
{
  u16 frontPoke[2]; ///<先頭ポケモン(厳密にはパーティーの最初と2番目。シングルで1体の時は2体目に０が入ってます)
  u16 lastWaza;     ///<最後に使った技
  u16 megaPoke;     ///<メガシンカしたポケモン(0でしていない)
}BATTLE_TVNAVI_DATA;

/* ------------------------------------------------------------------------- */
/**
 * @brief バトル結果受け渡し用ワーク（ゲームデータに受け渡す)
 */
/* ------------------------------------------------------------------------- */
typedef struct {
  BtlResult result; ///< バトル結果コード
  bool b_zenryoku_waza_used;  //全力技を使ったか？

  u32 turnNum;      ///< 経過ターン
  u8 sakasaPoint;   ///< さかさバトルのポイント
  b8 sakasaSpFlag;  ///< さかさバトル特別な戦い方をしたか
  BATTLE_TVNAVI_DATA tvNaviData;  ///< テレビ用データ

  u16 captureMonsNo;  ///< 捕獲したポケモン番号
} BATTLE_RESULT_DATA;

#endif // USE_PAWN


#endif // __BATTLE_RESULTDATA_H__
