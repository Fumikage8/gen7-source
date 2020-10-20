//=============================================================================
/**
 * @brief  アーカイブファイルのアクセッサ
 * @file   gfl_ArcFileAccessor.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_V2_H__
#define __GFL_ARCFILEACCESSOR_V2_H__ 
#include "./gfl_ArcFileAccessor_FileHeaderBlock.h"
#include "./gfl_ArcFileAccessor_FileAllocationOffsetBlock.h"
#include "./gfl_ArcFileAccessor_FileAllocationTableBlock.h"
#include "./gfl_ArcFileAccessor_FileImageBlock.h"
#include "../gfl_IArcFileAccessor.h"

namespace gfl {
  namespace fs {
    namespace ver2 {


      class ArcFileAccessor : public IArcFileAccessor
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileAccessor );

        public:

        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        //---------------------------------------------------------------------
        ArcFileAccessor( void );

        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         * @param file_data  アーカイブファイル全体のデータ
         */
        //---------------------------------------------------------------------
        ArcFileAccessor( const void* file_data );

        //---------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //---------------------------------------------------------------------
        virtual ~ArcFileAccessor( void );

        //---------------------------------------------------------------------
        /**
         * @brief アクセス対象のデータを関連付ける
         * @param arc_data アクセス対象のアーカイブデータ
         */
        //---------------------------------------------------------------------
        virtual void AttachData( const void* arc_data );

        //---------------------------------------------------------------------
        /**
         * @brief ファイルIDの数を取得する
         */
        //---------------------------------------------------------------------
        virtual u32 GetFileIdNum( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief データサイズを取得する
         * @param file_id    アーカイブ内ファイルID
         * @param lang_code  取得するデータの言語コード
         * @return 指定したファイルのデータサイズ[Byte]
         */
        //---------------------------------------------------------------------
        virtual size_t GetDataSize( u32 file_id, u32 lang_code ) const;

        //---------------------------------------------------------------------
        /**
         * @brief データサイズ( パディングを含まない )を取得する
         * @param file_id    アーカイブ内ファイルID
         * @param lang_code  取得するデータの言語コード
         * @return 指定したファイルのデータサイズ[Byte]
         */
        //---------------------------------------------------------------------
        virtual size_t GetRealDataSize( u32 file_id, u32 lang_code ) const;

        //---------------------------------------------------------------------
        /**
         * @brief 最も大きいデータのサイズを取得する
         */
        //---------------------------------------------------------------------
        virtual size_t GetMaxDataSize( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief アーカイブファイル先頭からのオフセットを取得する
         * @param file_id    アーカイブ内ファイルID
         * @param lang_code  取得するデータの言語コード
         * @return 指定したファイルの, アーカイブファイル先頭からのオフセット値
         */
        //---------------------------------------------------------------------
        virtual size_t GetOffsetToData( u32 file_id, u32 lang_code ) const; 

        /**
         * @brief アーカイブファイルへアクセスするための情報を取得する
         */
        virtual size_t GetOffsetToTotalBlockSize( void ) const                  { return 16; }
        virtual size_t GetOffsetToFileIDNum( void ) const                       { return 32; }
        virtual size_t GetOffsetToAllocationOffset( void ) const                { return 36; }
        virtual size_t GetOffsetToFileNumFromAllocationTableBlock( void ) const { return  8; }
        virtual size_t GetElementSizeOfAllocationOffsetBlock( void ) const      { return  4; }
        virtual size_t GetHeaderSizeOfAllocationTableBlock( void ) const        { return 12; }
        virtual size_t GetHeaderSizeOfAllocationTableElement( void ) const      { return  4; }
        virtual size_t GetElementSizeOfAllocationTable( void ) const            { return  8; }
        virtual size_t GetSizeOfImageBlock( void ) const                        { return 12; }



        private:
        ArcFileHeaderBlock m_file_header_block;
        ArcFileAllocationOffsetBlock m_file_allocation_offset_block;
        ArcFileAllocationTableBlock m_file_allocation_table_block;
        ArcFileImageBlock m_file_image_block;
      };


    } //namespace ver2
  } //namespace fs
} //namespace gfl

#endif //__GFL_ARCFILEACCESSOR_V2_H__
