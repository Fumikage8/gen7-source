//========================================================================================
/**
 * @file   gfl_BackupData.cpp
 * @brief  �o�b�N�A�b�v�f�[�^
 * @author obata_toshihiro
 * @date   2011.02.21
 */
//========================================================================================
#include <gfl_Base.h>
#include <gfl_Math.h>
#include <gfl_Heap.h>
#include <fs/gfl_BackupData.h>


namespace gfl {
  namespace fs { 


    //--------------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     *
     * @param heap       �o�b�t�@�̊m�ۂɎg�p����q�[�v
     * @param data_form  �o�b�N�A�b�v�f�[�^���`������
     */
    //--------------------------------------------------------------------------------
    BackupData::BackupData( gfl::heap::HeapBase* heap, const DataForm* data_form ) : 
      m_data_form( data_form ),
      m_block_info( NULL ),
      m_buffer( NULL )
    {
      // �u���b�N���̓o�^�E�o�b�t�@�̊m��
      m_block_info = GFL_NEW_ARRAY(heap) BlockInfo[ data_form->block_num ]; 
      size_t buffer_size = 0;
      for( int i=0; i<data_form->block_num; i++ )
      {
        m_block_info[i].offset_address = buffer_size;
        buffer_size += this->GetBlockSize(i);
      } 
      m_buffer = GflHeapAllocMemoryAlign( heap, buffer_size, BLOCK_ALIGNMENT );

      // �u���b�N������
      this->Clear();
    }

    //--------------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //--------------------------------------------------------------------------------
    BackupData::~BackupData( void )
    {
      if( m_block_info ) {
        GFL_DELETE_ARRAY( m_block_info );
      } 
      if( m_buffer ) {
        GflHeapFreeMemory( m_buffer );
      }
    }


    //--------------------------------------------------------------------------------
    /**
     * @brief �����f�[�^�����C���X�^���X�𐶐�����
     *
     * @param heap  �C���X�^���X�̐����Ɏg�p����q�[�v
     *
     * @return ���������C���X�^���X
     */
    //--------------------------------------------------------------------------------
    BackupData* BackupData::CreateClone( gfl::heap::HeapBase* heap ) const
    { 
      BackupData* clone = GFL_NEW(heap) BackupData( heap, this->m_data_form );
      this->CopyTo( clone );
      return clone;
    }


    //--------------------------------------------------------------------------------
    /**
     * @brief �f�[�^���R�s�[����
     *
     * @param dest  �R�s�[��̃C���X�^���X
     *
     * @retval true   �R�s�[����
     * @retval false  �R�s�[���s
     *
     * @note �R�s�[���ƃR�s�[��͓����f�[�^�ł���K�v������܂�.
     */
    //--------------------------------------------------------------------------------
    bool BackupData::CopyTo( BackupData* dest ) const
    {
      if( ( this->m_data_form->form_id != dest->m_data_form->form_id ) ||
          ( this->m_data_form->block_num != dest->m_data_form->block_num ) ||
          ( this->GetBufferSize() != dest->GetBufferSize() ) )
      {
        GFL_ASSERT(0);  // �f�[�^�t�H�[�}�b�g����v���Ȃ�
        return false;  
      } 

      gfl::std::MemCopy( this->m_buffer, dest->m_buffer, this->GetBufferSize() );
      return true;
    }

    //--------------------------------------------------------------------------------
    /**
     * @brief �o�b�t�@�̃T�C�Y���擾����
     */
    //--------------------------------------------------------------------------------
    size_t BackupData::GetBufferSize( void ) const
    {
      size_t buffer_size = 0; 
      for( int i=0; i<m_data_form->block_num; i++ ) {
        buffer_size += GetBlockSize(i);
      }
      return buffer_size;
    }

    //--------------------------------------------------------------------------------
    /**
     * @brief �u���b�N�T�C�Y���擾����
     *
     * @param index  �Ώۃu���b�N�̃C���f�b�N�X
     *
     * @note
     * �e�u���b�N�̐擪�A�h���X��4�o�C�g�A���C�������g�ɂȂ�悤��,
     * �e�u���b�N�̃T�C�Y��4�̔{���ɂȂ�悤�ɐ؂�グ��.
     */
    //--------------------------------------------------------------------------------
    size_t BackupData::GetBlockSize( BlockIndex index ) const
    {
      size_t size = m_data_form->block_form[index].size_get_func();
      return gfl::math::RoundUp( size, BLOCK_ALIGNMENT );
    } 


    //--------------------------------------------------------------------------------
    /**
     * @brief �N���A����( ������Ԃɂ��� )
     */
    //--------------------------------------------------------------------------------
    void BackupData::Clear( void )
    {
      for( int i=0; i<m_data_form->block_num; i++ )
      {
        this->ClearBlock(i);
      }
    }

    /**
     * @brief �w�肵���u���b�N������������
     */
    void BackupData::ClearBlock( BlockIndex index )
    {
      if( m_data_form->block_num <= index )
      {
        GFL_ASSERT(0);
        return;
      }

      void* data_ptr = this->GetBlockPointer(index);
      m_data_form->block_form[index].clear_func( data_ptr );
    } 

    //--------------------------------------------------------------------------------
    /**
     * @brief �u���b�N�̐擪�A�h���X���擾����
     *
     * @param index  �Ώۃu���b�N�̃C���f�b�N�X
     */
    //--------------------------------------------------------------------------------
    void* BackupData::GetBlockPointer( BlockIndex index ) const
    {
      if( m_data_form->block_num <= index )
      {
        GFL_ASSERT(0);
        index = 0;  // �C���f�b�N�X0�̃u���b�N�A�h���X��Ԃ�
      } 

      uptr address = reinterpret_cast<uptr>( m_buffer ) + m_block_info[index].offset_address; 
      return reinterpret_cast<void*>( address ); 
    }



  }  // namespace fs
}  // namespace gfl 
