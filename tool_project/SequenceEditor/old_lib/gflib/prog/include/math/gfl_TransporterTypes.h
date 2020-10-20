#ifndef __GFL_TRANSPORTERTYPES_H__
#define __GFL_TRANSPORTERTYPES_H__
//======================================================================
/**
 * @file	gfl_TransporterType.h
 * @brief	数値補間計算クラス(型宣言
 * @author	ariizumi
 * @data	10/12/06
 */
//======================================================================

#pragma once
#include <math/gfl_Transporter.h>

namespace gfl
{
namespace math
{

//
typedef TransBase<f32> TransBaseF32;
typedef TransLiner<f32> TransLinerF32;
typedef TransCurveFast<f32> TransCurveFastF32;
typedef TransCurveSlow<f32> TransCurveSlowF32;
typedef TransCurveSFS<f32> TransCurveSFSF32;
typedef TransSin TransSinF32;   //f32専用
typedef TransVec<gfl::math::TransLiner<f32> > TransVecLiner;


}//namespace gfl
}//namespace math
#endif // __GFL_TRANSPORTERTYPES_H__
