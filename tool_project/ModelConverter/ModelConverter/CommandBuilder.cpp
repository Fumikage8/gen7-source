#include "Commandbuilder.h"

#define __gl2_h_
//#include <GLES2/gl2.h>のインクルードを抑止
#include <gl/glew.h>
#include <gl/gl.h>

#include <nn/gx/CTR/gx_MacroCommon.h>

u32* Commandbuilder::MakeUniformCommandVS( u32* command, u8 location, const gfl2::math::Vector4& vec4 )
{
  // 0x2c0
  *command++ = 0x80000000 | location;
  *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_VS_FLOAT_ADDR, 5 );
  // 0x2c1-0x2c4
  *command++ = F32AsU32( vec4.w );
  *command++ = F32AsU32( vec4.z );
  *command++ = F32AsU32( vec4.y );
  *command++ = F32AsU32( vec4.x );
  return command;
}

u32* Commandbuilder::CopyMtx44WithHeader( f32* dst, gfl2::math::Matrix* src, u32 size, u32 header )
{
  u32 outCnt = 0;

  for( u32 i = 0; i < size; ++i )
  {
    gfl2::math::Matrix outMat = src[i];

    for( u32 col = 0; col < 4; ++col )
    {
      for( s32 row = 3; row >= 0; --row )
      {
        if ( outCnt == 1 )
        {
          u32 *pU32 = reinterpret_cast<u32*>( &dst[outCnt] );
          *pU32 = header;
          ++outCnt;
        }

        dst[outCnt] = outMat[col][row];
        ++outCnt;
      }
    }
  }

  return reinterpret_cast<u32*>( &dst[outCnt] );
}

u32* Commandbuilder::CopyMtx34WithHeader( f32* dst, gfl2::math::Matrix* src, u32 size, u32 header )
{
  u32 outCnt = 0;

  for( u32 i = 0; i < size; ++i )
  {
    gfl2::math::Matrix outMat = src[i];

    for( u32 col = 0; col < 3; ++col )
    {
      for( s32 row = 3; row >= 0; --row )
      {
        if ( outCnt == 1 )
        {
          u32 *pU32 = reinterpret_cast<u32*>( &dst[outCnt] );
          *pU32 = header;
          ++outCnt;
        }

        dst[outCnt] = outMat[col][row];
        ++outCnt;
      }
    }
  }

  dst[outCnt] = 0;
  ++outCnt;//パディング

  return reinterpret_cast<u32*>( &dst[outCnt] );

  /*
  command   3.0000000   0x0FFFFE98  float[18]
  [0x0] 3.0000000   0x0FFFFE98  float
  [0x1] 2.3419039e-038  0x0FFFFE9C  float 0x00FF02C1
  [0x2] 2.0000000   0x0FFFFEA0  float
  [0x3] 1.0000000   0x0FFFFEA4  float

  [0x4] 0.00000000  0x0FFFFEA8  float
  [0x5] 7.0000000   0x0FFFFEAC  float
  [0x6] 6.0000000   0x0FFFFEB0  float
  [0x7] 5.0000000   0x0FFFFEB4  float

  [0x8] 4.0000000   0x0FFFFEB8  float
  [0x9] 11.000000   0x0FFFFEBC  float
  [0xA] 10.000000   0x0FFFFEC0  float
  [0xB] 9.0000000   0x0FFFFEC4  float

  [0xC] 8.0000000   0x0FFFFEC8  float
  [0xD] 15.000000   0x0FFFFECC  float
  [0xE] 14.000000   0x0FFFFED0  float
  [0xF] 13.000000   0x0FFFFED4  float

  [0x10]    12.000000   0x0FFFFED8  float
  [0x11]    3.8519407e-034  0x0FFFFEDC  float   ダミー

  */
}

u32* Commandbuilder::MakeUniformCommandVSMat44( u32* command, u8 location, gfl2::math::Matrix *mtx44, u32 size )
{
  // 浮動小数点数レジスタへの書き込み用コマンド
  // PICA_REG_VS_FLOAT0 0x2c1
  //#define PICA_CMD_HEADER_VS_F32( size )           PICA_CMD_HEADER(PICA_REG_VS_FLOAT0, (size) * 4 -1, 0xf, 0)

  *command++ = 0x80000000 | location;
  *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_FLOAT_ADDR );
  return CopyMtx44WithHeader( reinterpret_cast<f32*>( command ), mtx44, size, PICA_CMD_HEADER_VS_F32( size * 4 ) );
}

u32* Commandbuilder::MakeUniformCommandVSMat34( u32* command, u8 location, gfl2::math::Matrix *mtx44, u32 size )
{
  // 浮動小数点数レジスタへの書き込み用コマンド
  // PICA_REG_VS_FLOAT0 0x2c1
  //#define PICA_CMD_HEADER_VS_F32( size )           PICA_CMD_HEADER(PICA_REG_VS_FLOAT0, (size) * 4 -1, 0xf, 0)

  *command++ = 0x80000000 | location;
  *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_FLOAT_ADDR );
  return CopyMtx34WithHeader( reinterpret_cast<f32*>( command ), mtx44, size, PICA_CMD_HEADER_VS_F32( size * 3 ) );
}

u32* Commandbuilder::MakeUniformCommandVSTextureMatrix( u32* command, u8 location, gfl2::math::Matrix *mtx44, u32 size )
{
  *command++ = 0x80000000 | location;
  *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_FLOAT_ADDR );
  return CopyMtx34WithHeader( reinterpret_cast<f32*>( command ), mtx44, size, PICA_CMD_HEADER_VS_F32( size * 3 ) );
}





