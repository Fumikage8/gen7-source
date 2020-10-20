#if !defined(V6_GFL2_FSARCFILEACCESSOR_FILEALLOCATIONTABLEBLOCK_H_INCLUDED)
#define V6_GFL2_FSARCFILEACCESSOR_FILEALLOCATIONTABLEBLOCK_H_INCLUDED
#pragma once


//=============================================================================
/**
 * @brief  アーカイブファイルのファイルアロケーションテーブルブロック
 * @file   gfl2_FsArcFileAccessor_FileAllocationTableBlock.h
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


      class ArcFileAllocationTableBlock : public ArcFileBlock
      {
        GFL_FORBID_COPY_AND_ASSIGN( ArcFileAllocationTableBlock );

        public:

        //---------------------------------------------------------------------
        /**
         * @brief ブロックのサイズ[Byte]を計算する
         * @param file_num     言語違いを含めた全てのファイルの数
         * @param file_id_num  ファイルIDの数
         */
        //---------------------------------------------------------------------
        static u32 CalcBlockSize( u32 file_num, u16 file_id_num );

        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        //---------------------------------------------------------------------
        ArcFileAllocationTableBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //---------------------------------------------------------------------
        ~ArcFileAllocationTableBlock( void );

        //---------------------------------------------------------------------
        /**
         * @brief 言語違いを含めた全てのファイルの数を取得する
         */
        //---------------------------------------------------------------------
        u32 FileNum( void ) const;

        //---------------------------------------------------------------------
        /**
         * @brief 各言語専用のデータがあるかどうかを表すビット列を取得する
         */
        //---------------------------------------------------------------------
        u32 LanguageBit( u32 table_offset ) const; 

        //---------------------------------------------------------------------
        /**
         * @brief ファイル先頭へのオフセット値を取得する
         */
        //---------------------------------------------------------------------
        u32 FileImageOffsetToFileTop( u32 table_offset, u32 language ) const;

        //---------------------------------------------------------------------
        /**
         * @brief ファイル末尾へのオフセット値を取得する
         */
        //---------------------------------------------------------------------
        u32 FileImageOffsetToFileBottom( u32 table_offset, u32 language ) const;

        //---------------------------------------------------------------------
        /**
         * @brief ファイルのパディングを含まないサイズを取得する
         */
        //---------------------------------------------------------------------
        u32 FileRealSize( u32 table_offset, u32 language ) const;

        //---------------------------------------------------------------------
        /**
         * @brief 指定した言語専用のファイルが存在するかどうかを調べる
         *
         * @param table_offset  チェック対象のアロケーションテーブルの先頭へのオフセット
         * @param language      チェック対象の言語 
         *
         * @retval true   存在する
         * @retval false  存在しない
         */
        //---------------------------------------------------------------------
        bool HaveLanguageFile( u32 table_offset, u32 language ) const;




        private:

        // ブロックの構成( 属性部 )
        struct Attribute 
        {
          ArcFileBlock::Header header;
          u32 file_num;           // 言語違いを含めた全てのファイル数
        };

        // ブロックの構成( テーブルヘッダ部 )
        struct AllocationTableHeader 
        {
          u32 language_bit;  // 各言語専用のデータがあるかどうかのビット列
        };

        // ブロックの構成( テーブル部 )
        struct AllocationTableElement 
        {
          u32 file_top;     // ファイル先頭へのオフセット
          u32 file_bottom;  // ファイル末尾へのオフセット + 1
          u32 real_size;    // ファイルの実サイズ( パディングを含まないサイズ )
        };

        /**
         * @brief ブロックデータから属性部分を取り出す
         */
        const Attribute* GetAttribute( void ) const;

        /**
         * @brief ブロックデータからテーブルヘッダ部分を取り出す
         *
         * @param table_offset  アロケーションテーブルへのオフセット値
         */
        const AllocationTableHeader* GetAllocationTableHeader( u32 table_offset ) const;

        /**
         * @brief ブロックデータからテーブル部分を取り出す
         *
         * @param table_offset  アロケーションテーブルへのオフセット値
         */
        const AllocationTableElement* GetAllocationTable( u32 table_offset ) const;

        /**
         * @brief 言語専用ファイルの, アロケーションテーブル中のインデックスを取得する
         */
        u16 GetLanguageFileIndex( const AllocationTableHeader* table_header, u32 language ) const;

        /**
         * @brief 言語専用ファイルが存在するかどうかを調べる
         */
        bool CheckLanguageFileExist( const AllocationTableHeader* table_header, u32 language ) const;
      };


GFL_NAMESPACE_END(ver6)
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // V6_GFL2_FSARCFILEACCESSOR_FILEALLOCATIONTABLEBLOCK_H_INCLUDED
