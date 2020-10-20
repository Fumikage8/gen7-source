//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dModelMemoryManager.cpp
 *	@brief  H3D�@���f������������Ǘ��@�}�l�[�W��
 *	@author	tomoya takahashi
 *	@date		2012.08.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if GFL_GRP_G3D_H3D_USE


#include <grp/g3d/gfl_G3dH3dModelMemoryManager.h>


namespace gfl {
namespace grp {
namespace g3d {


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
   *	@brief  �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  H3dModelMemoryManager::H3dModelMemoryManager(void) : 
    m_pBuffer(NULL),
    m_BufferMax(0)
  {
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  H3dModelMemoryManager::~H3dModelMemoryManager(void)
  {
    this->Finalize();
  }
  
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �}�l�[�W��������
   *
   *	@param  p_heap        �q�[�v
   *	@param  h3dmodel_max  ���f���ő吔
   */
  //-----------------------------------------------------------------------------
  void H3dModelMemoryManager::Initialize( gfl::heap::HeapBase* p_heap, u32 h3dmodel_max )
  {
    if( m_pBuffer ){
      GFL_ASSERT(0);
      this->Finalize();
    }
    
    m_pBuffer = static_cast<Data*>(GflHeapAllocMemory( p_heap, sizeof(Data) * h3dmodel_max ));
    m_BufferMax = h3dmodel_max;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �}�l�[�W���j���@�i�o�^���ꂽ���������ꊇ�ŉ�����܂��j
   */
  //-----------------------------------------------------------------------------
  void H3dModelMemoryManager::Finalize( void )
  {
    if( m_pBuffer ){

      for( u32 i=0; i<m_BufferMax; ++i ){
        if( m_pBuffer[i].data_in ){
          FreeMemoryData( &m_pBuffer[i] );
        }
      }
      GflHeapFreeMemory( m_pBuffer );
      m_pBuffer = NULL;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ��������̍X�V    Calculate��SubmitView�̃^�C�~���O�ŌĂ΂��z��ł���B
   *	                          �P�t���[���ɂP��B
   */
  //-----------------------------------------------------------------------------
  void H3dModelMemoryManager::Update( void )
  {
    for( u32 i=0; i<m_BufferMax; ++i ){
      if( m_pBuffer[i].data_in ){
        if( m_pBuffer[i].count <= 0 ){
          FreeMemoryData(&m_pBuffer[i]);
        }else{
          --m_pBuffer[i].count;
        }
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �j�����郁������o�^    �P�񂾂��o�^����悤�ɒ���
   */
  //-----------------------------------------------------------------------------
#if 1
  void H3dModelMemoryManager::SetDeleteMemory( IMemortObject* pObject )
  {
    for( u32 i=0; i<m_BufferMax; ++i ){

      if( !m_pBuffer[i].data_in ){
        m_pBuffer[i].pMemortObject  = pObject;
        m_pBuffer[i].count          = 1;
        m_pBuffer[i].data_in        = true;
        return ;
      }
    }

    GFL_ASSERT_STOP(0);
  }
  
#else
  void H3dModelMemoryManager::SetDeleteMemory( void * p_state_heap, void * p_state_device )
  {
    for( u32 i=0; i<m_BufferMax; ++i ){

      if( !m_pBuffer[i].data_in ){
        m_pBuffer[i].p_state_heap   = p_state_heap;
        m_pBuffer[i].p_state_device = p_state_device;
        m_pBuffer[i].count          = 1;
        m_pBuffer[i].data_in        = true;
        return ;
      }
    }

    GFL_ASSERT_STOP(0);
  }
#endif
  //----------------------------------------------------------------------------
  /**
   *	@brief    �f�[�^�̗L�����`�F�b�N
   */
  //-----------------------------------------------------------------------------
  bool H3dModelMemoryManager::IsHeapIdMemoryExists( void ) const
  {
    for( u32 i=0; i<m_BufferMax; ++i ){

      if( m_pBuffer[i].data_in ){
        return true;
      }
    }
    return false;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief    heapID������ꂽ�����������݂��Ă��邩�`�F�b�N
   *
   *	@retval true  ���݂��Ă���
   *	@retval false ���݂��Ă��Ȃ�
   */
  //-----------------------------------------------------------------------------
  bool H3dModelMemoryManager::IsHeapIdMemoryExists( gfl::heap::HEAPID heapID ) const
  {
    return false;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �������̔j��
   */
  //-----------------------------------------------------------------------------
  void H3dModelMemoryManager::FreeMemoryData( Data* p_data )
  {
    if( p_data->data_in ){
#if 1
      GFL_SAFE_DELETE( p_data->pMemortObject );
#else
      if(p_data->p_state_heap){
        GflHeapFreeMemory( p_data->p_state_heap );
      }
      if(p_data->p_state_device){
        GflHeapFreeMemory( p_data->p_state_device );
      }
      p_data->p_state_heap    = NULL;
      p_data->p_state_device  = NULL;
#endif
      p_data->data_in = false;
    }
  }


}  // namespace g3d
}  // namespace grp
}  // namespace gfl


#endif  // GFL_GRP_G3D_H3D_USE

