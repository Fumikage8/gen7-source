#include <Animation/include/gfl2_DrawEnvAnimationController.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>

using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

DrawEnvAnimationController::DrawEnvAnimationController() :
  m_pHeap(NULL),
  m_pDrawEnvNode(NULL)
{
}

DrawEnvAnimationController::~DrawEnvAnimationController()
{
}

void DrawEnvAnimationController::Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  m_pHeap = pHeap;
  m_pDrawEnvNode = pDrawEnvNode;

  // ライトスロットセット初期化
  m_LightSlotSet.Initialize();

  // カメラスロットセット初期化
  m_CameraSlotSet.Initialize();

  //kawa20150514
  // フォグスロットセット初期化
  m_FogSlotSet.Initialize();

  // グレア(ブルーム)スロットセット初期化
  m_GlareSlotSet.Initialize();
}

void DrawEnvAnimationController::Finalize()
{
  //kawa20150514
  m_GlareSlotSet.Finalize();
  m_FogSlotSet.Finalize();

  m_CameraSlotSet.Finalize();
  m_LightSlotSet.Finalize();
}

LightAnimationSlot* DrawEnvAnimationController::GetLightSlot(u32 slotIndex, SlotType slotType)
{
  return m_LightSlotSet.GetSlotOrCreate(m_pHeap, m_pDrawEnvNode, slotIndex, (u32)slotType);
}

const LightAnimationSlot* DrawEnvAnimationController::GetLightSlot(u32 slotIndex, SlotType slotType) const
{
  return m_LightSlotSet.GetSlot(slotIndex, (u32)slotType);
}

void DrawEnvAnimationController::LightInterpRequest(f32 interpFrame)
{
  m_LightSlotSet.InterpRequest(interpFrame);
}

bool DrawEnvAnimationController::IsLightInterp() const
{
  return m_LightSlotSet.IsInterp();
}

CameraAnimationSlot* DrawEnvAnimationController::GetCameraSlot(u32 slotIndex, SlotType slotType)
{
  return m_CameraSlotSet.GetSlotOrCreate(m_pHeap, m_pDrawEnvNode, slotIndex, (u32)slotType);
}

const CameraAnimationSlot* DrawEnvAnimationController::GetCameraSlot(u32 slotIndex, SlotType slotType) const
{
  return m_CameraSlotSet.GetSlot(slotIndex, (u32)slotType);
}

void DrawEnvAnimationController::CameraInterpRequest(f32 interpFrame)
{
  m_CameraSlotSet.InterpRequest(interpFrame);
}

bool DrawEnvAnimationController::IsCameraInterp() const
{
  return m_CameraSlotSet.IsInterp();
}

//kawa20150514
FogAnimationSlot* DrawEnvAnimationController::GetFogSlot(u32 slotIndex, SlotType slotType)
{
  return m_FogSlotSet.GetSlotOrCreate(m_pHeap, m_pDrawEnvNode, slotIndex, (u32)slotType);
}

const FogAnimationSlot* DrawEnvAnimationController::GetFogSlot(u32 slotIndex, SlotType slotType) const
{
  return m_FogSlotSet.GetSlot(slotIndex, (u32)slotType);
}

void DrawEnvAnimationController::FogInterpRequest(f32 interpFrame)
{
  m_FogSlotSet.InterpRequest(interpFrame);
}

bool DrawEnvAnimationController::IsFogInterp() const
{
  return m_FogSlotSet.IsInterp();
}

GlareAnimationSlot* DrawEnvAnimationController::GetGlareSlot(u32 slotIndex, SlotType slotType)
{
  return m_GlareSlotSet.GetSlotOrCreate(m_pHeap, m_pDrawEnvNode, slotIndex, (u32)slotType);
}

const GlareAnimationSlot* DrawEnvAnimationController::GetGlareSlot(u32 slotIndex, SlotType slotType) const
{
  return m_GlareSlotSet.GetSlot(slotIndex, (u32)slotType);
}

void DrawEnvAnimationController::GlareInterpRequest(f32 interpFrame)
{
  m_GlareSlotSet.InterpRequest(interpFrame);
}

bool DrawEnvAnimationController::IsGlareInterp() const
{
  return m_GlareSlotSet.IsInterp();
}

void DrawEnvAnimationController::Update()
{
  // ライトアニメーション更新
  this->UpdateLightAnimation();

  // カメラアニメーション更新
  this->UpdateCameraAnimation();

  //kawa20150514
  // フォグアニメーション更新
  this->UpdateFogAnimation();

  // グレア(ブルーム)アニメーション更新
  this->UpdateGlareAnimation();
}

void DrawEnvAnimationController::UpdateLightAnimation()
{
  // 補間フレーム更新
  f32 interpRate = m_LightSlotSet.UpdateInterpFrame();

  // ライト適用
  if (interpRate < 0.0f)
  {
    // 補間なし
    m_LightSlotSet.ApplyLight(m_pDrawEnvNode);
  }
  else
  {
    // 補間あり
    m_LightSlotSet.ApplyLightInterp(m_pDrawEnvNode, interpRate);
  }

  // アニメーションフレーム更新
  m_LightSlotSet.UpdateFrame();
}

void DrawEnvAnimationController::UpdateCameraAnimation()
{
  // 補間フレーム更新
  f32 interpRate = m_CameraSlotSet.UpdateInterpFrame();

  // カメラ適用
  if (interpRate < 0.0f)
  {
    // 補間なし
    m_CameraSlotSet.ApplyCamera(m_pDrawEnvNode);
  }
  else
  {
    // 補間あり
    m_CameraSlotSet.ApplyCameraInterp(m_pDrawEnvNode, interpRate);
  }

  // アニメーションフレーム更新
  m_CameraSlotSet.UpdateFrame();
}

//kawa20150514
void DrawEnvAnimationController::UpdateFogAnimation()
{
  // 補間フレーム更新
  f32 interpRate = m_FogSlotSet.UpdateInterpFrame();

  // フォグ適用
  if (interpRate < 0.0f)
  {
    // 補間なし
    m_FogSlotSet.ApplyFog(m_pDrawEnvNode);
  }
  else
  {
    // 補間あり
    m_FogSlotSet.ApplyFogInterp(m_pDrawEnvNode, interpRate);
  }

  // アニメーションフレーム更新
  m_FogSlotSet.UpdateFrame();
}

void DrawEnvAnimationController::UpdateGlareAnimation()
{
  // 補間フレーム更新
  f32 interpRate = m_GlareSlotSet.UpdateInterpFrame();

  // フォグ適用
  if (interpRate < 0.0f)
  {
    // 補間なし
    m_GlareSlotSet.ApplyGlare(m_pDrawEnvNode);
  }
  else
  {
    // 補間あり
    m_GlareSlotSet.ApplyGlareInterp(m_pDrawEnvNode, interpRate);
  }

  // アニメーションフレーム更新
  m_GlareSlotSet.UpdateFrame();
}

}}
