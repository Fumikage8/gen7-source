//======================================================================
/**
 * @file Archive_Effect.cpp
 * @date 2016/11/25 18:54:43
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Effect.h"
#include "../Assert.h"
#include "arc_def_index/arc_def.h"
#include "arc_index/LunaSolDiveEffect.gaix"

namespace LunaSolDive{
namespace Archive{
// constructor
Effect::Effect()
    : Base(){}
// Initialize
void Effect::Initialize(
            gfl2::heap::HeapBase* pHeap,
            gfl2::fs::AsyncFileManager* pFileManager)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pFileManager != NULL);
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Effect();
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  pInstance->Base::Initialize(
              pHeap,
              pFileManager,
              ARCID_LUNA_SOL_DIVE_EFFECT,
              GARC_LunaSolDiveEffect_DATA_NUM,
              true);
}
// Terminate
void Effect::Terminate()
{
  if(pInstance != NULL)
  {
    pInstance->Base::Terminate();
    GFL_DELETE(pInstance);
    pInstance = NULL;
  }
}
// getter
const Effect& Effect::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// Instance
Effect* Effect::pInstance = NULL;
}// namespace Archive
}// namespace LunaSolDive
