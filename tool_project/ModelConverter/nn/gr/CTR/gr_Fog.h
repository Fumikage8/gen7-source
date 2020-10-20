/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Fog.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_FOG_H_
#define NN_GR_CTR_GR_FOG_H_

#include <nn/gr/CTR/gr_Utility.h>

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            /*!
                @brief フォグ関連のコマンドを生成するのためのクラスです。
             */
            class Fog
            {
            public :
                //! フォグ参照テーブルは128個の要素を持ちます。
                static const u32 FOG_LOOKUP_TABLE_ELEMENT_NUM = 128;

            public :
                /*!
                    @brief フォグを有効にする場合はtrueを設定します。
                           型は bool です。
                */
                bool isEnable;

                /*!
                    @brief Zフリップモードの有効化/無効化の設定です。
                           型は bool です。
                */
                bool isEnableZFlip;

                /*!
                    @brief フォグカラーの赤成分です。
                           型は u8 です。
                */
                u8 colorR;

                /*!
                    @brief フォグカラーの緑成分です。
                           型は u8 です。
                */
                u8 colorG;

                /*!
                    @brief フォグカラーの青成分です。
                           型は u8 です。
                */
                u8 colorB;
                NN_PADDING3;
                
            public :
                /*!
                    @brief フォグ参照テーブルを設定します。
                    
                    @param[in] table 128個の参照データです。
                    @param[in] lastDiffValue 参照データの最後の差分値です。
                */
                void SetTable( const f32 table[ FOG_LOOKUP_TABLE_ELEMENT_NUM ],
                               const f32 lastDiffValue = 0.0f );

                /*!
                    @brief フォグを設定するコマンドを生成します。
                    
                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeAllCommand( bit32* command ) const
                {
                    // 設定用コマンド
                    command = MakeConfigCommand( command );

                    // フォグの参照テーブルコマンド
                    if ( isEnable )
                    {
                        command = MakeTableCommand( command );
                    }
                    return command;
                }


                /*!
                    @brief フォグを無効化する最低限のコマンドを生成します。
                    
                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                static bit32* MakeDisableCommand( bit32* command )
                {
                    // 0x0e0
                    *command++ = PICA_DATA_FOG_FALSE;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_GAS_FOG_MODE, 0x1 );
   
                    return command;
                }

                /*!
                    @brief フォグを設定するコマンドを生成します。
                    
                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。

                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeConfigCommand( bit32* command ) const
                {
                    // 0x0e0
                    *command++ = ( isEnable ? PICA_DATA_FOG : PICA_DATA_FOG_FALSE ) | ( ( isEnableZFlip ? 1 : 0 ) << 16 );
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_GAS_FOG_MODE, 0x5 );

                    // 0x0e1
                    *command++ = colorR | colorG << 8 | colorB << 16;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FOG_COLOR );

                    return command;
                }

                /*!
                    @brief フォグを設定するコマンドを生成します。
                    
                    @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
                    
                    @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
                 */
                bit32* MakeTableCommand( bit32* command ) const
                {
                    std::memcpy( command, m_TableCommand, sizeof( m_TableCommand ) );
                    return command + ( FOG_LOOKUP_TABLE_ELEMENT_NUM + 4 );
                }

            public :
                /*!
                    @brief フォグを設定するコマンドを生成します。
                */
                Fog() : 
                isEnable( false ),
                isEnableZFlip( false ),
                colorR( 0 ),
                colorG( 0 ),
                colorB( 0 )
                {
                    for ( u32 index = 0; index < (  FOG_LOOKUP_TABLE_ELEMENT_NUM + 4 ); index++ )
                    {
                        m_TableCommand[ index ] = 0;
                    }
                }
                
            protected :
                /*!
                    @brief 参照テーブルを描画コマンドの形に変換して保持します。
                           型は u32[] です。
                */
                u32 m_TableCommand[ FOG_LOOKUP_TABLE_ELEMENT_NUM + 4 ];
            };

        } // namespace CTR
    } // namespace gr
} //namespace nn

#endif // NN_GR_CTR_GR_FOG_H_
