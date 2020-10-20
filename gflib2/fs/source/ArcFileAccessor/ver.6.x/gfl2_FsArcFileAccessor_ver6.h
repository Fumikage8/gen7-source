#if !defined(V6_GFL2_FSARCFILEACCESSOR_H_INCLUDED)
#define V6_GFL2_FSARCFILEACCESSOR_H_INCLUDED
#pragma once


//=============================================================================
/**
 * @brief  アーカイブファイルのアクセッサ
 * @file   gfl2_FsArcFileAccessor.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// ArcFileAccessorのインクルード
#include "./gfl2_FsArcFileAccessor_FileHeaderBlock_ver6.h"
#include "./gfl2_FsArcFileAccessor_FileAllocationOffsetBlock_ver6.h"
#include "./gfl2_FsArcFileAccessor_FileAllocationTableBlock_ver6.h"
#include "./gfl2_FsArcFileAccessor_FileImageBlock_ver6.h"
#include "../gfl2_FsIArcFileAccessor.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
GFL_NAMESPACE_BEGIN(ver6)


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
         * @brief データサイズ( パディングを含む )を取得する
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
         * @brief 最も大きいデータのサイズを取得する(全ての言語を通して最も大きい)(パディングを含んだサイズ)
         */
        //---------------------------------------------------------------------
        virtual size_t GetMaxDataSize( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief 最も大きいデータのサイズを取得する(全ての言語を通して最も大きい)(パディングを除いたサイズ)
         */
        //---------------------------------------------------------------------
        virtual size_t GetMaxRealDataSize( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief ユーザが指定したアラインメントを取得する
         */
        //---------------------------------------------------------------------
        virtual size_t GetAlignment( void ) const;

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
        virtual size_t GetOffsetToFileIDNum( void ) const                       { return 36; }
        virtual size_t GetOffsetToAllocationOffset( void ) const                { return 40; }
        virtual size_t GetOffsetToFileNumFromAllocationTableBlock( void ) const { return  8; }
        virtual size_t GetElementSizeOfAllocationOffsetBlock( void ) const      { return  4; }
        virtual size_t GetHeaderSizeOfAllocationTableBlock( void ) const        { return 12; }
        virtual size_t GetHeaderSizeOfAllocationTableElement( void ) const      { return  4; }
        virtual size_t GetElementSizeOfAllocationTable( void ) const            { return 12; }
        virtual size_t GetSizeOfImageBlock( void ) const                        { return 12; }



        private:
        ArcFileHeaderBlock m_file_header_block;
        ArcFileAllocationOffsetBlock m_file_allocation_offset_block;
        ArcFileAllocationTableBlock m_file_allocation_table_block;
        ArcFileImageBlock m_file_image_block;
      };


GFL_NAMESPACE_END(ver6)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // V6_GFL2_FSARCFILEACCESSOR_H_INCLUDED
