#ifndef __GFL_MATH_H__
#define __GFL_MATH_H__
//=============================================================================
/**
 * @file	  gfl2_math.h
 * @brief	  計算クラス
 * @author  k.ohno
 * @date	  2014.1.16
 */
//=============================================================================
#pragma once


#include "gfl2_math_control.h"
#include "gfl2_math_crc.h"
#include "gfl2_math_random.h"
#include "gfl2_MathTriangular.h"
#include "gfl2_MathCommon.h"
#include "gfl2_Matrix.h"
#include "gfl2_tinymt32.h"
#include "gfl2_Vector.h"
#include "gfl2_MathArithmetic.h"

#include "gfl2_math_convert.h"
#include "gfl2_Vector2.h"
#include "gfl2_Vector3.h"
#include "gfl2_Vector4.h"

#include "gfl2_Matrix34.h"
#include "gfl2_Matrix44.h"

#include "gfl2_Vec.h"
#include "gfl2_Mtx.h"

#include "gfl2_MathUtil.h"


#if 0
namespace gfl2 { namespace math {

enum PivotDirection
{
    PIVOT_NONE,             //!< 回転処理をおこないません。
    PIVOT_UPSIDE_TO_TOP,    //!< カメラ上方向が画面上方向を向くように回転します。
    PIVOT_UPSIDE_TO_RIGHT,  //!< カメラ上方向が画面右方向を向くように回転します。
    PIVOT_UPSIDE_TO_BOTTOM, //!< カメラ上方向が画面下方向を向くように回転します。
    PIVOT_UPSIDE_TO_LEFT,   //!< カメラ上方向が画面左方向を向くように回転します。
    PIVOT_NUM
};

} }
#endif


#endif // __GFL_MATH_H__
