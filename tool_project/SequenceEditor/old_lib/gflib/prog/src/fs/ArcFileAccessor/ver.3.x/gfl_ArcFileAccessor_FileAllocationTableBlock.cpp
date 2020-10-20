//=============================================================================
/**
 * @brief  アーカイブファイルのファイルアロケーションテーブルブロック
 * @file   gfl_ArcFileAccessor_FileAllocationTableBlock.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#include "gfl_ArcFileAccessor.h"
#include "gfl_ArcFileAccessor_FileAllocationTableBlock.h"


namespace gfl {
  namespace fs {
    namespace ver3 { 


      //-----------------------------------------------------------------------
      /**
       * @brief ブロックのサイズ[Byte]を計算する
       * @param file_num     言語違いを含めた全てのファイルの数
       * @param file_id_num  ファイルIDの数
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::CalcBlockSize( u32 file_num, u16 file_id_num )
      {
        u32 size = 
          sizeof(ArcFileAllocationTableBlock::Attribute) + 
          sizeof(ArcFileAllocationTableBlock::AllocationTableHeader) * file_id_num +
          sizeof(ArcFileAllocationTableBlock::AllocationTableElement) * file_num;
        return size;
      }

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileAllocationTableBlock::ArcFileAllocationTableBlock( void ) : 
        ArcFileBlock()
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileAllocationTableBlock::~ArcFileAllocationTableBlock( void )
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief 言語違いを含めた全てのファイルの数を取得する
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::FileNum( void ) const 
      {
        return this->GetAttribute()->file_num; 
      }

      // ブロックデータから属性部分を取り出す
      const ArcFileAllocationTableBlock::Attribute* ArcFileAllocationTableBlock::GetAttribute( void ) const
      {
        const void* block_data = this->BlockData();
        return reinterpret_cast<const ArcFileAllocationTableBlock::Attribute*>( block_data );
      }

      //--------------------------------------------------------------------------
      /**
       * @brief 各言語専用ファイルが存在するかどうかを表すビット列を取得する
       */
      //--------------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::LanguageBit( u32 table_offset ) const
      { 
        return this->GetAllocationTableHeader( table_offset )->language_bit;
      }

      // ブロックデータからテーブルヘッダ部分を取り出す
      const ArcFileAllocationTableBlock::AllocationTableHeader* ArcFileAllocationTableBlock::GetAllocationTableHeader( u32 table_offset ) const
      {
        const u8* block_data = reinterpret_cast<const u8*>( this->BlockData() );
        u32 header_pos = sizeof(ArcFileAllocationTableBlock::Attribute) + table_offset;
        return reinterpret_cast<const ArcFileAllocationTableBlock::AllocationTableHeader*>( &block_data[ header_pos ] );
      } 

      //--------------------------------------------------------------------------
      /**
       * @brief ファイル先頭へのオフセット値を取得する
       */
      //--------------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::FileImageOffsetToFileTop( u32 table_offset, u32 language ) const
      {
        const ArcFileAllocationTableBlock::AllocationTableHeader* table_header = this->GetAllocationTableHeader( table_offset );
        const ArcFileAllocationTableBlock::AllocationTableElement* allocation_table = this->GetAllocationTable( table_offset );
        u16 language_file_index = this->GetLanguageFileIndex( table_header, language );
        return allocation_table[ language_file_index ].file_top;
      } 

      //--------------------------------------------------------------------------
      /**
       * @brief ファイル末尾へのオフセット値を取得する
       */
      //--------------------------------------------------------------------------
      u32 ArcFileAllocationTableBlock::FileImageOffsetToFileBottom( u32 table_offset, u32 language ) const
      {
        const ArcFileAllocationTableBlock::AllocationTableHeader* table_header = this->GetAllocationTableHeader( table_offset );
        const ArcFileAllocationTableBlock::AllocationTableElement* allocation_table = this->GetAllocationTable( table_offset );
        u16 language_file_index = this->GetLanguageFileIndex( table_header, language );
        return allocation_table[ language_file_index ].file_bottom;
      } 

      // 言語専用ファイルの, アロケーションテーブル中のインデックスを取得する
      u16 ArcFileAllocationTableBlock::GetLanguageFileIndex( const AllocationTableHeader* table_header, u32 language ) const
      {
        if( !this->CheckLanguageFileExist( table_header, language ) ) {
          return 0;  // 言語専用ファイルがない場合は, デフォルト用のファイルを参照する
        }

        u16 index = 0;
        for( u16 check_language=0; check_language<language; check_language++ )
        {
          if( this->CheckLanguageFileExist( table_header, check_language ) )
          {
            index++;
          }
        }
        return index;
      }

      //--------------------------------------------------------------------------
      /**
       * @brief 指定した言語専用のファイルが存在するかどうかを調べる
       *
       * @param table_offset  チェック対象のアロケーションテーブルの先頭へのオフセット
       * @param language      チェック対象の言語 
       *
       * @retval true   存在する
       * @retval false  存在しない
       */
      //--------------------------------------------------------------------------
      bool ArcFileAllocationTableBlock::HaveLanguageFile( u32 table_offset, u32 language ) const
      {
        const ArcFileAllocationTableBlock::AllocationTableHeader* table_header = this->GetAllocationTableHeader( table_offset );
        return this->CheckLanguageFileExist( table_header, language );
      }

      // 言語専用ファイルが存在するかどうかを調べる
      bool ArcFileAllocationTableBlock::CheckLanguageFileExist( const AllocationTableHeader* table_header, u32 language ) const
      {
        u32 check_bit = 1 << language;
        return ( table_header->language_bit & check_bit ) != 0;
      } 

      // ブロックデータからテーブル部分を取り出す
      const ArcFileAllocationTableBlock::AllocationTableElement* ArcFileAllocationTableBlock::GetAllocationTable( u32 table_offset ) const
      {
        const u8* block_data = reinterpret_cast<const u8*>( this->BlockData() );
        u32 table_pos = sizeof(ArcFileAllocationTableBlock::Attribute) + table_offset + sizeof(ArcFileAllocationTableBlock::AllocationTableHeader);
        return reinterpret_cast<const ArcFileAllocationTableBlock::AllocationTableElement*>( &block_data[ table_pos ] );
      }


    } // namespace ver3
  } // namespace fs
} // namespace gfl
