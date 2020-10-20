/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Viewport.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_VIEWPORT_H_
#define NN_GR_CTR_GR_VIEWPORT_H_

#include <nn/gr/CTR/gr_Utility.h>

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief ビューポート関連のコマンドを生成するためのクラスです。
                       あらかじめ x, y, width, heightの各メンバに値を入れておき、MakeCommand() でコマンドを生成します。
            */
            class Viewport
            {
            public:
                /*!
                    @brief デフォルト値(0, 0, 240, 320)で初期化するコンストラクタです。
                */
                Viewport( void ) :
                x( 0 ),
                y( 0 ),
                width( 240 ),
                height( 320 )
                {
                }

                /*!
                    @brief 指定した値で初期化するコンストラクタです。

                    @param[in]   x_      ビューポート左下端のx座標です。
                    @param[in]   y_      ビューポート左下端のy座標です。
                    @param[in]   width_  ビューポートの幅です。
                    @param[in]   height_ ビューポートの高さです。
                 */
                Viewport( s32 x_, s32 y_, u32 width_, u32 height_ ) :
                x( x_ ),
                y( y_ ),
                width( width_ ),
                height( height_ )
                {
                }
            
            public:
                /*!
                    @brief 設定された情報をもとに、描画コマンドを生成します。
                
                    @param[in]    command    描画コマンド書き込み先の先頭アドレスです。

                    @return                  書き込まれた描画コマンドの終端の次のアドレスを返します。
                */
                bit32* MakeCommand( bit32* command ) const;
            
            public:
                /*!
                    @brief パラメータを一括で指定する便利関数です。         
                           この関数を使わずに直接メンバ変数に代入を行ってもかまいません。

                    @param[in]   x_      ビューポート左下端のx座標です。
                    @param[in]   y_      ビューポート左下端のy座標です。
                    @param[in]   width_  ビューポートの幅です。
                    @param[in]   height_ ビューポートの高さです。
                */
                void Set( s32 x_, s32 y_, u32 width_, u32 height_ )
                {
                    x = x_; y = y_; width = width_; height = height_;
                }
            
            public:
                /*!
                    @brief ビューポート左下端のx座標です。型は s32 です。
                */
                s32 x;

                /*!
                    @brief ビューポート左下端のy座標です。型は s32 です。
                */
                s32 y;

                /*!
                    @brief ビューポートの幅です。型は u32 です。
                */
                u32 width;

                /*!
                    @brief ビューポートの高さです。型は u32 です。
                */
                u32 height;
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_VIEWPORT_H_
