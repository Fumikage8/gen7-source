/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     util_Float24.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50866 $
 *---------------------------------------------------------------------------*/

#ifndef NN_UTIL_UTIL_FLOAT24_H_
#define NN_UTIL_UTIL_FLOAT24_H_

#include "types.h"

#ifdef __cplusplus

namespace nn {
namespace util {

//--------------------------------------------------------------------------
//! @brief        32bit 浮動小数用のユーティリティクラスです。
//
// 32bit 浮動小数のビットフォーマットは次の通りです。
// | sign | exponent | fraction |
// sign     : 符号 1 bit.
// exponent : 指数部 8 bit. bias 127.
// fraction : 仮数部 23 bit.
//---------------------------------------------------------------------------
class Float32
{
public:
    //--------------------------------------------------------------------------
    //! @brief        f32 から 32bit 浮動小数のビット表現に変換します。
    //!
    //! @param[in]    value   32bit float の値です。
    //!
    //! @return       32bit 浮動小数のビット表現を返します。
    //--------------------------------------------------------------------------
    static u32 Float32ToBits32(f32 value)
    {
        return *reinterpret_cast<u32*>(&value);
    }

    //--------------------------------------------------------------------------
    //! @brief        32bit 浮動小数のビット表現から f32 に変換します。
    //!
    //! @param[in]    value   32bit 浮動小数のビット表現です。
    //!
    //! @return       f32 の値です。
    //--------------------------------------------------------------------------
    static f32 Bits32ToFloat32(u32 value)
    {
        return *reinterpret_cast<f32*>(&value);
    }
};

//--------------------------------------------------------------------------
//! @brief        24bit 浮動小数用のユーティリティクラスです。
//
// 24bit 浮動小数のビットフォーマットは次の通りです。
// | sign | exponent | fraction |
// sign     : 符号 1 bit.
// exponent : 指数部 7 bit. bias 63.
// fraction : 仮数部 16 bit.
//---------------------------------------------------------------------------
class Float24
{
public:
    //--------------------------------------------------------------------------
    //! @brief        デフォルトコンストラクタです。
    //--------------------------------------------------------------------------
    Float24() : m_Float32( 0.0f ) {}

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    bits24   24bit 浮動小数のビットを表現した整数値です。
    //--------------------------------------------------------------------------
    /* implicit */ Float24( u32 bits24 )
    {
        m_Float32 = Bits24ToFloat32( bits24 );
    }

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    value   32bit 浮動小数の値から 24bit float を生成します。
    //--------------------------------------------------------------------------
    /* implicit */ Float24( f32 value ) : m_Float32( value ) {}

    f32     GetFloat32Value() const { return m_Float32; }
    u32     GetFloat24Value() const { return Float32ToBits24( m_Float32 ); }

    Float24& operator =(f32 value) { this->m_Float32 = value; return *this; }
    Float24& operator =(u32 bits24) { this->m_Float32 = Bits24ToFloat32( bits24 ); return *this; }

    operator f32() const { return m_Float32; }

    f32 operator +(f32 right) const { return this->m_Float32 + right; }
    f32 operator -(f32 right) const { return this->m_Float32 - right; }
    f32 operator *(f32 right) const { return this->m_Float32 * right; }
    f32 operator /(f32 right) const { return this->m_Float32 / right; }

    Float24& operator +=(f32 rhs) { this->m_Float32 += rhs; return *this; }
    Float24& operator -=(f32 rhs) { this->m_Float32 -= rhs; return *this; }
    Float24& operator *=(f32 rhs) { this->m_Float32 *= rhs; return *this; }
    Float24& operator /=(f32 rhs) { this->m_Float32 /= rhs; return *this; }

    bool operator ==(f32 rhs) const { return (rhs == this->m_Float32); }
    bool operator !=(f32 rhs) const { return !(*this == rhs); }

    //--------------------------------------------------------------------------
    //! @brief        24bit float から 32bit float に変換します。
    //!
    //! @param[in]    bits24   24bit 浮動小数のビットを表現した整数値です。
    //!
    //! @return       f32 に変換した値を返します。
    //--------------------------------------------------------------------------
    static f32 Bits24ToFloat32(u32 bits24)
    {
        u32 sign = bits24 & SIGN24;
        int exp = static_cast<int>((bits24 & EXP_MASK24) >> FRACTION_WIDTH24);
        u32 fraction = bits24 & FRACTION_MASK24;

        u32 bits32 = 0;
        bits32 |= (sign != 0) ? SIGN32 : 0;

        if ((bits24 & ~SIGN24) == 0)
        {
            exp = 0;
        }
        else
        {
            exp = exp - EXP_BIAS24 + EXP_BIAS32;
        }

        fraction = fraction << (FRACTION_WIDTH32 - FRACTION_WIDTH24);

        // ビット拡張なので、exp がオーバーフローすることは無い。
        bits32 |= fraction & FRACTION_MASK32;
        bits32 |= ((u32)exp & 0xFF) << FRACTION_WIDTH32;

        return *reinterpret_cast<f32*>(&bits32);
    }

    //--------------------------------------------------------------------------
    //! @brief        f32 から 24bit float に変換します。
    //!
    //! @param[in]    value   32bit float の値です。
    //!
    //! @return       24bit 浮動小数のビット表現を返します。
    //--------------------------------------------------------------------------
    static u32 Float32ToBits24(f32 value)
    {
        u32 bits32 = *reinterpret_cast<u32*>(&value);

        u32 sign = bits32 & SIGN32;
        int exp = static_cast<int>((bits32 & EXP_MASK32) >> FRACTION_WIDTH32);
        u32 fraction = bits32 & FRACTION_MASK32;

        u32 bits24 = 0;
        bits24 |= (sign != 0) ? SIGN24 : 0;

        if ((bits32 & ~SIGN32) == 0)
        {
            exp = 0;
        }
        else
        {
            exp = exp - EXP_BIAS32 + EXP_BIAS24;
        }

        fraction = fraction >> (FRACTION_WIDTH32 - FRACTION_WIDTH24);

        if (exp < 0)
        {
            // +0 もしくは -0 なのでそのまま。
        }
        else if (exp > 127)
        {
            // 無限大の処理
            // TODO: IEEE float の無限大の表現がGPU上で有効なのかどうかを要確認。
            bits24 = (u32)0x7F << FRACTION_WIDTH24;
        }
        else
        {
            bits24 |= fraction & FRACTION_MASK24;
            bits24 |= ((u32)exp & 0x7F) << FRACTION_WIDTH24;
        }

        return bits24;
    }

private:
    f32 m_Float32;

    enum
    {
        SIGN32 = 0x80000000,
        SIGN24 = 0x00800000,

        EXP_BIAS32 = 127,
        EXP_BIAS24 = 63,
        EXP_MASK32 = 0x7F800000,
        EXP_MASK24 = 0x007F0000,

        FRACTION_WIDTH32 = 23,
        FRACTION_MASK32  = 0x007FFFFF,
        FRACTION_WIDTH24 = 16,
        FRACTION_MASK24  = 0x0000FFFF
    };
};

//--------------------------------------------------------------------------
//! @brief        31bit 浮動小数用のユーティリティクラスです。
//
// 31bit 浮動小数のビットフォーマットは次の通りです。
// | sign | exponent | fraction |
// sign     : 符号 1 bit.
// exponent : 指数部 7 bit. bias 63.
// fraction : 仮数部 23 bit.
//---------------------------------------------------------------------------
class Float31
{
public:
    //--------------------------------------------------------------------------
    //! @brief        デフォルトコンストラクタです。
    //--------------------------------------------------------------------------
    Float31() : m_Float32( 0.0f ) {}

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    bits31   31bit 浮動小数のビットを表現した整数値です。
    //--------------------------------------------------------------------------
    /* implicit */ Float31( u32 bits31 )
    {
        m_Float32 = Bits31ToFloat32( bits31 );
    }

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    value   32bit 浮動小数の値から 31bit float を生成します。
    //--------------------------------------------------------------------------
    /* implicit */ Float31( f32 value ) : m_Float32( value ) {}

    f32     GetFloat32Value() const { return m_Float32; }
    u32     GetFloat31Value() const { return Float32ToBits31( m_Float32 ); }

    Float31& operator =(f32 value) { this->m_Float32 = value; return *this; }
    Float31& operator =(u32 bits31) { this->m_Float32 = Bits31ToFloat32( bits31 ); return *this; }

    operator f32() const { return m_Float32; }

    f32 operator +(f32 right) const { return this->m_Float32 + right; }
    f32 operator -(f32 right) const { return this->m_Float32 - right; }
    f32 operator *(f32 right) const { return this->m_Float32 * right; }
    f32 operator /(f32 right) const { return this->m_Float32 / right; }

    Float31& operator +=(f32 rhs) { this->m_Float32 += rhs; return *this; }
    Float31& operator -=(f32 rhs) { this->m_Float32 -= rhs; return *this; }
    Float31& operator *=(f32 rhs) { this->m_Float32 *= rhs; return *this; }
    Float31& operator /=(f32 rhs) { this->m_Float32 /= rhs; return *this; }

    bool operator ==(f32 rhs) const { return (rhs == this->m_Float32); }
    bool operator !=(f32 rhs) const { return !(*this == rhs); }

    //--------------------------------------------------------------------------
    //! @brief        31bit float から 32bit float に変換します。
    //!
    //! @param[in]    bits31   31bit 浮動小数のビットを表現した整数値です。
    //!
    //! @return       f32 に変換した値を返します。
    //--------------------------------------------------------------------------
    static f32 Bits31ToFloat32(u32 bits31)
    {
        u32 sign = bits31 & SIGN31;
        int exp = static_cast<int>((bits31 & EXP_MASK31) >> FRACTION_WIDTH31);
        u32 fraction = bits31 & FRACTION_MASK31;

        u32 bits32 = 0;
        bits32 |= (sign != 0) ? SIGN32 : 0;

        if ((bits31 & ~SIGN31) == 0)
        {
            exp = 0;
        }
        else
        {
            exp = exp - EXP_BIAS31 + EXP_BIAS32;
        }

        // ビット拡張なので、exp がオーバーフローすることは無い。
        bits32 |= fraction & FRACTION_MASK32;
        bits32 |= ((u32)exp & 0xFF) << FRACTION_WIDTH32;

        return *reinterpret_cast<f32*>(&bits32);
    }

    //--------------------------------------------------------------------------
    //! @brief        f32 から 31bit float に変換します。
    //!
    //! @param[in]    value   32bit float の値です。
    //!
    //! @return       31bit 浮動小数のビット表現を返します。
    //--------------------------------------------------------------------------
    static u32 Float32ToBits31(f32 value)
    {
        u32 bits32 = *reinterpret_cast<u32*>(&value);

        u32 sign = bits32 & SIGN32;
        int exp = static_cast<int>((bits32 & EXP_MASK32) >> FRACTION_WIDTH32);
        u32 fraction = bits32 & FRACTION_MASK32;

        u32 bits31 = 0;
        bits31 |= (sign != 0) ? SIGN31 : 0;

        if ((bits32 & ~SIGN32) == 0)
        {
            exp = 0;
        }
        else
        {
            exp = exp - EXP_BIAS32 + EXP_BIAS31;
        }

        if (exp < 0)
        {
            // +0 もしくは -0 なのでそのまま。
        }
        else if (exp > 127)
        {
            // 無限大の処理
            // TODO: IEEE float の無限大の表現がGPU上で有効なのかどうかを要確認。
            bits31 = (u32)0x7F << FRACTION_WIDTH31;
        }
        else
        {
            bits31 |= fraction & FRACTION_MASK31;
            bits31 |= ((u32)exp & 0x7F) << FRACTION_WIDTH31;
        }

        return bits31;
    }

private:
    f32 m_Float32;

    enum
    {
        SIGN32 = 0x80000000,
        SIGN31 = 0x40000000,

        EXP_BIAS32 = 127,
        EXP_BIAS31 = 63,
        EXP_MASK32 = 0x7F800000,
        EXP_MASK31 = 0x3F800000,

        FRACTION_WIDTH32 = 23,
        FRACTION_MASK32  = 0x007FFFFF,
        FRACTION_WIDTH31 = 23,
        FRACTION_MASK31  = 0x007FFFFF
    };
};


//--------------------------------------------------------------------------
//! @brief        20bit 浮動小数用のユーティリティクラスです。
//
// 20bit 浮動小数のビットフォーマットは次の通りです。
// | sign | exponent | fraction |
// sign     : 符号 1 bit.
// exponent : 指数部 7 bit. bias 63.
// fraction : 仮数部 12 bit.
//---------------------------------------------------------------------------
class Float20
{
public:
    //--------------------------------------------------------------------------
    //! @brief        デフォルトコンストラクタです。
    //--------------------------------------------------------------------------
    Float20() : m_Float32( 0.0f ) {}

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    bits20   20bit 浮動小数のビットを表現した整数値です。
    //--------------------------------------------------------------------------
    /* implicit */ Float20( u32 bits20 )
    {
        m_Float32 = Bits20ToFloat32( bits20 );
    }

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    value   32bit 浮動小数の値から 20bit float を生成します。
    //--------------------------------------------------------------------------
    /* implicit */ Float20( f32 value ) : m_Float32( value ) {}

    f32     GetFloat32Value() const { return m_Float32; }
    u32     GetFloat20Value() const { return Float32ToBits20( m_Float32 ); }

    Float20& operator =(f32 value) { this->m_Float32 = value; return *this; }
    Float20& operator =(u32 bits20) { this->m_Float32 = Bits20ToFloat32( bits20 ); return *this; }

    operator f32() const { return m_Float32; }

    f32 operator +(f32 right) const { return this->m_Float32 + right; }
    f32 operator -(f32 right) const { return this->m_Float32 - right; }
    f32 operator *(f32 right) const { return this->m_Float32 * right; }
    f32 operator /(f32 right) const { return this->m_Float32 / right; }

    Float20& operator +=(f32 rhs) { this->m_Float32 += rhs; return *this; }
    Float20& operator -=(f32 rhs) { this->m_Float32 -= rhs; return *this; }
    Float20& operator *=(f32 rhs) { this->m_Float32 *= rhs; return *this; }
    Float20& operator /=(f32 rhs) { this->m_Float32 /= rhs; return *this; }

    bool operator ==(f32 rhs) const { return (rhs == this->m_Float32); }
    bool operator !=(f32 rhs) const { return !(*this == rhs); }

    //--------------------------------------------------------------------------
    //! @brief        20bit float から 32bit float に変換します。
    //!
    //! @param[in]    bits20   20bit 浮動小数のビットを表現した整数値です。
    //!
    //! @return       f32 に変換した値を返します。
    //--------------------------------------------------------------------------
    static f32 Bits20ToFloat32(u32 bits20)
    {
        u32 sign = bits20 & SIGN20;
        int exp = static_cast<int>((bits20 & EXP_MASK20) >> FRACTION_WIDTH20);
        u32 fraction = bits20 & FRACTION_MASK20;

        u32 bits32 = 0;
        bits32 |= (sign != 0) ? SIGN32 : 0;

        if ((bits20 & ~SIGN20) == 0)
        {
            exp = 0;
        }
        else
        {
            exp = exp - EXP_BIAS20 + EXP_BIAS32;
        }

        fraction = fraction << (FRACTION_WIDTH32 - FRACTION_WIDTH20);

        // ビット拡張なので、exp がオーバーフローすることは無い。
        bits32 |= fraction & FRACTION_MASK32;
        bits32 |= ((u32)exp & 0xFF) << FRACTION_WIDTH32;

        return *reinterpret_cast<f32*>(&bits32);
    }

    //--------------------------------------------------------------------------
    //! @brief        f32 から 20bit float に変換します。
    //!
    //! @param[in]    value   32bit float の値です。
    //!
    //! @return       20bit 浮動小数のビット表現を返します。
    //--------------------------------------------------------------------------
    static u32 Float32ToBits20(f32 value)
    {
        u32 bits32 = *reinterpret_cast<u32*>(&value);

        u32 sign = bits32 & SIGN32;
        int exp = static_cast<int>((bits32 & EXP_MASK32) >> FRACTION_WIDTH32);
        u32 fraction = bits32 & FRACTION_MASK32;

        u32 bits20 = 0;
        bits20 |= (sign != 0) ? SIGN20 : 0;

        if ((bits32 & ~SIGN32) == 0)
        {
            exp = 0;
        }
        else
        {
            exp = exp - EXP_BIAS32 + EXP_BIAS20;
        }

        fraction = fraction >> (FRACTION_WIDTH32 - FRACTION_WIDTH20);

        if (exp < 0)
        {
            // +0 もしくは -0 なのでそのまま。
        }
        else if (exp > 127)
        {
            // 無限大の処理
            // TODO: IEEE float の無限大の表現がGPU上で有効なのかどうかを要確認。
            bits20 = (u32)0x7F << FRACTION_WIDTH20;
        }
        else
        {
            bits20 |= fraction & FRACTION_MASK20;
            bits20 |= ((u32)exp & 0x7F) << FRACTION_WIDTH20;
        }

        return bits20;
    }

private:
    f32 m_Float32;

    enum
    {
        SIGN32 = 0x80000000,
        SIGN20 = 0x00080000,

        EXP_BIAS32 = 127,
        EXP_BIAS20 = 63,
        EXP_MASK32 = 0x7F800000,
        EXP_MASK20 = 0x0007F000,

        FRACTION_WIDTH32 = 23,
        FRACTION_MASK32  = 0x007FFFFF,
        FRACTION_WIDTH20 = 12,
        FRACTION_MASK20  = 0x00000FFF
    };
};


//--------------------------------------------------------------------------
//! @brief        16bit 浮動小数用のユーティリティクラスです。
//
// 16bit 浮動小数のビットフォーマットは次の通りです。
// | sign | exponent | fraction |
// sign     : 符号 1 bit.
// exponent : 指数部 5 bit. bias 15.
// fraction : 仮数部 10 bit.
//---------------------------------------------------------------------------
class Float16
{
public:
    //--------------------------------------------------------------------------
    //! @brief        デフォルトコンストラクタです。
    //--------------------------------------------------------------------------
    Float16() : m_Float32( 0.0f ) {}

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    bits16   16bit 浮動小数のビットを表現した整数値です。
    //--------------------------------------------------------------------------
    /* implicit */ Float16( u32 bits16 )
    {
        m_Float32 = Bits16ToFloat32( bits16 );
    }

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    value   32bit 浮動小数の値から 16bit float を生成します。
    //--------------------------------------------------------------------------
    /* implicit */ Float16( f32 value ) : m_Float32( value ) {}

    f32     GetFloat32Value() const { return m_Float32; }
    u16     GetFloat16Value() const { return Float32ToBits16( m_Float32 ); }

    Float16& operator =(f32 value) { this->m_Float32 = value; return *this; }
    Float16& operator =(u32 bits16) { this->m_Float32 = Bits16ToFloat32( bits16 ); return *this; }

    operator f32() const { return m_Float32; }
    operator u16() const { return GetFloat16Value(); }

    f32 operator +(f32 right) const { return this->m_Float32 + right; }
    f32 operator -(f32 right) const { return this->m_Float32 - right; }
    f32 operator *(f32 right) const { return this->m_Float32 * right; }
    f32 operator /(f32 right) const { return this->m_Float32 / right; }

    Float16& operator +=(f32 rhs) { this->m_Float32 += rhs; return *this; }
    Float16& operator -=(f32 rhs) { this->m_Float32 -= rhs; return *this; }
    Float16& operator *=(f32 rhs) { this->m_Float32 *= rhs; return *this; }
    Float16& operator /=(f32 rhs) { this->m_Float32 /= rhs; return *this; }

    bool operator ==(f32 rhs) const { return (rhs == this->m_Float32); }
    bool operator !=(f32 rhs) const { return !(*this == rhs); }

    //--------------------------------------------------------------------------
    //! @brief        16bit float から 32bit float に変換します。
    //!
    //! @param[in]    bits16   16bit 浮動小数のビットを表現した整数値です。
    //!
    //! @return       f32 に変換した値を返します。
    //--------------------------------------------------------------------------
    static f32 Bits16ToFloat32(u32 bits16)
    {
        u32 sign = bits16 & SIGN16;
        int exp = static_cast<int>((bits16 & EXP_MASK16) >> FRACTION_WIDTH16);
        u32 fraction = bits16 & FRACTION_MASK16;

        u32 bits32 = 0;
        bits32 |= (sign != 0) ? SIGN32 : 0;

        if ((bits16 & ~SIGN16) == 0)
        {
            exp = 0;
        }
        else
        {
            exp = exp - EXP_BIAS16 + EXP_BIAS32;
        }

        fraction = fraction << (FRACTION_WIDTH32 - FRACTION_WIDTH16);

        // ビット拡張なので、exp がオーバーフローすることは無い。
        bits32 |= fraction & FRACTION_MASK32;
        bits32 |= ((u32)exp & 0xFF) << FRACTION_WIDTH32;

        return *reinterpret_cast<f32*>(&bits32);
    }

    //--------------------------------------------------------------------------
    //! @brief        f32 から 16bit float に変換します。
    //!
    //! @param[in]    value   32bit float の値です。
    //!
    //! @return       16bit 浮動小数のビット表現を返します。
    //--------------------------------------------------------------------------
    static u16 Float32ToBits16(f32 value)
    {
        u32 bits32 = *reinterpret_cast<u32*>(&value);

        u32 sign = bits32 & SIGN32;
        int exp = static_cast<int>((bits32 & EXP_MASK32) >> FRACTION_WIDTH32);
        u32 fraction = bits32 & FRACTION_MASK32;

        u32 bits16 = 0;
        bits16 |= (sign != 0) ? SIGN16 : 0;

        if ((bits32 & ~SIGN32) == 0)
        {
            exp = 0;
        }
        else
        {
            exp = exp - EXP_BIAS32 + EXP_BIAS16;
        }

        fraction = fraction >> (FRACTION_WIDTH32 - FRACTION_WIDTH16);

        if (exp < 0)
        {
            // +0 もしくは -0 なのでそのまま。
        }
        else if (exp > 31)
        {
            // 無限大の処理
            // TODO: IEEE float の無限大の表現がGPU上で有効なのかどうかを要確認。
            bits16 = (u32)0x1F << FRACTION_WIDTH16;
        }
        else
        {
            bits16 |= fraction & FRACTION_MASK16;
            bits16 |= ((u32)exp & 0x1F) << FRACTION_WIDTH16;
        }

        return static_cast<u16>(bits16);
    }

private:
    f32 m_Float32;

    enum
    {
        SIGN32 = 0x80000000,
        SIGN16 = 0x00008000,

        EXP_BIAS32 = 127,
        EXP_BIAS16 = 15,
        EXP_MASK32 = 0x7F800000,
        EXP_MASK16 = 0x00007C00,

        FRACTION_WIDTH32 = 23,
        FRACTION_MASK32  = 0x007FFFFF,
        FRACTION_WIDTH16 = 10,
        FRACTION_MASK16  = 0x000003FF
    };
};

//--------------------------------------------------------------------------
//! @brief        13bit 固定小数用のユーティリティクラスです。
//
// 13bit 固定小数のビットフォーマットは次の通りです。
// | int | decimal | (2の補数)
// int      : 整数部 2 bit.
// decimal  : 小数部 11 bit.
//---------------------------------------------------------------------------
class Fixed13
{
public:
    //--------------------------------------------------------------------------
    //! @brief        デフォルトコンストラクタです。
    //--------------------------------------------------------------------------
    Fixed13() : m_Float32( 0.0f ) {}

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    fixed13   13bit 固定小数のビットを表現した整数値です。
    //--------------------------------------------------------------------------
    explicit Fixed13( u32 fixed13 )
    {
        m_Float32 = Fixed13ToFloat32( fixed13 );
    }

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    fvalue   32bit 浮動小数の値です。
    //--------------------------------------------------------------------------
    explicit Fixed13( f32 fvalue )
    {
        m_Float32 = fvalue;
    }


    f32     GetFloat32Value() const { return m_Float32; }
    u16     GetFixed13Value() const { return Float32ToFixed13( m_Float32 ); }

    //--------------------------------------------------------------------------
    //! @brief        13bit fixed から 32bit float に変換します。
    //!
    //! @param[in]    fixed13   13bit 固定小数のビットを表現した整数値です。
    //!
    //! @return       f32 に変換した値を返します。
    //--------------------------------------------------------------------------
    static f32 Fixed13ToFloat32(u32 fixed13)
    {
        f32 float32 = static_cast<f32>(fixed13);

        if (fixed13 & (0x1 << (TOTAL_WIDTH - 1)))
        {
            float32 -= (0x1 << TOTAL_WIDTH);
        }

        return float32 / (0x1 << DECIMAL_WIDTH);
    }

    //--------------------------------------------------------------------------
    //! @brief        f32 から 13bit fixed に変換します。
    //!
    //! @param[in]    value   32bit float の値です。
    //!
    //! @return       16bit 浮動小数のビット表現を返します。
    //--------------------------------------------------------------------------
    static u16 Float32ToFixed13(f32 value)
    {
        f32 fixed = value;
        fixed += ((0x1 << INT_WIDTH) / 2);
        fixed *= (0x1 << DECIMAL_WIDTH);

        if (fixed < 0)
        {
            fixed = 0.0f;
        }
        else if (fixed >= (0x1 << TOTAL_WIDTH))
        {
            fixed = (0x1 << TOTAL_WIDTH) - 1;
        }

        fixed -= 0x1 << (TOTAL_WIDTH - 1);

        return static_cast<u16>(static_cast<s16>(fixed) & MASK);
    }

private:
    f32 m_Float32;

    enum
    {
        INT_WIDTH = 2,
        DECIMAL_WIDTH = 11,
        TOTAL_WIDTH = 13,
        MASK = (0x1 << TOTAL_WIDTH) - 1
    };
};


//--------------------------------------------------------------------------
//! @brief        11bit 固定小数用のユーティリティクラスです。
//
// 11bit 固定小数のビットフォーマットは次の通りです。
// decimal  : 小数部 11 bit.
//---------------------------------------------------------------------------
class Fixed11
{
public:
    //--------------------------------------------------------------------------
    //! @brief        デフォルトコンストラクタです。
    //--------------------------------------------------------------------------
    Fixed11() : m_Float32( 0.0f ) {}

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    fixed11   11bit 固定小数のビットを表現した整数値です。
    //--------------------------------------------------------------------------
    explicit Fixed11( u32 fixed11 )
    {
        m_Float32 = Fixed11ToFloat32( fixed11 );
    }

    //--------------------------------------------------------------------------
    //! @brief        コンストラクタです。
    //!
    //! @param[in]    fvalue   32bit 浮動小数の値です。
    //--------------------------------------------------------------------------
    explicit Fixed11( f32 fvalue )
    {
        m_Float32 = fvalue;
    }


    f32     GetFloat32Value() const { return m_Float32; }
    u16     GetFixed11Value() const { return Float32ToFixed11( m_Float32 ); }

    //--------------------------------------------------------------------------
    //! @brief        11bit fixed から 32bit float に変換します。
    //!
    //! @param[in]    fixed11   11bit 固定小数のビットを表現した整数値です。
    //!
    //! @return       f32 に変換した値を返します。
    //--------------------------------------------------------------------------
    static f32 Fixed11ToFloat32(u32 fixed11)
    {
        f32 float32 = static_cast<f32>(fixed11);
        return float32 / MASK;
    }

    //--------------------------------------------------------------------------
    //! @brief        f32 から 11bit fixed に変換します。
    //!
    //! @param[in]    value   32bit float の値です。
    //!
    //! @return       16bit 浮動小数のビット表現を返します。
    //--------------------------------------------------------------------------
    static u16 Float32ToFixed11(f32 value)
    {
        u32 fixed;
        u32 v_ = *reinterpret_cast<u32*>(&value);

        if (value <= 0 || (v_ & 0x7f800000) == 0x7f800000)
        {
            fixed = 0;
        }
        else
        {
            value *= 1 << (DECIMAL_WIDTH - 0);
            if (value >= (1 << DECIMAL_WIDTH))
            {
                fixed = (1 << DECIMAL_WIDTH) - 1;
            }
            else
            {
                fixed = (unsigned)(value);
            }
        }

        return static_cast<u16>(static_cast<s16>(fixed) & MASK);
    }

private:
    f32 m_Float32;

    enum
    {
        INT_WIDTH = 2,
        DECIMAL_WIDTH = 11,
        TOTAL_WIDTH = 11,
        MASK = (0x1 << TOTAL_WIDTH) - 1
    };
};

} /* namespace util */
} /* namespace nn */

#endif // __cplusplus

#endif //  NN_UTIL_UTIL_FLOAT24_H_
