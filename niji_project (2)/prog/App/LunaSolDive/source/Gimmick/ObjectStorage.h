//======================================================================
/**
 * @file ObjectStorage.h
 * @date 2016/11/16 21:53:46
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__OBJECT_STORAGE_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__OBJECT_STORAGE_H_INCLUDED__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include "./GimmickType.h"

namespace LunaSolDive{
namespace Gimmick{
class ObjectStorage
{
public:
  // constructor
  ObjectStorage();
  // destructor
  virtual ~ObjectStorage() = 0;
  // Initialize
  void Initialize();
  // Terminate
  void Terminate();
  // Gimmick Type (override)
  virtual GimmickType Type() const = 0;
  // Distance at the Object appears
  const f32& AppearanceDistance() const;
  // Distance at the Object disappears
  const f32& DisappearanceDistance() const;
  // Distance at the object is activated
  virtual const f32& ActivationDistance() const = 0;
  // Distance at the object is inactivated
  virtual const f32& InactivationDistance() const = 0;
private:
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__OBJECT_STORAGE_H_INCLUDED__
