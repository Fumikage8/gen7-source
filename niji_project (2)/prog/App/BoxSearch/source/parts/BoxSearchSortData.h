﻿#if !defined( __APP_BOX_SEARCH_SORT_DATA_H_INCLUDED__ )
#define __APP_BOX_SEARCH_SORT_DATA_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchSortData.h
 * @date    2015/07/30 18:35:39
 * @author  fukushima_yuya
 * @brief   ボックス検索：ソートデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// pml
#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( sort )
class StringBinary;
class StringTokusei;
class StringWaza;
GFL_NAMESPACE_END( sort )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( PokeTool )
class PersonalSort;
GFL_NAMESPACE_END( PokeTool )

GFL_NAMESPACE_BEGIN( Savedata )
class ZukanData;
GFL_NAMESPACE_END( Savedata )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
//static const u32 LIST_MAX_KANA_CONSONANT  = 10 + 1;   //!< 日本語子音リスト数
//static const u32 LIST_MAX_KANA_VOWEL      = 5;        //!< 日本語母音リスト数
//static const u32 LIST_MAX_HANGUL          = 14 + 1;   //!< ハングル文字リスト数
//static const u32 LIST_MAX_ALPHABET        = 26 + 1;   //!< アルファベットリスト数
//static const u32 LIST_MAX_SIMPLIFIED      = 23 + 1;   //!< 簡体字リスト数
//static const u32 LIST_MAX_TRADITIONAL     = 35 + 1;   //!< 繁体字リスト数

static const u32 STR_HED_LIST2_MAX = 64;

//------------------------------------------------------------------------------
// @brief   検索条件ソートデータクラス
//------------------------------------------------------------------------------
class SortData
{
  GFL_FORBID_COPY_AND_ASSIGN( SortData );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  SortData( gfl2::heap::HeapBase* pHeap, Savedata::ZukanData* pZukanData, u8 lang );

  //------------------------------------------------------------------
  /**
   * @brief	  デストラクタ
   */
  //------------------------------------------------------------------
  ~SortData( void );


//------------------------------------------------------------------------------
// @brief   ポケモン名ソートデータ
//------------------------------------------------------------------------------
public:
  //------------------------------------------------------------------
  /**
   * @brief	  パーソナルソートの生成
   */
  //------------------------------------------------------------------
  void CreatePersonalSort( void );

  //------------------------------------------------------------------
  /**
   * @brief	  パーソナルソートの破棄
   */
  //------------------------------------------------------------------
  void DeletePersonalSort( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  ポケモン名の頭文字ごとのソートデータ設定
   *
   * @param   con     子音
   * @param   vowMax  母音項目数
   */
  //------------------------------------------------------------------
  void InitPokemonSortItemMax( u32 con, u32 vowMax );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモン名の母音項目数の取得
   *
   * @param   index   母音インデックス
   *
   * @return  母音項目数
   */
  //------------------------------------------------------------------
  u32 GetPokemonSortVowelNum( u32 index ) const;

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモン名ソートデータの取得
   *
   * @param   pos   位置
   * @return  モンスターNo
   */
  //------------------------------------------------------------------
  MonsNo GetPokemonSortItemData( u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモン名ソートデータの頭文字を設定
   *
   * @param   con   子音
   * @param   vow   母音
   */
  //------------------------------------------------------------------
  void SetInitialPokemonSortData( u32 con, u32 vow );


//------------------------------------------------------------------------------
// @brief   特性ソートデータ
//------------------------------------------------------------------------------
public:
  //------------------------------------------------------------------
  /**
   * @brief	  特性ソートデータの生成
   */
  //------------------------------------------------------------------
  void CreateTokuseiSort( void );

  //------------------------------------------------------------------
  /**
   * @brief	  特性ソートデータの破棄
   */
  //------------------------------------------------------------------
  void DeleteTokuseiSort( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  特性の頭文字ごとのソートデータ設定
   *
   * @param   con     子音
   * @param   vowMax  母音項目数
   */
  //------------------------------------------------------------------
  void InitTokuseiSortItemMax( u32 con, u32 vowMax );

  //------------------------------------------------------------------
  /**
   * @brief	  母音の項目数の取得
   *
   * @param   con     子音
   *
   * @return  母音項目数
   */
  //------------------------------------------------------------------
  u32 GetTokuseiSortVowelNum( u32 con ) const;

public:
  //------------------------------------------------------------------
  /**
   * @brief	  特性データテーブル生成
   *
   * @param   con   子音
   * @param   vow   母音
   */
  //------------------------------------------------------------------
  void CreateTokuseiSortData( u32 con, u32 vow );

  //------------------------------------------------------------------
  /**
   * @brief	  特性データテーブル削除
   */
  //------------------------------------------------------------------
  void DeleteTokuseiSortData( void );

  //------------------------------------------------------------------
  /**
   * @brief   特性IDの取得
   *
   * @param   index   ソートデータの配列インデックス
   *
   * @return  特性ID
   */
  //------------------------------------------------------------------
  TokuseiNo GetTokuseiNo( u32 index ) const;

  //------------------------------------------------------------------
  /**
   * @brief   特性ソートデータ数取得
   *
   * @return  特性ソートデータ数
   */
  //------------------------------------------------------------------
  u32 GetTokuseiCount( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief	  特性ソートデータの取得
   * @param   con   子音
   * @param   vow   母音
   * @param   pos   リスト位置
   * @return  特性
   */
  //------------------------------------------------------------------
  TokuseiNo GetTokuseiSortData( u32 con, u32 vow, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief	  特性ソートデータ数取得
   * @param   con   子音
   * @param   vow   母音
   */
  //------------------------------------------------------------------
  u32 GetTokuseiSortMax( u32 con, u32 vow );


//------------------------------------------------------------------------------
// @brief   技ソートデータ
//------------------------------------------------------------------------------
public:
  //------------------------------------------------------------------
  /**
   * @brief	  技ソートデータ生成
   */
  //------------------------------------------------------------------
  void CreateWazaSort( void );

  //------------------------------------------------------------------
  /**
   * @brief	  技ソートデータ破棄
   */
  //------------------------------------------------------------------
  void DeleteWazaSort( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  技の頭文字ごとのソートデータ設定
   *
   * @param   con     子音
   * @param   vowMax  母音項目数
   */
  //------------------------------------------------------------------
  void InitWazaSortItemMax( u32 con, u32 vowMax );

  //------------------------------------------------------------------
  /**
   * @brief	  母音の項目数の取得
   *
   * @param   con     子音
   *
   * @return  母音項目数
   */
  //------------------------------------------------------------------
  u32 GetWazaSortVowelNum( u32 con ) const;

public:
  //------------------------------------------------------------------
  /**
   * @brief	  技データテーブル生成
   *
   * @param   con   子音
   * @param   vow   母音
   */
  //------------------------------------------------------------------
  void CreateWazaSortData( u32 con, u32 vow );

  //------------------------------------------------------------------
  /**
   * @brief	  技データテーブル破棄
   */
  //------------------------------------------------------------------
  void DeleteWazaSortData( void );

  //------------------------------------------------------------------
  /**
   * @brief   技Noの取得
   *
   * @param   index   ソートデータの配列インデックス
   *
   * @return  技No
   */
  //------------------------------------------------------------------
  WazaNo GetWazaNo( u32 index ) const;

  //------------------------------------------------------------------
  /**
   * @brief   技ソートデータ数取得
   *
   * @return  技ソートデータ数
   */
  //------------------------------------------------------------------
  u32 GetWazaCount( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief	  技ソートデータ取得
   *
   * @param   con   子音
   * @param   vow   母音
   * @param   pos   リスト位置
   *
   * @return  技
   */
  //------------------------------------------------------------------
  WazaNo GetWazaSortData( u32 con, u32 vow, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief	  技ソートデータ数取得
   *
   * @param   con   子音
   * @param   vow   母音
   *
   * @return  データ数
   */
  //------------------------------------------------------------------
  u32 GetWazaSortMax( u32 con, u32 vow );


//------------------------------------------------------------------------------
// @brief   共通処理
//------------------------------------------------------------------------------
private:
  //------------------------------------------------------------------
  /**
   * @brief	  頭文字ソートテーブルからデータを取得
   *
   * @param   sort    ソートデータ
   * @param   id      頭文字
   * @param   pos     データ位置
   *
   * @return  "STRING_SORT_NONE" == データなし
   * @return  "STRING_SORT_NONE" != データ番号
   */
  //------------------------------------------------------------------
  u32 GetStringSortData( app::sort::StringBinary* sort, u32 id, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief	  頭文字ソートテーブルからデータ数を取得
   *
   * @param   sort    ソートデータ
   * @param   id      頭文字ID
   *
   * @return  データ数
   */
  //------------------------------------------------------------------
  u32 GetStringSortMax( app::sort::StringBinary* sort, u32 id );

private:
  gfl2::heap::HeapBase*       m_pDevHeap;

  Savedata::ZukanData*        m_pZukanData;

private:
  PokeTool::PersonalSort*     m_pPersonalSort;

private:
  app::sort::StringTokusei*   m_pTokuseiSort;

  TokuseiNo*                  m_pTokuseiNoSortData;

  u32                         m_TokuseiSortDataMax;

private:
  app::sort::StringWaza*      m_pWazaSort;

  WazaNo*                     m_pWazaNoSortData;

  u32                         m_WazaSortDataMax;

private:
  u32   m_VowelItemMax[STR_HED_LIST2_MAX];

  u8    m_Language;
};

GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

#endif // __APP_BOX_SEARCH_SORT_DATA_H_INCLUDED__