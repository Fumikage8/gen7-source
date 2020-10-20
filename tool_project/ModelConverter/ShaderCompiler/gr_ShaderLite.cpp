/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_ShaderLite.cpp

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#include "gr_ShaderLite.h"

//#include "gd_System.h"

#define NN_GR_ASSERT //
#define NN_NULL_ASSERT //
#define NN_UNUSED_VAR(var)  ((void)&var);

namespace nn
{
    namespace gr
    {
        namespace CTR
        {
            ShaderLite::ShaderLite( void ) :     
            m_VtxShaderIndex( 0 ),
            m_GeoShaderIndex( - 1 ),
            m_ExeImageInfoNum( 0 ),
            m_InstructionCount( 0 ),
            m_SwizzleCount( 0 ),
            m_DrawMode( PICA_DATA_DRAW_TRIANGLES ),
            m_VtxShaderBoolMapUniform( 0 ),
            m_GeoShaderBoolMapUniform( 0 ),
            m_GeometryShaderNum( 0 )
            {
            }

            u32 ShaderLite::GetRequiredBufferSize( const void* shader_binary )
            {
                const bit32* binary = reinterpret_cast< const bit32* >( shader_binary );
                NN_GR_ASSERT( binary != NULL );

                NN_GR_ASSERT( *binary == 0x424C5644 ); // DVLB
                ++binary;

                NN_GR_ASSERT( *binary < EXE_IMAGE_MAX );

                // シェーダバイナリ内の実行イメージの数です。
                m_ExeImageInfoNum = *binary;
                ++binary;

                // シェーダバイナリ内の実行イメージへのポインタを保存します
                for ( s32 i = 0; i < m_ExeImageInfoNum; ++i )
                {
                    m_ExeImageInfo[ i ] = 
                        reinterpret_cast< const ExeImageInfo* >( (u8*)shader_binary + *binary );
                    NN_GR_ASSERT( m_ExeImageInfo[ i ]->signature == 0x454c5644 ); // DVLP
                    ++binary;
                }
                
                NN_GR_ASSERT( *binary == 0x504C5644 ); // DVLP
                ++binary;
                ++binary;
                ++binary;       // instruction
                ++binary;       // instruction count
                ++binary;       // swizzle

                m_SwizzleCount = *binary;
                NN_GR_ASSERT( m_SwizzleCount <= SWIZZLE_PATTERN_MAX );

                return CalculateBufferSize_();
            }

            void ShaderLite::SetupBinary( const void* shader_binary, const s32 vtx_shader_index, const s32 geo_shader_index, bit32* buffer )
            {
                const bit32* binary = reinterpret_cast< const bit32* >( shader_binary );
                NN_GR_ASSERT( binary != NULL );

                NN_GR_ASSERT( *binary == 0x424C5644 ); // DVLB
                ++binary;

                NN_GR_ASSERT( *binary < EXE_IMAGE_MAX );

                // シェーダバイナリ内の実行イメージの数です。
                m_ExeImageInfoNum = *binary;
                ++binary;

                m_VtxShaderBoolMapUniform = 0;
                m_GeoShaderBoolMapUniform = 0;

                // シェーダバイナリ内の実行イメージへのポインタを保存します
                for ( s32 i = 0; i < m_ExeImageInfoNum; ++i )
                {
                    m_ExeImageInfo[ i ] = 
                        reinterpret_cast< const ExeImageInfo* >( (u8*)shader_binary + *binary );
                    NN_GR_ASSERT( m_ExeImageInfo[ i ]->signature == 0x454c5644 ); // DVLE
                    ++binary;
                }
                
                const bit32* package_info = binary;
                NN_GR_ASSERT( *binary == 0x504C5644 ); // DVLP
                ++binary;
                ++binary;

                m_Instruction = reinterpret_cast< const bit32* >( (u8*)package_info + *binary );
                ++binary;

                m_InstructionCount = *binary;
                ++binary;

                const bit32* swizzle = reinterpret_cast< const bit32* >( (u8*)package_info + *binary );
                ++binary;

                m_SwizzleCount = *binary;
                NN_GR_ASSERT( m_SwizzleCount <= SWIZZLE_PATTERN_MAX );
                ++binary;

                // バッファの割り当て
                NN_NULL_ASSERT(buffer);
                AssignBuffer_( buffer );

                for ( u32 i = 0; i < m_SwizzleCount; i++ )
                {
                    m_Swizzle[ i ] = swizzle[ i * 2 ] ;
                }

                PicaDataDrawModeu8 drawMode     = m_DrawMode;

                MakeShaderConstCommandCache_();
                MakeRgCmdOffsetCache_();

                SetShaderIndex( vtx_shader_index, geo_shader_index );
                MakeShaderOutAttrCommandCache_();
                
                if ( ! IsEnableGeoShader() )
                {
                    m_DrawMode                = drawMode;
                }
            }

            //------------------------------------------------------------------------
            
            void ShaderLite::SetShaderIndex( const s32 vtx_shader_index, const s32 geo_shader_index )
            {
                // 頂点シェーダとジオメトリシェーダのテスト        
                CheckVtxShaderIndex_( vtx_shader_index );
                // ジオメトリシェーダのテスト
                CheckGeoShaderIndex_( geo_shader_index );
                
                m_VtxShaderIndex     = vtx_shader_index;
                m_GeoShaderIndex     = geo_shader_index;
                m_CombShaderIndex    = m_CmdCacheOutAttrIndexArray[m_VtxShaderIndex + 1] + m_CmdCacheOutAttrIndexArray[m_GeoShaderIndex + 1];

                if ( IsEnableGeoShader() )
                {
                    m_DrawMode = PICA_DATA_DRAW_GEOMETRY_PRIMITIVE;
                }

            }

            //------------------------------------------------------------------------

            void ShaderLite::MakeShaderConstCommandCache_(void)
            {
                u32 offset = 0;
                for ( s32 shader_index = 0; shader_index < m_ExeImageInfoNum; shader_index++ )
                {
                    // 定数レジスタのコマンドキャッシュを生成します。
                    MakeConstRgCommand_( &m_CmdCacheConstArray[ offset ], shader_index );
                    offset += m_CmdCacheConstNumArray[ shader_index ];

                    NN_GR_ASSERT( m_CmdCacheConstNumArray[ shader_index ] <= CONST_REG_COMMAND_MAX );
                }
            }

            //------------------------------------------------------------------------

            void ShaderLite::MakeShaderOutAttrCommandCache_( void )
            {
                u8  shader_idx = 0;
                s32 tmp        = m_GeoShaderIndex;
                u32 offset     = 0;
                for ( u32 vert_idx = 0; vert_idx < m_ExeImageInfoNum; vert_idx++ )
                {
                    if( !m_ExeImageInfo[vert_idx]->isGeoShader )
                    {
                        m_GeoShaderIndex = -1;
                        // 出力属性のコマンドキャッシュを生成します。(ジオメトリシェーダなし)
                        MakeOutAttrCommand_( &m_CmdCacheOutAttrArray[ offset ],
                                             vert_idx,
                                             m_GeoShaderIndex );
                        offset += m_CmdCacheOutAttrNumArray[ shader_idx ];
                        shader_idx++;

                        for ( u32 geo_idx = 0; geo_idx < m_ExeImageInfoNum; geo_idx++ )
                        {
                            if( m_ExeImageInfo[geo_idx]->isGeoShader )
                            {
                                m_GeoShaderIndex = geo_idx;
                                // 出力属性のコマンドキャッシュを生成します。(ジオメトリシェーダあり)
                                MakeOutAttrCommand_( &m_CmdCacheOutAttrArray[ offset ],
                                                     vert_idx,
                                                     m_GeoShaderIndex );
                                offset += m_CmdCacheOutAttrNumArray[ shader_idx ];
                                shader_idx++;
                            }
                        }
                    }
                }
                m_GeoShaderIndex = tmp;
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakeFullCommand( bit32* command ) const
            {
                // ジオメトリシェーダー使用設定コマンドなどの生成
                {
                    command = MakePrepareCommand( command );
                }

                // ジオメトリシェーダーのコマンドを生成
                if ( IsEnableGeoShader() )
                {                     
                    command = MakeGeoProgramCommand( command );
                    command = MakeGeoSwizzleCommand( command );
                    command = MakeGeoConstRgCommand( command );
                    command = MakeGeoBoolMapCommand( command );
                }            

                // 頂点シェーダーのコマンドを生成
                {
                    command = MakeVtxProgramCommand( command );
                    command = MakeVtxSwizzleCommand( command );
                    command = MakeVtxConstRgCommand( command );
                    command = MakeVtxBoolMapCommand( command );
                }

                // 出力属性（outmap）関連のコマンドを生成
                {
                    command = MakeOutAttrCommand( command );
                }
                
                return command;
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakeDisableCommand( bit32* command )
            {
                const bool isEnableGeometryShader = false;
                const PicaDataDrawModeu8 drawMode   = PICA_DATA_DRAW_TRIANGLES;

                command = MakeShaderModeCommand_( command,
                                                  isEnableGeometryShader,
                                                  drawMode );

                return command;
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakeShaderCommand( bit32* command, const bool isMakePrepareCommand ) const
            {
                // ジオメトリシェーダー使用設定コマンドなどの生成
                if ( isMakePrepareCommand )
                {
                    command = MakePrepareCommand( command );
                }

                // ジオメトリシェーダーの定数レジスタのコマンドを生成
                if ( IsEnableGeoShader() )
                {
                    command = MakeGeoConstRgCommand( command );
                    command = MakeGeoBoolMapCommand( command );
                }

                // 頂点シェーダーの定数レジスタのコマンドを生成
                {
                    command = MakeVtxConstRgCommand( command );
                    command = MakeVtxBoolMapCommand( command );
                }

                // 出力属性（outmap）関連のコマンドを生成
                {
                    command = MakeOutAttrCommand( command );
                }

                return command;
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakePrepareCommand( bit32* command ) const
            {
                bool isEnableGeoShader    = IsEnableGeoShader();
                PicaDataDrawModeu8 drawMode = m_DrawMode;

                command = 
                    MakeShaderModeCommand_( command,
                                            isEnableGeoShader,
                                            drawMode );

                return command;
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakeVtxProgramCommand( bit32* command ) const
            {
                s32 shader_index   = GetVtxShaderIndex();
                bit32 reg_addr     = PICA_REG_VS_PROG_ADDR;        // 0x2cb
                bit32 reg_load     = PICA_REG_VS_PROG_DATA0;       // 0x2cc
                bit32 reg_end      = PICA_REG_VS_PROG_UPDATE_END;  // 0x2bf

                { // プログラムコードのロードアドレスを設定
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( reg_addr );
                }

                { // プログラムコードのロード
                    NN_GR_ASSERT( 0 <= shader_index && shader_index < m_ExeImageInfoNum );
                    
                    const ExeImageInfo* exe_info = m_ExeImageInfo[ shader_index ];

                    NN_UNUSED_VAR( exe_info );

                    u32 instructionCount = m_InstructionCount;
                    if ( instructionCount > 512 )
                    {
                        instructionCount = 512;
                    }

                    command = MakeLoadCommand_( command, reg_load,
                                                m_Instruction,
                                                m_InstructionCount < 512 ? m_InstructionCount : 512 );
                }

                { // プログラム更新の完了通知
                    *command++ = 1;
                    *command++ = PICA_CMD_HEADER_SINGLE( reg_end );
                }

                return command;
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakeGeoProgramCommand( bit32* command ) const
            {
                s32 shader_index   = GetGeoShaderIndex();
                bit32 reg_addr     = PICA_REG_GS_PROG_ADDR;        // 0x29b
                bit32 reg_load     = PICA_REG_GS_PROG_DATA0;       // 0x29c
                bit32 reg_end      = PICA_REG_GS_PROG_UPDATE_END;  // 0x28f

                { // プログラムコードのロードアドレスを設定
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( reg_addr );
                }

                { // プログラムコードのロード
                    NN_GR_ASSERT( ( 0 <= shader_index ) && 
                                  ( shader_index < m_ExeImageInfoNum ) );
                    
                    const ExeImageInfo* exe_info = m_ExeImageInfo[ shader_index ];

                    NN_UNUSED_VAR( exe_info );

                    command = MakeLoadCommand_( command, reg_load,
                                                m_Instruction,
                                                m_InstructionCount );
                }

                { // プログラム更新の完了通知
                    *command++ = 1;
                    *command++ = PICA_CMD_HEADER_SINGLE( reg_end );
                }

                return command;
            }

            //------------------------------------------------------------------------
            
            bit32* ShaderLite::MakeShaderModeCommand_( bit32* command,
                                                   const bool isEnableGeoShader,
                                                   const PicaDataDrawModeu8 drawMode )
            {
                // 0x25e[9:8] への設定
                { 
                    if ( isEnableGeoShader )
                    {
                        *command++ = PICA_DATA_DRAW_GEOMETRY_PRIMITIVE << 8;
                    }
                    else
                    {
                        *command++ = drawMode << 8;
                    }
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_VS_OUT_REG_NUM3, 2 );
                }

                // 0x251 へのダミーコマンド
                {
                    command = MakeDummyCommand_( command, PICA_REG_VS_OUT_REG_NUM2, DUMMY_DATA_NUM_251 );
                }

                // 0x200 へのダミーコマンド
                {
                    command = MakeDummyCommand_( command, PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, DUMMY_DATA_NUM_200 );
                }

                // 0x229[1:0] ジオメトリシェーダーの有効、無効を設定
                {
                    *command++ = isEnableGeoShader ? 2 : 0;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE0, 1 );
                }

                // 0x200 へのダミーコマンド
                {
                    command = MakeDummyCommand_( command, PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, DUMMY_DATA_NUM_200 );
                }

                // 0x244 ジオメトリシェーダーの設定を頂点シェーダーと共有するかどうか
                { 
                    *command++ = isEnableGeoShader ? 1 : 0;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_VS_COM_MODE, 1 );
                }

                return command;
            }

            //------------------------------------------------------------------------

            void ShaderLite::CheckVtxShaderIndex_( const s32 vtx_shader_index )
            {
                NN_UNUSED_VAR( vtx_shader_index );

                // 頂点シェーダのインデックスの範囲テスト
                NN_GR_ASSERT( ( 0 <= vtx_shader_index ) && ( vtx_shader_index < GetShaderNum() ) );

                // シェーダバイナリの整合性のテスト
                NN_GR_ASSERT( ! m_ExeImageInfo[ vtx_shader_index ]->isGeoShader );
            }

            //------------------------------------------------------------------------

            void ShaderLite::CheckGeoShaderIndex_( const s32 geo_shader_index )
            {
                NN_UNUSED_VAR( geo_shader_index );

                // ジオメトリシェーダのインデックスの範囲テスト
                NN_GR_ASSERT( m_GeoShaderIndex < GetShaderNum() );

                // シェーダバイナリの整合性のテスト
                if ( geo_shader_index > - 1 )
                {
                    NN_GR_ASSERT( m_ExeImageInfo[ geo_shader_index ]->isGeoShader );
                }
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakeConstRgCommand_( bit32* command,
                                                const s32 shader_index )                                                
            {
                bit32  reg_float     = PICA_REG_VS_FLOAT_ADDR; // 0x2c0
                bit32  reg_integer   = PICA_REG_VS_INT0;       // 0x2b1
                bit32* boolMap       = &m_VtxShaderBoolMapUniform;

                bool is_geometry_shader = m_ExeImageInfo[ shader_index ]->isGeoShader;
                if ( is_geometry_shader )
                {
                    reg_float    = PICA_REG_GS_FLOAT_ADDR; // 0x290
                    reg_integer  = PICA_REG_GS_INT0;       // 0x281
                    boolMap      = &m_GeoShaderBoolMapUniform;
                }

                // プログラム情報
                NN_GR_ASSERT( ( 0 <= shader_index ) && 
                              ( shader_index < m_ExeImageInfoNum ) );
                const ExeImageInfo* exe_info = m_ExeImageInfo[ shader_index ];

                // 定数レジスタ情報
                struct SetupInfo
                {
                    u16 type;
                    u16 index;
                    bit32 value[4];
                };

                const SetupInfo* setupInfo =
                    reinterpret_cast< const SetupInfo* >(
                        reinterpret_cast< const u8* >( exe_info ) + exe_info->setupOffset );

                // 定数レジスタのコマンド生成
                for ( s32 i = 0; i < exe_info->setupCount; ++i )
                {
                    const SetupInfo& info = setupInfo[ i ];
                    const bit32* value = info.value;

                    switch ( info.type )
                    {
                    case 0 : // bool register setup
                        *boolMap |= ( info.value[ 0 ] << info.index ) & ( 1 << info.index );
                        break;

                    case 1 : // integer register setup
                        *command++ = value[ 0 ] | value[ 1 ] <<  8 | value[ 2 ] << 16 | value[ 3 ] << 24;
                        *command++ = PICA_CMD_HEADER_SINGLE( reg_integer + info.index );
                        break;

                    case 2 : // float register setup
                        *command++ = info.index;
                        *command++ = PICA_CMD_HEADER_BURSTSEQ( reg_float, 4 );
                        *command++ = ( value[ 3 ] <<  8 & 0xffffff00 ) | ( value[ 2 ] >> 16 & 0x000000ff );
                        *command++ = ( value[ 2 ] << 16 & 0xffff0000 ) | ( value[ 1 ] >>  8 & 0x0000ffff );
                        *command++ = ( value[ 1 ] << 24 & 0xff000000 ) | ( value[ 0 ] >>  0 & 0x00ffffff );
                        *command++ = PADDING_DATA; // padding
                        break;
                    }
                }

                return command;
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakeOutAttrCommand_( bit32* command,
                                                const s32 vtx_shader_index,
                                                const s32 geo_shader_index )
            {
                s32 shader_index = vtx_shader_index;

                bool is_geometry_shader = false;
                if ( 0 <= GetGeoShaderIndex() )
                {
                    is_geometry_shader = true;

                    shader_index = geo_shader_index;
                }

                NN_GR_ASSERT(  0 <= shader_index && shader_index < m_ExeImageInfoNum );

                // 出力属性の設定
                // ジオメトリシェーダーが有効な場合、
                // 出力属性は、ジオメトリシェーダーについて設定を行います

                const s32 OUT_ATTR_INDEX_MAX     = 7;
                const s32 OUT_ATTR_DIMENTION_MAX = 4;
                const s32 OUT_ATTR_BUFFER_MAX    = 16 * 4;
                const s32 VS_OUT_ATTR_INDEX_MAX  = 16;

                // 出力属性情報
                struct OutmapInfo
                {
                    u16 type;
                    u16 index;
                    u16 mask;
                    u16 reserve;
                };

                u32 outNum  = 0;
                bit32 useTex  = 0;
                bit32 clock   = 0;
                bit32 outMask = 0;
                bit32 attr[ OUT_ATTR_INDEX_MAX ];

                // 出力属性の設定を求めます
                {
                    // プログラム情報
                    const ExeImageInfo* exe_info = m_ExeImageInfo[ shader_index ];

                    // 実際の出力属性情報
                    OutmapInfo outmap_buffer[ OUT_ATTR_BUFFER_MAX ];
                    s32 outMapBufferCount = 0;
                    
                    // マージ
                    if ( is_geometry_shader && exe_info->outputMaps )
                    {
                        bit32 gs_copy_mask = 0;
                        bit32 vs_copy_mask = 0;
                        // GS 出力属性情報
                        const OutmapInfo* outmapInfo =
                            reinterpret_cast< const OutmapInfo* >(
                                reinterpret_cast< const u8* >( exe_info ) + exe_info->outMapOffset );
                        // VS プログラム情報    
                        NN_GR_ASSERT(  0 <= vtx_shader_index && vtx_shader_index < m_ExeImageInfoNum );
                        const ExeImageInfo* vtx_exe_info = m_ExeImageInfo[ vtx_shader_index ];

                        // VS 出力属性情報
                        const OutmapInfo* vtxOutmapInfo =
                            reinterpret_cast< const OutmapInfo* >(
                                reinterpret_cast< const u8* >( vtx_exe_info ) + vtx_exe_info->outMapOffset );
                        
                        // VS と GS の両方で定義されている generic 以外の出力属性をマージ
                        NN_GR_ASSERT( outMapBufferCount < OUT_ATTR_BUFFER_MAX );
                        for( s32 g = 0; g < exe_info->outMapCount; ++g )
                        {
                            // GS の generic 属性( 値 9 ) は除きます
                            if ( ( outmapInfo[ g ].type >= 0 ) &&
                                 ( outmapInfo[ g ].type <  9 ) && 
                                 ( outmapInfo[ g ].type != 7 ) )
                            {
                                for( s32 v = 0; v < vtx_exe_info->outMapCount; ++v )
                                {
                                    // VS の generic 属性( 値 9 ) は除きます
                                    if ( ( vtxOutmapInfo[ v ].type >= 0 ) &&
                                         ( vtxOutmapInfo[ g ].type <  9 ) && 
                                         ( vtxOutmapInfo[ g ].type != 7 ) )
                                    {
                                        if ( outmapInfo[ g ].type == vtxOutmapInfo[ v ].type )
                                        {
                                            NN_GR_ASSERT( outMapBufferCount < OUT_ATTR_INDEX_MAX );
                                            outmap_buffer[ outMapBufferCount ].type = outmapInfo[ g ].type;
                                            outmap_buffer[ outMapBufferCount ].index = outMapBufferCount;
                                            outmap_buffer[ outMapBufferCount ].mask = outmapInfo[ g ].mask;
                                            gs_copy_mask |= 1 << g;
                                            vs_copy_mask |= 1 << v;
                                            ++outMapBufferCount;
                                        }
                                    }
                                }
                            }
                        }
                        
                        // 次にGSのみで定義されている出力属性を設定
                        for( s32 g = 0; g < exe_info->outMapCount; ++g )
                        {
                            if ( ( !( gs_copy_mask & ( 1 << g ) )) && 
                                 ( outmapInfo[ g ].type >= 0  )    &&
                                 ( outmapInfo[ g ].type < 9 )      && 
                                 ( outmapInfo[ g ].type != 7 ) )
                            {
                                NN_GR_ASSERT( outMapBufferCount < OUT_ATTR_BUFFER_MAX );
                                outmap_buffer[ outMapBufferCount ].type = outmapInfo[ g ].type;
                                outmap_buffer[ outMapBufferCount ].index = outMapBufferCount;
                                outmap_buffer[ outMapBufferCount ].mask = outmapInfo[ g ].mask;
                                ++outMapBufferCount;
                            }
                        }

                        // 最後にVSのみで定義されている出力属性を設定
                        for( s32 v = 0; v < vtx_exe_info->outMapCount; ++v )
                        {
                            if ( ( !( vs_copy_mask & ( 1 << v ) ) ) && 
                                 ( vtxOutmapInfo[ v ].type >= 0 )   &&
                                 ( vtxOutmapInfo[ v ].type < 9  )   && 
                                 ( vtxOutmapInfo[ v ].type != 7 ) )
                            {
                                NN_GR_ASSERT( outMapBufferCount < OUT_ATTR_BUFFER_MAX );
                                outmap_buffer[ outMapBufferCount ].type = vtxOutmapInfo[ v ].type;
                                outmap_buffer[ outMapBufferCount ].index = outMapBufferCount;
                                outmap_buffer[ outMapBufferCount ].mask = vtxOutmapInfo[ v ].mask;
                                ++outMapBufferCount;
                            }
                        }
                    }
                    else
                    {
                        // 出力属性情報
                        const OutmapInfo* outmapInfo =
                            reinterpret_cast< const OutmapInfo* >(
                                reinterpret_cast< const u8* >( exe_info ) + exe_info->outMapOffset );
                        // マージしない場合はコピー.
                        for( s32 i = 0; i < exe_info->outMapCount; ++i )
                        {
                            outmap_buffer[ i ] = outmapInfo[ i ];
                        }
                        outMapBufferCount = exe_info->outMapCount;
                    }

                    for ( s32 index = 0; index < OUT_ATTR_INDEX_MAX; ++index )
                    {
                        attr[ index ] = 0x1f1f1f1f;
                        for ( s32 i = 0; i <outMapBufferCount; ++i )
                        {
                            bit32 c = 0;
                            for ( s32 j = 0; outmap_buffer[ i ].index == index && j < OUT_ATTR_DIMENTION_MAX; ++j )
                            {
                                if ( ( outmap_buffer[ i ].mask & ( 1 << j ) ) == 0 ) continue;

                                s32 value = 0x1f;
                                switch ( outmap_buffer[ i ].type )
                                {
                                case 0  :            value = 0x00 + c++; if (c == 2) clock |= 1 <<  0; break; // position
                                case 1  :            value = 0x04 + c++;             clock |= 1 << 24; break; // quaternion
                                case 2  :            value = 0x08 + c++;             clock |= 1 <<  1; break; // color
                                case 3  : if (c < 2) value = 0x0c + c++; useTex = 1; clock |= 1 <<  8; break; // texcoord0
                                case 4  :            value = 0x10;       useTex = 1; clock |= 1 << 16; break; // texcoord0w
                                case 5  : if (c < 2) value = 0x0e + c++; useTex = 1; clock |= 1 <<  9; break; // texcoord1
                                case 6  : if (c < 2) value = 0x16 + c++; useTex = 1; clock |= 1 << 10; break; // texcoord2
                                case 8  : if (c < 3) value = 0x12 + c++;             clock |= 1 << 24; break; // view
                                }
                                attr[ index ] = attr[ index ] & ~( 0xff << ( j * 8 ) ) | value << ( j * 8 );
                            }
                        }
                        if ( attr[ index ] != 0x1f1f1f1f )
                        {
                            outMask |= ( 1 << index );
                            ++outNum;
                        }
                    }
                }

                if ( is_geometry_shader )
                {  
                    // 頂点シェーダーの出力属性の設定を求めます
                    //m_DrawMode = PICA_DATA_DRAW_GEOMETRY_PRIMITIVE;

                    u32 vtxOutNum = 0;
                    bit32 vtxOutMask = 0;
                    bit32 vtxAttr[ VS_OUT_ATTR_INDEX_MAX ];

                    // プログラム情報
                    const ExeImageInfo* exe_info = m_ExeImageInfo[ vtx_shader_index ];
                    const OutmapInfo* outmapInfo =
                        reinterpret_cast< const OutmapInfo* >(
                            reinterpret_cast< const u8* >( exe_info ) + exe_info->outMapOffset );

                    for ( s32 index = 0; index < VS_OUT_ATTR_INDEX_MAX; ++index )
                    {
                        vtxAttr[ index ] = 0x1f1f1f1f;
                        for ( s32 i = 0; i < exe_info->outMapCount; ++i )
                        {
                            u32 c = 0;
                            for ( s32 j = 0; outmapInfo[ i ].index == index && j < OUT_ATTR_DIMENTION_MAX; ++j )
                            {
                                if ( ( outmapInfo[ i ].mask & ( 1 << j ) ) == 0 ) continue;

                                s32 value = 0x1f;
                                switch ( outmapInfo[ i ].type )
                                {
                                case 0  :            value = 0x00 + c++; break; // position
                                case 1  :            value = 0x04 + c++; break; // quaternion
                                case 2  :            value = 0x08 + c++; break; // color
                                case 3  : if (c < 2) value = 0x0c + c++; break; // texcoord0
                                case 4  :            value = 0x10;       break; // texcoord0w
                                case 5  : if (c < 2) value = 0x0e + c++; break; // texcoord1
                                case 6  : if (c < 2) value = 0x16 + c++; break; // texcoord2
                                case 8  : if (c < 3) value = 0x12 + c++; break; // view
                                case 9  : value = 0xff;
                                }
                                vtxAttr[ index ] = vtxAttr[ index ] & ~( 0xff << ( j * 8 ) ) | value << ( j * 8 );
                            }
                        }
                        if ( vtxAttr[ index ] != 0x1f1f1f1f )
                        {
                            vtxOutMask |= ( 1 << index );
                            ++vtxOutNum;
                        }
                    }

                    bit32 gsDataMode = m_ExeImageInfo[ geo_shader_index ]->gsDataMode;

                    // GL_GEOMETRY_PRIMITIVE_DMP

                    // 0x229 [31:31], 
                    // データモード 1 の場合
                    if ( gsDataMode == 1 )
                    {
                        *command++ = 0x80000000;
                    }
                    // データモード 0 と データモード2 の場合
                    else
                    {
                        *command++ = 0x00000000;
                    }
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE0, 0xa );
                    
                    // 0x253
                    *command++ = 0x00000000;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE1, 0x3 );

                    // 0x289 頂点シェーダーの出力数
                    *command++ = 0x08000000 | (gsDataMode == 0 ? 0x0000 : 0x0100) | vtxOutNum - 1;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_GS_ATTR_NUM, 0xb );

                    // 0x28a ジオメトリシェーダーのmainラベルのアドレス
                    *command++ = 0x7fff0000 | m_ExeImageInfo[ geo_shader_index ]->mainAddress;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_START_ADDR );

                    // 0x28d 頂点シェーダーの出力mask
                    *command++ = outMask;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_OUT_REG_MASK );

                    // 0x2ba 頂点シェーダーのmainラベルのアドレス
                    *command++ = 0x7fff0000 | m_ExeImageInfo[ vtx_shader_index ]->mainAddress;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_START_ADDR );

                    // 0x2bd 頂点シェーダーの出力レジスタマスク
                    *command++ = vtxOutMask;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_REG_MASK );

                    // 0x251 頂点シェーダーの出力数
                    *command++ = vtxOutNum - 1;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_REG_NUM2 );

                    // 0x28b 頂点シェーダーの出力とジオメトリシェーダーの入力は一致させているのが前提
                    *command++ = 0x76543210;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_ATTR_IN_REG_MAP0 ); // 0x28b
                    
                    // 0x28c
                    *command++ = 0xfedcba98;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_ATTR_IN_REG_MAP1 );

                    // 0x254
                    if ( ( gsDataMode == 1 ) &&
                         ( m_ExeImageInfo[ geo_shader_index ]->gsPatchSize != 0 ) )
                    {
                        *command++ = m_ExeImageInfo[ geo_shader_index ]->gsPatchSize - 1;
                        *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_GS_MISC_REG1, 0x1 );
                    }

                    // 0x252 用の値
                    if ( gsDataMode == 2 )
                    {
                        gsDataMode |= 0x01                                                       << 24;
                        gsDataMode |= ( m_ExeImageInfo[ geo_shader_index ]->gsVertexStartIndex ) << 16;
                        gsDataMode |= ( vtxOutNum - 1 )                                          << 12;
                        gsDataMode |= ( m_ExeImageInfo[ geo_shader_index ]->gsVertexNum - 1 )    <<  8;
                    }

                    // 0x252 データモードの指定
                    *command++ = gsDataMode;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_MISC_REG0 );

                    // 0x24a
                    *command++ = vtxOutNum - 1;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_REG_NUM1 );
                }
                else
                {
                    // 0x229 [31:31]
                    *command++ = 0x0;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE0, 0x8 );

                    // 0x253 [0:0]
                    *command++ = 0x0;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE1, 0x1 );
                    
                    // 0x289 [31:24], [15:8], [3:0] 頂点シェーダーのモードの設定
                    *command++ = 0xa0000000;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_GS_ATTR_NUM, 0xb );

                    // 0x2ba 頂点シェーダーのmainラベルのアドレス
                    *command++ = 0x7fff0000 | m_ExeImageInfo[ vtx_shader_index ]->mainAddress;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_START_ADDR );

                    // 0x2bd 頂点シェーダーの出力レジスタマスク
                    *command++ = outMask;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_REG_MASK );

                    // 0x251
                    *command++ = outNum - 1;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_REG_NUM2 );

                    // 0x252
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_GS_MISC_REG0 );

                    // 0x24a
                    *command++ = outNum - 1;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_REG_NUM1 );
                }

                {
                    // 0x25e [3:0]
                    *command++ = outNum - 1;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_VS_OUT_REG_NUM3, 0x1 );
                    
                    // 0x04f
                    *command++ = outNum;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_REG_NUM0 );

                    // 出力属性のコマンド
                    outNum = 0;
                    for ( s32 index = 0; index < OUT_ATTR_INDEX_MAX; ++index )
                    {
                        if ( attr[ index ] != 0x1f1f1f1f )
                        {
                            *command++ = attr[ index ];
                            *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_ATTR0 + outNum );
                            ++outNum;
                        }
                    }
                    for ( s32 index = outNum; index < OUT_ATTR_INDEX_MAX; ++index )
                    {
                        *command++ = attr[ index ];
                        *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_ATTR0 + index );
                    }
                }

                // 0x064 テクスチャ座標を使うかどうかを設定
                *command++ = useTex;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_ATTR_MODE );

                // 0x06f 出力属性のクロック制御を設定
                *command++ = clock;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_OUT_ATTR_CLK );

                if ( is_geometry_shader )
                {
                    // 0x25e
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_VS_OUT_REG_NUM3, 8 );
                }

                return command;
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakeLoadCommand_( bit32* command,
                                             const bit32  load_reg,
                                             const bit32* src_buffer_ptr,
                                             const u32  src_data_num ) const
            {
                const s32 WRITE_MAX = 128;

                u32 rest = src_data_num;

                while ( true )
                {
                    if ( rest <= WRITE_MAX )
                    {
                        *command++ = *src_buffer_ptr++;
                        *command++ = PICA_CMD_HEADER_BURST( load_reg, rest );
                        memcpy( command, src_buffer_ptr, ( rest - 1 ) * sizeof( bit32 ) );
                        command += rest - 1;

                        if ( ( rest & 1 ) == 0 ) *command++ = PADDING_DATA; // padding
                        break;
                    }
                    else
                    {
                        *command++ = *src_buffer_ptr++;
                        *command++ = PICA_CMD_HEADER_BURST( load_reg, WRITE_MAX );
                        memcpy( command, src_buffer_ptr, ( WRITE_MAX - 1 ) * sizeof( bit32 ) );

                        command += WRITE_MAX - 1;
                        src_buffer_ptr += WRITE_MAX - 1;

                        rest -= WRITE_MAX;
                        if ( ( WRITE_MAX & 1 ) == 0 ) *command++ = PADDING_DATA; // padding
                    }
                }

                return command;
            }

            //------------------------------------------------------------------------

            bit32* ShaderLite::MakeDummyCommand_( bit32* command,
                                              const bit32 load_reg,
                                              const u32 dataNum )
            {
                *command++ = 0;
                *command++ = PICA_CMD_HEADER_BURST_BE( load_reg, dataNum, 0 );

                for ( s32 i = 0; i < dataNum - ( dataNum & 1 ); ++i)
                {
                    *command++ = PADDING_DATA;
                }

                return command;
            }

            //------------------------------------------------------------------------

            void ShaderLite::MakeRgCmdOffsetCache_(void)
            {
                // 定数レジスタ用のオフセット値計算
                m_ConstRgCmdOffsetArray[0] = 0;
                for(int i = 1; i < m_ExeImageInfoNum; i++ )
                {
                    m_ConstRgCmdOffsetArray[i] = m_CmdCacheConstNumArray[i - 1] + m_ConstRgCmdOffsetArray[i - 1];
                }

                // 出力属性コマンドキャッシュに必要なインデックス配列の計算
                // この配列をもとに、頂点シェーダとジオメトリシェーダを組み合わせた時のインデックスを算出する

                // m_GeoShaderIndex = -1 の時
                m_CmdCacheOutAttrIndexArray[0] = 0;

                u8 vtx_index = 0;
                u8 geo_index = 1;
                for(u32 idx = 1; idx < m_ExeImageInfoNum + 1; idx++)
                {
                    if(!m_ExeImageInfo[idx - 1]->isGeoShader)
                    {
                        m_CmdCacheOutAttrIndexArray[idx] = vtx_index;
                        vtx_index += (m_GeometryShaderNum + 1);
                    }
                    else
                    {
                        m_CmdCacheOutAttrIndexArray[idx] = geo_index;
                        geo_index++;
                    }
                }

                // 出力属性コマンドキャッシュに必要なオフセット値の計算
                u32 combination_num = (m_ExeImageInfoNum - m_GeometryShaderNum) * (m_GeometryShaderNum + 1);

                m_CmdCacheOutAttrOffsetArray[0] = 0;
                for(u32 i = 1; i < combination_num; i++)
                {
                    m_CmdCacheOutAttrOffsetArray[i] = m_CmdCacheOutAttrNumArray[i - 1] + m_CmdCacheOutAttrOffsetArray[i - 1];
                }
            }

            //------------------------------------------------------------------------
            u32 ShaderLite::CalculateBufferSize_() const
            {
                u32 size = 0;

                // m_Swizzle
                size += m_SwizzleCount * sizeof(bit32);

                // m_CmdCacheConstNumArray
                size += m_ExeImageInfoNum * sizeof(u32);

                // m_CmdCacheConstArray
                int num = 0;
                for ( u8 i = 0; i < m_ExeImageInfoNum; i++ )
                {
                    // プログラム情報
                    NN_GR_ASSERT( ( 0 <= i ) && 
                                  ( i < m_ExeImageInfoNum ) );
                    const ExeImageInfo* exe_info = m_ExeImageInfo[ i ];

                    // 定数レジスタ情報
                    struct SetupInfo
                    {
                        u16 type;
                        u16 index;
                        bit32 value[4];
                    };

                    const SetupInfo* setupInfo =
                        reinterpret_cast< const SetupInfo* >(
                            reinterpret_cast< const u8* >( exe_info ) + exe_info->setupOffset );

                    // 定数レジスタのコマンド数
                    for ( u32 j = 0; j < exe_info->setupCount; ++j )
                    {
                        const SetupInfo& info = setupInfo[ j ];

                        switch ( info.type )
                        {
                        case 0 : // bool register setup
                            break;

                        case 1 : // integer register setup
                            num += 2;
                            break;

                        case 2 : // float register setup
                            num += 6;
                            break;
                        }
                    }
                }

                size += num * sizeof(bit32);

                // 頂点シェーダとジオメトリシェーダの組み合わせ数
                u32 num_geo = 0;
                for(u8 i = 0; i < m_ExeImageInfoNum; i++)
                {
                    num_geo += m_ExeImageInfo[i]->isGeoShader;
                }
                u32 num_combination = (m_ExeImageInfoNum - num_geo) * (num_geo + 1);

                // m_CmdCacheOutAttrNumArray
                size += num_combination * sizeof(u32);

                // m_CmdCacheOutAttrArray
                for(u8 i = 0; i < m_ExeImageInfoNum; i++)
                {
                    if( !m_ExeImageInfo[i]->isGeoShader )
                    {
                        // 頂点シェーダ単体での動作
                        size += 38 * sizeof(u32);
                        // ジオメトリシェーダとの組み合わせ
                        for(int j = 0; j < m_ExeImageInfoNum; j++)
                        {
                            if( m_ExeImageInfo[j]->isGeoShader )
                            {
                                size += (m_ExeImageInfo[j]->gsDataMode == 1) ? 50 * sizeof(u32): 48 * sizeof(u32);
                            }
                        }
                    }
                }

                // m_ConstRgCmdOffsetArray
                size += m_ExeImageInfoNum * sizeof(u32);

                // m_CmdCacheOutAttrIndexArray
                size += (m_ExeImageInfoNum + 1) * sizeof(u32);

                // m_CmdCacheOutAttrOffsetArray
                size += num_combination * sizeof(u32);

                return size;
            }

            void ShaderLite::AssignBuffer_(bit32* buf)
            {
                // m_Swizzle
                m_Swizzle = buf;
                buf += m_SwizzleCount;

                // m_CmdCacheConstNumArray
                m_CmdCacheConstNumArray = buf;
                buf += m_ExeImageInfoNum;

                // m_CmdCacheConstArray
                m_CmdCacheConstArray = buf;
                int num = 0;
                for ( u8 i = 0; i < m_ExeImageInfoNum; i++ )
                {
                    // プログラム情報
                    NN_GR_ASSERT( ( 0 <= i ) && 
                                  ( i < m_ExeImageInfoNum ) );
                    const ExeImageInfo* exe_info = m_ExeImageInfo[ i ];

                    // 定数レジスタ情報
                    struct SetupInfo
                    {
                        u16 type;
                        u16 index;
                        bit32 value[4];
                    };

                    const SetupInfo* setupInfo =
                        reinterpret_cast< const SetupInfo* >(
                            reinterpret_cast< const u8* >( exe_info ) + exe_info->setupOffset );

                    // 定数レジスタのコマンド数
                    for ( u32 j = 0; j < exe_info->setupCount; ++j )
                    {
                        const SetupInfo& info = setupInfo[ j ];

                        switch ( info.type )
                        {
                        case 0 : // bool register setup
                            break;

                        case 1 : // integer register setup
                            num += 2;
                            break;

                        case 2 : // float register setup
                            num += 6;
                            break;
                        }
                    }

                    m_CmdCacheConstNumArray[i] = num;
                    buf += num;
                    num = 0;
                }

                // 頂点シェーダとジオメトリシェーダの組み合わせ数
                m_GeometryShaderNum = 0;
                for(u8 i = 0; i < m_ExeImageInfoNum; i++)
                {
                    m_GeometryShaderNum += m_ExeImageInfo[i]->isGeoShader;
                }
                u32 combination_num = (m_ExeImageInfoNum - m_GeometryShaderNum) * (m_GeometryShaderNum + 1);

                // m_CmdCacheOutAttrNumArray
                m_CmdCacheOutAttrNumArray = buf;
                u8 idx = 0;
                for(u8 i = 0; i < m_ExeImageInfoNum; i++)
                {
                    if( !m_ExeImageInfo[i]->isGeoShader )
                    {
                        // 頂点シェーダ単体での動作を考慮する
                        m_CmdCacheOutAttrNumArray[idx] = 38;
                        idx++;
                        // 全てのジオメトリシェーダとの組み合わせを考慮
                        for(int j = 0; j < m_ExeImageInfoNum; j++)
                        {
                            if( m_ExeImageInfo[j]->isGeoShader )
                            {
                                m_CmdCacheOutAttrNumArray[idx] = (m_ExeImageInfo[j]->gsDataMode == 1) ? 50 : 48;
                                idx++;
                            }
                        }
                    }
                }
                buf += combination_num;

                // m_CmdCacheOutAttrArray
                m_CmdCacheOutAttrArray = buf;
                for(u8 i = 0; i < combination_num; i++)
                {
                    buf += m_CmdCacheOutAttrNumArray[i];
                }

                // m_ConstRgCmdOffsetArray
                m_ConstRgCmdOffsetArray = buf;
                buf += m_ExeImageInfoNum;

                // m_CmdCacheOutAttrIndexArray
                m_CmdCacheOutAttrIndexArray = buf;
                buf += m_ExeImageInfoNum + 1;

                // m_CmdCacheOutAttrOffsetArray
                m_CmdCacheOutAttrOffsetArray = buf;
                
            }

        } //namespace CTR
    } //namespace gr
} //namespace nn
