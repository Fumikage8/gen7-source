/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_ProcedureTexture.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_PROCEDURE_TEXTURE_H_
#define NN_GR_CTR_GR_PROCEDURE_TEXTURE_H_

#include <nn/util/util_SizedEnum.h>
#include <nn/gr/CTR/gr_Utility.h>

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief プロシージャルテクスチャのコマンドを生成するのためのクラスです。
             */
            class ProcedureTexture
            {
            public:
                static const u32 PROCTEX_LOOKUP_TABLE_ELEMENT_NUM       = 128; //!< 参照テーブルは 128 個のデータを持ちます。         
                static const u32 PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM = 256; //!< カラー用の参照テーブルは 256 個のデータを持ちます。

            public :
                /*!
                    @brief RGB のマッピング関数のモードです。
                           型は @ref PicaDataProcTexMap です。
                           dmp_Texture[3].ptRgbMap ユニフォームに相当します。
                */
                util::SizedEnum1<PicaDataProcTexMap> rgbMap;

                /*!
                    @brief アルファのマッピング関数のモードです。
                           型は @ref PicaDataProcTexMap です。
                           dmp_Texture[3].ptAlphaMap ユニフォームに相当します。
                */

                util::SizedEnum1<PicaDataProcTexMap> alphaMap;

                /*!
                    @brief マッピング関数でアルファを独立して扱うかどうかです。
                           型は bool です。
                           dmp_Texture[3].ptAlphaSeparate ユニフォームに相当します。
                */
                bool alphaSeparate;

                /*!
                    @brief テクスチャの U 方向のクランプのモードです。
                           型は @ref PicaDataProcTexClamp です。
                           dmp_Texture[3].ptClampU ユニフォームに相当します。
                */
                util::SizedEnum1<PicaDataProcTexClamp> clampU;

                /*!
                    @brief テクスチャの V 方向のクランプのモードです。
                           型は @ref PicaDataProcTexClamp です。
                           dmp_Texture[3].ptClampV ユニフォームに相当します。
                */
                util::SizedEnum1<PicaDataProcTexClamp> clampV;

                /*!
                    @brief テクスチャの U 方向のシフト計算のモードです。
                           型は @ref PicaDataProcTexShift です。
                           dmp_Texture[3].ptShiftU ユニフォームに相当します。
                */
                util::SizedEnum1<PicaDataProcTexShift> shiftU;

                /*!
                    @brief テクスチャの V 方向のシフト計算のモードです。
                           型は @ref PicaDataProcTexShift です。
                           dmp_Texture[3].ptShiftV ユニフォームに相当します。
                */
                util::SizedEnum1<PicaDataProcTexShift> shiftV;

                /*!
                    @brief テクスチャの MinFilter の設定です。
                           型は @ref PicaDataProcTexFilter です。
                           dmp_Texture[3].ptMinFilter ユニフォームに相当します。
                */
                util::SizedEnum1<PicaDataProcTexFilter> minFilter;

                /*!
                    @brief カラー参照テーブルの LOD バイアスです。
                           型は f32 で値は [0.0, 6.0] でデフォルト値は 0.5f です。
                           dmp_Texture[3].texBias ユニフォームに相当します。
                */
                f32 texBias;

                /*!
                    @brief 使用される部分配列の先頭要素を指定します。
                           型は u8 で、値は [0, 128] でデフォルト値は 0 です。
                           dmp_Texture[3].ptTexOffset ユニフォームに相当します。
                */
                u8  texOffset;

                /*!
                    @brief 参照テーブルのデータ幅を指定します
                           型は u8 で、値は [0, 128] でデフォルト値は 0 です。
                           dmp_Texture[3].ptTexWidth ユニフォームに相当します。
                */
                u8  texWidth;

                /*!
                    @brief ノイズを有効化します。
                           型は bool で、デフォルト値は false です。
                           dmp_Texture[3].ptNoiseEnable ユニフォームに相当します。
                */
                bool isEnableNoise;
                NN_PADDING1;

                /*!
                    @brief テクスチャの U 方向のノイズの振幅を設定します。
                           型は f32 で、デフォルト値は 0.0f です。
                           dmp_Texture[3].ptNoiseU[2] ユニフォームに相当します。
                */
                f32 noiseUAmplitude;

                /*!
                    @brief テクスチャの V 方向のノイズの振幅を設定します。
                           型は f32 で、デフォルト値は 0.0f です。
                           dmp_Texture[3].ptNoiseV[2] ユニフォームに相当します。
                */
                f32 noiseVAmplitude;

                /*!
                    @brief テクスチャの U 方向のノイズの周波数を設定します。
                           型は f32 で、デフォルト値は 0.0f です。
                           dmp_Texture[3].ptNoiseU[0] ユニフォームに相当します。
                */
                f32 noiseUFrequency;

                /*!
                    @brief テクスチャの V 方向のノイズの周波数を設定します。
                           型は f32 で、デフォルト値は 0.0f です。
                           dmp_Texture[3].ptNoiseV[0] ユニフォームに相当します。
                */
                f32 noiseVFrequency;

                /*!
                    @brief テクスチャの U 方向のノイズの位相を設定します。
                           型は f32 で、デフォルト値は 0.0f です。
                           dmp_Texture[3].ptNoiseU[1] ユニフォームに相当します。
                */
                f32 noiseUPhase;

                /*!
                    @brief テクスチャの V 方向のノイズの位相を設定します。
                           型は f32 で、デフォルト値は 0.0f です。
                           dmp_Texture[3].ptNoiseV[1] ユニフォームに相当します。
                */
                f32 noiseVPhase;

            public :
                /*!
                    @brief ノイズの連続性関数の参照テーブルを設定します。

                    @param[in] lookUpTable        128個の参照データです。
                    @param[in] lookUpTableIndex   参照テーブルのインデックスです。
                    @param[in] lastDiffValue      参照データの最後の差分値です。
                */
                void SetNoiseLookUpTable( const f32 lookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                          const u32 lookUpTableIndex = 0,
                                          const f32 lastDiffValue    = 0.0f );

                /*!
                    @brief ノイズの連続性関数の参照テーブルを設定します。

                    @param[in] lookUpTable            128個の参照データです。
                    @param[in] diffLookUpTable        128個の参照データの差分値です。
                    @param[in] lookUpTableIndex       参照テーブルのインデックスです。
                */
                void SetNoiseLookUpTable( const f32 lookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                          const f32 diffLookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                          const u32 lookUpTableIndex = 0 );

                /*!
                    @brief RGB のマッピング関数の参照テーブルを設定します。

                    @param[in] lookUpTable        128個の参照データです。
                    @param[in] lookUpTableIndex   参照テーブルのインデックスです。
                    @param[in] lastDiffValue      参照データの最後の差分値です。
                */
                void SetRgbMapLookUpTable( const f32 lookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                           const u32 lookUpTableIndex = 0,
                                           const f32 lastDiffValue    = 0.0f );

                /*!
                    @brief RGB のマッピング関数の参照テーブルを設定します。

                    @param[in] lookUpTable           128個の参照データです。
                    @param[in] diffLookUpTable       128個の参照データの差分値です。
                    @param[in] lookUpTableIndex      参照テーブルのインデックスです。
                */
                void SetRgbMapLookUpTable( const f32 lookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                           const f32 diffLookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                           const u32 lookUpTableIndex = 0 );

                /*!
                    @brief アルファのマッピング関数の参照テーブルを設定します。

                    @param[in] lookUpTable        128個の参照データです。
                    @param[in] lookUpTableIndex   参照テーブルのインデックスです。
                    @param[in] lastDiffValue      参照データの最後の差分値です。
                */
                void SetAlphaMapLookUpTable( const f32 lookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                             const u32 lookUpTableIndex = 0,
                                             const f32 lastDiffValue    = 0.0f );

                /*!
                    @brief アルファのマッピング関数の参照テーブルを設定します。

                    @param[in] lookUpTable           128個の参照データです。
                    @param[in] diffLookUpTable       128個の参照データの差分値です。
                    @param[in] lookUpTableIndex      参照テーブルのインデックスです。
                */
                void SetAlphaMapLookUpTable( const f32 lookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                             const f32 diffLookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                             const u32 lookUpTableIndex = 0 );

                /*!
                    @brief 赤成分のカラー参照テーブルを設定します。

                    @param[in] lookUpTable        赤成分の256個の参照データです。
                    @param[in] lookUpTableIndex   参照テーブルのインデックスです。
                */
                void SetRedLookUpTable( const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                        const u32 lookUpTableIndex = 0 );

                /*!
                    @brief 赤成分のカラー参照テーブルを設定します。

                    @param[in] lookUpTable          赤成分の256個の参照データです。
                    @param[in] lookUpDiffTable      赤成分の256個の参照データの差分値です。
                    @param[in] lookUpTableIndex     参照テーブルのインデックスです。
                    @param[in] diffLookUpTableIndex 参照テーブルの差分値のインデックスです。
                */
                void SetRedLookUpTable( const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                        const f32 diffLookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                        const u32 lookUpTableIndex = 0,
                                        const u32 diffLookUpTableIndex = 0 );

                /*!
                    @brief 緑成分のカラー参照テーブルを設定します。

                    @param[in] lookUpTable        緑成分の256個の参照データです。
                    @param[in] lookUpTableIndex   参照テーブルのインデックスです。
                */
                void SetGreenLookUpTable( const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                          const u32 lookUpTableIndex = 0 );

                /*!
                    @brief 緑成分のカラー参照テーブルを設定します。

                    @param[in] lookUpTable          緑成分の256個の参照データです。
                    @param[in] lookUpDiffTable      緑成分の256個の参照データの差分値です。
                    @param[in] lookUpTableIndex     参照テーブルのインデックスです。
                    @param[in] diffLookUpTableIndex 参照テーブルの差分値のインデックスです。
                */
                void SetGreenLookUpTable( const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                          const f32 diffLookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                          const u32 lookUpTableIndex = 0,
                                          const u32 diffLookUpTableIndex = 0 );

                /*!
                    @brief 青成分のカラー参照テーブルを設定します。

                    @param[in] lookUpTable        青成分の256個の参照データです。
                    @param[in] lookUpTableIndex   参照テーブルのインデックスです。
                */
                void SetBlueLookUpTable( const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                         const u32 lookUpTableIndex = 0 );

                /*!
                    @brief 青成分のカラー参照テーブルを設定します。

                    @param[in] lookUpTable          青成分の256個の参照データです。
                    @param[in] lookUpDiffTable      青成分の256個の参照データの差分値です。
                    @param[in] lookUpTableIndex     参照テーブルのインデックスです。
                    @param[in] diffLookUpTableIndex 参照テーブルの差分値のインデックスです。
                */
                void SetBlueLookUpTable( const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                         const f32 diffLookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                         const u32 lookUpTableIndex = 0,
                                         const u32 diffLookUpTableIndex = 0 );

               /*!
                    @brief アルファ成分のカラー参照テーブルを設定します。

                    @param[in] lookUpTable        アルファ成分の256個の参照データです。
                    @param[in] lookUpTableIndex   参照テーブルのインデックスです。
                */
                void SetAlphaLookUpTable( const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                          const u32 lookUpTableIndex = 0 );

                /*!
                    @brief アルファ成分のカラー参照テーブルを設定します。

                    @param[in] lookUpTable          アルファ成分の256個の参照データです。
                    @param[in] lookUpDiffTable      アルファ成分の256個の参照データの差分値です。
                    @param[in] lookUpTableIndex     参照テーブルのインデックスです。
                    @param[in] diffLookUpTableIndex 参照テーブルの差分値のインデックスです。
                */
                void SetAlphaLookUpTable( const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                          const f32 diffLookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                          const u32 lookUpTableIndex = 0,
                                          const u32 diffLookUpTableIndex = 0 );

                /*!
                    @brief プロシージャルテクスチャのレジスタを設定する全てのコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddDummyCommand 0x80 へのダミー書き込みコマンドを 3回 生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeAllCommand( bit32* command, bool isAddDummyCommand = true ) const
                {
                    // 設定用コマンド
                    command = MakeConfigCommand( command, isAddDummyCommand );

                    // プロシージャルテクスチャの参照テーブルのコマンド 
                    command = MakeTableCommand( command ); 

                    return command;
                }

                /*!
                    @brief プロシージャルテクスチャの全ての設定のコマンドを生成します。(参照テーブルのコマンドは生成しません。)

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddDummyCommand レジスタの変更前に、0x80 へのダミー書き込みコマンドを 3回 生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeConfigCommand( bit32* command, bool isAddDummyCommand = true ) const
                {
                    command = MakeProcedureTextureModeCommand( command, isAddDummyCommand );
                    command = MakeTextureCommand( command, false, false );
                    command = MakeNoiseCommand( command, false, false );

                    return command;
                }

                /*!
                    @brief プロシージャルテクスチャのモード設定用のコマンドだけを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddDummyCommand レジスタの変更前に、0x80 へのダミー書き込みコマンドを 3回 生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeProcedureTextureModeCommand( bit32* command, bool isAddDummyCommand = true ) const
                {
                    if ( isAddDummyCommand )
                    {
                        command = MakeDummyCommand( command );
                    }

                    // f32 -> f16
                    u16 texBiasFloat16 = Float32ToFloat16( texBias );
                    u8  texBiasFloat16Low8 = ( texBiasFloat16          & 0xFF );

                    // 0x0a8
                    *command++ = PICA_CMD_DATA_PROCTEX0( clampU, clampV,
                                                         rgbMap, alphaMap,
                                                         ( alphaSeparate ? 1 : 0),
                                                         ( isEnableNoise ? 1 : 0 ),
                                                         shiftU, shiftV,
                                                         texBiasFloat16Low8 );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE3_PROCTEX0 );

                    return command;
                }

                /*!
                    @brief プロシージャルテクスチャのテクスチャ設定用のコマンドだけを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddProcedureTextureModeCommand  テクスチャに関連した texBias, clampU, clampV, shiftU, shiftV のいずれかのコマンドを生成したいときに true にします。
                    @param[in] isAddDummyCommand レジスタの変更前に、0x80 へのダミー書き込みコマンドを 3回 生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeTextureCommand( bit32* command, bool isAddProcedureTextureModeCommand = true, bool isAddDummyCommand = true ) const
                {
                    if ( isAddProcedureTextureModeCommand )
                    {
                        command = MakeProcedureTextureModeCommand( command, isAddDummyCommand );
                    }
                    else if ( isAddDummyCommand )
                    {
                        command = MakeDummyCommand( command );
                    }

                    // f32 -> f16 
                    u16 texBiasFloat16 = Float32ToFloat16( texBias );
                    u8  texBiasFloat16Hi8 =  ( ( texBiasFloat16 >> 8 ) & 0xFF );

                    // 0x0ac
                    *command++ = PICA_CMD_DATA_PROCTEX4( minFilter, texWidth, texBiasFloat16Hi8 );
                    *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_TEXTURE3_PROCTEX4, 0x2 );

                    // 0x0ad
                    *command++ = PICA_CMD_DATA_PROCTEX5( texOffset );
                    // padding
                    *command++ = 0x0;

                    return command;
                }

                /*!
                    @brief プロシージャルテクスチャのノイズ設定用のコマンドだけを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] isAddProcedureTextureModeCommand  ノイズに関連した isEnableNoise, rgbMap, alphaMap のいずれかのコマンドを生成したいときに true にします。
                    @param[in] isAddDummyCommand レジスタの変更前に、0x80 へのダミー書き込みコマンドを 3回 生成します。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeNoiseCommand( bit32* command, bool isAddProcedureTextureModeCommand = true, bool isAddDummyCommand = true ) const
                {
                    if ( isAddProcedureTextureModeCommand )
                    {
                        command = MakeProcedureTextureModeCommand( command, isAddDummyCommand );
                    }
                    else if ( isAddDummyCommand )
                    {
                        command = MakeDummyCommand( command );
                    }

                    // 0x0a9
                    *command++ = PICA_CMD_DATA_PROCTEX1( Float32ToFloat16( noiseUPhase ),
                                                         Float32ToFix16( noiseUAmplitude ) );
                    *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_TEXTURE3_PROCTEX1, 0x3 );

                    // 0x0aa
                    *command++ = PICA_CMD_DATA_PROCTEX2( Float32ToFloat16( noiseVPhase ),
                                                         Float32ToFix16( noiseVAmplitude ) );
                    // 0x0ab
                    *command++ = PICA_CMD_DATA_PROCTEX3( Float32ToFloat16( noiseUFrequency ), 
                                                         Float32ToFloat16( noiseVFrequency ) );

                    return command;
                }

                /*!
                    @brief プロシージャルテクスチャを参照テーブルのコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeTableCommand( bit32* command ) const;

                /*!
                    @brief プロシージャルテクスチャのレジスタを更新する前に必要な 0x80 へのダミーコマンドを生成します。

                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeDummyCommand( bit32* command )
                {
                    *command++ = 0x0;
                    *command++ = PICA_CMD_HEADER_BURST_BE( PICA_REG_TEXTURE_FUNC, 0x3, 0x0 );

                    *command++ = 0x0;
                    *command++ = 0x0;

                    return command;
                }

            public :
                /*!
                    @brief プロシージャルテクスチャのコマンドを生成するクラスのコンストラクタです。
                */
                ProcedureTexture() : 
                   rgbMap( PICA_DATA_PROCTEX_U_DMP ),
                   alphaMap( PICA_DATA_PROCTEX_U_DMP ),
                   alphaSeparate( false ),
                   clampU( PICA_DATA_PROCTEX_CLAMP_TO_EDGE ), clampV( PICA_DATA_PROCTEX_CLAMP_TO_EDGE ),
                   shiftU( PICA_DATA_PROCTEX_NONE_DMP ), shiftV( PICA_DATA_PROCTEX_NONE_DMP ), 
                   minFilter( PICA_DATA_PROCTEX_LINEAR ),
                   texBias( 0.5f ), texOffset( 0 ), texWidth( 0 ),
                   isEnableNoise( false ),
                   noiseUAmplitude( 0.0f ), noiseVAmplitude( 0.0f ),
                   noiseUFrequency( 0.0f ), noiseVFrequency( 0.0f ),
                   noiseUPhase( 0.0f ), noiseVPhase( 0.0f )
                   {
                   }

            protected:
                /*!
                    @brief 描画コマンドの配列に、参照テーブルのコマンドを設定します。

                    @param[in] command         描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] procTexLookUpTable 参照テーブルの種類です。
                    @param[in] lookUpTable        128個の参照テーブルです。
                    @param[in] diffLookUptable    128個の参照テーブルです。
                    @param[in] lookUpTableIndex   参照テーブルのインデックスです。
                */
                void SetLookUpDualTables_( bit32* command, 
                                           PicaDataProcTexRefTable procTexLookUpTable, 
                                           const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                           const f32 diffLookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                           const u32 lookUpTableIndex = 0 );

                /*!
                    @brief 描画コマンドの配列に、カラー用の参照テーブルと差分テーブルのコマンドを設定します。

                    @param[in] colorShiftNum        色成分の値を左シフトするビット数です。
                    @param[in] lookUpTable          参照テーブルです。
                    @param[in] lookUpTableIndex     参照テーブルのインデックスです。
                */
                void SetColorLookUpDualTables_( const u32 colorShiftNum,
                                                const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                                const u32 lookUpTableIndex = 0 );

                /*!
                    @brief 描画コマンドの配列に、カラーの参照テーブルのコマンドを設定します。

                    @param[in] colorShiftNum       色成分をコマンドに代入するときに、左シフトする回数です。
                    @param[in] lookUpTable         参照テーブルです。
                    @param[in] lookUpTableIndex    参照テーブルのインデックスです。
                */
                void SetColorLookUpTable_( const u32 colorShiftNum,
                                           const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                           const u32 lookUpTableIndex = 0 );

                /*!
                    @brief 描画コマンドの配列に、参照テーブルの差分値のコマンドを設定します。

                    @param[in] colorShiftNum          色成分をコマンドに代入するときに、左シフトする回数です。
                    @param[in] diffLookUpTable        参照テーブルの差分値です。
                    @param[in] diffLookUpTableIndex   参照テーブルのインデックスです。
                */
                void SetColorDiffLookUpTable_( const u32 colorShiftNum,
                                               const f32 diffLookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                               const u32 diffLookUpTableIndex = 0 );

                /*!
                    @brief 256個の参照テーブルの差分値を求めます。

                    @param[in]  lookUpTable        256個の参照テーブルです。
                    @param[out] diffLookUpTable    256個の参照テーブルの差分値です。
                */
                void GetColorDiffLookUpTable( const f32 lookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ],
                                              f32 diffLookUpTable[ PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM ] );

                /*!
                    @brief 128個の参照テーブルの差分値を求めます。

                    @param[in]  lookUpTable        128個の参照テーブルです。
                    @param[out] diffLookUpTable    128個の参照テーブルの差分値です。
                    @param[in]  lastDiffValue      参照データの最後の差分値です。
                */
                void GetDiffLookUpTable( const f32 lookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                         f32 diffLookUpTable[ PROCTEX_LOOKUP_TABLE_ELEMENT_NUM ],
                                         const f32 lastDiffValue = 0.0f );

            protected :
                //! 参照テーブルを送るために追加するコマンドのサイズです。
                static const u32  PROCTEX_LOOKUP_TABLE_OPTION_CMD_NUM      = 4;

                //! 参照テーブルを送るコマンドを保存する配列サイズです。
                static const u32 PROCTEX_LOOKUP_TABLE_CMD_NUM              = PROCTEX_LOOKUP_TABLE_ELEMENT_NUM  + PROCTEX_LOOKUP_TABLE_OPTION_CMD_NUM;

                //! カラー用の参照テーブルを送るために追加するコマンドのサイズです。
                static const u32 PROCTEX_COLOR_LOOKUP_TABLE_OPTION_CMD_NUM = 6;

                //! カラー用の参照テーブルを送るコマンドを保存する配列サイズです。
                static const u32 PROCTEX_COLOR_LOOKUP_TABLE_CMD_NUM        = PROCTEX_COLOR_LOOKUP_TABLE_ELEMENT_NUM + PROCTEX_COLOR_LOOKUP_TABLE_OPTION_CMD_NUM;

                /*!
                    @brief プロシージャルテクスチャの参照テーブルに関連した定数です。
                */
                enum ProcedureTextureColorShift
                {
                    //! 赤成分の値を左シフトするビット数です。
                    PROCTEX_RED_COLOR_SHIFT_NUM   =  0,
                    //! 緑成分の値を左シフトするビット数です。
                    PROCTEX_GREEN_COLOR_SHIFT_NUM =  8,
                    //! 青成分の値を左シフトするビット数です。
                    PROCTEX_BLUE_COLOR_SHIFT_NUM  = 16,
                    //! アルファ成分の値を左シフトするビット数です。
                    PROCTEX_ALPHA_COLOR_SHIFT_NUM = 24
                };

                /*!
                    @brief ノイズの連続性関数の参照テーブルを描画コマンドの形に変換して保持します。
                           型は u32[] です。
                */
                u32 m_NoiseLookUpTableCommand[ PROCTEX_LOOKUP_TABLE_CMD_NUM ];

                /*!
                    @brief RGB のマッピング関数の参照テーブルを描画コマンドの形に変換して保持します。
                           型は u32[] です。
                */
                u32 m_RgbMapLookUpTableCommand[ PROCTEX_LOOKUP_TABLE_CMD_NUM ];

                /*!
                    @brief アルファのマッピング関数の参照テーブルを描画コマンドの形に変換して保持します。
                           型は u32[] です。
                */
                u32 m_AlphaMapLookUpTableCommand[ PROCTEX_LOOKUP_TABLE_CMD_NUM ];

                /*!
                    @brief カラー参照テーブルを描画コマンドの形に変換して保持します。
                           型は u32[] です。
                */
                u32 m_ColorLookUpTableCommand[ PROCTEX_COLOR_LOOKUP_TABLE_CMD_NUM ];

                /*!
                    @brief カラー参照テーブルの差分値を描画コマンドの形に変換して保持します。
                           型は u32[] です。
                */
                u32 m_ColorDiffLookUpTableCommand[ PROCTEX_COLOR_LOOKUP_TABLE_CMD_NUM ];
            };

        } // namespace CTR
    } // namespace gr
} //namespace nn

#endif // NN_GR_CTR_GR_PROCEDURE_TEXTURE_H_
