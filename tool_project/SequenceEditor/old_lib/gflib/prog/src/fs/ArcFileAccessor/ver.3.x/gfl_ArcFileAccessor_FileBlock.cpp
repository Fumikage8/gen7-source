//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C�����\������u���b�N�̊�{�N���X
 * @file   gfl_ArcFileAccessor_FileBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor_FileBlock.h"


namespace gfl {
  namespace fs {
    namespace ver3 {


      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileBlock::ArcFileBlock( void ) :
        m_block_data( NULL )
      {
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileBlock::~ArcFileBlock( void )
      {
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief �u���b�N�̎��ʎq���擾����
       */
      //-----------------------------------------------------------------------
      u32 ArcFileBlock::Signature( void ) const
      {
        const ArcFileBlock::Header* header = reinterpret_cast<const ArcFileBlock::Header*>( m_block_data );
        return header->signature;
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �u���b�N�̃T�C�Y[Byte]���擾����
       */
      //-----------------------------------------------------------------------
      u32 ArcFileBlock::BlockSize( void ) const
      {
        const ArcFileBlock::Header* header = reinterpret_cast<const ArcFileBlock::Header*>( m_block_data );
        return header->block_size;
      }


    } // namespace ver3
  } // namespace fs
} // namespace gfl
