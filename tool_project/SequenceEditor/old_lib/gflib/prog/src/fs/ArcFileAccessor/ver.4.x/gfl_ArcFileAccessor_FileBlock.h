//=============================================================================
/**
 * @brief  �A�[�J�C�u�t�@�C�����\������u���b�N�̊�{�N���X
 * @file   gfl_ArcFileAccessor_FileBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_FILEBLOCK_V4_H__
#define __GFL_ARCFILEACCESSOR_FILEBLOCK_V4_H__


namespace gfl {
  namespace fs {
    namespace ver4 {


      class ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileBlock );

        public:

        // �u���b�N�ɋ��ʂ̏��
        struct Header 
        {
          u32 signature;   // �u���b�N�̎��ʎq
          u32 block_size;  // �u���b�N�̃T�C�Y
        };

        //---------------------------------------------------------------------
        /**
         * @brief �R���X�g���N�^
         */
        //---------------------------------------------------------------------
        ArcFileBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief �f�X�g���N�^
         */
        //---------------------------------------------------------------------
        ~ArcFileBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief �u���b�N�̎��ʎq���擾����
         */
        //---------------------------------------------------------------------
        u32 Signature( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �u���b�N�̃T�C�Y[Byte]���擾����
         */
        //---------------------------------------------------------------------
        u32 BlockSize( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief �u���b�N�̃f�[�^���擾����
         */
        //---------------------------------------------------------------------
        const void* BlockData( void ) const { return m_block_data; }

        //---------------------------------------------------------------------
        /**
         * @brief �u���b�N�̃f�[�^��ݒ肷��
         */
        //---------------------------------------------------------------------
        void SetBlockData( const void* block_data ) { m_block_data = block_data; }


        private:
        const void* m_block_data;
      };


    } // namespace ver4
  } // namespace fs
} // namespace gfl

#endif //__GFL_ARCFILEACCESSOR_FILEBLOCK_V4_H__
