//======================================================================
/**
 * @file Factory.h
 * @date 2016/10/21 11:46:31
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__FACTORY_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__FACTORY_H_INCLUDED__
#pragma once

#include "./PartStorage.h"

namespace LunaSolDive{
namespace Rail{

struct PartSetUpParameter
{
  PartShape shape;
  f32 angle;
};

class Factory
{
  GFL_FORBID_COPY_AND_ASSIGN(Factory);
public:
  // constructor
  Factory();
  // Initialize
  void Initialize();
  // Terminate
  void Terminate();
  // Next Part
  PartSetUpParameter Next();
private:
  // Part Counter
  u32 m_PartCount;
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__FACTORY_H_INCLUDED__
