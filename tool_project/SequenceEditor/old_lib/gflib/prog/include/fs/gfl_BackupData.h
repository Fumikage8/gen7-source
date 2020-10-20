#ifndef __GFL_BACKUPDATA_H__
#define __GFL_BACKUPDATA_H__
//========================================================================================
/**
 * @file   gfl_BackupData.h
 * @brief  �o�b�N�A�b�v�f�[�^
 * @author obata_toshihiro
 * @date   2011.02.16
 */
//========================================================================================
#pragma once

#include <gfl_Heap.h>

namespace gfl {
  namespace fs { 




    /**
     * @class BackupData
     *
     *
     *
     *
     *
     *
     *
     *
     *
     * @attention �폜�\��̃N���X�ł��B�g�p���Ȃ��ł��������B
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     */
    class BackupData
    {
      GFL_FORBID_COPY_AND_ASSIGN( BackupData );

      public:
        typedef u32 FormId;                       //�u���̃f�[�^�Ȃ̂��v�����ʂ���ID
        typedef u32 BlockIndex;                   // �u���b�N�̊Ǘ��C���f�b�N�X
        typedef void (*DataClearFunc)(void*);     // �f�[�^������������֐�
        typedef size_t (*DataSizeGetFunc)(void);  // �f�[�^�T�C�Y���擾����֐�


        /**
         * @brief �e�u���b�N���`������
         */
        struct BlockForm {
          DataClearFunc clear_func;       // �f�[�^������������֐�
          DataSizeGetFunc size_get_func;  // �f�[�^�T�C�Y���擾����֐�
        };

        /**
         * @brief �o�b�N�A�b�v�f�[�^���`������
         */
        struct DataForm {
          FormId form_id;         //�u���̃f�[�^�Ȃ̂��v�����ʂ���ID
          u32 block_num;          // �u���b�N�̑���
          BlockForm* block_form;  // �e�u���b�N�̒�`���
        };


        //--------------------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         *
         * @param heap   �o�b�t�@�̊m�ۂɎg�p����q�[�v
         * @param param  �o�b�N�A�b�v�f�[�^���`������
         */
        //--------------------------------------------------------------------------------
        BackupData( gfl::heap::HeapBase* heap, const DataForm* param );

        //--------------------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //--------------------------------------------------------------------------------
        virtual ~BackupData( void );

        //--------------------------------------------------------------------------------
        /**
         * @brief �����f�[�^�����C���X�^���X�𐶐�����
         *
         * @param heap  �C���X�^���X�̐����Ɏg�p����q�[�v
         *
         * @return ���������C���X�^���X
         */
        //--------------------------------------------------------------------------------
        BackupData* CreateClone( gfl::heap::HeapBase* heap ) const;

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
        bool CopyTo( BackupData* dest ) const;

        //--------------------------------------------------------------------------------
        /**
         * @brief �N���A����( ������Ԃɂ��� )
         */
        //--------------------------------------------------------------------------------
        void Clear( void );

        //--------------------------------------------------------------------------------
        /**
         * @brief �o�b�t�@�̃A�h���X���擾����
         */
        //--------------------------------------------------------------------------------
        inline void* GetBuffer( void ) const { return m_buffer; }

        //--------------------------------------------------------------------------------
        /**
         * @brief �o�b�t�@�̃T�C�Y���擾����
         */
        //--------------------------------------------------------------------------------
        size_t GetBufferSize( void ) const;

        //--------------------------------------------------------------------------------
        /**
         * @brief �u���b�N�̐擪�A�h���X���擾����
         *
         * @param index  �Ώۃu���b�N�̃C���f�b�N�X
         */
        //--------------------------------------------------------------------------------
        void* GetBlockPointer( BlockIndex index ) const;

        //--------------------------------------------------------------------------------
        /**
         * @brief �u���b�N�T�C�Y���擾����
         *
         * @param index  �Ώۃu���b�N�̃C���f�b�N�X
         */
        //--------------------------------------------------------------------------------
        size_t GetBlockSize( BlockIndex index ) const;


      private:
        /**
         * @brief �u���b�N�̈�̃A���C�������g
         */
        static const size_t BLOCK_ALIGNMENT = 4;

        /**
         * @brief �w�肵���u���b�N������������
         */
        void ClearBlock( BlockIndex index );

        /**
         * @brief �u���b�N���
         */
        struct BlockInfo {
          u32 offset_address;  // �o�b�t�@�̐擪����̃I�t�Z�b�g�A�h���X
        }; 

        const DataForm* m_data_form;
        BlockInfo* m_block_info;
        void* m_buffer;  // ���ۂɃf�[�^���i�[�����o�b�t�@
    }; 

  }  // namespace fs
}  // namespace gfl 

#endif  // __GFL_BACKUPDATA_H__ 
