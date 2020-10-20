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
#include <math/include/gfl2_Transporter.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(math)


//
typedef TransBase<f32> TransBaseF32;
typedef TransLiner<f32> TransLinerF32;
typedef TransCurveFast<f32> TransCurveFastF32;
typedef TransCurveSlow<f32> TransCurveSlowF32;
typedef TransCurveSFS<f32> TransCurveSFSF32;
typedef TransSin TransSinF32;   //f32専用
typedef TransVec<gfl2::math::TransLiner<f32> > TransVecLiner;


GFL_NAMESPACE_END(math)
GFL_NAMESPACE_END(gfl2)
#endif // __GFL_TRANSPORTERTYPES_H__
