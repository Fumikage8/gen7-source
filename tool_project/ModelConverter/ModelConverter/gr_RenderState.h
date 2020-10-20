/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_RenderState.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_RENDER_STATE_H_
#define NN_GR_CTR_GR_RENDER_STATE_H_

#include "gr_gl.h"
#include "util_SizedEnum.h"
#include "gr_Utility.h"

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief レンダーステート設定のためのクラスです。
             */
            class RenderState
            {
            public :
                /*!
                    @brief カラーマスク設定のための定義です。
                 */
                enum ColorMask
                {
                    //! 赤、緑、青、アルファのどの成分も指定しません。
                    COLOR_MASK_NONE = 0,
                    //! 赤成分を表します。
                    COLOR_MASK_R    = 1 << 0,
                    //! 緑成分を表します。
                    COLOR_MASK_G    = 1 << 1,
                    //! 青成分を表します。
                    COLOR_MASK_B    = 1 << 2,
                    //! アルファ成分を表します。
                    COLOR_MASK_A    = 1 << 3, 
                    //! すべての成分を表します。
                    COLOR_MASK_RGBA = COLOR_MASK_R | COLOR_MASK_G | COLOR_MASK_B | COLOR_MASK_A 
                };

                /*!
                    @brief アルファテスト設定のためのクラスです。
                 */
                /*
                    各値を設定し、 MakeCommand()を呼ぶことで、描画コマンドを生成することができます。
                */
                class AlphaTest
                {
                public :
                    /*!
                        @brief アルファテストの有効/無効の設定です。
                               型は bool です。
                               無効化されている場合は、必ずパスします。
                               初期値はfalse（無効）です。
                     */
                    bool isEnable;

                    /*!
                        @brief アルファテストで参照する定数を設定します。
                               型は u8 です。
                               ユニフォーム値[0.f, 1.f]を[0, 255]の8ビットの整数にマップした値を設定します。
                               初期値は0です。
                     */
                    u8 refValue;

                    /*!
                        @brief アルファテストで使用される比較方法を設定します。
                               型は @ref PicaDataAlphaTest です。
                               初期値は、PICA_DATA_ALPHA_TEST_ALWAYSです。
                    */
                    util::SizedEnum1<PicaDataAlphaTest> func;

                    NN_PADDING1;

                public :
                    /*!
                        @brief アルファテスト設定の初期値で初期化します。

                        @param[in] renderState_ レンダーステートです。
                     */
                    explicit AlphaTest( const RenderState& renderState_ );

                    /*!
                        @brief 設定された情報をもとに、描画コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isUpdateFBAccess フレームバッファアクセスを更新します

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool isUpdateFBAccess = true ) const;


                    /*!
                        @brief 無効化するコマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isClearFrameBufferCache    フレームバッファのキャッシュフラッシュを行います。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    static bit32* MakeDisableCommand( bit32* command, bool isClearFrameBufferCache = true );

                protected:
                    const RenderState& m_RenderState;
                };

                /*!
                    @brief ブレンド設定のためのクラスです。
                           各値を設定し、 MakeCommand()を呼ぶことで、描画コマンドを生成することができます。
                           レジスタを共有しているため、論理演算設定と同時に使用することはできません。
                */
                class Blend
                {
                public :
                    /*!
                        @brief ブレンドの有効/無効の設定です。
                               型は bool です。初期値はfalse（無効）です。
                               論理演算とブレンドはどちらかしか有効にできないです。
                     */
                    bool isEnable;
             /*!
                        @brief RGB成分のブレンド方程式の設定です。型は @ref PicaDataBlendEquation です。
                        　　   初期値は PICA_DATA_BLEND_EQUATION_ADD です。
                    */
                    util::SizedEnum1<PicaDataBlendEquation> eqRgb;


                    /*!
                        @brief アルファ成分のブレンド関数の設定です。型は @ref PicaDataBlendEquation です。
                               初期値はPICA_DATA_BLEND_EQUATION_ADDです。
                    */
                    util::SizedEnum1<PicaDataBlendEquation> eqAlpha;

                    /*!
                        @brief ブレンドのRGB成分のソースの設定です。
                               型は @ref PicaDataBlendFunc です。
                               初期値はPICA_DATA_BLEND_FUNC_SRC_ALPHAです。
                    */
                    util::SizedEnum1<PicaDataBlendFunc> srcRgb;

                    /*!
                        @brief ブレンドの Alpha 成分のソースの設定です。
                               型は @ref PicaDataBlendFunc です。
                               初期値は PICA_DATA_BLEND_FUNC_SRC_ALPHA です。
                    */
                    util::SizedEnum1<PicaDataBlendFunc> srcAlpha;


                    /*!
                        @brief ブレンドのRGB成分のデスティネーションの設定です。
                               型は @ref PicaDataBlendFunc です。
                               初期値は PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA です。
                    */
                    util::SizedEnum1<PicaDataBlendFunc> dstRgb;

                    /*!
                        @brief ブレンドのALPHA成分のデスティネーションの設定です。
                               型は @ref PicaDataBlendFunc です。
                        　　　 初期値は PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA です。
                    */
                    util::SizedEnum1<PicaDataBlendFunc> dstAlpha;


                    /*!
                        @brief ブレンドのコンスタントカラーの赤成分の設定です。
                               型は u8 です。
                               [0,255]の範囲で指定します。初期値は255です。
                    */
                    u8 colorR;


                    /*!
                        @brief ブレンドのコンスタントカラーの緑成分の設定です。
                               型は u8 です。
                               [0,255]の範囲で指定します。初期値は255です。
                    */
                    u8 colorG;


                    /*!
                        @brief ブレンドのコンスタントカラーの青成分の設定です。
                               型は u8 です。
                               [0,255]の範囲で指定します。初期値は255です。
                    */
                    u8 colorB;


                    /*!
                        @brief ブレンドのコンスタントカラーのアルファ成分の設定です。
                        型は u8 です。[0,255]の範囲で指定します。初期値は255です。
                    */
                    u8 colorA;

                    NN_PADDING1;

                public :
                    /*!
                        @brief ブレンド設定の初期値で初期化します。

                        @param[in] renderState_ レンダーステートです。
                     */
                    explicit Blend( const RenderState& renderState_ );


                    /*!
                        @brief 設定された情報をもとに、描画コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isUpdateFBAccess フレームバッファアクセスを更新します。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool isUpdateFBAccess = true ) const;

                    /*!
                        @brief ブレンドを無効化します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isClearFrameBufferCache    フレームバッファのキャッシュフラッシュを行います。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    static bit32* MakeDisableCommand( bit32* command, bool isClearFrameBufferCache = true );

                    // ブレンドに DST カラーが参照されているかどうか
                    bool IsDstColorUsed(void) const;

                protected:
                    const RenderState& m_RenderState;

                };

                /*!
                    @brief 論理演算設定のためのクラスです。
                           各値を設定し、 MakeCommand()を呼ぶことで、描画コマンドを生成することができます。
                           レジスタを共有しているため、ブレンド設定と同時に使用することはできません。
                */
                class LogicOp
                {
                public :
                    /*!
                        @brief 論理演算の有効/無効の設定です。型は bool です。初期値はfalse（無効）です。
                               論理演算とブレンドはどちらかしか有効にできないです。
                     */
                    bool isEnable;

                    /*!
                        @brief 論理演算の設定です。
                        型は @ref PicaDataLogicOp です。
                        初期値は PICA_DATA_LOGIC_NOOPです。
                    */
                    util::SizedEnum1<PicaDataLogicOp> opCode;

                    NN_PADDING2;

                public :
                    /*!
                        @brief 論理演算設定の初期値で初期化します。

                        @param[in] renderState_ レンダーステートです。
                     */
                    explicit LogicOp( const RenderState& renderState_ );

                    /*!
                        @brief 設定された情報をもとに、描画コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isUpdateFBAccess フレームバッファアクセスを更新します

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool isUpdateFBAccess = true ) const;

                    // 論理演算に DST カラーが参照されているかどうか
                    bool IsDstColorUsed(void) const;

                protected:
                    const RenderState& m_RenderState;
                };

                /*!
                    @brief シャドウマップの設定のためのクラスです。
                           レジスタを共有しているためシャドウマップを有効にしたときは、
                           通常のフラグメントオペレーションやガス密度情報描画と同時に使うことはできません。
                */
                class ShadowMap
                {
                public :
                    /*!
                        @brief コンストラクタで初期値を設定します。

                        @param[in] renderState_ レンダーステートです。
                    */
                    explicit ShadowMap( const RenderState& renderState_ );

                    /*!
                        @brief 設定された情報をもとに、シャドウマップの設定コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isUpdateFBAccess フレームバッファアクセスを更新します。
                        @param[in] isAddDummyCommand 0x80 へのダミー書き込みコマンドを 3回 生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                    bit32* MakeCommand( bit32* command,
                                        bool isUpdateFBAccess = true,
                                        bool isAddDummyCommand = true ) const;

                    /*!
                        @brief 設定された情報をもとに、シャドウマップのテクスチャの設定コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isAddDummyCommand レジスタの変更前に、0x80 へのダミーコマンドを 3 回生成します。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                    */
                    bit32* MakeTextureCommand( bit32* command,
                                               bool isAddDummyCommand = true ) const;

                    /*!
                        @brief 設定された情報をもとに、シャドウの減衰の設定コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                    */
                    bit32* MakeAttenuationCommand( bit32* command ) const;

                    /*!
                        @brief シャドウマップの有効/無効の設定です。
                               型は bool です。
                               無効化されている場合は、必ずパスします。
                               初期値は false（無効）です。
                               レジスタを共有しているためシャドウマップを有効にしたとき、
                               通常のフラグメントオペレーションやガスと同時に使うことはできないです。
                     */
                    bool isEnable;

                    /*!
                        @brief シャドウテクスチャへの参照に対するテクスチャ座標生成の透視投影適用の有効/無効にします。
                               型は bool です。デフォルトは true です。
                               dmp_Texture[0].isPerspective ユニフォームに相当します。
                    */
                    bool isPerspective;
                    NN_PADDING2;

                    /*!
                        @brief 光源までの距離から減算されるバイアス値です。
                               型は f32 です。0.0 より大きな値を設定します。
                               デフォルトは本来未定義ですが、0.0f に初期化しておきます。
                               dmp_Texture[ 0 ].shadowZBias ユニフォームに相当します。
                    */
                    f32 zBias;


                    /*!
                        @brief 光源座標系のスクリーン空間におけるデプス値導関数の評価値に対するスケールです。
                               しかし、zScale による設定は無効です。互換性のためだけに残してあります。
                               （型は f32 です。デフォルトは 1.0f です。
                               dmp_Texture[ 0 ].shadowZScale ユニフォームに相当します。）
                    */
                    f32 zScale;

                    /*!
                        @brief 半影のハード化計算時に適用されるスケールです。
                               型は f32 です。デフォルトは 0.0f です。
                               dmp_FragOperation.penumbraScale ユニフォームに相当します。
                    */
                    f32 penumbraScale;

                    /*!
                        @brief  半影のハード化計算時に適用されるバイアスです。
                                型は f32 です。デフォルトは 1.0f です。
                                dmp_FragOperation.penumbraBias ユニフォームに相当します。
                    */
                    f32 penumbraBias;

                protected:
                    const RenderState& m_RenderState;
                };

                /*!
                    @brief ステンシルテスト設定のためのクラスです。
                 */
                class StencilTest
                {
                public :
                    /*!
                        @brief ステンシルテストの有効/無効の設定です。無効化されている場合は、必ずパスします。初期値はfalse（無効）です。
                     */
                    bool isEnable;

                    /*!
                        @brief ステンシルバッファへのマスクの設定です。
                               型は bit8 です。初期値は 0xff です。glStencilMask( mask ) に相当します。

                     */
                    bit8 maskOp;
                    NN_PADDING2;

                    /*!
                        @brief ステンシルテストに使用される比較関数の設定です。
                               型は @ref PicaDataStencilTest です。
                               初期値は、PICA_DATA_STENCIL_TEST_ALWAYSです。
                               glStencilFunc( func, ref, mask ) の ref に相当します。
                    */
                    util::SizedEnum1<PicaDataStencilTest> func;
                    NN_PADDING3;

                    /*!
                        @brief ステンシルテストに使用される参照値の設定です。
                               型は bit32 です。初期値は、0です。
                    */
                    bit32  ref;


                    /*!
                        @brief ステンシルテストに使用されるマスク値の設定です。
                               型は bit32 です。初期値は、0xffです。
                               glStencilFunc( func, ref, mask ) の mask に相当します。
                    */
                    /*
                        ステンシルバッファの値をstencilとすると、
                        ( stencil & mask )と( ref & mask )との間で比較がおこなわれます。
                     */
                    bit32  mask;

                    /*!
                        @brief ステンシルテストがフェイルした場合に適用する、
                               ステンシルバッファの値の操作を設定します。
                               型は @ref PicaDataStencilOp です。
                               初期値は、PICA_DATA_STENCIL_OP_KEEPです。
                               glStencilFunc( func, ref, mask ) の func に相当します。
                    */
                    util::SizedEnum1<PicaDataStencilOp> opFail;


                    /*!
                        @brief ステンシルテストにパスし、
                               デプステストにフェイルした場合に適用する、
                               ステンシルバッファの値の操作を設定します。
                               型は @ref PicaDataStencilOp です。
                               初期値は、PICA_DATA_STENCIL_OP_KEEPです。
                    */
                    util::SizedEnum1<PicaDataStencilOp>  opZFail;

                    /*!
                            @brief ステンシルテストとデプステストの両方にパスした場合に適用する、
                                   ステンシルバッファの値の操作を設定します。
                                   型は @ref PicaDataStencilOp です。
                                   初期値は、PICA_DATA_STENCIL_OP_KEEPです。
                    */
                    util::SizedEnum1<PicaDataStencilOp>  opZPass;

                    NN_PADDING1;

                public :
                    /*!
                        @brief ステンシルテスト設定の初期値で初期化します。

                        @param[in] renderState_ レンダーステートです。
                     */
                    explicit StencilTest( const RenderState& renderState_ );

                    /*!
                        @brief 設定された情報をもとに、描画コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isUpdateFBAccess フレームバッファアクセスを更新します

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool isUpdateFBAccess = true ) const;

                    /*!
                        @brief ステンシルテストを無効化するコマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isClearFrameBufferCache    フレームバッファのキャッシュフラッシュを行います。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    static bit32* MakeDisableCommand( bit32* command, bool isClearFrameBufferCache = true );

                protected:
                    const RenderState& m_RenderState;
                };

                /*!
                    @brief デプステスト設定のためのクラスです。
                 */
                class DepthTest
                {
                public :
                    /*!
                        @brief デプステストの有効/無効の設定です。
                               型は bool です。
                               無効化されている場合は、必ずパスします。
                               初期値はtrue（有効）です。
                     */
                    bool             isEnable;


                    /*!
                        @brief デプスバッファへの書き込みの有効/無効の設定です。
                               型は bool です。
                               初期値はtrue（有効）です。
                     */
                    bool             isEnableWrite;

                    /*!
                        @brief デプステストに使用される比較関数の設定です。
                               型は @ref PicaDataDepthTest です。
                               初期値は PICA_DATA_DEPTH_TEST_LESS です。
                    */
                    util::SizedEnum1<PicaDataDepthTest> func;

                    NN_PADDING1;

                public :
                    /*!
                        @brief デプステスト設定の初期値で初期化します。

                        @param[in] renderState_ レンダーステートです。
                     */
                    explicit DepthTest( const RenderState& renderState_ );


                    /*!
                        @brief 設定された情報をもとに、描画コマンドを生成します。
                               生成される描画コマンドの中には、カラーマスクの設定も含まれますので、
                               注意してください。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。型は bit32* です。
                        @param[in] isUpdateFBAccess フレームバッファアクセスを更新します。型は bool です。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool isUpdateFBAccess = true ) const;

                    /*!
                        @brief デプステストを無効化します。
                               またカラーマスクは全て true に設定しますので、注意してください。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。型は bit32* です。
                        @param[in] isClearFrameBufferCache    フレームバッファのキャッシュフラッシュを行います。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    static bit32* MakeDisableCommand( bit32* command, bool isClearFrameBufferCache = true );

                protected:
                    const RenderState& m_RenderState;
                };

                /*!
                    @brief wバッファ設定のためのクラスです。
                 */
                class WBuffer
                {
                public :

                    /*!
                        @brief wバッファ有効時のスケール値です。型は f32 です。
                               wScaleが0のとき、wバッファは無効になります。初期値は0です。
                     */
                    f32 wScale;

                    /*!
                        @brief ポリゴンオフセットの有効/無効の設定です。型は bool です。
                               一応、wバッファと同時に使用することができます。
                     */
                    bool isEnablePolygonOffset;

                    NN_PADDING3;

                    /*!
                        @brief デプス値の分解可能最小値に対するスケール値の設定です。
                               型は f32 です。
                               デプス値の分解可能最小値は、デプスバッファのビット値に依存します。
                               初期値は0です。
                               wバッファが有効でかつデプスバッファが 24 bit のとき、
                               精度誤差で消えてしまうの防ぐために
                               ポリゴンオフセットに定数値をかけています。

                        @see depthRangeBit;
                     */

                    f32  polygonOffsetUnit;

                    /*!
                        @brief クリップ空間におけるNear平面のdepth値の設定です。
                               型は f32 です。
                               初期値は0です。
                               0 <= depthRangeNear <= depthRangeFar <= 1となるように設定します。
                               wバッファを有効にしている場合は設定を無視して0が設定されます。
                     */
                    f32  depthRangeNear;

                    /*!
                        @brief クリップ空間におけるFar平面のdepth値の設定です。
                               型は f32 です。
                               初期値は1です。
                               0 <= depthRangeNear <= depthRangeFar <= 1となるように設定します。
                               wバッファを有効にしている場合は設定を無視して1が設定されます。
                     */
                    f32  depthRangeFar;

                    /*!
                        @brief デブスバッファのビット値を指定します。
                               型は f32 です。
                               初期値は24です。
                     */
                    u8   depthRangeBit;

                    NN_PADDING3;

                public :
                    /*!
                        @brief wバッファ設定の初期値で初期化します。
                     */
                    explicit WBuffer( );

                    /*!
                        @brief 設定された情報をもとに、描画コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command ) const;
                };

                /*!
                    @brief カリング設定のためのクラスです。
                 */
                class Culling
                {
                public :
                    /*!
                        @brief カリングの有効/無効の設定です。
                               型は bool です。
                               無効化されている場合は、両面ポリゴンが描画されます。
                               初期値はtrue（有効）です。
                     */
                    bool isEnable;

                    /*!
                        @brief ポリゴン面の表の向きをあらわします。
                     */
                    enum FrontFace
                    {
                        /*!
                            @brief 頂点が時計回りに配置されている場合を表とします。
                        */
                        FRONT_FACE_CW , 

                        /*!
                            @brief 頂点が反時計回りに配置されている場合を表とします。
                        */
                        FRONT_FACE_CCW
                    };

                    /*!
                        @brief ポリゴン面の表の向きを設定します。 
                               型は nn::gr::CTR::RenderState::Culling::FrontFace です。
                               初期値は FRONT_FACE_CCW（反時計回り）です。
                     */
                    util::SizedEnum1<FrontFace> frontFace;

                    /*!
                        @brief カリングするポリゴン面をあらわします。
                     */
                    enum CullFace
                    {
                        /*!
                            @brief 表面をカリングします。
                        */
                        CULL_FACE_FRONT,
                         /*!
                            @brief 裏面をカリングします。
                        */
                        CULL_FACE_BACK
                    };

                    /*!
                        @brief カリングするポリゴン面を設定します。
                               型は nn::gr::CTR::RenderState::Culling::CullFace です。
                               初期値は CULL_FACE_BACK です。
                     */
                    util::SizedEnum1<CullFace> cullFace;

                    NN_PADDING1;

                public :
                    /*!
                        @brief カリング設定の初期値で初期化します。

                        @param[in] renderState_ レンダーステートです。
                     */
                    explicit Culling( const RenderState& renderState_ );


                    /*!
                        @brief 設定された情報をもとに、描画コマンドを生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isUpdateFBAccess フレームバッファアクセスを更新します
                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool isUpdateFBAccess = true ) const;

                    /*!
                        @brief カリングを無効化します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isClearFrameBufferCache    フレームバッファのキャッシュフラッシュを行います。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    static bit32* MakeDisableCommand( bit32* command, bool isClearFrameBufferCache = true );

                protected:
                    const RenderState& m_RenderState;
                };

                /*!
                    @brief フレームバッファアクセスの設定のためのクラスです。
                */
                /*
                    フレームバッファアクセスの設定は、カラーマスク、ブレンド、論理演算、デプステスト、ステンシルテストの設定
                    に依存するので、コンストラクタで参照を指定します。
                    上記の設定を変えるコマンドを生成した後で、
                    フレームバッファアクセスに変更が生じる場合は、
                    FBAccess::MakeCommand()によりコマンドを生成する必要があります。
                */
                class FBAccess
                {
                public :
                    /*!
                        @brief フレームバッファアクセスの設定は、カラーマスク、ブレンド、論理演算、デプステスト、ステンシルテストの設定
                               に依存するので、コンストラクタで参照を指定します。

                        @param[in] renderState_ レンダーステートです。
                     */
                    explicit FBAccess( const RenderState& renderState_ );

                    /*!
                        @brief 各種の設定された情報をもとに、描画コマンドを生成します。

                        @param[in] command                    描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isClearFrameBufferCache    フレームバッファのキャッシュフラッシュを行います。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    bit32* MakeCommand( bit32* command, bool isClearFrameBufferCache = true ) const;

                    /*!
                        @brief 無効化するコマンドを生成します.

                        @param[in] command                    描画コマンドの書き込み先の先頭アドレスです。
                        @param[in] isClearFrameBufferCache    フレームバッファのキャッシュフラッシュを行います。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    static bit32* MakeDisableCommand( bit32* command, bool isClearFrameBufferCache = true );

                    /*!
                        @brief フレームバッファのキャッシュフラッシュをするコマンドを生成します.

                        @param[in] command                    描画コマンドの書き込み先の先頭アドレスです。

                        @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                     */
                    static bit32* MakeClearCacheCommand( bit32* command );

                protected:
                    const RenderState& m_RenderState;
                };

            public :
                /*!
                    @brief ブレンド設定です。
                           型は nn::gr::CTR::RenderState::Blend です。
                           論理演算とブレンドはどちらかしか有効にできないです。
                           また有効にした場合、シャドウマップへの描画やガス密度情報描画と同時に使うことはできません。
                 */
                Blend       blend;

                /*!
                    @brief 論理演算設定です。
                           型は nn::gr::CTR::RenderState::LogicOp です。
                           論理演算とブレンドはどちらかしか有効にできないです。
                           また有効にした場合、シャドウマップへの描画やガス密度情報描画と同時に使うことはできません。
                 */
                LogicOp     logicOp;

                /*!
                    @brief シャドウマップの設定です。
                           型は nn::gr::CTR::RenderState::ShadowMap です。
                           また有効にした場合、通常のフラグメントオペレーションやガス密度情報描画と同時に使うことはできません。
                 */
                ShadowMap  shadowMap;

                /*!
                    @brief アルファテスト設定です。
                           型は nn::gr::CTR::RenderState::AlphaTest です。
                 */
                AlphaTest   alphaTest;

                /*!
                    @brief ステンシルテスト設定です。
                           型は nn::gr::CTR::RenderState::StencilTest です。
                 */
                StencilTest stencilTest;

                /*!
                    @brief カラーマスク設定です。
                           型は nn::gr::CTR::RenderState::ColorMask です。
                 */
                ColorMask   colorMask;
                NN_PADDING3;

                /*!
                    @brief デプステスト設定です。
                           型は nn::gr::CTR::RenderState::DepthTest です。

                    @ref  RenderState::DepthTest
                 */
                DepthTest   depthTest;

                /*!
                    @brief カリング設定です。
                           型は nn::gr::CTR::RenderState::Culling です。

                    @ref   RenderState::Culling
                 */
                Culling     cullingTest;

                /*!
                    @brief wバッファ設定です。
                           型は nn::gr::CTR::RenderState::WBuffer です。

                    @ref   RenderState::WBuffer
                 */
                WBuffer     wBuffer;

                /*!
                    @brief フレームバッファアクセス設定です。
                           型は nn::gr::CTR::RenderState::FBAccess です。

                 */
                FBAccess    fbAccess;

            public :
                /*!
                    @brief レンダーステートの各々の設定を、初期値で初期化します。

                    @see RenderState::Culling、
                         RenderState::Blend、
                         RenderState::LogicOp、
                         RenderState::ShadowMap、
                         RenderState::AlphaTest、
                         RenderState::StencilTest、
                         RenderState::DepthTest、
                         RenderState::WBuffer、
                         RenderState::FBAccess
                 */
                explicit RenderState();

                /*!
                    @brief 設定された情報をもとに、すべてのレンダーステートの設定コマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isClearFrameBufferCache    フレームバッファのキャッシュフラッシュを行います。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。

                    @see RenderState::Culling::MakeCommand()、
                         RenderState::Blend::MakeCommand()、
                         RenderState::LogicOp::MakeCommand()、
                         RenderState::ShadowMap::MakeCommand()、
                         RenderState::AlphaTest::MakeCommand()、
                         RenderState::StencilTest::MakeCommand()、
                         RenderState::DepthTest::MakeCommand()、
                         RenderState::WBuffer::MakeCommand()、
                         RenderState::FBAccess::MakeCommand() 
                 */
                bit32* MakeCommand( bit32* command, bool isClearFrameBufferCache = true ) const;

                /*!
                    @brief レンダーステートを無効化するコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isClearFrameBufferCache    フレームバッファのキャッシュフラッシュを行います。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeDisableCommand( bit32* command, bool isClearFrameBufferCache = true );
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_RENDER_STATE_H_
