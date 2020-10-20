/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_BindSymbol.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_BIND_SYMBOL_H_
#define NN_GR_CTR_GR_BIND_SYMBOL_H_

#include "gr_Utility.h"

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief 各シンボルについての情報をやり取りするためのクラスです。
            */
            class BindSymbol
            {
            public :
                /*!
                    @brief シンボルの型情報を表します。
                 */
                enum SymbolType
                {
                    //! 無効な型です。
                    SYMBOL_TYPE_INVALID,
                    //! シェーダーへの入力を表します。v0 ... v15 に対応します。
                    SYMBOL_TYPE_INPUT,
                    //! 浮動小数レジスタです。 c0 ... c95に対応します。
                    SYMBOL_TYPE_FLOAT,
                    //! 整数レジスタです。i0 ... i3に対応します。
                    SYMBOL_TYPE_INTEGER,
                    //! ブールレジスタです。b0 ... b15に対応します。
                    SYMBOL_TYPE_BOOL
                };

                /*!
                    @brief シェーダの種類を表します。
                 */
                enum ShaderType
                {
                    //! 頂点シェーダーを表します。
                    SHADER_TYPE_VERTEX,
                    //! ジオメトリシェーダーを表します。
                    SHADER_TYPE_GEOMETRY
                };

                /*!
                    @brief バインドされているシェーダーの種類です。
                           型は nn::gr::CTR::BindSymbol::ShaderType です。
                 */
                ShaderType shaderType;

                /*!
                    @brief シンボルの型情報です。
                           型は nn::gr::CTR::BindSymbol::SymbolType です。
                 */
                SymbolType symbolType;

                /*!
                    @brief バインドされている先頭のレジスタ番号です。
                           型は u8 です。
                */
                u8 start;

                /*!
                    @brief バインドされている最終のレジスタ番号です。
                           型は u8 です。
                */
                u8 end;

                /*!
                    @brief バインド名が格納されている、シェーダーバイナリデータ内のアドレス指すポインタです。
                           型は const char* です。
                */
                char* name;

            protected :
                /*!
                    @brief コンストラクタです。直接生成することを禁止します。

                    @param[in] shader_type シェーダの種類です。
                    @param[in] symbol_type シンボルの型情報です。
                */
                explicit BindSymbol( ShaderType shader_type, SymbolType symbol_type )
                    : shaderType( shader_type ),
                      symbolType( symbol_type ),
                      start( 0xff ),
                      end( 0xff ),
                      name( NULL )
                {
                }
            };

            /*!
                @brief 頂点シェーダーの入力レジスタシンボルについての情報をやり取りするためのクラスです。
             */
            class BindSymbolVSInput : public BindSymbol
            {
            public :
                /*!
                    @brief コンストラクタです。
                */
                explicit BindSymbolVSInput() : 
                BindSymbol( SHADER_TYPE_VERTEX, SYMBOL_TYPE_INPUT )
                {
                }
            };

            /*!
                @brief 頂点シェーダーの浮動小数レジスタシンボルについての情報をやり取りするためのクラスです。
             */
            class BindSymbolVSFloat : public BindSymbol
            {
            public :
                /*!
                    @brief コンストラクタです。
                */
                explicit BindSymbolVSFloat() : 
                BindSymbol( SHADER_TYPE_VERTEX, SYMBOL_TYPE_FLOAT )
                {
                }

#if 0
                /*!
                    @brief 34行列を頂点シェーダのユニフォームにセットするコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] mtx34   34行列です。

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, const nn::math::MTX34& mtx34 ) const
                {
                    return MakeUniformCommandVS( command, start, mtx34 );
                }
#endif

                /*!
                    @brief 44行列を頂点シェーダのユニフォームにセットするコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] mtx44   44行列です。

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, const nn::math::MTX44& mtx44 ) const
                {
                    return MakeUniformCommandVS( command, start, mtx44 );
                }

                /*!
                    @brief 4次元のベクトルを頂点シェーダのユニフォームにセットするコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] vec4    4次元のベクトルです。

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, const nn::math::VEC4& vec4 ) const
                {
                    return MakeUniformCommandVS( command, start, vec4 );
                }

                /*!
                    @brief 4次元のベクトルの配列を頂点シェーダのユニフォームにセットするコマンドを生成します.

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @param[in] vec4[]  設定する4次元ベクトルの配列です。
                    @param[in] num     設定するベクトルの数です。( 2以上を設定するようにして下さい。)

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, const nn::math::VEC4 vec4[], const int num ) const
                {
                    return MakeUniformCommandVS( command, start, vec4, num );
                }
            };

            /*!
                @brief 整数レジスタシンボルについての情報をやり取りするためのクラスです。
             */
            class BindSymbolVSInteger : public BindSymbol
            {
            public :
                explicit BindSymbolVSInteger() :
                BindSymbol( SHADER_TYPE_VERTEX, SYMBOL_TYPE_INTEGER )
                {
                }

                /*!
                    @brief 整数を頂点シェーダのユニフォームにセットするコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] x       x座標の値です。
                    @param[in] y       y座標の値です。
                    @param[in] z       z座標の値です。

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, u8 x, u8 y, u8 z ) const
                {
                    return MakeUniformCommandVS( command, start, x, y, z );
                }
            };

            /*!
                @brief ブールレジスタシンボルについての情報をやり取りするためのクラスです。
             */
            class BindSymbolVSBool : public BindSymbol
            {
            public :
                /*!
                    @brief コンストラクタです。
                */
                explicit BindSymbolVSBool( void ) :
                BindSymbol( SHADER_TYPE_VERTEX, SYMBOL_TYPE_BOOL )
                {
                }
            };

            /*!
                @brief ジオメトリシェーダーの浮動小数レジスタシンボルについての情報をやり取りするためのクラスです。
             */
            class BindSymbolGSFloat : public BindSymbol
            {
            public :
                /*!
                    @brief コンストラクタです。
                */
                explicit BindSymbolGSFloat( void ) : 
                BindSymbol( SHADER_TYPE_GEOMETRY, SYMBOL_TYPE_FLOAT )
                {
                }

#if 0
                /*!
                    @brief 34行列をジオメトリシェーダのユニフォームにセットするコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] mtx34   34行列です。

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, const nn::math::MTX34& mtx34 ) const
                {
                    return MakeUniformCommandGS( command, start, mtx34 );
                }
#endif

                /*!
                    @brief 44行列をジオメトリシェーダのユニフォームにセットするコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] mtx44   44行列です。

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, const nn::math::MTX44& mtx44 ) const
                {
                    return MakeUniformCommandGS( command, start, mtx44 );
                }

                /*!
                    @brief ベクトルをジオメトリシェーダのユニフォームにセットするコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] vec4    4次元のベクトルです。

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, const nn::math::VEC4& vec4 ) const
                {
                    return MakeUniformCommandGS( command, start, vec4 );
                }

                /*!
                    @brief 4次元ベクトルの配列をジオメトリシェーダのユニフォームにセットするコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] vec4[]  設定する値の配列です。
                    @param[in] num     設定するベクトルの数です。( 2以上を設定して下さい。)

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, const nn::math::VEC4 vec4[], const int num ) const
                {
                    return MakeUniformCommandGS( command, start, vec4, num );
                }

                /*!
                    @brief 整数をジオメトリシェーダのユニフォームにセットするコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] x       x座標の値です。
                    @param[in] y       y座標の値です。
                    @param[in] z       z座標の値です。

                    @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeUniformCommand( bit32* command, u8 x, u8 y, u8 z ) const
                {
                    return MakeUniformCommandGS( command, start, x, y, z );
                }
            };

            /*!
                @brief ブールレジスタシンボルについての情報をやり取りするためのクラスです。
             */
            class BindSymbolGSBool : public BindSymbol
            {
            public :
                /*!
                    @brief コンストラクタです。
                */
                explicit BindSymbolGSBool() :
                BindSymbol( SHADER_TYPE_GEOMETRY, SYMBOL_TYPE_BOOL )
                {
                }
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_BIND_SYMBOL_H_
