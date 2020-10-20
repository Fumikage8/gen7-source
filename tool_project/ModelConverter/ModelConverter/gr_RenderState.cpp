/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_RenderState.cpp

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#include "gr_RenderState.h"

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            bit32* RenderState::Culling::MakeCommand( bit32* command, bool isUpdateFBAccess ) const
            {
                bit32 culling = 0;

                if ( isEnable )
                {
                    if ( ( frontFace == FRONT_FACE_CW  && cullFace == CULL_FACE_FRONT ) ||
                         ( frontFace == FRONT_FACE_CCW && cullFace == CULL_FACE_BACK ) )
                    {
                        culling = 2;
                    }
                    else
                    {
                        culling = 1;
                    }
                }

                // 0x040
                *command++ = culling;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_CULL_FACE );

                return isUpdateFBAccess ? m_RenderState.fbAccess.MakeCommand( command ) : command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::Culling::MakeDisableCommand( bit32* command, bool isClearFrameBufferCache )
            {
                // 0x040
                *command++ = 0x0;
                *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_CULL_FACE, 0x1 );

                if ( isClearFrameBufferCache )
                {
                    command = FBAccess::MakeClearCacheCommand( command );
                }

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::Blend::MakeCommand( bit32* command, bool isUpdateFBAccess ) const
            {
                if ( isEnable )
                {
                    // 0x100
                    *command++ = PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE_DMP, PICA_DATA_ENABLE_BLEND );
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_OPERATION, 0x3 );

                    // 0x101
                    *command++ = PICA_CMD_DATA_BLEND_FUNC_SEPARATE( eqRgb, eqAlpha, srcRgb, dstRgb, srcAlpha, dstAlpha );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_FUNC );

                    // 0x102
                    *command++ = PICA_CMD_DATA_LOGIC_OP( PICA_DATA_LOGIC_NOOP );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_LOGIC_OP );

                    // 0x103
                    *command++ = colorR | colorG << 8 | colorB << 16 | colorA << 24;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_COLOR );
                }
                else
                {
                    command = Blend::MakeDisableCommand( command, false );
                }

                return isUpdateFBAccess ? m_RenderState.fbAccess.MakeCommand( command ) : command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::Blend::MakeDisableCommand( bit32* command, bool isClearFrameBufferCache )
            {
                // 0x100
                *command++ = PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE_DMP, PICA_DATA_ENABLE_BLEND );
                *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_OPERATION, 0x3 );

                // 0x101
                *command++ = PICA_CMD_DATA_BLEND_FUNC( PICA_DATA_BLEND_EQUATION_ADD, PICA_DATA_BLEND_FUNC_ONE, PICA_DATA_BLEND_FUNC_ZERO );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_FUNC );

                if ( isClearFrameBufferCache )
                {
                    command = FBAccess::MakeClearCacheCommand( command );
                }

                return command;
            }

            bool RenderState::Blend::IsDstColorUsed(void) const
            {
                if ( dstRgb != PICA_DATA_BLEND_FUNC_ZERO || dstAlpha != PICA_DATA_BLEND_FUNC_ZERO )
                {
                    return true;
                }

                if ( srcRgb == PICA_DATA_BLEND_FUNC_DST_COLOR || srcRgb == PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR ||
                     srcRgb == PICA_DATA_BLEND_FUNC_DST_ALPHA || srcRgb == PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA ||
                     srcRgb == PICA_DATA_BLEND_FUNC_SRC_ALPHA_SATURATE )
                {
                    return true;
                }

                if ( srcAlpha == PICA_DATA_BLEND_FUNC_DST_COLOR || srcAlpha == PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR ||
                     srcAlpha == PICA_DATA_BLEND_FUNC_DST_ALPHA || srcAlpha == PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA ||
                     srcAlpha == PICA_DATA_BLEND_FUNC_SRC_ALPHA_SATURATE )
                {
                    return true;
                }

                return false;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::LogicOp::MakeCommand( bit32* command, bool isUpdateFBAccess ) const
            {
                if ( isEnable )
                {
                    // 0x100
                    *command++ = PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE_DMP, PICA_DATA_ENABLE_COLOR_LOGIC_OP );
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_OPERATION, 0x3 );

                    // 0x101
                    *command++ = PICA_CMD_DATA_LOGIC_OP_ENABLE();
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_FUNC );

                    // 0x102
                    *command++ = PICA_CMD_DATA_LOGIC_OP( opCode );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_LOGIC_OP );
                }

                return isUpdateFBAccess ? m_RenderState.fbAccess.MakeCommand( command ) : command;
            }

            bool RenderState::LogicOp::IsDstColorUsed(void) const
            {
                if ( opCode == PICA_DATA_LOGIC_CLEAR || opCode == PICA_DATA_LOGIC_COPY ||
                     opCode == PICA_DATA_LOGIC_SET || opCode == PICA_DATA_LOGIC_COPY_INVERTED )
                {
                    return false;
                }

                return true;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::ShadowMap::MakeCommand( bit32* command,
                bool isUpdateFBAccess, bool isAddDummyCommand ) const
            {
                if ( isEnable )
                {
                    // 0x100
                    *command++ = PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE_SHADOW_DMP, PICA_DATA_ENABLE_BLEND );                        
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_OPERATION, 0x1 );

                    // 0x8b
                    command = MakeTextureCommand( command, isAddDummyCommand );

                    // 0x130
                    command = MakeAttenuationCommand( command );
                }

                return isUpdateFBAccess ? m_RenderState.fbAccess.MakeCommand( command ) : command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::ShadowMap::MakeTextureCommand( bit32* command, bool isAddDummyCommand ) const
            {
                // 0x080 へのダミーコマンド を 3 回送ります
                if ( isAddDummyCommand )
                {
                    *command++ = 0x0;
                    *command++ = PICA_CMD_HEADER_BURST_BE( PICA_REG_TEXTURE_FUNC, 0x3, 0x0 );

                    *command++ = 0x0;
                    *command++ = 0x0;
                }

                // 下位 1 bit をクリアして、24 bit のうち上位 23 bit を利用します
                u32 zBiasFix24   = Float32ToUnsignedFix24( zBias );
                zBiasFix24       = zBiasFix24 >> 1;

                // zScale の値は無効で、互換性のためだけに残してあります。
                // ( ここでは 0 を代入しておきます。 )
                u8 zScale8 = 0;

                // 0x8b
                *command++ = 
                    PICA_CMD_DATA_TEXTURE_SHADOW( isPerspective,
                        // f32 -> 24bit 符号なし固定小数点数 の上位 23 bit
                        zBiasFix24,
                        // zScale の値は無効
                        zScale8 );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_SHADOW );

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::ShadowMap::MakeAttenuationCommand( bit32* command ) const
            {
                // 0x130
                *command++ = ( Float32ToFloat16( - penumbraScale ) ) << 16 |
                                Float32ToFloat16( penumbraScale + penumbraBias );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_SHADOW );

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::AlphaTest::MakeCommand( bit32* command, bool isUpdateFBAccess ) const
            {
                // 0x104
                *command++ = PICA_CMD_DATA_FRAGOP_ALPHA_TEST( isEnable, func, refValue );
                *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_FRAGOP_ALPHA_TEST, 0x3 );

                return isUpdateFBAccess ? m_RenderState.fbAccess.MakeCommand( command ) : command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::AlphaTest::MakeDisableCommand( bit32* command, bool isClearFrameBufferCache )
            {
                // 0x104
                *command++ = 0x0;
                *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_FRAGOP_ALPHA_TEST, 0x1 );

                if ( isClearFrameBufferCache )
                {
                    command = FBAccess::MakeClearCacheCommand( command );
                }

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::StencilTest::MakeCommand( bit32* command, bool isUpdateFBAccess ) const
            {
                // 0x105
                *command++ = PICA_CMD_DATA_STENCIL_TEST( isEnable, func, maskOp, ref, mask );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_STENCIL_TEST );

                // 0x106
                *command++ = PICA_CMD_DATA_STENCIL_OP( opFail, opZFail, opZPass );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_STENCIL_OP );

                return isUpdateFBAccess ? m_RenderState.fbAccess.MakeCommand( command ) : command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::StencilTest::MakeDisableCommand( bit32* command, bool isClearFrameBufferCache )
            {
                // 0x105
                *command++ = 0x0;
                *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_STENCIL_TEST, 0x1 );

                if ( isClearFrameBufferCache )
                {
                    command = FBAccess::MakeClearCacheCommand( command );
                }

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::DepthTest::MakeCommand( bit32* command, bool isUpdateFBAccess ) const
            {
                // 0x107の[ 0:7 ]と[12:12]がデプステストの設定で、[8:11]がカラーマスクの設定です。
                *command++ = PICA_CMD_DATA_DEPTH_COLOR_MASK( isEnable,
                                                             func,
                                                             m_RenderState.colorMask & COLOR_MASK_R,
                                                             m_RenderState.colorMask & COLOR_MASK_G,
                                                             m_RenderState.colorMask & COLOR_MASK_B,
                                                             m_RenderState.colorMask & COLOR_MASK_A,
                                                             isEnableWrite );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_DEPTH_COLOR_MASK );

                return isUpdateFBAccess ? m_RenderState.fbAccess.MakeCommand( command ) : command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::DepthTest::MakeDisableCommand( bit32* command, bool isClearFrameBufferCache )
            {
                const bool isEnableDepth          = false;
                const PicaDataDepthTest depthFunc = PICA_DATA_DEPTH_TEST_LESS;
                const bool colorMaskRed           = true;
                const bool colorMaskGreen         = true;
                const bool colorMaskBlue          = true;
                const bool colorMaskAlpha         = true;
                const bool isEnableDepthWrite     = false;

                // 0x107の[ 0:7 ]と[12:12]がデプステストの設定で、[8:11]がカラーマスクの設定です。
                *command++ = PICA_CMD_DATA_DEPTH_COLOR_MASK( isEnableDepth,
                                                             depthFunc,
                                                             colorMaskRed,
                                                             colorMaskGreen,
                                                             colorMaskBlue,
                                                             colorMaskAlpha,
                                                             isEnableDepthWrite );
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_DEPTH_COLOR_MASK );

                if ( isClearFrameBufferCache )
                {
                    command = FBAccess::MakeClearCacheCommand( command );
                }

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::WBuffer::MakeCommand( bit32* command ) const
            {
                // w バッファが無効のとき
                if ( wScale == 0.0f )
                {
                    // 0x6d
                    *command++ = 1;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_WSCALE );

                    // 0x04d
                    *command++ = Float32ToFloat24( depthRangeNear - depthRangeFar );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_WSCALE_DATA1 );

                    // 0x04e
                    // ポリゴンオフセットを設定する
                    f32 zNear = isEnablePolygonOffset ? 
                        // 24 bit float の精度限界により誤差で消えてしまうのを避けるため、
                        // 0.5f 付近の深度値を基準として、128.0f をかけておきます。
                        depthRangeNear - (depthRangeNear - depthRangeFar) * polygonOffsetUnit * 128.0f / f32( (1 << depthRangeBit) - 1) :
                        depthRangeNear;
                    *command++ = Float32ToFloat24( zNear );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_WSCALE_DATA2 );
                }
                // w バッファが有効のとき
                else
                {
                    // 0x6d
                    *command++ = 0;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_WSCALE );

                    // 0x04d
                    *command++ = Float32ToFloat24( - wScale );
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_WSCALE_DATA1 );

                    // 0x04e
                    *command++ = isEnablePolygonOffset ? 
                        ( ( depthRangeBit == 24 ) ?
                          // 24 bit float の精度限界により誤差で消えてしまうのを避けるため、
                          // 0.5f 付近の深度値を基準として、128.0f をかけておきます。
                          Float32ToFloat24( ( polygonOffsetUnit * 128.0f / f32( (1 << depthRangeBit) - 1 ) ) ):
                          Float32ToFloat24( ( polygonOffsetUnit / f32( (1 << depthRangeBit) - 1 ) ) ) )
                        : 0.0f;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_WSCALE_DATA2 );
                }

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::FBAccess::MakeCommand( bit32* command, bool isClearFrameBufferCache ) const
            {
                if ( isClearFrameBufferCache )
                {
                    // 0x111
                    *command++ = 0x1;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1 );

                    // 0x110
                    *command++ = 0x1;
                    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0 );
                }

                if ( m_RenderState.shadowMap.isEnable )
                {
                    // 0x112 カラーバッファの READ を有効
                    *command++ = 0xf;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_BUFFER_READ, 0x1 );

                    // 0x113 カラーバッファの WRITE を有効
                    *command++ = 0xf;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_BUFFER_WRITE, 0x1 );

                    // 0x114 デプス・ステンシルバッファの READ を無効
                    *command++ = 0x0;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DEPTH_STENCIL_BUFFER_READ, 0x1 );

                    // 0x115 デプス・ステンシルバッファの WRITE を無効
                    *command++ = 0x0;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DEPTH_STENCIL_BUFFER_WRITE, 0x1 );
                }
                else
                {
                    // 0x112
                    *command++ = ( (m_RenderState.colorMask && m_RenderState.colorMask != 0xf) ||
                                   (m_RenderState.colorMask && m_RenderState.blend.isEnable && m_RenderState.blend.IsDstColorUsed()) ||
                                   (m_RenderState.colorMask && m_RenderState.logicOp.isEnable && m_RenderState.logicOp.IsDstColorUsed()) ) ? 0xf : 0;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_BUFFER_READ, 0x1 );

                    // 0x113
                    *command++ = m_RenderState.colorMask ? 0xf : 0;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_BUFFER_WRITE, 0x1 );

                    bit32 depth_stencil_read  = 0;
                    bit32 depth_stencil_write = 0;

                    if ( m_RenderState.depthTest.isEnable )
                    {
                        if ( m_RenderState.depthTest.isEnableWrite )
                        {
                            depth_stencil_read  |= 2;
                            depth_stencil_write |= 2;
                        }
                        else if ( m_RenderState.colorMask )
                        {
                            depth_stencil_read  |= 2;
                        }
                    }

                    if ( m_RenderState.stencilTest.isEnable )
                    {
                        if (  m_RenderState.stencilTest.maskOp != 0 )
                        {
                            depth_stencil_read  |= 1;
                            depth_stencil_write |= 1;
                        }
                        else if ( m_RenderState.colorMask )
                        {
                            depth_stencil_read  |= 1;
                        }
                    }

                    // 0x114
                    *command++ = depth_stencil_read;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DEPTH_STENCIL_BUFFER_READ,  0x1 );

                    // 0x115
                    *command++ = depth_stencil_write;
                    *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DEPTH_STENCIL_BUFFER_WRITE, 0x1 );
                }

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::FBAccess::MakeDisableCommand( bit32* command, bool isClearFrameBufferCache )
            {
                if ( isClearFrameBufferCache )
                {
                    command = MakeClearCacheCommand( command );
                }

                // 0x112 : Color read
                *command++ = 0xf;
                *command++ = PICA_CMD_HEADER_BURSTSEQ_BE( PICA_REG_COLOR_BUFFER_READ, 0x4, 0x1 );

                // 0x113 : Color write
                *command++ = 0xf;
                // 0x114 : Depth/Stencil read
                *command++ = 0x0;

                // 0x115 : Depth/Stencil write
                *command++ = 0x0;
                // padding
                *command++ = 0x0;

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::FBAccess::MakeClearCacheCommand( bit32* command )
            {
                // 0x111
                *command++ = 0x1;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1 );

                // 0x110
                *command++ = 0x1;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0 );

                return command;
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::MakeCommand( bit32* buffer, bool isClearFrameBufferCache ) const
            {
                bit32* command = buffer;

                command = cullingTest.MakeCommand( command, false );
                command = blend.MakeCommand( command, false );
                command = logicOp.MakeCommand( command, false );
                command = shadowMap.MakeCommand( command, false );
                command = alphaTest.MakeCommand( command, false );
                command = stencilTest.MakeCommand( command, false );
                command = depthTest.MakeCommand( command, false );
                command = wBuffer.MakeCommand( command );
                command = fbAccess.MakeCommand( command, isClearFrameBufferCache );

                return command;
            }

            //------------------------------------------------------------------------------

            RenderState::Culling::Culling( const RenderState& renderState_ )
                : isEnable( true ),
                  frontFace( FRONT_FACE_CCW ),
                  cullFace(  CULL_FACE_BACK ),
                  m_RenderState( renderState_ )
            {
            }

            //------------------------------------------------------------------------------

            RenderState::Blend::Blend( const RenderState& renderState_ )
                : isEnable( false ),
                  eqRgb   ( PICA_DATA_BLEND_EQUATION_ADD ),
                  eqAlpha ( PICA_DATA_BLEND_EQUATION_ADD ),
                  srcRgb  ( PICA_DATA_BLEND_FUNC_SRC_ALPHA ),
                  srcAlpha( PICA_DATA_BLEND_FUNC_SRC_ALPHA ),
                  dstRgb  ( PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA ),
                  dstAlpha( PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA ),
                  colorR( 0xff ),
                  colorG( 0xff ),
                  colorB( 0xff ),
                  colorA( 0xff ),
                  m_RenderState( renderState_ )
            {
            }

            //------------------------------------------------------------------------------

            RenderState::LogicOp::LogicOp( const RenderState& renderState_ )
                : isEnable( false ),
                  opCode( PICA_DATA_LOGIC_NOOP ),
                  m_RenderState( renderState_ )
            {
            }

            //------------------------------------------------------------------------------

            RenderState::ShadowMap::ShadowMap( const RenderState& renderState_ )
                : isEnable( false ),
                  isPerspective( true ),
                  zBias( 0.0f ),
                  zScale( 1.0f ),
                  penumbraScale( 0.0f ),
                  penumbraBias( 1.0f ),
                  m_RenderState( renderState_ )
            {
            }

            //------------------------------------------------------------------------------

            RenderState::AlphaTest::AlphaTest( const RenderState& renderState_ )
                : isEnable( false ),
                  refValue( 0 ),
                  func ( PICA_DATA_ALPHA_TEST_NEVER ),
                  m_RenderState( renderState_ )
            {
            }

            //------------------------------------------------------------------------------

            RenderState::StencilTest::StencilTest( const RenderState& renderState_ )
                : isEnable     ( false ),
                  maskOp       ( 0xff ),
                  func         ( PICA_DATA_STENCIL_TEST_ALWAYS ),
                  ref          ( 0 ),
                  mask         ( 0xff ),
                  opFail       ( PICA_DATA_STENCIL_OP_KEEP ),
                  opZFail      ( PICA_DATA_STENCIL_OP_KEEP ),
                  opZPass      ( PICA_DATA_STENCIL_OP_KEEP ),
                  m_RenderState( renderState_ )
            {
            }

            //------------------------------------------------------------------------------

            RenderState::DepthTest::DepthTest( const RenderState& renderState_ )
                : isEnable( true ),
                  isEnableWrite( true ),
                  func( PICA_DATA_DEPTH_TEST_LESS ),
                  m_RenderState( renderState_ )
            {
            }

            //------------------------------------------------------------------------------

            RenderState::WBuffer::WBuffer( )
                : wScale( 0.f ),
                  isEnablePolygonOffset( false ),
                  polygonOffsetUnit    ( 0.f ),
                  depthRangeNear       ( 0.0f ),
                  depthRangeFar        ( 1.0f ),
                  depthRangeBit        ( 24 )
            {
            }

            //------------------------------------------------------------------------------

            RenderState::FBAccess::FBAccess( const RenderState& renderState_ )
                : m_RenderState( renderState_ )
            {
            }

            //------------------------------------------------------------------------------

            RenderState::RenderState() :
                  blend       ( *this ),
                  logicOp     ( *this ),
                  shadowMap   ( *this ),
                  alphaTest   ( *this ),
                  stencilTest ( *this ),
                  colorMask   ( COLOR_MASK_RGBA ),
                  depthTest   ( *this ),
                  cullingTest ( *this ),
                  wBuffer     ( ),
                  fbAccess    ( *this )
            {
            }

            //------------------------------------------------------------------------------

            bit32* RenderState::MakeDisableCommand( bit32* command, bool isClearFrameBufferCache )
            {
                command = Culling::MakeDisableCommand( command, false );
                command = Blend::MakeDisableCommand( command, false );
                command = AlphaTest::MakeDisableCommand( command, false );
                command = StencilTest::MakeDisableCommand( command, false );
                command = DepthTest::MakeDisableCommand( command, false );
                command = FBAccess::MakeDisableCommand( command, isClearFrameBufferCache );

                return command;
            }

        } // namespace CTR
    } // namespace gr
} // namespace nn
