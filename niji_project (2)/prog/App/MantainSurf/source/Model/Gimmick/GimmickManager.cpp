//======================================================================
/**
 * @file GimmickManager.cpp
 * @date 2016/09/13 12:20:41
 * @author fang_yicheng
 * @brief 障害物マネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "GimmickManager.h"
#include "../../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32 CREATE_BLOCK_OFFSET = 5.0f;     // 作成開始のブロックオフセット
const f32 DEAD_BLOCK_OFFSET   = -0.5f;    // 消滅時のブロックオフセット
const u32 MAX_BLOCK_GMK_NUM   = 5;        // 1ブロックの障害物最大数

const u32 HOERUOH_BLOCK_HISTORY_MAX_SIZE = 5;        // ホエルオー出現ブロック履歴の最大サイズ
const f32 GIMMICK_DRAWABLE_CHECK_SIZE    = 150.0f;   // 障害物描画可能範囲のチェック用サイズ
const f32 UNVISIBLE_DELAY_TIME           = 15;       // 障害物が不可視になる際の遅延フレーム

GimmickManager::GimmickManager() :
  m_pGfxAllocator(NULL),
  m_pHeap(NULL),
  m_pRenderingPipeLine(NULL),
  m_pAppManager(NULL),
  m_pCollisionManager(NULL),
  m_pResourceManager(NULL),
  m_pEffectManager(NULL),
  m_pCourseDataManager(NULL),
  m_pNamiController(NULL),
  m_pPlayerController(NULL),
  m_pPlayerModelSet(NULL),
  m_ppGimmickList(NULL),
  m_maxGimmickSize(0),
  m_curBlockCount(0.0f),
  m_blockNumUpDown(0.0f),
  m_blockNumArrived(0.0f),
  m_curFixedBlockCount(0.0f),
  m_totalResetLength(0.0f),
  m_isPlayerArrived(true),
  m_tutorialEndPosZ(0.0f)
{
#if PM_DEBUG
  m_isShowGimmickAll = true;
  for(u32 i = 0; i < GimmickBase::TYPE_MAX; i++)
  {
    m_isShowGimmick[i] = (i != GimmickBase::TYPE_COMMON);
  }
#endif

}

GimmickManager::~GimmickManager()
{
}

void GimmickManager::Initialize(SetupData &setupData)
{
  m_pGfxAllocator = setupData.m_pGfxAllocator;
  m_pHeap = setupData.m_pHeap;
  m_pRenderingPipeLine = setupData.m_pRenderingPipeLine;
  m_pAppManager = setupData.m_pAppManager;
  m_pCollisionManager = setupData.m_pCollisionManager;
  m_pResourceManager = setupData.m_pResourceManager;
  m_pCourseDataManager = setupData.m_pCourseDataManager;
  m_pEffectManager = setupData.m_pEffectManager;
  m_pNamiController = setupData.m_pNamiController;
  m_pPlayerController = setupData.m_pPlayerController;
  m_pPlayerModelSet = setupData.m_pPlayerModelSet;
  m_BlockDataWork.m_blockDataUnitArray = GFL_NEW_ARRAY(m_pHeap) Course::CourseData::BlockDataUnit[MAX_BLOCK_GMK_NUM];
  m_HoeruOhBlockHistoryList.CreateBuffer(m_pHeap, HOERUOH_BLOCK_HISTORY_MAX_SIZE);
  InitializeGimmickList();
}

void GimmickManager::Terminate()
{
  if(m_ppGimmickList != NULL)
  {
    for(u32 i = 0; i < m_maxGimmickSize; i++)
    {
      m_ppGimmickList[i]->Terminate();
      GFL_SAFE_DELETE(m_ppGimmickList[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_ppGimmickList);
  }
  m_HoeruOhBlockHistoryList.Clear();
}

void GimmickManager::InitializeGimmickList()
{
  // 障害物の最大数を計算
  Course::CourseData* pCourseData = m_pCourseDataManager->GetCourseData();
  for(u32 type = 0; type < BLOCK_TYPE_MAX; type++)
  {
    m_blockStartIndex[type] = m_maxGimmickSize;
    u32 gmkNum = 0;
    for(u32 gmkId = 0; gmkId < Course::CourseData::GMKID_MAX; gmkId++)
    {
      m_gimmickStartIndex[type][gmkId] = m_maxGimmickSize;
      if(type != BLOCK_TYPE_FIXED)
      {
        m_gimmickSizeList[type][gmkId] = pCourseData->GetMaxGimmickNum(static_cast<Course::CourseData::BlockDataType>(type), static_cast<Course::CourseData::GimmickId>(gmkId));
      }
      else
      {
        m_gimmickSizeList[type][gmkId] = pCourseData->GetMaxFixedGimmickNum(static_cast<Course::CourseData::GimmickId>(gmkId));
      }
      m_maxGimmickSize += m_gimmickSizeList[type][gmkId];
      gmkNum += m_gimmickSizeList[type][gmkId];
    }
    m_blockGimmickSize[type] = gmkNum;
  }

#if PM_DEBUG
  HOU_PRINT("[InitializeGimmickList]GimmickMaxNum = %d\r\n", m_maxGimmickSize);
  // デバッグ用に全種類の障害物を追加
  m_maxGimmickSize += GimmickBase::TYPE_MAX - 1;
#endif

  if(m_maxGimmickSize == 0)
  {
    return;
  }

  // 障害物を生成する
  m_ppGimmickList = GFL_NEW_ARRAY(m_pHeap) GimmickBase*[m_maxGimmickSize];
  for(u32 type = 0; type < BLOCK_TYPE_MAX; type++)
  {
    for(u32 gmkId = 0; gmkId < Course::CourseData::GMKID_MAX; gmkId++)
    {
      for(u32 i = 0; i < m_gimmickSizeList[type][gmkId]; i++)
      {
        u32 index = m_gimmickStartIndex[type][gmkId] + i;
        GimmickBase* pGimmick = NULL;
        switch(gmkId)
        {
        case Course::CourseData::GMKID_MENOKURAGE:
          pGimmick = GFL_NEW(m_pHeap) GimmickMenokurage;
          break;
        case Course::CourseData::GMKID_HOERUKO:
          pGimmick = GFL_NEW(m_pHeap) GimmickHoeruko;
          break;
        case Course::CourseData::GMKID_SAMEHADER:
          pGimmick = GFL_NEW(m_pHeap) GimmickSamehader;
          break;
        case Course::CourseData::GMKID_HOERUOH:
          pGimmick = GFL_NEW(m_pHeap) GimmickHoeruoh;
          break;
        case Course::CourseData::GMKID_REEF_S:
          pGimmick = GFL_NEW(m_pHeap) GimmickReefSmall;
          break;
        case Course::CourseData::GMKID_REEF_M:
          pGimmick = GFL_NEW(m_pHeap) GimmickReefMedium;
          break;
        case Course::CourseData::GMKID_REEF_L:
          pGimmick = GFL_NEW(m_pHeap) GimmickReefLarge;
          break;
        case Course::CourseData::GMKID_COMMON:
          pGimmick = GFL_NEW(m_pHeap) GimmickCommon;
          break;
        }
        pGimmick->Initialize(m_pGfxAllocator, m_pHeap, m_pResourceManager, m_pEffectManager);
        pGimmick->SetController(m_pNamiController, m_pPlayerController);
        pGimmick->SetCamera(m_pAppManager->GetCamera());
        pGimmick->AddToRenderingPipeLine(m_pRenderingPipeLine, true);
        pGimmick->SetDead();
        m_pCollisionManager->AddTargetModel(pGimmick);
        m_ppGimmickList[index] = pGimmick;
      }
    }
  }

#if PM_DEBUG
  // デバッグ用障害物を生成
  for(u32 i = 0; i < GimmickBase::TYPE_MAX; i++)
  {
    if(i == GimmickBase::TYPE_COMMON)
    {
      continue;
    }
    GimmickBase* pGimmick = NULL;
    switch(i)
    {
    case GimmickBase::TYPE_COMMON:
      pGimmick = GFL_NEW(m_pHeap) GimmickCommon;
      break;
    case GimmickBase::TYPE_REEF_S:
      pGimmick = GFL_NEW(m_pHeap) GimmickReefSmall;
      break;
    case GimmickBase::TYPE_REEF_M:
      pGimmick = GFL_NEW(m_pHeap) GimmickReefMedium;
      break;
    case GimmickBase::TYPE_REEF_L:
      pGimmick = GFL_NEW(m_pHeap) GimmickReefLarge;
      break;
    case GimmickBase::TYPE_MENOKURAGE:
      pGimmick = GFL_NEW(m_pHeap) GimmickMenokurage;
      break;
    case GimmickBase::TYPE_HOERUKO:
      pGimmick = GFL_NEW(m_pHeap) GimmickHoeruko;
      break;
    case GimmickBase::TYPE_SAMEHADER:
      pGimmick = GFL_NEW(m_pHeap) GimmickSamehader;
      break;
    case GimmickBase::TYPE_HOERUOH:
      pGimmick = GFL_NEW(m_pHeap) GimmickHoeruoh;
      break;
    }
    pGimmick->Initialize(m_pGfxAllocator, m_pHeap, m_pResourceManager, m_pEffectManager);
    pGimmick->SetController(m_pNamiController, m_pPlayerController);
    pGimmick->SetCamera(m_pAppManager->GetCamera());
    pGimmick->AddToRenderingPipeLine(m_pRenderingPipeLine, true);
    pGimmick->SetDead();
    m_pCollisionManager->AddTargetModel(pGimmick);
    m_ppGimmickList[m_maxGimmickSize - GimmickBase::TYPE_MAX + i] = pGimmick;
  }
#endif
}

void GimmickManager::Update()
{
  UpdateInput();
  CheckGimmickLifeCycle();
  UpdateGimmick();
  UpdateGimmickDraw();
}

//-----------------------------------------
// 障害物の描画要否を更新
//-----------------------------------------
void GimmickManager::UpdateGimmickDraw()
{
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    if(m_ppGimmickList[i]->IsDead()) continue;
    bool isVisible = true;
    u32 delay = 0;
#if PM_DEBUG
    isVisible &= m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmickAll && m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmick[m_ppGimmickList[i]->GetType()];
#endif
    if(m_ppGimmickList[i]->GetType() == GimmickBase::TYPE_HOERUOH)
    {
      // ホエルオーの描画チェック
      f32 len;
      GimmickHoeruoh* pHoeruoh = static_cast<GimmickHoeruoh*>(m_ppGimmickList[i]);
      isVisible &= !CheckCollisionHoeruohCamera(pHoeruoh, len);
      if(!pHoeruoh->IsVisible() && len < 0.0f)
      {
        // 表示/非表示のちらつきを防ぐために、非表示したあとカメラ至近距離内も非表示し続ける
        isVisible = false;
      }
    }
    else
    {
      // その他障害物の描画チェック
      isVisible &= CheckDrawEnable(m_ppGimmickList[i]);
      delay = isVisible ? 0 : UNVISIBLE_DELAY_TIME;
    }
    m_ppGimmickList[i]->SetDrawEnable(isVisible, delay);
  }
}

bool GimmickManager::CheckDrawEnable(GimmickBase *pGimmick)
{
  Camera::Camera::ViewPlane plane = m_pAppManager->GetCamera()->GetNearViewPlane();
  gfl2::math::Vector3 bodyPos = pGimmick->GetBodyPosition();
  f32 lenToPlane = (bodyPos - plane.center).Dot(plane.normal);
  if(lenToPlane < -GIMMICK_DRAWABLE_CHECK_SIZE)
  {
    // カメラの背面に居る
    return false;
  }

  // カメラ正面に居るため、画面上に映しているかどうかをチェック
  gfl2::math::Vector3 bodyOffset = plane.center + plane.normal * lenToPlane - bodyPos;
  gfl2::math::Vector3 checkPos = bodyPos;
  if(bodyOffset.Length() > 0.0f)
  {
    checkPos += bodyOffset.Normalize() * GIMMICK_DRAWABLE_CHECK_SIZE;
  }
  gfl2::math::Vector2 screenPos = m_pAppManager->GetCamera()->ConvWorldPosToScreenPos(checkPos);
  if(screenPos.x >= 0.0f && screenPos.x <= 1.0f && screenPos.y >= 0.0f && screenPos.y <= 1.0f)
  {
    // 画面上に映している
    return true;
  }
  return false;
}

//-----------------------------------------
// ホエルオーとカメラの当たり判定
//-----------------------------------------
bool GimmickManager::CheckCollisionHoeruohCamera(GimmickHoeruoh* pHoeruoh, f32 &out_lenCameraToHoeruoh)
{
  f32 radius = pHoeruoh->GetBodyRadius();                     // ホエルオー胴体の半径
  gfl2::math::Vector3 posHead = pHoeruoh->GetHeadPosition();  // ホエルオーの頭の位置
  gfl2::math::Vector3 posHip  = pHoeruoh->GetHipPosition();   // ホエルオーの尻の位置

  // カメラのニアクリップ平面
  Camera::Camera::ViewPlane plane = m_pAppManager->GetCamera()->GetNearViewPlane();

  // 頭情報
  f32 lenHeadToPlane = (posHead - plane.center).Dot(plane.normal);          // 頭とニアクリップ平面の垂直距離
  gfl2::math::Vector3 vecHeadToPlane = -plane.normal * lenHeadToPlane;      // 頭とニアクリップ平面の距離ベクトル
  gfl2::math::Vector3 posHeadOnPlane = posHead - vecHeadToPlane;            // 頭がニアクリップ平面上の投影位置
  bool isHeadInSidePlane = IsPointInsidePlane(plane, posHeadOnPlane);

  // 尻情報
  f32 lenHipToPlane = (posHip - plane.center).Dot(plane.normal);            // 尻とニアクリップ平面の垂直距離
  gfl2::math::Vector3 vecHipToPlane = -plane.normal * lenHipToPlane;        // 尻とニアクリップ平面の距離ベクトル
  gfl2::math::Vector3 posHipOnPlane = posHead - vecHipToPlane;              // 尻がニアクリップ平面上の投影位置
  bool isHipInSidePlane = IsPointInsidePlane(plane, posHipOnPlane);

  out_lenCameraToHoeruoh = gfl2::math::Min(lenHeadToPlane, lenHipToPlane);
  if(out_lenCameraToHoeruoh < -radius * 2.0f)
  {
    // カメラ背面に居るため表示不要
    HOU_PRINT("[GimmickManager]カメラ背面[%f.2]\r\n", out_lenCameraToHoeruoh);
    return true;
  }
  
  // 外側からの距離チェック
  if(!isHeadInSidePlane && !isHipInSidePlane)
  {
    const gfl2::math::Vector3 POINT[] = { posHead, posHip };
    const gfl2::math::Vector3 LINE_START_VEC[] = { plane.vertex_left_up, plane.vertex_right_up, plane.vertex_right_down, plane.vertex_left_down };
    const gfl2::math::Vector3 LINE_END_VEC[]   = { plane.vertex_right_up, plane.vertex_right_down, plane.vertex_left_down, plane.vertex_left_up };
    const f32 POINT_SIZE = GFL_NELEMS(POINT);
    const f32 VEC_SIZE   = GFL_NELEMS(LINE_START_VEC);
    // 点(頭/尻)から平面4辺の距離を求める
    for(u32 pointIndex = 0; pointIndex < POINT_SIZE; pointIndex++)
    {
      for(u32 vecIndex = 0; vecIndex < VEC_SIZE; vecIndex++)
      {
        f32 len = CalcLenPointToLine(POINT[pointIndex], LINE_START_VEC[vecIndex], LINE_END_VEC[vecIndex]);
        if(out_lenCameraToHoeruoh > len)
        {
          out_lenCameraToHoeruoh = len;
        }
        if(len <= radius)
        {
          // 頭/尻の外側が平面と衝突している
          HOU_PRINT("[GimmickManager]%s外側衝突[%f.2]\r\n", pointIndex == 0 ? "頭" : "尻", len);
          return true;
        }
      }
    }
    return false;
  }

  // 内側チェック
  if(vecHeadToPlane.Dot(vecHipToPlane) < 0.0f)
  {
    // ホエルオーがカメラのニアクリップ平面と交差している
    HOU_PRINT("[GimmickManager]交差衝突[%f.2]\r\n", 0.0f);
    return true;
  }
  else if(lenHeadToPlane <= radius)
  {
    // ホエルオーの頭がカメラのニアクリップ平面と当たっている
    HOU_PRINT("[GimmickManager]頭衝突[%f.2]\r\n", lenHeadToPlane);
    return true;
  }
  else if(lenHipToPlane <= radius)
  {
    // ホエルオーの尻がカメラのニアクリップ平面と当たっている
    HOU_PRINT("[GimmickManager]尻衝突[%f.2]\r\n", lenHipToPlane);
    return true;
  }

  // ホエルオーがカメラと衝突していない
  return false;
}

//-----------------------------------------
// 平面上の点が平面の範囲内に居るかどうか
//-----------------------------------------
bool GimmickManager::IsPointInsidePlane(const Camera::Camera::ViewPlane &plane, const gfl2::math::Vector3 &pointOnPlane)
{
  gfl2::math::Vector3 vecCheck = pointOnPlane - plane.center;
  // 左右側チェック
  {
    f32 len = vecCheck.Dot(plane.right);
    if(gfl2::math::FAbs(len) > plane.width / 2.0f)
    {
      // 点が平面左右の外側に居る
      return false;
    }
  }
  // 上下側チェック
  {
    f32 len = vecCheck.Dot(plane.up);
    if(gfl2::math::FAbs(len) > plane.height / 2.0f)
    {
      // 点が平面上下の外側に居る
      return false;
    }
  }
  // 点が平面の内側に居る
  return true;
}

//-----------------------------------------
// 点から線分の距離を求める
//-----------------------------------------
f32 GimmickManager::CalcLenPointToLine(const gfl2::math::Vector3 &point, const gfl2::math::Vector3 &lineStart, const gfl2::math::Vector3 &lineEnd)
{
  gfl2::math::Vector3 lineVec = lineEnd - lineStart;  // 線分ベクトル
  gfl2::math::Vector3 lineDir = lineVec.Normalize();  // 線分の方向
  gfl2::math::Vector3 pointVec = point - lineStart;   // 線分のスタートから点までのベクトル
  f32 p = pointVec.Dot(lineDir);
  if(p < 0.0f)
  {
    return pointVec.Length();
  }
  if(p > pointVec.Length())
  {
    return (point - lineEnd).Length();
  }
  return (lineStart + lineDir * p - point).Length();
}

void GimmickManager::UpdateTimeZone(s32 timeZone)
{
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    m_ppGimmickList[i]->UpdateTimeZone(timeZone);
  }
}

void GimmickManager::UpdateAfterTraverse()
{
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    m_ppGimmickList[i]->UpdateAfterTraverse();
  }
}

void GimmickManager::MakeGimmickForTutorial()
{
  Course::CourseData* pCourseData = m_pCourseDataManager->GetCourseData();

  // 障害物成功開始位置を計算
  m_totalResetLength = 0.0f;
  f32 startPosZ = m_pPlayerModelSet->GetPosition().z + pCourseData->GetHeader()->m_blockLengthZ;
  s32 startPosX = Course::CourseData::MAX_BLOCK_POS_X - 1;

  // 障害物を生成
  pCourseData->GetNextFixedBlockData(&m_BlockDataWork, MAX_BLOCK_GMK_NUM);
  GFL_ASSERT(m_BlockDataWork.m_blockDataUnitArraySize >= 1);
  for(s32 i = 0; i < MAX_BLOCK_GMK_NUM; i++)
  {
    // 障害物位置を修正
    if(i % 2 == 0)
    {
      m_BlockDataWork.m_blockDataUnitArray[0].m_posX = startPosX + i;
    }
    else
    {
      m_BlockDataWork.m_blockDataUnitArray[0].m_posX = startPosX - (i + 1);
    }
    CreateGimmick(BLOCK_TYPE_FIXED, startPosZ, &m_BlockDataWork);
    startPosZ += pCourseData->GetHeader()->m_blockLengthZ;
  }
  m_tutorialEndPosZ = startPosZ;
}

void GimmickManager::ClearGimmickForTutorial()
{
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    m_ppGimmickList[i]->SetDead();
  }
}

bool GimmickManager::IsReachTutorialEndPosition()
{
  return m_pPlayerModelSet->GetPosition().z + m_totalResetLength >= m_tutorialEndPosZ;
}

void GimmickManager::UpdateInput()
{
#if PM_DEBUG
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  bool isToggleGimmick = pButton->IsTrigger(gfl2::ui::BUTTON_START) && m_pAppManager->GetDebugMenu()->m_dbvIsHotKeyEnable;
  if(isToggleGimmick)
  {
    // 障害物有無の切り替え
    m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmickAll = !m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmickAll;
  }
#endif
}

#if PM_DEBUG
void GimmickManager::MakeGimmickForDebug()
{
  // デバッグ用障害物を生成するため、一旦すべての障害物を非表示
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    m_ppGimmickList[i]->SetVisible(false);
  }

  // デバッグ用障害物の生成
  m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmickAll = true;
  m_isShowGimmickAll = true;
  u32 count = 0;
  for(u32 type = 0; type < GimmickBase::TYPE_MAX; type++)
  {
    gfl2::math::Vector3 position = m_pPlayerModelSet->GetPosition();
    if(!m_isShowGimmick[type])
    {
      continue;
    }
    count++;
    GimmickBase* pGimmick = m_ppGimmickList[m_maxGimmickSize - GimmickBase::TYPE_MAX + type];
    if(type == GimmickBase::TYPE_HOERUOH)
    {
      position.x = m_pNamiController->GetNamiTopMaxPosition().x;
    }
    position.y = 0.0f;
    position.z += 600.0f * count;
    pGimmick->SetDead();
    pGimmick->Reset(position);
  }
}

void GimmickManager::SetState(const GimmickBase::State state)
{
  gfl2::math::Vector3 posPlayer = m_pPlayerModelSet->GetPosition();
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    gfl2::math::Vector3 posGimmick = m_ppGimmickList[i]->GetPosition();
    if(m_ppGimmickList[i]->IsVisible() && (posPlayer - posGimmick).Length() < 5000.0f)
    {
      m_ppGimmickList[i]->SetState(state);
    }
  }
}

void GimmickManager::Reset()
{
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    if(!m_ppGimmickList[i]->IsDead())
    {
      gfl2::math::Vector3 pos = m_ppGimmickList[i]->GetInitPosition();
      m_ppGimmickList[i]->SetDead();
      m_ppGimmickList[i]->Reset(pos);
      bool isVisible = m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmickAll && m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmick[m_ppGimmickList[i]->GetType()];
      m_ppGimmickList[i]->SetVisible(isVisible);
    }
  }
}
#endif

// 障害物の死活監視
void GimmickManager::CheckGimmickLifeCycle()
{
  if(m_pAppManager->GetGameState() != MantainSurfAppManager::ST_SURFING)
  {
    return;
  }

  Course::CourseData* pCourseData = m_pCourseDataManager->GetCourseData();

  f32 blockNumNow = CalcBlockNum(m_pPlayerModelSet->GetPosition().z + m_totalResetLength);  // プレイヤーが現在居るブロック番号
  f32 createBlockNum = blockNumNow + CREATE_BLOCK_OFFSET;                                   // 障害物作成位置のブロック番号
  f32 deadBlockNum = blockNumNow + DEAD_BLOCK_OFFSET;                                       // 障害物消滅位置のブロック番号

  f32 posStartBlock = CalcBlockNum(m_pPlayerModelSet->GetPosition().z);
  f32 startPosZ = (posStartBlock + CREATE_BLOCK_OFFSET) * pCourseData->GetHeader()->m_blockLengthZ;
  f32 deadPosZ = (posStartBlock + DEAD_BLOCK_OFFSET) * pCourseData->GetHeader()->m_blockLengthZ;

  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    if(m_ppGimmickList[i]->IsDead()) continue;
    // 死活状態更新
    if(m_ppGimmickList[i]->GetPosition().z <= deadPosZ && m_ppGimmickList[i]->GetBodyPosition().z <= deadPosZ)
    {
      // @fix GFMMCat[159] オリジンとジョイント両方の座標をチェックすることでアクション中に突然消える(Dead)ことを防ぐ
      m_ppGimmickList[i]->SetDead();
    }
  }

  // 波上り/波下り/ランダム障害物
  {
    Course::CourseData::BlockDataType type = Course::CourseData::BLOCK_DATA_TYPE_NONE;
    if(m_pNamiController->IsGoingUp() || m_pNamiController->IsGoingDown())
    {
      // 波上り/上りタイミング
      if(m_curBlockCount >= createBlockNum)
      {
        // 既に障害物ブロック存在する場合は1個ずらす
        createBlockNum = m_curBlockCount + 1;
        startPosZ = (posStartBlock + CREATE_BLOCK_OFFSET + 1) * pCourseData->GetHeader()->m_blockLengthZ;
      }
      m_blockNumUpDown = createBlockNum;
      type = m_pNamiController->IsGoingUp() ? Course::CourseData::BLOCK_DATA_TYPE_UPWARD : Course::CourseData::BLOCK_DATA_TYPE_DOWNWARD;
    }
    else if(m_curBlockCount < createBlockNum && !m_pNamiController->IsLastWave())
    {
      // 該当位置にブロック未作成のため、障害物を作成する
      type = Course::CourseData::BLOCK_DATA_TYPE_RANDOM;
    }

    if(type != Course::CourseData::BLOCK_DATA_TYPE_NONE)
    {
      m_curBlockCount = createBlockNum;
      pCourseData->GetNextBlockData(type, &m_BlockDataWork, MAX_BLOCK_GMK_NUM);
      CreateGimmick(static_cast<BlockType>(type), startPosZ, &m_BlockDataWork);
      if(HasHoeruoh(&m_BlockDataWork))
      {
        // ホエルオーが存在している場合は履歴に追加
        if(m_HoeruOhBlockHistoryList.Size() >= m_HoeruOhBlockHistoryList.MaxSize())
        {
          // 最古の記録を削除
          m_HoeruOhBlockHistoryList.Remove(*m_HoeruOhBlockHistoryList.Begin());
        }
        m_HoeruOhBlockHistoryList.PushBack(m_curBlockCount);
      }
    }
  }

  // 固定障害物
  {
    if(m_curFixedBlockCount < createBlockNum && !m_pNamiController->IsLastWave() && !IsHitHoeruohBlockHistory(createBlockNum))
    {
      m_curFixedBlockCount = createBlockNum;
      pCourseData->GetNextFixedBlockData(&m_BlockDataWork, MAX_BLOCK_GMK_NUM);
      CreateGimmick(BLOCK_TYPE_FIXED, startPosZ, &m_BlockDataWork);
    }
  }

  // プレイヤー到着の更新
  {
    m_isPlayerArrived = true;
    f32 blockOffset = 1.0f + m_pPlayerController->GetSpeedRate();
    if(m_blockNumArrived < blockNumNow)
    {
      m_blockNumArrived = blockNumNow;
    }
    if(m_blockNumArrived < m_blockNumUpDown - blockOffset)
    {
      // プレイヤーが障害物発生ブロックに未到着
      m_isPlayerArrived = false;
    }
  }
}

bool GimmickManager::IsHitHoeruohBlockHistory(const u32 blockNum)
{
  gfl2::util::ListIterator<u32> it = m_HoeruOhBlockHistoryList.Begin();
  for(; it != m_HoeruOhBlockHistoryList.End(); it++)
  {
    if(blockNum == *it)
    {
      return true;
    }
  }
  return false;
}

void GimmickManager::CreateGimmick(BlockType type, f32 startPosZ, const Course::CourseData::BlockDataOut* pBlockData)
{
  Course::CourseData* pCourseData = m_pCourseDataManager->GetCourseData();
  f32 unitLengthX = static_cast<f32>(pCourseData->GetHeader()->m_blockLengthX) / static_cast<f32>(Course::CourseData::MAX_BLOCK_POS_X);
  f32 unitLengthZ = static_cast<f32>(pCourseData->GetHeader()->m_blockLengthZ) / static_cast<f32>(Course::CourseData::MAX_BLOCK_POS_Z);
  for(u32 i = 0; i < pBlockData->m_blockDataUnitArraySize; i++)
  {
    Course::CourseData::BlockDataUnit *pUnit = &pBlockData->m_blockDataUnitArray[i];
    HOU_PRINT("[CreateGimmick%d]PatternId=%d, GmkId=%d, PosX=%d, PosZ=%d\r\n", type, pUnit->m_patternId, pUnit->m_gmkId, pUnit->m_posX, pUnit->m_posZ);
    GimmickBase* pGimmick = FindUseableGimmick(type, static_cast<Course::CourseData::GimmickId>(pUnit->m_gmkId));
    if(pGimmick == NULL)
    {
      // 使える障害物が見つからない
      HOU_PRINT("[CreateGimmick]Failed to find useable gimmick instance.[GmkId=%d]\r\n", pUnit->m_gmkId);
      continue;
    }
    gfl2::math::Vector3 position = gfl2::math::Vector3::GetZero();
    position.x = GetStartPosX(type, static_cast<Course::CourseData::GimmickId>(pUnit->m_gmkId)) - pUnit->m_posX * unitLengthX;
    position.z = startPosZ + pUnit->m_posZ * unitLengthZ;
    pGimmick->Reset(position);
    // 描画要否をチェック
    bool isVisible = true;
#if PM_DEBUG
    isVisible = m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmickAll && m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmick[pGimmick->GetType()];
#endif
    isVisible &= CheckDrawEnable(pGimmick);
    pGimmick->SetDrawEnable(isVisible, 0);
  }
}

bool GimmickManager::HasHoeruoh(const Course::CourseData::BlockDataOut* pBlockData)
{
  for(u32 i = 0; i < pBlockData->m_blockDataUnitArraySize; i++)
  {
    Course::CourseData::BlockDataUnit *pUnit = &pBlockData->m_blockDataUnitArray[i];
    if(pUnit->m_gmkId == Course::CourseData::GMKID_HOERUOH)
    {
      return true;
    }
  }
  return false;
}

f32 GimmickManager::GetStartPosX(BlockType type, Course::CourseData::GimmickId gmkId)
{
  if(gmkId == Course::CourseData::GMKID_HOERUOH)
  {
    // ホエルオーの出現位置は固定
    return m_pNamiController->GetNamiTopMaxPosition().x;
  }

  switch(type)
  {
  case BLOCK_TYPE_UPWARD:
  case BLOCK_TYPE_DOWNWARD:
  case BLOCK_TYPE_RANDOM:
    return m_pNamiController->GetNamiPointInfo(Nami::NamiController::ZONE_UP_START).m_point.x - 50.0f;
  case BLOCK_TYPE_FIXED:
    return m_pNamiController->GetNamiPointInfo(Nami::NamiController::ZONE_LEFT_END).m_point.x - 250.0f;
  default:
    return m_pPlayerModelSet->GetPosition().x;
  }
}

GimmickBase* GimmickManager::FindUseableGimmick(BlockType type, Course::CourseData::GimmickId gmkId)
{
  u32 startIndex = m_gimmickStartIndex[type][gmkId];
  u32 size = m_gimmickSizeList[type][gmkId];
  for(u32 i = startIndex; i < startIndex + size; i++)
  {
    GimmickBase* pGimmick = m_ppGimmickList[i];
    if(pGimmick->IsDead())
    {
      return pGimmick;
    }
  }
  return NULL;
}

void GimmickManager::UpdateGimmick()
{
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    m_ppGimmickList[i]->Update();
  }

#if PM_DEBUG
  // 障害物表示の切り替え
  if(m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmickAll != m_isShowGimmickAll)
  {
    // 全体表示の切り替え
    m_isShowGimmickAll = m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmickAll;
    for(u32 i = 0; i < m_maxGimmickSize; i++)
    {
      if(!m_ppGimmickList[i]->IsDead()) m_ppGimmickList[i]->SetVisible(m_isShowGimmickAll && m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmick[m_ppGimmickList[i]->GetType()]);
    }
  }
  else
  {
    // 個別表示の切り替え
    s32 type = -1;
    for(u32 i = 0; i < GimmickBase::TYPE_MAX; i++)
    {
      if(m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmick[i] != m_isShowGimmick[i])
      {
        m_isShowGimmick[i] = m_pAppManager->GetDebugMenu()->m_dbvIsShowGimmick[i];
        type = i;
        break;
      }
    }
    if(type >= 0)
    {
      for(u32 i = 0; i < m_maxGimmickSize; i++)
      {
        if(m_ppGimmickList[i]->GetType() == type && !m_ppGimmickList[i]->IsDead())
        {
          m_ppGimmickList[i]->SetVisible(m_isShowGimmickAll && m_isShowGimmick[type]);
        } 
      }
    }
  }
#endif
}

void GimmickManager::ResetZPosition(const f32 resetLength)
{
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    m_ppGimmickList[i]->ResetZPosition(resetLength);
#if PM_DEBUG
    // デバッグ機能用に初期位置を更新
    gfl2::math::Vector3 initPos = m_ppGimmickList[i]->GetInitPosition();
    initPos.z -= resetLength;
    m_ppGimmickList[i]->SetInitPosition(initPos);
#endif
  }
  m_totalResetLength += resetLength;
}

bool GimmickManager::IsPlayerArrived()
{
  return m_isPlayerArrived;
}

void GimmickManager::StopEffect()
{
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    m_ppGimmickList[i]->StopEffect();
  }
}

void GimmickManager::ResumeEffect()
{
  for(u32 i = 0; i < m_maxGimmickSize; i++)
  {
    m_ppGimmickList[i]->ResumeEffect();
  }
}

f32 GimmickManager::CalcBlockNum(f32 posZ)
{
  Course::CourseData* pCourseData = m_pCourseDataManager->GetCourseData();
  f32 blockLengthZ = static_cast<f32>(pCourseData->GetHeader()->m_blockLengthZ);
  s32 blockNum = static_cast<s32>(posZ / blockLengthZ);
  if(blockNum < 0)
  {
    blockNum--;
  }
  return static_cast<f32>(blockNum);
}

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
