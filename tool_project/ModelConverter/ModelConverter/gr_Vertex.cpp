/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Vertex.cpp

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#include "gr_Vertex.h"

namespace nn
{
    namespace gr
    {
        namespace CTR
        {
           
            const uptr scBaseAddr = 402653184 >> 3;//402653184 = nngxGetPhysicalAddr( nngxGetVramStartAddr( nn::gx::MEM_VRAMA ) );

            //------------------------------------------------------------------------

            bit32* Vertex::MakeDrawCommand( bit32* command, const IndexStream& index_stream ) const
            {
                bit32 fmt  = index_stream.isUnsignedByte ? 0x00000000 : 0x80000000;
                uptr addr = index_stream.physicalAddr - scBaseAddr * 8;

                // 頂点情報をリセットします      
                *command++ = 0x1;
                // 0x25F
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_START_DRAW_FUNC1 );

                // offset
                *command++ = fmt | addr;
                // 0x227
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_INDEX_ARRAY_ADDR_OFFSET );

                // 描画頂点数
                *command++ = index_stream.drawVtxNum;
                // 0x228
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_DRAW_VERTEX_NUM );
                *command++ = 0x00000000;
                // 0x245
                *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_START_DRAW_FUNC0, 0x1 );
                
                // 描画キック
                *command++ = 0x00000001;
                // 0x22f
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_START_DRAW_ELEMENT );
                
                *command++ = 0x00000001;
                // 0x245
                *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_START_DRAW_FUNC0, 0x1 );

                // 頂点キャッシュのフラッシュ
                *command++ = 0x00000001;
                // 0x231
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VERTEX_FUNC );

                // ダミーコマンド を 2回送ります
                for ( u32 index = 0; index < 2; index++ )
                {
                    *command++ = 0x0;
                    // 0x25E
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE2, 0x8 );
                }
                
                return command;
            }

            //------------------------------------------------------------------------

            void Vertex::EnableAttrAsArray( const BindSymbolVSInput& symbol,
                                            const uptr physical_addr,
                                            const PicaDataVertexAttrType type )
            {
                const bit32 bind_reg = symbol.start;
                const u32 byte     = PicaDataVertexAttrTypeToByteSize( type );
                NN_GR_ASSERT( bind_reg < VERTEX_ATTRIBUTE_MAX );

                // ほかで設定されている可能性があるので無効化しておく
                DisableAttr_( bind_reg );

                // 空いているストリームを探す
                LoadArray* array = NULL;
                for ( int i = 0; i < VERTEX_ATTRIBUTE_MAX; ++i )
                {
                    if ( m_LoadArray[ i ].IsEnable() == false )
                    {
                        array = &m_LoadArray[ i ];
                        break;
                    }
                }
                NN_GR_ASSERT( array != NULL );

                m_IsEnableReg[ bind_reg ] = true;
                array->physicalAddr       = physical_addr;
                array->type[ 0 ]          = type;
                array->bind[ 0 ]          = bind_reg;
                array->byte[ 0 ]          = byte;

                for ( int i = 1; i < VERTEX_ATTRIBUTE_MAX; ++i )
                {
                    array->bind[ i ] = -1;
                    array->byte[ i ] = 0;
                }

                // キャッシュの無効化
                m_CmdCacheVertexNum = 0;
            }

            //------------------------------------------------------------------------

            void Vertex::EnableAttrAsConst( const BindSymbolVSInput& symbol,
                                            const u8  dimension,
                                            const f32 param[] )
            {
                const bit32 bind_reg = symbol.start;

                NN_GR_ASSERT( bind_reg < VERTEX_ATTRIBUTE_MAX );

                // ほかで設定されている可能性があるので無効化しておく
                DisableAttr_( bind_reg );

                m_IsEnableReg[ bind_reg ] = true;
                m_AttrConst[ bind_reg ].dimension = dimension;

                f32 defaultParam[ 4 ] = { 0.f, 0.f, 0.f, 1.f };
                for ( int i = 0; i < 4; ++i )
                {
                    m_AttrConst[ bind_reg ].param[ i ] = i < dimension ? param[ i ] : defaultParam[ i ];
                }

                // キャッシュの無効化
                m_CmdCacheVertexNum = 0;
            }

            //------------------------------------------------------------------------

            void Vertex::EnableInterleavedArray( const nn::gr::CTR::Vertex::InterleaveInfo& interleave_info, const uptr physical_addr )
            {
                // 空いているストリームを探す
                LoadArray* array = NULL;
                for ( int i = 0; i < VERTEX_ATTRIBUTE_MAX; ++i )
                {
                    if ( m_LoadArray[ i ].IsEnable() == false )
                    {
                        array = &m_LoadArray[ i ];
                        break;
                    }
                }
                NN_GR_ASSERT( array != NULL );

                for ( int i = 0; i < VERTEX_ATTRIBUTE_MAX; ++i )
                {
                    if ( interleave_info.symbol[ i ] == NULL )
                    {
                        array->bind[ i ] = -1;
                    }
                    else
                    {
                        array->bind[ i ] = interleave_info.symbol[ i ]->start;
                        array->physicalAddr = physical_addr;
                    }

                    array->type[ i ] = interleave_info.dataType[ i ];

                    array->byte[ i ] = i < interleave_info.dataNum ? PicaDataVertexAttrTypeToByteSize( array->type[ i ] ) : 0;
                }
                
                // キャッシュの無効化
                m_CmdCacheVertexNum = 0;
            }

            //------------------------------------------------------------------------

            void Vertex::DisableAttr_( const bit32 bind_reg )
            {    
                NN_GR_ASSERT( bind_reg < VERTEX_ATTRIBUTE_MAX );
                
                // すでに無効化されていたら何もしない
                if ( !m_IsEnableReg[ bind_reg ] ) return;

                // キャッシュの無効化
                m_CmdCacheVertexNum = 0;
                
                if ( m_AttrConst[ bind_reg ].IsEnable() )
                {
                    // 固定頂点属性の無効化
                    m_AttrConst[ bind_reg ].Disable();

                    m_IsEnableReg[ bind_reg ] = false;
                    return;
                }


                // ロードアレイの中を探しながら無効化
                for ( int i = 0; i < VERTEX_ATTRIBUTE_MAX; ++i )
                {
                    if ( ! m_LoadArray[ i ].IsEnable() ) continue;

                    for ( int j = 0; j < VERTEX_ATTRIBUTE_MAX; ++j )
                    {
                        if ( m_LoadArray[ i ].bind[ j ] != bind_reg ) continue;
                        
                        m_LoadArray[ i ].bind[ j ] = -1;                       
                        m_IsEnableReg[ bind_reg ] = false;

                        // 無効化できた場合、ロードアレイ自体が無効化できないか調べる
                        m_LoadArray[ i ].CheckDisable();
                        
                        return;
                    }
                }

                NN_GR_ASSERT( m_IsEnableReg[ bind_reg ] == false );
            }

            //------------------------------------------------------------------------

            void Vertex::LoadArray::CheckDisable()
            {
                // 無効化されているなら、特に何もしない
                if ( !IsEnable() )
                {
                    return;
                }

                // 有効化されている場合、中身がすべて無効なら無効化する
                for ( u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; ++index )
                {
                    if ( bind[ index ] != -1 )
                    {
                        return;
                    }
                }

                physicalAddr = NULL;

                for ( u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; ++index )
                {
                    byte[ index ] = 0;
                }
            }

            //------------------------------------------------------------------------

            void Vertex::LoadArray::DisableAll()
            {
                physicalAddr = NULL;

                for ( u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; index++ )
                {
                    // 0x0 で初期化しておきます
                    type[ index ] = PICA_DATA_SIZE_1_BYTE;

                    byte[ index ] = 0;

                    bind[ index ] = -1;
                }
            }

            //------------------------------------------------------------------------

            bit32* Vertex::MakeEnableAttrCommand_( bit32* command ) const
            {
            // #define NN_GR_VERTEX_DUMP
            #if defined( NN_GR_VERTEX_DUMP ) // デバッグ用
                bit32 * start = command;
            #endif

                // 頂点シェーダーの入力レジスタ数を後で設定
                bit32* reg0x2b9 = command++; // = vtx_attr_num | 0xa0000000;
                *command++ = 0x000b02b9;

                bit32* reg0x242 = command++; // = vtx_attr_num;
                *command++ = 0x00010242;

                // bind_reg_mapの設定
                bit32* bind_reg_command = command;

                // 0x2bb
                *command++ = 0;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_ATTR_IN_REG_MAP0 );

                // 0x2bc
                *command++ = 0;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_ATTR_IN_REG_MAP1 );

                // 0x201
                bit32* type = &command[ 2 ];
                
                // 0x202[27:16]
                bit32& fixed_attr_mask = command[ 3 ];
                
                // 0x200
                *command++            = scBaseAddr;          
                bit32* headerBaseAddr = command++; // = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, 3 + 3 * m_AttrArrayNum );
                
                // 0x201
                *command++            = 0;
                
                // 0x202
                bit32* reg0x202       = command++; // vtx_attr_num << 28;

                int input_index = 0;
                int array_num   = 0;
                for ( int i = 0; i < VERTEX_ATTRIBUTE_MAX; ++i )
                {
                    const LoadArray& vtx_array = m_LoadArray[ i ];

                    if ( !vtx_array.IsEnable() ) continue;

                    ++array_num;
                    int total_byte = 0;
                    int elem_num   = 0;
                    int elem[ 2 ]  = { 0, 0 };
                    int maxAlign   = 1;

                    for ( int j = 0; j < VERTEX_ATTRIBUTE_MAX; ++j )
                    {
                        // 各要素の型の並びによって、自動挿入されるパディングを考慮
                        switch (vtx_array.type[ j ])
                        {
                        case PICA_DATA_SIZE_1_SHORT:
                        case PICA_DATA_SIZE_2_SHORT:
                        case PICA_DATA_SIZE_3_SHORT:
                        case PICA_DATA_SIZE_4_SHORT:
                            total_byte = ((total_byte + 1) & ~0x1);
                            if (maxAlign < 2) { maxAlign = 2; }
                        break;
                        
                        case PICA_DATA_SIZE_1_FLOAT:
                        case PICA_DATA_SIZE_2_FLOAT:
                        case PICA_DATA_SIZE_3_FLOAT:
                        case PICA_DATA_SIZE_4_FLOAT:
                            total_byte = ((total_byte + 3) & ~0x3);
                            if (maxAlign < 4) { maxAlign = 4; }
                        break;
                        
                        default:
                        break;
                        }

                        if ( vtx_array.byte[ j ] == 0 ) break;
                        total_byte += vtx_array.byte[ j ];

                        // Bind されているとき
                        if ( vtx_array.bind[ j ] >= 0 )
                        {                            
                            // 0x204 + ( 3 * N ) or 0x205 + ( 3 * N )
                            elem[ j / 8 ] |= input_index << ( 4 * ( j % 8 ) );

                            // 0x201 or 0x202
                            type[ input_index / 8 ] |= vtx_array.type[ j ] << ( 4 * ( input_index % 8 ) );
                            
                            // 0x2bb or 0x2bc に入力レジスタのインデックスを設定
                            bind_reg_command[ ( input_index >> 3 ) << 1 ] &= ~( 0xf << ( 4 * ( input_index % 8 ) ) );
                            bind_reg_command[ ( input_index >> 3 ) << 1 ] |= vtx_array.bind[ j ] << ( 4 * ( input_index % 8 ) );                           

                            ++input_index;
                            
                            #if defined( NN_GR_VERTEX_DUMP ) // デバッグ用
                            NN_LOG( "+ 0x%08x 0x%08x 0x%08x\n", vtx_array.byte[ j ], elem[0], elem[1] );
                            #endif
                        }
                        // Padding のとき
                        else
                        {
                            // 0x204 + ( 3 * N ) or 0x205 + ( 3 * N )
                            elem[ j / 8 ] |= ( ( vtx_array.byte[ j ] >> 2 ) + 0xb ) << ( 4 * ( j % 8 ) );
                            
                            #if defined( NN_GR_VERTEX_DUMP ) // デバッグ用
                            NN_LOG( "- 0x%08x 0x%08x 0x%08x\n",  vtx_array.byte[ j ] >> 2, elem[0], elem[1] );
                            #endif
                        }

                        ++elem_num;
                    }

                    // 0x203 + ( 3 * N )
                    //*command++ = vtx_array.physicalAddr  - scBaseAddr * 8;
										*command++ = 0x99999999;

                    // 0x204 + ( 3 * N )
                    *command++ = elem[ 0 ];

                    // 0x205 + ( 3 * N )
                    // 1 頂点ごとのストライドは、頂点データ構造体に含まれる頂点属性のうち最も大きい型のサイズの倍数であり
                    // 自動挿入されるパディングを考慮に入れなければならない
                    *command++ = elem[ 1 ] | ((total_byte + (maxAlign - 1)) & ~(maxAlign - 1)) << 16 | elem_num << 28;
                }

                // 0x200
                *headerBaseAddr = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, 3 + 3 * array_num );

                if ( array_num % 2 ) *command++ = 0;

                // 固定頂点のセットアップ
                for ( int bind_reg = VERTEX_ATTRIBUTE_MAX-1; bind_reg >= 0; --bind_reg )
                {
                    const AttrConst& vtxConst = m_AttrConst[ bind_reg ];

                    if ( !vtxConst.IsEnable() ) continue;

                    // 頂点シェーダへ入力するインデックス
                    *command++ = input_index;                    
                    // 0x232
                    *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_VS_FIXED_ATTR , 4 );
                    // 固定頂点属性を 24bit に変換した 3 つのデータ, 0x233, 0x234, 0x235
                    *command++ = ( ( Float32ToFloat24( vtxConst.param[3] ) <<  8 ) & 0xffffff00 ) | ( ( Float32ToFloat24( vtxConst.param[2] ) >> 16 ) & 0x000000ff );
                    *command++ = ( ( Float32ToFloat24( vtxConst.param[2] ) << 16 ) & 0xffff0000 ) | ( ( Float32ToFloat24( vtxConst.param[1] ) >>  8 ) & 0x0000ffff );
                    *command++ = ( ( Float32ToFloat24( vtxConst.param[1] ) << 24 ) & 0xff000000 ) | ( ( Float32ToFloat24( vtxConst.param[0] ) >>  0 ) & 0x00ffffff );
                    *command++ = 0; // padding

                    // 0x2bb or 0x2bc に入力レジスタのインデックスを設定
                    bind_reg_command[ ( input_index / 8 ) * 2 ] &= ~( 0xf << ( 4 * ( input_index % 8 ) ) );
                    bind_reg_command[ ( input_index / 8 ) * 2 ] |= bind_reg << ( 4 * ( input_index % 8 ) );

                    // 0x202[27:16] 固定頂点属性マスク
                    fixed_attr_mask |= ( 1 << input_index + 16 );

                    ++input_index;
                }

                // 頂点属性の入力数の設定
                // 0x2b9
                *reg0x2b9  = (input_index - 1) | 0xa0000000;
                // 0x242
                *reg0x242  = input_index - 1;
                // 0x202
                *reg0x202 |= (input_index - 1) << 28;
                
            #if defined( NN_GR_VERTEX_DUMP ) // デバッグ用
                static int a = 0;
                if ( ++a == 1 )
                {
                    for ( bit32* i = start; i != command; i +=2 )
                    {
                        NN_LOG( "0x%08x 0x%08x\n", *i, *(i+1) );
                    }
                }
            #endif

                return command;
            }

        }  // namespace CTR
    }  // namespace gr
} // namespace nn
