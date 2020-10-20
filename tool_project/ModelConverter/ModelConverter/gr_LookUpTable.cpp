/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_LookUpTable.cpp

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#include "gr_LookUpTable.h"

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            void LookUpTable::SetTable( const f32 table[ LOOKUP_TABLE_ELEMENT_NUM ],
                                        const f32 lastDiffValue,
                                        const bool isInputAbs )
            {
                // 入力値が [0.0, 1.0] の場合
                if ( isInputAbs )
                {
                    SetAbsTable( table, lastDiffValue );
                }
                // 入力値が [-1.0, 1.0] の場合
                else
                {
                    SetNotAbsTable( table, lastDiffValue );
                }
            }

            void LookUpTable::SetAbsTable( const f32 table[ LOOKUP_TABLE_ELEMENT_NUM ],
                                           const f32 lastDiffValue )
            {
                bit32* command = m_TableCommand;
                
                // 入力値が [0.0, 1.0] の場合

                int i = 0;
                *command++ = Float32ToUnsignedFix12( table[ i ] ) | 
                             Float32ToFix12( table[ i + 1 ] - table[ i ] ) << 12;
                *command++ = PICA_CMD_HEADER_BURST( PICA_REG_FRAG_LIGHT_LUT_DATA0, 256 );

                for ( ++i; i < 0xff; ++i )
                {
                    *command++ = Float32ToUnsignedFix12( table[ i ] ) | 
                                 Float32ToFix12( table[ i + 1 ] - table[ i ] ) << 12;
                }

                // 最後の差分値
                *command++ = Float32ToUnsignedFix12( table[ i ] ) |
                             Float32ToFix12( lastDiffValue ) << 12;
                *command++ = 0;
            }

            void LookUpTable::SetNotAbsTable( const f32 table[ LOOKUP_TABLE_ELEMENT_NUM ],
                                              const f32 lastDiffValue )
            {
                bit32* command = m_TableCommand;

                // 入力値が [0.0, 1.0] の場合

                const u32 LOOKUP_TABLE_HALF_ELEMENT_NUM = LOOKUP_TABLE_ELEMENT_NUM / 2;

                u32 i      = LOOKUP_TABLE_HALF_ELEMENT_NUM;
                
                *command++ = Float32ToUnsignedFix12( table[ i ] ) | 
                             Float32ToFix12( table[ i + 1 ] - table[ i ] ) << 12;
                *command++ = PICA_CMD_HEADER_BURST( PICA_REG_FRAG_LIGHT_LUT_DATA0, 256 );

                for ( ++i; i < 0xff; ++i )
                {
                    *command++ = Float32ToUnsignedFix12( table[ i ] ) | 
                                 Float32ToFix12( table[ i + 1 ] - table[ i ] ) << 12;
                }

                // 最後の差分値
                *command++ = Float32ToUnsignedFix12( table[ i ] ) |
                             Float32ToFix12( lastDiffValue ) << 12;

                // 入力値が [-1.0, 0.0] の場合

                i = 0;

                *command++ = Float32ToUnsignedFix12( table[ i ] ) | 
                             Float32ToFix12( table[ i + 1 ] - table[ i ] ) << 12;

                for ( ++i; i < 0x80; ++i )
                {
                    *command++ = Float32ToUnsignedFix12( table[ i ] ) | 
                                 Float32ToFix12( table[ i + 1 ] - table[ i ] ) << 12;
                }
                *command++ = 0;
            }

        } // namespace CTR
    } // namespace gr
} // namespace nn
