/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Utility.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_UTILITY_H_
#define NN_GR_CTR_GR_UTILITY_H_

#include <nn/types.h>
//#include <nn/math.h>
//#include <nn/nstd.h>
//#include <nn/gx.h>
#include <nn/gx/CTR/gx_CommandAccess.h>
//#include <math/include/gfl2_math.h>

#include <math/include/gfl2_math_control.h>
#include <math/include/gfl2_math_crc.h>
#include <math/include/gfl2_math_random.h>
#include <math/include/gfl2_MathCommon.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_tinymt32.h>
#include <math/include/gfl2_Vector.h>


#include "gr_Prefix.h"
#include <memory>

#define NN_GR_ASSERT			assert
#define NN_UNUSED_VAR(var)      ((void)&var);

namespace nn
{
	namespace math
	{
		typedef gfl2::math::Matrix			MTX44;
		typedef gfl2::math::Vector			VEC4;		

		static inline u32 F32AsU32(f32 x)
		{
			return *reinterpret_cast<u32*>(&x);
		}
	}
}


namespace nn
{
    namespace gr
    {
        namespace CTR
        {
            //------------------------------------------------------------------------------

            /*!
                @brief 別のコマンドバッファへジャンプし、そこから元のコマンドバッファへ戻る一連のコマンド生成を補助するクラスです。
            */
            class CommandBufferJumpHelper
            {
            public:
                /*!
                    @brief コンストラクタです。

                    @param[in]  address    書き込み先のコマンドバッファアドレス
                */
                CommandBufferJumpHelper(bit32* address);
                //! @brief デストラクタです。
                ~CommandBufferJumpHelper();

                /*!
                    @brief 作成したコマンドのサイズを取得します。

                    @return これまで作成したコマンドのサイズを返します。
                */
                inline size_t GetSize() const { return GetOffsetFromPtr(m_StartPtr, m_CurrentPtr); }

                /*!
                    @brief コマンドバッファの先頭から、最初のジャンプコマンドまでのサイズを取得します。

                    @return コマンドバッファの先頭から、最初のジャンプコマンドまでのサイズを返します。
                */
                inline size_t GetSizeToFirstJump() const { return m_SizeToFirstJump; }

                /*!
                    @brief コマンドバッファの先頭アドレスを取得します。

                    @return コマンドバッファの先頭アドレスを返します。
                */
                inline bit32* GetStartPtr() { return m_StartPtr; }

                /*!
                    @brief 次のコマンドの書き込み先アドレスを取得します。

                    @return 次のコマンドの書き込み先アドレスを返します。
                */
                inline bit32* GetCurrentPtr() { return m_CurrentPtr; }

                /*!
                    @brief 次のコマンドの書き込み先アドレスを、指定したアドレスに設定します。

                    @param[in] address 変更先のアドレス
                */
                void Put(bit32* address) {
                    //NN_MIN_ASSERT(reinterpret_cast<u32*>(address), reinterpret_cast<u32*>(m_CurrentPtr));
                    m_CurrentPtr = address;
                }

                /*!
                    @brief 次のコマンドの書き込み先アドレスを、指定したアドレスに設定します。

                    @param[in] address 変更先のアドレス
                */
                void Put(uptr address)
                {
                    //NN_MIN_ASSERT(reinterpret_cast<u32*>(address), reinterpret_cast<u32*>(m_CurrentPtr));
                    m_CurrentPtr = reinterpret_cast<bit32*>(address);
                }

                /*!
                    @brief 指定した範囲のコマンドをコピーして書き込みます。

                    @param[in] begin  コピー元コマンドバッファの先頭アドレス
                    @param[in] end    コピー元コマンドバッファの終了アドレス
                */
                void CopyFrom(const bit32* begin, const bit32* end)
                {
                    CopyFrom(begin, end - begin);
                }
                /*!
                    @brief 指定したコマンドバッファをコピーして書き込みます。

                    @param[in] begin  コピー元コマンドバッファの先頭アドレス
                    @param[in] count  コピーしたいコマンドの個数
                */
                void CopyFrom(const bit32* begin, int count)
                {
                    memcpy(m_CurrentPtr, begin, count * sizeof(bit32));
                    m_CurrentPtr += count;
                }

                /*!
                    @brief 別のコマンドバッファへのジャンプのために、一連のコマンドを生成します。

                    @param[in]  subroutine     ジャンプ先のコマンドバッファのアドレス
                    @param[in]  subroutineSize ジャンプ先のコマンドバッファサイズ
                */
                void Jump(uptr subroutine, size_t subroutineSize );

                /*!
                    @brief Jump 関数によるコマンドバッファのジャンプ処理を完成させます。

                    @param[in]  endAddress    書き込み先のコマンドバッファアドレス
                */
                void FinalizeJump(bit32* endAddress);

            private:
                //! 次の書き込み先アドレス
                bit32* m_CurrentPtr;
                //! コマンドバッファの先頭アドレス
                bit32* m_StartPtr;
                //! サブルーチンからの戻り先アドレス
                bit32* m_ReturnPtr;
                //! 最後のキックコマンドとセットで生成された、実行バッファサイズの書き込み先アドレス
                uptr   m_CmdSizePtr;
                //! コマンドバッファ先頭ポインタから、最初のキック（ジャンプ）コマンドまでのバッファサイズ
                u32    m_SizeToFirstJump;

                //! バッファサイズを取得します。
                inline size_t GetOffsetFromPtr( bit32* start, bit32* end ) const
                {
                    return (end - start) * sizeof(bit32);
                }
            };

            /*!
                @brief 指定したサイズ分のコマンド追加時に、バッファサイズが 16 の倍数となるように補正します。

                @param[in] command   次のコマンドの書き込み先アドレス
                @param[in] addSize   今後追加する予定のコマンドも含めた、コマンドバッファサイズ

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
            */
            bit32* AddDummyDataForCommandBuffer( bit32* command, const size_t addSize );

#if 0
            //------------------------------------------------------------------------------
            /*!
                @brief 3x4行列をwHzyxの順に逆転してコピーします。（ヘッダーあり）

                @param[in] dst 結果を書き込む先のポインタです。
                @param[in] src 34行列です。
                @param[in] header ヘッダです。
            */
            void CopyMtx34WithHeader(
                f32*  dst,
                const nn::math::MTX34* src,
                bit32   header );
#endif

            /*!
                @brief 4x4行列をwHzyxの順に逆転してコピーします。（ヘッダーあり）

                @param[in] dst 結果を書き込む先のポインタです。
                @param[in] src 44行列です。
                @param[in] header ヘッダです。
            */
            void CopyMtx44WithHeader(
                f32* dst,
                const nn::math::MTX44* src,
                bit32 header );

#if 0
            //------------------------------------------------------------------------------------

            /*!
                 @brief 34行列を頂点シェーダのユニフォームにセットするコマンドを生成します。

                 @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                 @param[in] location     レジスタの場所です。
                 @param[in] mtx34        34行列です。

                 @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandVS( bit32* command, u8 location, const nn::math::MTX34& mtx34 )
            {
                *command++ = 0x80000000 | location;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_FLOAT_ADDR );
                CopyMtx34WithHeader( reinterpret_cast<f32*>( command ), &mtx34, PICA_CMD_HEADER_VS_F32( 3 ) );
                return command + 14;
            }
#endif

            //------------------------------------------------------------------------------------

            /*!
                @brief 44行列を頂点シェーダのユニフォームにセットするコマンドを生成します。

                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                @param[in] location     レジスタの場所です。
                @param[in] mtx44        44行列です。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandVS( bit32* command, u8 location, const nn::math::MTX44& mtx44 )
            {
                *command++ = 0x80000000 | location;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_FLOAT_ADDR );
                CopyMtx44WithHeader( reinterpret_cast<f32*>( command ), &mtx44, PICA_CMD_HEADER_VS_F32( 4 ) );
                return command + 18;
            }

            //------------------------------------------------------------------------------------

            /*!
                @brief ベクトルを頂点シェーダのユニフォームにセットするコマンドを生成します。

                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                @param[in] location     レジスタの場所です。
                @param[in] vec4         4次元のベクトルです。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandVS( bit32* command, u8 location, const nn::math::VEC4& vec4 )
            {
                // 0x2c0
                *command++ = 0x80000000 | location;
                *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_VS_FLOAT_ADDR, 5 );
                // 0x2c1-0x2c4
                *command++ = nn::math::F32AsU32( vec4.w );
                *command++ = nn::math::F32AsU32( vec4.z );
                *command++ = nn::math::F32AsU32( vec4.y );
                *command++ = nn::math::F32AsU32( vec4.x );
                return command;
            }

            
            //------------------------------------------------------------------------------------

            /*!
                @brief ベクトルの配列を頂点シェーダのユニフォームにセットするコマンドを生成します。
                
                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                @param[in] location     レジスタの場所です。
                @param[in] vec4         4次元のベクトルの配列です。
                @param[in] num          配列のサイズです。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandVS( bit32* command, u8 location, const nn::math::VEC4 vec4[], const int num )
            {
                const u32 MAX_VS_F32_VEC4_NUM = 64;
                NN_GR_ASSERT( num <= MAX_VS_F32_VEC4_NUM );

                NN_UNUSED_VAR( MAX_VS_F32_VEC4_NUM );

                // 0x2c0
                *command++ = 0x80000000 | location;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_FLOAT_ADDR );
                
                // 0x2c1
                *command++ = nn::math::F32AsU32( vec4[0].w );
                *command++ = PICA_CMD_HEADER_VS_F32( num );
                *command++ = nn::math::F32AsU32( vec4[0].z );
                *command++ = nn::math::F32AsU32( vec4[0].y );
                *command++ = nn::math::F32AsU32( vec4[0].x );

                for ( int i = 1; i < num; ++i )
                {
                    *command++ = nn::math::F32AsU32( vec4[i].w );
                    *command++ = nn::math::F32AsU32( vec4[i].z );
                    *command++ = nn::math::F32AsU32( vec4[i].y );
                    *command++ = nn::math::F32AsU32( vec4[i].x );
                }

                *command++ = 0; // padding
                
                return command;
            }
            
            //------------------------------------------------------------------------------------

            /*!
                @brief 整数を頂点シェーダのユニフォームにセットするコマンドを生成します。

                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                @param[in] location     レジスタの場所です。
                @param[in] x            x 座標の値です。
                @param[in] y            y 座標の値です。
                @param[in] z            z 座標の値です。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandVS( bit32* command, u8 location, u8 x, u8 y, u8 z )
            {
                *command++ = PICA_CMD_DATA_VS_INT( x, y, z );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_INT0 + location );
                return command;
            }

#if 0
            //------------------------------------------------------------------------------------

            /*!
                @brief 34行列をジオメトリシェーダのユニフォームにセットするコマンドを生成します。

                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                @param[in] location     レジスタの場所です。
                @param[in] mtx34        34行列です。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandGS( bit32* command, u8 location, const nn::math::MTX34& mtx34 )
            {
                *command++ = 0x80000000 | location;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_FLOAT_ADDR );
                CopyMtx34WithHeader( reinterpret_cast<f32*>( command ), &mtx34, PICA_CMD_HEADER_GS_F32( 3 ) );
                return command + 14;
            }
#endif

            //------------------------------------------------------------------------------------

            /*!
                @brief 44行列をジオメトリシェーダのユニフォームにセットするコマンドを生成します。

                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                @param[in] location     レジスタの場所です。
                @param[in] mtx44        44行列です。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandGS( bit32* command, u8 location, const nn::math::MTX44& mtx44 )
            {
                *command++ = 0x80000000 | location;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_FLOAT_ADDR );
                CopyMtx44WithHeader( reinterpret_cast<f32*>( command ), &mtx44, PICA_CMD_HEADER_GS_F32( 4 ) );
                return command + 18;
            }

            //------------------------------------------------------------------------------------

            /*!
                @brief ベクトルをジオメトリシェーダのユニフォームにセットするコマンドを生成します.
                
                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                @param[in] location     レジスタの場所です。
                @param[in] vec4         4次元のベクトルです。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandGS( bit32* command, u8 location, const nn::math::VEC4& vec4 )
            {
                *command++ = 0x80000000 | location;
                *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_GS_FLOAT_ADDR, 5 );
                *command++ = nn::math::F32AsU32( vec4.w ); // a
                *command++ = nn::math::F32AsU32( vec4.z ); // b
                *command++ = nn::math::F32AsU32( vec4.y ); // g
                *command++ = nn::math::F32AsU32( vec4.x ); // r
                return command;
            }
            
            //------------------------------------------------------------------------------------

            /*!
                @brief ベクトルの配列をジオメトリシェーダのユニフォームにセットするコマンドを生成します。

                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                @param[in] location     レジスタの場所です。
                @param[in] vec4         4次元のベクトルの配列です。
                @param[in] num          配列のサイズです。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandGS( bit32* command, u8 location, const nn::math::VEC4 vec4[], const int num )
            {
                const u32 MAX_GS_F32_VEC4_NUM = 64;
                NN_GR_ASSERT( num <= MAX_GS_F32_VEC4_NUM );
                
                NN_UNUSED_VAR( MAX_GS_F32_VEC4_NUM );

                *command++ = 0x80000000 | location;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_FLOAT_ADDR );
                *command++ = nn::math::F32AsU32( vec4[0].w );
                *command++ = PICA_CMD_HEADER_GS_F32( num );
                *command++ = nn::math::F32AsU32( vec4[0].z );
                *command++ = nn::math::F32AsU32( vec4[0].y );
                *command++ = nn::math::F32AsU32( vec4[0].x );

                for ( int i = 1; i < num; ++i )
                {
                    *command++ = nn::math::F32AsU32( vec4[i].w );
                    *command++ = nn::math::F32AsU32( vec4[i].z );
                    *command++ = nn::math::F32AsU32( vec4[i].y );
                    *command++ = nn::math::F32AsU32( vec4[i].x );
                }

                *command++ = 0; // padding
                
                return command;
            }
            
            //------------------------------------------------------------------------------------

            /*!
                @brief 整数をジオメトリシェーダのユニフォームにセットするコマンドを生成します。

                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。
                @param[in] location     レジスタの場所です。
                @param[in] x            x 座標の値です。
                @param[in] y            y 座標の値です。
                @param[in] z            z 座標の値です。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
             */
            inline bit32* MakeUniformCommandGS( bit32* command, u8 location, u8 x, u8 y, u8 z )
            {
                *command++ = PICA_CMD_DATA_GS_INT( x, y, z );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_INT0 + location );
                return command;
            }

            //------------------------------------------------------------------------------------

            /*!
                @brief float32 を u8 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です。
            */
            inline u8 FloatToUnsignedByte(
                f32 val
                )
            {
                return ( u8 )( 0.5f + ( val < 0.f ? 0.f : ( 1.f < val ? 1.f : val ) )  * ( 0xff ) );
            }

            //------------------------------------------------------------------------------------
            
            /*!
                @brief float32 を クランプせずに u8 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u8 FloatToUnsignedByteNoClamp(
                f32 val
                )
            {
                return ( u8 )( 0.5f + val * 0xff );
            }

            //------------------------------------------------------------------------------------
            
            /*!
                @brief float32 から float16 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u16 Float32ToFloat16( f32 val )
            {
                static const int bias_ = 128 - (1 << (5 - 1));

                u32 uval_ = *( reinterpret_cast<u32*>( &val ) );
                int e_ = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0;
                if (e_ >= 0)
                {
                    return ( u16 )( ((uval_ & 0x7fffff) >> (23 - 10)) | (e_ << 10) | ((uval_ >> 31) << (10 + 5)) );
                }
                return ( u16 )((uval_ >> 31) << (10 + 5));
            }

            //------------------------------------------------------------------------------------
            
            /*!
                @brief float32 から float24 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToFloat24( f32 val )
            {
                static const int bias_ = 128 - (1 << (7 - 1));
                u32 uval_   = *( reinterpret_cast<unsigned*>( &val ) );
                s32 e_      = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0;

                return e_ >= 0 ? ((uval_ & 0x7fffff) >> (23 - 16)) | (e_ << 16) | ((uval_ >> 31) << (16 + 7)) : ((uval_ >> 31) << (16 + 7));
            }

            //------------------------------------------------------------------------------------

            /*!
                @brief float32 から float20 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToFloat20( f32 val )
            {
                static const int bias_ = 128 - (1 << (7 - 1));
                u32 uval_   = *( reinterpret_cast<unsigned*>( &val ) );
                s32 e_      = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0;

                return e_ >= 0 ? ((uval_ & 0x7fffff) >> (23 - 12)) | (e_ << 12) | ((uval_ >> 31) << (12 + 7)) : ((uval_ >> 31) << (12 + 7));
            }

            //------------------------------------------------------------------------------------
            
            /*!
                @brief float32 から float31 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToFloat31( f32 val )
            {
                unsigned uval_, m_;
                int e_;
                float f_ = val;
                static const int bias_ = 128 - (1 << (7 - 1));
                uval_ = *( reinterpret_cast<unsigned*>( &f_ ) );
                e_ = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0;
                m_ = (uval_ & 0x7fffff) >> (23 - 23);
                return e_ >= 0 ? m_ | (e_ << 23) | ((uval_ >> 31) << (23 + 7)) : ((uval_ >> 31) << (23 + 7));
            }

            //------------------------------------------------------------------------------------            
            /*!
                @brief float32 から 符号なし fixed24 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToUnsignedFix24( f32 val )
            {
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val ) );

                if (val <= 0 || (v_ & 0x7f800000) == 0x7f800000)
                {
                    return 0;
                }
                else
                {
                    val *= 1 << (24 - 0);

                    if ( val >= (1 << 24) )
                    {
                        return (1 << 24) - 1;
                    }
                    else
                    {
                        return (unsigned)(val);
                    }
                }
            }

            //------------------------------------------------------------------------------------            
            /*!
                @brief float32 から 符号なし fixed16 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToUnsignedFix16( f32 val )
            {
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val) );

                if (val <= 0 || (v_ & 0x7f800000) == 0x7f800000)
                {
                    return 0;
                }
                else
                {
                    val *= 1 << (16 - 0);
                    if ( val >= (1 << 16) )
                    {
                                return (1 << 16) - 1;
                    }
                    else
                    {
                                return (unsigned)( val );
                    }
                }
            }

            //------------------------------------------------------------------------------------            
            /*!
                @brief float32 から 符号あり fixed16 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u16 Float32ToFix16( f32 val )
            {                
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val ) );

                if ( (val == 0.f) || ( (v_ & 0x7f800000) == 0x7f800000 ) )
                {
                    return 0;
                }
                else
                {
                    val += 0.5f * (1 << 4);
                    val *= 1 << (16 - 4);
                    if (val < 0)
                    {
                        val = 0;
                    }
                    else if (val >= (1 << 16))
                    {
                        val = (1 << 16) - 1;
                    }

                    if (val >= (1 << (16 - 1)))
                    {
                        return (unsigned)(val - (1 << (16 - 1)));
                    }
                    else
                    {
                        return (unsigned)(val + (1 << (16 - 1)));
                    }
                }
            }

            //------------------------------------------------------------------------------------

            /*!
                @brief float32 から 符号なし fixed12 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToUnsignedFix12( f32 val )
            {
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val ) );
                if( val <= 0 || (v_ & 0x7f800000) == 0x7f800000 )
                {
                    return 0;
                }
                else
                {
                    unsigned uval_;

                    val *= 1 << (12 - 0);
                    if (val >= (1 << 12))
                    {
                        uval_ = (1 << 12) - 1;
                    }
                    else
                    {
                        uval_ = (unsigned)(val);
                    }

                    return uval_;
                }
            }

            //------------------------------------------------------------------------------------
            
            /*!
                @brief float32 から 符号あり fixed12 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToFix12( f32 val )
            {
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val ) );
                if( val == 0.f || (v_ & 0x7f800000) == 0x7f800000 )
                    return 0;

                int ret;

                val *= (1 << (12 - 1));

                if( val < 0 )
                {
                    ret = 1 << (12 - 1);
                    val = -val;
                }
                else
                    ret = 0;

                if( val >= (1 << (12 - 1)) )
                    val  = (1 << (12 - 1)) - 1;

                ret |= (unsigned)(val);
                return ret;
            }

            /*!
                @brief float32 から fixed12 ( fraction 11) に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToFix12Fraction11( f32 val )
            {                                
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val ) );

                if (val == 0.f || (v_ & 0x7f800000) == 0x7f800000)
                {
                    return 0;
                }
                else
                {
                    val += 0.5f * (1 << 1);
                    val *= 1 << (12 - 1);
                    
                    if (val < 0)
                    {
                        val = 0;
                    }
                    else if (val >= (1 << 12))
                    {
                            val = (1 << 12) - 1;
                    }

                    if (val >= (1 << (12 - 1)))
                    {
                        return (unsigned)(val - (1 << (12 - 1)));
                    }
                    else
                    {
                        return (unsigned)(val + (1 << (12 - 1)));
                    }
                }
            }

            //------------------------------------------------------------------------------------
            
            /*!
                @brief float32 から fixed13 ( fraction 8) に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToFix13Fraction8( f32 val )
            {
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val ) );
                if ( ( val == 0.0f ) || 
                     ( (v_ & 0x7f800000) == 0x7f800000 ) )
                {
                    return 0;
                }

                val += 0.5f * (1 << 5);
                val *= 1 << (13 - 5);
                if (val < 0)
                {
                    val = 0;
                }
                else if (val >= (1 << 13))
                {
                    val = (1 << 13) - 1;
                }

                return (val >= (1 << (13 - 1))) ? (unsigned)(val - (1 << (13 - 1))) : (unsigned)(val + (1 << (13 - 1)));
            }

            //------------------------------------------------------------------------------------
           
            /*!
                @brief float32 から fixed11 ( fraction 8) に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToFix13Fraction11( f32 val )
            {
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val ) );
                if( val == 0.f || (v_ & 0x7f800000) == 0x7f800000 )
                    return 0;

                val += 0.5f * (1 << 2);
                val *= 1 << (13 - 2);
                if (val < 0)
                    val = 0;
                else if (val >= (1 << 13))
                    val = (1 << 13) - 1;

                return (val >= (1 << (13 - 1))) ? (unsigned)(val - (1 << (13 - 1))) : (unsigned)(val + (1 << (13 - 1)));
            }

            //------------------------------------------------------------------------------------
            
            /*!
                @brief float32 から 符号無し fixed11 に変換します。

                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u32 Float32ToUnsignedFix11( f32 val )
            {
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val ) );
                if( val <= 0 || (v_ & 0x7f800000) == 0x7f800000 )
                    return 0;

                unsigned uval_;

                val *= 1 << (11 - 0);
                if (val >= (1 << 11))
                    uval_ = (1 << 11) - 1;
                else
                    uval_ = (unsigned)(val);

                return uval_;
            }

             //------------------------------------------------------------------------------------
            
            /*!
                @brief float32 から fixed8 ( 7bits fraction ) に変換します。
                
                @param[in] val      変換前の値です。

                @return             変換後の値です
            */
            inline u8 Float32ToFix8Fraction7( f32 val )
            {
                unsigned v_ = *( reinterpret_cast<unsigned*>( &val ) );
                
                if (val == 0.f || (v_ & 0x7f800000) == 0x7f800000)
                {
                    return 0;
                }
                else
                {
                    val += 0.5f * (1 << 1);
                    val *= 1 << (8 - 1);
                    
                    if (val < 0)
                    {
                        val = 0;
                    }
                    else if (val >= (1 << 8))
                    {
                        val = (1 << 8) - 1;
                    }
                    
                    if (val >= (1 << (8 - 1)))
                    {
                        return (unsigned)(val - (1 << (8 - 1)));
                    }
                    else
                    {
                        return (unsigned)(val + (1 << (8 - 1)));
                    }
                }
            }

            //------------------------------------------------------------------------------------
            /*!
                @brief 頂点配列の型から、バイトサイズを求めます。

                @param[in] type      頂点配列の型です。

                @return             バイトサイズです。
            */
            inline u32 PicaDataVertexAttrTypeToByteSize( const PicaDataVertexAttrType type )
            {
                switch ( type )
                {
                    case PICA_DATA_SIZE_1_BYTE          : return 1 * sizeof(  s8 );
                    case PICA_DATA_SIZE_1_UNSIGNED_BYTE : return 1 * sizeof(  u8 );
                    case PICA_DATA_SIZE_1_SHORT         : return 1 * sizeof( s16 );
                    case PICA_DATA_SIZE_1_FLOAT         : return 1 * sizeof( f32 );
                    case PICA_DATA_SIZE_2_BYTE          : return 2 * sizeof(  s8 );
                    case PICA_DATA_SIZE_2_UNSIGNED_BYTE : return 2 * sizeof(  u8 );
                    case PICA_DATA_SIZE_2_SHORT         : return 2 * sizeof( s16 );
                    case PICA_DATA_SIZE_2_FLOAT         : return 2 * sizeof( f32 );
                    case PICA_DATA_SIZE_3_BYTE          : return 3 * sizeof(  s8 );
                    case PICA_DATA_SIZE_3_UNSIGNED_BYTE : return 3 * sizeof(  u8 );
                    case PICA_DATA_SIZE_3_SHORT         : return 3 * sizeof( s16 );
                    case PICA_DATA_SIZE_3_FLOAT         : return 3 * sizeof( f32 );
                    case PICA_DATA_SIZE_4_BYTE          : return 4 * sizeof(  s8 );
                    case PICA_DATA_SIZE_4_UNSIGNED_BYTE : return 4 * sizeof(  u8 );
                    case PICA_DATA_SIZE_4_SHORT         : return 4 * sizeof( s16 );
                    case PICA_DATA_SIZE_4_FLOAT         : return 4 * sizeof( f32 );
                }
                return 0;
            }


            //------------------------------------------------------------------------------------

            /*!
                @brief グラフィクスの機能を無効化するコマンドを生成します。
                       Shader, Vertex, Fog/Gas, FragmentLight, Texture,
                       Early depth test, Clipping Plane, RenderState, RenderBlockMode を無効化します。

                @param[in] command      描画コマンドの書き込み先の先頭アドレスです。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
            */
            bit32* MakeDisableAllCommand( bit32* command );

            //------------------------------------------------------------------------------------

            /*!
                @brief コマンドバッファ実行のためのチャネルを表します。
            */
            enum CommandBufferChannel
            {
                //! チャネル 0
                CMD_BUF_CH0 = 0x0,
                //! チャネル 1
                CMD_BUF_CH1 = 0x1
            };

            /*!
                @brief     指定したチャネルのキックコマンドを追加します。

                @param[in] command    描画コマンドの書き込み先の先頭アドレスです。
                @param[in] channel    チャネル番号です。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
            */
            bit32* MakeChannelKickCommand( bit32* command, const CommandBufferChannel channel );

            //------------------------------------------------------------------------------------

            /*!
                @brief     別のコマンドバッファへジャンプ（チャネル0で実行）し、そこから元のコマンドバッファへ戻るためのコマンドセットを追加します。

                @param[in]  command           描画コマンドの書き込み先の先頭アドレスです。
                @param[out] bufferSizePtr     ジャンプ先から戻った後に実行されるコマンドバッファのバイトサイズを格納するデータアドレスです。
                @param[in]  commandBufferPtr  ジャンプ先コマンドバッファのアドレスです。
                @param[in]  commandBufferSize ジャンプ先コマンドバッファのバイトサイズです。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
            */
            bit32* MakeChannel0SubroutineCommand( bit32* command, uptr* bufferSizePtr, const uptr commandBufferPtr, const size_t commandBufferSize );

            //------------------------------------------------------------------------------------

            /*!
                @brief     別のコマンドバッファへジャンプ（チャネル1で実行）し、そこから元のコマンドバッファへ戻るためのコマンドセットを追加します。

                @param[in]  command           描画コマンドの書き込み先の先頭アドレスです。
                @param[out] bufferSizePtr     ジャンプ先から戻った後に実行されるコマンドバッファのバイトサイズを格納するデータアドレスです。
                @param[in]  commandBufferPtr  ジャンプ先コマンドバッファのアドレスです。
                @param[in]  commandBufferSize ジャンプ先コマンドバッファのバイトサイズです。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
            */
            bit32* MakeChannel1SubroutineCommand( bit32* command, uptr* bufferSizePtr, const uptr commandBufferPtr, const size_t commandBufferSize );

            //------------------------------------------------------------------------------------

            /*!
                @brief     別のコマンドバッファへジャンプ（チャネル0で実行）させるコマンドセットを追加します。

                @param[in] command            描画コマンドの書き込み先の先頭アドレスです。
                @param[in] commandBufferPtr   ジャンプ先コマンドバッファのアドレスです。
                @param[in] commandBufferSize  ジャンプ先コマンドバッファのバイトサイズです。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
            */
            bit32* MakeChannel0JumpCommand( bit32* command, const uptr commandBufferPtr, const size_t commandBufferSize );

            //------------------------------------------------------------------------------------

            /*!
                @brief     別のコマンドバッファへジャンプ（チャネル1で実行）させるコマンドセットを追加します。

                @param[in] command            描画コマンドの書き込み先の先頭アドレスです。
                @param[in] commandBufferPtr   ジャンプ先コマンドバッファのアドレスです。
                @param[in] commandBufferSize  ジャンプ先コマンドバッファのバイトサイズです。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
            */
            bit32* MakeChannel1JumpCommand( bit32* command, const uptr commandBufferPtr, const size_t commandBufferSize );

            /*!
                @brief ポスト頂点キャッシュのエントリ数のパターンです。
            */
            enum PostVertexCacheEntryType
            {
                //! 32個
                POST_VERTEX_CACHE_ENTRY_TYPE_32 = 0x0,
                //! 16個
                POST_VERTEX_CACHE_ENTRY_TYPE_16 = 0x1
            };

            /*!
                @brief     ポスト頂点キャッシュのエントリ数を指定した数へ変更します。

                @param[in] command            描画コマンドの書き込み先の先頭アドレスです。
                @param[in] type               変更したいエントリ数を指定します。

                @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
            */
            bit32* MakePostVertexCacheEntryTypeCommand( bit32* command, const PostVertexCacheEntryType type );

            //------------------------------------------------------------------------------------

            inline void CommandBufferJumpHelper::Jump(uptr subroutine, size_t subroutineSize )
            {
                if ( !subroutine )
                    return;

                // nn::gr::MakeChannel0SubroutineCommand 関数の使用が前提なので、追加されるコマンドのサイズを 24 バイトとする。
                m_CurrentPtr = AddDummyDataForCommandBuffer(m_CurrentPtr, GetOffsetFromPtr(m_ReturnPtr, m_CurrentPtr) + 24);

                uptr cmdSizePtr;
                m_CurrentPtr = MakeChannel0SubroutineCommand(m_CurrentPtr, &cmdSizePtr, subroutine, subroutineSize);

                if ( m_CmdSizePtr )
                {
                    *(reinterpret_cast<u32*>(m_CmdSizePtr)) = GetOffsetFromPtr(m_ReturnPtr, m_CurrentPtr) >> 3;
                }
                m_CmdSizePtr = cmdSizePtr;
                m_ReturnPtr  = m_CurrentPtr;

                if ( m_SizeToFirstJump == 0 )
                {
                    m_SizeToFirstJump = GetOffsetFromPtr(m_StartPtr, m_CurrentPtr);
                }
            }
        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_UTILITY_H_
