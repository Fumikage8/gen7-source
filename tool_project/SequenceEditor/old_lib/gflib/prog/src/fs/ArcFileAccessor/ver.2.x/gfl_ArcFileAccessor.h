//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃A�N�Z�b�T
 * @file   gfl_ArcFileAccessor.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_V2_H__
#define __GFL_ARCFILEACCESSOR_V2_H__ 
#include "./gfl_ArcFileAccessor_FileHeaderBlock.h"
#include "./gfl_ArcFileAccessor_FileAllocationOffsetBlock.h"
#include "./gfl_ArcFileAccessor_FileAllocationTableBlock.h"
#include "./gfl_ArcFileAccessor_FileImageBlock.h"
#include "../gfl_IArcFileAccessor.h"

namespace gfl {
  namespace fs {
    namespace ver2 {


      class ArcFileAccessor : public IArcFileAccessor
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileAccessor );

        public:

        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        //---------------------------------------------------------------------
        ArcFileAccessor( void );

        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         * @param file_data  �A�[�J�C�u�t�@�C���S�̂̃f�[�^
         */
        //---------------------------------------------------------------------
        ArcFileAccessor( const void* file_data );

        //---------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //---------------------------------------------------------------------
        virtual ~ArcFileAccessor( void );

        //---------------------------------------------------------------------
        /**
         * @brief �A�N�Z�X�Ώۂ̃f�[�^���֘A�t����
         * @param arc_data �A�N�Z�X�Ώۂ̃A�[�J�C�u�f�[�^
         */
        //---------------------------------------------------------------------
        virtual void AttachData( const void* arc_data );

        //---------------------------------------------------------------------
        /**
         * @brief �t�@�C��ID�̐����擾����
         */
        //---------------------------------------------------------------------
        virtual u32 GetFileIdNum( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �f�[�^�T�C�Y���擾����
         * @param file_id    �A�[�J�C�u���t�@�C��ID
         * @param lang_code  �擾����f�[�^�̌���R�[�h
         * @return �w�肵���t�@�C���̃f�[�^�T�C�Y[Byte]
         */
        //---------------------------------------------------------------------
        virtual size_t GetDataSize( u32 file_id, u32 lang_code ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �f�[�^�T�C�Y( �p�f�B���O���܂܂Ȃ� )���擾����
         * @param file_id    �A�[�J�C�u���t�@�C��ID
         * @param lang_code  �擾����f�[�^�̌���R�[�h
         * @return �w�肵���t�@�C���̃f�[�^�T�C�Y[Byte]
         */
        //---------------------------------------------------------------------
        virtual size_t GetRealDataSize( u32 file_id, u32 lang_code ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �ł��傫���f�[�^�̃T�C�Y���擾����
         */
        //---------------------------------------------------------------------
        virtual size_t GetMaxDataSize( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �A�[�J�C�u�t�@�C���擪����̃I�t�Z�b�g���擾����
         * @param file_id    �A�[�J�C�u���t�@�C��ID
         * @param lang_code  �擾����f�[�^�̌���R�[�h
         * @return �w�肵���t�@�C����, �A�[�J�C�u�t�@�C���擪����̃I�t�Z�b�g�l
         */
        //---------------------------------------------------------------------
        virtual size_t GetOffsetToData( u32 file_id, u32 lang_code ) const; 

        /**
         * @brief �A�[�J�C�u�t�@�C���փA�N�Z�X���邽�߂̏����擾����
         */
        virtual size_t GetOffsetToTotalBlockSize( void ) const                  { return 16; }
        virtual size_t GetOffsetToFileIDNum( void ) const                       { return 32; }
        virtual size_t GetOffsetToAllocationOffset( void ) const                { return 36; }
        virtual size_t GetOffsetToFileNumFromAllocationTableBlock( void ) const { return  8; }
        virtual size_t GetElementSizeOfAllocationOffsetBlock( void ) const      { return  4; }
        virtual size_t GetHeaderSizeOfAllocationTableBlock( void ) const        { return 12; }
        virtual size_t GetHeaderSizeOfAllocationTableElement( void ) const      { return  4; }
        virtual size_t GetElementSizeOfAllocationTable( void ) const            { return  8; }
        virtual size_t GetSizeOfImageBlock( void ) const                        { return 12; }



        private:
        ArcFileHeaderBlock m_file_header_block;
        ArcFileAllocationOffsetBlock m_file_allocation_offset_block;
        ArcFileAllocationTableBlock m_file_allocation_table_block;
        ArcFileImageBlock m_file_image_block;
      };


    } //namespace ver2
  } //namespace fs
} //namespace gfl

#endif //__GFL_ARCFILEACCESSOR_V2_H__
