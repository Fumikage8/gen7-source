//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃t�@�C���C���[�W�u���b�N
 * @file   gfl_ArcFileAccessor_FileImageBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_FILEIMAGEBLOCK_V3_H__
#define __GFL_ARCFILEACCESSOR_FILEIMAGEBLOCK_V3_H__
#include "gfl_ArcFileAccessor_FileBlock.h"


namespace gfl {
  namespace fs {
    namespace ver3 {


      class ArcFileImageBlock : public ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileImageBlock );
        friend class ArcFileFactory;

        public:

        //---------------------------------------------------------------------
        /**
         * @brief �u���b�N�̃T�C�Y[Byte]���v�Z����
         */
        //---------------------------------------------------------------------
        static u32 CalcBlockSize( void ); 

        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        //---------------------------------------------------------------------
        ArcFileImageBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //---------------------------------------------------------------------
        ~ArcFileImageBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief �C���[�W�f�[�^�̃T�C�Y���擾����
         */
        //---------------------------------------------------------------------
        u32 ImageSize( void ) const;



        private:

        // �u���b�N�̍\��
        struct Attribute 
        {
          ArcFileBlock::Header header;  // �u���b�N�̃w�b�_
          u32 image_size;            // �t�@�C���C���[�W�̑傫��
        };

        /**
         * @brief �u���b�N�f�[�^���\���f�[�^�Ƃ��Ď擾����
         */
        const Attribute* GetAttribute( void ) const;
      };


    } // namespace ver3
  } // namespace fs
} // namespace gfl


#endif  // __GFL_ARCFILEACCESSOR_FILEIMAGEBLOCK_V3_H__
