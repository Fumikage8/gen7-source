/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_ShaderLite.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_SHADER_LITE_H_
#define NN_GR_CTR_GR_SHADER_LITE_H_

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <memory.h>

#define			bit32			u32
#define			NN_GR_ASSERT		//
// TORIAEZU: コンパイラ依存ではないのでここにあるのはおかしい
#define NN_PADDING1             int :  8
#define NN_PADDING2             int : 16
#define NN_PADDING3             int : 24
#define NN_PADDING4             int : 32

#define __gl2_h_
//#include <GLES2/gl2.h>のインクルードを抑止
#include <gl/glew.h>
#include <gl/gl.h>

#include <nn/gx/CTR/gx_MacroShader.h>

//#include <nn/gr/CTR/gr_Utility.h>
//#include <nn/gr/CTR/gr_BindSymbol.h>

#define	PicaDataDrawModeu8			u8

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief シェーダーバイナリ設定のためのクラスです。Shader クラスと比較して、メモリの消費を抑えることが出来ます。
             */
            class ShaderLite
            {
            public :
                /*!
                    @brief シェーダーバイナリ設定を初期化するコンストラクタです。
                */
                ShaderLite( void );

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
                    @param [in] buffer 動作に必要なバッファです。事前に GetRequiredBufferSize でサイズを取得し、アロケートしたものを渡してください。
                */
                void SetupBinary( const void* shader_binary, const s32 vtx_shader_index, const s32 geo_shader_index, bit32* buffer );


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
                    @brief 描画モードの設定を行います。
                           デフォルトは PICA_DATA_DRAW_TRIANGLESです。

                    @param[in] draw_mode 設定する描画モード
                 */
                void SetDrawMode( const PicaDataDrawModeu8 draw_mode )
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
                    memcpy( command, &m_CmdCacheConstArray[ m_ConstRgCmdOffsetArray[m_VtxShaderIndex] ],
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
                    memcpy( command, &m_CmdCacheConstArray[ m_ConstRgCmdOffsetArray[m_GeoShaderIndex] ], 
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

                    memcpy( command, &m_CmdCacheOutAttrArray[ m_CmdCacheOutAttrOffsetArray[ m_CombShaderIndex ] ],
                                 m_CmdCacheOutAttrNumArray[ m_CombShaderIndex ] * sizeof( bit32 ) );

                    return command + m_CmdCacheOutAttrNumArray[ m_CombShaderIndex ];
                }

            public : // バイナリからの情報を取得
                /*!
                    @brief シェーダバイナリから SetupBinary で必要となるバッファサイズを取得します。             

                    @param[in] shader_binary   ctr_VertexShaderLinker32.exeでリンクされたシェーダーバイナリです。

                    @return    必要なバッファサイズを返します。
                 */
                u32 GetRequiredBufferSize( const void* shader_binary );

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

            public :
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
                    @brief 必要なバッファサイズを計算します。

                    @return    必要なバッファサイズを返します。
                 */
                u32  CalculateBufferSize_() const;

                /*!
                    @brief バッファを割り当てます。

                    @param[in] 入力バッファ。
                 */
                void AssignBuffer_( bit32* buf);

                /*!
                    @brief 頂点シェーダ・ジオメトリシェーダの定数レジスタのコマンドキャッシュを生成します。
                 */
                void MakeShaderConstCommandCache_( void );

                /*!
                    @brief 頂点シェーダ・ジオメトリシェーダの出力属性のコマンドキャッシュを生成します。
                 */
                void MakeShaderOutAttrCommandCache_( void );

                /*!
                    @brief コマンドキャッシュ配列から適切に情報を得るためのオフセット値のキャッシュを生成します。
                 */
                 void MakeRgCmdOffsetCache_(void);

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
                                                      const PicaDataDrawModeu8 drawMode );

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
                    @brief 有効になっている頂点シェーダとジオメトリシェーダーを組み合わせたときのインデックスです。型は s32 です。
                */
                s32 m_CombShaderIndex;

                /*!
                    @brief シェーダーバイナリ内の、実行可能シェーダーの数です。
                */
                u8 m_ExeImageInfoNum;
                u8 m_ExeImageInfoNumPad[3];

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
                    @brief Swizzleパターンをコマンド形式に表現したものです。型は bit32* です。
                */
                bit32* m_Swizzle;

                /*!
                    @brief Swizzleパターンの数です。型は u32 です。
                */
                u32 m_SwizzleCount;

                /*!
                    @brief 描画モードをあらわします。デフォルトはPICA_DATA_DRAW_TRIANGLESです。型は @ref PicaDataDrawMode です。
                */
                PicaDataDrawModeu8 m_DrawMode;
                u8								m_DrawModePad[3];

                /*!
                    @brief 設定される頂点シェーダーのブールマップです。型は bit32 です。
                */
                bit32 m_VtxShaderBoolMapUniform;

                /*!
                    @brief 設定されるジオメトリシェーダーのブールマップです。型は bit32 です。
                */
                bit32 m_GeoShaderBoolMapUniform;

                /*!
                    @brief 出力属性のコマンドキャッシュの配列です。型は bit32* です。
                */
                bit32* m_CmdCacheOutAttrArray;

                /*!
                    @brief 出力属性のコマンド数の配列です。型は u32* です。
                */
                u32*   m_CmdCacheOutAttrNumArray;

                /*!
                    @brief シェーダーの定数レジスタのコマンドキャッシュの配列です。型は bit32* です。
                */
                bit32* m_CmdCacheConstArray;

                /*!
                    @brief シェーダーの定数レジスタのコマンド数の配列です。型は u32* です。
                */
                u32*   m_CmdCacheConstNumArray;
                
            protected :
                /*!
                    @brief ジオメトリシェーダの数です。型は u32 です。
                */
                u32    m_GeometryShaderNum;
                /*!
                    @brief シェーダの定数レジスタコマンド生成用のオフセット値キャッシュの配列です。型は u32* です。
                */
                u32*   m_ConstRgCmdOffsetArray;
                /*!
                    @brief 出力属性のコマンドキャッシュに必要なインデックス用配列です。型は u32* です。
                */
                u32*   m_CmdCacheOutAttrIndexArray;
                /*!
                    @brief 出力属性のコマンドキャッシュに必要なオフセット値の配列です。型は u32* です。
                */
                u32*   m_CmdCacheOutAttrOffsetArray;
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_SHADER_H_
