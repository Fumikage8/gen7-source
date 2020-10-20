//=============================================================================
/**
 * @brief  アーカイブファイルのアクセッサ
 * @file   gfl2_FsArcFileAccessor.cpp
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================


// ArcFileAccessorのインクルード
#include "./gfl2_FsArcFileAccessor_FileHeaderBlock_ver6.h"
#include "./gfl2_FsArcFileAccessor_FileAllocationOffsetBlock_ver6.h"
#include "./gfl2_FsArcFileAccessor_FileAllocationTableBlock_ver6.h"
#include "./gfl2_FsArcFileAccessor_FileImageBlock_ver6.h"
#include "./gfl2_FsArcFileAccessor_ver6.h"
#include "../gfl2_FsIArcFileAccessor.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
GFL_NAMESPACE_BEGIN(ver6)


      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileAccessor::ArcFileAccessor( void ) :
        IArcFileAccessor(),
        m_file_header_block(),
        m_file_allocation_offset_block(),
        m_file_allocation_table_block(),
        m_file_image_block()
      {
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param file_data  アーカイブファイル全体のデータ
       */
      //-----------------------------------------------------------------------
      ArcFileAccessor::ArcFileAccessor( const void* file_data ) : 
        IArcFileAccessor(),
        m_file_header_block(),
        m_file_allocation_offset_block(),
        m_file_allocation_table_block(),
        m_file_image_block()
      {
        this->AttachData( file_data );
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      ArcFileAccessor::~ArcFileAccessor( void )
      {
      }

      //-----------------------------------------------------------------------
      /**
       * @brief アクセス対象のデータを関連付ける
       * @param arc_data アクセス対象のアーカイブデータ
       */
      //-----------------------------------------------------------------------
      void ArcFileAccessor::AttachData( const void* arc_data )
      {
        // ヘッダブロック
        uptr header_block_address = reinterpret_cast<uptr>( arc_data );
        m_file_header_block.SetBlockData( arc_data );

        // ファイルアロケーションオフセットブロック
        size_t header_block_size =  ArcFileHeaderBlock::CalcBlockSize();
        uptr allocation_offset_block_address = header_block_address + header_block_size;
        void* allocation_offset_block_data = reinterpret_cast<void*>( allocation_offset_block_address );
        m_file_allocation_offset_block.SetBlockData( allocation_offset_block_data );

        // ファイルアロケーションテーブルブロック
        u32 file_id_num = m_file_allocation_offset_block.FileIdNum();
        size_t allocation_offset_block_size = ArcFileAllocationOffsetBlock::CalcBlockSize( file_id_num );
        uptr allocation_table_block_address = allocation_offset_block_address + allocation_offset_block_size;
        void* allocation_table_block_data = reinterpret_cast<void*>( allocation_table_block_address );
        m_file_allocation_table_block.SetBlockData( allocation_table_block_data );

        // ファイルイメージブロック
        u32 file_num = m_file_allocation_table_block.FileNum();
        size_t allocation_table_block_size = ArcFileAllocationTableBlock::CalcBlockSize( file_num, file_id_num );
        uptr file_image_block_address = allocation_table_block_address + allocation_table_block_size;
        void* file_image_block_data = reinterpret_cast<void*>( file_image_block_address );
        m_file_image_block.SetBlockData( file_image_block_data );
      } 

      //-----------------------------------------------------------------------
      /**
       * @brief ファイルIDの数を取得する
       */
      //-----------------------------------------------------------------------
      u32 ArcFileAccessor::GetFileIdNum( void ) const
      {
        return m_file_allocation_offset_block.FileIdNum();
      }

      //-----------------------------------------------------------------------
      /**
       * @brief データサイズ( パディングを含む )を取得する
       * @param file_id    アーカイブ内ファイルID
       * @param lang_code  取得するデータの言語コード
       * @return 指定したファイルのデータサイズ[Byte]
       */
      //-----------------------------------------------------------------------
      size_t ArcFileAccessor::GetDataSize( u32 file_id, u32 lang_code ) const
      {
        u32 allocation_table_offset = m_file_allocation_offset_block.AllocationOffset( file_id );
        u32 image_top_offset = m_file_allocation_table_block.FileImageOffsetToFileTop( allocation_table_offset, lang_code );
        u32 image_bottom_offset = m_file_allocation_table_block.FileImageOffsetToFileBottom( allocation_table_offset, lang_code );
        size_t data_size = image_bottom_offset - image_top_offset;
        return data_size;
      }

      //-----------------------------------------------------------------------
      /**
       * @brief データサイズ( パディングを含まない )を取得する
       * @param file_id    アーカイブ内ファイルID
       * @param lang_code  取得するデータの言語コード
       * @return 指定したファイルのデータサイズ[Byte]
       */
      //-----------------------------------------------------------------------
      size_t ArcFileAccessor::GetRealDataSize( u32 file_id, u32 lang_code ) const
      {
        u32 allocation_table_offset = m_file_allocation_offset_block.AllocationOffset( file_id );
        size_t real_size = m_file_allocation_table_block.FileRealSize( allocation_table_offset, lang_code );
        return real_size;
      }

      //---------------------------------------------------------------------
      /**
       * @brief 最も大きいデータのサイズを取得する(全ての言語を通して最も大きい)(パディングを含んだサイズ)
       */
      //---------------------------------------------------------------------
      size_t ArcFileAccessor::GetMaxDataSize( void ) const
      {
        return m_file_header_block.MaxDataSize();
      }

      //---------------------------------------------------------------------
      /**
       * @brief 最も大きいデータのサイズを取得する(全ての言語を通して最も大きい)(パディングを除いたサイズ)
       */
      //---------------------------------------------------------------------
      size_t ArcFileAccessor::GetMaxRealDataSize( void ) const
      {
        return m_file_header_block.MaxRealDataSize();
      }

      //---------------------------------------------------------------------
      /**
       * @brief ユーザが指定したアラインメントを取得する
       */
      //---------------------------------------------------------------------
      size_t ArcFileAccessor::GetAlignment( void ) const
      {
        return m_file_header_block.Alignment();
      }

      //-----------------------------------------------------------------------
      /**
       * @brief アーカイブファイル先頭からのオフセットを取得する
       * @param file_id    アーカイブ内ファイルID
       * @param lang_code  取得するデータの言語コード
       * @return 指定したファイルの, アーカイブファイル先頭からのオフセット値
       */
      //-----------------------------------------------------------------------
      size_t ArcFileAccessor::GetOffsetToData( u32 file_id, u32 lang_code ) const
      {
        u32 allocation_table_offset = m_file_allocation_offset_block.AllocationOffset( file_id );
        u32 image_top_offset = m_file_allocation_table_block.FileImageOffsetToFileTop( allocation_table_offset, lang_code );
        size_t offset_from_file_top = m_file_header_block.TotalBlockSize() + image_top_offset;
        return offset_from_file_top;
      }


GFL_NAMESPACE_END(ver6)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)
