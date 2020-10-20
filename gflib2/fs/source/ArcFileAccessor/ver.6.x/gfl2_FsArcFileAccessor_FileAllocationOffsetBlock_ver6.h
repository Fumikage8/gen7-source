#if !defined(V6_GFL2_FSARCFILEACCESSOR_FILEALLOCATIONOFFSETBLOCK_H_INCLUDED)
#define V6_GFL2_FSARCFILEACCESSOR_FILEALLOCATIONOFFSETBLOCK_H_INCLUDED
#pragma once


//=============================================================================
/**
 * @brief  アーカイブファイルのファイルアロケーションオフセットブロック
 * @file   gfl2_FsArcFileAccessor_FileAllocationOffsetBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// ArcFileAccessorのインクルード
#include "./gfl2_FsArcFileAccessor_FileBlock_ver6.h" 


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
GFL_NAMESPACE_BEGIN(ver6)


      class ArcFileAllocationOffsetBlock : public ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileAllocationOffsetBlock );

        public:

        //---------------------------------------------------------------------
        /**
         * @brief ブロックのサイズ[Byte]を計算する
         * @param file_id_num  ファイルIDの数
         */
        //---------------------------------------------------------------------
        static u32 CalcBlockSize( u16 file_id_num );

        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        //---------------------------------------------------------------------
        ArcFileAllocationOffsetBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //---------------------------------------------------------------------
        ~ArcFileAllocationOffsetBlock( void ); 

        //---------------------------------------------------------------------
        /**
         * @brief ファイルIDの個数を取得する
         */
        //---------------------------------------------------------------------
        u16 FileIdNum( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief ファイルアロケーションテーブルのオフセット値を取得する
         * @param file_id  ファイルID
         */
        //---------------------------------------------------------------------
        u32 AllocationOffset( u16 file_id ) const;



        private:

        // ブロックの構成( 属性部 )
        struct Attribute 
        {
          ArcFileBlock::Header header;
          u16 file_id_num;  // ファイルIDの個数
          u16 padding;      // 
        };

        // ブロックの構成( テーブル部 )
        struct OffsetTable 
        {
          u32 offset_for_allocation_table;  // ファイルアロケーションテーブルのオフセット
        };

        /**
         * @brief ブロックデータから属性部分を取り出す
         */
        const Attribute* GetAttribute( void ) const;

        /**
         * @brief ブロックデータからテーブル部分を取り出す
         */
        const OffsetTable* GetOffsetTable( void ) const;
      };


GFL_NAMESPACE_END(ver6)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // V6_GFL2_FSARCFILEACCESSOR_FILEALLOCATIONOFFSETBLOCK_H_INCLUDED
