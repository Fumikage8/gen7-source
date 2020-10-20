//=============================================================================
/**
 * @brief  アーカイブファイルのファイルアロケーションオフセットブロック
 * @file   gfl_ArcFileAccessor_FileAllocationOffsetBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_FILEALLOCATIONOFFSETBLOCK_V3_H__
#define __GFL_ARCFILEACCESSOR_FILEALLOCATIONOFFSETBLOCK_V3_H__
#include "gfl_ArcFileAccessor_FileBlock.h" 

namespace gfl {
  namespace fs {
    namespace ver3 {


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


    } //namespace ver3
  } //namespace fs
} //namespace gfl

#endif  //__GFL_ARCFILEACCESSOR_FILEALLOCATIONOFFSETBLOCK_V3_H__
