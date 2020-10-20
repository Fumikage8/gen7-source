//======================================================================
/**
 * @file CharaViewerCharaModelManagerAnimation.h
 * @date 2015/07/24 20:05:03
 * @author takebe_makoto
 * @brief キャラビューア：キャラモデル管理：アニメーション
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __CHARA_VIEWER_CHARA_MODEL_MANAGER_ANIMATION__
#define __CHARA_VIEWER_CHARA_MODEL_MANAGER_ANIMATION__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

// niji
#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"
#include <system/include/nijiAllocator.h>

#include <model/include/gfl2_CharaModelFactory.h>
#include <model/include/gfl2_CharaModel.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include "CharaViewerHeaderId.h"

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)

/**
 * @class CharaViewerCharaModelManager_Animation
 * @brief キャラモデル管理：アニメーション
 */
class CharaViewerCharaModelManager_Animation
{
private:
  GFL_FORBID_COPY_AND_ASSIGN(CharaViewerCharaModelManager_Animation);

public:
  enum CheckMotionType
  {
    CHECK_MOTION_TYPE_BATTLE_MALE,
    CHECK_MOTION_TYPE_FIELD_MALE,
    CHECK_MOTION_TYPE_BATTLE_FEMALE,
    CHECK_MOTION_TYPE_FIELD_FEMALE,

    CHECK_MOTION_TYPE_COUNT
  };

  enum FpsMode
  {
    FPS_MODE_15,
    FPS_MODE_30,

    FPS_MODE_COUNT
  };

  enum PlayState
  {
    PLAY_STATE_PAUSE,
    PLAY_STATE_PLAY,
    PLAY_STATE_END,

    PLAY_STATE_BIND_POSE,
    PLAY_STATE_CHECK_MOTION,
    PLAY_STATE_CHECK_MOTION_PAUSE,

    PLAY_STATE_COUNT
  };

  static const s32 MOTION_LIST_COUNT = 6;

private:

  struct CheckMotion
  {
    void* pData;
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode;
  };

private:

  struct MotionUnit
  {
  public:

    void Initialize()
    {
      m_Enabled = false;
      m_ListIndex = -1;
      m_MotionIndex = -1;
      m_TotalFrame = 0;
      m_CurrentFrame = 0;
    }

    void SetEnabled(bool enabled)
    {
      m_Enabled = enabled;
    }

    void ToggleEnabled()
    {
      m_Enabled ^= 1;
    }

    bool IsEnabled() const
    {
      return GFL_BOOL_CAST(m_Enabled);
    }

    void SetMotionIndex(s32 listIndex, s32 motionIndex)
    {
      m_ListIndex = listIndex;
      m_MotionIndex = motionIndex;
    }

    s32 GetListIndex() const
    {
      return m_ListIndex;
    }

    s32 GetMotionIndex() const
    {
      return m_MotionIndex;
    }

    void SetTotalFrame(u32 totalFrame)
    {
      m_TotalFrame = totalFrame;
    }

    u32 GetTotalFrame() const
    {
      return m_TotalFrame;
    }

    void SetCurrentFrame(u32 currentFrame)
    {
      m_CurrentFrame = currentFrame;
    }

    u32 GetCurrentFrame() const
    {
      return m_CurrentFrame;
    }

  private:
    b8 m_Enabled;
    s32 m_ListIndex;
    s32 m_MotionIndex;
    u32 m_TotalFrame;
    u32 m_CurrentFrame;
  };

public:

  struct StoreValue
  {
    MotionUnit motionUnitList[MOTION_LIST_COUNT];
    u32 currentMotionIndex;
    bool loopFlag;
    s32 fpsUpdateCount;
    FpsMode fpsMode;
    PlayState playState;
    CheckMotionType checkMotionType;
    u32 currentFrame; 
  };

  struct SortMotionId
  {
    const char* pName;
    s32 motionId;
  };

public:

  CharaViewerCharaModelManager_Animation();
  ~CharaViewerCharaModelManager_Animation();

  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, u32 arcId, u32 datIdList[CHECK_MOTION_TYPE_COUNT]);
  void Finalize();

  void Store(StoreValue* out_pStoreValue);
  void Restore(StoreValue* pStoreValue);

  void ChangeCharaModel(gfl2::heap::HeapBase* pHeap, poke_3d::model::CharaModel* pCharaModel, poke_3d::model::CharaModel* pSubObjectModel, const HeaderIdList** pMotionIdList, u32 motionIdCount);
  void Update();

  // 4ページ目
  bool IsFpsMode30() const;
  void GetCurrentMotionName(u32 index, const char** out_ppMotionName, s32* out_pListIndex, s32* out_pMotionIndex) const;
  u32 GetCurrentMotionTotalFrame(u32 index) const;
  u32 GetCurrentMotionFrame(u32 index) const;
  bool IsMotionEnabled(u32 index) const;
  bool IsMotionLoop() const;
  void ToggleLoopFlag();
  void SetLoopFlag(bool loopFlag);
  void PlaySingleFrame(bool isForward);
  bool IsMotionPlaying() const;
  void PlayMotion();
  void SlideMotionSelector(u8 index, s32 slideValue);
  void ToggleMotion(u8 index);
  void PlayTPose();
  void PlayStackCheckMotion(CheckMotionType type);
  bool GetTranslateFlag() const;
  void ToggleTranslateFlag();
  void SlideFpsSelector(bool isForward);

  s32 GetCurrentAnimationId() const;

  const gfl2::math::Vector3& GetWalkPosition() const;

private:

  s32 GetStepFrame() const;
  void InitMotionList();
  void PlayStart();
  void ChangeAnimation(s32 motionId, bool isSmooth = true);
private:

  poke_3d::model::CharaModel* m_pCharaModel;
  poke_3d::model::CharaModel* m_pSubObjectModel;
  const HeaderIdList** m_pMotionIdList;
  SortMotionId** m_pSortedMotionIdList;
  u32 m_MotionIdCount;
  MotionUnit m_MotionUnitList[MOTION_LIST_COUNT];
  u32 m_CurrentMotionIndex;
  bool m_LoopFlag;
  bool m_HasAnimationFlag;
  bool m_WalkFlag;
  s32 m_FpsUpdateCount;
  FpsMode m_FpsMode;
  PlayState m_PlayState;
  CheckMotion m_CheckMotionList[CHECK_MOTION_TYPE_COUNT];
  CheckMotionType m_CheckMotionType;
  gfl2::math::Vector3 m_WalkPosition;
};

GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // __HEADER_H_INCLUDED__

#endif // PM_DEBUG
