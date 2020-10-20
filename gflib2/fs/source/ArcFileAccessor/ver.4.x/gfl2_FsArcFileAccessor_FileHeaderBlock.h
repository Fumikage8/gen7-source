#if !defined(V4_GFL2_FSARCFILEACCESSOR_FILEHEADERBLOCK_H_INCLUDED)
#define V4_GFL2_FSARCFILEACCESSOR_FILEHEADERBLOCK_H_INCLUDED
#pragma once


//=============================================================================
/**
 * @brief  アーカイブファイルのヘッダブロック
 * @file   gfl2_FsArcFileAccessor_FileHeaderBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// ArcFileAccessorのインクルード
#include "./gfl2_FsArcFileAccessor_FileBlock.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
GFL_NAMESPACE_BEGIN(ver4)


      class ArcFileHeaderBlock : public ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileHeaderBlock );

        public:

        ArcFileHeaderBlock( void );
        ~ArcFileHeaderBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief ブロックのサイズ[Byte]を計算する
         */
        //---------------------------------------------------------------------
        static u32 CalcBlockSize( void );

        //---------------------------------------------------------------------
        /**
         * @brief ヘッダ情報を取得する
         */
        //---------------------------------------------------------------------
        u16 ByteOrder( void ) const { return this->GetBlockDataAsHeaderStructure()->byte_order; }
        u16 Version( void ) const { return this->GetBlockDataAsHeaderStructure()->version; }
        u16 BlockNum( void ) const { return this->GetBlockDataAsHeaderStructure()->block_num; }
        u32 TotalBlockSize( void ) const { return this->GetBlockDataAsHeaderStructure()->total_block_size; }
        u32 FileSize( void ) const { return this->GetBlockDataAsHeaderStructure()->file_size; }
        u32 MaxDataSize( void ) const { return this->GetBlockDataAsHeaderStructure()->max_data_size; }


        private:

        // ヘッダの構成
        struct Structure
        {
          ArcFileBlock::Header header;
          u16 byte_order;        // バイトオーダ・マーク
          u16 version;           // アーカイブ・バージョン
          u16 block_num;         // データブロックの数
          u16 padding;           //
          u32 total_block_size;  // データブロックの合計サイズ
          u32 file_size;         // アーカイブファイル全体の大きさ
          u32 max_data_size;     // 最も大きいデータのサイズ
        };

        /**
         * @brief ブロックデータをヘッダとして取得する
         */
        const Structure* GetBlockDataAsHeaderStructure( void ) const;
      };


GFL_NAMESPACE_END(ver4)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // V4_GFL2_FSARCFILEACCESSOR_FILEHEADERBLOCK_H_INCLUDED
