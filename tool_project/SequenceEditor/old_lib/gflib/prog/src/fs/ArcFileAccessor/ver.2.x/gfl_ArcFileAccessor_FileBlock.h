//=============================================================================
/**
 * @brief  アーカイブファイルを構成するブロックの基本クラス
 * @file   gfl_ArcFileAccessor_FileBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_FILEBLOCK_V2_H__
#define __GFL_ARCFILEACCESSOR_FILEBLOCK_V2_H__


namespace gfl {
  namespace fs {
    namespace ver2 {


      class ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileBlock );

        public:

        // ブロックに共通の情報
        struct Header 
        {
          u32 signature;   // ブロックの識別子
          u32 block_size;  // ブロックのサイズ
        };

        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        //---------------------------------------------------------------------
        ArcFileBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //---------------------------------------------------------------------
        ~ArcFileBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief ブロックの識別子を取得する
         */
        //---------------------------------------------------------------------
        u32 Signature( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief ブロックのサイズ[Byte]を取得する
         */
        //---------------------------------------------------------------------
        u32 BlockSize( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief ブロックのデータを取得する
         */
        //---------------------------------------------------------------------
        const void* BlockData( void ) const { return m_block_data; }

        //---------------------------------------------------------------------
        /**
         * @brief ブロックのデータを設定する
         */
        //---------------------------------------------------------------------
        void SetBlockData( const void* block_data ) { m_block_data = block_data; }


        private:
        const void* m_block_data;
      };


    } // namespace ver2
  } // namespace fs
} // namespace gfl

#endif //__GFL_ARCFILEACCESSOR_FILEBLOCK_V2_H__
