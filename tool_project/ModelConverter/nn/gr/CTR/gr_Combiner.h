/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Combiner.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_COMBINER_H_
#define NN_GR_CTR_GR_COMBINER_H_

#include <nn/util/util_SizedEnum.h>
#include <nn/gr/CTR/gr_Utility.h>

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief コンバイナ設定のためのクラスです。
             */
            class Combiner
            {
            public :
                /*!
                    @brief コンバイナのステージ設定のためのクラスです。
                 */
                class Stage
                {
                public :
                    /*!
                        @brief コンバイン関数の入出力設定をあらわす構造体です。
                     */
                    struct CombineFunction
                    {
                        /*!
                            @brief コンバイン関数の設定です。
                                   型は @ref PicaDataTexEnvCombine です。
                                   値には PICA マクロヘッダの列挙型の名前が
                                   @ref PicaDataTexEnvCombine の列挙定数を使って下さい。
                                   初期値はPICA_DATA_TEX_ENV_COMBINE_REPLACEです。
                        */
                        util::SizedEnum1<PicaDataTexEnvCombine> combine;

                        /*!
                            @brief オペランドの設定です。
                                   型は @ref PicaDataTexEnvOperand[] です。
                                   値にはPICAマクロヘッダの列挙型の名前が
                                   @ref PicaDataTexEnvOperand
                                   の列挙定数を使って下さい。
                                   カラーのオペランドは、デフォルトは PICA_DATA_OPE_RGB_SRC_COLOR です。
                                   アルファのオペランドは、デフォルトは PICA_DATA_OPE_ALPHA_SRC_ALPHA です。
                        */
                        util::SizedEnum1<PicaDataTexEnvOperand> operand[3];

                        /*!
                            @brief ソース0、ソース1、ソース2の設定です。
                                   型は @ref PicaDataTexEnvSrc です。
                                   値にはPICAマクロヘッダの列挙型の名前が
                                   @ref PicaDataTexEnvSrc
                                   の列挙定数を使って下さい。
                                   ステージ0についての設定の場合、
                                   デフォルトはPICA_DATA_TEX_ENV_SRC_RGBA_CONSTANTです。
                                   ステージ0以外についての設定の場合は、
                                   デフォルトはPICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUSです。
                        */
                        util::SizedEnum1<PicaDataTexEnvSrc> source[3];

                        /*!
                            @brief スケールの設定です。
                                   型は @ref PicaDataTexEnvScale です。
                                   値にはPICAマクロヘッダの列挙型の名前が
                                   @ref PicaDataTexEnvScale
                                   の列挙定数を使って下さい。
                                   デフォルトはPICA_DATA_TEX_ENV_SCALE_1です。
                        */
                        util::SizedEnum1<PicaDataTexEnvScale> scale;

                        /*!
                            @brief コンバイナバッファのソースの設定です。
                                   型は @ref PicaDataTexEnvBufferInput です。
                                   値にはPICAマクロヘッダの列挙型の名前が
                                   @ref PicaDataTexEnvBufferInput
                                   の列挙定数を使って下さい。
                                   デフォルトはPICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMPです。
                        */
                        util::SizedEnum1<PicaDataTexEnvBufferInput> bufferInput;

                        /*!
                            @brief コンバイン関数の設定について、初期値を設定します。

                            @param[in] stage_index コンバイン関数が使われるステージのインデックスです。
                            @param[in] is_rgb      コンバイン関数がカラーが対象であればtrue、アルファが対象であればfalseです。
                        */
                        explicit CombineFunction( const u8 stage_index, bool is_rgb );

                    protected :
                        /*!
                            @brief Stage クラスを friend に設定しています。
                        */
                        friend class Stage;

                        /*!
                            @brief デフォルトコンストラクタを呼べるのは、Stageクラスのみです。
                        */
                        explicit CombineFunction(){};
                    };

                    /*!
                        @brief カラーについてのコンバイン関数です。
                               型は nn::gr::CTR::Combiner::Stage::CombineFunction です。
                    */
                    CombineFunction rgb;

                    /*!
                        @brief アルファについてのコンバイン関数です。
                               型は nn::gr::CTR::Combiner::Stage::CombineFunction です。
                    */
                    CombineFunction alpha;

                    /*!
                        @brief コンスタントカラーの赤成分です。
                               型は u8 です。
                    */
                    u8 constColorR;

                    /*!
                        @brief コンスタントカラーの青成分です。
                               型は u8 です。
                    */
                    u8 constColorG;

                    /*!
                        @brief コンスタントカラーの緑成分です。
                               型は u8 です。
                    */
                    u8 constColorB;

                    /*!
                        @brief コンスタントカラーのアルファ成分です。
                               型は u8 です。
                    */
                    u8 constColorA;

                    /*!
                        @brief 設定するステージの先頭レジスタを設定します。
                               型は @ref PicaReg です。
                    */
                    util::SizedEnum2<PicaReg> headRegister;

                public :
                    /*!
                        @brief PRIMARY_COLORで置き換える設定にします。
                    */
                    void SetupPrimary( void );

                    /*!
                        @brief FRAGMENT_PRIMARY_COLORで置き換える設定にします.
                    */
                    void SetupFragmentPrimary( void );

                    /*!
                        @brief TEXTURE0で置き換える設定にします。
                    */
                    void SetupTexture0( void );

                    /*!
                        @brief 前ステージの色でスルーする設定にします。
                    */
                    void SetupPrevious( void );

                    /*!
                        @brief PRIMARY_COLORとTEXTURE0との乗算の設定にします。
                    */
                    void SetupPrimaryModulateTexture0( void );

                    /*!
                        @brief FRAGMENT_PRIMARY_COLORとTEXTURE0との乗算の設定にします。
                    */
                    void SetupFragmentPrimaryModulateTexture0( void );

                    /*!
                        @brief     ステージの初期値を設定します。

                        @param[in] stage_index コンバイナステージインデックスです（値は、0-5 です）
                    */
                    explicit Stage( const int stage_index );

                    /*!
                        @brief 設定された情報をもとに、コンバイナステージの設定コマンドを生成します。
                               コンバイナバッファについてのコマンドは含まれませんので、注意して下さい。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                        @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                        @see               Combiner::MakeCombinerBufferCommand()
                    */
                    bit32* MakeCommand( bit32* command ) const;

                    /*!
                        @brief コンバイナステージのコンスタントカラーのコマンドだけ生成します。

                        @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                        @return            書き込まれた描画コマンドの終端の次のアドレスを返します。
                    */
                    bit32* MakeConstantColorCommand( bit32* command ) const
                    {
                        NN_GR_ASSERT( ( PICA_REG_TEX_ENV0 <= headRegister ) &&
                                      ( PICA_REG_TEX_ENV5 >= headRegister ) );

                        u32 constColorReg = headRegister + 3;


                        *command++ = PICA_CMD_DATA_TEX_ENV_CONST(
                                        constColorR, constColorG,
                                        constColorB, constColorA );

                        *command++ = PICA_CMD_HEADER_SINGLE( constColorReg );

                        return command;
                    }

                protected :
                    /*!
                        @brief デフォルトコンストラクタを呼べるのは、Combinerクラスのみです。
                    */
                    friend class Combiner;

                    /*!
                        @brief デフォルトコンストラクタを呼べるのは、Combinerクラスのみです。
                    */
                    explicit Stage( void )
                    {
                    };
                };

            public :
                //! コンバイナは 6 つのステージから構成されます。
                static const u32 COMBINER_STAGE_MAX = 6;

                /*!
                    @brief 各ステージのコンバイナ設定を保持します。
                           型は nn::gr::CTR::Combiner::Stage [] で、
                           配列のサイズは @ref COMBINER_STAGE_MAX です。
                           各コンバイナのステージのコマンドを個別に生成したい場合は、
                           Stage::MakeCommand() を利用して下さい。
                */
                Stage stage[ COMBINER_STAGE_MAX ];

            public :
                /*!
                    @brief 先頭ステージのコンバイナバッファの出力の赤成分です。
                           型は u8 です。
                */
                u8 bufferColorR;

                /*!
                     @brief 先頭ステージのコンバイナバッファの出力の緑成分です。
                            型は u8 です。
                 */
                u8 bufferColorG;

                /*!
                     @brief 先頭ステージのコンバイナバッファの出力の青成分です。
                            型は u8 です。
                 */
                u8 bufferColorB;

                /*!
                     @brief 先頭ステージのコンバイナバッファの出力のアルファ成分です。
                            型は u8 です。
                 */
                u8 bufferColorA;

                /*!
                     @brief 各ステージの初期値を設定します。
                 */
                explicit Combiner();

                /*!
                    @brief 設定された情報をもとに、すべてのコンバイナステージの設定コマンドを生成します。
                           コンバイナバッファのコマンドも同時に生成されます。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。

                    @see       MakeBufferInputCommand()
                */
                bit32* MakeCommand( bit32* command ) const;


                /*!
                    @brief 設定された情報をもとに、コンバイナバッファの設定コマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeCombinerBufferCommand( bit32* command ) const;
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_COMBINER_H_
