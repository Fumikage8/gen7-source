/* 
 * File:   gfl_Vector.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 13:18
 */

#ifndef GFL_VECTOR_H
#define	GFL_VECTOR_H
#pragma once

//#if defined(GFL_PLATFORM_3DS) // 3DS にはない…
//#include <arm_neon.h>
//#endif
	
// 長いので…
#define GFL_TP GFL_TEMPLATE_PARAMETER

namespace gfl {
namespace core {

#ifdef GFL_PLATFORM_3DS
#include <nn/math/math_Vector2.h>
#include <nn/math/math_Vector3.h>
#include <nn/math/math_Vector4.h>
#else
#include "gfl_VectorNative2.h"
#include "gfl_VectorNative3.h"
#include "gfl_VectorNative4.h"
#endif

#include "gfl_Tuple2.h"
#include "gfl_Vector2.h"
#include "gfl_Vector3.h"
#include "gfl_Vector4.h"

#ifdef GFL_PLATFORM_PC
VectorNative3* VEC3Set( VectorNative3 * pOut, const Vector4& p1 );
VectorNative3 * VEC3Maximize( VectorNative3 *pOut, const VectorNative3 * p1, const VectorNative3 * p2 );
VectorNative3 * VEC3Minimize( VectorNative3 *pOut, const VectorNative3 * p1, const VectorNative3 * p2 );
VectorNative3 * VEC3Add( VectorNative3 *pOut, const VectorNative3 * p1, const VectorNative3 * p2 );
VectorNative3 * VEC3Sub( VectorNative3 *pOut, const VectorNative3 * p1, const VectorNative3 * p2 );
VectorNative3 * VEC3Scale( VectorNative3 *pOut, const VectorNative3 * p1, f32 scl );
#endif
}
}

#endif	/* GFL_Vector_H */
