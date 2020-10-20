//======================================================================
/**
 * @file Player_Manager.cpp
 * @date 2016/11/29 12:15:14
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <math/include/gfl2_Quaternion.h>
#include <math/include/gfl2_MathTriangular.h>
#include <model/include/gfl2_CharaModelFactory.h>
#include "./EnergyAura.h"
#include "./Hero.h"
#include "./Manager.h"
#include "./MotionController.h"
#include "./MotionStorage.h"
#include "./Parameter.h"
#include "./Pokemon.h"
#include "../Assert.h"
#include "../Effect/Manager.h"
#include "../Effect/Particle.h"
#include "../Run/Aim.h"
#include "../Run/Energy.h"
#include "arc_def_index/arc_def.h"
#include "arc_index/LunaSolDiveEffect.gaix"

namespace LunaSolDive{
namespace Player{
// static function
// get Elevation Angle
static f32 ToElevationAngle(
            const Run::Aim& aim);
// get Motion Speed
static MotionSpeed ToMotionSpeed(
            const Run::Energy& energy);
// get Motion Direction
static MotionDirection ToMotionDirection(
            const Run::Aim& aim);
// constructor
Manager::Manager()
    : m_Version(VERSION__NONE),
      m_Sex(HERO_SEX__NONE),
      m_pCharaModelFactory(NULL),
      m_pEffectManager(NULL),
      m_pMotionStorage(NULL),
      m_pMotionController(NULL),
      m_pHero(NULL),
      m_pPokemon(NULL),
      m_pEnergyAura(NULL),
      m_pEffectPower(NULL),
      m_pEffectDamage(NULL),
      m_IsDamaged(false),
      m_DamageFrameCount(0),
      m_Energy(0.0f){}
// Initialize
void Manager::Initialize(
            const Version& version,
            const HeroSex& sex,
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline,
            Effect::Manager* pEffectManager)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  // set parameter
  m_Version = version;
  LUNA_SOL_DIVE_ASSERT(m_Version != VERSION__NONE);
  m_Sex = sex;
  LUNA_SOL_DIVE_ASSERT(m_Sex != HERO_SEX__NONE);
  m_pEffectManager = pEffectManager;
  LUNA_SOL_DIVE_ASSERT(m_pEffectManager != NULL);
  // initialize CharaModelFactory
  m_pCharaModelFactory = GFL_NEW(pHeap) poke_3d::model::CharaModelFactory();
  LUNA_SOL_DIVE_ASSERT(m_pCharaModelFactory != NULL);
  // initialize MotionStorage
  m_pMotionStorage = GFL_NEW(pHeap) MotionStorage();
  LUNA_SOL_DIVE_ASSERT(m_pMotionStorage != NULL);
  m_pMotionStorage->Initialize(
              m_Version,
              m_Sex,
              pHeap);
  // initialize MotionController
  m_pMotionController = GFL_NEW(pHeap) MotionController();
  LUNA_SOL_DIVE_ASSERT(m_pMotionController != NULL);
  m_pMotionController->Initialize(
              m_Version,
              m_pMotionStorage);
  // initialize Hero
  m_pHero = GFL_NEW(pHeap) Hero();
  LUNA_SOL_DIVE_ASSERT(m_pHero != NULL);
  m_pHero->Initialize(
              m_Sex,
              pHeap,
              pRenderingPipeline,
              m_pMotionController);
  // initialize Pokemon
  m_pPokemon = GFL_NEW(pHeap) Pokemon();
  LUNA_SOL_DIVE_ASSERT(m_pPokemon != NULL);
  m_pPokemon->Initialize(
              m_Version,
              pHeap,
              pRenderingPipeline,
              m_pEffectManager,
              m_pMotionController);
  // initialize Energy Aura
  m_pEnergyAura = GFL_NEW(pHeap) EnergyAura();
  LUNA_SOL_DIVE_ASSERT(m_pEnergyAura != NULL);
  m_pEnergyAura->Initialize(
              pHeap,
              pRenderingPipeline,
              pEffectManager);
}
// Terminate
void Manager::Terminate()
{
  // delete MotionController
  if(m_pMotionController != NULL)
  {
    m_pMotionController->Terminate();
    GFL_DELETE(m_pMotionController);
    m_pMotionController = NULL;
  }
  // delete Energy Aura
  if(m_pEnergyAura != NULL)
  {
    m_pEnergyAura->Terminate();
    GFL_DELETE(m_pEnergyAura);
    m_pEnergyAura = NULL;
  }
  // delet Hero
  if(m_pHero != NULL)
  {
    m_pHero->Terminate();
    GFL_DELETE(m_pHero);
    m_pHero = NULL;
  }
  // delete Pokemon
  if(m_pPokemon != NULL)
  {
    m_pPokemon->Terminate();
    GFL_DELETE(m_pPokemon);
    m_pPokemon = NULL;
  }
  // delete CharaModelFactory
  if(m_pCharaModelFactory != NULL)
  {
    m_pCharaModelFactory->Finalize();
    GFL_DELETE(m_pCharaModelFactory);
    m_pCharaModelFactory = NULL;
  }
  // delete MotionStorage
  if(m_pMotionStorage != NULL)
  {
    m_pMotionStorage->Terminate();
    GFL_DELETE(m_pMotionStorage);
    m_pMotionStorage = NULL;
  }
  // delete Effect
  GFL_SAFE_DELETE(m_pEffectPower);
  GFL_SAFE_DELETE(m_pEffectDamage);
  // reset parameter
  m_Version = VERSION__NONE;
  m_Sex = HERO_SEX__NONE;
  m_IsDamaged = false;
  m_DamageFrameCount = 0;
  m_Energy = 0.0f;
}
// Create
void Manager::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator,
            gfl2::fs::AsyncFileManager* pFileManager,
            const poke_3d::model::DressUpParam& dressUpParameter)
{
  // initialize CharaModelFactory
  m_pCharaModelFactory->Initialize(
              pFileManager,
              pHeap,
              ARCID_CHARA_MODEL_FIELD_NORMAL);
  // create Motion
  m_pMotionStorage->Create(pAllocator);
  // crate Hero
  m_pHero->Create(
              pHeap,
              pAllocator,
              pFileManager,
              m_pCharaModelFactory,
              dressUpParameter);
  // create Pokemon
  m_pPokemon->Create(
              pHeap,
              pAllocator,
              m_pCharaModelFactory);
  // create Energy Aura
  m_pEnergyAura->Create(
              pHeap,
              pAllocator);
  // set Initial Motion
  m_pMotionController->SetInitialMotion();
}
// Update at the Beginning stage
void Manager::UpdateBeginning(
            const Rail::Directions& location)
{
  // update location
  m_Location = location;
  // update motion
  m_pMotionController->Update(
              MOTION_SPEED__RUN,
              MOTION_DIRECTION__STRAIGHT,
              Run::RESET_SIGNAL__NONE,
              0.0f);
  // update common
  UpdateCommon();
}
// Update at the Main stage
void Manager::UpdateMain(
            const Rail::Directions& location,
            const Run::Energy& energy,
            const Run::Aim& aim,
            const Run::ResetSignal& reset)
{
  // update location
  m_Location = location;
  // damage
  if(m_IsDamaged)
  {
    ++m_DamageFrameCount;
    if(m_DamageFrameCount > Parameter::Get().DamageRecoveryFrames())
    {
      PowerDown();
      m_pEnergyAura->UpdateEnergy(m_Energy);
      m_IsDamaged = false;
    }
  }
  // update motion
  if(! m_IsDamaged
          || m_DamageFrameCount > Parameter::Get().DamageMotionFreezeFrames())
  {
    m_pMotionController->Update(
                ToMotionSpeed(energy),
                ToMotionDirection(aim),
                reset,
                ToElevationAngle(aim));
  }
  // update common
  UpdateCommon();
}
// Update at the Ending stage
void Manager::UpdateEnding(
            const Rail::Directions& location,
            const f32& scaleChangeRate)
{
  // update location
  m_Location = location;
  // damage
  if(m_IsDamaged)
  {
    ++m_DamageFrameCount;
    if(m_DamageFrameCount > Parameter::Get().DamageMotionFreezeFrames())
    {
      m_IsDamaged = false;
    }
  }
  // update motion
  if(! m_IsDamaged)
  {
    m_pMotionController->Update(
                m_pMotionController->Speed(),
                MOTION_DIRECTION__STRAIGHT,
                Run::RESET_SIGNAL__NONE,
                0.0f);
  }
  // scale down
  if(m_pHero->IsVisible())
  {
    const f32 scale = m_pHero->GetScale() * scaleChangeRate;
    m_pHero->SetScale(scale);
    m_pPokemon->SetScale(scale);
    if(scale < 0.1f)
    {
      m_pHero->NonVisualize();
      m_pPokemon->NonVisualize();
    }
  }
  // update common
  UpdateCommon();
}
// Update: Common
void Manager::UpdateCommon()
{
  // update Hero
  m_pHero->Update(
              m_Location,
              m_pMotionController->ElevationAngle());
  // update Pokemon
  m_pPokemon->Update(
              m_Location,
              m_pHero->Offset(),
              m_pMotionController->ElevationAngle());
  // update power up/down effect
  if(m_pEffectPower != NULL
          && m_pEffectPower->IsAlive())
  {
    m_pEffectPower->Update(
                m_pPokemon->Waist().Point(),
                m_pPokemon->Waist().RotationQuaternion(),
                m_pPokemon->GetScale());
  }
  // update damage effect
  if(m_pEffectDamage != NULL
          && m_pEffectDamage->IsAlive())
  {
    m_pEffectDamage->Update(
                m_pPokemon->Waist().Point(),
                m_pPokemon->Waist().RotationQuaternion(),
                m_pPokemon->GetScale());
  }
}
// Power Up
void Manager::PowerUp(
            const bool& isEnergyOverLimit)
{
  // create power up effect
  GFL_SAFE_DELETE(m_pEffectPower);
  m_pEffectPower = m_pEffectManager->Create(
              GARC_LunaSolDiveEffect_ea_dive_powerup_lz_BPTCL);
  if(m_pEffectPower != NULL)
  {
    m_pEffectPower->Start();
    m_pEffectPower->Update(
                m_pPokemon->Waist().Point(),
                m_pPokemon->Waist().RotationQuaternion(),
                1.0f);
  }
  // energy aura up
  m_pEnergyAura->Up(
              isEnergyOverLimit);
}
// Power Down
void Manager::PowerDown()
{
  // create power down effect
  GFL_SAFE_DELETE(m_pEffectPower);
  m_pEffectPower = m_pEffectManager->Create(
              GARC_LunaSolDiveEffect_ea_dive_powerdown_lz_BPTCL);
  if(m_pEffectPower != NULL)
  {
    m_pEffectPower->Start();
    m_pEffectPower->Update(
                m_pPokemon->Waist().Point(),
                m_pPokemon->Waist().RotationQuaternion(),
                1.0f);
  }
  // energy aura down
  m_pEnergyAura->Down();
}
// Damage
void Manager::Damage()
{
  // update flag
  m_IsDamaged = true;
  m_DamageFrameCount = 0;
  // start damage effect
  GFL_SAFE_DELETE(m_pEffectDamage);
  m_pEffectDamage = m_pEffectManager->Create(
              GARC_LunaSolDiveEffect_ea_dive_hurdle_hit_lz_BPTCL);
  if(m_pEffectDamage != NULL)
  {
    m_pEffectDamage->Start();
    m_pEffectDamage->Update(
                m_pPokemon->Waist().Point(),
                m_pPokemon->Waist().RotationQuaternion(),
                1.0f);
  }
}
// Whether the player is dameged or not
const bool& Manager::IsDamaged() const
{
  return m_IsDamaged;
}
// Activate EnergyAura
void Manager::ActivateEnergyAura()
{
  m_pEnergyAura->Activate();
}
// Update EnergyAura
void Manager::UpdateEnergyAura(
            const gfl2::math::Vector3& cameraLocation)
{
  // direction from camera to Pokkemon's waist
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(
              (m_pPokemon->Waist().Point() - cameraLocation).Length()));
  const gfl2::math::Vector3 forward =
          (m_pPokemon->Waist().Point() - cameraLocation).Normalize();
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(
              forward.Cross(m_pPokemon->Waist().Vertical()).Length()));
  const gfl2::math::Vector3 vertical =
          (m_pPokemon->Waist().Vertical()
                  - forward * forward.Dot(m_pPokemon->Waist().Vertical())
          ).Normalize();
  const Rail::Directions cameraDirections(
              cameraLocation,
              forward,
              vertical);
  LUNA_SOL_DIVE_ASSERT(cameraDirections.IsValid());
  // diffrence from Pokemon's waist position
  const f32& forwardDifference =
          (m_Version == VERSION__SOL)
          ? Parameter::Get().EnergyAuraForwardSol()
          : Parameter::Get().EnergyAuraForwardLuna();
  const f32& verticalDifference =
          (m_Version == VERSION__SOL)
          ? Parameter::Get().EnergyAuraVerticalSol()
          : Parameter::Get().EnergyAuraVerticalLuna();
  // update energy aura
  m_pEnergyAura->Update(
              m_pPokemon->Waist().Point()
                      + cameraDirections.Forward() * forwardDifference
                      + cameraDirections.Vertical() * verticalDifference,
              cameraDirections.RotationQuaternion());
}
// Update Energy
void Manager::UpdateEnergy(
            const f32& energy)
{
  m_Energy = energy;
  if(! m_IsDamaged)
  {
    m_pPokemon->UpdateEnergy(
                energy);
    m_pEnergyAura->UpdateEnergy(
                energy);
  }
}
// change from Main to Ending
void Manager::ChangeMainToEnding()
{
  // vanish energy aura
  m_pEnergyAura->Vanish();
}
// Location
const Rail::Directions& Manager::Location() const
{
  return m_Location;
}
// Origin
Rail::Directions Manager::OriginLocation() const
{
  LUNA_SOL_DIVE_ASSERT(m_pHero != NULL);
  return Rail::Directions(
              m_Location.Point()+ m_pHero->Offset(),
              m_Location.Forward(),
              m_Location.Vertical());
}
// Hero: Spine2 joint
const Rail::Directions& Manager::HeroSpine2() const
{
  LUNA_SOL_DIVE_ASSERT(m_pHero != NULL);
  return m_pHero->Spine2();
}
// Scale
f32 Manager::Scale() const
{
  LUNA_SOL_DIVE_ASSERT(m_pHero != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pPokemon != NULL);
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_pHero->GetScale(), m_pPokemon->GetScale()));
  return m_pHero->GetScale();
}
// Translation
void Manager::Translation(
            const gfl2::math::Vector3& displacement)
{
  // hero
  m_pHero->Translation(displacement);
  // pokemon
  m_pPokemon->Translation(displacement);
  // energy aura
  m_pEnergyAura->Translation(displacement);
  // location
  m_Location.Translation(displacement);
  // translation of effects is executed by Effect::Manager
}
// static function
// get Elevation Angle
static f32 ToElevationAngle(
            const Run::Aim& aim)
{
  const f32 verticalMovingRate = aim.Input().GetY() - aim.Point().GetY();
  const f32& range =
          (verticalMovingRate > 0.0f)
          ? Parameter::Get().ElevationAngleMax()
          : Parameter::Get().ElevationAngleMin();
  return gfl2::math::Clamp(
              gfl2::math::FAbs(verticalMovingRate) * range,
              Parameter::Get().ElevationAngleMin(),
              Parameter::Get().ElevationAngleMax());
}
// get Motion Speed
static MotionSpeed ToMotionSpeed(
            const Run::Energy& energy)
{
  return (energy.Rate() > Parameter::Get().GlideBoundary())
          ? MOTION_SPEED__GLIDE
          : (energy.Rate() > Parameter::Get().RunBoundary())
          ? MOTION_SPEED__RUN
          : MOTION_SPEED__WALK;
}
// get Motion Direction
static MotionDirection ToMotionDirection(
            const Run::Aim& aim)
{
  const f32 horizontalMoveRate =
          (aim.Input().GetX() - aim.Point().GetX());
  return (gfl2::math::FAbs(horizontalMoveRate) < Parameter::Get().StraightBoundary())
          ? MOTION_DIRECTION__STRAIGHT
          : (horizontalMoveRate > 0.0f)
           ? MOTION_DIRECTION__RIGHT
           : MOTION_DIRECTION__LEFT;
}
#if PM_DEBUG
// Restart: change GameOver -> Main
void Manager::Restart()
{
  // visualize EnergyAura
  m_pEnergyAura->Visualize();
  // reset scale
  m_pHero->SetScale(1.0f);
  m_pPokemon->SetScale(1.0f);
  // visualize
  m_pHero->Visualize();
  m_pPokemon->Visualize();
}
#endif// PM_DEBUG
}// namespace Player
}// namespace LunaSolDive
