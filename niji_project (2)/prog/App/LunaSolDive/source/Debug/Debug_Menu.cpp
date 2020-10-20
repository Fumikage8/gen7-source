//======================================================================
/**
 * @file Debug_Menu.cpp
 * @date 2016/12/22 20:15:38
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include "System/include/GflUse.h"
#include "./DrawUtil.h"
#include "./Menu.h"
#include "./Parameter.h"
#include "./Timer.h"
#include "../Assert.h"
#include "../Camera/Parameter.h"
#include "../Gimmick/Parameter.h"
#include "../Gimmick/Placement/Parameter.h"
#include "../Player/Parameter.h"
#include "../Rail/Parameter.h"
#include "../Run/Parameter.h"
#include "../Sound/Parameter.h"

namespace LunaSolDive{
namespace Debug{
// static function
// Reset Parameter
static void ResetParameter(
          void* pWork,
          gfl2::debug::DebugWinItem* pItem);
// Draw Name
static wchar_t* DrawName(
            void* pWork,
            gfl2::debug::DebugWinItem* pItem);
// Draw Label
static wchar_t* DrawLabel(
            void* pWork,
            gfl2::debug::DebugWinItem* pItem);
// Draw f32
static wchar_t* DrawF32(
            void* pValue,
            gfl2::debug::DebugWinItem* pItem);
// Draw gfl2::math::Vector2
static wchar_t* DrawVector2(
            void* pValue,
            gfl2::debug::DebugWinItem* pItem);
// Draw gfl2::math::Vector3
static wchar_t* DrawVector3(
            void* pValue,
            gfl2::debug::DebugWinItem* pItem);
// Draw Timer
static wchar_t* DrawTimer(
            void* pTimer,
            gfl2::debug::DebugWinItem* pItem);
// create Motion Brend Frames Page
static void CreateMotionBrendFramesPage(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap,
            wchar_t* name,
            Player::MotionBrendFrames* pTarget);
// Add Volume Control Parameter Items
static void AddVolumeControlParameterItems(
            gfl2::debug::DebugWinGroup* pPage,
            gfl2::heap::HeapBase* pHeap,
            wchar_t* name,
            Sound::VolumeControlParameter* pTarget);
// DebugWinItemValueU32 modified
class U32Item: public gfl2::debug::DebugWinItem
{
public:
  U32Item(
              gfl2::debug::DebugWinGroup* pParent,
              const wchar_t* name,
              u32* pTarget,
              const u32& min,
              const u32& max,
              const u32& step);
  virtual void UpdateFunc(
              gfl2::debug::DebugWin* pDebugSystem);
  virtual const wchar_t* GetDispName(
              gfl2::debug::DebugWin* pDebugSystem);
  static void Create(
              gfl2::debug::DebugWinGroup* pParent,
              gfl2::heap::HeapBase* pHeap,
              const wchar_t* name,
              u32* pTarget,
              const u32& min,
              const u32& max,
              const u32& step);
private:
  u32* m_pTarget;
  u32 m_Min;
  u32 m_Max;
  u32 m_Step;
};
// DebugWinItemValueFloat with precision
class FloatItem: public gfl2::debug::DebugWinItemValue<f32>
{
public:
  FloatItem(
              gfl2::debug::DebugWinGroup* pParent,
              const wchar_t* name,
              f32* pTarget,
              const f32& min,
              const f32& max,
              const f32& step,
              const u32& precision);
  virtual const wchar_t* GetDispName(
              gfl2::debug::DebugWin *debSys);
  const u32& Precision() const;
  static void Create(
              gfl2::debug::DebugWinGroup* pParent,
              gfl2::heap::HeapBase* pHeap,
              const wchar_t* name,
              f32* pTarget,
              const f32& min,
              const f32& max,
              const f32& step,
              const u32& precision);
protected:
  static const u32 FORMAT_STR_SIZE = 12;// "%ls [%+.XXf]"
private:
  u32 m_Precision;
};
// DebugWinItemValueFloat with precision (radian ver)
class RadianItem: public FloatItem
{
public:
  RadianItem(
              gfl2::debug::DebugWinGroup* pParent,
              const wchar_t* name,
              f32* pTarget,
              const f32& minDegree,
              const f32& maxDegree,
              const f32& stepDegree,
              const u32& precision);
  virtual const wchar_t* GetDispName(
              gfl2::debug::DebugWin *debSys);
  static void Create(
              gfl2::debug::DebugWinGroup* pParent,
              gfl2::heap::HeapBase* pHeap,
              const wchar_t* name,
              f32* pTarget,
              const f32& min,
              const f32& max,
              const f32& step,
              const u32& precision);
private:
};
// DistortionType Item
class DistortionTypeItem: public gfl2::debug::DebugWinItemValue<u8>
{
public:
  DistortionTypeItem(
              gfl2::debug::DebugWinGroup* pParent,
              const wchar_t* name,
              DistortionType* pTarget);
  virtual const wchar_t* GetDispName(
              gfl2::debug::DebugWin *debSys);
  static void Create(
              gfl2::debug::DebugWinGroup* pParent,
              gfl2::heap::HeapBase* pHeap,
              const wchar_t* name,
              DistortionType* pTarget);
private:
};
// Add Label
static void AddLabel(
            gfl2::debug::DebugWinGroup* pParent,
            gfl2::heap::HeapBase* pHeap,
            const wchar_t* name);

// constructor
Menu::Menu()
  : m_pHeap(NULL),
    m_pRoot(NULL){}
// Initialize
void Menu::Initialize()
{
  // Debug Heap
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
  LUNA_SOL_DIVE_ASSERT(m_pHeap != NULL);
  // Root Page
  m_pRoot = gfl2::debug::DebugWin_AddGroup(
              m_pHeap,
              L"LunaSolDive",
              NULL);
  LUNA_SOL_DIVE_ASSERT(m_pRoot != NULL);
  // Info
  AddPageInfo(m_pRoot, m_pHeap);
  // Mode
  AddPageMode(m_pRoot, m_pHeap);
  // Gyroscope
  AddPageGyroscope(m_pRoot, m_pHeap);
  // Parameter
  AddPageParameter(m_pRoot, m_pHeap);
  // Model
  AddPageModel(m_pRoot, m_pHeap);
  // DrawUtil
  AddPageDrawUtil(m_pRoot, m_pHeap);
}
// Terminate
void Menu::Terminate()
{
  // remove root page
  if(m_pRoot != NULL)
  {
    gfl2::debug::DebugWin_RemoveGroup(m_pRoot);
  }
  // unset pointer
  m_pHeap = NULL;
  m_pRoot = NULL;
}
// Update
void Menu::Update()
{
  // enable & disable assert
  gfl2::debug::SetAssertEnable(Parameter::Get().isAssertEnabled);
}
// static functions
// Add Page: Info
void Menu::AddPageInfo(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Info",
              pParentPage);
  // Info/Epalsed Time
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Epalsed Time",
              Parameter::Get().pTimer,
              NULL,
              DrawTimer);
  // Info: 進行距離
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Travel Distance[cm]",
              &Parameter::Get().travelDistance,
              NULL,
              DrawF32);
  // Energy
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Energy",
              &Parameter::Get().energy,
              NULL,
              DrawF32);
  // Limit of Energy
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Limit of Energy",
              &Parameter::Get().energyLimit,
              NULL,
              DrawF32);
  // Info: 前進速度
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Forward Speed[cm/frame]",
              &Parameter::Get().forwardSpeed,
              NULL,
              DrawF32);
  // Info: 上下左右移動速度
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Vertical Speed[/frame]",
              &Parameter::Get().verticalSpeed,
              NULL,
              DrawF32);
  // Info: Aim 目的地
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Aim Input",
              &Parameter::Get().aimInput,
              NULL,
              DrawVector2);
  // Info: Aim 現在地
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Aim Point",
              &Parameter::Get().aimPoint,
              NULL,
              DrawVector2);
}
// Add Page: Mode
void Menu::AddPageMode(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Mode",
              pParentPage);
  // Record Count
  U32Item::Create(
              pPage,
              pHeap,
              L"Record Count",
              &Parameter::Get().pGimmickPlacementParameter->m_RecordCount,
              0,
              9999,
              1);
  // Stick Mode
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Stick Mode",
              &Parameter::Get().isStickMode);
  // Energy
  AddLabel(pPage, pHeap, L"Energy");
  // Energy/Manually Up/Down
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Manually Up/Down",
              &Parameter::Get().isManuallyEnergyUpDownEnabled);
  // Energy/Constant Mode
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Constant Mode",
              &Parameter::Get().isConstantEnergyMode);
  // Energy/Energy in Constant Mode
  FloatItem::Create(
              pPage,
              pHeap,
              L"Energy in Constant Mode",
              &Parameter::Get().energyInConstantEnergyMode,
              0.0f,
              2.0f,
              0.1f,
              5);
  // Distortion
  AddLabel(pPage, pHeap, L"Distortion");
  // Distortion/Fixed Type Mode
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Fixed Type Mode",
              &Parameter::Get().isDistortionTypeFixed);
  // Distortion/Type in Fixed Mode
  DistortionTypeItem::Create(
              pPage,
              pHeap,
              L"Type in Fixed Mode",
              &Parameter::Get().distortionTypeInFixedMode);
  // Distortion/Fixed Rarity Mode
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Fixed Rarity Mode",
              &Parameter::Get().isDistortionRarityFixed);
  // Distortion/Rarity in Fixed Mode
  gfl2::debug::DebugWin_AddItemValueU8(
              pPage,
              pHeap,
              L"Rarity in Fixed Mode",
              reinterpret_cast<u8*>(&Parameter::Get().distortionRarityInFixedMode),
              DISTORTION_RARITY__0,
              DISTORTION_RARITY__3);
  // Placement Probability
  AddLabel(pPage, pHeap, L"Placement Probability");
  // Placement Probability/Set to 0
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Set to 0",
              &Parameter::Get().isPlacementProbabilitySet0);
  // Placement Probability/Set to 1
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Set to 1",
              &Parameter::Get().isPlacementProbabilitySet1);
  // Rail
  AddLabel(pPage, pHeap, L"Rail");
  // Rail/Fixed Shape Mode
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Fixed Shape Mode",
              &Parameter::Get().isRailShapeFixed);
  // Rail/Shape in Fixed Mode
  gfl2::debug::DebugWin_AddItemValueU8(
              pPage,
              pHeap,
              L"Shape in Fixed Mode",
              reinterpret_cast<u8*>(&Parameter::Get().railShapeInFixedMode),
              0,
              Rail::PART_SHAPE__SIZE - 1);
  // Rail/Fixed Angle Mode
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Fixed Angle Mode",
              &Parameter::Get().isRailAngleFixed);
  // Rail/Angle in Fixed Mode
  RadianItem::Create(
              pPage,
              pHeap,
              L"Angle in Fixed Mode[degree]",
              &Parameter::Get().railAngleInFixedMode,
              -180.0f,
              180.0f,
              1.0f,
              2);
  // Result
  AddLabel(pPage, pHeap, L"Result");
  // Result/Overwrite Distance
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Overwrite Distance",
              &Parameter::Get().isOutputDistanceOverwritten);
  // Result/Distance used for Overwriting
  U32Item::Create(
              pPage,
              pHeap,
              L"Distance used for Overwriting[m]",
              &Parameter::Get().distanceForOverwriting,
              0,
              10000,
              1);
  // Debug
  AddLabel(pPage, pHeap, L"Debug");
  // Debug/Manual Camera Enable
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Manual Camera Enable",
              &Parameter::Get().isManualCameraEnable);
  // Debug/Random Dress Up
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Random Dress Up",
              &Parameter::Get().isRandomDressUp);
  // Debug/Assert
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Assert",
              &Parameter::Get().isAssertEnabled);
}
// Add Page: Gyroscope
void Menu::AddPageGyroscope(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Gyroscope",
              pParentPage);
  // Raw
  AddLabel(pPage, pHeap, L"Raw");
  // Gyroscope: Speed
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Speed",
              &Parameter::Get().gyroscopeSpeed,
              NULL,
              DrawVector3);
  // Gyroscope: Angle (raw)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Angle",
              &Parameter::Get().gyroscopeRawAngle,
              NULL,
              DrawVector3);
  // Gyroscope: Direction X (raw)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Direction X",
              &Parameter::Get().gyroscopeRawDirectionX,
              NULL,
              DrawVector3);
  // Gyroscope: Direction Y (raw)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Direction Y",
              &Parameter::Get().gyroscopeRawDirectionY,
              NULL,
              DrawVector3);
  // Gyroscope: Direction Z (raw)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Direction Z",
              &Parameter::Get().gyroscopeRawDirectionZ,
              NULL,
              DrawVector3);
  // Gyroscope: Acceleration (raw)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Acceleration",
              &Parameter::Get().gyroscopeRawAcceleration,
              NULL,
              DrawVector3);
  // Adjusted
  AddLabel(pPage, pHeap, L"Adjusted");
  // Gyroscope: Angle (adjusted)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Angle",
              &Parameter::Get().gyroscopeAdjustedAngle,
              NULL,
              DrawVector3);
  // Gyroscope: Direction X (adjusted)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Direction X",
              &Parameter::Get().gyroscopeAdjustedDirectionX,
              NULL,
              DrawVector3);
  // Gyroscope: Direction Y (adjusted)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Direction Y",
              &Parameter::Get().gyroscopeAdjustedDirectionY,
              NULL,
              DrawVector3);
  // Gyroscope: Direction Z (adjusted)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Direction Z",
              &Parameter::Get().gyroscopeAdjustedDirectionZ,
              NULL,
              DrawVector3);
  // Gyroscope: Acceleration (adjusted)
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Acceleration",
              &Parameter::Get().gyroscopeAdjustedAcceleration,
              NULL,
              DrawVector3);
}
// Add Page: Parameter
void Menu::AddPageParameter(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Parameter",
              pParentPage);
  // Reset
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pPage,
              pHeap,
              L"Reset",
              NULL,
              ResetParameter,
              DrawName);
  // Parameter/Run
  AddPageParameterRun(pPage, pHeap);
  // Parameter/Gyroscope
  AddPageParameterGyroscope(pPage, pHeap);
  // Parameter/Camera
  AddPageParameterCamera(pPage, pHeap);
  // Parameter/Rail
  AddPageParameterRail(pPage, pHeap);
  // Parameter/Gimmick
  AddPageParameterGimmick(pPage, pHeap);
  // Parameter/Gimmick Placement
  AddPageParameterGimmickPlacement(pPage, pHeap);
  // Parameter/Player
  AddPageParameterPlayer(pPage, pHeap);
  // Parameter/Motion Brend Frames
  AddPageParameterMotionBrendFrames(pPage, pHeap);
  // Parameter/Beginning
  AddPageParameterBeginning(pPage, pHeap);
  // Parameter/Ending
  AddPageParameterEnding(pPage, pHeap);
  // Parameter/Sound
  AddPageParameterSound(pPage, pHeap);
  // Parameter/Translation
  AddPageParameterTranslation(pPage, pHeap);
}
// Add Page: Parameter/Run
void Menu::AddPageParameterRun(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Run",
              pParentPage);
  // Speed
  AddLabel(pPage, pHeap, L"Speed");
  // Speed/Forward Min
  FloatItem::Create(
              pPage,
              pHeap,
              L"Forward Min[cm/frame]",
              &Parameter::Get().pRunParameter->m_ForwardSpeedMin,
              0.0f,
              100.0f,
              1.0f,
              2);
  // Speed/Forward Max
  FloatItem::Create(
              pPage,
              pHeap,
              L"Forward Max[cm/frame]",
              &Parameter::Get().pRunParameter->m_ForwardSpeedMax,
              50.0f,
              200.0f,
              1.0f,
              2);
  // Speed/Cross-Section Min
  FloatItem::Create(
              pPage,
              pHeap,
              L"Cross-Section Min[/frame]",
              &Parameter::Get().pRunParameter->m_CrossSectionSpeedMin,
              0.010f,
              0.035f,
              0.001f,
              4);
  // Speed/Cross-Section Max
  FloatItem::Create(
              pPage,
              pHeap,
              L"Cross-Section Max[/frame]",
              &Parameter::Get().pRunParameter->m_CrossSectionSpeedMax,
              0.020f,
              0.050f,
              0.001f,
              4);
  // Speed/Cross-Section Empty
  FloatItem::Create(
              pPage,
              pHeap,
              L"Cross-Section Empty[/frame]",
              &Parameter::Get().pRunParameter->m_CrossSectionSpeedEmpty,
              0.0000f,
              0.0200f,
              0.0005f,
              5);
  // Energy
  AddLabel(pPage, pHeap, L"Energy");
  // Energy/Initial Rate
  FloatItem::Create(
              pPage,
              pHeap,
              L"Initial Rate",
              &Parameter::Get().pRunParameter->m_EnergyInitialRate,
              0.00f,
              1.00f,
              0.01f,
              3);
  // Energy/Attenuation Frames
  U32Item::Create(
              pPage,
              pHeap,
              L"Atteneuation Frames",
              &Parameter::Get().pRunParameter->m_EnergyAttenuationFrames,
              600,
              3000,
              1);
  // Energy/Increase of Limit
  FloatItem::Create(
              pPage,
              pHeap,
              L"Increase of Limit",
              &Parameter::Get().pRunParameter->m_EnergyLimitIncrease,
              0.00f,
              1.00f,
              0.005f,
              4);
  // Energy/Limit of Limit
  FloatItem::Create(
              pPage,
              pHeap,
              L"Limit of Limit",
              &Parameter::Get().pRunParameter->m_EnergyLimitLimit,
              1.00f,
              3.00f,
              0.10f,
              3);
}
// Add Page: Parameter/Gyroscope
void Menu::AddPageParameterGyroscope(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Gyroscope",
              pParentPage);
  // Yaw Angle Range
  RadianItem::Create(
              pPage,
              pHeap,
              L"Yaw Angle Range[degree]",
              &Parameter::Get().pRunParameter->m_YawAngleRange,
              5.0f,
              30.0f,
              0.2f,
              3);
  // Pitch Angle Range
  RadianItem::Create(
              pPage,
              pHeap,
              L"Pitch Angle Range[degree]",
              &Parameter::Get().pRunParameter->m_PitchAngleRange,
              5.0f,
              30.0f,
              0.2f,
              3);
  // Reset
  AddLabel(pPage, pHeap, L"Reset");
  // Reset/Threshold
  FloatItem::Create(
              pPage,
              pHeap,
              L"Threshold",
              &Parameter::Get().pRunParameter->m_ResetThreshold,
              0.2f,
              0.7f,
              0.02f,
              3);
  // Reset/Wait Frames
  U32Item::Create(
              pPage,
              pHeap,
              L"Wait Frames",
              &Parameter::Get().pRunParameter->m_ResetWaitFrame,
              0,
              60,
              1);
  // Reset/Interval Frames
  U32Item::Create(
              pPage,
              pHeap,
              L"Interval Frames",
              &Parameter::Get().pRunParameter->m_ResetIntervalFrame,
              0,
              60,
              1);
}
// Add Page: Parameter/Camera
void Menu::AddPageParameterCamera(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Camera",
              pParentPage);
  // Perspective
  AddLabel(pPage, pHeap, L"Perspective");
  // Perspective/Field of View Y
  RadianItem::Create(
              pPage,
              pHeap,
              L"Field of View Y[degree]",
              &Parameter::Get().pCameraParameter->m_FieldOfViewY,
              20.0f,
              50.0f,
              0.5f,
              2);
  // Perspective/Near Clip
  FloatItem::Create(
              pPage,
              pHeap,
              L"Near Clip[cm]",
              &Parameter::Get().pCameraParameter->m_NearClip,
              0.0f,
              100.0f,
              1.0f,
              2);
  // Perspective/Far Clip
  FloatItem::Create(
              pPage,
              pHeap,
              L"Far Clip[cm]",
              &Parameter::Get().pCameraParameter->m_FarClip,
              1.0e+4f,
              5.0e+4f,
              1.0e+3f,
              2);
  // View
  AddLabel(pPage, pHeap, L"View");
  // View/Camera Forward
  FloatItem::Create(
              pPage,
              pHeap,
              L"Camera Forward",
              &Parameter::Get().pCameraParameter->m_CameraForward,
              -5000.0f,
              5000.0f,
              10.0f,
              2);
  // View/Camera Forward
  FloatItem::Create(
              pPage,
              pHeap,
              L"Camera Vertical",
              &Parameter::Get().pCameraParameter->m_CameraVertical,
              -500.0f,
              500.0f,
              1.0f,
              2);
  // View/Traget Forward
  FloatItem::Create(
              pPage,
              pHeap,
              L"Traget Forward",
              &Parameter::Get().pCameraParameter->m_TargetForward,
              -5000.0f,
              5000.0f,
              10.0f,
              2);
  // View/Traget Forward
  FloatItem::Create(
              pPage,
              pHeap,
              L"Traget Vertical",
              &Parameter::Get().pCameraParameter->m_TargetVertical,
              -500.0f,
              500.0f,
              1.0f,
              2);
  // Trace
  AddLabel(pPage, pHeap, L"Trace");
  // Trace/Movement
  FloatItem::Create(
              pPage,
              pHeap,
              L"Movement",
              &Parameter::Get().pCameraParameter->m_TraceMovement,
              0.000f,
              2.000f,
              0.001f,
              4);
  // Trace/Vertical Range
  FloatItem::Create(
              pPage,
              pHeap,
              L"Vertical Range",
              &Parameter::Get().pCameraParameter->m_TraceVerticalRange,
              0.00f,
              1.50f,
              0.01f,
              4);
  // Trace/Horizontal Range
  FloatItem::Create(
              pPage,
              pHeap,
              L"Horizontal Range",
              &Parameter::Get().pCameraParameter->m_TraceHorizontalRange,
              0.00f,
              1.50f,
              0.01f,
              4);
  // Acceleration
  AddLabel(pPage, pHeap, L"Acceleration");
  // Acceleration/Acceleration Displacement
  FloatItem::Create(
              pPage,
              pHeap,
              L"Acceleration Displacement[cm]",
              &Parameter::Get().pCameraParameter->m_AccelerationDisplacement,
              -1000.0f,
              0.0f,
              5.0f,
              2);
  // Acceleration/Deceleration Displacement
  FloatItem::Create(
              pPage,
              pHeap,
              L"Deceleration Displacement[cm]",
              &Parameter::Get().pCameraParameter->m_DecelerationDisplacement,
              0.0f,
              1000.0f,
              5.0f,
              2);
  // Acceleration/Kickstart Rate
  FloatItem::Create(
              pPage,
              pHeap,
              L"Kickstart Rate",
              &Parameter::Get().pCameraParameter->m_AccelerationKickstartRate,
              0.0f,
              1.0f,
              0.01f,
              4);
  // Acceleration/Attenuation Rate
  FloatItem::Create(
              pPage,
              pHeap,
              L"Attenuation Rate",
              &Parameter::Get().pCameraParameter->m_AccelerationAttenuationRate,
              0.0f,
              1.0f,
              0.001f,
              4);
  // Acceleration/End Threshold
  FloatItem::Create(
              pPage,
              pHeap,
              L"End Threshold",
              &Parameter::Get().pCameraParameter->m_AccelerationEndThreshold,
              0.0f,
              0.1f,
              0.0001f,
              5);
  // Damage Vibration
  AddLabel(pPage, pHeap, L"Damage Vibration");
  // Damage Vibration/Amplitude
  RadianItem::Create(
              pPage,
              pHeap,
              L"Amplitude[degree]",
              &Parameter::Get().pCameraParameter->m_DamageVibrationAmplitude,
              5.0f,
              30.0f,
              0.5f,
              2);
  // Damage Vibration/Damping Factor
  FloatItem::Create(
              pPage,
              pHeap,
              L"Damping Factor[frame]",
              &Parameter::Get().pCameraParameter->m_DamageVibrationDampingFactor,
              5.0f,
              40.0f,
              1.0f,
              2);
  // Damage Vibration/Period
  FloatItem::Create(
              pPage,
              pHeap,
              L"Period[frame]",
              &Parameter::Get().pCameraParameter->m_DamageVibrationPeriod,
              0.0f,
              30.0f,
              0.1f,
              2);
  // Gravity Vibration
  AddLabel(pPage, pHeap, L"Gravity Vibration");
  // Gravity Vibration/Amplitude
  FloatItem::Create(
              pPage,
              pHeap,
              L"Amplitude[cm]",
              &Parameter::Get().pCameraParameter->m_GravityVibrationAmplitude,
              0.0f,
              30.0f,
              1.0f,
              2);
  // Gravity Vibration/Period
  FloatItem::Create(
              pPage,
              pHeap,
              L"Period[frame]",
              &Parameter::Get().pCameraParameter->m_GravityVibrationPeriod,
              0.0f,
              10.0f,
              0.1f,
              2);
  // Effect
  AddLabel(pPage, pHeap, L"Effect");
  // Effect/Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Distance[cm]",
              &Parameter::Get().pCameraParameter->m_EffectDistance,
              0.0f,
              500.0f,
              1.0f,
              2);
}
// Add Page: Parameter/Rail
void Menu::AddPageParameterRail(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Rail",
              pParentPage);
  // Radius
  FloatItem::Create(
              pPage,
              pHeap,
              L"Radius[cm]",
              &Parameter::Get().pRailParameter->m_Radius,
              0.0f,
              600.0f,
              10.0f,
              2);
  // Initial Straight Number
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Initial Straight Number",
              &Parameter::Get().pRailParameter->m_InitialStraightNumber,
              0,
              20,
              1);
  // Curve Forbidden Angle
  AddLabel(pPage, pHeap, L"Curve Forbidden Angle");
  // Curve Forbidden Angle/Upper Range
  RadianItem::Create(
              pPage,
              pHeap,
              L"Upper Range[degree]",
              &Parameter::Get().pRailParameter->m_CurveForbiddenAngleUpperRange,
              0.0f,
              90.0f,
              1.0f,
              2);
  // Curve Forbidden Angle/Lower Range
  RadianItem::Create(
              pPage,
              pHeap,
              L"Lower Range[degree]",
              &Parameter::Get().pRailParameter->m_CurveForbiddenAngleLowerRange,
              0.0f,
              90.0f,
              1.0f,
              2);
}
// Add Page: Parameter/Gimmick
void Menu::AddPageParameterGimmick(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Gimmick",
              pParentPage);
  // Object
  AddLabel(pPage, pHeap, L"Object");
  // Object/Appearance Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Appearance Distance[cm]",
              &Parameter::Get().pGimmickParameter->m_ObjectAppearanceDistance,
              0.0f,
              1.0e+5f,
              1.0e+3f,
              2);
  // Object/Disappearance Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Disappearance Distance[cm]",
              &Parameter::Get().pGimmickParameter->m_ObjectDisappearanceDistance,
              -1.0e+4f,
              1.0e+4f,
              1.0e+3f,
              2);
  // Energy
  AddLabel(pPage, pHeap, L"Energy");
  // Energy/Energy Gain
  FloatItem::Create(
              pPage,
              pHeap,
              L"Energy Gain",
              &Parameter::Get().pGimmickParameter->m_EnergyEnergyGain,
              0.0f,
              1.0f,
              0.01f,
              2);
  // Energy/Activation Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Activation Distance[cm]",
              &Parameter::Get().pGimmickParameter->m_EnergyActivationDistance,
              0.0f,
              1000.0f,
              10.0f,
              2);
  // Energy/Inctivation Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Inactivation Distance[cm]",
              &Parameter::Get().pGimmickParameter->m_EnergyInactivationDistance,
              -1000.0f,
              0.0f,
              10.0f,
              2);
  // Energy/Collision Radius
  FloatItem::Create(
              pPage,
              pHeap,
              L"Collision Radius[cm]",
              &Parameter::Get().pGimmickParameter->m_EnergyCollisionRadius,
              0.0f,
              450.0f,
              1.0f,
              2);
  // Energy/Collision Height
  FloatItem::Create(
              pPage,
              pHeap,
              L"Collision Height[cm]",
              &Parameter::Get().pGimmickParameter->m_EnergyCollisionHeight,
              0.0f,
              450.0f,
              1.0f,
              2);
  // Obstacle
  AddLabel(pPage, pHeap, L"Obstacle");
  // Obstacle/Activation Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Activation Distance[cm]",
              &Parameter::Get().pGimmickParameter->m_ObstacleActivationDistance,
              0.0f,
              1000.0f,
              10.0f,
              2);
  // Obstacle/Inctivation Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Inactivation Distance[cm]",
              &Parameter::Get().pGimmickParameter->m_ObstacleInactivationDistance,
              -1000.0f,
              0.0f,
              10.0f,
              2);
  // Obstacle/Energy Loss
  FloatItem::Create(
              pPage,
              pHeap,
              L"Energy Loss",
              &Parameter::Get().pGimmickParameter->m_ObstacleEnergyLoss,
              0.0f,
              1.0f,
              0.01f,
              2);
  // Obstacle/Collision Radius
  FloatItem::Create(
              pPage,
              pHeap,
              L"Collision Radius[cm]",
              &Parameter::Get().pGimmickParameter->m_ObstacleCollisionRadius,
              0.0f,
              450.0f,
              1.0f,
              2);
  // Obstacle/Collision Height
  FloatItem::Create(
              pPage,
              pHeap,
              L"Collision Height[cm]",
              &Parameter::Get().pGimmickParameter->m_ObstacleCollisionHeight,
              0.0f,
              450.0f,
              1.0f,
              2);
  // Distortion
  AddLabel(pPage, pHeap, L"Distortion");
  // Distortion/Activation Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Activation Distance[cm]",
              &Parameter::Get().pGimmickParameter->m_DistortionActivationDistance,
              0.0f,
              100.0e+2f,
              1.0e+2f,
              2);
  // Distortion/Inctivation Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Inactivation Distance[cm]",
              &Parameter::Get().pGimmickParameter->m_DistortionInactivationDistance,
              -1000.0f,
              0.0f,
              10.0f,
              2);
  // Distortion/Vanish Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Vanish Distance[cm]",
              &Parameter::Get().pGimmickParameter->m_DistortionVanishDistance,
              0.0f,
              20.0e+2f,
              0.1e+2f,
              2);
  // Distortion/Collision Radius
  FloatItem::Create(
              pPage,
              pHeap,
              L"Collision Radius[cm]",
              &Parameter::Get().pGimmickParameter->m_DistortionCollisionRadius,
              1.0f,
              450.0f,
              1.0f,
              2);
  // Distortion/Gravity Strength
  FloatItem::Create(
              pPage,
              pHeap,
              L"Gravity Strength",
              &Parameter::Get().pGimmickParameter->m_DistortionGravityStrength,
              0.0f,
              1.0f,
              0.001f,
              4);
  // Distortion/Gravity Forward Range
  FloatItem::Create(
              pPage,
              pHeap,
              L"Gravity Forward Range[cm]",
              &Parameter::Get().pGimmickParameter->m_DistortionGravityForwardRange,
              0.0f,
              100.0e+2f,
              0.1e+2f,
              2);
  // Distortion/Gravity Backward Range
  FloatItem::Create(
              pPage,
              pHeap,
              L"Gravity Backward Range[cm]",
              &Parameter::Get().pGimmickParameter->m_DistortionGravityBackwardRange,
              0.0f,
              100.0e+2f,
              0.1e+2f,
              2);
  // Distortion/Gravity Side Range
  FloatItem::Create(
              pPage,
              pHeap,
              L"Gravity Side Range[cm]",
              &Parameter::Get().pGimmickParameter->m_DistortionGravitySideRange,
              1.0f,
              300.0f,
              1.0f,
              2);
  // Distortion/Rarity Effect Offset
  FloatItem::Create(
              pPage,
              pHeap,
              L"Rarity Effect Offset",
              &Parameter::Get().pGimmickParameter->m_DistortionRarityEffectOffset,
              0.0f,
              500.0f,
              10.0f,
              2);
}
// Add Page: Parameter/Gimmick Placement
void Menu::AddPageParameterGimmickPlacement(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Gimmick Placement",
              pParentPage);
  // Unit Length
  FloatItem::Create(
              pPage,
              pHeap,
              L"Unit Length[cm]",
              &Parameter::Get().pGimmickPlacementParameter->m_UnitLength,
              5.0e+2f,
              30.0e+2f,
              1.0e+2f,
              2);
  // Block Size
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"BlockSize",
              &Parameter::Get().pGimmickPlacementParameter->m_BlockSize,
              0,
              10,
              1);
  // Ahead Units
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Ahead Units",
              &Parameter::Get().pGimmickPlacementParameter->m_AheadUnits,
              0,
              20,
              1);
  // Initial Placement
  AddLabel(pPage, pHeap, L"Initial Placement");
  // Initial Placement/Range
  FloatItem::Create(
              pPage,
              pHeap,
              L"Range[cm]",
              &Parameter::Get().pGimmickPlacementParameter->m_InitialPlacementRange,
              1000.0e+2f,
              0.0e+2f,
              10.0e+2f,
              2);
  // Initial Placement/Forbidden Range
  FloatItem::Create(
              pPage,
              pHeap,
              L"Forbidden Range[cm]",
              &Parameter::Get().pGimmickPlacementParameter->m_InitialPlacementForbiddenRange,
              100.0e+2f,
              0.0e+2f,
              1.0e+2f,
              2);
}
// Add Page: Parameter/Player
void Menu::AddPageParameterPlayer(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Player",
              pParentPage);
  // Motion
  AddLabel(pPage, pHeap, L"Motion");
  // Motion/Glide Boundary
  FloatItem::Create(
              pPage,
              pHeap,
              L"Glide Boundary",
              &Parameter::Get().pPlayerParameter->m_GlideBoundary,
              0.00f,
              1.00f,
              0.01f,
              3);
  // Motion/Run Boundary
  FloatItem::Create(
              pPage,
              pHeap,
              L"Run Boundary",
              &Parameter::Get().pPlayerParameter->m_RunBoundary,
              0.00f,
              1.00f,
              0.01f,
              3);
  // Motion/Straight Boundary
  FloatItem::Create(
              pPage,
              pHeap,
              L"Straight Boundary",
              &Parameter::Get().pPlayerParameter->m_StraightBoundary,
              0.00f,
              2.00f,
              0.01f,
              3);
  // Elevation Angle
  AddLabel(pPage, pHeap, L"Elevation Angle");
  // Elevation Angle/Max
  RadianItem::Create(
              pPage,
              pHeap,
              L"Max",
              &Parameter::Get().pPlayerParameter->m_ElevationAngleMax,
              0.0f,
              90.0f,
              1.0f,
              2);
  // Elevation Angle/Min
  RadianItem::Create(
              pPage,
              pHeap,
              L"Min",
              &Parameter::Get().pPlayerParameter->m_ElevationAngleMin,
              -90.0f,
              0.0f,
              1.0f,
              2);
  // Elevation Angle/Change Rate
  FloatItem::Create(
              pPage,
              pHeap,
              L"Change Rate",
              &Parameter::Get().pPlayerParameter->m_ElevationAngleChangeRate,
              0.00f,
              1.00f,
              0.01f,
              4);
  // Effect
  AddLabel(pPage, pHeap, L"Effect");
  // Effect/High Speed Trajectory Boundary
  FloatItem::Create(
              pPage,
              pHeap,
              L"High Speed Trajectory Boundary",
              &Parameter::Get().pPlayerParameter->m_HighSpeedTrajectoryBoundary,
              0.00f,
              2.00f,
              0.05f,
              3);
  // Damage
  AddLabel(pPage, pHeap, L"Damage");
  // Damage/Recovery Frames
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Recovery Frames",
              &Parameter::Get().pPlayerParameter->m_DamageRecoveryFrames,
              0,
              60,
              1);
  // Damage/Motion Freeze Frames
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"MotionFreeze Frames",
              &Parameter::Get().pPlayerParameter->m_DamageMotionFreezeFrames,
              0,
              60,
              1);
  // Energy Aura
  AddLabel(pPage, pHeap, L"Energy Aura");
  // Energy Aura/Min Scale
  FloatItem::Create(
              pPage,
              pHeap,
              L"Min Scale",
              &Parameter::Get().pPlayerParameter->m_EnergyAuraMinScale,
              0.1f,
              1.0f,
              0.05f,
              4);
  // Energy Aura/Max Scale
  FloatItem::Create(
              pPage,
              pHeap,
              L"Max Scale",
              &Parameter::Get().pPlayerParameter->m_EnergyAuraMaxScale,
              1.0f,
              2.5f,
              0.05f,
              4);
}
// Add Page: Parameter/Motion Brend Frames
void Menu::AddPageParameterMotionBrendFrames(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Motion Brend Frames",
              pParentPage);
  // Sol
  CreateMotionBrendFramesPage(
              pPage,
              pHeap,
              L"Sol",
              &Parameter::Get().pPlayerParameter->m_MotionBrendFramesSol);
  // Luna
  CreateMotionBrendFramesPage(
              pPage,
              pHeap,
              L"Luna",
              &Parameter::Get().pPlayerParameter->m_MotionBrendFramesLuna);
}
// Add Page: Parameter/Beginning
void Menu::AddPageParameterBeginning(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Beginning",
              pParentPage);
  // Movement
  AddLabel(pPage, pHeap, L"Movement");
  // Movement/Distance
  FloatItem::Create(
              pPage,
              pHeap,
              L"Distance[cm]",
              &Parameter::Get().pRunParameter->m_BeginningDistance,
              0.0f,
              1000.0e+2f,
              1.0e+2f,
              2);
  // Number of Frames
  AddLabel(pPage, pHeap, L"Number of Frames");
  // Number of Frames/Entry
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Entry",
              &Parameter::Get().pCameraParameter->m_BeginningEntryFrames,
              2,
              100,
              1);
  // Number of Frames/Entry End
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Entry End",
              &Parameter::Get().pCameraParameter->m_BeginningEntryEndFrames,
              1,
              100,
              1);
  // Number of Frames/Overtaken
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Overtaken",
              &Parameter::Get().pCameraParameter->m_BeginningOvertakenFrames,
              2,
              200,
              1);
  // Number of Frames/ZoomIn
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Zoom In",
              &Parameter::Get().pCameraParameter->m_BeginningZoomInFrames,
              2,
              100,
              1);
  // Overtaken
  AddLabel(pPage, pHeap, L"Overtaken");
  // Overtaken/Angle Increasing Factor
  FloatItem::Create(
              pPage,
              pHeap,
              L"Angle Increasing Rate",
              &Parameter::Get().pCameraParameter->m_BeginningOvertakenAngleIncreasingRate,
              0.0f,
              10.0f,
              0.1f,
              3);
  // Overtaken/Radius Increasing Factor
  FloatItem::Create(
              pPage,
              pHeap,
              L"Radius Increasing Rate",
              &Parameter::Get().pCameraParameter->m_BeginningOvertakenRadiusIncreasingRate,
              0.0f,
              10.0f,
              0.1f,
              3);
}
// Add Page: Parameter/Ending
void Menu::AddPageParameterEnding(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Ending",
              pParentPage);
  // Entry
  AddLabel(pPage, pHeap, L"Entry");
  // Entry
  FloatItem::Create(
              pPage,
              pHeap,
              L"Player Scale",
              &Parameter::Get().pRunParameter->m_EndingEntryEndPlayerScale,
              0.00f,
              1.00f,
              0.01f,
              3);
  // Trajectory
  AddLabel(pPage, pHeap, L"Trajectory");
  // Trajectory/Frames
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Frames",
              &Parameter::Get().pRunParameter->m_EndingTrajectoryFrames,
              0,
              70,
              1);
  // Trajectory/Player Scale Change Rate
  FloatItem::Create(
              pPage,
              pHeap,
              L"Player Scale Change Rate",
              &Parameter::Get().pRunParameter->m_EndingTrajectoryPlayerScaleChangeRate,
              0.0f,
              1.0f,
              0.01f,
              3);
  // Trajectory/Distortion Scale Change Rate
  FloatItem::Create(
              pPage,
              pHeap,
              L"Distortion Scale Change Rate",
              &Parameter::Get().pGimmickParameter->m_DistortionCollapseScaleChangeRate,
              0.00f,
              1.00f,
              0.01f,
              3);
  // Effect
  AddLabel(pPage, pHeap, L"Effect");
  // Effect/Hit Effect Offset
  FloatItem::Create(
              pPage,
              pHeap,
              L"Hit Effect Offset[cm]",
              &Parameter::Get().pRunParameter->m_EndingHitEffectOffset,
              0.0f,
              500.0f,
              10.0f,
              2);
  // Effect/Hit02 Effect Scale
  FloatItem::Create(
              pPage,
              pHeap,
              L"Hit Effect Offset[cm]",
              &Parameter::Get().pRunParameter->m_EndingHit02EffectScale,
              0.0f,
              10.0f,
              0.1f,
              3);
}
// Add Page: Parameter/Sound
void Menu::AddPageParameterSound(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Sound",
              pParentPage);
  // Pokemon Voice Volume
  FloatItem::Create(
              pPage,
              pHeap,
              L"Pokemon Voice Volume",
              &Parameter::Get().pSoundParameter->m_PokemonVoiceVolume,
              0.0f,
              2.0f,
              0.05f,
              3);
  // Volume Control for Z Ring
  AddLabel(pPage, pHeap, L"Volume Control for Z Ring");
  // Activate Energy Aura
  AddVolumeControlParameterItems(
              pPage,
              pHeap,
              L"Activate Energy Aura",
              &Parameter::Get().pSoundParameter->m_VolumeControlActivateEnergyAura);
  // Hit Distortion
  AddVolumeControlParameterItems(
              pPage,
              pHeap,
              L"Hit Distortion",
              &Parameter::Get().pSoundParameter->m_VolumeControlHitDistortion);
}
// Add Page: Parameter/Translation
void Menu::AddPageParameterTranslation(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Translation",
              pParentPage);
  // Manual Trigger
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Manual Trigger[L Button]",
              &Parameter::Get().isTranslationTriggerEnable);
  // Threshold
  FloatItem::Create(
              pPage,
              pHeap,
              L"Threshold[cm]",
              &Parameter::Get().pRunParameter->m_TranslationThreshold,
              1.0e+4f,
              1.0e+5f,
              5.0e+3f,
              2);
  // Wait Frame
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Wait Frame",
              &Parameter::Get().pRunParameter->m_TranslationWaitFrame,
              0,
              60,
              1);
}
// Add Page: Model
void Menu::AddPageModel(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"Model",
              pParentPage);
  // Visualize SkyBox
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize SkyBox",
              &Parameter::Get().modelVisualizeSkyBox);
  // Visualize Rail
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize Rail",
              &Parameter::Get().modelVisualizeRail);
  // Visualize Initial Rail
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize Initial Rail",
              &Parameter::Get().modelVisualizeInitialRail);
  // Visualize Hero
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize Hero",
              &Parameter::Get().modelVisualizeHero);
  // Visualize Pokemon
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize Pokemon",
              &Parameter::Get().modelVisualizePokemon);
  // Visualize Energy Aura
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize Energy Aura",
              &Parameter::Get().modelVisualizeEnergyAura);
  // Visualize Energy
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize Energy",
              &Parameter::Get().modelVisualizeEnergy);
  // Visualize Obstacle
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize Obstacle",
              &Parameter::Get().modelVisualizeObstacle);
  // Visualize Distortion
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize Distortion",
              &Parameter::Get().modelVisualizeDistortion);
  // Visualize Distortion Vortex
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Visualize Distortion Vortex",
              &Parameter::Get().modelVisualizeDistortionVortex);
}
// Add Page: DrawUtil
void Menu::AddPageDrawUtil(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap)
{
  // create page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              L"DrawUtil",
              pParentPage);
  // Active
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Active",
              &Parameter::Get().pDrawUtil->m_Setting.active);
  // Input Point
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Input Point",
              &Parameter::Get().pDrawUtil->m_Setting.input_point);
  // Present Point
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Present Point",
              &Parameter::Get().pDrawUtil->m_Setting.present_point);
  // Collision Point
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Collision Point",
              &Parameter::Get().pDrawUtil->m_Setting.collision_point);
  // Rail Section
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Rail Section",
              &Parameter::Get().pDrawUtil->m_Setting.rail_section);
  // Gimmick Energy Collision
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Gimmick Energy Collision",
              &Parameter::Get().pDrawUtil->m_Setting.gimmick_energy_collision);
  // Gimmick Obstacle Collision
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Gimmick Obstacle Collision",
              &Parameter::Get().pDrawUtil->m_Setting.gimmick_obstacle_collision);
  // Gimmick Distortion Collision
  gfl2::debug::DebugWin_AddItemValueBool(
              pPage,
              pHeap,
              L"Gimmick Distortion Collision",
              &Parameter::Get().pDrawUtil->m_Setting.gimmick_distortion_collision);
}
// static function
// Reset Parameter
static void ResetParameter(
          void* /*pWork*/,
          gfl2::debug::DebugWinItem* /*pItem*/)
{
  const gfl2::ui::Button* pButton =
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)
          ->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    GFL_PRINT("Reset All Parameters\n");
    Parameter::Get().pCameraParameter->Reset();
    Parameter::Get().pGimmickParameter->Reset();
    Parameter::Get().pGimmickPlacementParameter->Reset();
    Parameter::Get().pPlayerParameter->Reset();
    Parameter::Get().pRunParameter->Reset();
  }
}
// Draw Name
static wchar_t* DrawName(
            void* /*pWork*/,
            gfl2::debug::DebugWinItem* pItem)
{
  wchar_t* const workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           L"%ls",
           pItem->GetName());
  return workStr;
}
// Draw Label
static wchar_t* DrawLabel(
            void* /*pWork*/,
            gfl2::debug::DebugWinItem* pItem)
{
  wchar_t* const workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           L"== %ls ==",
           pItem->GetName());
  return workStr;
}
// Draw f32
static wchar_t* DrawF32(
            void* pValue,
            gfl2::debug::DebugWinItem* pItem)
{
  wchar_t* const workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           L"%ls: %+.5f",
           pItem->GetName(),
           *reinterpret_cast<const f32*>(pValue));
  return workStr;
}
// Draw gfl2::math::Vector2
static wchar_t* DrawVector2(
            void* pValue,
            gfl2::debug::DebugWinItem* pItem)
{
  wchar_t* const workStr = gfl2::debug::DebugWin_GetWorkStr();
  const gfl2::math::Vector2& value = *reinterpret_cast<gfl2::math::Vector2*>(pValue);
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           L"%ls: x%+.5f y%+.5f",
           pItem->GetName(),
           value.GetX(),
           value.GetY());
  return workStr;
}
// Draw gfl2::math::Vector3
static wchar_t* DrawVector3(
            void* pValue,
            gfl2::debug::DebugWinItem* pItem)
{
  wchar_t* const workStr = gfl2::debug::DebugWin_GetWorkStr();
  const gfl2::math::Vector3& value = *reinterpret_cast<gfl2::math::Vector3*>(pValue);
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           L"%ls: x%+.5f y%+.5f z%+.5f",
           pItem->GetName(),
           value.GetX(),
           value.GetY(),
           value.GetZ());
  return workStr;
}
// Draw Timer
static wchar_t* DrawTimer(
            void* pTimer,
           gfl2::debug::DebugWinItem* pItem)
{
  wchar_t* const workStr = gfl2::debug::DebugWin_GetWorkStr();
  const Timer& timer = *reinterpret_cast<const Timer*>(pTimer);
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           L"%ls: %ls",
           pItem->GetName(),
           timer.ElapsedTimeStr());
  return workStr;
}
// create Motion Brend Frames Page
static void CreateMotionBrendFramesPage(
            gfl2::debug::DebugWinGroup* pParentPage,
            gfl2::heap::HeapBase* pHeap,
            wchar_t* name,
            Player::MotionBrendFrames* pTarget)
{
  // create Page
  gfl2::debug::DebugWinGroup* const pPage = gfl2::debug::DebugWin_AddGroup(
              pHeap,
              name,
              pParentPage);
  // Reset End
  AddLabel(pPage, pHeap, L"Reset End");
  // Reset End Walk
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Reset End Walk",
              &pTarget->reset_end_walk,
              0,
              60,
              1);
  // Reset End Run
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Reset End Run",
              &pTarget->reset_end_run,
              0,
              60,
              1);
  // Reset End Glide
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Reset End Glide",
              &pTarget->reset_end_glide,
              0,
              60,
              1);
  // Reset Begin
  AddLabel(pPage, pHeap, L"Reset Begin");
  // Reset Begin Walk
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Reset Begin Walk",
              &pTarget->reset_begin_walk,
              0,
              60,
              1);
  // Reset Begin Run
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Reset Begin Run",
              &pTarget->reset_begin_run,
              0,
              60,
              1);
  // Reset Begin Glide
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Reset Begin Glide",
              &pTarget->reset_begin_glide,
              0,
              60,
              1);
  // Direction Change
  AddLabel(pPage, pHeap, L"Direction Change");
  // Direction Change Walk
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Direction Change Walk",
              &pTarget->direction_change_walk,
              0,
              60,
              1);
  // Direction Change Run
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Direction Change Run",
              &pTarget->direction_change_run,
              0,
              60,
              1);
  // Direction Change Glide
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Direction Change Glide",
              &pTarget->direction_change_glide,
              0,
              60,
              1);
  // Speed Change
  AddLabel(pPage, pHeap, L"Speed Change");
  // Walk to Run
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Walk to Run",
              &pTarget->walk_to_run,
              0,
              60,
              1);
  // Walk to Glide
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Walk to Glide",
              &pTarget->walk_to_glide,
              0,
              60,
              1);
  // Run to Walk
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Run to Walk",
              &pTarget->run_to_walk,
              0,
              60,
              1);
  // Run to Glide
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Run to Glide",
              &pTarget->run_to_glide,
              0,
              60,
              1);
  // Glide to Walk
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Glide to Walk",
              &pTarget->glide_to_walk,
              0,
              60,
              1);
  // Glide to Run
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Glide to Run",
              &pTarget->glide_to_run,
              0,
              60,
              1);
  // Turn End
  AddLabel(pPage, pHeap, L"Turn End");
  // Turn End Walk
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Turn End Walk",
              &pTarget->turn_end_walk,
              0,
              60,
              1);
  // Turn End Run
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Turn End Run",
              &pTarget->turn_end_run,
              0,
              60,
              1);
  // Turn End Glide
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Turn End Glide",
              &pTarget->turn_end_glide,
              0,
              60,
              1);
}
// Add Volume Control Parameter Items
static void AddVolumeControlParameterItems(
            gfl2::debug::DebugWinGroup* pPage,
            gfl2::heap::HeapBase* pHeap,
            wchar_t* name,
            Sound::VolumeControlParameter* pTarget)
{
  // Name
  AddLabel(pPage, pHeap, name);
  // BGM Volume
  FloatItem::Create(
              pPage,
              pHeap,
              L"BGM Volume",
              &pTarget->bgm_volume,
              0.00f,
              1.00f,
              0.05f,
              3);
  // Voice Volume
  FloatItem::Create(
              pPage,
              pHeap,
              L"Voice Volume",
              &pTarget->voice_volume,
              0.00f,
              1.00f,
              0.05f,
              3);
  // Down Frames
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Down Frames",
              &pTarget->down_frames,
              0,
              30,
              1);
  // Keep Frames
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Keep Frames",
              &pTarget->keep_frames,
              0,
              30,
              1);
  // Up Frames
  gfl2::debug::DebugWin_AddItemValueU32(
              pPage,
              pHeap,
              L"Up Frames",
              &pTarget->up_frames,
              0,
              30,
              1);
}
// DebugWinItemValueFloat with precision
FloatItem::FloatItem(
            gfl2::debug::DebugWinGroup* pParent,
            const wchar_t* name,
            f32* pTarget,
            const f32& min,
            const f32& max,
            const f32& step,
            const u32& precision)
    : gfl2::debug::DebugWinItemValue<f32>(
                  pParent,
                  name,
                  pTarget,
                  min,
                  max,
                  step),
      m_Precision(precision){}
const wchar_t* FloatItem::GetDispName(
            gfl2::debug::DebugWin *debSys)
{
  wchar_t formatStr[FORMAT_STR_SIZE];
  swprintf(formatStr,
           FORMAT_STR_SIZE,
           L"%%ls [%%+.%uf]",
           m_Precision);
  wchar_t* const workStr = gfl2::debug::DebugWinItemValue<f32>::GetSysWorkStr();
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           formatStr,
           gfl2::debug::DebugWinItemValue<f32>::GetName(),
           *gfl2::debug::DebugWinItemValue<f32>::mTarget);
  return workStr;
}
const u32& FloatItem::Precision() const
{
  return m_Precision;
}
void FloatItem::Create(
            gfl2::debug::DebugWinGroup* pParent,
            gfl2::heap::HeapBase* pHeap,
            const wchar_t* name,
            f32* pTarget,
            const f32& min,
            const f32& max,
            const f32& step,
            const u32& precision)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pTarget != NULL);
  FloatItem* const pItem = GFL_NEW(pHeap) FloatItem(
              pParent,
              name,
              pTarget,
              min,
              max,
              step,
              precision);
  LUNA_SOL_DIVE_ASSERT(pItem != NULL);
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->AddItem(
              pItem,
              pParent);
}
// DebugWinItemValueFloat with precision (radian ver)
RadianItem::RadianItem(
            gfl2::debug::DebugWinGroup* pParent,
            const wchar_t* name,
            f32* pTarget,
            const f32& minDegree,
            const f32& maxDegree,
            const f32& stepDegree,
            const u32& precision)
    : FloatItem(
                  pParent,
                  name,
                  pTarget,
                  gfl2::math::ConvDegToRad(minDegree),
                  gfl2::math::ConvDegToRad(maxDegree),
                  gfl2::math::ConvDegToRad(stepDegree),
                  precision){}
const wchar_t* RadianItem::GetDispName(
            gfl2::debug::DebugWin *debSys)
{
  wchar_t formatStr[FORMAT_STR_SIZE];
  swprintf(formatStr,
           FORMAT_STR_SIZE,
           L"%%ls [%%+.%uf]",
           Precision());
  wchar_t* const workStr = gfl2::debug::DebugWinItemValue<f32>::GetSysWorkStr();
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           formatStr,
           gfl2::debug::DebugWinItemValue<f32>::GetName(),
           gfl2::math::ConvRadToDeg(
                   *gfl2::debug::DebugWinItemValue<f32>::mTarget));
  return workStr;
}
void RadianItem::Create(
            gfl2::debug::DebugWinGroup* pParent,
            gfl2::heap::HeapBase* pHeap,
            const wchar_t* name,
            f32* pTarget,
            const f32& min,
            const f32& max,
            const f32& step,
            const u32& precision)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pTarget != NULL);
  RadianItem* const pItem = GFL_NEW(pHeap) RadianItem(
              pParent,
              name,
              pTarget,
              min,
              max,
              step,
              precision);
  LUNA_SOL_DIVE_ASSERT(pItem != NULL);
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->AddItem(
              pItem,
              pParent);
}
// DebugWinItemValueU32 modified
U32Item::U32Item(
            gfl2::debug::DebugWinGroup* pParent,
            const wchar_t* name,
            u32* pTarget,
            const u32& min,
            const u32& max,
            const u32& step)
    : gfl2::debug::DebugWinItem(pParent),
      m_pTarget(pTarget),
      m_Min(min),
      m_Max(max),
      m_Step(step)
{
  SetName(name);
}
void U32Item::UpdateFunc(
            gfl2::debug::DebugWin* pDebugSystem)
{
  const gfl2::ui::Button* const pButton = GetSysUiButton();
  u32 step = m_Step;
  if(pButton->IsHold(gfl2::ui::BUTTON_L))
  {
    step *= 100;
  }
  if(pButton->IsHold(gfl2::ui::BUTTON_R))
  {
    step *= 10;
  }
  const s32 sign =
          pButton->IsRepeat(gfl2::ui::BUTTON_LEFT)
          ? -1
          : pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT)
          ? 1
          : 0;
  if(sign != 0)
  {
    if(*m_pTarget == m_Min && sign == -1)
    {
      *m_pTarget = m_Max;
    }
    else if(*m_pTarget == m_Max && sign == 1)
    {
      *m_pTarget = m_Min;
    }
    else
    {
      *m_pTarget = gfl2::math::clamp(
                  *m_pTarget + sign * step,
                  m_Min,
                  m_Max);
    }
    pDebugSystem->SoundCallBack(gfl2::debug::SE_CHANGE_VALUE);
    ChangedValue();
  }
}
const wchar_t* U32Item::GetDispName(
            gfl2::debug::DebugWin* pDebugSystem)
{
  wchar_t* const workStr = gfl2::debug::DebugWinItem::GetSysWorkStr();
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           L"%ls [%u]",
           gfl2::debug::DebugWinItem::GetName(),
           *m_pTarget);
  return workStr;
}
void U32Item::Create(
            gfl2::debug::DebugWinGroup* pParent,
            gfl2::heap::HeapBase* pHeap,
            const wchar_t* name,
            u32* pTarget,
            const u32& min,
            const u32& max,
            const u32& step)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pTarget != NULL);
  U32Item* const pItem = GFL_NEW(pHeap) U32Item(
              pParent,
              name,
              pTarget,
              min,
              max,
              step);
  LUNA_SOL_DIVE_ASSERT(pItem != NULL);
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->AddItem(
              pItem,
              pParent);
}
// DistortionType Item
DistortionTypeItem::DistortionTypeItem(
            gfl2::debug::DebugWinGroup* pParent,
            const wchar_t* name,
            DistortionType* pTarget)
    : gfl2::debug::DebugWinItemValue<u8>(
                  pParent,
                  name,
                  reinterpret_cast<u8*>(pTarget),
                  DISTORTION_TYPE__UB,
                  DISTORTION_TYPE__PLAINS,
                  1){}
const wchar_t* DistortionTypeItem::GetDispName(
            gfl2::debug::DebugWin *debSys)
{
  wchar_t* const workStr = gfl2::debug::DebugWinItemValue<u8>::GetSysWorkStr();
  const DistortionType type = static_cast<DistortionType>(
              *gfl2::debug::DebugWinItemValue<u8>::mTarget);
  swprintf(workStr,
           gfl2::debug::DEBUGWIN_NAME_LEN,
           L"%ls [%ls]",
           gfl2::debug::DebugWinItemValue<u8>::GetName(),
           (type == DISTORTION_TYPE__UB)? L"UB":
           (type == DISTORTION_TYPE__CLIFF)? L"崖":
           (type == DISTORTION_TYPE__WATER)? L"水":
           (type == DISTORTION_TYPE__CAVE)? L"洞窟":
           (type == DISTORTION_TYPE__PLAINS)? L"平地":
           L"Error");
  return workStr;
}
void DistortionTypeItem::Create(
            gfl2::debug::DebugWinGroup* pParent,
            gfl2::heap::HeapBase* pHeap,
            const wchar_t* name,
            DistortionType* pTarget)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pTarget != NULL);
  DistortionTypeItem* const pItem = GFL_NEW(pHeap) DistortionTypeItem(
              pParent,
              name,
              pTarget);
  LUNA_SOL_DIVE_ASSERT(pItem != NULL);
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->AddItem(
              pItem,
              pParent);
}
// Add Label
static void AddLabel(
            gfl2::debug::DebugWinGroup* pParent,
            gfl2::heap::HeapBase* pHeap,
            const wchar_t* name)
{
  LUNA_SOL_DIVE_ASSERT(pParent != NULL);
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  gfl2::debug::DebugWin_AddItemUpdateFunc(
              pParent,
              pHeap,
              name,
              NULL,
              NULL,
              DrawLabel);
}
}// namespace Debug
}// namespace LunaSolDive
#endif// PM_DEBUG
