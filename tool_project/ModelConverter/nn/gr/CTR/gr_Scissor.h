/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Scissor.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_SCISSOR_H_
#define NN_GR_CTR_GR_SCISSOR_H_

#include <nn/gr/CTR/gr_Utility.h>

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief シザー関連のコマンドを生成するためのクラスです。glScissorと異なり、描画対象のカラーバッファサイズを与える必要があります。
            */
            class Scissor
            {
            public:
                /*!
                    @brief コンストラクタです。初期値(シザー有効、シザー範囲 (0,0)-(240,320)、 バッファサイズ(256,320))を設定します.
                 */
                Scissor():
                isEnable( true ),
                x( 0 ),
                y( 0 ),
                width( 240 ),
                height( 320 ),
                bufferWidth( 256 ),
                bufferHeight( 320 )
                {
                }

                /*!
                    @brief コンストラクタです。値を指定して初期化します。

                    @param[in]  isEnable_     シザーを有効にするか。
                    @param[in]  x_            シザー範囲の左下の X の値です。
                    @param[in]  y_            シザー範囲の左下の Y の値です。
                    @param[in]  width_        シザー範囲の幅です。
                    @param[in]  height_       シザー範囲の高さです。
                    @param[in]  bufferWidth_  カラーバッファの幅です。
                    @param[in]  bufferHeight_ カラーバッファの高さです。
                */
                Scissor( bool isEnable_, s32 x_, s32 y_, u32 width_, u32 height_, s32 bufferWidth_, s32 bufferHeight_ ) :
                isEnable( isEnable_ ),
                x( x_ ),
                y( y_ ),
                width( width_ ),
                height( height_ ),
                bufferWidth( bufferWidth_ ),
                bufferHeight( bufferHeight_ )
                {
                }
                
            public:
                /*!
                    @brief 設定された情報をもとに、描画コマンドを生成します。
                 
                    @param[in]    command  描画コマンド書き込み先の先頭アドレスです。

                    @return                書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeCommand( bit32* command ) const;

                /*!
                    @brief シザーを無効化するコマンドを生成します。

                    @param[in]  command            描画コマンド書き込み先の先頭アドレスです。
                    @param[in]  colorBufferWidth   カラーバッファの幅です。
                    @param[in]  colorBufferHeight  カラーバッファの高さです。

                    @return                 書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeDisableCommand( bit32* command,
                    const s32 colorBufferWidth,
                    const s32 colorBufferHeight )
                {
                    s32 temp_width  = colorBufferWidth  - 1;
                    s32 temp_height = colorBufferHeight - 1;
                
                    // 0x65-0x67
                    *command++ = PICA_CMD_DATA_SCISSOR( false );
                    *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_SCISSOR, 3 );
                    *command++ = 0;
                    *command++ = PICA_CMD_DATA_SCISSOR_SIZE( temp_width, temp_height );

                    return command;
                }
               
            public:
                /*!
                    @brief シザー範囲を一括で指定する便利関数です。

                    @param[in]    x_            シザー範囲の左下の X の値です。
                    @param[in]    y_            シザー範囲の左下の Y の値です。
                    @param[in]    width_        シザー範囲の幅です。
                    @param[in]    height_       シザー範囲の高さです。
                 */
                void Set( s32 x_, s32 y_, u32 width_, u32 height_ )
                {
                    x = x_; y = y_; width = width_; height = height_;
                }
                
                /*!
                    @brief バッファサイズを一括で指定する便利関数です。

                    @param[in]    bufferWidth_  カラーバッファの幅です。
                    @param[in]    bufferHeight_ カラーバッファの高さです。
                 */
                void SetBufferSize( s32 bufferWidth_, s32 bufferHeight_ )
                {
                    bufferWidth = bufferWidth_; bufferHeight = bufferHeight_;
                }
                
            public:
                /*!
                    @brief シザーを有効にするか。 型は bool です。
                */
                bool    isEnable;
                NN_PADDING3;

                /*!
                    @brief シザー範囲の左下の X の値です。 型は s32 です。
                */
                s32        x;

                /*!
                    @brief シザー範囲の左下の Y の値です。型は s32 です。
                */
                s32        y;

                /*!
                    @brief シザー範囲の幅です。型は u32 です。
                */
                u32        width;

                /*!
                    @brief シザー範囲の高さです。型は u32 です。
                */
                u32        height;

                /*!
                    @brief シザー範囲のカラーバッファの幅です。型は s32 です。
                */
                s32        bufferWidth;

                /*!
                    @brief シザー範囲のカラーバッファの幅です。型は s32 です。
                */
                s32        bufferHeight;
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_SCISSOR_H_
