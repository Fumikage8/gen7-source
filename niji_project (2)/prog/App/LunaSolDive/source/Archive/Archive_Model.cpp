//======================================================================
/**
 * @file Archive_Model.cpp
 * @date 2016/11/25 16:06:11
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Model.h"
#include "../Assert.h"
#include "arc_def_index/arc_def.h"
#include "arc_index/LunaSolDiveModel.gaix"

namespace LunaSolDive{
namespace Archive{
// constructor
Model::Model()
    : Base(){}
// Initialize
void Model::Initialize(
            gfl2::heap::HeapBase* pHeap,
            gfl2::fs::AsyncFileManager* pFileManager)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pFileManager != NULL);
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Model();
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  pInstance->Base::Initialize(
              pHeap,
              pFileManager,
              ARCID_LUNA_SOL_DIVE_MODEL,
              GARC_LunaSolDiveModel_DATA_NUM,
              false);
}
// Terminate
void Model::Terminate()
{
  if(pInstance != NULL)
  {
    pInstance->Base::Terminate();
    GFL_DELETE(pInstance);
    pInstance = NULL;
  }
}
// getter
const Model& Model::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// Instance
Model* Model::pInstance = NULL;
}// namespace Archive
}// namespace LunaSolDive
