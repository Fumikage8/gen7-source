//======================================================================
/**
 * @file Player_Hero.cpp
 * @date 2016/12/02 12:51:57
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_CharaModelFactory.h>
#include <model/include/gfl2_DressUpModel.h>
#include "System/include/DressUp.h"
#include "System/include/nijiAllocator.h"
#include "System/include/PokemonVersion.h"
#include "./Hero.h"
#include "./MotionController.h"
#include "../Assert.h"
#include "../RenderingPipeline.h"
#include "niji_conv_header/field/chara/chara_model_id.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Player{
// constructor
Hero::Hero()
    : m_Sex(HERO_SEX__NONE),
      m_pRenderingPipeline(NULL),
      m_pModel(NULL),
      m_pDressUpManager(NULL),
      m_Offset(gfl2::math::Vector3::GetZero()),
      m_IsVisible(false),
      m_IsOffsetParameterSet(false)
{
  m_OffsetParameter.forward = 0.0f;
  m_OffsetParameter.vertical = 0.0f;
  m_OffsetParameter.horizontal = 0.0f;
}
// Initialize
void Hero::Initialize(
            const HeroSex& sex,
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline,
            MotionController* pMotionController)
{
  // set parameter
  m_Sex = sex;
  LUNA_SOL_DIVE_ASSERT(m_Sex != HERO_SEX__NONE);
  // set pointer
  m_pRenderingPipeline = pRenderingPipeline;
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  // initialize Model
  m_pModel = GFL_NEW(pHeap) poke_3d::model::DressUpModel();
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  m_pDressUpManager = GFL_NEW(pHeap) poke_3d::model::DressUpModelResourceManager();
  LUNA_SOL_DIVE_ASSERT(m_pDressUpManager != NULL);
  // register the model with the MotionController
  LUNA_SOL_DIVE_ASSERT(pMotionController != NULL);
  pMotionController->RegisterHeroModel(m_pModel);
}
// Terminate
void Hero::Terminate()
{
  if(m_pModel != NULL)
  {
    NonVisualize();
    m_pModel->Destroy();
    GFL_DELETE(m_pModel);
    m_pModel = NULL;
  }
  // delete DressUpModelResourceManager
  if(m_pDressUpManager != NULL)
  {
    m_pDressUpManager->Finalize();
    GFL_DELETE(m_pDressUpManager);
    m_pDressUpManager = NULL;
  }
  // reset parameter
  m_Sex = HERO_SEX__NONE;
  m_pRenderingPipeline = NULL;
  m_Spine2.Terminate();
  m_Offset = gfl2::math::Vector3::GetZero();
  m_OffsetParameter.forward = 0.0f;
  m_OffsetParameter.vertical = 0.0f;
  m_OffsetParameter.horizontal = 0.0f;
  m_IsOffsetParameterSet = false;
}
// Create
void Hero::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator,
            gfl2::fs::AsyncFileManager* pFileManager,
            poke_3d::model::CharaModelFactory* pCharaModelFactory,
            const poke_3d::model::DressUpParam& dressUpParameter)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  LUNA_SOL_DIVE_ASSERT(pFileManager != NULL);
  LUNA_SOL_DIVE_ASSERT(pCharaModelFactory != NULL);
  // create model
  const u32 charaID =
              m_Sex == HERO_SEX__MALE
              ? CHARA_MODEL_ID_PC0001_00
              : CHARA_MODEL_ID_PC0002_00;
  pCharaModelFactory->LoadModelSync(charaID);
  pCharaModelFactory->SetupModel(
              pAllocator,
              charaID);
  // dress up
  m_pDressUpManager->Initialize(
              pFileManager,
              pHeap,
              System::DressUp::GetDressUpArcIdListField());
  poke_3d::model::DressUpParam diveDressUpParameter = dressUpParameter;
#if PM_DEBUG
  if(Debug::Parameter::Get().isRandomDressUp)
  {
  System::DressUp::Debug_GetDressUpParamRandom(
              (m_Sex == HERO_SEX__MALE)? PM_MALE: PM_FEMALE,
              m_pDressUpManager->GetDressUpModelResourceManagerCore(
                          (m_Sex == HERO_SEX__MALE)
                          ? poke_3d::model::DressUpParam::SEX_MALE
                          : poke_3d::model::DressUpParam::SEX_FEMALE),
              &diveDressUpParameter);
  }
#endif// PM_DEBUG
  System::DressUp::OverwrideDressUpParamSpace(
              (m_Sex == HERO_SEX__MALE)? PM_MALE: PM_FEMALE,
              &diveDressUpParameter);
  m_pDressUpManager->LoadDressUpPartsSync(
              diveDressUpParameter);
  m_pDressUpManager->SetupDressUpParts(
              pAllocator,
              diveDressUpParameter);
  pCharaModelFactory->CreateDressUpModel(
              pAllocator,
              pHeap,
              m_pModel,
              m_pDressUpManager,
              charaID);
  m_pModel->SetPosition(gfl2::math::Vector3::GetZero());
  m_pModel->SetRotation(gfl2::math::Vector3::GetZero());
  m_pModel->SetScale(1.0f, 1.0f, 1.0f);
  // Visualize
  Visualize();
}
// Update
void Hero::Update(
            const Rail::Directions& location,
            const f32& elevationAngle)
{
  // calculate Spine2 offset
  const s32 indexSpine2Joint = m_pModel->GetJointIndex("Spine2");
  LUNA_SOL_DIVE_ASSERT(indexSpine2Joint != -1);
  const gfl2::math::Vector3 difference =
          m_pModel->GetJointWorldPosition(indexSpine2Joint)
          - m_pModel->GetPosition();
  const gfl2::math::Vector3 forwardDirection =
          m_pModel->GetRotationQuat().Transform(gfl2::math::Vector3::GetZUnit());
  const gfl2::math::Vector3 verticalDirection =
          m_pModel->GetRotationQuat().Transform(gfl2::math::Vector3::GetYUnit());
  const gfl2::math::Vector3 horizontalDirection =
          m_pModel->GetRotationQuat().Transform(- gfl2::math::Vector3::GetXUnit());
  const f32 spine2Forward = forwardDirection.Dot(difference);
  const f32 spine2Vertical = verticalDirection.Dot(difference);
  const f32 spine2Horizontal = horizontalDirection.Dot(difference);
  // initialize offset parameter
  if(! m_IsOffsetParameterSet)
  {
    m_OffsetParameter.forward = - spine2Forward;
    m_OffsetParameter.vertical = - spine2Vertical;
    m_OffsetParameter.horizontal = - spine2Horizontal;
    m_IsOffsetParameterSet = true;
  }
  // update offset
  m_Offset =
          location.Forward() * m_OffsetParameter.forward
          + location.Vertical() * m_OffsetParameter.vertical
          + location.Horizontal() * m_OffsetParameter.horizontal;
  // update location & rotation
  m_pModel->SetPosition(
          location.Point() + m_Offset);
  m_pModel->SetRotationQuat(
              gfl2::math::Quaternion(
                          location.Horizontal(),
                          elevationAngle)
              * location.RotationQuaternion());
  // motion update is execute by MotionController
  // update Spine2 location
  m_Spine2.Initialize(
              m_pModel->GetPosition()
                      + location.Forward() * spine2Forward
                      + location.Vertical() * spine2Vertical
                      + location.Horizontal() * spine2Horizontal,
              location.Forward(),
              location.Vertical());
}
// Whether this Model is visible or not
const bool& Hero::IsVisible() const
{
  return m_IsVisible;
}
// Visualize
void Hero::Visualize()
{
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  if(! m_IsVisible)
  {
    m_IsVisible = true;
#if PM_DEBUG
    if(! Debug::Parameter::Get().modelVisualizeHero) return;
#endif// PM_DEBUG
    for(u32 i = 0 ; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      const poke_3d::model::DressUpModel::PartsModel parts =
              static_cast<poke_3d::model::DressUpModel::PartsModel>(i);
      m_pRenderingPipeline->AddObject(
                  m_pModel->GetPartsModelInstanceNode(parts),
                  true);
    }
  }
}
// Non-Visualize
void Hero::NonVisualize()
{
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  if(m_IsVisible)
  {
    m_IsVisible = false;
    for(u32 i = 0 ; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      const poke_3d::model::DressUpModel::PartsModel parts =
              static_cast<poke_3d::model::DressUpModel::PartsModel>(i);
      m_pRenderingPipeline->RemoveObject(
                  m_pModel->GetPartsModelInstanceNode(parts));
    }
  }
}
// Spine2 Location
const Rail::Directions& Hero::Spine2() const
{
  return m_Spine2;
}
// Offset
const gfl2::math::Vector3& Hero::Offset() const
{
  return m_Offset;
}
// Scale: getter
f32 Hero::GetScale() const
{
  return m_pModel->GetScale().GetX();
}
// Scale: setter
void Hero::SetScale(
            const f32& value)
{
  m_pModel->SetScale(
              value,
              value,
              value);
}
// Translation
void Hero::Translation(
            const gfl2::math::Vector3& displacement)
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  m_pModel->SetPosition(
              m_pModel->GetPosition() + displacement);
  m_Spine2.Translation(displacement);
}
}// namespace Player
}// namespace LunaSolDive
