//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FragmentOperation.cpp
 *	@brief  �t���O�����g�I�y���[�V�����@�Ǘ�
 *	@author	tomoya takahashi
 *	@date		2010.10.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <grp/gfl_FragmentOperation.h>

#include <nw/font/font_RectDrawerCommand.h>

#include <debug/gfl_DebugPrint.h>
#include <debug/gfl_Assert.h>

#include <gfl_Std.h>

namespace gfl {
namespace grp {



//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *  2D�`�����{�ݒ�
 */
//-----------------------------------------------------------------------------
const FragmentOperationParam FragmentOperation::DefaultParam = 
{
  CULL_NONE,             // �J�����O���[�h�iENV_CULL�`�j
  false,                        // �V�U�[�e�X�g�itrue/false�j
  {0},
  false,                        // �A�[���[�f�v�X�e�X�g(true/false)
  false,                        // �X�e���V���e�X�g(true/false)
  {0},                          // 
  0.0f,                         // W�o�b�t�@�ݒ�i0.0f�Ŗ����j
  0.0f,                         // �f�v�X�����W Near
  1.0f,                         // �f�v�X�����W Far
  0,                            // �f�v�X�l �����ŏ��l�ɑ΂���X�P�[���l
  false,                        // �f�v�X�e�X�g(true/false)
  0,                            // �f�v�X�e�X�g��r�֐�(ENV_TEST_FUNC_�`)
  true,                         // �f�v�X�o�b�t�@�ւ̏������݂��\��(true/false)
  true,                         // �J���[�o�b�t�@�֐Ԃ��������ނ��itrue/false�j
  true,                         // �J���[�o�b�t�@�֐Ԃ��������ނ��itrue/false�j
  true,                         // �J���[�o�b�t�@�֐Ԃ��������ނ��itrue/false�j
  true,                         // �J���[�o�b�t�@�֐Ԃ��������ނ��itrue/false�j
  true,                         // �J���[�o�b�t�@�̓ǂݍ��݂��\���itrue/false�j
  true,                         // �J���[�o�b�t�@�̏������݂��\���itrue/false�j
  false,                        // �f�v�X�o�b�t�@�̓ǂݍ��݂��\���itrue/false�j
  false,                        // �f�v�X�o�b�t�@�̏������݂��\���itrue/false�j
  false,                        // �X�e���V���o�b�t�@�̓ǂݍ��݂��\���itrue/false�j
  false,                        // �X�e���V���o�b�t�@�̏������݂��\���itrue/false�j
};

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					����
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^  �f�t�H���g�ݒ�
 */
//-----------------------------------------------------------------------------
FragmentOperation::FragmentOperation(void)
{
  std::MemCopy( &DefaultParam, &m_setup, sizeof(FragmentOperationParam) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
FragmentOperation::~FragmentOperation()
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p�����[�^�Z�b�g�A�b�v
 *
 *	@param	param
 */
//-----------------------------------------------------------------------------
void FragmentOperation::SetParam( const FragmentOperationParam& param )
{
  gfl::std::MemCopy( &param, &m_setup, sizeof(FragmentOperationParam) );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���݂̃p�����[�^���擾
 *
 *	@return ���݂̐ݒ�
 */
//-----------------------------------------------------------------------------
const FragmentOperationParam& FragmentOperation::GetParam(void) const
{
  return m_setup;
}

//----------------------------------------------------------------------------
/**
 *	@brief  3D�R�}���h���J�����g�̃R�}���h���X�g�ɐݒ�
 *
 *	@param	�t���[���o�b�t�@�N���X
 */
//-----------------------------------------------------------------------------
void FragmentOperation::Add3DCommand( const RenderTarget& renderTarget ) const
{
  f32 left, right, top, bottom;
  renderTarget.GetViewport( &left, &top, &right, &bottom );

  Add3DCommand( left, top, 
      right - left, bottom - top, 
      renderTarget.GetRenderDepthFormat() );
}

//----------------------------------------------------------------------------
/**
 *	@brief  3D�R�}���h���J�����g�̃R�}���h���X�g�ɐݒ�
 *
 *  @param  vp_x            �r���[�|�[�gX
 *  @param  vp_y            �r���[�|�[�gY
 *	@param	vp_width        �J���[�o�b�t�@�@����
 *	@param	vp_height       �J���[�o�b�t�@�@�c��
 *	@param  depth_format    �f�v�X�o�b�t�@�@�t�H�[�}�b�g
 */
//-----------------------------------------------------------------------------
void FragmentOperation::Add3DCommand( u32 vp_x, u32 vp_y, u32 vp_width, u32 vp_height, RenderDepthFormat depth_format ) const
{
  // PICA DEPTH FORMAT�ɕϊ�
  u32 pica_depth_format;
  switch( depth_format ){
  case RENDER_DEPTH_FORMAT_16:
    pica_depth_format = PICA_DATA_DEPTH_COMPONENT16;
    break;

  case RENDER_DEPTH_FORMAT_24:
    pica_depth_format = PICA_DATA_DEPTH_COMPONENT24_OES;
    break;

  case RENDER_DEPTH_FORMAT_24_STENCIL8:
    pica_depth_format = PICA_DATA_DEPTH24_STENCIL8_EXT;
    break;

  default:
    GFL_ASSERT(0);
    pica_depth_format = PICA_DATA_DEPTH24_STENCIL8_EXT; //Coverity[12753] ��������
    break;
  }

  nw::font::ColorBufferInfo colbuf_info = { vp_height, vp_width, pica_depth_format };
  
  // �J�����O���[�h
  {
    const u32 local_cmd[] = {
      PICA_CMD_SET_CULL_FACE( m_setup.cullmode ),
      NW_FONT_CMD_SET_VIEWPORT(vp_x, vp_y, colbuf_info.width, colbuf_info.height)
    };
    //�R�}���h���X�g�̏����o�b�t�@�Ɋi�[
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }
  

  
  // �V�U�[�e�X�g
  if( m_setup.scissor ){
    const u32 local_cmd[] = {
      PICA_CMD_DATA_SCISSOR( true ),
      PICA_CMD_HEADER_BURSTSEQ( PICA_REG_SCISSOR, 3 ),
      (m_setup.scissor_rect.x) | (m_setup.scissor_rect.y<<16),
      (m_setup.scissor_rect.x+m_setup.scissor_rect.width-1) | ((m_setup.scissor_rect.y+m_setup.scissor_rect.height-1)<<16),
    };
    //�R�}���h���X�g�̏����o�b�t�@�Ɋi�[
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }else{
    const u32 local_cmd[] = {
      PICA_CMD_DATA_SCISSOR( false ),
      PICA_CMD_HEADER_BURSTSEQ( PICA_REG_SCISSOR, 3 ),
      0,
      colbuf_info.height | (colbuf_info.width<<16),
    };
    //�R�}���h���X�g�̏����o�b�t�@�Ɋi�[
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }

  // W�o�b�t�@�A�f�v�X�����W
  {
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_WBUFFER_DEPTHRANGE_POLYGONOFFSET( 
          m_setup.wscale, m_setup.depth_range_near,
          m_setup.depth_range_far, m_setup.polygon_offset,
          colbuf_info ),
    };
    //�R�}���h���X�g�̏����o�b�t�@�Ɋi�[
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }


  // �A�[���[�f�v�X�e�X�g 
  {
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_ENABLE_EARLY_DEPTH_TEST( m_setup.arly_depth_test ),
    };
    //�R�}���h���X�g�̏����o�b�t�@�Ɋi�[
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }

  // �X�e���V���e�X�g
  if( m_setup.stencil_test == false ){
    // OFF
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_DISABLE_STENCIL_TEST(),
    };
    //�R�}���h���X�g�̏����o�b�t�@�Ɋi�[
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }else{
    // ON
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_STENCIL_FUNC_MASK( m_setup.stencil_para.func_type, m_setup.stencil_para.func_ref, m_setup.stencil_para.func_msk, m_setup.stencil_para.write_msk ),
      NW_FONT_CMD_SET_STENCIL_OP( m_setup.stencil_para.op_fail, m_setup.stencil_para.op_zfail, m_setup.stencil_para.op_zpass ),
    };
    //�R�}���h���X�g�̏����o�b�t�@�Ɋi�[
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }

  // �f�v�X�e�X�g�E�J���[�}�X�N
  {
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_DEPTH_FUNC_COLOR_MASK( 
          m_setup.depth_test, m_setup.depth_func,
          m_setup.depth_write, m_setup.colormsk_red,
          m_setup.colormsk_green, m_setup.colormsk_blue,
          m_setup.colormsk_alpha ),
    };
    //�R�}���h���X�g�̏����o�b�t�@�Ɋi�[
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }
  
  // �t���[���o�b�t�@�A�N�Z�X
  {
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_FBACCESS( 
          m_setup.frame_color_read, m_setup.frame_color_write,
          m_setup.frame_depth_read, m_setup.frame_depth_write,
          m_setup.frame_stencil_read, m_setup.frame_stencil_write ),
    };
    //�R�}���h���X�g�̏����o�b�t�@�Ɋi�[
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }
}




} /* namespace end grp */
} /* namespace end gfl */
