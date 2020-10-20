//==============================================================================
/**
 * @file	JoinFestaAttractionDefine.h
 * @brief	ジョインフェスタアトラクション定義
 * @author	ichiraku_katsuhiko
 * @date	2015/11/17　12:47
 */
// =============================================================================
#ifndef __JOIN_FESTA_ATTRACTION_DEFINE_H__
#define __JOIN_FESTA_ATTRACTION_DEFINE_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <str/include/gfl2_StrSys.h>
#include "System/include/GameStrLen.h"

// JoinFestaFriendKey
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"
// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaAttractionDefine
{
public:

  // 最大値
  enum{
    PRIVATE_SCORE_MAX = 999,    //! 個人スコア最大値
    TOTAL_SCORE_MAX   = 99999,  //! トータルスコア最大値
    SCORE_NUM_MAX     = 100,    //! スコア収集最大人数
    KENTAROSU_ROCK_DISP_MAX = 12, //! ケンタロスライドアトラクション時に壊せる岩出現数
    KENTAROSU_ROCK_NUM_MAX = 16, //! ケンタロスライドアトラクション用に用意している壊せる岩最大数
    ATTRACTION_ENTRY_TIME_LIMIT = 60, //! アトラクション参加可能時間
  };

  /*
   *  @enum 動作モデル用出現状態
   *
   */
  enum AppearanceState{
    APPEARANCE_STATE_NOT,   //!< 未出現
    APPEARANCE_STATE_OK,    //!< 出現済み
    APPEARANCE_STATE_TALK,  //!< 出現済み+話しかけた
  };

  /**
   * @enum AppearanceLimit
   *  出現制限
   */
  enum AppearanceLimit{
    APPEARANCE_LIMIT_ONLY,  //!< 同じグループID同士の場合には１つだけ、参加カウンターが低い方を優先して出す。
    APPEARANCE_LIMIT_RND,   //!< 同じグループID同士の場合にはランダムで１つ出す。
    APPEARANCE_LIMIT_NONE,  //!< 特に制限なく出す。
  };

  /**
   * @struct CoreData
   *         アトラクション1つのデータ構造
   */
  struct CoreData{
    AppearanceLimit   appearanceLimit;      //!< 出現制限
    u8                group;                //!< グループID
    u16               attractionName;       //!< アトラクション名
    u16               attractionExplain;    //!< アトラクション説明
    u32               clearCondition;       //!< クリア条件
    u8                baseFasCoin;          //!< 基礎報酬（フェスコイン）
    u16               manEvaluationStar1;   //!< 個人評価★1(以上を満たせば点灯）
    u16               manEvaluationStar2;   //!< 個人評価★2(以上を満たせば点灯）
    u16               manEvaluationStar3;   //!< 個人評価★3(以上を満たせば点灯）
    u16               manEvaluationStar4;   //!< 個人評価★4(以上を満たせば点灯）
    u16               manEvaluationStar5;   //!< 個人評価★5(以上を満たせば点灯）
    u32               totalEvaluationStar1; //!< 全体評価★1(以上を満たせば点灯）
    u32               totalEvaluationStar2; //!< 全体評価★2(以上を満たせば点灯）
    u32               totalEvaluationStar3; //!< 全体評価★3(以上を満たせば点灯）
    u32               totalEvaluationStar4; //!< 全体評価★4(以上を満たせば点灯）
    u32               totalEvaluationStar5; //!< 全体評価★5(以上を満たせば点灯）
    u16               timeLimit;            //!< 制限時間（秒）
    b8                isAfterClear;         //!< クリア後解禁かどうか
    u8                openPlayCount;        //!< アトラクション参加回数（レコード）の開放値
  };

  /**
   * @struct SaveData
   *         アトラクション1つにつきセーブするデータ
   */
  struct SaveData{
    u16               playCount;            //!< プレイ回数 
    u16               manHighScore;         //!< 最高個人スコア
    u32               totalHighScore;       //!< 最高トータルスコア
  };


  /**
   * @struct ScoreInfo
   *         アトラクション中の個人スコア情報
   */
  struct ScoreInfo{
    JoinFestaFriendKey  friendKey;
    u16                 score;
    gfl2::str::STRCODE  name[ ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ];
  };

  /**
   * @struct ResultData
   *         アトラクション結果データ
   */
  struct ResultData{
    JoinFestaScript::AttractionId   attractionId;             //! アトラクションID
    u32                             clearCondition;           //!< クリア条件
    u16                             timeLimit;                //!< 制限時間（秒）
    u16                             getFesCoin;               //!< もらえるフェスコイン
    u16                             privateScore;             //!< 個人スコア
    b8                              isPrivateScoreNewRecord;  //!< 個人スコア記録更新したか(trueでした）
    u8                              manEvaluationStarNum;     //!< 個人評価の★の数（0～5）
    u32                             totalScore;               //!< トータルスコア
    b8                              isTotalScoreNewRecord;    //!< トータルスコア記録更新したか(trueでした）
    u8                              totalEvaluationStarNum;   //!< トータル評価の★の数（0～5）
    ScoreInfo                       scoreInfoList[ SCORE_NUM_MAX ]; //!< 収集したスコア情報リスト
  };


};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

#endif // __JOIN_FESTA_ATTRACTION_DEFINE_H__
