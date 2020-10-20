#if !defined(GFL2_FSIARCFILEACCESSOR_H_INCLUDED)
#define GFL2_FSIARCFILEACCESSOR_H_INCLUDED
#pragma once


//=============================================================================
/**
 * @brief  アーカイブファイルアクセッサのインターフェース
 * @file   gfl2_FsArchiveFileAccessor.h
 * @author obata_toshihiro
 * @date   2012.04.27
 */
//=============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


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
         * @brief 最も大きいデータのサイズを取得する(全ての言語を通して最も大きい)(パディングを含んだサイズ)
         */
        //---------------------------------------------------------------------
        virtual size_t GetMaxDataSize( void ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief 最も大きいデータのサイズを取得する(全ての言語を通して最も大きい)(パディングを除いたサイズ)
         */
        //---------------------------------------------------------------------
        virtual size_t GetMaxRealDataSize( void ) const = 0;

        //---------------------------------------------------------------------
        /**
         * @brief ユーザが指定したアラインメントを取得する
         */
        //---------------------------------------------------------------------
        virtual size_t GetAlignment( void ) const = 0;

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


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSIARCFILEACCESSOR_H_INCLUDED

