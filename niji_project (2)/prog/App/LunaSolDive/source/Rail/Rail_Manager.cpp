//======================================================================
/**
 * @file Rail_Manager.cpp
 * @date 2016/10/17 12:24:24
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "GameSys/include/GameManager.h"
#include "./Factory.h"
#include "./InitialPart.h"
#include "./Manager.h"
#include "./Part.h"
#include "./PartStorage.h"
#include "./PartStorageList.h"
#include "./Section.h"
#include "./SkyBox.h"
#include "./TravelDistanceCounter.h"
#include "../Assert.h"
#include "../Manager.h"
#if PM_DEBUG
#include "../Debug/DrawUtil.h"
#include "../Debug/Parameter.h"
#endif// PM_DEBNUG

namespace LunaSolDive{
namespace Rail{
// static parameter
static const u32 PART_LIST_SIZE = 5;
static const s32 PRESENT_PART = 0;
static const s32 NEXT_PART = 1;
static const s32 PREVIOUS_PART = -1;
static const s32 MOST_FORWARD_PART = PART_LIST_SIZE - 2;
static const s32 NEW_PART = -2;
static const f32 DistancePreviousPartVanish = 20.0e+2f;
// constructor
Manager::Manager()
    : m_pPartStorageList(NULL),
      m_pSkyBox(NULL),
      m_pFactory(NULL),
      m_PartList(NULL),
      m_PresentPartIndex(0),
      m_pInitialPart(NULL),
      m_pTravelDistanceCounter(NULL)
{
  m_InternalCoordinates.z = 0.0f;
  m_InternalCoordinates.radius_rate = 0.0f;
  m_InternalCoordinates.angle = 0.0f;
}
// Initialize
void Manager::Initialize(
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline,
            Effect::Manager* pEffectManager)
{
  // initialize PartStorageList
  m_pPartStorageList = GFL_NEW(pHeap) PartStorageList;
  m_pPartStorageList->Initialize(
              pHeap,
              pRenderingPipeline);
  // initialize SkyBox
  m_pSkyBox = GFL_NEW(pHeap) SkyBox();
  m_pSkyBox->Initialize(
              pHeap,
              pRenderingPipeline);
  // initialize Factory
  m_pFactory = GFL_NEW(pHeap) Factory();
  // initialize Travel Distance Counter
  m_pTravelDistanceCounter = GFL_NEW(pHeap) TravelDistanceCounter();
  // initialize Part
  m_PartList = GFL_NEW(pHeap) Part[PART_LIST_SIZE];
  LUNA_SOL_DIVE_ASSERT(m_PartList != NULL);
  m_PresentPartIndex = 0;
  // initialize InitialPart
  m_pInitialPart = GFL_NEW(pHeap) InitialPart();
  m_pInitialPart->Initialize(
              pHeap,
              pRenderingPipeline,
              pEffectManager);
  // initialize Initial Directions
  m_InitialDirections.Initialize(
              gfl2::math::Vector3::GetZero(),
              gfl2::math::Vector3::GetZUnit(),
              gfl2::math::Vector3::GetYUnit());
}
// Initialize Part & Section
void Manager::InitializePartSection()
{
  // initialize Part
  LUNA_SOL_DIVE_ASSERT(m_pPartStorageList != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pFactory != NULL);
  // initialize Present Part
  const PartSetUpParameter firstPartInfo = m_pFactory->Next();
  m_pPartStorageList->SetUp(
              PartPtr(PRESENT_PART),
              firstPartInfo);
  PartPtr(PRESENT_PART)->ConnectTo(
              InitialDirections());
  // initialize Next Part
  for(s32 i = NEXT_PART; i <= MOST_FORWARD_PART; ++i)
  {
    const PartSetUpParameter partInfo = m_pFactory->Next();
    m_pPartStorageList->SetUp(
                PartPtr(i),
                partInfo);
    PartPtr(i)->ConnectTo(*PartPtr(i - 1));
  }
  // visualize First Part
  PartPtr(PRESENT_PART)->SetModel();
  PartPtr(PRESENT_PART)->Visualize();
  // visualize InitialPart
  m_pInitialPart->Visualize();
  // initialize Section
  const u32 index = 0;
  // initialize Present Section
  m_PresentSection = PartPtr(PRESENT_PART)->CreateSection(index);
  // initialize Internal Coordinates
  m_InternalCoordinates.z = 0.0f;
  m_InternalCoordinates.radius_rate = 0.0f;
  m_InternalCoordinates.angle = 0.0f;
  // initialize TravelDistance Counter
  m_pTravelDistanceCounter->Initialize(0.0f);
  // initialize Directions
  UpdateLocation();
}
// Terminate
void Manager::Terminate()
{
  // delete Part
  if(m_PartList != NULL)
  {
    for(u32 i = 0; i < PART_LIST_SIZE; ++i)
    {
      m_PartList[i].Terminate();
    }
    GFL_DELETE_ARRAY(m_PartList);
    m_PartList = NULL;
  }
  // delete Travel Distance Counter
  if(m_pTravelDistanceCounter != NULL)
  {
    m_pTravelDistanceCounter->Terminate();
    GFL_DELETE(m_pTravelDistanceCounter);
    m_pTravelDistanceCounter = NULL;
  }
  // delete Factory
  if(m_pFactory != NULL)
  {
    m_pFactory->Terminate();
    GFL_DELETE(m_pFactory);
    m_pFactory = NULL;
  }
  // delete SkyBox
  if(m_pSkyBox != NULL)
  {
    m_pSkyBox->Terminate();
    GFL_DELETE(m_pSkyBox);
    m_pSkyBox = NULL;
  }
  // delete InitialPart
  if(m_pInitialPart != NULL)
  {
    m_pInitialPart->Terminate();
    GFL_DELETE(m_pInitialPart);
    m_pInitialPart = NULL;
  }
  // delete Part Storage List
  if(m_pPartStorageList != NULL)
  {
    m_pPartStorageList->Terminate();
    GFL_DELETE(m_pPartStorageList);
    m_pPartStorageList = NULL;
  }
  // reset Parameters
  m_PresentPartIndex = 0;
  m_PresentLocation.Terminate();
  m_InitialDirections.Terminate();
}
// Create
void Manager::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  // create PartStorageList
  m_pPartStorageList->Create(
              pHeap,
              pAllocator);
  // create SkyBox
  m_pSkyBox->Create(
              pHeap,
              pAllocator);
  // create InitialPart
  m_pInitialPart->Create(
              pHeap,
              pAllocator,
              InitialDirections());
  // Initialize Part & Section
  InitializePartSection();
}
// Update
void Manager::Update()
{
  // update Part
  for(s32 i = PREVIOUS_PART; i <= NEXT_PART; ++i)
  {
    if(PartPtr(i)->IsValid())
    {
      PartPtr(i)->Update();
    }
  }
  // update SkyBox
  m_pSkyBox->Update(Location());
  // update InitialPart
  m_pInitialPart->Update();
#if PM_DEBUG
  // update Debug::DrawUtilData
  Debug::Parameter::Get().pDrawUtil->Data().RailSection(CreateSection(-1));
  Debug::Parameter::Get().pDrawUtil->Data().RailSection(CreateSection(0));
  Debug::Parameter::Get().pDrawUtil->Data().RailSection(CreateSection(1));
  Debug::Parameter::Get().pDrawUtil->Data().RailSection(CreateSection(2));
#endif// PM_DEBUG
}
// Progress
void Manager::Progress(
            const f32& forward,
            const f32& radiusRate,
            const f32& angle)
{
  LUNA_SOL_DIVE_ASSERT(0.0f - EPSILON <= radiusRate && radiusRate <= 1.0f + EPSILON);
  m_InternalCoordinates.z += forward;
  m_InternalCoordinates.radius_rate = radiusRate;
  m_InternalCoordinates.angle = angle;
  // Progression Judgement of the Section
#if PM_DEBUG// infinity loop check
  u32 loopCounter = 0;
#endif// PM_DEBUG
  while(m_PresentSection.IsForward(m_InternalCoordinates))
  {
#if PM_DEBUG// inifinity loop check
    ++loopCounter;
    LUNA_SOL_DIVE_ASSERT(loopCounter < 2);
#endif// PM_DEBUG
    // to Next Section
    ToNextSection();
  }
  // update Travel Distance
  const f32 beginZ = m_PresentSection.BeginCylindricSectionZ(
              m_InternalCoordinates.radius_rate,
              m_InternalCoordinates.angle);
  const f32 endZ = m_PresentSection.EndCylindricSectionZ(
              m_InternalCoordinates.radius_rate,
              m_InternalCoordinates.angle);
  const f32 distance =
          m_PresentSection.Length()
          * (m_InternalCoordinates.z - beginZ) / (endZ - beginZ);
  LUNA_SOL_DIVE_ASSERT(m_pTravelDistanceCounter != NULL);
  m_pTravelDistanceCounter->UpdatePresentSection(
              gfl2::math::clamp(
                          distance,
                          0.0f,
                          m_PresentSection.Length()));
  // visualize
  if(! PartPtr(NEXT_PART)->IsVisible()
      && m_pTravelDistanceCounter->PresentPart() > DistancePreviousPartVanish)
  {
    if(m_pInitialPart->IsVisible())
    {
      ARAI_PRINT("non-visualize Initial Part\n");
      m_pInitialPart->NonVisualize();
    }
    else
    {
      ARAI_PRINT("non-visualize Previous Part\n");
      PartPtr(PREVIOUS_PART)->NonVisualize();
      PartPtr(PREVIOUS_PART)->UnsetModel();
    }
    ARAI_PRINT("visualize Next Part\n");
    PartPtr(NEXT_PART)->SetModel();
    PartPtr(NEXT_PART)->Visualize();
  }
  // update Location
  UpdateLocation();
}
// Progress
void Manager::Progress(
            const f32& forward,
            const gfl2::math::Vector2& normalizedVerticalLocation)
{
  Progress(
              forward,
              normalizedVerticalLocation.Length(),
              gfl2::math::Atan2Rad(
                      normalizedVerticalLocation.GetX(),
                      normalizedVerticalLocation.GetY()));
}
// Location
const Location& Manager::Location() const
{
  return m_PresentLocation;
}
// Location
Location Manager::Location(
            const f32& forward,
            const f32& radiusRate,
            const f32& angle) const
{
  LUNA_SOL_DIVE_ASSERT(m_pTravelDistanceCounter != NULL);
  // conditional branch by travel distance
  s32 partIndex = 0;
  f32 partTravelDistance = m_pTravelDistanceCounter->PresentPart() + forward;
  if(partTravelDistance < 0.0f)
  {// Previous Part
    partTravelDistance = PartPtr(partIndex)->TotalLength() - partTravelDistance;
    --partIndex;
  }
  else if(partTravelDistance > PartPtr(partIndex)->TotalLength())
  {// Next Part, ..., Most Forward Part
    while(partIndex <= MOST_FORWARD_PART
            && partTravelDistance > PartPtr(partIndex)->TotalLength())
    {
      partTravelDistance -= PartPtr(partIndex)->TotalLength();
      ++partIndex;
    }
  }
  const Part* const pPart = PartPtr(partIndex);
  const Directions objectDirections = AdjustedDirections(
              *pPart,
              partTravelDistance,
              radiusRate,
              angle);
  const Directions railDirections = pPart->IntermediateDirections(
              partTravelDistance);
  return Rail::Location(
              m_pTravelDistanceCounter->Total() + forward,
              objectDirections,
              railDirections,
              pPart->Radius());
}
// Location
Location Manager::Location(
            const f32& forward,
            const gfl2::math::Vector2& normalizedVerticalLocation) const
{
  return Location(
              forward,
              normalizedVerticalLocation.Length(),
              gfl2::math::Atan2Rad(
                      normalizedVerticalLocation.GetX(),
                      normalizedVerticalLocation.GetY()));
}
// Translation
void Manager::Translation(
            const gfl2::math::Vector3& displacement)
{
  // skybox
  m_pSkyBox->Translation(displacement);
  // part
  for(u32 i = 0; i < PART_LIST_SIZE; ++i)
  {
    if(! m_PartList[i].IsValid()) continue;
    m_PartList[i].Translation(displacement);
  }
  // initial part
  m_pInitialPart->Translation(displacement);
  // section
  m_PresentSection.Translation(displacement);
  // location
  m_PresentLocation.Translation(displacement);
  m_InitialDirections.Translation(displacement);
}
// Update Location
void Manager::UpdateLocation()
{
  // next Section Directions
  Directions nextDirections;
  const Section nextSection = CreateSection(1);
  if(nextSection.IsValid())
  {
    nextDirections.Initialize(
                nextSection.Direction(),
                nextSection.VerticalDirection());
  }
  // Previous Section Directions
  Directions previousDirections;
  const Section previousSection = CreateSection(-1);
  if(previousSection.IsValid())
  {
    previousDirections.Initialize(
                previousSection.Direction(),
                previousSection.VerticalDirection());
  }
  // update Present Location
  const Directions presentDirections = AdjustedDirections(
              m_PresentSection,
              m_InternalCoordinates,
              nextDirections,
              previousDirections);
  const Directions railDirections = PartPtr(PRESENT_PART)->IntermediateDirections(
              m_pTravelDistanceCounter->PresentPart());
  m_PresentLocation.Initialize(
              m_pTravelDistanceCounter->Total(),
              presentDirections,
              railDirections,
              PartPtr(PRESENT_PART)->Radius());
}
// Create Section
Section Manager::CreateSection(
            const s32& indexDifference) const
{
  const s32 baseIndex = m_PresentSection.Index() + indexDifference;
  if(baseIndex < 0)
  {
    if(! PartPtr(PREVIOUS_PART)->IsValid())
    {
      return Section();
    }
    const s32 effectiveIndex = PartPtr(PREVIOUS_PART)->PointNumber() - 1 + baseIndex;
    return (effectiveIndex >= 0)
            ? PartPtr(PREVIOUS_PART)->CreateSection(effectiveIndex)
            : Section();
  }
  else if(static_cast<s32>(PartPtr(PRESENT_PART)->PointNumber()) - 1 <= baseIndex)
  {
    const u32 effectiveIndex = baseIndex - PartPtr(PRESENT_PART)->PointNumber() + 1;
    return (effectiveIndex < PartPtr(NEXT_PART)->PointNumber())
            ? PartPtr(NEXT_PART)->CreateSection(effectiveIndex)
            : Section();
  }
  else
  {
    return PartPtr(PRESENT_PART)->CreateSection(baseIndex);
  }
}
// Part pointer
Part* Manager::PartPtr(
            const s32& indexDifference) const
{
  LUNA_SOL_DIVE_ASSERT(m_PartList != NULL);
  const s32 difference =
          (indexDifference < 0)
          ? indexDifference + PART_LIST_SIZE
          : indexDifference;
  LUNA_SOL_DIVE_ASSERT(difference >= 0);
  const u32 index = (m_PresentPartIndex + difference) % PART_LIST_SIZE;
  return &m_PartList[index];
}
// to Next Section
void Manager::ToNextSection()
{
  // update Internal Coordinates
  const Section nextSection = CreateSection(1);
  const f32 presentSectionEndZ = m_PresentSection.EndCylindricSectionZ(
              m_InternalCoordinates.radius_rate,
              m_InternalCoordinates.angle);
  const f32 residualZ = m_InternalCoordinates.z - presentSectionEndZ;
  LUNA_SOL_DIVE_ASSERT(residualZ >= - EPSILON);
  m_InternalCoordinates.z = presentSectionEndZ;
  const f32 nextSectionBeginZ = nextSection.ToInternalPoint(
              m_PresentSection.ToWorldPoint(m_InternalCoordinates)).z;
  m_InternalCoordinates.z = nextSectionBeginZ + residualZ;
  // update Travel Distance
  LUNA_SOL_DIVE_ASSERT(m_pTravelDistanceCounter != NULL);
  if(nextSection.Index() != 0)
  {
    m_pTravelDistanceCounter->AddSectionLength(
                m_PresentSection.Length());
  }
  else
  {
    m_pTravelDistanceCounter->AddPartLength(
                PartPtr(PRESENT_PART)->TotalLength());
  }
  // update Section
  m_PresentSection = nextSection;
  // update Part
  if(m_PresentSection.Index() == 0)
  {
    ToNextPart();
  }
}
// to Next Part
void Manager::ToNextPart()
{
  LUNA_SOL_DIVE_ASSERT(m_pFactory != NULL);
  ARAI_PRINT("set up Part\n");
  // update Part Index
  m_PresentPartIndex = (m_PresentPartIndex + 1) % PART_LIST_SIZE;
  // pointer to Part
  Part* const pMostForwardPart= PartPtr(MOST_FORWARD_PART - 1);
  LUNA_SOL_DIVE_ASSERT(pMostForwardPart->IsValid());
  Part* const pNewPart = PartPtr(NEW_PART);
  // Initialize New Part
  pNewPart->Terminate();
  const PartSetUpParameter nextPartInfo = m_pFactory->Next();
  m_pPartStorageList->SetUp(
              pNewPart,
              nextPartInfo);
  // Connect to Present Part with Next Part
  pNewPart->ConnectTo(*pMostForwardPart);
}
// Initial Directions
const Directions& Manager::InitialDirections() const
{
  return m_InitialDirections;
}
}// namespace Rail
}// namespace LunaSolDive
