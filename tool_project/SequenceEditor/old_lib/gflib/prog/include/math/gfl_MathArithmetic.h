#ifndef __GFL_MATHARITHMETIC_H__
#define __GFL_MATHARITHMETIC_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MathArithmetic.h
 *	@brief  算術　関数定義
 *	@author	tomoya takahashi
 *	@date		2010.10.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn/math.h>

#include <math/gfl_MathType.h>

namespace gfl {
namespace math {

/* =======================================================================
        算術演算に使用する一般的な定数(from nn/math/math_Constant.h)
   ======================================================================== */
extern const f32 F_PI;   // π
extern const f32 F_PIx2; // 2π
extern const f32 F_PI_2; // π/2
extern const f32 F_MAX;  // 最大の正の数
extern const f32 F_MIN;  // 最小の正の数

/* =======================================================================
        浮動少数に関する数学関数
   ======================================================================== */

/*!
    @name    浮動小数に関する数学関数
    @{
*/

// u32 F32AsU32(f32 x)
//
// 説明： f32 型の値をビット列を変更することなく u32 型にします
// 引数： x： u32型 にする値。
// 返値： x と等しいビット表現を持つ u32 型の値。
inline u32 F32AsU32( f32 x) { return nn::math::F32AsU32( x); }



/*!--------------------------------------------------------------------------*
  @brief        u32 型の値をビット列を変更することなく f32 型にします

  @param[in]    x  f32型にする値。

  @return       x と等しいビット表現を持つ f32 型の値。
 *---------------------------------------------------------------------------*/
inline f32 U32AsF32( u32 x) { return nn::math::U32AsF32( x); }


/*!--------------------------------------------------------------------------*
  @brief        浮動小数点数の指数部を取得します。

  @param[in]    f   指数部を取得する数。

  @return       f の指数部を返します。ただし負の値はオフセット表現ではなく 2 の補数表現です。
 *---------------------------------------------------------------------------*/
inline s32 FGetExpPart( f32 f) { return nn::math::FGetExpPart( f); }


/*!--------------------------------------------------------------------------*
  @brief        浮動小数点数の仮数部を求めます。

  @param[in]    f   仮数部を取得する数。

  @return       f の仮数部を返します。返り値は f と同じ符号を持っています。
 *---------------------------------------------------------------------------*/
inline f32 FGetMantPart( f32 f) { return nn::math::FGetMantPart( f); }


/*!--------------------------------------------------------------------------*
  @brief        数の正負に応じて値を選択します。

  @param[in]    cond    返り値を選択する条件。
  @param[in]    ifPos   cond が0以上の場合の返り値。
  @param[in]    ifNeg   cond が負の場合の返り値。

  @return       cond が 0 以上であれば ifPos を、そうでなければ ifNeg を返します。
 *---------------------------------------------------------------------------*/
inline f32 FSelect( f32 cond, f32 ifPos, f32 ifNeg) { return nn::math::FSelect( cond,ifPos,ifNeg); }


/*!--------------------------------------------------------------------------*
  @brief        絶対値を求めます

  @param[in]    x   絶対値を求める値

  @return       x の絶対値を返します
 *---------------------------------------------------------------------------*/
inline f32 FAbs( f32 x) { return nn::math::FAbs( x); }


/*!--------------------------------------------------------------------------*
  @brief        絶対値が等しく、符号が負である数を求めます

  @param[in]    x   元の数。

  @return       x と絶対値が等しく、符号が負である数を返します
 *---------------------------------------------------------------------------*/
inline f32 FNAbs( f32 x) { return nn::math::FNAbs( x); }


/*!--------------------------------------------------------------------------*
  @brief        一方の数の符号を他方の数にコピーします

  @param[in]    abs     絶対値の元となる数。
  @param[in]    sign    符号を採用する数。

  @return       abs の絶対値と sign の符号を持った数を返します。
 *---------------------------------------------------------------------------*/
inline f32 FCopySign( f32 abs, f32 sign) { return nn::math::FCopySign( abs,sign); }


/*!--------------------------------------------------------------------------*
  @brief        e^x を求めます

  @param[in]    x   指数の値

  @return       e^x を返します
 *---------------------------------------------------------------------------*/
inline f32 FExp( f32 x) { return nn::math::FExp( x); }


/*!--------------------------------------------------------------------------*
  @brief        自然対数を求めます

  @param[in]    x   自然対数を求める値

  @return       x の自然対数を返します
 *---------------------------------------------------------------------------*/
inline f32 FLog( f32 x) { return nn::math::FLog( x); }


/*!--------------------------------------------------------------------------*
  @brief        常用対数を求めます

  @param[in]    x   常用対数を求める値

  @return       x の常用対数を返します
 *---------------------------------------------------------------------------*/
inline f32 FLog10( f32 x) { return nn::math::FLog10( x); }


/*!--------------------------------------------------------------------------*
  @brief        剰余を計算します

  @param[in]    x   割られる数
  @param[in]    y   割る数

  @return       x / y の剰余を返します
 *---------------------------------------------------------------------------*/
inline f32 FMod( f32 x, f32 y) { return nn::math::FMod( x,y); }


/*!--------------------------------------------------------------------------*
  @brief        浮動小数点数を整数部と小数部に分けます。整数部、小数部共に x と同じ符号を持ちます。

  @param[in]    x   元の浮動小数点数。
  @param[out]   y   整数部を格納するバッファへのポインタ。

  @return       小数部を返します
 *---------------------------------------------------------------------------*/
inline f32 FModf( f32 x, f32* y) { return nn::math::FModf( x,y); }


/*!--------------------------------------------------------------------------*
  @brief        x を下回らない最小の整数値を求めます

  @param[in]    x   対象の値

  @return       x を下回らない最小の整数値を返します
 *---------------------------------------------------------------------------*/
inline f32 FCeil( f32 x) { return nn::math::FCeil( x); }


/*!--------------------------------------------------------------------------*
  @brief        x を超えない最大の整数値を求めます

  @param[in]    x   対象の値

  @return       x を超えない最大の整数値を返します
 *---------------------------------------------------------------------------*/
inline f32 FFloor( f32 x) { return nn::math::FFloor( x); }


/*!--------------------------------------------------------------------------*
  @brief        f32型からs16型に変換します

  @param[in]    x   s16型に変換する値

  @return       x と同じ値を持ったs16型の値を返します
 *---------------------------------------------------------------------------*/
inline s16 F32ToS16( f32 x) { return nn::math::F32ToS16( x); }


/*!--------------------------------------------------------------------------*
  @brief        f32型からu16型に変換します

  @param[in]    x   u16型に変換する値

  @return       x と同じ値を持ったu16型の値を返します
 *---------------------------------------------------------------------------*/
inline u16 F32ToU16( f32 x) { return nn::math::F32ToU16( x); }


/*!--------------------------------------------------------------------------*
  @brief        u16型からf32型に変換します

  @param[in]    x   f32型に変換する値

  @return       x と同じ値を持ったf32型の値を返します
 *---------------------------------------------------------------------------*/
inline f32 U16ToF32( u16 x) { return nn::math::U16ToF32( x); }


/*!--------------------------------------------------------------------------*
  @brief        s16型からf32型に変換します

  @param[in]    x   f32型に変換する値

  @return       x と同じ値を持ったf32型の値を返します
 *---------------------------------------------------------------------------*/
inline f32 S16ToF32( s16 x) { return nn::math::S16ToF32( x); }



/*!--------------------------------------------------------------------------*
  @brief        逆数を高速低精度に求めます

  @param[in]    x   逆数を求める値

  @return       x の逆数を返します
 *---------------------------------------------------------------------------*/
inline f32 FInv( f32 x) { return nn::math::FInv( x); }


/*!--------------------------------------------------------------------------*
  @brief        平方根の逆数を求めます。

  @param[in]    x 平方根の逆数を求める値。

  @return       x の平方根の逆数を返します。
 *---------------------------------------------------------------------------*/
inline f32 FrSqrt( f32 x) { return nn::math::FrSqrt( x); }


/*!--------------------------------------------------------------------------*
  @brief        平方根を求めます

  @param[in]    x   平方根を求める値

  @return       x の平方根を返します
 *---------------------------------------------------------------------------*/
inline f32 FSqrt( f32 x) { return nn::math::FSqrt( x); }


/*!--------------------------------------------------------------------------*
  @brief        立方根を求めます

  @param[in]    x   立方根を求める値

  @return       x の立方根を返します
 *---------------------------------------------------------------------------*/
inline f32 FCbrt( f32 x) { return nn::math::FCbrt( x); }


/*!--------------------------------------------------------------------------*
  @brief        エルミート補間を行います

  @param[in]    v0  点1での値。
  @param[in]    t0  点1での傾き。
  @param[in]    v1  点2での値。
  @param[in]    t1  点2での傾き。
  @param[in]    s   補間対象位置。(点1:0.0〜1.0:点2)

  @return       補間結果の値。
 *---------------------------------------------------------------------------*/
inline f32 Hermite( f32 v0, f32 t0, f32 v1, f32 t1, f32 s) { return nn::math::Hermite( v0,t0,v1,t1,s); }


/*!--------------------------------------------------------------------------*
  @brief        エルミート補間計算です。

  @param[in]    v0  点1での値。
  @param[in]    t0  点1での傾き。
  @param[in]    v1  点2での値。
  @param[in]    t1  点2での傾き。
  @param[in]    p   点1から補間対象位置の距離。
  @param[in]    d   点1と点2の距離。

  @return       エルミート補間の結果です。
 *---------------------------------------------------------------------------*/
inline f32 Hermite( f32 v0, f32 t0, f32 v1, f32 t1, f32 p, f32 d) { return nn::math::Hermite( v0,t0,v1,t1,p,d); }

inline f32 Bezier( f32 p1, f32 p2, f32 p3, f32 p4, f32 s) { return nn::math::Bezier( p1,p2,p3,p4,s); }
inline f32 CatmullRom( f32 p0, f32 p1, f32 p2, f32 p3, f32 s) { return nn::math::CatmullRom( p0,p1,p2,p3,s); }

/*!
    @}
*/

/* =======================================================================
        整数関連
   ======================================================================== */

/*!
    @name    整数に関する数学関数
    @{
*/



/*!--------------------------------------------------------------------------*
  @brief        MSBからの連続する 0 のビットを数えます。

  @param[in]    x  対象のビット列

  @return       MSBからの連続する 0 のビット数を返します。
 *---------------------------------------------------------------------------*/
inline u32 CntLz( u32 x) { return nn::math::CntLz( x); }


/*!--------------------------------------------------------------------------*
  @brief        整数値が2の整数乗かどうかを判定します。

  @param[in]    x  判定対象の整数値

  @return       x が 2 の累乗であれば true を、そうでなければ false を返します。
 *---------------------------------------------------------------------------*/
//          ただし x が 0 の場合は true を返します。
inline bool IsPwr2( int x) { return nn::math::IsPwr2( x); }

/*!--------------------------------------------------------------------------*
  @brief        1 になっているビットで一番右(LSB側)のビットを取得します。

  @param[in]    x  対象とするビット列

  @return       x で一番右の 1 となっているビット位置のみが 1 となっているビット列を返します。
 *---------------------------------------------------------------------------*/
inline int Rightmost1( int x) { return nn::math::Rightmost1( x); }
inline bit32 Rightmost1( bit32 x) { return nn::math::Rightmost1( x); }
inline bit64 Rightmost1( bit64 x) { return nn::math::Rightmost1( x); }
/*!--------------------------------------------------------------------------*
  @brief        0 になっているビットで一番右(LSB側)のビットを取得します。

  @param[in]    x  対象とするビット列

  @return       x で一番右の 0 となっているビット位置のみが 1 となっているビット列を返します。
 *---------------------------------------------------------------------------*/
inline int Rightmost0( int x) { return nn::math::Rightmost0( x); }
inline bit32 Rightmost0( bit32 x) { return nn::math::Rightmost0( x); }
inline bit64 Rightmost0( bit64 x) { return nn::math::Rightmost0( x); }

/*!--------------------------------------------------------------------------*
  @brief        ビット列同士の距離を計算します。

  @param[in]    x  ビット列 1
  @param[in]    y  ビット列 2

  @return       ビット列 1 とビット列 2 の距離を返します。
 *---------------------------------------------------------------------------*/
inline u32 DistBit( u32 x, u32 y) { return nn::math::DistBit( x,y); }

/*!--------------------------------------------------------------------------*
  @brief        LSBからの連続する 0 のビットを数えます。

  @param[in]    x  対象のビット列

  @return       LSBからの連続する 0 のビット数を返します。
 *---------------------------------------------------------------------------*/
inline u32 CntTz( u32 x) { return nn::math::CntTz( x); }

/*!--------------------------------------------------------------------------*
  @brief        整数の 2 を底とする対数を計算し、結果を整数で返します。

  @param[in]    x  真数

  @return       整数の 2 を底とする対数を整数で返します。
 *---------------------------------------------------------------------------*/
inline u32 ILog2( u32 x) { return nn::math::ILog2( x); }

/*!--------------------------------------------------------------------------*
  @brief        与えられた整数以下の最大の 2 の累乗を計算します。

  @param[in]    x  対象の整数

  @return       x 以下の最大の2の累乗を返します。
 *---------------------------------------------------------------------------*/
inline u32 FloorPwr2( u32 x) { return nn::math::FloorPwr2( x); }

/*!--------------------------------------------------------------------------*
  @brief        与えられた整数以上の最小の 2 の累乗を計算します。

  @param[in]    x  対象の整数

  @return       x 以上の最小の 2 の累乗を返します。
 *---------------------------------------------------------------------------*/
inline u32 CeilPwr2( u32 x) { return nn::math::CeilPwr2( x); }

/*!--------------------------------------------------------------------------*
  @brief        fXのfY乗を計算します。

  @param[in]    fX
  @param[in]    fY

  @return       fXのfY乗を返します。
 *---------------------------------------------------------------------------*/
inline f32 PowF(const float fX, const float fY) { return ::std::powf(fX, fY); }

/*!--------------------------------------------------------------------------*
  @brief        nXのnY乗を計算します。

  @param[in]    nX
  @param[in]    nY(正数)

  @return       nXのnY乗を返します。
 *---------------------------------------------------------------------------*/
inline int PowInt(const int nX, const u32 nY) { if( nY == 0 ) { return 1; } else { int nRet = nX; for( int i = 1; i < nY; i++ ) { nRet *= nX; } return nRet; } }

} /* end of namespace math */
} /* end of namespace gfl */

#endif // __GFL_MATHARITHMETIC_H__
