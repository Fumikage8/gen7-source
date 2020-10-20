#if !defined( __GFL_G3DH3DCOMMANDUTIL_H__ )
#define __GFL_G3DH3DCOMMANDUTIL_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dCommandUtil.h
 *	@brief  H3D���[�e�B���e�B
 *	@author	Koji Kawada
 *	@date		2012.07.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
//����
//����̓R�}���h�ɓ�������H3dUtil�ł��B
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>


#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D�R�}���h���X�i�[
//=====================================
class H3dCommandListener
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dCommandListener);
public:
  enum Access 
  {
    SINGLE_ACCESS,
    BURST_ACCESS
  };
public:
  H3dCommandListener(void) {}
  virtual ~H3dCommandListener() {}
public:
  virtual void OnReadHeader(u32 index, bit32 command, u32 addr, u32 be, u32 size, u32 seq, Access access) {}
  virtual bool OnReadData(u32 index, bit32 command, u32 addr, u32 mask, u32 count, u32 count_max, Access access) { return false; }
  virtual void OnReadIgnore(u32 index, bit32 command) {}
  virtual void OnReadError(u32 index, bit32 command) {}
};

class H3dCommandListenerDump : public H3dCommandListener
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dCommandListenerDump);
public:
  static const char* STR_SINGLE_ACCESS;
  static const char* STR_BURST_ACCESS;
public:
  H3dCommandListenerDump(void) {}
  virtual ~H3dCommandListenerDump() {}
public:
  virtual void OnReadHeader(u32 index, bit32 command, u32 addr, u32 be, u32 size, u32 seq, Access access)
  {
    GFL_PRINT("[%2d](0x%X) [HEADER] %s ADDR=0x%X, BE=0x%X, SIZE=%d, SEQ=%d\n", index, command,
        (access==SINGLE_ACCESS)?STR_SINGLE_ACCESS:STR_BURST_ACCESS,
        addr, be, size, seq);
  }
  virtual bool OnReadData(u32 index, bit32 command, u32 addr, u32 mask, u32 count, u32 count_max, Access access)
  {
    GFL_PRINT("[%2d](0x%X) [DATA] %s ADDR=0x%X, MASK=0x%X, count=%d/%d\n", index, command,
        (access==SINGLE_ACCESS)?STR_SINGLE_ACCESS:STR_BURST_ACCESS,
        addr, mask, count, count_max);

    return false;
  }
  virtual void OnReadIgnore(u32 index, bit32 command)
  {
    GFL_PRINT("[%2d](0x%X) [IGNORE]\n", index, command);
  }
  virtual void OnReadError(u32 index, bit32 command)
  {
    GFL_PRINT("[%2d](0x%X) [ERROR]\n", index, command);
  }
};
const char* H3dCommandListenerDump::STR_SINGLE_ACCESS = "SINGLE_ACCESS";
const char* H3dCommandListenerDump::STR_BURST_ACCESS  = "BURST_ACCESS";




//-------------------------------------
///	H3D�R�}���h���[�e�B���e�B
//=====================================
class H3dCommandUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dCommandUtil);

public:
  // @brief  �R���X�g���N�^
  H3dCommandUtil(void) {}
  // @brief  �f�X�g���N�^
  virtual ~H3dCommandUtil() {}

public:
  // @brief  ���W�X�^
  static inline u32 GetTextureCombinerCombineRegister(s32 step)
  {
    static const u32 REG[6] =
    {
      PICA_REG_TEX_ENV0_COMBINE,
      PICA_REG_TEX_ENV1_COMBINE,
      PICA_REG_TEX_ENV2_COMBINE,
      PICA_REG_TEX_ENV3_COMBINE,
      PICA_REG_TEX_ENV4_COMBINE,
      PICA_REG_TEX_ENV5_COMBINE,
    };
    return REG[step];
  }
  static inline u32 GetTextureCombinerScaleRegister(s32 step)
  {
    static const u32 REG[6] =
    {
      PICA_REG_TEX_ENV0_SCALE,
      PICA_REG_TEX_ENV1_SCALE,
      PICA_REG_TEX_ENV2_SCALE,
      PICA_REG_TEX_ENV3_SCALE,
      PICA_REG_TEX_ENV4_SCALE,
      PICA_REG_TEX_ENV5_SCALE,
    };
    return REG[step];
  }
  static inline u32 GetTextureCombinerOperandRegister(s32 step)
  {
    static const u32 REG[6] =
    {
      PICA_REG_TEX_ENV0_OPERAND,
      PICA_REG_TEX_ENV1_OPERAND,
      PICA_REG_TEX_ENV2_OPERAND,
      PICA_REG_TEX_ENV3_OPERAND,
      PICA_REG_TEX_ENV4_OPERAND,
      PICA_REG_TEX_ENV5_OPERAND,
    };
    return REG[step];
  }
  static inline u32 GetTextureCombinerSourceRegister(s32 step)
  {
    static const u32 REG[6] =
    {
      PICA_REG_TEX_ENV0,
      PICA_REG_TEX_ENV1,
      PICA_REG_TEX_ENV2,
      PICA_REG_TEX_ENV3,
      PICA_REG_TEX_ENV4,
      PICA_REG_TEX_ENV5,
    };
    return REG[step];
  }
  static inline u32 GetTextureSamplerWrapFilterRegister(s32 no)
  {
    static const u32 REG[3] =
    {
      PICA_REG_TEXTURE0_WRAP_FILTER,
      PICA_REG_TEXTURE1_WRAP_FILTER,
      PICA_REG_TEXTURE2_WRAP_FILTER,
    };
    return REG[no];
  }

  // @brief  �R�}���h
  static inline bit32* MakeTextureCombinerCombineRgbCommand(bit32* cmd, s32 step, PicaDataTexEnvCombine combine)
  {
    // NintendoWare/CTR/include/nw/h3d/fnd/h3d_Util.h
    // static NW_INLINE bit32*
    //     MakeCombinerConstantCommand(bit32* cmd, u16 headRegister, ut::Color8 color)

    // @note �R�}���h�𕪂���ƁA�����A�h���X�ɑ΂���2��R�}���h�𔭍s���邱�ƂɂȂ��Ă��܂��B�����Ȃ��ق����������B
    static const u32 BE = 0x3;  // 0011
    *cmd++ = PICA_CMD_DATA_TEX_ENV_COMBINE(combine, 0);
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( GetTextureCombinerCombineRegister(step), BE );
    return cmd;
  }
  static inline bit32* MakeTextureCombinerCombineAlphaCommand(bit32* cmd, s32 step, PicaDataTexEnvCombine combine)
  {
    // @note �R�}���h�𕪂���ƁA�����A�h���X�ɑ΂���2��R�}���h�𔭍s���邱�ƂɂȂ��Ă��܂��B�����Ȃ��ق����������B
    static const u32 BE = 0xC;  // 1100
    *cmd++ = PICA_CMD_DATA_TEX_ENV_COMBINE(0, combine);
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( GetTextureCombinerCombineRegister(step), BE );
    return cmd;
  }
  static inline bit32* MakeTextureCombinerScaleRgbCommand(bit32* cmd, s32 step, PicaDataTexEnvScale scale)
  {
    // @note �R�}���h�𕪂���ƁA�����A�h���X�ɑ΂���2��R�}���h�𔭍s���邱�ƂɂȂ��Ă��܂��B�����Ȃ��ق����������B
    static const u32 BE = 0x3;  // 0011
    *cmd++ = PICA_CMD_DATA_TEX_ENV_SCALE(scale, 0);
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( GetTextureCombinerScaleRegister(step), BE );
    return cmd;
  }
  static inline bit32* MakeTextureCombinerScaleAlphaCommand(bit32* cmd, s32 step, PicaDataTexEnvScale scale)
  {
    // @note �R�}���h�𕪂���ƁA�����A�h���X�ɑ΂���2��R�}���h�𔭍s���邱�ƂɂȂ��Ă��܂��B�����Ȃ��ق����������B
    static const u32 BE = 0xC;  // 1100
    *cmd++ = PICA_CMD_DATA_TEX_ENV_SCALE(0, scale);
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( GetTextureCombinerScaleRegister(step), BE );
    return cmd;
  }
  static inline bit32* MakeTextureCombinerOperandCommand(bit32* cmd, s32 step,
      PicaDataTexEnvOperand ope_rgb[3], PicaDataTexEnvOperand ope_alpha[3])
  {
    static const u32 BE = 0x7;  // 0111  // �urgb0&rgb1�v�Ɓurgb2&alpha0�v�Ɓualpha1&alpha2�v�ɕ����邱�Ƃ��ł��邪�A�S���ЂƂ܂Ƃ߂ɂ���B
    *cmd++ = PICA_CMD_DATA_TEX_ENV_OPERAND( \
        ope_rgb[0],   ope_rgb[1],   ope_rgb[2],  \
        ope_alpha[0], ope_alpha[1], ope_alpha[2] \
    );
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( GetTextureCombinerOperandRegister(step), BE );
    return cmd;
  }
  static inline bit32* MakeTextureCombinerSourceCommand(bit32* cmd, s32 step,
      PicaDataTexEnvSrc source_rgb[3], PicaDataTexEnvSrc source_alpha[3])
  {
    static const u32 BE = 0xF;  // 1111  // �urgb0&rgb1�v�Ɓurgb2�v�Ɓualpha0&alpha1�v�Ɓualpha2�v�ɕ����邱�Ƃ��ł��邪�A�S���ЂƂ܂Ƃ߂ɂ���B
    *cmd++ = PICA_CMD_DATA_TEX_ENV_SRC( \
        source_rgb[0],   source_rgb[1],   source_rgb[2],  \
        source_alpha[0], source_alpha[1], source_alpha[2] \
    );
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( GetTextureCombinerSourceRegister(step), BE );
    return cmd;
  }

  static inline bit32* MakeFragmentShaderBufferInputCommand(bit32* cmd,
      PicaDataTexEnvBufferInput input_rgb[4], PicaDataTexEnvBufferInput input_alpha[4])
  {
    static const u32 BE = 0x2;  // 0010
    *cmd++ = ( \
          PICA_CMD_DATA_TEX_ENV_BUFFER_INPUT(1, input_rgb[0], input_alpha[0]) \
        | PICA_CMD_DATA_TEX_ENV_BUFFER_INPUT(2, input_rgb[1], input_alpha[1]) \
        | PICA_CMD_DATA_TEX_ENV_BUFFER_INPUT(3, input_rgb[2], input_alpha[2]) \
        | PICA_CMD_DATA_TEX_ENV_BUFFER_INPUT(4, input_rgb[3], input_alpha[3]) \
    );
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_TEX_ENV_BUF_INPUT, BE );
    return cmd;
  }

  static inline bit32* MakeStencilOperationTestCommand(bit32* cmd, u8 enable, PicaDataStencilTest func_func, u8 mask, u8 func_ref, u8 func_mask)
  {
    // @note �R�}���h�𕪂����邪�����Ă��Ȃ��B�����Ă����΁A���Ƃ��Ƃ̒l���o���Ă����Ȃ��Ă��㏑�������ŉ��Ƃ��ł������B
    static const u32 BE = 0xF;  // 1111
    *cmd++ = PICA_CMD_DATA_STENCIL_TEST(enable, func_func, mask, func_ref, func_mask);
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_STENCIL_TEST, BE );
    return cmd;
  }
  
  static inline bit32* MakeStencilOperationOpCommand(bit32* cmd, PicaDataStencilOp op_fail, PicaDataStencilOp op_zfail, PicaDataStencilOp op_zpass)
  {
    // @note BE�ݒ肵�Ȃ��Ă����Ƃ��́A���Ȃ��ق������������肷��̂��H
    static const u32 BE = 0x3;  // 0011
    *cmd++ = PICA_CMD_DATA_STENCIL_OP(op_fail, op_zfail, op_zpass);
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_STENCIL_OP, BE );
    return cmd;
  }

  static inline bit32* MakeAlphaTestCommand(bit32* cmd, u8 enable, PicaDataAlphaTest func, u8 ref)
  {
    // @note �R�}���h�𕪂����邪�����Ă��Ȃ��B�����Ă����΁A���Ƃ��Ƃ̒l���o���Ă����Ȃ��Ă��㏑�������ŉ��Ƃ��ł������B
    static const u32 BE = 0x3;  // 0011
    *cmd++ = PICA_CMD_DATA_FRAGOP_ALPHA_TEST(enable, func, ref);
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_FRAGOP_ALPHA_TEST, BE );
    return cmd;
  }

  static inline bit32* MakeDepthTestCommand(bit32* cmd, u8 enable, PicaDataDepthTest func)
  {
    // @note �R�}���h��BE�ŕ������������Ȃ��ق����������Ȃ��H
    static const u32 BE = 0x1;  // 0001
    *cmd++ = PICA_CMD_DATA_DEPTH_COLOR_MASK(enable, func, 1, 1, 1, 1, 1);  // �f�v�X�J���[�}�X�N�̂Ƃ���������Ă��邪�A
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DEPTH_COLOR_MASK, BE );   // ����Ɋւ��Ă�BE�Ń}�X�N���Ă���̂ŏ�����Ȃ��B
    return cmd;
  }
      
  static inline bit32* MakeDepthColorMaskCommand(bit32* cmd, u8 red_mask, u8 green_mask, u8 blue_mask, u8 alpha_mask, u8 depth_mask)  // 1�̂Ƃ��X�V����
  {
    // @note �R�}���h��BE�ŕ������������Ȃ��ق����������Ȃ��H
    static const u32 BE = 0x2;  // 0010
    *cmd++ = PICA_CMD_DATA_DEPTH_COLOR_MASK(1, PICA_DATA_DEPTH_TEST_NEVER, red_mask, green_mask, blue_mask, alpha_mask, depth_mask);  // �f�v�X�e�X�g�L�������ƃf�v�X�e�X�g�֐��̂Ƃ���������Ă��邪�A
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DEPTH_COLOR_MASK, BE );                                                              // ����Ɋւ��Ă�BE�Ń}�X�N���Ă���̂ŏ�����Ȃ��B
    return cmd;
  }
 
  static inline bit32* MakeBlendModeCommand(bit32* cmd, u8 mode_is_blend)  // LOGIC(�_�����Z)�̂Ƃ�0�ABLEND(�u�����h)�̂Ƃ�1
  {
    // @note �u�����h�̃R�}���h���ׂ����������������Ȃ��ق����������Ȃ��H
    static const u32 BE = 0x2;  // 0010
    *cmd++ = ( (mode_is_blend) << 8 );
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_COLOR_OPERATION, BE );
    return cmd;
  }

  static inline bit32* MakeBlendFunctionCommand(
      bit32*  cmd,
      PicaDataBlendEquation  equation_rgb,
      PicaDataBlendFunc      function_src_rgb,
      PicaDataBlendFunc      function_dst_rgb,
      PicaDataBlendEquation  equation_alpha,
      PicaDataBlendFunc      function_src_alpha,
      PicaDataBlendFunc      function_dst_alpha
  )
  {
    // @note �u�����h�̃R�}���h���ׂ����������������Ȃ��ق����������Ȃ��H
    // @note PICA_CMD_DATA_BLEND_FUNC��PICA_CMD_DATA_BLEND_FUNC_SEPARATE����ʂ����ق����������H
    *cmd++ = PICA_CMD_DATA_BLEND_FUNC_SEPARATE(equation_rgb, equation_alpha, function_src_rgb, function_dst_rgb, function_src_alpha, function_dst_alpha);
    *cmd++ = PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_FUNC );
    return cmd;
  }

  static inline bit32* MakeBlendLogicOperationCommand(bit32* cmd, PicaDataLogicOp operation)
  {
    // @note �u�����h�̃R�}���h���ׂ����������������Ȃ��ق����������Ȃ��H
    static const u32 BE = 0x1;  // 0001
    *cmd++ = ( operation );
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_LOGIC_OP, BE );
    return cmd;
  }

  static inline bit32* MakeTextureSamplerTypeCommand(
      bit32* cmd,
      PicaDataTexture0SamplerType type0,
      PicaDataTexture1SamplerType type1,
      PicaDataTexture2SamplerType type2,
      u32                         min_filter_up0
  )
  {
    u32 type012 = (type2<<2) | (type1<<1);
    if( type0 != PICA_DATA_TEXTURE0_SAMPLER_TYPE_FALSE )
    {
      type012 |= 1;
    }

    static const u32 BE = 0x1;  // 0001
    *cmd++ = type012;
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_TEXTURE_FUNC, BE );

    static const u32 BE0 = 0x8;  // 1000
    *cmd++ = (type0<<28) | (min_filter_up0<<24);
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_TEXTURE0_WRAP_FILTER, BE0 );

    return cmd;
  }

  static inline bit32* MakeTextureSamplerWrapCommand(bit32* cmd, s32 no, PicaDataTextureWrap wrap_u, PicaDataTextureWrap wrap_v)
  {
    static const u32 BE = 0x2;  // 0010
    *cmd++ = ( wrap_u<<12 | wrap_v<<8 );
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( GetTextureSamplerWrapFilterRegister(no), BE );
    return cmd;
  }

  static inline bit32* MakeTextureSamplerFilterCommand(bit32* cmd, s32 no, PicaDataTextureMagFilter mag_filter, u32 min_filter_up, u32 min_filter_down)
  {
    static const u32 BE = 0x9;  // 1001
    *cmd++ = ( min_filter_up<<24 | min_filter_down<<2 | mag_filter<<1 );
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( GetTextureSamplerWrapFilterRegister(no), BE );
    return cmd;
  }

  static inline bit32* MakeTextureSamplerWrapFilterCommand(
      bit32* cmd,
      s32 no,
      PicaDataTextureMagFilter mag_filter,
      PicaDataTextureMinFilter min_filter,
      PicaDataTextureFormat format,
      PicaDataTextureWrap wrap_u,
      PicaDataTextureWrap wrap_v,
      u32 sampler_type)
  {
    *cmd++ = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(
        mag_filter,
        min_filter,
        format,
        wrap_u,
        wrap_v,
        0,
        sampler_type
    );
    *cmd++ = PICA_CMD_HEADER_SINGLE( GetTextureSamplerWrapFilterRegister(no) );
    return cmd;
  }

  static inline bit32* MakeRasterizationCullingModeCommand(bit32* cmd, u8 culling_mode)
  {
    static const u32 BE = 0x1;  // 0001
    *cmd++ = PICA_CMD_DATA_CULL_FACE(culling_mode);
    *cmd++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_CULL_FACE, BE );
    return cmd;
  }


public:
  // @brief  �R�}���h�𑖍�����
  // command��32�r�b�g�v�f�̔z��
  // command_size��32�r�b�g�v�f�̌�
  //
  // @retval  �X�L�������f���́A�ŋ߂̃w�b�_�[�ʒu
  static u32 ScanCommand(const bit32* command, u32 command_size, H3dCommandListener* listener)
  {
#if 0
/*    �Q�l

    CTR �v���O���~���O�}�j���A��
    �O���t�B�b�N�X���p��
    2012-03-09
    Ver. 4.0

    8.7.2 3D �R�}���h�o�b�t�@�̎d�l*/
#endif

    //////// ���[�h
    enum
    {
      MODE_HEADER  = 0,  // �w�b�_
      MODE_DATA    = 1,  // �f�[�^
      MODE_IGNORE  = 2   // �����f�[�^
    };
    
    u32 mode = MODE_HEADER;

    u32 addr = 0;
    u32 be   = 0;
    u32 size = 0;
    u32 seq  = 0;
    u32 header_index = 0;

  #define BE2MASK(be) ( 0xFF000000*((be&0x8)>>3) + 0xFF0000*((be&0x4)>>2) + 0xFF00*((be&0x2)>>1) + 0xFF*((be&0x1)>>0) )

    u32 count = 0;  // �f�[�^�̌��̃J�E���g

    u32 i = 0;
    while( i < command_size )
    {
      if( mode == MODE_HEADER )
      {
        //////// ���32�r�b�g���w�b�_�A����32�r�b�g���f�[�^
        // ��ʉ��ʂ��Ђ�����Ԃ��Ȃ��Ƃ����Ȃ��̂ŁA64�r�b�g�̃R�}���h��ǂ�

        if( i+2 > command_size )
        {
          // �R�}���h��64�r�b�g�ɖ����Ȃ��ꍇ�A�G���[
          listener->OnReadError( i, command[i] );
          break;
        }

        // �w�b�_�[�̃R�}���h�ʒu��ۑ�
        header_index = i;
         
        // ���32�r�b�g�̃w�b�_��ǂ�
        ++i;
        addr = ( command[i] ) & 0xFFFF;
        be   = ( command[i] >> 16 ) & 0xF;
        size = ( command[i] >> 20 ) & 0xFF;
        seq  = ( command[i] >> 31 );
        listener->OnReadHeader( i, command[i], addr, be, size, seq,
            ( size == 0 )?( H3dCommandListener::SINGLE_ACCESS ):( H3dCommandListener::BURST_ACCESS ) );
        
        // ����32�r�b�g�̃f�[�^��ǂ�
        --i;
        if( size == 0 )
        {
          // �V���O���A�N�Z�X
          if( listener->OnReadData( i, command[i], addr, BE2MASK(be), 0, 1, H3dCommandListener::SINGLE_ACCESS ) ){
            return header_index;
          }
        }
        else  // if( size >= 1 )
        {
          // �o�[�X�g�A�N�Z�X
          count = 0;
          if( listener->OnReadData( i, command[i], addr+((seq==1)?count:0), BE2MASK(be), count, size+1, H3dCommandListener::BURST_ACCESS ) )
          {
            return header_index;
          }
          ++count;
        }
      
        // ���̃R�}���h��
        if( size >= 1 )
        {
          mode = MODE_DATA;
        }
        i += 2;
      }
      else if( mode == MODE_DATA )
      {
        ////// �f�[�^
        // ��ʉ��ʂ��Ђ�����Ԃ��Ȃ��Ă��悢�̂ŁA32�r�b�g�v�f��ǂ�
        
        // �o�[�X�g�A�N�Z�X
        if( listener->OnReadData( i, command[i], addr+((seq==1)?count:0), BE2MASK(be), count, size+1, H3dCommandListener::BURST_ACCESS ) ){
          return header_index;
        }
        ++count;

        // �f�[�^���������A�f�[�^���I����
        if( count >= size+1 )
        {
          // �f�[�^���I��
          if( (size+1)%2 == 0 )
          {
            // �f�[�^�̌��������̏ꍇ
            // 64�r�b�g�f�[�^�̏��32�r�b�g�̃f�[�^�𖳎�����
            mode = MODE_IGNORE;
          }
          else
          {
            // �f�[�^�̌�����̏ꍇ
            // ���̓w�b�_
            mode = MODE_HEADER;
          }
        }
        ++i;
      }
      else  // if( mode == MODE_IGNORE )
      {
        //////// ���32�r�b�g�̃f�[�^�𖳎�����
        listener->OnReadIgnore( i, command[i] );

        // �f�[�^���I��
        // ���̓w�b�_
        mode = MODE_HEADER;
        ++i;
      }
    }

  #undef BE2MASK

    return command_size;
  }

public:
  // @brief  �R�}���h�̏����o�͂���(�P���o��)
  static void DumpCommandSimple(const nw::h3d::fnd::Command& command)
  {
    GFL_PRINT("%s\n", __PRETTY_FUNCTION__);
    GFL_PRINT("size=%d\n", command.size());

    for(s32 i=0; i<command.size(); ++i)
    {
      GFL_PRINT("[%d] 0x%X\n", i, command[i]);
    }
  }

  // @brief  �R�}���h�̏����o�͂���(�ǂ݈Ղ�)
  // command��32�r�b�g�v�f�̔z��
  // command_size��32�r�b�g�v�f�̌�
  static void DumpCommandEasy(const bit32* command, u32 command_size)
  {
    H3dCommandListenerDump listener;

    GFL_PRINT("%s\n", __PRETTY_FUNCTION__);

    GFL_PRINT("-------- Start --------\n");

    //////// 32�r�b�g�v�f�̌�
    // �R�}���h��64�r�b�g��1�Ȃ̂ŁA32�r�b�g�v�f�͋����łȂ���΂Ȃ�Ȃ��B
    GFL_PRINT("command_size=%d, 64bits_num=%d, Remain=%d[%s]\n",
        command_size, command_size/2,
        command_size%2, (command_size%2==1)?("ERROR"):("OK"));

    ////////
    ScanCommand(command, command_size, &listener);
    
    GFL_PRINT("-------- End --------\n");
  }
  static void DumpCommandEasy(const nw::h3d::fnd::Command& command)
  {
    DumpCommandEasy(reinterpret_cast<const bit32*>(command.GetBeginAddress()), command.size());
  }


#if 0
//���܂������Ă���L�q���c���Ă���

  // @brief  �R�}���h�̏����o�͂���(�ǂ݈Ղ�)
  // command��32�r�b�g�v�f�̔z��
  // command_size��32�r�b�g�v�f�̌�
  static void DumpCommandEasy(const bit32* command, u32 command_size)
  {
#if 0
/*    �Q�l

    CTR �v���O���~���O�}�j���A��
    �O���t�B�b�N�X���p��
    2012-03-09
    Ver. 4.0

    8.7.2 3D �R�}���h�o�b�t�@�̎d�l*/
#endif

    GFL_PRINT("%s\n", __PRETTY_FUNCTION__);
   
    //////// 32�r�b�g�v�f�̌�
    // �R�}���h��64�r�b�g��1�Ȃ̂ŁA32�r�b�g�v�f�͋����łȂ���΂Ȃ�Ȃ��B
    GFL_PRINT("command_size=%d, 64bits_num=%d, Remain=%d[%s]\n",
        command_size, command_size/2,
        command_size%2, (command_size%2==1)?("ERROR"):("OK"));

    //////// ���[�h
    enum
    {
      MODE_HEADER  = 0,  // �w�b�_
      MODE_DATA    = 1,  // �f�[�^
      MODE_IGNORE  = 2   // �����f�[�^
    };
    
    u32 mode = MODE_HEADER;

    u32 addr = 0;
    u32 be   = 0;
    u32 size = 0;
    u32 seq  = 0;

    u32 count = 0;  // �f�[�^�̌��̃J�E���g

    u32 i = 0;
    while( i < command_size )
    {
      if( mode == MODE_HEADER )
      {
        //////// ���32�r�b�g���w�b�_�A����32�r�b�g���f�[�^
        // ��ʉ��ʂ��Ђ�����Ԃ��Ȃ��Ƃ����Ȃ��̂ŁA64�r�b�g�̃R�}���h��ǂ�

        if( i+2 > command_size )
        {
          // �R�}���h��64�r�b�g�ɖ����Ȃ��ꍇ�A�G���[
          GFL_PRINT("[%2d](0x%X) [ERROR]\n", i, command[i]);
          break;
        }
         
        // ���32�r�b�g�̃w�b�_��ǂ�
        ++i;
        addr = ( command[i] ) & 0xFFFF;
        be   = ( command[i] >> 16 ) & 0xF;
        size = ( command[i] >> 20 ) & 0xFF;
        seq  = ( command[i] >> 31 );
        GFL_PRINT("[%2d](0x%X) [HEADER] ADDR=0x%X, BE=0x%X, SIZE=%d, SEQ=%d\n", i, command[i], addr, be, size, seq);
        
        // ����32�r�b�g�̃f�[�^��ǂ�
        --i;
        if( size == 0 )
        {
          // �V���O���A�N�Z�X
          GFL_PRINT("[%2d](0x%X) [DATA] SINGLE_ACCESS ADDR=0x%X, MASK=0x%X\n", i, command[i], addr,
              ( 0xFF000000*((be&0x8)>>3) + 0xFF0000*((be&0x4)>>2) + 0xFF00*((be&0x2)>>1) + 0xFF*((be&0x1)>>0) )
          );
        }
        else  // if( size >= 1 )
        {
          // �o�[�X�g�A�N�Z�X
          count = 0;
          GFL_PRINT("[%2d](0x%X) [DATA] BURST_ACCESS ADDR=0x%X, MASK=0x%X, count=%d/%d\n", i, command[i], addr+((seq==1)?count:0),
              ( 0xFF000000*((be&0x8)>>3) + 0xFF0000*((be&0x4)>>2) + 0xFF00*((be&0x2)>>1) + 0xFF*((be&0x1)>>0) )
              , count, size+1
          );
          ++count;
        }
      
        // ���̃R�}���h��
        if( size >= 1 )
        {
          mode = MODE_DATA;
        }
        i += 2;
      }
      else if( mode == MODE_DATA )
      {
        ////// �f�[�^
        // ��ʉ��ʂ��Ђ�����Ԃ��Ȃ��Ă��悢�̂ŁA32�r�b�g�v�f��ǂ�
        
        // �o�[�X�g�A�N�Z�X
        GFL_PRINT("[%2d](0x%X) [DATA] BURST_ACCESS ADDR=0x%X, MASK=0x%X, count=%d/%d\n", i, command[i], addr+((seq==1)?count:0),
            ( 0xFF000000*((be&0x8)>>3) + 0xFF0000*((be&0x4)>>2) + 0xFF00*((be&0x2)>>1) + 0xFF*((be&0x1)>>0) )
            , count, size+1
        );
        ++count;

        // �f�[�^���������A�f�[�^���I����
        if( count >= size+1 )
        {
          // �f�[�^���I��
          if( (size+1)%2 == 0 )
          {
            // �f�[�^�̌��������̏ꍇ
            // 64�r�b�g�f�[�^�̏��32�r�b�g�̃f�[�^�𖳎�����
            mode = MODE_IGNORE;
          }
          else
          {
            // �f�[�^�̌�����̏ꍇ
            // ���̓w�b�_
            mode = MODE_HEADER;
          }
        }
        ++i;
      }
      else  // if( mode == MODE_IGNORE )
      {
        //////// ���32�r�b�g�̃f�[�^�𖳎�����
        GFL_PRINT("[%2d](0x%X) [IGNORE]\n", i, command[i]);

        // �f�[�^���I��
        // ���̓w�b�_
        mode = MODE_HEADER;
        ++i;
      }
    }
  }
  static void DumpCommandEasy(const nw::h3d::fnd::Command& command)
  {
    DumpCommandEasy(reinterpret_cast<const bit32*>(command.GetBeginAddress()), command.size());
  }
#endif


};




//-------------------------------------
///	H3D�e�N�X�`���R�}���h���[�e�B���e�B�̂��߂̃e�N�X�`��0��p�R�}���h���X�i�[(�e�N�X�`��1,2�̓A�h���X���قȂ�܂�)
//=====================================
class H3dCommandListenerReadTexture0 : public H3dCommandListener
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dCommandListenerReadTexture0);

public:
  struct TextureInfo
  {
    void*                        physical_addr;
    u16                          width;
    u16                          height;
    gfl::grp::g3d::TextureFormat format;

    TextureInfo(void)
      : physical_addr(NULL),
        width(0),
        height(0),
        format(TEXTURE_FORMAT_RGBA8)
    {}
  };

public:
  H3dCommandListenerReadTexture0(TextureInfo* info)
    : m_info(info)
  {}
  virtual ~H3dCommandListenerReadTexture0() {}

public:
  virtual bool OnReadData(u32 index, bit32 command, u32 addr, u32 mask, u32 count, u32 count_max, Access access)
  {
    if( addr == PICA_REG_TEXTURE0_ADDR1 )
    {
/*
  CTR 
  DMPGL2.0�V�X�e��API�d�l��
  2012-03-14
  Ver 3.6
  5.8.16 �e�N�X�`���A�h���X�ݒ背�W�X�^
  �e�N�X�`���������̃A�h���X�͑S�āA�����A�h���X��8�o�C�g�A�h���X�Őݒ肵�܂��B�i�����A�h���X��8�Ŋ������l�ł��B�j
*/
      m_info->physical_addr = reinterpret_cast<void*>( (command & 0xFFFFFFF) << 3 );
    }
    else if( addr == PICA_REG_TEXTURE0_SIZE )
    {
      m_info->width  = ( command & 0x7FF0000 ) >> 16;
      m_info->height = ( command & 0x7FF );
    }
    else if( addr == PICA_REG_TEXTURE0_FORMAT )
    {
      m_info->format = static_cast<gfl::grp::g3d::TextureFormat>( command & 0xF );
    }

    return false;
  }

private:
  TextureInfo*  m_info;
};


//-------------------------------------
///	H3D�e�N�X�`���R�}���h���[�e�B���e�B
//=====================================
class H3dTextureCommandUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dTextureCommandUtil);

public:
  // @brief  �R���X�g���N�^
  H3dTextureCommandUtil(void) {}
  // @brief  �f�X�g���N�^
  virtual ~H3dTextureCommandUtil() {}

public:
  // @brief  
  static void GetTextureInfo(H3dCommandListenerReadTexture0::TextureInfo* info, const nw::h3d::fnd::Command& command)
  {
    H3dCommandListenerReadTexture0 listener(info);
    H3dCommandUtil::ScanCommand(reinterpret_cast<const bit32*>(command.GetBeginAddress()), command.size(), &listener);
  }

  // @brief  
  static void* GetPhysicalAddress(const nw::h3d::fnd::Command& command)
  {
    H3dCommandListenerReadTexture0::TextureInfo info;
    GetTextureInfo(&info, command);
    return info.physical_addr;
  }

  // @brief  
  static u16 GetWidth(const nw::h3d::fnd::Command& command)
  {
    H3dCommandListenerReadTexture0::TextureInfo info;
    GetTextureInfo(&info, command);
    return info.width;
  }

  // @brief  
  static u16 GetHeight(const nw::h3d::fnd::Command& command)
  {
    H3dCommandListenerReadTexture0::TextureInfo info;
    GetTextureInfo(&info, command);
    return info.height;
  }

  // @brief  
  static gfl::grp::g3d::TextureFormat GetFormat(const nw::h3d::fnd::Command& command)
  {
    H3dCommandListenerReadTexture0::TextureInfo info;
    GetTextureInfo(&info, command);
    return info.format;
  }
};




}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

#endif // __GFL_G3DH3DCOMMANDUTIL_H__
