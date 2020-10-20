/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Texture.cpp

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#include "gr_Texture.h"

#define NN_ASSERT assert

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            bit32* Texture::Unit0::MakeCommand( bit32* command, bool is_update_texture_func ) const
            {
                if ( is_update_texture_func )
                {
                    command = m_Texture.MakeFuncCommand( command );
                }
                
                // 0x081
                *command++ = borderColorR | borderColorG << 8 | borderColorB << 16 | borderColorA << 24;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_BORDER_COLOR );

                // 0x082
                //*command++ = PICA_CMD_DATA_TEXTURE_SIZE( width, height );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_SIZE );

                // 0x083
                u8 useShadow = 0;
                if ( ( texType == PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_SHADOW_2D_DMP ) ||
                     ( texType == PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_SHADOW_CUBE_DMP ) )
                {
                    useShadow = 1;
                }
                //*command++ = PICA_CMD_DATA_TEXTURE_WRAP_FILTER( magFilter, minFilter, format, wrapT, wrapS, useShadow, texType );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_WRAP_FILTER );

                /*if ( IsEnableMipMap() )
                {
                    NN_ASSERT( -16.f <= lodBias && lodBias <= 16.f );

                    // 0x084
                    *command++ = PICA_CMD_DATA_TEXTURE_LOD_LEVEL( Float32ToFix13Fraction8( lodBias ), maxLodLevel, minLodLevel );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_LOD );
                }
                else
                {
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_LOD );
                }*/

                /*if ( ( texType == PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D ) || 
                     ( texType == PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_PROJECTION_DMP ) || 
                     ( texType == PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_SHADOW_2D_DMP ) )
                {
                    // 0x085
                    *command++ = PICA_CMD_DATA_TEXTURE_ADDR( physicalAddr );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR1 );
                }
                else if ( ( texType == PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_CUBE_MAP ) ||
                          ( texType == PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_SHADOW_CUBE_DMP ) )
                {
                    // 0x085
                    *command++ = PICA_CMD_DATA_TEXTURE_ADDR( cubeMapAddrPosX );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR1 );

                    // 0x086
                    *command++ = PICA_CMD_DATA_TEXTURE_CUBE_MAP_ADDR( PICA_CMD_DATA_TEXTURE_ADDR( cubeMapAddrNegX ) );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR2 );

                    // 0x087
                    *command++ = PICA_CMD_DATA_TEXTURE_CUBE_MAP_ADDR( PICA_CMD_DATA_TEXTURE_ADDR( cubeMapAddrPosY ) );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR3 );

                    // 0x088
                    *command++ = PICA_CMD_DATA_TEXTURE_CUBE_MAP_ADDR( PICA_CMD_DATA_TEXTURE_ADDR( cubeMapAddrNegY ) );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR4 );

                    // 0x089
                    *command++ = PICA_CMD_DATA_TEXTURE_CUBE_MAP_ADDR( PICA_CMD_DATA_TEXTURE_ADDR( cubeMapAddrPosZ ) );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR5 );

                    // 0x08a
                    *command++ = PICA_CMD_DATA_TEXTURE_CUBE_MAP_ADDR( PICA_CMD_DATA_TEXTURE_ADDR( cubeMapAddrNegZ ) );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR6 );
                }*/

                // 0x08e
                //*command++ = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( format );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_FORMAT );

                return command;
            }

            //------------------------------------------------------------------------------
            bit32* Texture::Unit1::MakeCommand( bit32* command, bool is_update_texture_func ) const
            {
                if ( is_update_texture_func )
                {
                    command = m_Texture.MakeFuncCommand( command );
                }
                
                // 0x091
                *command++ = borderColorR | borderColorG << 8 | borderColorB << 16 | borderColorA << 24;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_BORDER_COLOR );

                // 0x092
                //*command++ = PICA_CMD_DATA_TEXTURE_SIZE( width, height );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_SIZE );

                // 0x093
                //*command++ = PICA_CMD_DATA_TEXTURE1_WRAP_FILTER( magFilter, minFilter, format, wrapT, wrapS );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_WRAP_FILTER );


                /*if ( IsEnableMipMap() )
                {
                    NN_ASSERT( -16.f <= lodBias && lodBias <= 16.f );
                    // 0x094
                    *command++ = PICA_CMD_DATA_TEXTURE_LOD_LEVEL( Float32ToFix13Fraction8( lodBias ), maxLodLevel, minLodLevel );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_LOD );
                }
                else
                {
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_LOD );
                }*/

                // 0x095
                //*command++ = PICA_CMD_DATA_TEXTURE_ADDR( physicalAddr );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_ADDR );

                // 0x096
                //*command++ = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( format );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE1_FORMAT );

                return command;
            }

            //------------------------------------------------------------------------------
            bit32* Texture::Unit2::MakeCommand( bit32* command, bool is_update_texture_func ) const
            {
                if ( is_update_texture_func )
                {
                    command = m_Texture.MakeFuncCommand( command );
                }
                
                // 0x099
                *command++ = borderColorR | borderColorG << 8 | borderColorB << 16 | borderColorA << 24;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE2_BORDER_COLOR );

                // 0x09a
                //*command++ = PICA_CMD_DATA_TEXTURE_SIZE( width, height );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE2_SIZE );

                // 0x09b
                //*command++ = PICA_CMD_DATA_TEXTURE2_WRAP_FILTER( magFilter, minFilter, format, wrapT, wrapS );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE2_WRAP_FILTER );

                /*if ( IsEnableMipMap() )
                {
                    NN_ASSERT( -16.f <= lodBias && lodBias <= 16.f );
                    // 0x09c
                    *command++ = PICA_CMD_DATA_TEXTURE_LOD_LEVEL( Float32ToFix13Fraction8( lodBias ), maxLodLevel, minLodLevel );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE2_LOD );
                }
                else
                {
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE2_LOD );
                }*/

                // 0x09d
                //*command++ = PICA_CMD_DATA_TEXTURE_ADDR( physicalAddr );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE2_ADDR );

                // 0x09e
                //*command++ = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( format );
                //*command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE2_FORMAT );

                return command;
            }

            //------------------------------------------------------------------------------
            bit32* Texture::Unit3::MakeCommand( bit32* command, bool is_update_texture_func ) const
            {
                if ( is_update_texture_func )
                {
                    command = m_Texture.MakeFuncCommand( command );
                }
                
                return command;
            }

            //------------------------------------------------------------------------------

            bit32* Texture::MakeCommand( bit32* command, bool isAddDummyCommand ) const
            {
                command = MakeFuncCommand( command, isAddDummyCommand );
                command = unit0.MakeCommand( command, false );
                command = unit1.MakeCommand( command, false );
                command = unit2.MakeCommand( command, false );
                command = unit3.MakeCommand( command, false );

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* Texture::MakeFuncCommand( bit32* command, bool isAddDummyCommand ) const
            {
                if ( isAddDummyCommand )
                {
                    command = MakeDummyCommand_( command );
                }

                // 0x080
                *command++ = PICA_CMD_DATA_TEXTURE_FUNC( unit0.texType, unit1.texType, unit2.texType, 
                                                         unit3.texCoord, unit3.texType, unit2.texCoord, 0x1 );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE_FUNC );

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* Texture::MakeDisableCommand( bit32* command, bool isAddDummyCommand )
            {
                if ( isAddDummyCommand )
                {
                    command = MakeDummyCommand_( command );
                }

                const PicaDataTexture0SamplerType texture0SamplerType = PICA_DATA_TEXTURE0_SAMPLER_TYPE_FALSE;
                const PicaDataTexture1SamplerType texture1SamplerType = PICA_DATA_TEXTURE1_SAMPLER_TYPE_FALSE;
                const PicaDataTexture2SamplerType texture2SamplerType = PICA_DATA_TEXTURE2_SAMPLER_TYPE_FALSE;
                const PicaDataTexture2TexCoord    texture2TexCoord    = PICA_DATA_TEXTURE2_TEXCOORD_TEXTURE1;
                const PicaDataTexture3SamplerType texture3SamplerType = PICA_DATA_TEXTURE3_SAMPLER_TYPE_FALSE;
                const PicaDataTexture3TexCoord    texture3TexCoord    = PICA_DATA_TEXTURE3_TEXCOORD_TEXTURE0;
                const bit32                       clearTextureCache   = 1;

                // 0x080
                *command++ = PICA_CMD_DATA_TEXTURE_FUNC( texture0SamplerType,
                                                         texture1SamplerType,
                                                         texture2SamplerType,
                                                         texture3TexCoord,
                                                         texture3SamplerType,
                                                         texture2TexCoord,
                                                         clearTextureCache );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE_FUNC );

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* Texture::MakeDummyCommand_( bit32* command )
            {
                // 0x080 へのダミーコマンド を 3 回送ります      
                *command++ = 0x0;
                *command++ = PICA_CMD_HEADER_BURST_BE( PICA_REG_TEXTURE_FUNC, 0x3, 0x0 );

                *command++ = 0x0;
                *command++ = 0x0;

                return command;
            }

            //------------------------------------------------------------------------------

            Texture::UnitBase::UnitBase()
                : physicalAddr( 0 ),
                  width( 0 ),
                  height( 0 ),
                  format( PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP ),
                  wrapT( PICA_DATA_TEXTURE_WRAP_REPEAT ),
                  wrapS( PICA_DATA_TEXTURE_WRAP_REPEAT ),
                  magFilter( PICA_DATA_TEXTURE_MAG_FILTER_NEAREST ),
                  minFilter( PICA_DATA_TEXTURE_MIN_FILTER_NEAREST ),
                  lodBias( 0.f ),              
                  minLodLevel( 0 ),
                  maxLodLevel( 0 ),
                  borderColorR( 0 ),
                  borderColorG( 0 ),
                  borderColorB( 0 ),
                  borderColorA( 0 )
            {
            }

            //------------------------------------------------------------------------------

            Texture::Unit0::Unit0( const Texture& texture_ )
                : texType( PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_FALSE ),
                  cubeMapAddrPosX( 0 ),
                  cubeMapAddrNegX( 0 ),
                  cubeMapAddrPosY( 0 ),
                  cubeMapAddrNegY( 0 ),
                  cubeMapAddrPosZ( 0 ),
                  cubeMapAddrNegZ( 0 ),
                  m_Texture( texture_ )
            {
            }

            //------------------------------------------------------------------------------

            Texture::Unit1::Unit1( const Texture& texture_ )
                : texType( PICA_DATA_TEXTURE1_SAMPLER_TYPE_FALSE ),
                  m_Texture( texture_ )
            {
            }

            //------------------------------------------------------------------------------

            Texture::Unit2::Unit2( const Texture& texture_ )
                : texType(  PICA_DATA_TEXTURE2_SAMPLER_TYPE_FALSE ),
                  texCoord( PICA_DATA_TEXTURE2_TEXCOORD_TEXTURE2 ),
                  m_Texture(  texture_ )
            {
            }

            //------------------------------------------------------------------------------

            Texture::Unit3::Unit3( const Texture& texture_ )
                : texType(  PICA_DATA_TEXTURE3_SAMPLER_TYPE_FALSE ),
                  texCoord( PICA_DATA_TEXTURE3_TEXCOORD_TEXTURE0 ),
                  m_Texture(  texture_ )
            {
            }

            //------------------------------------------------------------------------------

            Texture::Texture()
                : unit0( *this ),
                  unit1( *this ),
                  unit2( *this ),
                  unit3( *this )
            {
            }

        } // namespace CTR
    } // namespace gr
} // namespace nn
