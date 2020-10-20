//======================================================================
/**
 * @file GimmickManager.h
 * @date 2016/09/13 12:20:58
 * @author fang_yicheng
 * @brief 障害物マネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GIMMICK_MANAGER_H_INCLUDED__
#define __GIMMICK_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "../../Collision/CollisionManager.h"
#include "../../Nami/NamiController.h"
#include "../../Player/PlayerModelSet.h"
#include "../../Player/PlayerController.h"
#include "../../Course/CourseDataManager.h"
#include "../../Camera/Camera.h"
#include "GimmickCommon.h"
#include "GimmickSamehader.h"
#include "GimmickHoeruko.h"
#include "GimmickHoeruoh.h"
#include "GimmickReefLarge.h"
#include "GimmickReefMedium.h"
#include "GimmickReefSmall.h"
#include "GimmickMenokurage.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

class GimmickManager
{
  GFL_FORBID_COPY_AND_ASSIGN(GimmickManager);

public:

  struct SetupData
  {
    gfl2::gfx::IGLAllocator*              m_pGfxAllocator;
    gfl2::heap::HeapBase*                 m_pHeap;
    MantainSurfRenderingPipeline*         m_pRenderingPipeLine;
    MantainSurfAppManager*                m_pAppManager;
    Collision::CollisionManager*          m_pCollisionManager;
    Resource::ResourceManager*            m_pResourceManager;
    Effect::EffectManager*                m_pEffectManager;
    Course::CourseDataManager*            m_pCourseDataManager;
    Nami::NamiController*                 m_pNamiController;
    Player::PlayerController*             m_pPlayerController;
    Player::PlayerModelSet*               m_pPlayerModelSet;
  };

private:

  enum BlockType
  {
    BLOCK_TYPE_UPWARD   = Course::CourseData::BLOCK_DATA_TYPE_UPWARD,      // 波上がりタイプ
    BLOCK_TYPE_DOWNWARD = Course::CourseData::BLOCK_DATA_TYPE_DOWNWARD,    // 波下がりタイプ
    BLOCK_TYPE_RANDOM   = Course::CourseData::BLOCK_DATA_TYPE_RANDOM,      // ランダムタイプ
    BLOCK_TYPE_FIXED,       // 固定タイプ
    BLOCK_TYPE_MAX,
  };

public:

  GimmickManager();
  virtual ~GimmickManager();

  void Initialize(SetupData &setupData);
  void Terminate();
  void Update();
  void UpdateAfterTraverse();
  void UpdateTimeZone(s32 timeZone);

  void ResetZPosition(const f32 resetLength);
  bool IsPlayerArrived();

  void StopEffect();
  void ResumeEffect();

  void MakeGimmickForTutorial();
  void ClearGimmickForTutorial();
  bool IsReachTutorialEndPosition();

#if PM_DEBUG
  void SetState(const GimmickBase::State state);
  void Reset();
  void MakeGimmickForDebug();
  GimmickBase** GetGimmickList() { return m_ppGimmickList; }
  u32  GetMaxGimmickSize() { return m_maxGimmickSize; }
#endif

private:

  void InitializeGimmickList();
  void CheckGimmickLifeCycle();
  void CreateGimmick(BlockType type, f32 startPosZ, const Course::CourseData::BlockDataOut* pBlockData);

  bool HasHoeruoh(const Course::CourseData::BlockDataOut* pBlockData);
  bool IsHitHoeruohBlockHistory(const u32 blockNum);

  void UpdateInput();
  void UpdateGimmick();
  void UpdateGimmickDraw();

  GimmickBase* GetGimmickModel(const u32 type, const u32 index);

  f32  CalcBlockNum(f32 posZ);
  f32  GetStartPosX(BlockType type, Course::CourseData::GimmickId gmkId);

  GimmickBase* FindUseableGimmick(BlockType type, Course::CourseData::GimmickId gmkId);

  bool CheckCollisionHoeruohCamera(GimmickHoeruoh* pHoeruoh, f32 &out_lenCameraToHoeruoh);
  bool CheckDrawEnable(GimmickBase *pGimmick);
  bool IsPointInsidePlane(const Camera::Camera::ViewPlane &plane, const gfl2::math::Vector3 &pointOnPlane);
  f32  CalcLenPointToLine(const gfl2::math::Vector3 &point, const gfl2::math::Vector3 &lineStart, const gfl2::math::Vector3 &lineEnd);

private:

  gfl2::gfx::IGLAllocator*              m_pGfxAllocator;
  gfl2::heap::HeapBase*                 m_pHeap;
  MantainSurfRenderingPipeline*         m_pRenderingPipeLine;
  MantainSurfAppManager*                m_pAppManager;
  Collision::CollisionManager*          m_pCollisionManager;
  Resource::ResourceManager*            m_pResourceManager;
  Effect::EffectManager*                m_pEffectManager;
  Course::CourseDataManager*            m_pCourseDataManager;
  Nami::NamiController*                 m_pNamiController;
  Player::PlayerController*             m_pPlayerController;
  Player::PlayerModelSet*               m_pPlayerModelSet;

  GimmickBase**                         m_ppGimmickList;
  u32                                   m_maxGimmickSize;
  u32                                   m_blockStartIndex[BLOCK_TYPE_MAX];
  u32                                   m_blockGimmickSize[BLOCK_TYPE_MAX];
  u32                                   m_gimmickStartIndex[BLOCK_TYPE_MAX][Course::CourseData::GMKID_MAX];
  u32                                   m_gimmickSizeList[BLOCK_TYPE_MAX][Course::CourseData::GMKID_MAX];

  Course::CourseData::BlockDataOut      m_BlockDataWork;

  gfl2::math::Vector3   m_playerInitPos;

  f32   m_curBlockCount;        // 現在作成済みのブロック番号
  f32   m_curFixedBlockCount;   // 現在作成済みの固定ブロック番号
  f32   m_totalResetLength;     // 全体リセットした距離
  f32   m_blockNumUpDown;       // 次に作成する波上り/上りのブロック番号
  f32   m_blockNumArrived;      // プレイヤーが到着したブロック番号
  bool  m_isPlayerArrived;      // プレイヤーが障害物発生ブロックに到着するか
  f32   m_tutorialEndPosZ;      // チュートリアル終了のZ位置

  gfl2::util::List<u32> m_HoeruOhBlockHistoryList;    // ホエルオーが出現するブロック履歴

#if PM_DEBUG
  // デバッグ用
  bool m_isShowGimmickAll;                      // ギミック表示全体ON/OFF用デバッグフラグ
  bool m_isShowGimmick[GimmickBase::TYPE_MAX];  // 各ギミック表示ON/OFF用デバッグフラグ
#endif

};

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __GIMMICK_MANAGER_H_INCLUDED__
