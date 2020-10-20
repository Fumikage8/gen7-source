//======================================================================
/**
 * @file PartStorageList.h
 * @date 2016/10/24 16:20:40
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__PART_STORAGE_LIST_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__PART_STORAGE_LIST_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <types/include/gfl2_Typedef.h>
#include "./PartShape.h"

// forward declaration
namespace System{
class nijiAllocator;
}// namespace System

namespace LunaSolDive{
// forward declaration
class RenderingPipeline;
namespace Rail{
// forward declaration
struct PartSetUpParameter;
class Part;
class PartStorage;

class PartStorageList{
  GFL_FORBID_COPY_AND_ASSIGN(PartStorageList);
public:
  // constructor
  PartStorageList();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              RenderingPipeline* pRenderingPipeline);
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator);
  // Terminate
  void Terminate();
  // Size
  const u32& Size() const;
  // Get
  const PartStorage& Get(
              const PartShape& shape) const;
  // Set Up
  void SetUp(
              Part* pTarget,
              const PartSetUpParameter& parameter) const;
private:
  // Initialize: Part Storage (temporary)
  void InitializePartStorage(
              gfl2::heap::HeapBase* pHeap);
  // Get pointer
  PartStorage* GetPtr(
              const PartShape& shape) const;
  // Size
  u32 m_Size;
  // Elements
  PartStorage* m_Elements;
  // pointer to RenderingPipeline
  RenderingPipeline* m_pRenderingPipeline;
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__PART_STORAGE_LIST_H_INCLUDED__
