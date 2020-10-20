//======================================================================
/**
 * @file SurfSoundManager.h
 * @date 2017/2/14 15:30:00
 * @author fang_yicheng
 * @brief サウンドマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __SURF_SOUND_MANAGER_H_INCLUDED__
#define __SURF_SOUND_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(SurfSound)

/**
 * @class SurfSoundManager
 * @brief サウンドマネージャー
 */
class SurfSoundManager
{
  GFL_FORBID_COPY_AND_ASSIGN(SurfSoundManager);

public:

  // SEラベル
  enum SEIndex
  {
    SE_NONE = -1,
    SE_SURF_ENV,                  // サーフ環境音
    SE_SURF_LV_0,                 // サーフ通常移動Lv0(loop)
    SE_SURF_LV_1,                 // サーフ通常移動Lv1(loop)
    SE_SURF_LV_2,                 // サーフ通常移動Lv2(loop)
    SE_ACCEL,                     // 加速瞬間(左入力時/ターン時)
    SE_HIT_GIMMICK,               // 障害物ヒット音
    SE_TURN_OVER,                 // 転覆音
    SE_AERIAL_START,              // エアリアル開始
    SE_AERIAL_NORMAL,             // エアリアル待機(loop)
    SE_LANDING,                   // 着水
    SE_RECOVERING,                // 転覆移動(loop)
    SE_RECOVERED,                 // 転覆復帰
    SE_GIMMICK_ACTION_1,          // ギミック飛び出し（ホエルコ/サメハダー/メノクラゲ）
    SE_GIMMICK_ACTION_2,          // ギミック飛び出し（ホエルオー）(loop)
    SE_GIMMICK_ACTION_3,          // ギミック着水（サメハダー/メノクラゲ）
    SE_PADDLING_END,              // 自機が立つ時
    SE_SKILL_EX_01,               // スクリューハンテール
    SE_SKILL_EX_02,               // スクリューサクラビス
    SE_SKILL_EX_03,               // ランターン360
    SE_SKILL_EX_04,               // アシレーヌフリップ
    SE_SKILL_EX_05,               // オーバーザギャラドス
    SE_SKILL_EX_06,               // スターミートルネード
    SE_SKILL_EX_07,               // コイキングスペシャル
    SE_SKILL_NORMAL_01,           // 左回転、右回転
    SE_SKILL_NORMAL_02,           // 上回転、下回転
    SE_SUNSHINE,                  // 高く飛び上がった時の日光エフェクト音
    SE_SHOOTING_START,            // 高く飛び上がった時の流れ星エフェクト音
    SE_SKILL_END,                 // 技終了エフェクト音
    SE_SKILL_START,               // 大技開始エフェクト音
    SE_UI_SCORE_LOW,              // (UI)スコア低得点SE
    SE_UI_SCORE_HIGH,             // (UI)スコア高得点SE
    SE_UI_RESULT_INOUT,           // (UI)Result In/Out
    SE_UI_RESULTITEM_IN,          // (UI)Result 項目 in
    SE_UI_RESULT_COUNTUP_LOOP,    // (UI)Result カウントアップ（ループ）
    SE_UI_RESULT_COUNTUP_FINISH,  // (UI)Result カウントアップ 終了
    SE_MAX,
  };

  // サーフSE
  enum SurfSEIndex
  {
    SURF_SE_NONE = -1,
    SURF_SE_0,
    SURF_SE_1,
    SURF_SE_2,
  };

public:

  SurfSoundManager();
  virtual ~SurfSoundManager();

  void Initialize(MantainSurfAppManager* pAppManager);
  void Update();

  static void ChangeSurfSE(SurfSEIndex se);
  static void ChangeSurfSEVolume(f32 volume);
  static void PlaySE(SEIndex se, u32 fadein_frame = 0, f32 pitch = 1.0f);
  static void StopSE(SEIndex se, u32 fadeout_frame = 0);
  static void ChangeSEVolume(SEIndex se, f32 volume);
  static void ChangeSEPan(SEIndex se, f32 pan);
  static void ChangeSEPitch(SEIndex se, f32 pitch);
  static void StopAllSE(u32 fadeFrame = 0u);
  static bool IsAllSEStopped();

  static void StartBGM();
  static void EndBGM();

  static u32 GetSESoundId(SEIndex se);
  static u32 GetSESoundId(SurfSEIndex se);

  static bool StopSoundForTutorial(bool isForce);
  static void ResumeSoundForTutorial();

  static void ChangeMasterSEVolume(f32 volume, u32 changeFrame);

private:

  void UpdateSurfSE();
  void UpdateMasterSEVolume();

  static bool IsLoopSE(SEIndex se);

private:

  // SE管理情報
  struct SEInfo
  {
    u32 sound_id;
    f32 volume;

    SEInfo()
    {
      sound_id = 0;
      volume = 1.0f;
    }
  };

private:

  MantainSurfAppManager* m_pAppManager;

  static SurfSEIndex  m_SurfSENow;    // 現在再生中のサーフSE
  static SEInfo       m_SEInfoList[SE_MAX];

  static SEIndex      m_resumingSEList[SE_MAX];
  static u32          m_resumingSEListSize;

  static f32          m_masterSEVolume;

  struct MasterSEVolumeParam
  {
    f32  volumeStart;
    f32  volumeEnd;
    u32  changeFrame;
    u32  curFrame;

    void Initialize()
    {
      volumeStart = 1.0f;
      volumeEnd = 1.0f;
      changeFrame = 0;
      curFrame = 0;
    }
  };
  static MasterSEVolumeParam  m_MasterVolumeParam;
};

GFL_NAMESPACE_END(SurfSound)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __SURF_SOUND_MANAGER_H_INCLUDED__
