//======================================================================
/**
 * @file Gimmick_ObjectStorage.cpp
 * @date 2016/11/16 22:00:30
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./ObjectStorage.h"
#include "./Parameter.h"

namespace LunaSolDive{
namespace Gimmick{
// constructor
ObjectStorage::ObjectStorage(){}
// destructor
ObjectStorage::~ObjectStorage(){};
// Initialize
void ObjectStorage::Initialize(){}
// Terminate
void ObjectStorage::Terminate(){}
// Distance at the Object appears
const f32& ObjectStorage::AppearanceDistance() const
{
  return Parameter::Get().ObjectAppearanceDistance();
}
// Distance at the Object disappears
const f32& ObjectStorage::DisappearanceDistance() const
{
  return Parameter::Get().ObjectDisappearanceDistance();
}
}// namespace Gimmick
}// namespace LunaSolDive
