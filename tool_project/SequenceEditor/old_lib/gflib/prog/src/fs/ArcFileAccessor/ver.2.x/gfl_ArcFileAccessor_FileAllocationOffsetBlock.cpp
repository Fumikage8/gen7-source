//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃t�@�C���A���P�[�V�����I�t�Z�b�g�u���b�N
 * @file   gfl_ArcFileAccessor_FileAllocationOffsetBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor.h"
#include "gfl_ArcFileAccessor_FileAllocationOffsetBlock.h"

namespace gfl {
  namespace fs {
    namespace ver2 {


      //-----------------------------------------------------------------------
      /**
       * @brief �u���b�N�̃T�C�Y[Byte]���v�Z����
       * @param file_id_num  �t�@�C��ID�̐�
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAllocationOffsetBlock::CalcBlockSize( u16 file_id_num )
      {
        u32 size = 
          sizeof(ArcFileAllocationOffsetBlock::Attribute) + 
          sizeof(ArcFileAllocationOffsetBlock::OffsetTable) * file_id_num;
        return size;
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileAllocationOffsetBlock::ArcFileAllocationOffsetBlock( void ) :
        ArcFileBlock()
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileAllocationOffsetBlock::~ArcFileAllocationOffsetBlock( void )
      {
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief �t�@�C��ID�̌����擾����
       */
      //-----------------------------------------------------------------------
      u16 ArcFileAllocationOffsetBlock::FileIdNum( void ) const 
      { 
        return this->GetAttribute()->file_id_num; 
      } 

      // �u���b�N�f�[�^���瑮�����������o��
      const ArcFileAllocationOffsetBlock::Attribute* ArcFileAllocationOffsetBlock::GetAttribute( void ) const
      {
        const void* block_data = this->BlockData();
        return reinterpret_cast<const ArcFileAllocationOffsetBlock::Attribute*>( block_data );
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �t�@�C���A���P�[�V�����e�[�u���̃I�t�Z�b�g�l���擾����
       * @param file_id  �t�@�C��ID
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAllocationOffsetBlock::AllocationOffset( u16 file_id ) const
      {
        const ArcFileAllocationOffsetBlock::OffsetTable* table = this->GetOffsetTable();
        return table[ file_id ].offset_for_allocation_table;
      } 

      // �u���b�N�f�[�^����e�[�u�����������o��
      const ArcFileAllocationOffsetBlock::OffsetTable* ArcFileAllocationOffsetBlock::GetOffsetTable( void ) const
      {
        const u8* block_data = reinterpret_cast<const u8*>( this->BlockData() );
        size_t pos = sizeof(ArcFileAllocationOffsetBlock::Attribute);
        return reinterpret_cast<const ArcFileAllocationOffsetBlock::OffsetTable*>( &block_data[ pos ] );
      }


    } // namespace ver2
  } // namespace fs
} // namespace gfl
