//======================================================================
/**
 * @file Effect.h
 * @date 2016/11/25 18:51:49
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__ARCHIVE__EFFECT_H_INCLUDED__
#define __LUNA_SOL_DIVE__ARCHIVE__EFFECT_H_INCLUDED__
#pragma once

#include "./Base.h"

namespace LunaSolDive{
namespace Archive{
class Effect: public Base
{
  GFL_FORBID_COPY_AND_ASSIGN(Effect);
public:
  // Initialize
  static void Initialize(
              gfl2::heap::HeapBase* pHeap,
              gfl2::fs::AsyncFileManager* pFileManager);
  // Terminate
  static void Terminate();
  // getter
  static const Effect& Get();
private:
  // constructor
  Effect();
  // Instance
  static Effect* pInstance;
};
}// namespace Archive
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__ARCHIVE__EFFECT_H_INCLUDED__
