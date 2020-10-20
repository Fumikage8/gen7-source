//======================================================================
/**
 * @file DebugMenu.h
 * @date 2016/12/21 21:41:21
 * @author fang_yicheng
 * @brief マンタインサーフのデバッグメニュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __DEBUG_MENU_H_INCLUDED__
#define __DEBUG_MENU_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include "../Model/Gimmick/GimmickBase.h"
#include "../MantainSurfRenderingPipeline.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(MantainSurf)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Debug)

class DebugMenu
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugMenu);

public:

  enum
  {
    SRB_CONST_MAX_SKILL_NUM = 4,
    SRB_CONST_MAX_KEY_FRAME_NUM = 4,
  };

public:

  DebugMenu();
  ~DebugMenu();

  void Initialize(gfl2::heap::HeapBase* pHeap, MantainSurfAppManager* pAppManager);

  MantainSurfAppManager* GetAppManager();

public:

  bool m_dbvIsHotKeyEnable;
  bool m_dbvIsUIDrawEnable;
  bool m_dbvIsUIUppderDrawEnable;
  bool m_dbvIsUILowerDrawEnable;
  bool m_dbvIsCameraOffsetMode;
  bool m_dbvIsShowGimmickAll;
  bool m_dbvIsShowGimmick[Model::Gimmick::GimmickBase::TYPE_MAX];
  s32  m_dbvNextCourseId;
  s32  m_dbvPlayingEffectResId;

  f32  m_dbvScoreRateBase;
  f32  m_dbvScoreRateInput;
  f32  m_dbvScoreRateSpeed;
  f32  m_dbvScoreRateLanding;
  f32  m_dbvScoreRateRank[MantainSurfAppData::SKILL_SCORE_RANK_MAX];

  f32  m_scoreRateInput;
  s32  m_scoreSkillTimer;
  s32  m_scoreWaitTimer;

  f32  m_dbvSurfParamTurnRadiusUpward;
  f32  m_dbvSurfParamTurnRadiusDownward;
  f32  m_dbvSurfParamTurnSpeedUpward;
  f32  m_dbvSurfParamTurnSpeedDownward;
  f32  m_dbvSurfParamTurnAngleUpward;
  f32  m_dbvSurfParamTurnAngleDownward;
  f32  m_dbvSurfParamTurnFactorUpward;
  f32  m_dbvSurfParamTurnFactorDownward;
  f32  m_dbvSurfParamTurnSpeedAccel;
  f32  m_dbvSurfParamTurnAccelRadiusMag;
  f32  m_dbvSurfParamAccelForward;
  f32  m_dbvSurfParamLandingAgility;

  bool m_dbvDrawEnable[MantainSurfRenderingPipeline::VIEWER_PATH_COUNT];

  s32  m_dbvSkyBoxTimeHour;
  s32  m_dbvSkyBoxTimeMinute;
  s32  m_dbvSkyBoxTimeSecond;
  s32  m_dbvSkyBoxDay;
  f32  m_dbvSkyBoxRotationY;
  f32  m_dbvSkyBoxPositionY;
  f32  m_dbvLightingColorR;
  f32  m_dbvLightingColorG;
  f32  m_dbvLightingColorB;

  f32  m_dbvBgSeaScale;
  f32  m_dbvBgIslandScale;
  f32  m_dbvBgIslandOffsetX;
  f32  m_dbvBgIslandOffsetY;
  f32  m_dbvBgIslandOffsetZ;

  f32  m_dbvCameraFovy;
  f32  m_dbvCameraNear;
  f32  m_dbvCameraFar;
  f32  m_dbvCameraNormalLength;
  f32  m_dbvCameraZoomStep;

  s32  m_dbvCourseIdNow;
  bool m_dbvIsCourseRestart;
  bool m_dbvIsInfinityMode;
  bool m_dbvIsWaveHeightFix;
  bool m_dbvIsCollisionCheckOn;
  bool m_dbvIsUseSkillZRotBlend;
  bool m_dbvIsAutoPlayMode;
  bool m_dbvIsSyncWavePosition;
  bool m_dbvIsFixSkillInputFactor;

  bool m_dbvIsRestartTest;
  s32  m_dbvRestartInterval;

  s32  m_dbvSurfRightBlendFrame;

  s32  m_effectResId;
  s32  m_timeZone;
  s32  m_moonPattern;
  s32  m_seIndex1;
  s32  m_seIndex2;
  s32  m_seIndexNow1;
  s32  m_seIndexNow2;
  f32  m_sePitch1;
  f32  m_sePitch2;
  f32  m_jumpSpeedStart;
  f32  m_aerialTime;

  bool m_isAllSkillUnlocked;

  f32  m_dbvSkillRotBlendTables[SRB_CONST_MAX_SKILL_NUM * SRB_CONST_MAX_KEY_FRAME_NUM * 3];
  s32  m_curSkillRotBlendTableIndex;
  f32* m_curSkillRotBlendTable;

private:

  void InitSkillRotBlendTable();

private:

  gfl2::heap::HeapBase*       m_pHeap;
  MantainSurfAppManager*      m_pAppManager;
  gfl2::debug::DebugWinGroup* m_pRootDebugWinGroup;

};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __DEBUG_MENU_H_INCLUDED__

#endif // PM_DEBUG