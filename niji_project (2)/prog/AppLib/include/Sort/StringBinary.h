//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		StringSortBinary.h
 *	@brief  文字列ソート用のバイナリ管理クラス
 *	@author	Toru=Nagihashi
 *	@date		2012.09.27
 *
 *	●これは何？
 *	  
 *	  ポケモン名、技名等、下記の操作をすることがあります。
 *	  １．ポケモン名の昇順ソート
 *	  ２．頭文字からポケモン名を検索する
 *
 *	  上記は言語依存別処理が必要ですが、
 *	  本クラスを使用することで、修正箇所を抑えることが可能です。
 *
 *  ●用語解説
 *  ・頭文字インデックス(InitialIndex)
 *      日本語ならば「あが０」、「いが１」、「うが２」のように
 *      頭文字を番号化したものです。
 *      
 *      
 *  ●使い方と例
 *  
 *    １．全文ポケモンの昇順がしりたい
 *    for( int i = 0; i < hoge.GetSortOrderMax(); ++i )
 *    {
 *      MonsNo monsno = hoge.GetSortOrder(i);
 *      //厳密にはMonsNoが入っているのではなく、mstxtのラベルインデックスが入っているが、ラベルインデックス＝MONSNOという仕様なのでOK
 *      
 *      GFL_PRINT( "優先順位[%d] ポケモン番号[%d]", i, monsno );
 *    }
 *  
 *    ２．「あ〜お」までのポケモン番号がしりたい
 *    u32 start, end;
 *    if( hoge.GetInitialIndexToSortHeaderIndex( INITIAL_INDEX_JPN_AA, start, end ) )
 *    {
 *      //maxではなくendまでなので<=にする
 *      for( int i = start; i <= end; ++i )
 *      {
 *        MonsNo monsno = hoge.GetSortOrder(i);
 *        //厳密にはMonsNoが入っているのではなく、mstxtのラベルインデックスが入っているが、ラベルインデックス＝MONSNOという仕様なのでOK
 *        GFL_PRINT( "あ行のポケモン番号[%d]\n", monsno );
 *      }
 *    }
 *    else
 *    {
        GFL_PRINT( "あ行のポケモンはいなかった\n" );
 *    }
 *
 *
 *    ３．ポケモンの頭文字インデックスがしりたい
 *    u32 sort_priority = hoge.GetInitialIndex( MONSNO_HUSIGIDANE );
 *    //頭文字インデックスは単純にフシギダネだったら「フ」の頭文字インデックスがくるだけ
 *    //なので、フシギダネとフシギバナの差はない（ソート順とは異なる）のに注意
 *
 *	
 *	●実装仕様
 *    文字列のソート用バイナリはGFArchiverにて言語別のファイルを固めている。
 *    もしもAの言語のときにBの言語のソート用バイナリを使いたい場合は、
 *    上記を変更する必要がある。
 *
 *    sort_string.rbを使用しコンバートしたデータであり
 *	  すべてu16のテーブルになっている
 *
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( STRING_SORT_H_INCLUDED )
#define STRING_SORT_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
// resource
#include "niji_conv_header/app/data/sort_string/initial_index_jpn.h"
#include "niji_conv_header/app/data/sort_string/initial_index_efigs.h"
#include "niji_conv_header/app/data/sort_string/initial_index_kor.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)

class StringBinary
{
  GFL_FORBID_COPY_AND_ASSIGN(StringBinary);
  //-----------------------------------------------------------------------
  // 定数
  //-----------------------------------------------------------------------
public:
  /**
   * @brier 頭出しインデックスがない場合
   *        （あに含まれる文字列がない場合）
   */
  static const int SORT_HEADER_INDEX_NONE = 0xFFFF;
  
public:
  //-----------------------------------------------------------------------
  // 生成、破棄
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  StringBinary();
  /**
   * @brief デストラクタ
   */
  virtual ~StringBinary();

  //-----------------------------------------------------------------------
  // 読み込み
  //-----------------------------------------------------------------------
  /**
   * @brief 同期読み込み
   *
   * @param heap  ヒープ
   * @param initial_index_datid イニシャルインデックスデータ用アーカイブID
   * @param sort_table_datid  ソートテーブルデータ用アーカイブID
   * @param initial_to_sort_datid 頭文字インデックスから頭出しインデックス取得用テーブルアーカイブID
   * 
   * @note  Load後、新たにLoadしたらきちんと読み込まれる。
   */
  void LoadSync( gfl2::heap::HeapBase * heap, gfl2::fs::ArcFile::ARCDATID initial_index_datid, gfl2::fs::ArcFile::ARCDATID sort_table_datid, gfl2::fs::ArcFile::ARCDATID initial_to_sort_datid );

  /**
   * @brief 非同期読み込み設定
   *
   * @param heap  ヒープ
   * @param work_heap 作業用ヒープ
   * @param initial_index_datid イニシャルインデックスデータ用アーカイブID
   * @param sort_table_datid  ソートテーブルデータ用アーカイブID
   * @param initial_to_sort_datid 頭文字インデックスから頭出しインデックス取得用テーブルアーカイブID
   * 
   * @note  LoadMain()で読み込んでください
   */
  void LoadSetup( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * work_heap, gfl2::fs::ArcFile::ARCDATID initial_index_datid, gfl2::fs::ArcFile::ARCDATID sort_table_datid, gfl2::fs::ArcFile::ARCDATID initial_to_sort_datid );

  /**
   * @brief 非同期読み込み
   * 
   * @retval  true  終了
   * @retval  false それ以外
   */
  bool LoadMain(void);
  
  //-----------------------------------------------------------------------
  // 取得
  //-----------------------------------------------------------------------
  /**
   * @brief ソート順番を取得する
   *
   * @param index 順番（0が先頭、MAXが後尾）
   *
   * @return 中身（例えば特性のテーブルならば特性NOが入っている）
   */
  u32 GetSortOrder( u32 index ) const;
  
  /**
   * @brief ソート順番の最大を取得する
   *
   * @return 順番最大
   */
  u32 GetSortOrderMax( void ) const;

  /**
   * @brief 頭文字インデックスを取得する
   *
   * @param index 中身（例えば特性ならば特性NOを入れる）
   *
   * @return  頭文字インデックス（インデックスの定義は言語別にinitial_index_xxx.hを見ること）
   */
  u32 GetInitialIndex( u32 index ) const;
  
  /**
   * @brief 上記イニシャルインデクス取得用のバッファ最大をとる
   * 
   * @note 例えばMONSNO用ならばGetLength＝MONSNO_MAX
   */
  u32 GetInitialIndexLength( void ) const;
  /**
   * @brief 頭文字インデックスの最大を取得
   *
   * @return  頭文字インデックスの最大  日本語なら46 EFIGSなら26 韓国語なら13
   */
  u32 GetInitialIndexMax( void ) const;
  
  /**
   * @brief 頭文字インデックスから、ソート順番用の頭出しインデックスを取得する
   *
   * @param initial_index 頭出しインデックス（インデックスの定義は言語別にinitial_index_xxx.hを見ること）
   * @param start SortOrder用 頭文字開始インデックス
   * @param end SortOrder用 頭文字終了インデックス
   * 
   * @retval true その頭文字に対応するインデックスがあった
   * @retval false  その頭文字に対応するインデックスがない
   */
  bool GetInitialIndexToSortHeaderIndex( u32 initial_index, u32 & start, u32 & end ) const;


private:
  /**
   * @brief 非同期読み込みリクエスト
   * 
   * @param dat_id  アークデータID
   */
  void LoadRequest( gfl2::fs::ArcFile::ARCDATID dat_id );

  
private:
  struct InitialToSortHeader
  {
    u16 start;
    u16 end;
  };
  
private:
  u16 *m_sort_table;
  u32 m_sort_table_size;
  u16 *m_initial_index;
  u32 m_initial_index_size;
  InitialToSortHeader *m_initial_to_sort;

  gfl2::heap::HeapBase * m_heap;
  gfl2::heap::HeapBase * m_workHeap;
  gfl2::fs::ArcFile::ARCDATID m_sortTableID;
  gfl2::fs::ArcFile::ARCDATID m_initialIndexID;
  gfl2::fs::ArcFile::ARCDATID m_initialToSortID;
  void * m_tmpBuffer;
  size_t m_tmpSize;
  u32 m_seq;
};


GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)

#endif		// STRING_SORT_H_INCLUDED
