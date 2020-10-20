/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Combiner.cpp

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#include "gr_Combiner.h"

#include <nn/gx/CTR/gx_MacroShader.h>
#include <nn/gx/CTR/gx_MacroTexture.h>

namespace nn
{
    namespace gr
    {
        namespace CTR
        {

            bit32* Combiner::MakeCommand( bit32* command ) const
            {
                for ( int stage_index = 0; stage_index < COMBINER_STAGE_MAX; ++ stage_index )
                {
                    command = stage[ stage_index ].MakeCommand( command );
                }
                
                return MakeCombinerBufferCommand( command );
            }
            
            //------------------------------------------------------------------------------

            bit32* Combiner::MakeCombinerBufferCommand( bit32* command ) const
            {
                // 0x0e0
                *command++ =PICA_CMD_SET_TEX_ENV_BUFFER_INPUT(
                    stage[ 1 ].rgb.bufferInput, stage[ 1 ].alpha.bufferInput,
                    stage[ 2 ].rgb.bufferInput, stage[ 2 ].alpha.bufferInput,
                    stage[ 3 ].rgb.bufferInput, stage[ 3 ].alpha.bufferInput,
                    stage[ 4 ].rgb.bufferInput, stage[ 4 ].alpha.bufferInput );
                *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_TEX_ENV_BUFFER_INPUT, 0x2 );

                // 0x0fd
                *command++ = bufferColorR | bufferColorG << 8 | bufferColorB << 16 | bufferColorA << 24;
                *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEX_ENV_BUFFER_COLOR );
                return command;
            }

            //------------------------------------------------------------------------------

            bit32* Combiner::Stage::MakeCommand( bit32* command ) const
            {
//                NN_GR_ASSERT( PICA_REG_TEX_ENV0 <= headRegister &&
                              //PICA_REG_TEX_ENV5 >= headRegister );

                *command++ = PICA_CMD_DATA_TEX_ENV_SRC(
                    rgb.source[0],   rgb.source[1],   rgb.source[2],
                    alpha.source[0], alpha.source[1], alpha.source[2] );

                *command++ = PICA_CMD_HEADER_BURSTSEQ(
                    headRegister,
                    5 );

                *command++ = PICA_CMD_DATA_TEX_ENV_OPERAND(
                    rgb.operand[0],   rgb.operand[1],   rgb.operand[2],
                    alpha.operand[0], alpha.operand[1], alpha.operand[2] );

                *command++ = PICA_CMD_DATA_TEX_ENV_COMBINE(
                    rgb.combine,
                    alpha.combine );

                *command++ = PICA_CMD_DATA_TEX_ENV_CONST(
                    constColorR, constColorG, constColorB,
                    constColorA );
                
                *command++ = PICA_CMD_DATA_TEX_ENV_SCALE(
                    rgb.scale,
                    alpha.scale );
                
                return command;
            }
           
            //------------------------------------------------------------------------------

            void Combiner::Stage::SetupPrimary()
            {
                rgb.combine       = PICA_DATA_TEX_ENV_COMBINE_REPLACE;
                rgb.operand[0]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[1]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[2]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.source[0]     = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
                rgb.source[1]     = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
                rgb.source[2]     = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
                rgb.scale         = PICA_DATA_TEX_ENV_SCALE_1;
                rgb.bufferInput   = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
                
                alpha.combine     = PICA_DATA_TEX_ENV_COMBINE_REPLACE;
                alpha.operand[0]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[1]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[2]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.source[0]   = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
                alpha.source[1]   = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
                alpha.source[2]   = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
                alpha.scale       = PICA_DATA_TEX_ENV_SCALE_1;
                alpha.bufferInput = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
            }

            //------------------------------------------------------------------------------
            
            void Combiner::Stage::SetupFragmentPrimary()
            {
                rgb.combine       = PICA_DATA_TEX_ENV_COMBINE_REPLACE;
                rgb.operand[0]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[1]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[2]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.source[0]     = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP;
                rgb.source[1]     = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP;
                rgb.source[2]     = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP;
                rgb.scale         = PICA_DATA_TEX_ENV_SCALE_1;
                rgb.bufferInput   = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
                
                alpha.combine     = PICA_DATA_TEX_ENV_COMBINE_REPLACE;
                alpha.operand[0]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[1]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[2]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.source[0]   = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP;
                alpha.source[1]   = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP;
                alpha.source[2]   = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP;
                alpha.scale       = PICA_DATA_TEX_ENV_SCALE_1;
                alpha.bufferInput = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
            }
            
            void Combiner::Stage::SetupTexture0()
            {
                rgb.combine       = PICA_DATA_TEX_ENV_COMBINE_REPLACE;
                rgb.operand[0]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[1]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[2]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.source[0]     = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                rgb.source[1]     = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                rgb.source[2]     = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                rgb.scale         = PICA_DATA_TEX_ENV_SCALE_1;
                rgb.bufferInput   = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
                
                alpha.combine     = PICA_DATA_TEX_ENV_COMBINE_REPLACE;
                alpha.operand[0]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[1]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[2]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.source[0]   = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                alpha.source[1]   = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                alpha.source[2]   = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                alpha.scale       = PICA_DATA_TEX_ENV_SCALE_1;
                alpha.bufferInput = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
            }

            //------------------------------------------------------------------------------

            void Combiner::Stage::SetupPrevious()
            {
                rgb.combine       = PICA_DATA_TEX_ENV_COMBINE_REPLACE;
                rgb.operand[0]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[1]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[2]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.source[0]     = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                rgb.source[1]     = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                rgb.source[2]     = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                rgb.scale         = PICA_DATA_TEX_ENV_SCALE_1;
                rgb.bufferInput   = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
                
                alpha.combine     = PICA_DATA_TEX_ENV_COMBINE_REPLACE;
                alpha.operand[0]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[1]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[2]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.source[0]   = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                alpha.source[1]   = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                alpha.source[2]   = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                alpha.scale       = PICA_DATA_TEX_ENV_SCALE_1;
                alpha.bufferInput = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
            }

            //------------------------------------------------------------------------------

            void Combiner::Stage::SetupPrimaryModulateTexture0()
            {
                rgb.combine       = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
                rgb.operand[0]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[1]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[2]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.source[0]     = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
                rgb.source[1]     = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                rgb.source[2]     = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                rgb.scale         = PICA_DATA_TEX_ENV_SCALE_1;
                rgb.bufferInput   = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
                
                alpha.combine     = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
                alpha.operand[0]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[1]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[2]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.source[0]   = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
                alpha.source[1]   = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                alpha.source[2]   = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                alpha.scale       = PICA_DATA_TEX_ENV_SCALE_1;
                alpha.bufferInput = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
            }

            //------------------------------------------------------------------------------

            void Combiner::Stage::SetupFragmentPrimaryModulateTexture0()
            {
                rgb.combine       = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
                rgb.operand[0]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[1]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.operand[2]    = PICA_DATA_OPE_RGB_SRC_COLOR;
                rgb.source[0]     = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP;
                rgb.source[1]     = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                rgb.source[2]     = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                rgb.scale         = PICA_DATA_TEX_ENV_SCALE_1;
                rgb.bufferInput   = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
                
                alpha.combine     = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
                alpha.operand[0]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[1]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.operand[2]  = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
                alpha.source[0]   = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP;
                alpha.source[1]   = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
                alpha.source[2]   = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                alpha.scale       = PICA_DATA_TEX_ENV_SCALE_1;
                alpha.bufferInput = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP;
            }

            //------------------------------------------------------------------------------

            Combiner::Combiner()
                : bufferColorR( 0 ),
                  bufferColorG( 0 ),
                  bufferColorB( 0 ),
                  bufferColorA( 0 )
            {
                for ( int stage_index = 0; stage_index < COMBINER_STAGE_MAX; ++stage_index )
                {
                    stage[ stage_index ] = Stage( stage_index );
                }
            }

            //------------------------------------------------------------------------------

            Combiner::Stage::Stage( const int stage_index )
                : rgb  ( stage_index, true ),
                  alpha( stage_index, false ),
                  constColorR( 0 ),
                  constColorG( 0 ),
                  constColorB( 0 ),
                  constColorA( 0 )
            {
                switch( stage_index )
                {
                case 0 : headRegister = PICA_REG_TEX_ENV0; break;
                case 1 : headRegister = PICA_REG_TEX_ENV1; break;
                case 2 : headRegister = PICA_REG_TEX_ENV2; break;
                case 3 : headRegister = PICA_REG_TEX_ENV3; break;
                case 4 : headRegister = PICA_REG_TEX_ENV4; break;
                case 5 : headRegister = PICA_REG_TEX_ENV5; break;
                }
            }
            //------------------------------------------------------------------------------

            Combiner::Stage::CombineFunction::CombineFunction( const u8 stage_index, bool is_rgb )
                : combine( PICA_DATA_TEX_ENV_COMBINE_REPLACE ),
                  scale( PICA_DATA_TEX_ENV_SCALE_1 ),
                  bufferInput( PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP )
            {
                operand[ 0 ] = operand[ 1 ] = operand[ 2 ] =
                    ( is_rgb )
                        ? PICA_DATA_OPE_RGB_SRC_COLOR
                        : PICA_DATA_OPE_ALPHA_SRC_ALPHA;

                source[ 0 ] = source[ 1 ] = source[ 2 ] =
                    ( stage_index == 0 )
                        ? PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT
                        : PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
            }

        } // namespace CTR
    } // namespace gr
} // namespace nn
