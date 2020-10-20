//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃t�@�C���A���P�[�V�����e�[�u���u���b�N
 * @file   gfl_ArcFileAccessor_FileAllocationTableBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_FILEALLOCATIONTABLEBLOCK_V2_H__
#define __GFL_ARCFILEACCESSOR_FILEALLOCATIONTABLEBLOCK_V2_H__
#include "gfl_ArcFileAccessor_FileBlock.h"


namespace gfl {
  namespace fs {
    namespace ver2 {


      class ArcFileAllocationTableBlock : public ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileAllocationTableBlock );

        public:

        //---------------------------------------------------------------------
        /**
         * @brief �u���b�N�̃T�C�Y[Byte]���v�Z����
         * @param file_num     ����Ⴂ���܂߂��S�Ẵt�@�C���̐�
         * @param file_id_num  �t�@�C��ID�̐�
         */
        //---------------------------------------------------------------------
        static u32 CalcBlockSize( u32 file_num, u16 file_id_num );

        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        //---------------------------------------------------------------------
        ArcFileAllocationTableBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //---------------------------------------------------------------------
        ~ArcFileAllocationTableBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief ����Ⴂ���܂߂��S�Ẵt�@�C���̐����擾����
         */
        //---------------------------------------------------------------------
        u32 FileNum( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �e�����p�̃f�[�^�����邩�ǂ�����\���r�b�g����擾����
         */
        //---------------------------------------------------------------------
        u32 LanguageBit( u32 table_offset ) const; 

        //---------------------------------------------------------------------
        /**
         * @brief �t�@�C���擪�ւ̃I�t�Z�b�g�l���擾����
         */
        //---------------------------------------------------------------------
        u32 FileImageOffsetToFileTop( u32 table_offset, u32 language ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �t�@�C�������ւ̃I�t�Z�b�g�l���擾����
         */
        //---------------------------------------------------------------------
        u32 FileImageOffsetToFileBottom( u32 table_offset, u32 language ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �w�肵�������p�̃t�@�C�������݂��邩�ǂ����𒲂ׂ�
         *
         * @param table_offset  �`�F�b�N�Ώۂ̃A���P�[�V�����e�[�u���̐擪�ւ̃I�t�Z�b�g
         * @param language      �`�F�b�N�Ώۂ̌��� 
         *
         * @retval true   ���݂���
         * @retval false  ���݂��Ȃ�
         */
        //---------------------------------------------------------------------
        bool HaveLanguageFile( u32 table_offset, u32 language ) const;




        private:

        // �u���b�N�̍\��( ������ )
        struct Attribute 
        {
          ArcFileBlock::Header header;
          u32 file_num;           // ����Ⴂ���܂߂��S�Ẵt�@�C����
        };

        // �u���b�N�̍\��( �e�[�u���w�b�_�� )
        struct AllocationTableHeader 
        {
          u32 language_bit;  // �e�����p�̃f�[�^�����邩�ǂ����̃r�b�g��
        };

        // �u���b�N�̍\��( �e�[�u���� )
        struct AllocationTableElement 
        {
          u32 file_top;     // �t�@�C���擪�ւ̃I�t�Z�b�g
          u32 file_bottom;  // �t�@�C�������ւ̃I�t�Z�b�g + 1
        };

        /**
         * @brief �u���b�N�f�[�^���瑮�����������o��
         */
        const Attribute* GetAttribute( void ) const;

        /**
         * @brief �u���b�N�f�[�^����e�[�u���w�b�_���������o��
         *
         * @param table_offset  �A���P�[�V�����e�[�u���ւ̃I�t�Z�b�g�l
         */
        const AllocationTableHeader* GetAllocationTableHeader( u32 table_offset ) const;

        /**
         * @brief �u���b�N�f�[�^����e�[�u�����������o��
         *
         * @param table_offset  �A���P�[�V�����e�[�u���ւ̃I�t�Z�b�g�l
         */
        const AllocationTableElement* GetAllocationTable( u32 table_offset ) const;

        /**
         * @brief �����p�t�@�C����, �A���P�[�V�����e�[�u�����̃C���f�b�N�X���擾����
         */
        u16 GetLanguageFileIndex( const AllocationTableHeader* table_header, u32 language ) const;

        /**
         * @brief �����p�t�@�C�������݂��邩�ǂ����𒲂ׂ�
         */
        bool CheckLanguageFileExist( const AllocationTableHeader* table_header, u32 language ) const;
      };


    } // namespace ver2
  } // namespace fs
} // namespace gfl


#endif  // __GFL_ARCFILEACCESSOR_FILEALLOCATIONTABLEBLOCK_V2_H__
