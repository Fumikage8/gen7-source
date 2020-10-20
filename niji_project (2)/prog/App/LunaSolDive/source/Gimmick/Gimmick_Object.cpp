//======================================================================
/**
 * @file Object.cpp
 * @date 2016/11/14 11:46:40
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Object.h"
#include "./ObjectStorage.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Gimmick{
// constrcutor
Object::Object()
    : m_State(STATE__NONE){}
// destructor
Object::~Object(){}
// Initialize
void Object::Initialize(
            const Rail::Location& location)
{
  // set location
  m_Location = location;
  LUNA_SOL_DIVE_ASSERT(m_Location.IsValid());
  // initialize parameter
  m_State = STATE__INVISIBLE;
}
// Terminate (virtual)
void Object::Terminate()
{
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__NONE
              || m_State == STATE__INVISIBLE
              || m_State == STATE__VANISHED);
  // reset parameter
  m_Location.Terminate();
  m_State = STATE__NONE;
}
// Update
void Object::Update()
{
  LUNA_SOL_DIVE_ASSERT(m_State != STATE__NONE);
  LUNA_SOL_DIVE_ASSERT(IsValid());
  if(! IsValid()) return;
  // set model
  if(! IsModelSet())
  {
    SetModel();
  }
}
// Update
void Object::Update(
            const Rail::Location& location)
{
  LUNA_SOL_DIVE_ASSERT(m_State != STATE__NONE);
  LUNA_SOL_DIVE_ASSERT(IsValid());
  LUNA_SOL_DIVE_ASSERT(location.IsValid());
  if(! IsValid()) return;
  const f32 distanceDifference =
              Location().TravelDistance() - location.TravelDistance();
  // state: invisible
  if(m_State == STATE__INVISIBLE)
  {
    if(! IsModelSet())
    {
      SetModel();
      if(! IsModelSet()) return;
    }
    if(distanceDifference < Storage().AppearanceDistance())
    {
      Visualize();
    }
  }
  // state: visible
  if(m_State == STATE__VISIBLE)
  {
    if(distanceDifference < Storage().ActivationDistance())
    {
      Activate();
    }
  }
  // state: active
  if(m_State == STATE__ACTIVE)
  {
    if(distanceDifference < Storage().InactivationDistance())
    {
      Inactivate();
    }
  }
  // vanish
  if(distanceDifference < Storage().DisappearanceDistance())
  {
    if(IsVisible())
    {
      NonVisualize();
    }
    Terminate();
    LUNA_SOL_DIVE_ASSERT(! IsValid());
  }
}
// Location
const Rail::Location& Object::Location() const
{
  return m_Location;
}
// Translation
void Object::Translation(
            const gfl2::math::Vector3& displacement)
{
  m_Location.Translation(displacement);
}
// Whether this Object is visible or not
bool Object::IsVisible() const
{
  return m_State == STATE__VISIBLE
          || m_State == STATE__ACTIVE
          || m_State == STATE__INACTIVE;
}
// Whether this Object is active or not
bool Object::IsActive() const
{
  return m_State == STATE__ACTIVE;
}
// Visualize this Object (virtual)
void Object::Visualize()
{
  LUNA_SOL_DIVE_ASSERT(IsModelSet());
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__INVISIBLE);
  m_State = STATE__VISIBLE;
}
// Non-Visualize this Object (virtual)
void Object::NonVisualize()
{
  LUNA_SOL_DIVE_ASSERT(IsVisible());
  m_State = STATE__VANISHED;
}
// Activate this Object
void Object::Activate()
{
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__VISIBLE);
  m_State = STATE__ACTIVE;
}
// Inactivate this Object
void Object::Inactivate()
{
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__ACTIVE);
  m_State = STATE__INACTIVE;
}
}// namespace Gimmcik
}// namespace LunaSolDive
