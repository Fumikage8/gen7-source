#ifndef COMMAND_BUILDER_H_INCLUDED
#define COMMAND_BUILDER_H_INCLUDED


#include <stdio.h>
#include <string>
#include <memory.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

union CommandHeader{
  CommandHeader():m_DataU32(0){}

  struct
  {
    u32 addr    : 16;
    u32 be      : 4;
    u32 size    : 8;
    u32 padding : 3;
    u32 seq     : 1;
  };

  struct
  {
    u32 m_DataU32;
  };
};

class Commandbuilder{
public:

  static inline u32 F32AsU32(f32 x)
  {
    return *reinterpret_cast<u32*>(&x);
  }

  static u32* MakeUniformCommandVS( u32* command, u8 location, const gfl2::math::Vector4& vec4 );

  static u32* CopyMtx44WithHeader( f32* dst, gfl2::math::Matrix* src, u32 size, u32 header );
  static u32* CopyMtx34WithHeader( f32* dst, gfl2::math::Matrix* src, u32 size, u32 header );

  static u32* MakeUniformCommandVSMat44( u32* command, u8 location, gfl2::math::Matrix *mtx44, u32 size );
  static u32* MakeUniformCommandVSMat34( u32* command, u8 location, gfl2::math::Matrix *mtx44, u32 size );
  static u32* MakeUniformCommandVSTextureMatrix( u32* command, u8 location, gfl2::math::Matrix *mtx44, u32 size );
};


#endif


