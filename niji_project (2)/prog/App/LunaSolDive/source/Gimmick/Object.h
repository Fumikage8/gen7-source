//======================================================================
/**
 * @file Object.h
 * @date 2016/11/14 11:43:24
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__OBJECT_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__OBJECT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "./GimmickType.h"
#include "../Rail/Location.h"

namespace LunaSolDive{
namespace Gimmick{
// forward declaration
class ObjectStorage;

class Object{
  GFL_FORBID_COPY_AND_ASSIGN(Object);
public:
  // constructor
  Object();
  // destructor
  virtual ~Object() = 0;
  // Initialize
  void Initialize(
              const Rail::Location& location);
  // Terminate (virtual)
  virtual void Terminate();
  // Gimmick Type
  virtual GimmickType Type() const = 0;
  // check if this Object is valid (virtual)
  virtual bool IsValid() const = 0;
  // Whether this Object is visible or not
  bool IsVisible() const;
  // Whether this Object is active or not
  bool IsActive() const;
  // Location
  const Rail::Location& Location() const;
  // Translation
  virtual void Translation(
              const gfl2::math::Vector3& displacement);
protected:
  // Update
  void Update();
  // Update
  void Update(
              const Rail::Location& location);
  // Visualize this Object (virtual)
  virtual void Visualize();
  // Non-Visualize this Object (virtual)
  virtual void NonVisualize();
  // Activate this Object
  void Activate();
  // Inactivate this Object
  virtual void Inactivate();
private:
  // State
  enum State
  {
    STATE__NONE,
    STATE__INVISIBLE,
    STATE__VISIBLE,
    STATE__ACTIVE,
    STATE__INACTIVE,
    STATE__VANISHED,
  };
  // Storage
  virtual const ObjectStorage& Storage() const = 0;
  // Whether the Model is set for this Object
  virtual bool IsModelSet() const = 0;
  // Set the Model for this Object
  virtual void SetModel() = 0;
  // Location
  Rail::Location m_Location;
  // State
  State m_State;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__OBJECT_H_INCLUDED__
