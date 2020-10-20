//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃t�@�C���A���P�[�V�����e�[�u���u���b�N
 * @file   gfl_ArcFileAccessor_FileAllocationTableBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor.h"
#include "gfl_ArcFileAccessor_FileAllocationTableBlock.h"


namespace gfl {
  namespace fs {
    namespace ver3 { 


      //-----------------------------------------------------------------------
      /**
       * @brief �u���b�N�̃T�C�Y[Byte]���v�Z����
       * @param file_num     ����Ⴂ���܂߂��S�Ẵt�@�C���̐�
       * @param file_id_num  �t�@�C��ID�̐�
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::CalcBlockSize( u32 file_num, u16 file_id_num )
      {
        u32 size = 
          sizeof(ArcFileAllocationTableBlock::Attribute) + 
          sizeof(ArcFileAllocationTableBlock::AllocationTableHeader) * file_id_num +
          sizeof(ArcFileAllocationTableBlock::AllocationTableElement) * file_num;
        return size;
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileAllocationTableBlock::ArcFileAllocationTableBlock( void ) : 
        ArcFileBlock()
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileAllocationTableBlock::~ArcFileAllocationTableBlock( void )
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief ����Ⴂ���܂߂��S�Ẵt�@�C���̐����擾����
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::FileNum( void ) const 
      {
        return this->GetAttribute()->file_num; 
      }

      // �u���b�N�f�[�^���瑮�����������o��
      const ArcFileAllocationTableBlock::Attribute* ArcFileAllocationTableBlock::GetAttribute( void ) const
      {
        const void* block_data = this->BlockData();
        return reinterpret_cast<const ArcFileAllocationTableBlock::Attribute*>( block_data );
      }

      //--------------------------------------------------------------------------
      /**
       * @brief �e�����p�t�@�C�������݂��邩�ǂ�����\���r�b�g����擾����
       */
      //--------------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::LanguageBit( u32 table_offset ) const
      { 
        return this->GetAllocationTableHeader( table_offset )->language_bit;
      }

      // �u���b�N�f�[�^����e�[�u���w�b�_���������o��
      const ArcFileAllocationTableBlock::AllocationTableHeader* ArcFileAllocationTableBlock::GetAllocationTableHeader( u32 table_offset ) const
      {
        const u8* block_data = reinterpret_cast<const u8*>( this->BlockData() );
        u32 header_pos = sizeof(ArcFileAllocationTableBlock::Attribute) + table_offset;
        return reinterpret_cast<const ArcFileAllocationTableBlock::AllocationTableHeader*>( &block_data[ header_pos ] );
      } 

      //--------------------------------------------------------------------------
      /**
       * @brief �t�@�C���擪�ւ̃I�t�Z�b�g�l���擾����
       */
      //--------------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::FileImageOffsetToFileTop( u32 table_offset, u32 language ) const
      {
        const ArcFileAllocationTableBlock::AllocationTableHeader* table_header = this->GetAllocationTableHeader( table_offset );
        const ArcFileAllocationTableBlock::AllocationTableElement* allocation_table = this->GetAllocationTable( table_offset );
        u16 language_file_index = this->GetLanguageFileIndex( table_header, language );
        return allocation_table[ language_file_index ].file_top;
      } 

      //--------------------------------------------------------------------------
      /**
       * @brief �t�@�C�������ւ̃I�t�Z�b�g�l���擾����
       */
      //--------------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::FileImageOffsetToFileBottom( u32 table_offset, u32 language ) const
      {
        const ArcFileAllocationTableBlock::AllocationTableHeader* table_header = this->GetAllocationTableHeader( table_offset );
        const ArcFileAllocationTableBlock::AllocationTableElement* allocation_table = this->GetAllocationTable( table_offset );
        u16 language_file_index = this->GetLanguageFileIndex( table_header, language );
        return allocation_table[ language_file_index ].file_bottom;
      } 

      // �����p�t�@�C����, �A���P�[�V�����e�[�u�����̃C���f�b�N�X���擾����
      u16 ArcFileAllocationTableBlock::GetLanguageFileIndex( const AllocationTableHeader* table_header, u32 language ) const
      {
        if( !this->CheckLanguageFileExist( table_header, language ) ) {
          return 0;  // �����p�t�@�C�����Ȃ��ꍇ��, �f�t�H���g�p�̃t�@�C�����Q�Ƃ���
        }

        u16 index = 0;
        for( u16 check_language=0; check_language<language; check_language++ )
        {
          if( this->CheckLanguageFileExist( table_header, check_language ) )
          {
            index++;
          }
        }
        return index;
      }

      //--------------------------------------------------------------------------
      /**
       * @brief �w�肵�������p�̃t�@�C�������݂��邩�ǂ����𒲂ׂ�
       *
       * @param table_offset  �`�F�b�N�Ώۂ̃A���P�[�V�����e�[�u���̐擪�ւ̃I�t�Z�b�g
       * @param language      �`�F�b�N�Ώۂ̌��� 
       *
       * @retval true   ���݂���
       * @retval false  ���݂��Ȃ�
       */
      //--------------------------------------------------------------------------
      bool ArcFileAllocationTableBlock::HaveLanguageFile( u32 table_offset, u32 language ) const
      {
        const ArcFileAllocationTableBlock::AllocationTableHeader* table_header = this->GetAllocationTableHeader( table_offset );
        return this->CheckLanguageFileExist( table_header, language );
      }

      // �����p�t�@�C�������݂��邩�ǂ����𒲂ׂ�
      bool ArcFileAllocationTableBlock::CheckLanguageFileExist( const AllocationTableHeader* table_header, u32 language ) const
      {
        u32 check_bit = 1 << language;
        return ( table_header->language_bit & check_bit ) != 0;
      } 

      // �u���b�N�f�[�^����e�[�u�����������o��
      const ArcFileAllocationTableBlock::AllocationTableElement* ArcFileAllocationTableBlock::GetAllocationTable( u32 table_offset ) const
      {
        const u8* block_data = reinterpret_cast<const u8*>( this->BlockData() );
        u32 table_pos = sizeof(ArcFileAllocationTableBlock::Attribute) + table_offset + sizeof(ArcFileAllocationTableBlock::AllocationTableHeader);
        return reinterpret_cast<const ArcFileAllocationTableBlock::AllocationTableElement*>( &block_data[ table_pos ] );
      }


    } // namespace ver3
  } // namespace fs
} // namespace gfl
