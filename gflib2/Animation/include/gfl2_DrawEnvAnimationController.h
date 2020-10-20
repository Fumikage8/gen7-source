#ifndef GFLIB2_DRAW_ENV_ANIMATION_CONTROLLER_H_INCLUDED
#define GFLIB2_DRAW_ENV_ANIMATION_CONTROLLER_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <Animation/include/gfl2_LightAnimationSlot.h>
#include <Animation/include/gfl2_CameraAnimationSlot.h>
#include <Animation/include/gfl2_FogAnimationSlot.h>
#include <Animation/include/gfl2_GlareAnimationSlot.h>
#include <Animation/include/gfl2_AnimationSlotSetInterp.h>

#include <heap/include/gfl2_Heap.h>

namespace gfl2 { namespace animation {

/**
 * @brief シーン環境アニメーションコントローラー
 */
class DrawEnvAnimationController
{
public:

  enum SlotType
  {
    SLOT_TYPE_NORMAL,   ///< 通常スロット
    SLOT_TYPE_INTERP,   ///< 補間用スロット
    
    SLOT_TYPE_COUNT     ///< スロット総数
  };

public:

	DrawEnvAnimationController();
  ~DrawEnvAnimationController();

	void Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);
  void Finalize();

  LightAnimationSlot* GetLightSlot(u32 slotIndex = 0, SlotType slotType = SLOT_TYPE_NORMAL);
  const LightAnimationSlot* GetLightSlot(u32 slotIndex = 0, SlotType slotType = SLOT_TYPE_NORMAL) const;
  void LightInterpRequest(f32 interpFrame);
  bool IsLightInterp() const;

  CameraAnimationSlot* GetCameraSlot(u32 slotIndex = 0, SlotType slotType = SLOT_TYPE_NORMAL);
  const CameraAnimationSlot* GetCameraSlot(u32 slotIndex = 0, SlotType slotType = SLOT_TYPE_NORMAL) const;
  void CameraInterpRequest(f32 interpFrame);
  bool IsCameraInterp() const;

  //kawa20150514
  FogAnimationSlot* GetFogSlot(u32 slotIndex = 0, SlotType slotType = SLOT_TYPE_NORMAL);
  const FogAnimationSlot* GetFogSlot(u32 slotIndex = 0, SlotType slotType = SLOT_TYPE_NORMAL) const;
  void FogInterpRequest(f32 interpFrame);
  bool IsFogInterp() const;

  GlareAnimationSlot* GetGlareSlot(u32 slotIndex = 0, SlotType slotType = SLOT_TYPE_NORMAL);
  const GlareAnimationSlot* GetGlareSlot(u32 slotIndex = 0, SlotType slotType = SLOT_TYPE_NORMAL) const;
  void GlareInterpRequest(f32 interpFrame);
  bool IsGlareInterp() const;

  void Update();

private:

  void UpdateLightAnimation();
  void UpdateCameraAnimation();
  //kawa20150514
  void UpdateFogAnimation();
  void UpdateGlareAnimation();

private:

  gfl2::heap::HeapBase*                                             m_pHeap;                        ///< アニメーションに使用するヒープ
	gfl2::renderingengine::scenegraph::instance::DrawEnvNode*         m_pDrawEnvNode;                 ///< データ反映対象のシーン環境ノード

  AnimationSlotSetInterp<LightAnimationSlot>                        m_LightSlotSet;                 ///< ライトスロットセット
  AnimationSlotSetInterp<CameraAnimationSlot>                       m_CameraSlotSet;                ///< カメラスロットセット
  //kawa20150514
  AnimationSlotSetInterp<FogAnimationSlot>                          m_FogSlotSet;                   ///< フォグスロットセット
  AnimationSlotSetInterp<GlareAnimationSlot>                        m_GlareSlotSet;                 ///< グレア(ブルーム)スロットセット
};

}}

#endif