#if !defined(V4_GFL2_FSARCFILEACCESSOR_FILEBLOCK_H_INCLUDED)
#define V4_GFL2_FSARCFILEACCESSOR_FILEBLOCK_H_INCLUDED
#pragma once


//=============================================================================
/**
 * @brief  アーカイブファイルを構成するブロックの基本クラス
 * @file   gfl2_FsArcFileAccessor_FileBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
GFL_NAMESPACE_BEGIN(ver4)


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


GFL_NAMESPACE_END(ver4)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // V4_GFL2_FSARCFILEACCESSOR_FILEBLOCK_H_INCLUDED
