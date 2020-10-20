//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���A�N�Z�b�T�̃C���^�[�t�F�[�X
 * @file   gfl_ArchiveFileAccessor.h
 * @author obata_toshihiro
 * @date   2012.04.27
 */
//=============================================================================
#ifndef __GFL_IARCFILEACCESSOR_H__
#define __GFL_IARCFILEACCESSOR_H__ 

namespace gfl {
  namespace fs {


    class IArcFileAccessor 
    {
      public:
        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        //---------------------------------------------------------------------
        IArcFileAccessor( void );

        //---------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //---------------------------------------------------------------------
        virtual ~IArcFileAccessor();

        //---------------------------------------------------------------------
        /**
         * @brief �A�N�Z�X�Ώۂ̃f�[�^���֘A�t����
         * @param arc_data �A�N�Z�X�Ώۂ̃A�[�J�C�u�f�[�^
         */
        //---------------------------------------------------------------------
        virtual void AttachData( const void* arc_data ) = 0;

        //---------------------------------------------------------------------
        /**
         * @brief �t�@�C��ID�̐����擾����
         */
        //---------------------------------------------------------------------
        virtual u32 GetFileIdNum( void ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief �f�[�^�T�C�Y���擾����
         * @param file_id    �A�[�J�C�u���t�@�C��ID
         * @param lang_code  �擾����f�[�^�̌���R�[�h
         * @return �w�肵���t�@�C���̃f�[�^�T�C�Y[Byte]
         */
        //---------------------------------------------------------------------
        virtual size_t GetDataSize( u32 file_id, u32 lang_code ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief �f�[�^�T�C�Y( �p�f�B���O���܂܂Ȃ� )���擾����
         * @param file_id    �A�[�J�C�u���t�@�C��ID
         * @param lang_code  �擾����f�[�^�̌���R�[�h
         * @return �w�肵���t�@�C���̃f�[�^�T�C�Y[Byte]
         */
        //---------------------------------------------------------------------
        virtual size_t GetRealDataSize( u32 file_id, u32 lang_code ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief �ł��傫���f�[�^�̃T�C�Y���擾����
         */
        //---------------------------------------------------------------------
        virtual size_t GetMaxDataSize( void ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief �A�[�J�C�u�t�@�C���擪����̃I�t�Z�b�g���擾����
         * @param file_id    �A�[�J�C�u���t�@�C��ID
         * @param lang_code  �擾����f�[�^�̌���R�[�h
         * @return �w�肵���t�@�C����, �A�[�J�C�u�t�@�C���擪����̃I�t�Z�b�g�l
         */
        //---------------------------------------------------------------------
        virtual size_t GetOffsetToData( u32 file_id, u32 lang_code ) const = 0;

        /**
         * @brief �A�[�J�C�u�t�@�C���փA�N�Z�X���邽�߂̏����擾����
         */
        virtual size_t GetOffsetToTotalBlockSize( void ) const = 0;
        virtual size_t GetOffsetToFileIDNum( void ) const = 0;
        virtual size_t GetOffsetToAllocationOffset( void ) const = 0;
        virtual size_t GetOffsetToFileNumFromAllocationTableBlock( void ) const = 0;
        virtual size_t GetElementSizeOfAllocationOffsetBlock( void ) const = 0;
        virtual size_t GetHeaderSizeOfAllocationTableBlock( void ) const = 0;
        virtual size_t GetHeaderSizeOfAllocationTableElement( void ) const = 0;
        virtual size_t GetElementSizeOfAllocationTable( void ) const = 0;
        virtual size_t GetSizeOfImageBlock( void ) const = 0;
    };


  } //namespace fs
} //namespace gfl

#endif // __GFL_IARCFILEACCESSOR_H__
