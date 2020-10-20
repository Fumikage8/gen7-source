//======================================================================
/**
 * @file CharaViewerCharaModelManager_Animationo.cpp
 * @date 2015/07/24 20:07:20
 * @author takebe_makoto
 * @brief キャラビューア：キャラモデル管理：アニメーション
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "../include/CharaViewerCharaModelManager_Animation.h"

#include <model/include/gfl2_CharaModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)

using namespace poke_3d::model;

static const char* MOTION_NONE_NAME = "-- NONE --";

CharaViewerCharaModelManager_Animation::CharaViewerCharaModelManager_Animation() :
  m_pCharaModel(NULL),
  m_pMotionIdList(NULL),
  m_pSortedMotionIdList(NULL),
  m_MotionIdCount(0),
  m_CurrentMotionIndex(0),
  m_LoopFlag(false),
  m_HasAnimationFlag(false),
  m_WalkFlag(false),
  m_FpsUpdateCount(0),
  m_FpsMode(FPS_MODE_30),
  m_PlayState(PLAY_STATE_PAUSE),
  m_CheckMotionType(CHECK_MOTION_TYPE_BATTLE_MALE)
{
  this->InitMotionList();
}

CharaViewerCharaModelManager_Animation::~CharaViewerCharaModelManager_Animation()
{
}

void CharaViewerCharaModelManager_Animation::Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, u32 arcId, u32 datIdList[CHECK_MOTION_TYPE_COUNT])
{
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId				= arcId;
    req.heapForFile	= pHeap->GetLowerHandle();
    req.heapForReq	= pHeap->GetLowerHandle();

    pFileManager->SyncArcFileOpen(req);
  }

  for (u32 i = 0; i < CHECK_MOTION_TYPE_COUNT; ++i)
  {
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.arcId      = arcId;
      req.datId      = datIdList[i];
      req.ppBuf      = &m_CheckMotionList[i].pData;
      req.heapForBuf = pHeap;
      req.align      = 128; // テクスチャは128バイトアラインメント
      req.heapForReq = pHeap->GetLowerHandle();
      req.heapForCompressed = NULL;

      pFileManager->SyncArcFileLoadData(req);
    }

    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData motData;
      motData.SetMotionData((c8*)m_CheckMotionList[i].pData);

      m_CheckMotionList[i].pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(pGfxAllocator, &motData);
    }
  }

  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId				= arcId;
    req.heapForReq	= pHeap->GetLowerHandle();

    pFileManager->SyncArcFileClose(req);
  }
}

void CharaViewerCharaModelManager_Animation::Finalize()
{
  if (m_pSortedMotionIdList != NULL)
  {
    for (u32 i = 0; i < m_MotionIdCount; ++i)
    {
      GFL_SAFE_DELETE_ARRAY(m_pSortedMotionIdList[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_pSortedMotionIdList);
  }

  for (u32 i = 0; i < CHECK_MOTION_TYPE_COUNT; ++i)
  {
    GFL_SAFE_DELETE(m_CheckMotionList[i].pResourceNode);
    GflHeapSafeFreeMemory(m_CheckMotionList[i].pData);
  }
}

void CharaViewerCharaModelManager_Animation::Store(StoreValue* out_pStoreValue)
{
  for (u32 i = 0; i < MOTION_LIST_COUNT; ++i)
  {
    out_pStoreValue->motionUnitList[i] = m_MotionUnitList[i];
  }

  out_pStoreValue->currentMotionIndex = m_CurrentMotionIndex;
  out_pStoreValue->loopFlag = m_LoopFlag;
  out_pStoreValue->fpsUpdateCount = m_FpsUpdateCount;
  out_pStoreValue->fpsMode = m_FpsMode;
  out_pStoreValue->playState = m_PlayState;
  out_pStoreValue->checkMotionType = m_CheckMotionType;
  out_pStoreValue->currentFrame = (m_pCharaModel == NULL)
    ? 0
    : m_pCharaModel->GetAnimationFrame();
}

void CharaViewerCharaModelManager_Animation::Restore(StoreValue* pStoreValue)
{
  for (u32 i = 0; i < MOTION_LIST_COUNT; ++i)
  {
    m_MotionUnitList[i] = pStoreValue->motionUnitList[i];
  }

  m_CurrentMotionIndex = pStoreValue->currentMotionIndex ;
  m_LoopFlag = pStoreValue->loopFlag ;
  m_FpsUpdateCount = pStoreValue->fpsUpdateCount ;
  m_FpsMode = pStoreValue->fpsMode ;
  m_PlayState = pStoreValue->playState;
  m_CheckMotionType = pStoreValue->checkMotionType;

  // モーションリストがセットされていない
  if (m_pMotionIdList == NULL)
  {
    return;
  }

  s32 listIndex = m_MotionUnitList[m_CurrentMotionIndex].GetListIndex();
  s32 motionIndex = m_MotionUnitList[m_CurrentMotionIndex].GetMotionIndex();
  s32 motionId = (motionIndex < 0)
    ? -1
    : (s32)m_pMotionIdList[listIndex]->GetData(m_pSortedMotionIdList[listIndex][motionIndex].motionId)->id;
  this->ChangeAnimation(motionId, false);

  u32 currentFrame = m_MotionUnitList[m_CurrentMotionIndex].GetCurrentFrame();

  // Tポーズとチェックモーションは特別
  if (m_PlayState == PLAY_STATE_BIND_POSE)
  {
    this->PlayTPose();
  }
  // めり込み確認モーション
  else if ((m_PlayState == PLAY_STATE_CHECK_MOTION) || (m_PlayState == PLAY_STATE_CHECK_MOTION_PAUSE))
  {
    this->PlayStackCheckMotion(m_CheckMotionType);
    currentFrame = pStoreValue->currentFrame;

    // ポーズが解除されるのでもう一度入れる
    m_PlayState = pStoreValue->playState;
  }

  m_pCharaModel->SetAnimationStepFrame(0);
  m_pCharaModel->SetAnimationFrame(currentFrame);
  m_pCharaModel->UpdateAnimation();
  m_pCharaModel->SetAnimationStepFrame(this->GetStepFrame());
}

bool SortMotionFunc(const CharaViewerCharaModelManager_Animation::SortMotionId& a, const CharaViewerCharaModelManager_Animation::SortMotionId& b)
{
  return ::strcmp(a.pName, b.pName) < 0;
}

void CharaViewerCharaModelManager_Animation::ChangeCharaModel(gfl2::heap::HeapBase* pHeap, poke_3d::model::CharaModel* pCharaModel, poke_3d::model::CharaModel* pSubObjectModel, const HeaderIdList** pMotionIdList, u32 motionIdCount)
{
  if (m_pSortedMotionIdList != NULL)
  {
    for (u32 i = 0; i < m_MotionIdCount; ++i)
    {
      GFL_SAFE_DELETE_ARRAY(m_pSortedMotionIdList[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_pSortedMotionIdList);
  }

  m_pCharaModel = pCharaModel;
  m_pSubObjectModel = pSubObjectModel;
  m_pMotionIdList = pMotionIdList;
  m_MotionIdCount = motionIdCount;
  m_WalkPosition.Set(0, 0, 0);

  this->InitMotionList();

  if (m_pCharaModel == NULL)
  {
    return;
  }

  // モーションIDをソートする
  if (m_MotionIdCount > 0)
  {
    const gfl2::animation::AnimationPackList* animationPackList = m_pCharaModel->GetAnimationPackList();

    m_pSortedMotionIdList = GFL_NEW_ARRAY(pHeap) SortMotionId*[m_MotionIdCount];
    u32 emptyPackCount = 0;
    for (u32 i = 0; i < m_MotionIdCount; ++i)
    {
      m_pSortedMotionIdList[i] = GFL_NEW_ARRAY(pHeap) SortMotionId[m_pMotionIdList[i]->GetDataCount()];

      u32 emptyAnimCount = 0;
      for (u32 j = 0; j < m_pMotionIdList[i]->GetDataCount(); ++j)
      {
        m_pSortedMotionIdList[i][j].pName = m_pMotionIdList[i]->GetData(j)->pName;
        m_pSortedMotionIdList[i][j].motionId = j;

        u32 motionId = m_pMotionIdList[i]->GetData(j)->id;
        gfl2::animation::PackedAnimationId packedAnimationId(motionId);

        bool isEmptyAnim = false;

        // パック数オーバーチェック
        bool packIdOver = (packedAnimationId.packId >= animationPackList->GetAnimationPackCount());
        if (packIdOver)
        {
          isEmptyAnim = true;
        }
        // アニメーション数オーバーチェック
        if (!isEmptyAnim)
        {
          bool animationIdOver = (packedAnimationId.animationId >= animationPackList->GetResourceCount(packedAnimationId.packId));
          if (animationIdOver)
          {
            isEmptyAnim = true;
          }
        }
        // 空リソースチェック
        if (!isEmptyAnim)
        {
          isEmptyAnim = (animationPackList->GetResourceNode(packedAnimationId.packId, packedAnimationId.animationId) == NULL);
        }

        if (isEmptyAnim)
        {
          m_pSortedMotionIdList[i][j].motionId = -1;
          ++emptyAnimCount;
        }
      }

      if (emptyAnimCount == m_pMotionIdList[i]->GetDataCount())
      {
        ++emptyPackCount;
      }

      ::std::sort(&m_pSortedMotionIdList[i][0], &m_pSortedMotionIdList[i][m_pMotionIdList[i]->GetDataCount()], SortMotionFunc);
    }

    m_HasAnimationFlag = (emptyPackCount != m_MotionIdCount);
  }

  // アニメーションは設定しない

  m_PlayState = PLAY_STATE_PAUSE;
}

void CharaViewerCharaModelManager_Animation::Update()
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  // 表情更新
  m_pCharaModel->UpdateEye();
  m_pCharaModel->UpdateMouth();

  // 15FPSの対応：2フレームに一回だけ更新
  ++m_FpsUpdateCount;
  if (m_FpsMode == FPS_MODE_15)
  {
    if (m_FpsUpdateCount < 2)
    {
      return;
    }
  }
  m_FpsUpdateCount = 0;

  // チェックモーション中なら必ず更新
  if ((m_PlayState == PLAY_STATE_BIND_POSE) || (m_PlayState == PLAY_STATE_CHECK_MOTION))
  {
    m_pCharaModel->SetAnimationStepFrame(this->GetStepFrame());
    m_pCharaModel->UpdateAnimation();

    if (m_pSubObjectModel != NULL)
    {
      m_pSubObjectModel->SetAnimationStepFrame(this->GetStepFrame());
      m_pSubObjectModel->UpdateAnimation();
    }
    return;
  }

  // 再生中でなければ無視
  if (m_PlayState != PLAY_STATE_PLAY)
  {
    m_pCharaModel->SetAnimationStepFrame(0);
    m_pCharaModel->UpdateAnimation();

    if (m_pSubObjectModel != NULL)
    {
      m_pSubObjectModel->SetAnimationStepFrame(0);
      m_pSubObjectModel->UpdateAnimation();
    }
    return;
  }

  // 現在のフレームセット
  m_MotionUnitList[m_CurrentMotionIndex].SetCurrentFrame(m_pCharaModel->GetAnimationFrame());

  s32 animationFrame = (s32)m_pCharaModel->GetAnimationFrame();

  // アニメーション更新
  m_pCharaModel->SetAnimationStepFrame(this->GetStepFrame());
  m_pCharaModel->UpdateAnimation();

  if (m_pSubObjectModel != NULL)
  {
    m_pSubObjectModel->SetAnimationStepFrame(this->GetStepFrame());
    m_pSubObjectModel->SetAnimationLoop(m_pCharaModel->IsAnimationLoop());
    m_pSubObjectModel->UpdateAnimation();
  }

  // 歩き位置更新
  if (m_WalkFlag)
  {
    gfl2::math::Vector3 walkSpeed = m_pCharaModel->GetWalkSpeed(-1);
    m_WalkPosition.x = 0.0f;
    m_WalkPosition.y = walkSpeed.y;
    m_WalkPosition.z += walkSpeed.z;
  }

  // 再生終了したら次のアニメーションに変更
  if (m_pCharaModel->IsAnimationLastFrame())
  {
    u32 nextMotionIndex = m_CurrentMotionIndex;
    for (u32 i = nextMotionIndex + 1; i < MOTION_LIST_COUNT; ++i)
    {
      if (m_MotionUnitList[i].IsEnabled())
      {
        nextMotionIndex = i;
        break;
      }
    }

    // 次のモーションが同じならループチェックする
    if (m_CurrentMotionIndex == nextMotionIndex)
    {
      // 歩き位置初期化
      m_WalkPosition.Set(0, 0, 0);

      if (m_LoopFlag)
      {
        nextMotionIndex = 0;
        for (u32 i = 0; i < MOTION_LIST_COUNT; ++i)
        {
          if (m_MotionUnitList[i].IsEnabled())
          {
            nextMotionIndex = i;
            break;
          }
        }

        // 次のアニメーションに切り替え
        m_CurrentMotionIndex = nextMotionIndex;

        s32 listIndex = m_MotionUnitList[m_CurrentMotionIndex].GetListIndex();
        s32 motionIndex = m_MotionUnitList[m_CurrentMotionIndex].GetMotionIndex();
        s32 motionId = (motionIndex < 0)
          ? -1
          : (s32)m_pMotionIdList[listIndex]->GetData(m_pSortedMotionIdList[listIndex][motionIndex].motionId)->id;
        this->ChangeAnimation(motionId);
      }
      else
      {
        // 再生終了に変更
        m_PlayState = PLAY_STATE_END;
      }
    }
    else
    {
      // 次のアニメーションに切り替え
      m_CurrentMotionIndex = nextMotionIndex;

      s32 listIndex = m_MotionUnitList[m_CurrentMotionIndex].GetListIndex();
      s32 motionIndex = m_MotionUnitList[m_CurrentMotionIndex].GetMotionIndex();
      s32 motionId = (motionIndex < 0)
        ? -1
        : (s32)m_pMotionIdList[listIndex]->GetData(m_pSortedMotionIdList[listIndex][motionIndex].motionId)->id;
      this->ChangeAnimation(motionId);
    }
  }
}

// 4ページ目
bool CharaViewerCharaModelManager_Animation::IsFpsMode30() const
{
  return (m_FpsMode == FPS_MODE_30);
}

void CharaViewerCharaModelManager_Animation::GetCurrentMotionName(u32 index, const char** out_ppMotionName, s32* out_pListIndex, s32* out_pMotionIndex) const
{
  *out_ppMotionName = MOTION_NONE_NAME;
  *out_pListIndex = -1;
  *out_pMotionIndex = -1;

  // モーションリストがセットされていない
  if (m_pMotionIdList == NULL)
  {
    return;
  }

  // リストインデックスが無効
  s32 listIndex = m_MotionUnitList[index].GetListIndex();
  if (listIndex < 0)
  {
    return;
  }

  // モーションインデックスが無効
  s32 motionIndex = m_MotionUnitList[index].GetMotionIndex();
  if (motionIndex < 0)
  {
    return;
  }

  *out_ppMotionName = m_pMotionIdList[listIndex]->GetData(m_pSortedMotionIdList[listIndex][motionIndex].motionId)->pName;
  *out_pListIndex = listIndex;
  *out_pMotionIndex = motionIndex;
}

u32 CharaViewerCharaModelManager_Animation::GetCurrentMotionTotalFrame(u32 index) const
{
  if (m_pCharaModel == NULL)
  {
    return 0;
  }

  return m_MotionUnitList[index].GetTotalFrame();
}

u32 CharaViewerCharaModelManager_Animation::GetCurrentMotionFrame(u32 index) const
{
  if (m_pCharaModel == NULL)
  {
    return 0;
  }

  return m_MotionUnitList[index].GetCurrentFrame();
}

bool CharaViewerCharaModelManager_Animation::IsMotionEnabled(u32 index) const
{
  if (m_pCharaModel == NULL)
  {
    return false;
  }

  return m_MotionUnitList[index].IsEnabled();
}

bool CharaViewerCharaModelManager_Animation::IsMotionLoop() const
{
  if (m_pCharaModel == NULL)
  {
    return false;
  }

  return m_LoopFlag;
}

void CharaViewerCharaModelManager_Animation::ToggleLoopFlag()
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  m_LoopFlag ^= 1;
  m_pCharaModel->SetAnimationLoop(m_LoopFlag);
  if (m_pSubObjectModel != NULL)
  {
    m_pSubObjectModel->SetAnimationLoop(m_LoopFlag);
  }
}

void CharaViewerCharaModelManager_Animation::SetLoopFlag(bool loopFlag)
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  m_LoopFlag = loopFlag;
  m_pCharaModel->SetAnimationLoop(m_LoopFlag);
  if (m_pSubObjectModel != NULL)
  {
    m_pSubObjectModel->SetAnimationLoop(m_LoopFlag);
  }
}

void CharaViewerCharaModelManager_Animation::PlaySingleFrame(bool isForward)
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  s32 frame = m_pCharaModel->GetAnimationFrame();
  s32 step = this->GetStepFrame();
  frame += (isForward) ? step : -step;
  if (frame > static_cast<s32>(m_pCharaModel->GetAnimationEndFrame()))
  {
    frame = 0;
  }
  else if (frame < 0)
  {
    frame = m_pCharaModel->GetAnimationEndFrame();
  }
  m_MotionUnitList[m_CurrentMotionIndex].SetCurrentFrame(frame);
  m_pCharaModel->SetAnimationFrame(frame);
  m_pCharaModel->SetAnimationStepFrame(0.0f);
  m_pCharaModel->UpdateAnimation();
  m_pCharaModel->SetAnimationStepFrame(this->GetStepFrame());

  if (m_pSubObjectModel != NULL)
  {
    m_pSubObjectModel->SetAnimationFrame(frame);
    m_pSubObjectModel->SetAnimationStepFrame(0.0f);
    m_pSubObjectModel->UpdateAnimation();
    m_pSubObjectModel->SetAnimationStepFrame(this->GetStepFrame());
  }

  if ((m_PlayState == PLAY_STATE_CHECK_MOTION) || (m_PlayState == PLAY_STATE_CHECK_MOTION_PAUSE))
  {
    m_PlayState = PLAY_STATE_CHECK_MOTION_PAUSE;
  }
  else
  {
    m_PlayState = PLAY_STATE_PAUSE;
  }

  // 歩き位置更新
  if (m_WalkFlag)
  {
    gfl2::math::Vector3 walkSpeed = m_pCharaModel->GetWalkSpeed(-1);

    if (!isForward)
    {
      walkSpeed.z = -walkSpeed.z;
    }

    m_WalkPosition.x = 0.0f;
    m_WalkPosition.y = walkSpeed.y;
    m_WalkPosition.z += walkSpeed.z;

    if (frame == 0)
    {
      m_WalkPosition.z = 0.0f;
    }
  }
}

bool CharaViewerCharaModelManager_Animation::IsMotionPlaying() const
{
  if (m_pCharaModel == NULL)
  {
    return false;
  }

  return (m_PlayState == PLAY_STATE_PLAY) || (m_PlayState == PLAY_STATE_CHECK_MOTION);
}

void CharaViewerCharaModelManager_Animation::PlayMotion()
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  if (this->IsMotionPlaying())
  {
    // 止める
    if (m_PlayState == PLAY_STATE_CHECK_MOTION)
    {
      m_PlayState = PLAY_STATE_CHECK_MOTION_PAUSE;
    }
    else
    {
      m_PlayState = PLAY_STATE_PAUSE;
    }
  }
  else
  {
    // 一時停止中以外なら最初から再生する
    if ((m_PlayState != PLAY_STATE_PAUSE) && (m_PlayState != PLAY_STATE_CHECK_MOTION_PAUSE))
    {
      this->PlayStart();
    }
    m_pCharaModel->SetAnimationStepFrame(this->GetStepFrame());

    if (m_pSubObjectModel != NULL)
    {
      m_pSubObjectModel->SetAnimationStepFrame(this->GetStepFrame());
    }

    // チェックモーション
    if (m_PlayState == PLAY_STATE_CHECK_MOTION_PAUSE)
    {
      m_PlayState = PLAY_STATE_CHECK_MOTION;
    }
    else
    {
      // 再生
      m_PlayState = PLAY_STATE_PLAY;
    }
  }
}

void CharaViewerCharaModelManager_Animation::SlideMotionSelector(u8 index, s32 slideValue)
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  if (m_pMotionIdList == NULL)
  {
    return;
  }

  if (m_MotionIdCount <= 0)
  {
    return;
  }

  if (!m_HasAnimationFlag)
  {
    return;
  }

  s32 tmpMotionId = -1;
  s32 listIndex = listIndex = m_MotionUnitList[index].GetListIndex();
  s32 motionIndex = m_MotionUnitList[index].GetMotionIndex();

  while (tmpMotionId < 0)
  {
    // モーションインデックスをスライドさせる
    motionIndex += slideValue;
    if (motionIndex < 0)
    {
      --listIndex;
      if (listIndex < 0)
      {
        listIndex = m_MotionIdCount - 1;
      }

      motionIndex = m_pMotionIdList[listIndex]->GetDataCount() - 1;
    }
    else if (listIndex >= 0)
    {
      if (static_cast<u32>(motionIndex) >= m_pMotionIdList[listIndex]->GetDataCount())
      {
        ++listIndex;
        if (listIndex >= static_cast<s32>(m_MotionIdCount))
        {
          listIndex = 0;
        }

        motionIndex = 0;
      }
    }

    // インデックス範囲を制限
    if (listIndex < 0)
    {
      listIndex = 0;
    }
    else if (listIndex >= static_cast<s32>(m_MotionIdCount))
    {
      listIndex = m_MotionIdCount - 1;
    }
    if (motionIndex < 0)
    {
      motionIndex = 0;
    }
    else if (static_cast<u32>(motionIndex) >= m_pMotionIdList[listIndex]->GetDataCount())
    {
      motionIndex = m_pMotionIdList[listIndex]->GetDataCount() - 1;
    }

    m_MotionUnitList[index].SetMotionIndex(listIndex, motionIndex);

    // 選択されたモーションのトータルフレームを取得
    if (listIndex < 0)
    {
      listIndex = m_MotionIdCount - 1;
    }

    tmpMotionId = m_pSortedMotionIdList[listIndex][motionIndex].motionId;
  }

  s32 motionId = m_pMotionIdList[listIndex]->GetData(tmpMotionId)->id;
  m_pCharaModel->ChangeAnimation(motionId);
  u32 totalFrame = m_pCharaModel->GetAnimationEndFrame();
  m_MotionUnitList[index].SetTotalFrame(totalFrame);

  if (m_pSubObjectModel != NULL)
  {
    m_pSubObjectModel->ChangeAnimation(motionId);
  }

  // スライドされた項目は有効にする
  m_MotionUnitList[index].SetEnabled(true);

  // 最初から再生し直す
  this->PlayStart();
}

void CharaViewerCharaModelManager_Animation::ToggleMotion(u8 index)
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  m_MotionUnitList[index].ToggleEnabled();
}

void CharaViewerCharaModelManager_Animation::PlayTPose()
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  m_pCharaModel->ChangeAnimationBindpose();

  if (m_pSubObjectModel != NULL)
  {
    m_pSubObjectModel->ChangeAnimationBindpose();
  }

  m_PlayState = PLAY_STATE_BIND_POSE;
}

void CharaViewerCharaModelManager_Animation::PlayStackCheckMotion(CheckMotionType type)
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  DressUpModel* pDressUpModel = static_cast<DressUpModel*>(m_pCharaModel);
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pPartsModel = pDressUpModel->GetPartsModel(static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if (pPartsModel == NULL)
    {
      continue;
    }

    if (!pPartsModel->HasAnimationController())
    {
      continue;
    }

    pPartsModel->ChangeAnimationByResourceNode(m_CheckMotionList[type].pResourceNode);
  }

  m_pCharaModel->ChangeAnimationByResourceNode(m_CheckMotionList[type].pResourceNode);
  m_pCharaModel->SetAnimationStepFrame(this->GetStepFrame());
  m_pCharaModel->SetAnimationLoop(true);
  m_PlayState = PLAY_STATE_CHECK_MOTION;
  m_CheckMotionType = type;
}

bool CharaViewerCharaModelManager_Animation::GetTranslateFlag() const
{
  if (m_pCharaModel == NULL)
  {
    return false;
  }

  return m_WalkFlag;
}

void CharaViewerCharaModelManager_Animation::ToggleTranslateFlag()
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  // 歩きフラグ入れ替え
  m_WalkFlag = !m_WalkFlag;
  m_WalkPosition.Set(0, 0, 0);
}

void CharaViewerCharaModelManager_Animation::SlideFpsSelector(bool isForward)
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  // FPS切り替え
  if (m_FpsMode == FPS_MODE_15)
  {
    m_FpsMode = FPS_MODE_30;
  }
  else
  {
    m_FpsMode = FPS_MODE_15;
  }

  // 再生速度変更
  if (m_pCharaModel->GetAnimationStepFrame() > 0.0f)
  {
    m_pCharaModel->SetAnimationStepFrame(this->GetStepFrame());

    if (m_pSubObjectModel != NULL)
    {
      m_pSubObjectModel->SetAnimationStepFrame(this->GetStepFrame());
    }
  }
}

s32 CharaViewerCharaModelManager_Animation::GetStepFrame() const
{
  if (m_FpsMode == FPS_MODE_15)
  {
    return 2;
  }

  return 1;
}

void CharaViewerCharaModelManager_Animation::InitMotionList()
{
  for (u32 i = 0; i < MOTION_LIST_COUNT; ++i)
  {
    m_MotionUnitList[i].Initialize();
  }

  m_MotionUnitList[0].ToggleEnabled();
  m_MotionUnitList[0].SetMotionIndex(0, -1);
}

void CharaViewerCharaModelManager_Animation::PlayStart()
{
  // モーションリストがセットされていない
  if (m_pMotionIdList == NULL)
  {
    return;
  }

  // 有効になっているモーションの先頭から再生
  m_CurrentMotionIndex = 0;
  for (u32 i = 0; i < MOTION_LIST_COUNT; ++i)
  {
    m_MotionUnitList[i].SetCurrentFrame(0);
  }
  for (u32 i = 0; i < MOTION_LIST_COUNT; ++i)
  {
    if (m_MotionUnitList[i].IsEnabled())
    {
      m_CurrentMotionIndex = i;
      break;
    }
  }

  // リストインデックスが無効
  s32 listIndex = m_MotionUnitList[m_CurrentMotionIndex].GetListIndex();
  if (listIndex < 0)
  {
    return;
  }

  // モーションインデックスが無効
  s32 motionIndex = m_MotionUnitList[m_CurrentMotionIndex].GetMotionIndex();
  if (motionIndex < 0)
  {
    return;
  }

  // モーション切り替え
  s32 motionId = m_pMotionIdList[listIndex]->GetData(m_pSortedMotionIdList[listIndex][motionIndex].motionId)->id;
  this->ChangeAnimation(motionId);
  m_PlayState = PLAY_STATE_PLAY;

  // トータルフレーム数設定
  u32 totalFrame = m_pCharaModel->GetAnimationEndFrame();
  m_MotionUnitList[m_CurrentMotionIndex].SetTotalFrame(totalFrame);
}

void CharaViewerCharaModelManager_Animation::ChangeAnimation(s32 motionId, bool isSmooth)
{
  if (m_pCharaModel == NULL)
  {
    return;
  }

  if (motionId < 0)
  {
    m_pCharaModel->ChangeAnimationByResourceNode(NULL);
    if (m_pSubObjectModel != NULL)
    {
      m_pSubObjectModel->ChangeAnimationByResourceNode(NULL);
    }
    return;
  }

  // 同じアニメーションは補間しない
  if (m_pCharaModel->GetAnimationId() == motionId)
  {
    isSmooth = false;
  }

  if (isSmooth)
  {
    m_pCharaModel->ChangeAnimation(motionId);
  }
  else
  {
    m_pCharaModel->ChangeAnimationByResourceNode(NULL);
    m_pCharaModel->ChangeAnimationSmooth(motionId, 0);
  }
  
  // ループアニメは最後のフレームを再生しない
  bool adjustEndFrame = m_pCharaModel->IsAnimationLoop();

  if (adjustEndFrame)
  {
    m_pCharaModel->SetAnimationEndFrame(m_pCharaModel->GetAnimationEndFrame() - 1);
  }

  m_pCharaModel->SetAnimationStepFrame(this->GetStepFrame());
  m_pCharaModel->SetAnimationLoop(false);

  if (m_pSubObjectModel != NULL)
  {
    if (isSmooth)
    {
      m_pSubObjectModel->ChangeAnimation(motionId);
    }
    else
    {
      m_pSubObjectModel->ChangeAnimationSmooth(motionId, 0);
    }
    m_pSubObjectModel->SetAnimationStepFrame(this->GetStepFrame());
    m_pSubObjectModel->SetAnimationLoop(false);
    m_pSubObjectModel->SetAnimationEndFrame(m_pSubObjectModel->GetAnimationEndFrame());
  }
}

s32 CharaViewerCharaModelManager_Animation::GetCurrentAnimationId() const
{
  // リストインデックスが無効
  s32 listIndex = m_MotionUnitList[m_CurrentMotionIndex].GetListIndex();
  if (listIndex < 0)
  {
    return 0;
  }

  // モーションインデックスが無効
  s32 motionIndex = m_MotionUnitList[m_CurrentMotionIndex].GetMotionIndex();
  if (motionIndex < 0)
  {
    return 0;
  }

  s32 motionId = m_pMotionIdList[listIndex]->GetData(m_pSortedMotionIdList[listIndex][motionIndex].motionId)->id;
  return motionId;
}

const gfl2::math::Vector3& CharaViewerCharaModelManager_Animation::GetWalkPosition() const
{
  return m_WalkPosition;
}

GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
