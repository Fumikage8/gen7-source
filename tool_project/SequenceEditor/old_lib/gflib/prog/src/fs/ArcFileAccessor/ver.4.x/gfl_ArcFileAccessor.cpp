//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃A�N�Z�b�T
 * @file   gfl_ArcFileAccessor.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "./gfl_ArcFileAccessor_FileHeaderBlock.h"
#include "./gfl_ArcFileAccessor_FileAllocationOffsetBlock.h"
#include "./gfl_ArcFileAccessor_FileAllocationTableBlock.h"
#include "./gfl_ArcFileAccessor_FileImageBlock.h"
#include "./gfl_ArcFileAccessor.h"
#include "../gfl_IArcFileAccessor.h"

namespace gfl {
  namespace fs {
    namespace ver4 {

      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileAccessor::ArcFileAccessor( void ) :
        IArcFileAccessor(),
        m_file_header_block(),
        m_file_allocation_offset_block(),
        m_file_allocation_table_block(),
        m_file_image_block()
      {
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       * @param file_data  �A�[�J�C�u�t�@�C���S�̂̃f�[�^
       */
      //-----------------------------------------------------------------------
      ArcFileAccessor::ArcFileAccessor( const void* file_data ) : 
        IArcFileAccessor(),
        m_file_header_block(),
        m_file_allocation_offset_block(),
        m_file_allocation_table_block(),
        m_file_image_block()
      {
        this->AttachData( file_data );
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileAccessor::~ArcFileAccessor( void )
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �A�N�Z�X�Ώۂ̃f�[�^���֘A�t����
       * @param arc_data �A�N�Z�X�Ώۂ̃A�[�J�C�u�f�[�^
       */
      //-----------------------------------------------------------------------
      void ArcFileAccessor::AttachData( const void* arc_data )
      {
        // �w�b�_�u���b�N
        uptr header_block_address = reinterpret_cast<uptr>( arc_data );
        m_file_header_block.SetBlockData( arc_data );

        // �t�@�C���A���P�[�V�����I�t�Z�b�g�u���b�N
        size_t header_block_size =  ArcFileHeaderBlock::CalcBlockSize();
        uptr allocation_offset_block_address = header_block_address + header_block_size;
        void* allocation_offset_block_data = reinterpret_cast<void*>( allocation_offset_block_address );
        m_file_allocation_offset_block.SetBlockData( allocation_offset_block_data );

        // �t�@�C���A���P�[�V�����e�[�u���u���b�N
        u32 file_id_num = m_file_allocation_offset_block.FileIdNum();
        size_t allocation_offset_block_size = ArcFileAllocationOffsetBlock::CalcBlockSize( file_id_num );
        uptr allocation_table_block_address = allocation_offset_block_address + allocation_offset_block_size;
        void* allocation_table_block_data = reinterpret_cast<void*>( allocation_table_block_address );
        m_file_allocation_table_block.SetBlockData( allocation_table_block_data );

        // �t�@�C���C���[�W�u���b�N
        u32 file_num = m_file_allocation_table_block.FileNum();
        size_t allocation_table_block_size = ArcFileAllocationTableBlock::CalcBlockSize( file_num, file_id_num );
        uptr file_image_block_address = allocation_table_block_address + allocation_table_block_size;
        void* file_image_block_data = reinterpret_cast<void*>( file_image_block_address );
        m_file_image_block.SetBlockData( file_image_block_data );
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief �t�@�C��ID�̐����擾����
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAccessor::GetFileIdNum( void ) const
      {
        return m_file_allocation_offset_block.FileIdNum();
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �f�[�^�T�C�Y( �p�f�B���O���܂� )���擾����
       * @param file_id    �A�[�J�C�u���t�@�C��ID
       * @param lang_code  �擾����f�[�^�̌���R�[�h
       * @return �w�肵���t�@�C���̃f�[�^�T�C�Y[Byte]
       */
      //-----------------------------------------------------------------------
      size_t ArcFileAccessor::GetDataSize( u32 file_id, u32 lang_code ) const
      {
        u32 allocation_table_offset = m_file_allocation_offset_block.AllocationOffset( file_id );
        u32 image_top_offset = m_file_allocation_table_block.FileImageOffsetToFileTop( allocation_table_offset, lang_code );
        u32 image_bottom_offset = m_file_allocation_table_block.FileImageOffsetToFileBottom( allocation_table_offset, lang_code );
        size_t data_size = image_bottom_offset - image_top_offset;
        return data_size;
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �f�[�^�T�C�Y( �p�f�B���O���܂܂Ȃ� )���擾����
       * @param file_id    �A�[�J�C�u���t�@�C��ID
       * @param lang_code  �擾����f�[�^�̌���R�[�h
       * @return �w�肵���t�@�C���̃f�[�^�T�C�Y[Byte]
       */
      //-----------------------------------------------------------------------
      size_t ArcFileAccessor::GetRealDataSize( u32 file_id, u32 lang_code ) const
      {
        u32 allocation_table_offset = m_file_allocation_offset_block.AllocationOffset( file_id );
        size_t real_size = m_file_allocation_table_block.FileRealSize( allocation_table_offset, lang_code );
        return real_size;
      }

      //---------------------------------------------------------------------
      /**
       * @brief �ł��傫���f�[�^�̃T�C�Y���擾����
       */
      //---------------------------------------------------------------------
      size_t ArcFileAccessor::GetMaxDataSize( void ) const
      {
        return m_file_header_block.MaxDataSize();
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �A�[�J�C�u�t�@�C���擪����̃I�t�Z�b�g���擾����
       * @param file_id    �A�[�J�C�u���t�@�C��ID
       * @param lang_code  �擾����f�[�^�̌���R�[�h
       * @return �w�肵���t�@�C����, �A�[�J�C�u�t�@�C���擪����̃I�t�Z�b�g�l
       */
      //-----------------------------------------------------------------------
      size_t ArcFileAccessor::GetOffsetToData( u32 file_id, u32 lang_code ) const
      {
        u32 allocation_table_offset = m_file_allocation_offset_block.AllocationOffset( file_id );
        u32 image_top_offset = m_file_allocation_table_block.FileImageOffsetToFileTop( allocation_table_offset, lang_code );
        size_t offset_from_file_top = m_file_header_block.TotalBlockSize() + image_top_offset;
        return offset_from_file_top;
      }


    } // namespace ver4
  } // namespace fs
} // namespace gfl
