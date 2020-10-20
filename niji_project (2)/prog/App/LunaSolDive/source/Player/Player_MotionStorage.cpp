//======================================================================
/**
 * @file Player_MotionStorage.cpp
 * @date 2016/11/29 23:01:12
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include "System/include/nijiAllocator.h"
#include "./MotionStorage.h"
#include "../Assert.h"
#include "../Archive/Chara.h"
#include "arc_index/LunaSolDiveChara.gaix"

namespace LunaSolDive{
namespace Player{
// parameter
static const u32 INDEX__STRAIGHT = 0;
static const u32 INDEX__LEFT = 1;
static const u32 INDEX__RIGHT = 2;
static const u32 INDEX__TURN_LEFT = 3;
static const u32 INDEX__TURN_RIGHT = 4;
static const u32 INDEX__RESET = 5;
static const u32 INDEX__SIZE = 6;
static const u32 SIZE = MOTION_SPEED__SIZE * INDEX__SIZE;
// static function
// Create Motion ResourceNode List
void CreateMotion(
            gfl2::renderingengine::scenegraph::resource::ResourceNode** pMotionList,
            System::nijiAllocator* pAllocator,
            const Archive::MotionIndex& index);
// constructor
MotionStorage::MotionStorage()
    : m_Version(VERSION__NONE),
      m_Sex(HERO_SEX__NONE),
      m_HeroMotionList(NULL),
      m_PokemonMotionList(NULL){}
// Iniitalize
void MotionStorage::Initialize(
            const Version& version,
            const HeroSex& sex,
            gfl2::heap::HeapBase* pHeap)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  m_Version = version;
  LUNA_SOL_DIVE_ASSERT(m_Version != VERSION__NONE);
  m_Sex = sex;
  LUNA_SOL_DIVE_ASSERT(m_Sex != HERO_SEX__NONE);
  // initialize resource node list
  m_HeroMotionList = GFL_NEW(pHeap) resource::ResourceNode*[SIZE];
  LUNA_SOL_DIVE_ASSERT(m_HeroMotionList != NULL);
  m_PokemonMotionList = GFL_NEW(pHeap) resource::ResourceNode*[SIZE];
  LUNA_SOL_DIVE_ASSERT(m_PokemonMotionList != NULL);
  for(u32 i = 0; i < SIZE; ++i)
  {
    m_HeroMotionList[i] = NULL;
    m_PokemonMotionList[i] = NULL;
  }
}
// Terminate
void MotionStorage::Terminate()
{
  // delete hero motion
  if(m_HeroMotionList != NULL)
  {
    for(u32 i = 0 ; i < SIZE; ++i)
    {
      GFL_SAFE_DELETE(m_HeroMotionList[i]);
    }
    GFL_DELETE(m_HeroMotionList);
    m_HeroMotionList = NULL;
  }
  // delete pokemon motion
  if(m_PokemonMotionList != NULL)
  {
    for(u32 i = 0 ; i < SIZE; ++i)
    {
      GFL_SAFE_DELETE(m_PokemonMotionList[i]);
    }
    GFL_DELETE(m_PokemonMotionList);
    m_PokemonMotionList = NULL;
  }
  // reset parameter
  m_Version = VERSION__NONE;
  m_Sex = HERO_SEX__NONE;
}
// Create
void MotionStorage::Create(
            System::nijiAllocator* pAllocator)
{
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  // create motion resource node: hero
  CreateMotion(
              m_HeroMotionList,
              pAllocator,
              Archive::HeroMotionIndex(m_Version, m_Sex));
  // create motion resource node: pokemon
  CreateMotion(
              m_PokemonMotionList,
              pAllocator,
              Archive::PokemonMotionIndex(m_Version));
}
// Index
u32 MotionStorage::Index(
            const MotionSpeed& speed,
            const MotionDirection& direction,
            const bool& isTurn,
            const bool& isReset) const
{
  LUNA_SOL_DIVE_ASSERT(speed != MOTION_SPEED__SIZE);
  LUNA_SOL_DIVE_ASSERT(direction != MOTION_DIRECTION__SIZE);
  LUNA_SOL_DIVE_ASSERT(! (isTurn && direction == MOTION_DIRECTION__STRAIGHT));
  const u32 innerIndex =
              isReset
              ? INDEX__RESET
              : (direction == MOTION_DIRECTION__STRAIGHT)
              ? INDEX__STRAIGHT
              : (direction == MOTION_DIRECTION__LEFT)
              ? (isTurn? INDEX__TURN_LEFT: INDEX__LEFT)
              : (isTurn? INDEX__TURN_RIGHT: INDEX__RIGHT);
  LUNA_SOL_DIVE_ASSERT(innerIndex < INDEX__SIZE);
  return speed * INDEX__SIZE + innerIndex;
}
// Hero Motion
gfl2::renderingengine::scenegraph::resource::ResourceNode*
MotionStorage::Hero(
            const MotionSpeed& speed,
            const MotionDirection& direction,
            const bool& isTurn,
            const bool& isReset) const
{
  const u32 index = Index(
              speed,
              direction,
              isTurn,
              isReset);
  LUNA_SOL_DIVE_ASSERT(index < SIZE);
  return m_HeroMotionList[index];
}
// Pokemon Motion
gfl2::renderingengine::scenegraph::resource::ResourceNode*
MotionStorage::Pokemon(
            const MotionSpeed& speed,
            const MotionDirection& direction,
            const bool& isTurn,
            const bool& isReset) const
{
  const u32 index = Index(
              speed,
              direction,
              isTurn,
              isReset);
  LUNA_SOL_DIVE_ASSERT(index < SIZE);
  return m_PokemonMotionList[index];
}
// static function
// Create Motion ResourceNode List
void CreateMotion(
            gfl2::renderingengine::scenegraph::resource::ResourceNode** MotionList,
            System::nijiAllocator* pAllocator,
            const Archive::MotionIndex& index)
{
  using namespace gfl2::renderingengine::scenegraph;
  resource::AnimationResourceNodeFactory factory;
  u32 i = 0;
  // walk
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.walk_straight),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.walk_left),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.walk_right),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.walk_turn_left),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.walk_turn_right),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.walk_reset),
              0);
  // run
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.run_straight),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.run_left),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.run_right),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.run_turn_left),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.run_turn_right),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.run_reset),
              0);
  // glide
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.glide_straight),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.glide_left),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.glide_right),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.glide_turn_left),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.glide_turn_right),
              0);
  MotionList[i++] = factory.CreateResourceNode(
              pAllocator,
              Archive::Chara::Get().Data(index.glide_reset),
              0);
  LUNA_SOL_DIVE_ASSERT(i == SIZE);
}
}// namespace Player
}// namespace LunaSolDive
