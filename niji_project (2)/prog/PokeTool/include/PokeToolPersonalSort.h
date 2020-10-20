//======================================================================
/**
 *
 *  @file     PokeToolPersonalSort.h
 *  @brief    パーソルデータソートクラス
 *  @author   Toru=Nagihashi => niji:fukushima_yuya
 *  @date     2012.07.06 => 2015/07/30 17:20:19
 *  @par Copyright
 *  (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 *
 *  @note  
 *    制限
 *      フォルムは全て０としてソートします
 *
 *    使い方
 *    １．生成 GFL_NEWで生成してください
 *    ２．条件づけ    SetCondition～系で条件指定してください(PokemonSortクラスのメソッドです)
 *    ３．ソート開始  OrderBy～系で並び順を指定しソートしてください(PokemonSortクラスのメソッドです)
 *    ４．ソートされた結果受け取り。下記受け取りでモンスター番号がソートされています。
 *        for( int i=0; i<personal_sort->GetSortedDataMax(); ++i )
 *        {
 *          MonsNo mons_no = personal_sort->GetSortedMonsNo( i )
 *        }
 *    ３．破棄 GFL_DELETE
 *
 *    example
 *      例えば、図鑑登録されたポケモンを名前「あ」のみのポケモンを取得する場合
 *
 *      //生成
 *      sort  = GFL_NEW( heap ) poke_tool::PersonalSort( 
 *                                xy_system::GetLang(), zukan_save, heap );
 *                                
 *      //見たことがあるもののみに条件づけ
 *      sort->SetConditionZukan( poke_tool::PokemonSort::CONDITION_ZUKAN_SEE );  
 *      
 *      //あ～おまで順に並び替え　！実行！
 *      sort->OrderByAIUEO( ZKN_SORT_AIUEO_IDX_START_A, ZKN_SORT_AIUEO_IDX_END_O );
 *      
 *      //結果はGetSortedMonsNoで受け取る
 *      GFL_PRINT( "「あ～お」のみの、見たことのあるポケモン\n" );
 *      for( int i=0; i<personal_sort->GetSortedDataMax(); ++i )
 *      {
 *        MonsNo mons_no = personal_sort->GetSortedMonsNo( i )
 *        GFL_PRINT( "モンスター番号%d\n", mons_no );
 *      }
 *
 *      //破棄
 *      GFL_DELETE sort
 *
 */
//======================================================================
#if !defined __POKETOOL_PERSONAL_SORT_H_INCLUDED__
#define __POKETOOL_PERSONAL_SORT_H_INCLUDED__
#pragma once

// niji
#include "PokeToolPokemonSort.h"


GFL_NAMESPACE_BEGIN( PokeTool )

//------------------------------------------------------------------------------
// @brief   PersonalSortクラス
//------------------------------------------------------------------------------
class PersonalSort
  : public PokemonSort
  , public PokemonSort::IOriginalData
{
  GFL_FORBID_COPY_AND_ASSIGN(PersonalSort);

public:
  //-----------------------------------------------------------------------
  // 定数
  //-----------------------------------------------------------------------
  enum LoadType
  {
    LOAD_TYPE_PART, ///ソートの際にすこしづつパーソナル読む(ソート時の処理負荷高）
    LOAD_TYPE_FULL, ///最初に一気にパーソナルを読み込む（メモリ使用量高）
  };
  
  enum Mode
  {
    MODE_NORMAL,  //すべてフォルム０にて計算する
    MODE_ZUKAN,   //図鑑に登録されているフォルムで計算する
  };

public:
  //-----------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param language  言語
   * @param zukan_save  図鑑セーブデータ
   * @param heap  ヒープ
   * @param load_type ロードの種類
   */ 
  PersonalSort( u32 language, const Savedata::ZukanData *zukan_save, gfl2::heap::HeapBase * heap, LoadType load_type, Mode mode = MODE_NORMAL );
  /**
   * @brief デストラクタ
   */
  virtual ~PersonalSort();
  
  //-----------------------------------------------------------------------
  // 取得
  //-----------------------------------------------------------------------
  /**
   * @brief ソートされた結果受け取り
   *
   * @param index インデックス（０～GetSortedDataMax() ）
   *
   * @return モンスターナンバー
   * 
   * @note  constじゃないのは内部でLoadPesonalを行う可能性（モードによる）があるからです
   */
  MonsNo GetSortedMonsNo( u32 index );
  
  /**
   * @brief LoadTypeがFullだった際、再読み込みをする
   */
  void ReloadFullData( void );

public:
  //-----------------------------------------------------------------------
  // IOriginalDataクラス実装
  //-----------------------------------------------------------------------
  virtual MonsNo GetMonsNo( u32 original_index );
  virtual pml::PokeType GetPokeType1( u32 original_index );
  virtual pml::PokeType GetPokeType2( u32 original_index );
  virtual FormNo GetFormNo( u32 original_index );
  virtual pml::personal::Color GetColor( u32 original_index );

  virtual u32 GetOriginalDataMax( void );

private:
  void SetModeFunction( LoadType load_type );

  void CreatePersonalDataFull( gfl2::heap::HeapBase * heap );
  void DeletePersonalDataFull( void );
  pml::personal::PersonalData * GetPersonalDataFull( u32 index );
  
  void CreatePersonalDataPart( gfl2::heap::HeapBase * heap );
  void DeletePersonalDataPart( void );
  pml::personal::PersonalData * GetPersonalDataPart( u32 index );
  
private:
  struct DrawData
  {
    FormNo  formno;
  };

  union
  {
    pml::personal::PersonalData *m_personal_data;
    pml::personal::PersonalData **m_personal_array;
  };
  DrawData  *m_draw_data;
  void (PersonalSort::*CreatePersonalData)( gfl2::heap::HeapBase * heap );
  void (PersonalSort::*DeletePersonalData)( void );
  pml::personal::PersonalData * (PersonalSort::*GetPersonalData)( u32 index );
  Mode  m_mode;
  LoadType m_load_type;
};

GFL_NAMESPACE_END( PokeTool )

#endif // __POKETOOL_PERSONAL_SORT_H_INCLUDED__