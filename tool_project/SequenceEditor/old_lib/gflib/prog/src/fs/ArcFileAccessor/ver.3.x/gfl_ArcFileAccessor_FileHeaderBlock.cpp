//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃w�b�_�u���b�N
 * @file   gfl_ArcFileAccessor_FileHeaderBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor.h"
#include "gfl_ArcFileAccessor_FileHeaderBlock.h"


namespace gfl {
  namespace fs {
    namespace ver3 {


      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileHeaderBlock::ArcFileHeaderBlock( void ) :
        ArcFileBlock()
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      ArcFileHeaderBlock::~ArcFileHeaderBlock( void )
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief �u���b�N�̃T�C�Y[Byte]���v�Z����
       */
      //-----------------------------------------------------------------------
      u32 ArcFileHeaderBlock::CalcBlockSize( void )
      {
        return sizeof( ArcFileHeaderBlock::Structure );
      }

      /**
       * @brief �u���b�N�f�[�^���w�b�_�Ƃ��Ď擾����
       */
      const ArcFileHeaderBlock::Structure* ArcFileHeaderBlock::GetBlockDataAsHeaderStructure( void ) const
      {
        const void* block_data = this->BlockData();
        return reinterpret_cast<const ArcFileHeaderBlock::Structure*>( block_data );
      }


    } // namespace ver3
  } // namespace fs
} // namespace gfl
