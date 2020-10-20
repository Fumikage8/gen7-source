//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃t�@�C���A���P�[�V�����I�t�Z�b�g�u���b�N
 * @file   gfl_ArcFileAccessor_FileAllocationOffsetBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_FILEALLOCATIONOFFSETBLOCK_V3_H__
#define __GFL_ARCFILEACCESSOR_FILEALLOCATIONOFFSETBLOCK_V3_H__
#include "gfl_ArcFileAccessor_FileBlock.h" 

namespace gfl {
  namespace fs {
    namespace ver3 {


      class ArcFileAllocationOffsetBlock : public ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileAllocationOffsetBlock );

        public:

        //---------------------------------------------------------------------
        /**
         * @brief �u���b�N�̃T�C�Y[Byte]���v�Z����
         * @param file_id_num  �t�@�C��ID�̐�
         */
        //---------------------------------------------------------------------
        static u32 CalcBlockSize( u16 file_id_num );

        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        //---------------------------------------------------------------------
        ArcFileAllocationOffsetBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //---------------------------------------------------------------------
        ~ArcFileAllocationOffsetBlock( void ); 

        //---------------------------------------------------------------------
        /**
         * @brief �t�@�C��ID�̌����擾����
         */
        //---------------------------------------------------------------------
        u16 FileIdNum( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �t�@�C���A���P�[�V�����e�[�u���̃I�t�Z�b�g�l���擾����
         * @param file_id  �t�@�C��ID
         */
        //---------------------------------------------------------------------
        u32 AllocationOffset( u16 file_id ) const;



        private:

        // �u���b�N�̍\��( ������ )
        struct Attribute 
        {
          ArcFileBlock::Header header;
          u16 file_id_num;  // �t�@�C��ID�̌�
          u16 padding;      // 
        };

        // �u���b�N�̍\��( �e�[�u���� )
        struct OffsetTable 
        {
          u32 offset_for_allocation_table;  // �t�@�C���A���P�[�V�����e�[�u���̃I�t�Z�b�g
        };

        /**
         * @brief �u���b�N�f�[�^���瑮�����������o��
         */
        const Attribute* GetAttribute( void ) const;

        /**
         * @brief �u���b�N�f�[�^����e�[�u�����������o��
         */
        const OffsetTable* GetOffsetTable( void ) const;
      };


    } //namespace ver3
  } //namespace fs
} //namespace gfl

#endif  //__GFL_ARCFILEACCESSOR_FILEALLOCATIONOFFSETBLOCK_V3_H__
