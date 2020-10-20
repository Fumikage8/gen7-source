//======================================================================
/**
 * @file MantainSurfAppData.h
 * @date 2016/11/14 17:48:31
 * @author fang_yicheng
 * @brief マンタインサーフアプリケーションデータを管理するクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MANTAIN_SURF_APP_DATA_H_INCLUDED__
#define __MANTAIN_SURF_APP_DATA_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_List.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

//-----------------------------------------------------------------------------
/**
 * @class MantainSurfAppData
 * @brief マンタインサーフアプリケーションデータを管理するクラス
 */
//-----------------------------------------------------------------------------
class MantainSurfAppData
{
  GFL_FORBID_COPY_AND_ASSIGN(MantainSurfAppData);

public:

  /* 定数 */
  enum
  {
    SKILL_SCORE_RANK_MAX = 3, // 技得点ランク最大値
  };
  static const f32 SKILL_SCORE_RATE[];      // 各ランクの技得点率

  // 技ID
  enum SkillId
  {
    SKILL_ID_NONE = -1,    // なし
    SKILL_ID_EX_01,        // スクリュードライバー（右）
    SKILL_ID_EX_02,        // スクリュードライバー（左）
    SKILL_ID_EX_03,        // スカイハイダイブ
    SKILL_ID_EX_04,        // リバースアンドリバース
    SKILL_ID_EX_05,        // オーバーザレインボー
    SKILL_ID_EX_06,        // スプラッシュトルネード
    SKILL_ID_EX_07,        // がむしゃら
    SKILL_ID_NORMAL_01,    // 上方向
    SKILL_ID_NORMAL_02,    // 下方向
    SKILL_ID_NORMAL_03,    // 左方向
    SKILL_ID_NORMAL_04,    // 右方向
    SKILL_ID_MAX,
    SKILL_ID_BONUS = SKILL_ID_MAX, // 着水ボーナス
  };

  /**  チュートリアルID */
  enum TutorialID
  {
    TUTORIAL_Info01,   /**<  マンタインサーフは　もくてきのしまに…  */
    TUTORIAL_Info02,   /**<  マンタインサーフは　オートで進みます…  */
      /*  （速度アップ操作練習）  */
    TUTORIAL_Info03,   /**<  レッスン1　終了！…                     */
    TUTORIAL_Info04,   /**<  レッスン2　障害物…                     */
      /*  （障害物操作練習）  */
    TUTORIAL_Info05,   /**<  レッスン2　クリア！…                   */
    TUTORIAL_Info06,   /**<  レッスン3　ジャンプ…                   */
      /*  （ジャンプ操作練習）  */
    TUTORIAL_Info07,   /**<  レッスン3　クリア！…                   */
    TUTORIAL_Info08,   /**<  レッスン4　エアリアル技…               */
      /*  （エアリアル操作練習）  */
    TUTORIAL_Info09,   /**<  レッスン4　クリア！…                   */
    TUTORIAL_Info10,   /**<  レッスン5　着水…                       */
      /*  （着水操作練習）  */
    TUTORIAL_Info11,   /**<  レッスン5　クリア!…                    */
    TUTORIAL_Info12,   /**<  全てのレッスン　クリア!…               */
    
    TUTORIAL_MAX
  };

  /**  技表表示 */
  enum SkillListMode
  {
    SKILLLIST_MODE_NOTHING, /**<  初期技も含めウインドウ以外全て非表示  */
    
    SKILLLIST_MODE_0,       /**<  技未解放  */
    SKILLLIST_MODE_1,       /**<  技1解放   */
    SKILLLIST_MODE_2,       /**<  技2解放   */
    SKILLLIST_MODE_3,       /**<  技3解放   */
  };


  /**  コースID  */
  enum CourseID
  {
    COURSE_NORMAL,
    COURSE_SUPER,
    COURSE_HYPER,
    COURSE_MASTER,
  };


  static bool IsHighScoreSkill(const SkillId skillId) {return ((SKILL_ID_EX_01 <= skillId) && (skillId <= SKILL_ID_EX_07));}
  static bool IsSkillUnlocked(const SkillId skillId);

public:

  MantainSurfAppData();
  virtual ~MantainSurfAppData();

  static f32 GetSkillBaseScore(const SkillId skillId, const u32 counter);
  static f32 GetSkillScoreRate(const u32 rank);

private:

};

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MANTAIN_SURF_APP_DATA_H_INCLUDED__