//=============================================================================
/**
 * @brief  アーカイブファイルのファイルアロケーションオフセットブロック
 * @file   gfl_ArcFileAccessor_FileAllocationOffsetBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor.h"
#include "gfl_ArcFileAccessor_FileAllocationOffsetBlock.h"

namespace gfl {
  namespace fs {
    namespace ver2 {


      //-----------------------------------------------------------------------
      /**
       * @brief ブロックのサイズ[Byte]を計算する
       * @param file_id_num  ファイルIDの数
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAllocationOffsetBlock::CalcBlockSize( u16 file_id_num )
      {
        u32 size = 
          sizeof(ArcFileAllocationOffsetBlock::Attribute) + 
          sizeof(ArcFileAllocationOffsetBlock::OffsetTable) * file_id_num;
        return size;
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileAllocationOffsetBlock::ArcFileAllocationOffsetBlock( void ) :
        ArcFileBlock()
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileAllocationOffsetBlock::~ArcFileAllocationOffsetBlock( void )
      {
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief ファイルIDの個数を取得する
       */
      //-----------------------------------------------------------------------
      u16 ArcFileAllocationOffsetBlock::FileIdNum( void ) const 
      { 
        return this->GetAttribute()->file_id_num; 
      } 

      // ブロックデータから属性部分を取り出す
      const ArcFileAllocationOffsetBlock::Attribute* ArcFileAllocationOffsetBlock::GetAttribute( void ) const
      {
        const void* block_data = this->BlockData();
        return reinterpret_cast<const ArcFileAllocationOffsetBlock::Attribute*>( block_data );
      }

      //-----------------------------------------------------------------------
      /**
       * @brief ファイルアロケーションテーブルのオフセット値を取得する
       * @param file_id  ファイルID
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAllocationOffsetBlock::AllocationOffset( u16 file_id ) const
      {
        const ArcFileAllocationOffsetBlock::OffsetTable* table = this->GetOffsetTable();
        return table[ file_id ].offset_for_allocation_table;
      } 

      // ブロックデータからテーブル部分を取り出す
      const ArcFileAllocationOffsetBlock::OffsetTable* ArcFileAllocationOffsetBlock::GetOffsetTable( void ) const
      {
        const u8* block_data = reinterpret_cast<const u8*>( this->BlockData() );
        size_t pos = sizeof(ArcFileAllocationOffsetBlock::Attribute);
        return reinterpret_cast<const ArcFileAllocationOffsetBlock::OffsetTable*>( &block_data[ pos ] );
      }


    } // namespace ver2
  } // namespace fs
} // namespace gfl
