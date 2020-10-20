//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C���̃w�b�_�u���b�N
 * @file   gfl_ArcFileAccessor_FileHeaderBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_FILEHEADERBLOCK_V3_H__
#define __GFL_ARCFILEACCESSOR_FILEHEADERBLOCK_V3_H__
#include "gfl_ArcFileAccessor_FileBlock.h"

namespace gfl {
  namespace fs {
    namespace ver3 {


      class ArcFileHeaderBlock : public ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileHeaderBlock );

        public:

        ArcFileHeaderBlock( void );
        ~ArcFileHeaderBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief �u���b�N�̃T�C�Y[Byte]���v�Z����
         */
        //---------------------------------------------------------------------
        static u32 CalcBlockSize( void );

        //---------------------------------------------------------------------
        /**
         * @brief �w�b�_�����擾����
         */
        //---------------------------------------------------------------------
        u16 ByteOrder( void ) const { return this->GetBlockDataAsHeaderStructure()->byte_order; }
        u16 Version( void ) const { return this->GetBlockDataAsHeaderStructure()->version; }
        u16 BlockNum( void ) const { return this->GetBlockDataAsHeaderStructure()->block_num; }
        u32 TotalBlockSize( void ) const { return this->GetBlockDataAsHeaderStructure()->total_block_size; }
        u32 FileSize( void ) const { return this->GetBlockDataAsHeaderStructure()->file_size; }
        u32 MaxDataSize( void ) const { return this->GetBlockDataAsHeaderStructure()->max_data_size; }


        private:

        // �w�b�_�̍\��
        struct Structure
        {
          ArcFileBlock::Header header;
          u16 byte_order;        // �o�C�g�I�[�_�E�}�[�N
          u16 version;           // �A�[�J�C�u�E�o�[�W����
          u16 block_num;         // �f�[�^�u���b�N�̐�
          u16 padding;           //
          u32 total_block_size;  // �f�[�^�u���b�N�̍��v�T�C�Y
          u32 file_size;         // �A�[�J�C�u�t�@�C���S�̂̑傫��
          u32 max_data_size;     // �ł��傫���f�[�^�̃T�C�Y
        };

        /**
         * @brief �u���b�N�f�[�^���w�b�_�Ƃ��Ď擾����
         */
        const Structure* GetBlockDataAsHeaderStructure( void ) const;
      };


    } // namespace ver3
  } // namespace fs
} // namespace gfl


#endif  // __GFL_ARCFILEACCESSOR_FILEHEADERBLOCK_V3_H__
