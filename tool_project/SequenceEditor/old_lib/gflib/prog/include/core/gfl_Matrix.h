/* 
 * File:   gfl_Matrix.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 14:20
 */

#ifndef GFL_MATRIX_H
#define	GFL_MATRIX_H
#pragma once

// 定義

#ifdef GFL_PLATFORM_3DS
#define GFL_USE_AOS 0 /**< Array Of Structures */
#define GFL_USE_SOA 1 /**< Structure Of Arrays */
#else // PC? GL はAOSなので絶対必要
#define GFL_USE_AOS 1 /**< Array Of Structures */
#define GFL_USE_SOA 1 /**< Structure Of Arrays */
#endif

#define GFL_DEFAULT_AOS (0 & GFL_USE_AOS) /**< デフォルト行列をAOSにする */
//#define GFL_DEFAULT_SOA (1 - GFL_DEFAULT_AOS) /**< デフォルト行列をSOAにする */

// =============================================================================
// このファイルに含まれるクラス群
// =============================================================================

namespace gfl {
namespace core {
#ifdef GFL_PLATFORM_3DS
typedef nn::math::QUAT QuaternionNative;
typedef nn::math::QUAT_ QuaternionStruct;
#else
#include "gfl_QuaternionNative.h"
#endif
class Quaternion;

/**
 * 共用体
 */
struct MatrixStruct44Base {
	union {
		/** パラメーターひとつずつアクセスする場合 */
		struct {
			f32 GFL_MTX_00, GFL_MTX_01, GFL_MTX_02, GFL_MTX_03,
				GFL_MTX_10, GFL_MTX_11, GFL_MTX_12, GFL_MTX_13,
				GFL_MTX_20, GFL_MTX_21, GFL_MTX_22, GFL_MTX_23,
				GFL_MTX_30, GFL_MTX_31, GFL_MTX_32, GFL_MTX_33;
		};
		/** １次元配列としてアクセスする場合 */
		f32 GFL_MTX_ARRAY1D[16];
		/** ２次元配列としてアクセスする場合 */
		f32 GFL_MTX_ARRAY2D[4][4];
		/** TupleStr としてアクセスする場合 */
		VectorStruct4 GFL_MTX_VECTOR[4];
	};
};

#if GFL_USE_SOA
#ifdef GFL_PLATFORM_3DS
#include <nn/math/math_Matrix33.h>
#include <nn/math/math_Matrix34.h>
#include <nn/math/math_Matrix44.h>
typedef nn::math::MTX33 MatrixSoaNative33;
typedef nn::math::MTX34 MatrixSoaNative34;
typedef nn::math::MTX44 MatrixSoaNative44;
typedef nn::math::MTX33_ MatrixStruct33;
typedef nn::math::MTX34_ MatrixStruct34;
typedef nn::math::MTX44_ MatrixStruct44;
#else
#include "gfl_MatrixSoaNative33.h"
#include "gfl_MatrixSoaNative34.h"
#include "gfl_MatrixSoaNative44.h"
#endif
#endif

#if GFL_USE_AOS
#include "gfl_MatrixAos44.h"
#include "gfl_MatrixAos43.h"
#endif

#if GFL_USE_SOA
#define CAST_34_PTR(T) reinterpret_cast<MatrixSoa34*> (T)
#define CONST_CAST_34_PTR(T) reinterpret_cast<const MatrixSoa34*> (T)
#define CONST_CAST_34_REF(T) reinterpret_cast<const MatrixSoa34&> (T)
#include "gfl_MatrixSoa34.h"
#include "gfl_MatrixSoa44.h"
#endif

// デフォルトマトリックスを設定
#if GFL_DEFAULT_AOS
typedef MatrixAos43 Matrix; /**<デフォルト行列型 */
typedef MatrixAos44 Matrix44; /**<デフォルト４ｘ４行列型*/
#else
typedef MatrixSoa34 Matrix; /**<デフォルト行列型 */
typedef MatrixSoa44 Matrix44; /**<デフォルト４ｘ４行列型*/
#endif
typedef MatrixAos44 Matrix44AOS; // 互換性維持のため
}
}

#endif // GFL_MATRIX_H
