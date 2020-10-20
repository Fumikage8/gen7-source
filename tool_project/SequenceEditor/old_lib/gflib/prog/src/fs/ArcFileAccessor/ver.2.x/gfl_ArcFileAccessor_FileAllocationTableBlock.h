//=============================================================================
/**
 * @brief  アーカイブファイルのファイルアロケーションテーブルブロック
 * @file   gfl_ArcFileAccessor_FileAllocationTableBlock.h
 * @author obata_toshihiro
 * @date   2011.05.09
 */
//=============================================================================
#ifndef __GFL_ARCFILEACCESSOR_FILEALLOCATIONTABLEBLOCK_V2_H__
#define __GFL_ARCFILEACCESSOR_FILEALLOCATIONTABLEBLOCK_V2_H__
#include "gfl_ArcFileAccessor_FileBlock.h"


namespace gfl {
  namespace fs {
    namespace ver2 {


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


    } // namespace ver2
  } // namespace fs
} // namespace gfl


#endif  // __GFL_ARCFILEACCESSOR_FILEALLOCATIONTABLEBLOCK_V2_H__
