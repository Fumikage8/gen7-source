//======================================================================
/**
 * @file Manager.cpp
 * @date 2016/11/02 15:54:44
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Action.h"
#include "./Distortion.h"
#include "./Energy.h"
#include "./Manager.h"
#include "./ObjectList.h"
#include "./Obstacle.h"
#include "./Parameter.h"
#include "./Storage.h"
#include "./Placement/Manager.h"
#include "../Assert.h"
#include "../Rail/Manager.h"

namespace LunaSolDive{
namespace Gimmick{
// constructor
Manager::Manager()
    : m_pRailManager(NULL),
      m_pStorage(NULL),
      m_pObjectList(NULL),
      m_pAction(NULL),
      m_pPlacement(NULL){}
// Initialize
void Manager::Initialize(
            gfl2::heap::HeapBase* pHeap,
            const Mode& mode,
            Effect::Manager* pEffectManager,
            const Rail::Manager* pRailManager,
            RenderingPipeline* pRenderingPipeline)
{
  // set pointer
  m_pRailManager = pRailManager;
  LUNA_SOL_DIVE_ASSERT(m_pRailManager != NULL);
  // initialize Storage
  m_pStorage = GFL_NEW(pHeap) Storage();
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  m_pStorage->Initialize(
              pHeap,
              pRenderingPipeline,
              pRailManager,
              pEffectManager,
              Parameter::ENERGY_NUMBER,
              Parameter::OBSTACLE_NUMBER,
              Parameter::DISTORTION_NUMBER);
  // initialize ObjectList
  m_pObjectList = GFL_NEW(pHeap) ObjectList();
  LUNA_SOL_DIVE_ASSERT(m_pObjectList != NULL);
  m_pObjectList->Initialize(
              pHeap,
              Parameter::ENERGY_NUMBER,
              Parameter::OBSTACLE_NUMBER,
              Parameter::DISTORTION_NUMBER);
  // initialize Action
  m_pAction = GFL_NEW(pHeap) Gimmick::Action();
  // initialize Placement::Manager
  m_pPlacement = GFL_NEW(pHeap) Placement::Manager();
  m_pPlacement->Initialize(
              pHeap,
              mode,
              m_pRailManager,
              m_pStorage,
              m_pObjectList);
}
// Terminate
void Manager::Terminate()
{
  // delete Placement
  if(m_pPlacement != NULL)
  {
    m_pPlacement->Terminate();
    GFL_DELETE(m_pPlacement);
    m_pPlacement = NULL;
  }
  // delete Action
  if(m_pAction != NULL)
  {
    m_pAction->Terminate();
    GFL_DELETE(m_pAction);
    m_pAction = NULL;
  }
  // delete ObjectList
  if(m_pObjectList != NULL)
  {
    m_pObjectList->Terminate();
    GFL_DELETE_ARRAY(m_pObjectList);
    m_pObjectList = NULL;
  }
  // delete Object Storage
  if(m_pStorage != NULL)
  {
    m_pStorage->Terminate();
    GFL_DELETE(m_pStorage);
    m_pStorage = NULL;
  }
  // unset pointer
  m_pRailManager = NULL;
}
// Create
void Manager::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  // create Storage
  m_pStorage->Create(
              pHeap,
              pAllocator);
  // initial placement
  m_pPlacement->InitialPlacement();
}
// Upadte
void Manager::Update()
{
  LUNA_SOL_DIVE_ASSERT(m_pAction != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pObjectList != NULL);
  // reset Action
  m_pAction->Terminate();
  // update energy
  for(u32 i = 0; i < m_pObjectList->EnergyNumber(); ++i)
  {
    if(! m_pObjectList->Energy(i).IsValid()) continue;
    m_pObjectList->Energy(i).Update();
  }
  // update obstacle
  for(u32 i = 0; i < m_pObjectList->ObstacleNumber(); ++i)
  {
    if(! m_pObjectList->Obstacle(i).IsValid()) continue;
    m_pObjectList->Obstacle(i).Update();
  }
  // update distortion
  for(u32 i = 0; i < m_pObjectList->DistortionNumber(); ++i)
  {
    if(! m_pObjectList->Distortion(i).IsValid()) continue;
    m_pObjectList->Distortion(i).Update();
  }
}
// Update
void Manager::Update(
            const Rail::Location& location,
            const Rail::Directions& presentCollisionPoint,
            const Rail::Directions& previousCollisionPoint)
{
  LUNA_SOL_DIVE_ASSERT(m_pAction != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pObjectList != NULL);
  LUNA_SOL_DIVE_ASSERT(location.IsValid());
  // reset & initialize Action
  m_pAction->Terminate();
  m_pAction->Initialize(
              location.TravelDistance());
  // update Energy
  for(u32 i = 0; i < m_pObjectList->EnergyNumber(); ++i)
  {
    if(! m_pObjectList->Energy(i).IsValid()) continue;
    m_pAction->Add(
                m_pObjectList->Energy(i).Update(
                        location,
                        presentCollisionPoint,
                        previousCollisionPoint));
  }
  // update Obstacle
  for(u32 i = 0; i < m_pObjectList->ObstacleNumber(); ++i)
  {
    if(! m_pObjectList->Obstacle(i).IsValid()) continue;
    m_pAction->Add(
                m_pObjectList->Obstacle(i).Update(
                        location,
                        presentCollisionPoint,
                        previousCollisionPoint));
  }
  // update Distortion
  for(u32 i = 0; i < m_pObjectList->DistortionNumber(); ++i)
  {
    if(! m_pObjectList->Distortion(i).IsValid()) continue;
    m_pAction->Add(
                m_pObjectList->Distortion(i).Update(
                        location,
                        presentCollisionPoint,
                        previousCollisionPoint));
  }
  // placement
  m_pPlacement->Update();
}
// Action
const Action& Manager::Action() const
{
  LUNA_SOL_DIVE_ASSERT(m_pAction != NULL);
  return *m_pAction;
}
// Start the collapse of the distortion after collision
void Manager::StartDistortionCollapse()
{
  for(u32 i = 0; i < m_pObjectList->DistortionNumber(); ++i)
  {
    Distortion& distortion = m_pObjectList->Distortion(i);
    if(distortion.IsValid())
    {
      distortion.Collapse();
    }
  }
}
// Translation
void Manager::Translation(
            const gfl2::math::Vector3& displacement)
{
  // objects
  LUNA_SOL_DIVE_ASSERT(m_pObjectList != NULL);
  m_pObjectList->Translation(displacement);
  // action
  LUNA_SOL_DIVE_ASSERT(m_pAction != NULL);
  m_pAction->Translation(displacement);
}
}// namespace Gimmick
}// namespace LunaSolDive
