/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Shader.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_SHADER_H_
#define NN_GR_CTR_GR_SHADER_H_

#include <nn/util/util_SizedEnum.h>
#include <nn/gr/CTR/gr_Utility.h>
#include <nn/gr/CTR/gr_BindSymbol.h>

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief シェーダーバイナリ設定のためのクラスです。
             */
            class Shader
            {
            public :
                /*!
                    @brief シェーダーバイナリ設定を初期化するコンストラクタです。
                */
                Shader( void );

                /*!
                    @brief シェーダーバイナリをもとに、コマンド生成用の情報をセットアップします。
                           ※バイナリのコピーはしないので、コマンド生成までバイナリは破棄しないで下さい。
                           指定されたインデクスが有効なシェーダーでない場合、ASSERTです。
                           指定するインデクスは、ctr_VertexShaderLinker32.exeでのリンク時に生成される
                           MAPファイルを参考にして下さい。
                           geo_shader_indexについては、-1を指定することでジオメトリシェーダーを無効化することが出来ます。

                    @param [in] shader_binary ctr_VertexShaderLinker32.exeでリンクされたシェーダーバイナリです。
                    @param [in] vtx_shader_index 有効にする頂点シェーダーのインデクスです。
                    @param [in] geo_shader_index 有効にするジオメトリシェーダーシェーダーのインデクスです。
                */
                void SetupBinary( const void* shader_binary, const s32 vtx_shader_index, const s32 geo_shader_index );


                /*!
                    @brief 使用するシェーダーのインデックスを設定します。
                           必ず、 SetupBinary() の呼び出した後に使用して下さい。
                           指定するインデクスは、ctr_VertexShaderLinker32.exeでのリンク時に生成されるMAPファイルを参考にして下さい。                  
                           geo_shader_indexについては、-1 を指定することでジオメトリシェーダーを無効化することが出来ます。
                           ジオメトリシェーダの使用・未使用を切り替えた際、
                           コマンド生成の前に SetDrawMode() で描画モードを再設定する必要があります。

                    @param [in] vtx_shader_index 有効にする頂点シェーダーのインデクスです。
                    @param [in] geo_shader_index 有効にするジオメトリシェーダーシェーダーのインデクスです。
                */
                void SetShaderIndex( const s32 vtx_shader_index, const s32 geo_shader_index );

            public :
                /*!
                    @brief 頂点シェーダーにブールレジスタの値を設定します。

                    @param [in] symbol   頂点シェーダのブール型のシンボルです。
                    @param [in] isEnable 有効にするときは true、そうでないときは false にします。
                 */
                void SetUniformBool( const BindSymbolVSBool& symbol, bool isEnable )
                {
                    m_VtxShaderBoolMapUniform = ( m_VtxShaderBoolMapUniform & ~( 1 << symbol.start ) ) | ( ( isEnable ? 1 : 0 ) << symbol.start );
                }

                /*!
                    @brief ジオメトリシェーダーにブールレジスタの値を設定します。

                    @param [in] symbol   ジオメトリシェーダのブール型のシンボルです。
                    @param [in] isEnable 有効にするときは true、そうでないときは false にします。
                 */
                void SetUniformBool( const BindSymbolGSBool& symbol, bool isEnable )
                {
                    m_GeoShaderBoolMapUniform = ( m_GeoShaderBoolMapUniform & ~( 1 << symbol.start ) ) | ( ( isEnable ? 1 : 0 ) << symbol.start );
                }

                /*!
                    @brief 描画モードの設定を行います。
                           デフォルトは PICA_DATA_DRAW_TRIANGLESです。

                    @param[in] draw_mode 設定する描画モード
                 */
                void SetDrawMode( const PicaDataDrawMode draw_mode )
                {
                    m_DrawMode = draw_mode;
                }

            public :
                /*!
                    @brief シェーダー切り替えに必要な全てのコマンドを生成します。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeFullCommand( bit32* command ) const;


                /*!
                    @brief シェーダバイナリ中のシェーダーイメージの切り替えに必要な最低限のコマンドを生成します。
                           MakeFullCommand() を呼び出して、コマンドを送った後に呼ぶ必要があります。
                           また SetShaderIndex() でジオメトリシェーダの使用・未使用のモードを切り替えたときには、
                           引数 isMakePrepareCommand を true にする必要があります。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isMakePrepareCommand ジオメトリシェーダーの使用・未使用のモードを切り替えるコマンドを生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeShaderCommand( bit32* command, const bool isMakePrepareCommand = true ) const;

                /*!
                    @brief 無効化するコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeDisableCommand( bit32* command );

            public :
                /*!
                    @brief 頂点シェーダーのブールレジスタへ値を設定するコマンドを生成します。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeVtxBoolMapCommand( bit32* command ) const
                {
                    *command++ = 0x7fff0000 | m_VtxShaderBoolMapUniform;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_BOOL ); // 0x2b0
                    return command;
                }

                /*!
                    @brief ジオメトリシェーダーのブールレジスタへ値を設定するコマンドを生成します。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。
                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeGeoBoolMapCommand( bit32* command ) const
                {
                    *command++ = 0x7fff0000 | m_GeoShaderBoolMapUniform;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_BOOL ); // 0x280
                    return command;
                }

                /*!
                    @brief ジオメトリシェーダー使用設定コマンドを生成します。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。
                           ジオメトリシェーダーの有効/無効を設定するときに必要なダミーコマンド、
                           有効、無効の設定コマンドなどが含まれます。
                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakePrepareCommand( bit32* command ) const;

                /*!
                    @brief 頂点シェーダーについて、プログラムロードのコマンドを生成します。
                           エントリポイントのアドレス、シェーダーバイナリ内の実行コード部分などが含まれます。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。
                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeVtxProgramCommand( bit32* command ) const;

                /*!
                    @brief ジオメトリシェーダーについて、プログラムロードのコマンドを生成します。
                           エントリポイントのアドレス、シェーダーバイナリ内の実行コード部分などが含まれます。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeGeoProgramCommand( bit32* command ) const;

                /*!
                    @brief 頂点シェーダーについて、Swizzleパターンロードのコマンドを生成します。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeVtxSwizzleCommand( bit32* command ) const
                {
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_PROG_SWIZZLE_ADDR );
                    return MakeLoadCommand_( command, PICA_REG_VS_PROG_SWIZZLE_DATA0, m_Swizzle, m_SwizzleCount );
                }

                /*!
                    @brief ジオメトリシェーダーについて、Swizzleパターンロードのコマンド生成します。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeGeoSwizzleCommand( bit32* command ) const
                {
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_PROG_SWIZZLE_ADDR );
                    return MakeLoadCommand_( command, PICA_REG_GS_PROG_SWIZZLE_DATA0, m_Swizzle, m_SwizzleCount );
                }

                /*!
                    @brief 頂点シェーダーについて、定数レジスタへ値を設定するコマンド生成をします。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeVtxConstRgCommand( bit32* command ) const
                {
                    std::memcpy( command, m_CmdCacheConstArray[ m_VtxShaderIndex ],
                                 m_CmdCacheConstNumArray[ m_VtxShaderIndex ] * sizeof( bit32 ) );
                    return command + m_CmdCacheConstNumArray[ m_VtxShaderIndex ];
                }

                /*!
                    @brief ジオメトリシェーダーについて、定数レジスタへ値を設定するコマンドを生成します。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeGeoConstRgCommand( bit32* command ) const
                {
                    std::memcpy( command, m_CmdCacheConstArray[ m_GeoShaderIndex ],
                                 m_CmdCacheConstNumArray[ m_GeoShaderIndex ] * sizeof( bit32 ) );
                    return command + m_CmdCacheConstNumArray[ m_GeoShaderIndex ];
                }

                /*!
                    @brief シェーダーの出力属性を設定するコマンド生成します。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeOutAttrCommand( bit32* command ) const
                {
                    // ジオメトリシェーダのモードと描画モードが一致していなかったら、ASSERT
                    NN_GR_ASSERT( ( ! IsEnableGeoShader() ) || ( m_DrawMode == PICA_DATA_DRAW_GEOMETRY_PRIMITIVE ) );

                    // 三角形の描画モードに応じて、コマンドを生成
                    // 0x229 [8:8]
                    bit32 value_229 = 0x0100;
                    if ( m_DrawMode != PICA_DATA_DRAW_TRIANGLES )
                    {
                        value_229 = 0x0;
                    }
                    *command++ = value_229;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE0, 0x2 );

                    // 0x253 [8:8]
                    bit32 value_253 = 0x0100;
                    if ( m_DrawMode != PICA_DATA_DRAW_TRIANGLES )
                    {
                        value_253 = 0x0;
                    }
                    *command++ = value_253;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE1, 0x2 );

                    std::memcpy( command, m_CmdCacheOutAttrArray,
                                 m_CmdCacheOutAttrNum * sizeof( bit32 ) );

                    return command + m_CmdCacheOutAttrNum;
                }

            public : // バイナリからの情報を取得
                /*!
                   @brief シンボル名から、シンボル情報への変換をおこないます。
                         「シェーダーコード内で#pragma bind_symbol() で設定したシンボル名」を引数に与えることで、
                          シンボルの型情報とバインドされているレジスタ番号が取得できます。
                          SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                 * @param[out] symbol             シンボル名によって検索されたシンボルです。
                 * @param[in]  name               検索したいシンボル名です。

                 * @return     検索対象が見つかった場合、trueを返します。
                 */
                bool SearchBindSymbol(
                    BindSymbol*      symbol,
                    const char*      name ) const;

                 /*!
                    @brief 指定された型のシンボルについて、いくつあるか検索します。
                           シェーダーコード内で#pragma bind_symbol() されているもののうち、
                           指定された型のものがいくつあるか検索します。
                           SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param[in]  shader_type 検索したいシェーダーの種類です。
                    @param[in]  symbol_type 検索したい型です。

                    @return     見つかったシンボルの数を返します。
                 */
                u32  SearchBindSymbolNum(
                    const BindSymbol::ShaderType shader_type,
                    const BindSymbol::SymbolType symbol_type ) const;

                /*!
                   @brief 指定された型のシンボルについて、インデックス（0からシンボル数-1）からシンボル情報への変換を行います。
                          シェーダーコード内で、#pragma bind_symbol() されている順番で、
                          0,1,...シンボル数-1というインデックスが振られます。
                          指定されたインデックスを検索し、見つかったシンボルの情報を取得できます。
                          SetupBinaryでシェーダーバイナリを設定した後に呼ぶようにして下さい。

                   @param[out] symbol             インデックスに対応するシンボルです。
                   @param[in]  symbol_index       検索したいインデックスです。

                   @return     検索対象が見つかった場合、trueを返します。
                 */
                bool SearchBindSymbol(
                    BindSymbol*      symbol,
                    const u8         symbol_index ) const;

            public : // 設定の取得
                /*!
                    @brief シェーダーバイナリに含まれる実行可能シェーダーの数を取得します。

                    @return 実行可能シェーダーの数です。
                 */
                u8 GetShaderNum()           const
                {
                    return m_ExeImageInfoNum;
                }

                /*!
                    @brief  ジオメトリシェーダーが有効かどうかを取得します。

                    @return EnableGeoShader()によって有効化されていれば、true です。
                    @see    EnableGeoShader()、DisableGeoShader()
                 */
                bool IsEnableGeoShader()    const
                {
                    return ( 0 <= GetGeoShaderIndex() );
                }

                /*!
                    @brief  有効になっている頂点シェーダーのインデックスを取得します。

                    @return EnableVtxShader()によって有効化されているシェーダーインデックスです。
                    @see    EnableVtxShader()
                 */

                s32 GetVtxShaderIndex()     const
                {
                    return m_VtxShaderIndex;
                }

                /*!
                    @brief  有効になっている有効になっているジオメトリシェーダーのインデックスを取得します。

                    @return EnableGeoShader()によって有効化されているシェーダーインデックス
                    @see    EnableGeoShader()、DisableGeoShader()
                 */
                s32 GetGeoShaderIndex()     const
                {
                    return m_GeoShaderIndex;
                }

                /*!
                    @brief 頂点シェーダーに設定されるブールマップを取得します。

                    @return ブールマップです。
                 */
                bit32 GetVtxShaderBoolMapUniform() const
                {
                    return m_VtxShaderBoolMapUniform;
                }

                /*!
                    @brief ジオメトリシェーダーに設定されるブールマップを取得します。

                    @return ブールマップです。
                 */
                bit32 GetGeoShaderBoolMap()    const
                {
                    return m_GeoShaderBoolMapUniform;
                }

            protected :
                /*!
                    @brief クラス定数です。
                 */
                 //! 実行可能シェーダーの最大数です。
                 static const s32 EXE_IMAGE_MAX = 32;

                 //! Swizzleパターンの最大数です。
                 static const s32 SWIZZLE_PATTERN_MAX = 128;

                 //! 0x251に設定するダミーコマンドの数です。
                 static const s32 DUMMY_DATA_NUM_251 = 10;

                 //! 0x200に設定するダミーコマンドの数です。
                 static const s32 DUMMY_DATA_NUM_200 = 30;

                 //! ダミーコマンドなど、パディングにつかう値です。
                 static const u32 PADDING_DATA = 0xead0fead;

                 //! 出力属性のコマンドの最大数です。
                 static const s32 OUT_ATTR_COMMAND_MAX = 48;

                 //! 定数レジスタコマンドの最大数です。
                 static const u32 CONST_REG_COMMAND_MAX = 96 * 6 + 5 * 2;

            protected :
                /*!
                    @brief 頂点シェーダのインデックスの有効性のテストをします。
                           指定されたインデクスが有効なシェーダーでない場合、ASSERT が発生します。
                   　　　　SetupBinary でシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param [in] vtx_shader_index テストする頂点シェーダーのインデクスです。
                */
                void CheckVtxShaderIndex_( const s32 vtx_shader_index );

               /*!
                    @brief ジオメトリシェーダのインデックスの有効性のテストをします。
                           指定されたインデクスが有効なシェーダーでない場合、ASSERT が発生します。
                   　　　　SetupBinary でシェーダーバイナリを設定した後に呼ぶようにして下さい。

                    @param [in] geo_shader_index テストするジオメトリシェーダのインデクスです。
                */
                void CheckGeoShaderIndex_( const s32 geo_shader_index );

                /*!
                    @brief 頂点シェーダ・ジオメトリシェーダの定数レジスタのコマンドキャッシュを生成します。
                 */
                void MakeShaderConstCommandCache_( void );

                /*!
                    @brief 頂点シェーダ・ジオメトリシェーダの出力属性のコマンドキャッシュを生成します。
                 */
                void MakeShaderOutAttrCommandCache_( void );

                /*!
                    @brief ジオメトリシェーダーの有効/無効を設定するときに必要なダミーコマンド、
                           有効、無効の設定コマンドを生成します。
                    @param[in] command           描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isEnableGeoShader ジオメトリシェーダモードにするときに true にします。
                    @param[in] drawMode          描画モードです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeShaderModeCommand_( bit32* command, 
                                                      const bool isEnableGeoShader,
                                                      const PicaDataDrawMode drawMode );

                /*!
                    @brief 定数レジスタへ値を設定するコマンド生成します。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] shader_index       有効にする頂点シェーダーのインデクスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */

                bit32* MakeConstRgCommand_( bit32* command,
                                            const s32 shader_index );

                /*!
                    @brief シェーダーの出力属性を設定するコマンド生成します。

                    @param[in] command            描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] vtx_shader_index   頂点シェーダーのインデクスです。
                    @param[in] geo_shader_index   ジオメトリシェーダーのインデクスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeOutAttrCommand_( bit32* command, const s32 vtx_shader_index, const s32 geo_shader_index );

                /*!
                    @brief ひとまとまりのデータを指定されたレジスタに送信するコマンドを生成します。SEQ、BEには非対応です。

                    @param[in]  command          コマンドを追加するバッファのアドレスです。
                    @param[in]   load_reg         送信するレジスタです。
                    @param[in]   src_buffer_ptr   送信したいデータのバッファの先頭アドレスです。
                    @param[in]   src_size         送信したいデータの数です。

                    @return                       書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeLoadCommand_( bit32* command,
                                         const bit32  load_reg,
                                         const u32* src_buffer_ptr,
                                         const u32  src_size ) const;

                /*!
                    @brief 指定されたレジスタにダミーコマンドを送信するコマンドを生成します。

                    @param   command        [in] コマンドを追加するバッファのアドレス
                    @param   load_reg       [in]  送信するレジスタ
                    @param   data_num       [in]  送信したいデータの数

                    @return                       書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                 static bit32* MakeDummyCommand_( bit32* command,
                                                  const bit32 load_reg,
                                                  const u32 data_num );

                /*!
                    @brief シェーダーバイナリ内の実行コード部分をあらわす構造体です。
                 */
                struct ExeImageInfo
                {
                    //! 型は bit32 です。
                    bit32 signature;
                    //! 型は u16 です。
                    u16 version;
                    //! 型は u8 です。
                    u8  isGeoShader;
                    //! 型は u8 です。
                    u8  outputMaps;
                    //! 型は uptr です。
                    uptr mainAddress;
                    //! 型は uptr です。
                    uptr endAddress;
                    //! 型は bit32 です。
                    bit32 maskInputOutput;
                    //! 型は u8 です。
                    u8  gsDataMode;
                    //! 型は u8 です。
                    u8  gsVertexStartIndex;
                    //! 型は u8 です。
                    u8  gsPatchSize;
                    //! 型は u8 です。
                    u8  gsVertexNum;
                    //! 型は u32 です。
                    u32 setupOffset;
                    //! 型は u32 です。  
                    u32 setupCount;
                    //! 型は u32 です。
                    u32 labelOffset;
                    //! 型は u32 です。
                    u32 labelCount;
                    //! 型は u32 です。
                    u32 outMapOffset;
                    //! 型は u32 です。
                    u32 outMapCount;
                    //! 型は u32 です。
                    u32 bindSymbolOffset;
                    //! 型は u32 です。
                    u32 bindSymbolCount;
                    //! 型は u32 です。
                    u32 stringOffset;
                    //! 型は u32 です。
                    u32 stringCount;
                };

                /*!
                    @brief 有効になっている頂点シェーダーのインデックスです。型は s32 です。
                */
                s32 m_VtxShaderIndex;

                /*!
                    @brief 有効になっているジオメトリシェーダーのインデックスです。型は s32 です。
                */
                s32 m_GeoShaderIndex;

                /*!
                    @brief シェーダーバイナリ内の、実行可能シェーダーの数です。
                */
                u8 m_ExeImageInfoNum;
                NN_PADDING3;

                /*!
                    @brief シェーダーバイナリ内の、実行可能シェーダーの情報へのポインタを保持します。型は @ref ExeImageInfo[] です。
                */
                const ExeImageInfo* m_ExeImageInfo[ EXE_IMAGE_MAX ];

                /*!
                    @brief シェーダーバイナリ内の、シェーダーの実行コード部分へのポインタです。型は bit32* です。
                */
                const bit32* m_Instruction;

                /*!
                    @brief シェーダーバイナリ内の、シェーダーの実行コード部分のサイズです。型は u32 です。
                */
                u32 m_InstructionCount;

                /*!
                    @brief Swizzleパターンをコマンド形式に表現したものです。型は bit32[] です。
                */
                bit32 m_Swizzle[ SWIZZLE_PATTERN_MAX ];

                /*!
                    @brief Swizzleパターンの数です。型は u32 です。
                */
                u32 m_SwizzleCount;

                /*!
                    @brief 描画モードをあらわします。デフォルトはPICA_DATA_DRAW_TRIANGLESです。型は @ref PicaDataDrawMode です。
                */
                util::SizedEnum1<PicaDataDrawMode> m_DrawMode;
                NN_PADDING3;

                /*!
                    @brief 設定される頂点シェーダーのブールマップです。型は bit32 です。
                */
                bit32 m_VtxShaderBoolMapUniform;

                /*!
                    @brief 設定されるジオメトリシェーダーのブールマップです。型は bit32 です。
                */
                bit32 m_GeoShaderBoolMapUniform;

                /*!
                    @brief 出力属性のコマンドキャッシュの 2次元配列です。型は bit32 です。
                */
                bit32 m_CmdCacheOutAttrArray[ OUT_ATTR_COMMAND_MAX ];

                /*!
                    @brief 出力属性のコマンド数の配列です。型は u32 です。
                */
                u32 m_CmdCacheOutAttrNum;

                /*!
                    @brief シェーダーの定数レジスタのコマンドキャッシュの 2 次元配列です。型は bit32[][] です。
                */
                bit32 m_CmdCacheConstArray[ EXE_IMAGE_MAX ][ CONST_REG_COMMAND_MAX ];

                /*!
                    @brief シェーダーの定数レジスタのコマンド数の配列です。型は u32[] です。
                */
                u32 m_CmdCacheConstNumArray[ EXE_IMAGE_MAX ];
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_SHADER_H_
