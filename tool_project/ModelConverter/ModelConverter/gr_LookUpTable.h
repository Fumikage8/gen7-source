/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_LookUpTable.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_LOOK_UP_TABLE_H_
#define NN_GR_CTR_GR_LOOK_UP_TABLE_H_

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
                @brief 参照テーブルロードのためのクラスです。
             */
            class LookUpTable
            {
            public :
                //! 参照テーブルは256個の要素を持ちます。
                static const u32 LOOKUP_TABLE_ELEMENT_NUM = 256;

                //! 参照テーブルのコマンドは260個の要素を持ちます。
                static const u32 LOOKUP_TABLE_CMD_NUM     = LOOKUP_TABLE_ELEMENT_NUM + 2;
                
                /*!
                    @brief コンストラクタです。
                */
                LookUpTable()
                {
                }
  
            public :
                /*!
                    @brief 参照テーブルを設定します。
                    
                    @param[in] table         256個の参照データです。
                    @param[in] lastDiffValue 参照データの最後の差分値です。
                    @param[in] isInputAbs    入力値が絶対値 [0.0, 1.0] の場合に true 、入力値が [-1.0, 1.0] のとき false にします。
                 */
                void SetTable( const f32 table[ LOOKUP_TABLE_ELEMENT_NUM ],
                               const f32 lastDiffValue = 0.0f,
                               const bool isInputAbs   = true );

                /*!
                    @brief 入力値が [0.0, 1.0] の参照テーブルを設定します。
                    
                    @param[in] table 256個の参照データです。入力値が 0.0 のものは 0 番目、入力値が 1.0 のものは 255 番目に対応させて下さい。
                    @param[in] lastDiffValue 参照データの最後の差分値です。
                 */
                void SetAbsTable( const f32 table[ LOOKUP_TABLE_ELEMENT_NUM ],
                                  const f32 lastDiffValue = 0.0f );

                /*!
                    @brief 入力値が [-1.0, 1.0] の参照テーブルを設定します。          
                    
                    @param[in] table 256個の参照データです。入力値が -1.0 のものは 0 番目、入力値が 1.0 のものは 255 番目に対応させて下さい。
                    @param[in] lastDiffValue 参照データの最後の差分値です。
                 */
                void SetNotAbsTable( const f32 table[ LOOKUP_TABLE_ELEMENT_NUM ],
                                     const f32 lastDiffValue = 0.0f );


                /*!
                    @brief 参照テーブルをロードするコマンドを生成します。
                    
                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] type    参照テーブルのロード先の指定です。
                    
                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                */

                bit32* MakeCommand( bit32* command, const PicaDataFragLightSampler type ) const
                {
                    // 0x1c5
                    *command++ = PICA_CMD_DATA_FRAG_LIGHT_LUT( 0, type );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAG_LIGHT_LUT );

                    std::memcpy( command, m_TableCommand, sizeof( m_TableCommand ) );

                    return command + LOOKUP_TABLE_CMD_NUM;
                };

                /*!
                    @brief スポットライトの角度減衰で用いられる参照テーブルをロードするコマンドを生成します。
                 
                    @param[in] command  描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] light_id スポットライト参照テーブルのロード先を、光源番号で指定します。
                    
                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeSpotTableCommand( bit32* command, u8 light_id ) const
                {
                    return MakeCommand( command, static_cast< PicaDataFragLightSampler >( PICA_DATA_SAMPLER_SP + light_id  ) );
                }

                /*!
                    @brief ライトの距離減衰で用いられる参照テーブルをロードするコマンドを生成します。
                 
                    @param[in] command  描画コマンドの書き込み先の先頭アドレスです。
                    @param[in] light_id 距離減衰参照テーブルのロード先を、光源番号で指定します。
                    
                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeDistAttnTableCommand( bit32* command, u8 light_id ) const
                {
                    return MakeCommand( command, static_cast< PicaDataFragLightSampler >( PICA_DATA_SAMPLER_DA + light_id  ) );
                }

            protected :
                /*!
                    @brief 参照テーブルを描画コマンドの形に変換して保持します。
                           型は bit32 です。
                           配列の上限値は nn::gr::CTR::LookUpTable::LOOKUP_TABLE_CMD_NUM です。
                */
                bit32 m_TableCommand[ LOOKUP_TABLE_CMD_NUM ];
            };

        } // namespace CTR
    } // namespace gr
} // namespace nn

#endif // NN_GR_CTR_GR_LOOK_UP_TABLE_H_
