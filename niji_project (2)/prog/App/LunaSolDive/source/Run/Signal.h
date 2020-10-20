//======================================================================
/**
 * @file Signal.h
 * @date 2017/02/14 20:31:42
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__SIGNAL_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__SIGNAL_H_INCLUDED__
#pragma once

namespace LunaSolDive{
namespace Run{
enum ResetSignal
{
  RESET_SIGNAL__NONE,
  RESET_SIGNAL__PULL,
  RESET_SIGNAL__PUSH,
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__SIGNAL_H_INCLUDED__
