//======================================================================
/**
 * @file Debug_Parameter.cpp
 * @date 2016/12/26 12:52:03
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#include "System/include/GflUse.h"
#include "./DrawUtil.h"
#include "./ManualCamera.h"
#include "./Parameter.h"
#include "./Timer.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Debug{
// Initialize
void Parameter::Initialize()
{
  // Debug Heap
  gfl2::heap::HeapBase* const pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  // initialize Instance
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Parameter(pHeap);
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
}
// Terminate
void Parameter::Terminate()
{
  // reset parameter
  if(pInstance != NULL)
  {
    gfl2::debug::SetAssertEnable(pInstance->initialIsAssertEnabled);
  }
  // delete
  GFL_SAFE_DELETE(pInstance);
}
// getter
Parameter& Parameter::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// instance
Parameter* Parameter::pInstance = NULL;
// constructor
Parameter::Parameter(
            gfl2::heap::HeapBase* pHeap)
    : isDebugMode(false),
      travelDistance(0.0f),
      energy(0.0f),
      energyLimit(0.0f),
      forwardSpeed(0.0f),
      verticalSpeed(0.0f),
      isStickMode(false),
      isManuallyEnergyUpDownEnabled(false),
      isConstantEnergyMode(false),
      energyInConstantEnergyMode(0.5f),
      isDistortionTypeFixed(false),
      distortionTypeInFixedMode(DISTORTION_TYPE__UB),
      isDistortionRarityFixed(false),
      distortionRarityInFixedMode(DISTORTION_RARITY__0),
      isPlacementProbabilitySet0(false),
      isPlacementProbabilitySet1(false),
      isRailShapeFixed(false),
      railShapeInFixedMode(Rail::PART_SHAPE__STRAIGHT),
      isRailAngleFixed(false),
      railAngleInFixedMode(0.0f),
      isOutputDistanceOverwritten(false),
      distanceForOverwriting(0),
      isManualCameraEnable(false),
      isRandomDressUp(false),
      isAssertEnabled(true),
      initialIsAssertEnabled(gfl2::debug::GetAssertEnable()),
      modelVisualizeSkyBox(true),
      modelVisualizeRail(true),
      modelVisualizeInitialRail(true),
      modelVisualizeHero(true),
      modelVisualizePokemon(true),
      modelVisualizeEnergyAura(true),
      modelVisualizeEnergy(true),
      modelVisualizeObstacle(true),
      modelVisualizeDistortion(true),
      modelVisualizeDistortionVortex(true),
      pCameraParameter(NULL),
      pGimmickParameter(NULL),
      pGimmickPlacementParameter(NULL),
      pPlayerParameter(NULL),
      pRailParameter(NULL),
      pRunParameter(NULL),
      pSoundParameter(NULL),
      isTranslationTriggerEnable(false),
      isManualCameraBeingUsed(false)
{
  pManualCamera = GFL_NEW(pHeap) ManualCamera();
  pTimer = GFL_NEW(pHeap) Timer();
  pTimer->Initialize(
              pHeap);
  pDrawUtil = GFL_NEW(pHeap) DrawUtil();
  pDrawUtil->Initialize(
              pHeap);
}
// destructor
Parameter::~Parameter()
{
  GFL_SAFE_DELETE(pManualCamera);
  GFL_SAFE_DELETE(pTimer);
  GFL_SAFE_DELETE(pDrawUtil);
}
}// namespace Debug
}// namespace LunaSolDive
#endif// PM_DEBUG
