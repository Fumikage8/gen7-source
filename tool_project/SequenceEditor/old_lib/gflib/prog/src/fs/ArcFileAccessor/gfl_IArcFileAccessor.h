//=============================================================================
/**
 * @brief  アーカイブファイルアクセッサのインターフェース
 * @file   gfl_ArchiveFileAccessor.h
 * @author obata_toshihiro
 * @date   2012.04.27
 */
//=============================================================================
#ifndef __GFL_IARCFILEACCESSOR_H__
#define __GFL_IARCFILEACCESSOR_H__ 

namespace gfl {
  namespace fs {


    class IArcFileAccessor 
    {
      public:
        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        //---------------------------------------------------------------------
        IArcFileAccessor( void );

        //---------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //---------------------------------------------------------------------
        virtual ~IArcFileAccessor();

        //---------------------------------------------------------------------
        /**
         * @brief アクセス対象のデータを関連付ける
         * @param arc_data アクセス対象のアーカイブデータ
         */
        //---------------------------------------------------------------------
        virtual void AttachData( const void* arc_data ) = 0;

        //---------------------------------------------------------------------
        /**
         * @brief ファイルIDの数を取得する
         */
        //---------------------------------------------------------------------
        virtual u32 GetFileIdNum( void ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief データサイズを取得する
         * @param file_id    アーカイブ内ファイルID
         * @param lang_code  取得するデータの言語コード
         * @return 指定したファイルのデータサイズ[Byte]
         */
        //---------------------------------------------------------------------
        virtual size_t GetDataSize( u32 file_id, u32 lang_code ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief データサイズ( パディングを含まない )を取得する
         * @param file_id    アーカイブ内ファイルID
         * @param lang_code  取得するデータの言語コード
         * @return 指定したファイルのデータサイズ[Byte]
         */
        //---------------------------------------------------------------------
        virtual size_t GetRealDataSize( u32 file_id, u32 lang_code ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief 最も大きいデータのサイズを取得する
         */
        //---------------------------------------------------------------------
        virtual size_t GetMaxDataSize( void ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief アーカイブファイル先頭からのオフセットを取得する
         * @param file_id    アーカイブ内ファイルID
         * @param lang_code  取得するデータの言語コード
         * @return 指定したファイルの, アーカイブファイル先頭からのオフセット値
         */
        //---------------------------------------------------------------------
        virtual size_t GetOffsetToData( u32 file_id, u32 lang_code ) const = 0;

        /**
         * @brief アーカイブファイルへアクセスするための情報を取得する
         */
        virtual size_t GetOffsetToTotalBlockSize( void ) const = 0;
        virtual size_t GetOffsetToFileIDNum( void ) const = 0;
        virtual size_t GetOffsetToAllocationOffset( void ) const = 0;
        virtual size_t GetOffsetToFileNumFromAllocationTableBlock( void ) const = 0;
        virtual size_t GetElementSizeOfAllocationOffsetBlock( void ) const = 0;
        virtual size_t GetHeaderSizeOfAllocationTableBlock( void ) const = 0;
        virtual size_t GetHeaderSizeOfAllocationTableElement( void ) const = 0;
        virtual size_t GetElementSizeOfAllocationTable( void ) const = 0;
        virtual size_t GetSizeOfImageBlock( void ) const = 0;
    };


  } //namespace fs
} //namespace gfl

#endif // __GFL_IARCFILEACCESSOR_H__
