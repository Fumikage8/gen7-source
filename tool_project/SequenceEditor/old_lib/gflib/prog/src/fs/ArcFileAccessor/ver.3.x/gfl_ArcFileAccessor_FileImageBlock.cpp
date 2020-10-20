//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃t�@�C���C���[�W�u���b�N
 * @file   gfl_ArcFileAccessor_FileImageBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor.h"
#include "gfl_ArcFileAccessor_FileImageBlock.h" 


namespace gfl {
  namespace fs {
    namespace ver3 {


      //-----------------------------------------------------------------------
      /**
       * @brief �u���b�N�̃T�C�Y[Byte]���v�Z����
       */
      //-----------------------------------------------------------------------
      u32 ArcFileImageBlock::CalcBlockSize( void )
      {
        return  sizeof(ArcFileImageBlock::Attribute);
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileImageBlock::ArcFileImageBlock( void ) :
        ArcFileBlock()
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileImageBlock::~ArcFileImageBlock( void )
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �C���[�W�f�[�^�̃T�C�Y���擾����
       */
      //-----------------------------------------------------------------------
      u32 ArcFileImageBlock::ImageSize( void ) const 
      { 
        return this->GetAttribute()->image_size; 
      } 

      /**
       * @brief �u���b�N�f�[�^���\���f�[�^�Ƃ��Ď擾����
       */
      const ArcFileImageBlock::Attribute* ArcFileImageBlock::GetAttribute( void ) const
      {
        const void* block_data = this->BlockData();
        return reinterpret_cast<const ArcFileImageBlock::Attribute*>( block_data );
      }


    } // namespace ver3
  } // namespace fs
} // namespace gfl
