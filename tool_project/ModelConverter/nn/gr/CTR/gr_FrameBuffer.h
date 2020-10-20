/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_FrameBuffer.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_FRAME_BUFFER_H_
#define NN_GR_CTR_GR_FRAME_BUFFER_H_

#include <nn/util/util_SizedEnum.h>
#include <nn/gr/CTR/gr_Utility.h>
#include <nn/gx/CTR/gx_CTRRaw.h>

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief フレームバッファの設定をするためのクラスです。
             */
            class FrameBuffer
            {
            public :
                static const u32 COLOR_BUFFER_BIT         = 0x1; //!< カラーバッファを指定するビットです。
                static const u32 DEPTH_BUFFER_BIT         = 0x2; //!< デプスバッファを指定するビットです。
                static const u32 STENCIL_BUFFER_BIT       = 0x4; //!< ステンシルバッファを指定するビットです。
                static const u32 DEPTH_STENCIL_BUFFER_BIT = DEPTH_BUFFER_BIT | STENCIL_BUFFER_BIT; //!< デプスバッファとステンシルバッファを指定するビットです。

                /*!
                    @brief ブロックサイズを設定するための列挙定数です。
                 */
                enum BlockSize
                {
                    /*!
                        @brief ブロックサイズを 8 にします。
                    */
                    BLOCK_SIZE8  = 0x0,

                    /*!
                        @brief ブロックサイズを 32 にします。
                               アーリーデプステスト時に使用します。
                    */
                    BLOCK_SIZE32 = 0x1
                };

                /*!
                    @brief カラーバッファの設定のためのクラスです。
                */
                class ColorBuffer
                {
                public :
                    /*!
                        @brief カラーバッファの仮想アドレスです。型は uptr です。
                    */
                    uptr  virtualAddr;

                    /*!
                        @brief カラーバッファのフォーマットです。型は @ref PicaDataColor です。
                    */
                    util::SizedEnum1<PicaDataColor> format;

                    /*!
                        @brief カラーバッファのブロックサイズです。型は @ref BlockSize です。
                    */
                    util::SizedEnum1<BlockSize> blockSize;
                    NN_PADDING2;

                    /*!
                        @brief カラーバッファの幅です。型は s32 です。
                    */
                    s32 width;

                    /*!
                        @brief カラーバッファの高さです。型は s32 です。
                    */
                    s32 height;

                    /*!
                        @brief クリアカラーを設定します。
                               型は f32[ 4 ] です。
                     */
                    f32 clearColor[ 4 ];

                public :
                    /*!
                        @brief カラーバッファの設定を初期値で初期化します。

                        @param[in] frameBuffer_ フレームバッファです。
                     */
                    explicit ColorBuffer( const FrameBuffer& frameBuffer_ );

                    /*!
                        @brief レンダーバッファのブロックサイズを設定するコマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                        @return 書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeRenderBlockModeCommand( bit32* command ) const;

                    /*!
                        @brief 設定された情報をもとに、描画コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isAddRenderBlockMode true ならレンダーバッファのブロックサイズ設定 MakeRenderBlockModeCommand() を内部で呼びます。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool isAddRenderBlockMode = false ) const;

                protected :
                    const FrameBuffer& m_FrameBuffer;
                };

                /*!
                    @brief デプス・ステンシルバッファを設定するためのクラスです。
                */
                class DepthStencilBuffer
                {
                public :
                    /*!
                        @brief デプス・ステンシルバッファの仮想ドレスです。型は uptr です。
                    */
                    uptr  virtualAddr;

                    /*!
                        @brief デプス・ステンシルバッファのフォーマットです。型は @ref PicaDataDepth です。
                    */
                    util::SizedEnum1<PicaDataDepth> format;
                    NN_PADDING3;

                    /*!
                        @brief デプス・ステンシルバッファの幅です。型は s32 です。
                    */
                    s32 width;

                    /*!
                        @brief デプス・ステンシルバッファの高さです。型は s32 です。
                    */
                    s32 height;

                    /*!
                        @brief デプスバッファのクリアの値です。
                               型は f32 です。
                               初期値は 1.0f です。
                     */
                    f32 clearDepth;

                    /*!
                        @brief ステンシルバッファのクリアの値です。
                               型は u8 です。
                               初期値は 0 です。
                     */
                    u8 clearStencil;
                    NN_PADDING3;

                public :
                    /*!
                        @brief デプス・ステンシルバッファを初期値で初期化します。

                        @param[in] frameBuffer_ フレームバッファです。
                     */
                    explicit DepthStencilBuffer( const FrameBuffer& frameBuffer_ );

                    /*!
                        @brief 設定された情報をもとに、描画コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command ) const;

                protected :
                    const FrameBuffer& m_FrameBuffer;
                };

            public :
                /*!
                    @brief カラーバッファの設定です。
                           型は nn::gr::CTR::FrameBuffer::ColorBuffer です。
                 */
                ColorBuffer colorBuffer;

                /*!
                    @brief デプス・ステンシルバッファの設定です。
                           型は nn::gr::CTR::FrameBuffer::DepthStencilBuffer です。
                 */
                DepthStencilBuffer depthStencilBuffer;

                /*!
                    @brief フレームバッファの幅です。型は s32 です。
                 */
                s32 width;

                /*!
                    @brief フレームバッファの高さです。型は s32 です。
                 */
                s32 height;

            public :
                /*!
                    @brief フレームバッファの各々の設定を、初期値で初期化します。

                    @see FrameBuffer::ColorBuffer、
                         FrameBuffer::DepthStencilBuffer
                 */
                explicit FrameBuffer();

                /*!
                    @brief フレームバッファのキャッシュをクリアします。
                           描画後に、0x100-0x130 のレジスタを変更する前に呼ぶ必要があります。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeClearCacheCommand( bit32* command );

                /*!
                    @brief 設定された情報をもとに、フレームバッファの設定コマンドを生成します。

                    @param[in] command   描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] bufferBit レンダーバッファの種類です。@ref COLOR_BUFFER_BIT | @ref DEPTH_STENCIL_BUFFER_BIT などを指定します。
                    @param[in] isClearCache フレームバッファのキャッシュクリアのメソッド MakeClearCacheCommand() を内部で最初に呼びます。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。

                    @see FrameBuffer::ColorBuffer、
                         FrameBuffer::DepthStencilBuffer
                 */
                bit32* MakeCommand( bit32* command, const u32 bufferBit, bool isClearCache = true ) const;

                /*!
                    @brief カラーバッファやデプス・ステンシルバッファをクリアするリクエストを生成します。
                           isSplitDrawCmdlist が false の場合、
                           リクエストの呼び出し前に nngxSplitDrawCmdlist() を呼ぶ必要があります。

                    @param[in] bufferBit             レンダーバッファの種類です。@ref COLOR_BUFFER_BIT | @ref DEPTH_STENCIL_BUFFER_BIT などを指定します。
                    @param[in] isAddSplitDrawCmdlist リクエストの生成前に、nngxSplitDrawCmdlist() を呼びます。
                 */
                void MakeClearRequest( const u32 bufferBit, bool isAddSplitDrawCmdlist = true );
                void MakeClearRequestRaw( nngxCommandList* commandList, const u32 bufferBit, bool isAddSplitDrawCmdlist = true );
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_FRAME_BUFFER_H_
