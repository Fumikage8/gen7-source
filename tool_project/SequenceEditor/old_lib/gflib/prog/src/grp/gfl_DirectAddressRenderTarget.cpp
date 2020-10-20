//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DirectAddressRenderTarget.cpp
 *	@brief  �A�h���X���w��̃����_�[�^�[�Q�b�g
 *	@author	takahashi tomoya
 *	@date		2011.03.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <grp/gfl_DirectAddressRenderTarget.h>

namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

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
 *	@brief  ���l�A�h���X�w��@�����_�[�^�[�Q�b�g����
 *
 *	@param	allocator         �f�o�C�X�A���P�[�^
 *	@param  width         �o�b�t�@��
 *	@param  height        �o�b�t�@����
 *	@param  color_format  �J���[�o�b�t�@�t�H�[�}�b�g
 *	@param  depth_format  �f�v�X�o�b�t�@�t�H�[�}�b�g
 *	@param  color_address �J���[�o�b�t�@�A�h���X
 *	@param  depth_address �f�v�X�o�b�t�@�A�h���X
 *	@param  fboID         �t���[���o�b�t�@ID  �iGL�p�@�Ȃ���΂O�j
 *
 *	@return �����_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
DirectAddressRenderTarget* DirectAddressRenderTarget::Create( gfl::heap::NwAllocator* allocator, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, u32 color_address, u32 depth_address, GLuint fboID )
{
  const int renderTargetAlignment = 32;
  void* memory = allocator->Alloc(sizeof(DirectAddressRenderTarget), renderTargetAlignment);
  GFL_ASSERT_STOP(memory);
  

  nw::gfx::IRenderTarget::Description description;
  
  description.width   = width;
  description.height  = height;
  description.colorFormat = static_cast<nw::gfx::RenderColorFormat>(color_format);
  description.depthFormat = static_cast<nw::gfx::RenderDepthFormat>(depth_format);

  return GFL_NEW_PLACEMENT(memory) DirectAddressRenderTarget( allocator, description, fboID , color_address , depth_address );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 *
 *	@param	pAllocator    �A���P�[�^
 *	@param  description   �������f�[�^
 *	@param  fboID         GL�p�t���[���o�b�t�@ID
 */
//-----------------------------------------------------------------------------
DirectAddressRenderTarget::DirectAddressRenderTarget( nw::os::IAllocator* pAllocator, const nw::gfx::IRenderTarget::Description& description, GLuint fboID , u32 color_address, u32 depth_address ) :
  nw::gfx::IRenderTarget( pAllocator ),
  m_ActivateCommand(NULL),
  m_Description(description)
{

  m_BackBufferObject.SetHeight( m_Description.height );
  m_BackBufferObject.SetWidth( m_Description.width );
  m_BackBufferObject.SetColorFormat( m_Description.colorFormat );
  m_BackBufferObject.SetColorAddress( color_address );
  m_BackBufferObject.SetDepthFormat( m_Description.depthFormat );
  m_BackBufferObject.SetDepthAddress( depth_address );
  
  // GL�p�t���[���o�b�t�@ID���w��i��{�̃o�b�t�@ID�Ȃ̂ŁA�j�����Ȃ��悤�ɒ��ӁI
  m_BackBufferObject.SetFboID(fboID);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
DirectAddressRenderTarget::~DirectAddressRenderTarget()
{
  // ���̃����_�[�^�[�Q�b�g���m�ۂ����t���[���o�b�t�@�I�u�W�F�N�g�łȂ��̂ŁA�N���A
  m_BackBufferObject.SetFboID(0);
}



}  // namespace grp
}  // namespace gfl
