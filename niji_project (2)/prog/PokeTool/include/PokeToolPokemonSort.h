//======================================================================
/**
 *
 *	@file		PokeToolPokemonSort.h
 *	@brief  ポケモン順番並び替え・抜き出しクラス
 *	@author	Toru=Nagihashi => niji:fukushima_yuya
 *	@date		2012.07.06 => 2015/07/30 17:24:01
 *  @par Copyright
 *  (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 *
 *	@attention  PokemonParam or CoreParamは　PokemonParamSortクラスを
 *	            Personalは　PersonalSortクラスを使う方がよいです。
 *	            本関数は上記クラスのスーパークラスです。
 *	
 *	@note 本クラスでは、以下を考慮しています。
 *	    １．ソート用バイナリデータの読み込み
 *          受け取ったheapから内部でバイナリデータを読み替えます。
 *          languageによってバイナリを読み替えることも可能です。
 *          
 *	    ２．データに依存しない
 *	        CoreParamのソートやPersonalのソートなどデータに依存させないように
 *	        本クラスを継承して使います。
 *
 *	        データに依存しないため、
 *	        ０～１００までのデータがあるとき、比較用のデータはOriginalDataメソッドでゲットします。
 *	        その元の０～１００をオリジナルデータ配列(original_data,original_index)と呼びます。
 *
 *          オリジナルデータをソート・抜き出したデータは、
 *          オリジナルデータのインデックスを並び替えた配列になります。
 *          
 *          この並び替えた後の配列をソート後データ配列(Sorted_data, sorted_index)と呼びます。
 *
 * 
 */

//======================================================================
#if !defined __POKETOOL_POKEMON_SORT_H_INCLUDED__
#define __POKETOOL_POKEMON_SORT_H_INCLUDED__
#pragma once

// niji
#include <AppLib/include/Sort/StringMonsName.h>
#include <Savedata/include/ZukanSave.h>

// gflib2
#include <macro/include/gfl2_Macros.h>

// pmlib
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN( PokeTool )

//------------------------------------------------------------------------------
// @brief   PokemonSortクラス
//------------------------------------------------------------------------------
class PokemonSort
{
  GFL_FORBID_COPY_AND_ASSIGN(PokemonSort);

public:
  //-----------------------------------------------------------------------
  // OriginalDataクラス
  //-----------------------------------------------------------------------
  class IOriginalData
  {
  public:
    virtual MonsNo GetMonsNo( u32 original_index ) = 0;
    virtual pml::PokeType GetPokeType1( u32 original_index ) = 0;
    virtual pml::PokeType GetPokeType2( u32 original_index ) = 0;
    virtual FormNo GetFormNo( u32 original_index ) = 0;
    virtual pml::personal::Color GetColor( u32 original_index ) = 0;
    
    virtual u32 GetOriginalDataMax( void ) = 0;
  };

public:
  //-----------------------------------------------------------------------
  // 定数
  //-----------------------------------------------------------------------
  static const int CONDITION_INITIAL_NONE = 0xFFFF;

public:
  //-----------------------------------------------------------------------
  // コンストラクタ・デストラクタ
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param language  言語  -> 未使用になりました
   * @param zukan_save  図鑑セーブデータ
   * @param heap  ヒープ（検索用バイナリ読み込みに使用）
   */
  PokemonSort( u32 language, const Savedata::ZukanData *zukan_save, gfl2::heap::HeapBase * heap );
  /**
   * @brief デストラクタ
   */
  virtual ~PokemonSort();
  
  //-----------------------------------------------------------------------
  // オリジナルデータのセット
  //-----------------------------------------------------------------------
  /**
   * @brief オリジナルデータ保持クラスをセット
   *
   * @param data_owner  CompareDataを継承したクラス
   */
  void SetOriginalDataOwner( IOriginalData *data_owner );

  //-----------------------------------------------------------------------
  // 並び替え結果の取得
  //-----------------------------------------------------------------------
  /**
   * @brief ソート完了データを取得
   *
   * @param sorted_index インデックス
   *
   * @return  ソート完了データのオリジナルインデックス
   */
  u32 GetSortedData( u32 sorted_index ) const;
  /**
   * @brief ソート完了データの配列長
   *
   * @return 配列長
   */
  u32 GetSortedDataMax( void ) const;
  
  //-----------------------------------------------------------------------
  // 並び替え操作
  // 
  //  Order…系の関数は順番指定です。
  //    こちらの関数を行うと並び替え操作が行われ、GetSortedData～にて結果を受け取れます。
  //  SetCondtion…系の関数は条件指定です。
  //    例えば「図鑑見たのみ」とか「タイプ炎のみ」とかの条件をつけられます。
  //    SetCondtion系を使った後にOrder…系を呼ぶと、条件＋順番にて結果が決まります。
  //
  //-----------------------------------------------------------------------
  /**
   * @brief ポケモン名にて全言語対応ソート  （開始・終了指定版）
   *        言語はアーカイブによって変更される
   *
   * @param start_index 開始インデックス  MonsNameStringSort.GetInitialIndexToSortHeaderIndexのstartの値
   * @param end_index   終了インデックス　MonsNameStringSort.GetInitialIndexToSortHeaderIndexのendの値
   * 
   * @note     用途 「あ」から「お」や「a」から「b」までのポケモンを出したい場合。
   * 
   * @caution 言語別対応必須
   */
  void OrderByMonsName( u32 start_index, u32 end_index );
  
  /**
   * @brief ポケモン名にて全言語対応ソート  （イニシャルインデックス版）
   *
   * @param 下記initial_index 「あ」だったら「あから開始するのポケモン」ぜんぶ
   *        LANG_JAPANならば、system/sort_string/initial_index_jpn.h
   *        英仏伊独西ならば、system/sort_string/initial_index_efigs.h
   *        LANG_KOREAならば、system/sort_string/initial_index_kor.h
   *
   * @return true ソート完了  false そのイニシャルインデックスにポケモンがいなかった
   *        
   * @note     用途 「あ」や「a」など頭文字１つだけ指定して、ポケモンをだしたい場合。
   *        
   * @caution 言語別対応必須
   *    
   */
  bool OrderByMonsName( u32 initial_index );
  /**
   * @brief ポケモン名にてソート（すべて版）
   * @note     用途 すべてのポケモンをだしたい場合
   */
  void OrderByMonsName( void );
  
  /**
   * @brief 全国図鑑番号順に並び替えます
   *
   */
  virtual void OrderByZenkokuNumber( void );

  /**
   * @brief 地方図鑑番号順に並び替えます。地方図鑑にでていないポケモンはソート後データに含まれません。
   * 
   * @param エリア
   */
  virtual void OrderByLocalNumber( ExtendData::LocalArea area );
  
  /**
   * @brief 図鑑のモードごとの番号順（全国図鑑番号or地方番号）に並べ替えます
   */
  virtual void OrderByZukanModeNumber( void );
  
  /**
   * @brief 図鑑リスト用に並び替えます。全国or地方は図鑑セーブの図鑑モードにより決定します。
   */
  virtual void OrderByZukanList( void );
  /**
   * @brief 体重が重い順に並び替えます。
   */
  virtual void OrderByHeavy( void );
  /**
   * @brief 体重が軽い順に並び替えます。
   */
  virtual void OrderByLight( void );
  /**
   * @brief 身長が高い順に並び替えます。
   */
  virtual void OrderByHigh( void );
  /**
   * @brief 身長が低い順に並び替えます。
   */
  virtual void OrderByLow( void );

  enum ContidionZukan
  {
    CONDITION_ZUKAN_NONE,   ///<図鑑条件なし
    CONDITION_ZUKAN_SEE,    ///<図鑑条件見た
    CONDITION_ZUKAN_CAPTURE,///<図鑑条件捕まえた
    CONDITION_ZUKAN_SEE_OR_CAPTURE,///<図鑑条件見たor捕まえた
  };
  /**
   * @brief 図鑑の条件を追加する
   *
   * @param condition 図鑑条件
   */
  void SetConditionZukan( ContidionZukan condition );

  /**
   * @brief ポケタイプの条件を追加する
   *
   * @param type1  ポケタイプ１つめの条件 type1,type2ともにPOKETYPE_NULLならば条件クリア
   * @param type2  ポケタイプ２つめの条件
   * 
   * @note  あくまでtype1,2はポケモンの２つのタイプです。
   *        （ほのお  むしを指定したらウルガモスがひっかかりガーディはひっかかりません）
   *        ほのおかむしのいずれか、という条件ではなりません。
   */
  void SetConditionPokeType( pml::PokeType type1, pml::PokeType type2 );
  
  /**
   * @brief 指定色条件を追加する
   *
   * @param color 何色のポケモンか　COLOR_MAXならば条件クリア
   */
  void SetConditionColor( pml::personal::Color color );
  
  /**
   * @brief 形状条件を追加する
   * 
   * @param style どの形状化  STYLE_TYPE_MAXならば条件クリア
   */
  void SetConditionStyle( ExtendData::StyleType style );
  
  /**
   * @brief 頭文字ポケモンのみを抜き出し条件に追加
   *
   * @param index 下記イニシャルインデックス  CONDITION_INITIAL_NONEならば条件クリア
   *        LANG_JAPANならば、system/sort_string/initial_index_jpn.h
   *        英仏伊独西ならば、system/sort_string/initial_index_efigs.h
   *        LANG_KOREAならば、system/sort_string/initial_index_kor.h
   * 
   * @caution 言語別対応必須
   * 
   * @note  OrderByMonsNameとの違いは、これは抜き出し条件のため
   *        「く」のポケモンのみ抜き出して、あいうえお順で並び替え。とか
   *        「く」のポケモンのみ抜き出して、全国図鑑番号順で並び替え。とかできます
   * 
   */
  void SetConditionInitial( u32 initial_index );
  
  /**
   * @brief 指定モンスター番号の進化系のみをだす
   *
   * @param mons モンスター番号 MONSNO_NULLならば条件クリア
   */
  void SetConditionEvolution( MonsNo monsno );
  
  /**
   * @brief 指定モンスター番号はOKとする
   *
   * @param monsno  指定モンスター番号
   */
  void SetConditionOnlyMonsNo( MonsNo monsno );
  
  /**
   * @brief 地方図鑑エリアのポケモン
   *
   * @param local_area  エリア  LOCAL_AREA_MAXだと全てのポケモン（全国）
   */
  void SetConditionLocalArea( PokeTool::ExtendData::LocalArea local_area );
  
  /**
   * @brief 条件を完全クリア
   */
  void ClearAllCondition( void );
  
  /**
   * @brief ポケモン名ソート管理クラス取得
   *
   * @return ポケモン名ソート管理クラス
   */
  const app::sort::StringMonsName *GetMonsNameStringSort( void ) const { return m_mons_sort; }
  
  /**
   * @brief ポケモン拡張データ取得
   *
   * @return ポケモン拡張データ
   */
  const ExtendData *GetPokemonExtendData( void ) const { return m_poke_data_ex; }
protected:
  typedef bool (PokemonSort::*IsConditionFunction)( u32 original_index );

protected:
  /**
   * @brief ソート用のバイナリデータを使用して並び替えを行います。
   *
   * @param sort_bin_id ソート用のびんりんかーIDを指定してください
   *        (ARCID_ZUKAN_DATAのZUKAN_SORT_DATAビンリンカーに入っている必要があります)
   *
   * @param start_index ソート用データアーカイブIDの内部データのインデックスを指定してください（開始）
   * @param end_index ソート用データアーカイブIDの内部データのインデックスを指定してください（終了）
   *
   */
  virtual void OrderByBinary( u32 sort_bin_id, u32 start_index, u32 end_index );
  
  /**
   * @brief ソート用のバイナリデータを使用して並び替えする（全て版）
   *
   * @param sort_bin_id ソート用のびんりんかーIDを指定してください
   *        (ARCID_ZUKAN_DATAのZUKAN_SORT_DATAビンリンカーに入っている必要があります)
   */
  virtual void OrderByBinary( u32 sort_bin_id );


  //ソート後データ操作
  void ClearSortedData( void );
  void AddSortedData( u32 original_index );
  
  //条件操作
  void AddConditionFunction( IsConditionFunction function );
  void RemoveConditionFunction( IsConditionFunction function );
  bool IsCondition( u32 original_index );
  
  //検索用バイナリデータ
  void LoadBinary( gfl2::fs::ArcFile::ARCDATID sort_dat_id );
  u16 GetBinaryData( u32 bin_id, u32 index ) const;
  u32 GetBinaryDataMax( u32 bin_id ) const;
  
private:
  //条件
  bool IsConditionZukanSee( u32 original_index );
  bool IsConditionZukanCapture( u32 original_index );
  bool IsConditionZukanSeeOrCapture( u32 original_index );
  bool IsConditionPokeType( u32 original_index );
  bool IsConditionPokeColor( u32 original_index );
  bool IsConditionPokeStyle( u32 original_index );
  bool IsConditionPokeInitial( u32 original_index );
  bool IsConditionZukanList( u32 original_index );
  bool IsConditionPokeEvolution( u32 original_index );
  bool IsConditionPokeOnly( u32 original_index );
  bool IsConditionLocalArea( u32 original_index );
      
private:
  static const int SORTED_DATA_TEMPORARY_LENGTH  = 1024;
  static const int SET_CONDITION_NUM  = 16;  ///<継承先にふやしたくなるかも…
 
protected:
  gfl2::heap::HeapBase * m_heap;
  const Savedata::ZukanData *m_zukan_save;
  ExtendData  *m_poke_data_ex;
  app::sort::StringMonsName  *m_mons_sort;
  IOriginalData *m_original_data;
  
private:
  u32 m_sorted_data[  SORTED_DATA_TEMPORARY_LENGTH  ];
  u32 m_sorted_data_max;
  IsConditionFunction m_condition_function[ SET_CONDITION_NUM ];
  
  gfl2::fs::BinLinkerAccessor m_bin_linker;

  void *m_sort_binary;
  u32 m_sort_binary_length;
  gfl2::fs::ArcFile::ARCDATID m_prev_sort_dat_id;
  
  pml::PokeType  m_condition_type1;
  pml::PokeType  m_condition_type2;
  pml::personal::Color m_condition_color;
  ExtendData::StyleType m_condition_style;
  u32 m_conditon_initial;
  MonsNo m_only_monsno;
  MonsNo m_evo_monsno;
  
  u32 m_tail_pokemon_no;
  

  static const int EVOLUTION_MAX  = 9;
  struct EvolutionData
  {
    u16 evolution_mons[ EVOLUTION_MAX ];
    u16 evolution_num;
  };
  const EvolutionData *m_evo_data;
  
  PokeTool::ExtendData::LocalArea m_condition_local_area;
};

GFL_NAMESPACE_END( PokeTool )

#endif // __POKETOOL_POKEMON_SORT_H_INCLUDED__
