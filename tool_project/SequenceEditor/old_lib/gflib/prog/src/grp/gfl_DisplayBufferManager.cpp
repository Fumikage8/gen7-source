//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DisplayBufferManager.cpp
 *	@brief  �f�B�X�v���C�o�b�t�@�}�l�[�W��
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <grp/gfl_DisplayBufferManager.h>
#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>
#include <gfl_Macros.h>

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
//-------------------------------------
/// �f�B�X�v���C�o�b�t�@�[�N���X
//=====================================
class DisplayBuffer
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(DisplayBuffer);

public:
  
  // �R���X�g���N�^/�f�X�g���N�^
  DisplayBuffer(void);
  ~DisplayBuffer();
  

  void Initialize( const DisplayBufferSetUp& setup );
  void Finalize( void );

  void ActivateBuffer(void);
  void ActivateBuffer(gfl::grp::DisplayType display_type);

  void TransferRenderImage( const RenderTarget& renderTarget );

  const DisplayBufferSetUp& GetSetupData(void) const;
   
  bool IsInitialized(void) const;

private:
  bool    m_initialized;
  GLuint  m_bufferID;
  DisplayBufferSetUp m_setup;
};



//=============================================================================
/**
 *					�f�B�X�v���C�o�b�t�@�}�l�[�W������
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 *
 *  @param  allocator �A���P�[�^
 *	@param	arrayMax  �z��ő吔
 */
//-----------------------------------------------------------------------------
DisplayBufferManager::DisplayBufferManager( heap::NwAllocator* allocator, u32 arrayMax )
{
  Initialize( allocator, arrayMax );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
DisplayBufferManager::~DisplayBufferManager()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief    ����������
 *
 *	@param	  allocator �A���P�[�^
 *	@param    arrayMax  �z��ő吔
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::Initialize( heap::NwAllocator* allocator, u32 arrayMax )
{
  heap::HeapBase* heap = allocator->GetHeapBase();
  
  if( m_array == NULL ){
    GFL_ASSERT(m_array == NULL); //@check
    Finalize();
  }
  
  m_array = GFL_NEW_ARRAY(heap) DisplayBuffer[arrayMax];
  m_arrayMax = arrayMax;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j������
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::Finalize( void )
{
  if( m_array != NULL ){
    GFL_DELETE_ARRAY m_array;
    m_array = NULL;
    m_arrayMax = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�B�X�v���C�o�b�t�@�̊m��
 *
 *	@param	setup   �o�^���
 *
 *	@retval �f�B�X�v���C�o�b�t�@ID
 *	@retval DISPLAYBUFFER_ID_ERROR    �m�ێ��s
 */
//-----------------------------------------------------------------------------
DisplayBufferID DisplayBufferManager::CreateBuffer( const DisplayBufferSetUp& setup )
{
  DisplayBufferID id;

  // �󂢂Ă��郏�[�N�擾
  id = GetClearBuffer();
  
  // �󂢂Ă��Ȃ��\��������B
  if( id == DISPLAYBUFFER_ID_ERROR ){
    return DISPLAYBUFFER_ID_ERROR;
  }

  // �o�b�t�@�̏�����
  m_array[id].Initialize( setup );
  
  return id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�B�X�v���C�o�b�t�@�̔j��
 *  
 *	@param	displayBufferID  �f�B�X�v���C�o�b�t�@ID
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::DeleteBuffer( DisplayBufferID displayBufferID )
{
  if( ( displayBufferID < m_arrayMax ) )
  {
    m_array[displayBufferID].Finalize();
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    �f�B�X�v���C�o�b�t�@���A�N�e�B�u��Ԃɂ���
 *
 *	@param	  displayBufferID �f�B�X�v���C�o�b�t�@
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::ActivateBuffer( DisplayBufferID displayBufferID )
{
  if( displayBufferID < m_arrayMax )
  {
    m_array[displayBufferID].ActivateBuffer();
  }
  else
  {
    GFL_ASSERT(0);
  }
}
void DisplayBufferManager::ActivateBuffer( DisplayBufferID displayBufferID, gfl::grp::DisplayType display_type )
{
  if( displayBufferID < m_arrayMax )
  {
    m_array[displayBufferID].ActivateBuffer(display_type);
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���o�b�t�@�̃J���[����]��
 *
 *  @param  graphics          �O���t�B�b�N�X
 *	@param  displayBufferID   �f�B�X�v���C�o�b�t�@ID  
 *	@param	renderTarget      �J���[�̓]�����ƂȂ郌���_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::TransferRenderImage( DisplayBufferID displayBufferID, const RenderTarget& renderTarget )
{
  if( displayBufferID < m_arrayMax )
  {
    m_array[displayBufferID].TransferRenderImage( renderTarget );
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�b�g�A�b�v���̎擾
 *
 *	@param	displayBufferID   �f�B�X�v���C�o�b�t�@�h�c
 *
 *	@return �Z�b�g�A�b�v���
 */
//-----------------------------------------------------------------------------
const DisplayBufferSetUp& DisplayBufferManager::GetSetupData( DisplayBufferID displayBufferID ) const
{
  if( displayBufferID < m_arrayMax )
  {
    return m_array[displayBufferID].GetSetupData();
  }
  else
  {
    GFL_ASSERT(0);
    return m_array[0].GetSetupData();
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  �󂢂Ă���f�B�X�v���C�o�b�t�@�̎擾
 *
 *	@retval �󂢂Ă���o�b�t�@ID
 *	@retval DISPLAYBUFFER_ID_ERROR    �m�ێ��s
 */
//-----------------------------------------------------------------------------
DisplayBufferID DisplayBufferManager::GetClearBuffer( void ) const
{
  DisplayBufferID ret = DISPLAYBUFFER_ID_ERROR;

  // �󂢂Ă��郏�[�N�擾
  for( int id=0; id<m_arrayMax; id++ ){
    if( m_array[id].IsInitialized() == false ){
      ret = id; 
      break;
    }
  }

  GFL_ASSERT( ret != DISPLAYBUFFER_ID_ERROR ); //@check

  return ret;
}


//=============================================================================
/**
 *					�f�B�X�v���C�o�b�t�@����
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
DisplayBuffer::DisplayBuffer(void)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
DisplayBuffer::~DisplayBuffer()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	  �Z�b�g�A�b�v�f�[�^
 */
//-----------------------------------------------------------------------------
void DisplayBuffer::Initialize( const DisplayBufferSetUp& setup )
{
  GLuint display;
  
  GFL_ASSERT( m_initialized == false );

  // ���������̕ۑ�
  m_setup = setup;
  
  // �������ʌ���
  if( setup.displayType == DISPLAY_UPPER ){
    display = NN_GX_DISPLAY0;
  }else if( setup.displayType == DISPLAY_UPPER_RIGHT ){
    display = NN_GX_DISPLAY0_EXT;
  }else if( setup.displayType == DISPLAY_LOWER ){
    display = NN_GX_DISPLAY1;
  }else{
    // ��������
    GFL_ASSERT( 0 );
    display = NN_GX_DISPLAY0;
  }

  // �J���[�t�H�[�}�b�g��RGBA8�ȊO�ł���K�v������B
  GFL_ASSERT( setup.colorFormat != RENDER_COLOR_FORMAT_RGBA8 );

  // ���̃f�B�X�v���C���A�N�e�B�u��Ԃɂ���B
  nngxActiveDisplay(display);

  // �f�B�X�v���CID�̎擾
  nngxGenDisplaybuffers(1, &m_bufferID);

  // �f�B�X�v���C�o�b�t�@�̊m��
  nngxBindDisplaybuffer(m_bufferID);
  nngxDisplaybufferStorage(
      setup.colorFormat,
      setup.height,
      setup.width,
      setup.memArea);
  
  nngxDisplayEnv( setup.displayOffsetY, setup.displayOffsetX );

  GFL_ASSERT_MSG( glGetError() == GL_NO_ERROR, "code = %d", glGetError() );


  nngxBindDisplaybuffer(0); // �f�t�H�ɖ߂�
  
  m_initialized = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j������
 */
//-----------------------------------------------------------------------------
void DisplayBuffer::Finalize( void )
{
  if( m_initialized ){
    //
    nngxDeleteDisplaybuffers( 1, &m_bufferID ); 
    GFL_ASSERT_MSG( glGetError() == GL_NO_ERROR, "code = %d", glGetError() );

    m_initialized = false;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    �f�B�X�v���C�o�b�t�@���A�N�e�B�u��Ԃɂ���
 */
//-----------------------------------------------------------------------------
void DisplayBuffer::ActivateBuffer(void)
{
  this->ActivateBuffer( m_setup.displayType );
}
void DisplayBuffer::ActivateBuffer(gfl::grp::DisplayType display_type)
{
  GLuint display;

  // �������ʌ���
  if( display_type == DISPLAY_UPPER ){
    display = NN_GX_DISPLAY0;
  }else if( display_type == DISPLAY_UPPER_RIGHT ){
    display = NN_GX_DISPLAY0_EXT;
  }else if( display_type == DISPLAY_LOWER ){
    display = NN_GX_DISPLAY1;
  }else{
    // ��������
    GFL_ASSERT( 0 );
    display = NN_GX_DISPLAY0;
  }

  nngxActiveDisplay(display);
  nngxBindDisplaybuffer(m_bufferID);
  nngxDisplayEnv( m_setup.displayOffsetY, m_setup.displayOffsetX );
  GFL_ASSERT_MSG( glGetError() == GL_NO_ERROR, "code = %d", glGetError() );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J���[�o�b�t�@�̓��e���f�B�X�v���C�o�b�t�@�ɓ]��
 *
 *	@param	  �t���[���o�b�t�@�C���X�^���X
 */
//-----------------------------------------------------------------------------
void DisplayBuffer::TransferRenderImage( const RenderTarget& renderTarget )
{
  GFL_ASSERT(m_initialized);

  GLuint mode = 0;
  GLint addr;
  GLint block_mode;
  int block;

  ActivateBuffer();

  if (m_setup.transMode == TRANSFER_MODE_ANTIALIASE_NOT_USED)
  {
    mode = NN_GX_ANTIALIASE_NOT_USED;
  }
  else if (m_setup.transMode == TRANSFER_MODE_ANTIALIASE_2x1)
  {
    mode = NN_GX_ANTIALIASE_2x1;
  }
  else if (m_setup.transMode == TRANSFER_MODE_ANTIALIASE_2x2)
  {
    mode = NN_GX_ANTIALIASE_2x2;
  }

  nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, &addr);

  // �u���b�N���[�h�擾
  glGetIntegerv( GL_RENDER_BLOCK_MODE_DMP, &block_mode );
  if( block_mode == GL_RENDER_BLOCK8_MODE_DMP ){
    block = 8;
  }else{
    block = 32;
  }

  u32 offset_size = 4;
  switch( renderTarget.GetRenderColorFormat() ){
  case RENDER_COLOR_FORMAT_RGBA8:
    offset_size = 4;
    break;

  case RENDER_COLOR_FORMAT_RGB8:
    offset_size = 3;
    break;

  case RENDER_COLOR_FORMAT_RGBA4:
  case RENDER_COLOR_FORMAT_RGB5_A1:
  case RENDER_COLOR_FORMAT_RGB565:
    offset_size = 2;
    break;

  default:
    GFL_ASSERT(0);
    break;
  }

  offset_size = offset_size * renderTarget.GetHeight() * (renderTarget.GetWidth() - m_setup.width);

  // �u���b�N�C���[�W�̃����_�[�o�b�t�@�����j�A�C���[�W�̃f�B�X�v���C�o�b�t�@�ɓ]��
  nngxAddB2LTransferCommand( 
      reinterpret_cast<GLvoid*>(renderTarget.GetColorAddress() + (offset_size)),  // srcaddr
      renderTarget.GetHeight(),          // srcwidth
      renderTarget.GetWidth(),         // srcheight
      static_cast<GLenum>(renderTarget.GetRenderColorFormat()),    // src�J���[�t�H�[�}�b�g
      reinterpret_cast<GLvoid*>(addr),    // dstaddr
      m_setup.height,                    // dstwidth
      m_setup.width,                   // dstheight
      m_setup.colorFormat,              // dstcolor_format
      mode,                             // �A���`�G�C���A�X���[�h
      m_setup.isTransferFlipX,          // Y�t���b�v
      block                             // �u���b�N���[�h
      );

  GFL_GRPGL_ERROR_CHECK();

  nngxSplitDrawCmdlist();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ݒ�f�[�^���擾
 */
//-----------------------------------------------------------------------------
const DisplayBufferSetUp& DisplayBuffer::GetSetupData(void) const
{
  return m_setup;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �������ς݃`�F�b�N
 *
 *	@retval true  �������ς�
 *	@retval false ����������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
bool DisplayBuffer::IsInitialized(void) const
{
  return m_initialized;
}


} /* end namespace grp */
} /* end namespace gfl */
