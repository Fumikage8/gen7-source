//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FrameBufferManager.cpp
 *	@brief  �t���[���o�b�t�@�}�l�[�W��
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/gfl_Assert.h>

#include <grp/gfl_FrameBufferManager.h>
#include <grp/gfl_RenderTarget.h>

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
 *	@brief    �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
FrameBufferManager::FrameBufferManager( heap::NwAllocator* allocator, u32 arrayMax )
{
  this->Initialize( allocator, arrayMax );
}


//----------------------------------------------------------------------------
/**
 *	@brief    �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
FrameBufferManager::~FrameBufferManager()
{
  this->Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���o�b�t�@�@����
 *
 *  @param    allocator �A���P�[�^�[
 *	@param    setup	�@  �Z�b�g�A�b�v���
 *
 *	@return �t���[���o�b�t�@ID
 */
//-----------------------------------------------------------------------------
FrameBufferID FrameBufferManager::CreateBuffer( heap::NwAllocator* allocator, const RenderBufferSetUp& setup )
{
  RenderTarget::Description l_description;

  GFL_ASSERT( m_array != NULL );

  // �󂢂Ă���ID���擾
  FrameBufferID id = this->GetClearBuffer();
  if( id == FRAMEBUFFER_ID_ERROR ){
    return id;
  }

  l_description.type    = RenderTarget::TYPE_ON_SCREEN_BUFFER;
  l_description.width   = setup.width;
  l_description.height  = setup.height;
  l_description.color_format = setup.colorFormat;
  l_description.depth_format = setup.depthFormat;
  l_description.color_area = setup.colorArea;
  l_description.depth_area = setup.depthArea;
  l_description.display_type = setup.targetDisplay;

  // share�t���[���o�b�t�@�̃A�h���X���w��
  if( setup.shareBuffer != DISPLAY_NONE ){

    // ���̕`��^�C�v�̃t���[���o�b�t�@�����邩�H
    FrameBufferID display_frameID = this->GetDisplayTypeBuffer(setup.shareBuffer);
    GFL_ASSERT_STOP(display_frameID != FRAMEBUFFER_ID_ERROR);

    // ���̃A�h���X�ŏ���������B
    l_description.color_address = m_array[display_frameID].GetColorAddress();
    l_description.depth_address = m_array[display_frameID].GetDepthAddress();
    l_description.fob_id        = m_array[display_frameID].GetFboID();
  }

  // ������
  m_array[id].Create( allocator, allocator, &l_description );

  // ViewPort��ݒ�B(�l����������B�j
  if( (setup.left != setup.right) && (setup.top != setup.bottom) ){
    m_array[id].SetViewport( setup.left, setup.top, setup.right, setup.bottom );
  }

  return id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���o�b�t�@�@�j��
 *
 *	@param	id    �t���[���o�b�t�@ID
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::DeleteBuffer( FrameBufferID id )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].Destroy();
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���o�b�t�@���J�����g�ɐݒ�
 *
 *	@param	  id  �t���[���o�b�t�@�h�c
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::Bind( FrameBufferID id )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].ActivateBuffer();
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���o�b�t�@���N���A����
 *
 *	@param	  id  �t���[���o�b�t�@�h�c
 *	@param    clearColoer �N���A�J���[�ݒ�
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::BindAndClear( FrameBufferID id, const FrameBufferClearColor& clearColor, const FrameBufferClearFlag& clearFlag )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].ActivateBuffer();
      m_array[id].ClearBuffer(
          clearFlag.f_color,
          gfl::grp::ColorF32(clearColor.red, clearColor.green, clearColor.blue, clearColor.alpha),
          clearFlag.f_depth,
          clearColor.depth,
          clearFlag.f_stencil,
          clearColor.stencil);
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���o�b�t�@�̏����������ɃR�s�[
 *
 *	@param	id      �t���[���o�b�t�@ID
 *	@param  type    �o�b�t�@�^�C�v
 *	@param  memory  �R�s�[��A�h���X
 *
 *	*�����ŃJ�����g�o�b�t�@��ύX���܂��B
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::OutputImage( FrameBufferID id, RenderTarget::RenderBufferType type, void* memory )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].OutputImage( type, memory );
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  ����������t���[���o�b�t�@�֏����R�s�[
 *
 *  @param  id      �t���[���o�b�t�@ID
 *  @param  type    �o�b�t�@�^�C�v
 *  @param  memory  �R�s�[���A�h���X
 *
 *  *�����ŃJ�����g�o�b�t�@��ύX���܂��B
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::InputImage( FrameBufferID id, void* memory )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].InputImage( memory );
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���o�b�t�@�̏����������ɃR�s�[ (Liner�t�H�[�}�b�g�j
 *
 *	@param	id      �t���[���o�b�t�@ID
 *	@param  type    �o�b�t�@�^�C�v
 *	@param  memory  �R�s�[��A�h���X
 *
 *	*�����ŃJ�����g�o�b�t�@��ύX���܂��B
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::OutputB2LImage( FrameBufferID id, RenderTarget::RenderBufferType type, void* memory )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].OutputB2LImage( type, memory );
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����_�[�^�[�Q�b�g�̎擾
 *
 *	@param	id      �t���[���o�b�t�@ID
 *
 *	@return ID�ɑΉ����������_�[�^�[�Q�b�g�擾
 */
//-----------------------------------------------------------------------------
gfl::grp::RenderTarget* FrameBufferManager::GetRenderTarget( FrameBufferID id ) const
{
  if(id < m_arrayMax){
    if( m_array[id].IsCreated() ){
      return &m_array[id];
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	allocator �A���P�[�^
 *	@param	arrayMax  �z��ő吔
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::Initialize( heap::NwAllocator* allocator, u32 arrayMax )
{
  heap::HeapBase* heap = allocator->GetHeapBase();
  GFL_ASSERT( m_array == NULL ); //@check

  m_array = GFL_NEW_ARRAY(heap) RenderTarget[arrayMax];
  m_arrayMax = arrayMax;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::Finalize( void )
{
  if(m_array){

    GFL_DELETE_ARRAY m_array;
    m_array = NULL;
    m_arrayMax = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �󂢂Ă���t���[���o�b�t�@ID���擾
 *
 *	@retval �t���[���o�b�t�@ID
 *	@retval FRAMEBUFFER_ID_ERROR �m�ۂł��Ȃ��B
 */
//-----------------------------------------------------------------------------
FrameBufferID FrameBufferManager::GetClearBuffer( void ) const
{
  FrameBufferID ret = FRAMEBUFFER_ID_ERROR;

  for( int i=0; i<m_arrayMax; ++i ){
    if( !m_array[i].IsCreated() ){
      ret = i;
      break;
    }
  }

  GFL_ASSERT( ret != FRAMEBUFFER_ID_ERROR ); //@check

  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �f�B�X�v���C�^�C�v�̃t���[���o�b�t�@ID���擾
 *
 *	@param	displayType   �f�B�X�v���C�^�C�v
 *
 *	@return �t���[���o�b�t�@ID
 */
//-----------------------------------------------------------------------------
FrameBufferID FrameBufferManager::GetDisplayTypeBuffer( DisplayType displayType ) const
{
  FrameBufferID ret = FRAMEBUFFER_ID_ERROR;
  grp::DisplayType buf_disp_type;

  for( u32 i=0; i<m_arrayMax; ++i ){
    if( m_array[i].IsCreated() ){
      buf_disp_type = m_array[i].GetDisplayType();
      if( buf_disp_type == displayType ){
        ret = i;
        break;
      }
    }
  }

  return ret;
}


} /* namespace end grp */
} /* namespace end gfl */
