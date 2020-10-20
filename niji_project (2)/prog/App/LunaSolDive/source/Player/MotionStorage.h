//======================================================================
/**
 * @file MotionStorage.h
 * @date 2016/11/29 22:54:31
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__PLAYER__MOTION_H_INCLUDED__
#define __LUNA_SOL_DIVE__PLAYER__MOTION_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "./MotionType.h"
#include "../Version.h"

// forward declaration
namespace gfl2{
namespace renderingengine{
namespace scenegraph{
namespace resource{
class ResourceNode;
}// namespace resource
}// namespace scenegraph
}// namespace renderingengine
}// namespace gfl2
namespace System{
class nijiAllocator;
}// namespace System

namespace LunaSolDive{
namespace Player{

class MotionStorage
{
  GFL_FORBID_COPY_AND_ASSIGN(MotionStorage);
public:
  // constructor
  MotionStorage();
  // Iniitalize
  void Initialize(
              const Version& version,
              const HeroSex& sex,
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  void Terminate();
  // Create
  void Create(
              System::nijiAllocator* pAllocator);
  // Hero Motion
  gfl2::renderingengine::scenegraph::resource::ResourceNode* Hero(
              const MotionSpeed& speed,
              const MotionDirection& direction,
              const bool& isTurn,
              const bool& isReset) const;
  // Pokemon Motion
  gfl2::renderingengine::scenegraph::resource::ResourceNode* Pokemon(
              const MotionSpeed& speed,
              const MotionDirection& direction,
              const bool& isTurn,
              const bool& isReset) const;
private:
  // Index
  u32 Index(
              const MotionSpeed& speed,
              const MotionDirection& direction,
              const bool& isTurn,
              const bool& isReset) const;
  // Version
  Version m_Version;
  // Sex of the hero
  HeroSex m_Sex;
  // Hero Motion ResourceNode List
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_HeroMotionList;
  // Pokemon Motion ResourceNode Listt
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_PokemonMotionList;
};
}// namespace Player
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__PLAYER__MOTION_H_INCLUDED__
