//======================================================================
/**
 * @file    PokeToolPokemonSort.cpp
 * @author  Toru=Nagihashi => niji:fukushima_yuya
 * @date    2012.07.06 => 2015/07/30 17:45:09
 * @brief   ポケモン順番並び替え・抜き出しクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "PokeTool/include/PokeToolPokemonSort.h"
#include <System/include/ArcIdGetter.h>
#include <GameSys/include/GameManager.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/zukan_data.gaix>
#include <niji_conv_header/app/zukan_data/zkn_sort_data.h>


GFL_NAMESPACE_BEGIN( PokeTool )

//-----------------------------------------------------------------------
// コンストラクタ・デストラクタ
//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param language  言語
 * @param zukan_save  図鑑セーブデータ
 * @param heap  ヒープ（検索用バイナリ読み込みに使用）
 */
PokemonSort::PokemonSort( u32 language, const Savedata::ZukanData *zukan_save, gfl2::heap::HeapBase * heap ) :
  m_heap( heap ),
  m_zukan_save( zukan_save ),
  m_poke_data_ex( NULL ),
  m_mons_sort( NULL ),
  m_original_data( NULL ),
  m_sorted_data(),
  m_sorted_data_max( 0 ),
  m_condition_function(),
  m_bin_linker(),
  m_sort_binary( NULL ),
  m_sort_binary_length( 0 ),
  m_prev_sort_dat_id( gfl2::fs::ArcFile::ARCDATID_NULL ),
  m_condition_type1(POKETYPE_NULL),
  m_condition_type2(POKETYPE_NULL),
  m_condition_color( pml::personal::COLOR_RED ),
  m_condition_style( ExtendData::STYLE_TYPE_CIRCLE ),
  m_conditon_initial( 0 ),
  m_only_monsno( MONSNO_NULL ),
  m_evo_monsno( MONSNO_NULL ),
  m_tail_pokemon_no( 0 ),
  m_evo_data( NULL ),
  m_condition_local_area( PokeTool::ExtendData::LOCAL_AREA_MAX )
{
  // poke_lib/pmlib/prog/include/poketype_def.h  PokeType_tag  POKETYPE_****  と
  // poke_lib/pmlib/prog/include/monsno_def.h  FORMNO_ARUSEUSU_****  の並び順は同じ
  // でなければならないので、ずれていないかチェックしておく。
  {
    STATIC_ASSERT( POKETYPE_NORMAL  == FORMNO_ARUSEUSU_NORMAL   );
    STATIC_ASSERT( POKETYPE_KAKUTOU == FORMNO_ARUSEUSU_BATTLE   );
    STATIC_ASSERT( POKETYPE_HIKOU   == FORMNO_ARUSEUSU_HIKOU    );
    STATIC_ASSERT( POKETYPE_DOKU    == FORMNO_ARUSEUSU_POISON   );
    STATIC_ASSERT( POKETYPE_JIMEN   == FORMNO_ARUSEUSU_JIMEN    );
    STATIC_ASSERT( POKETYPE_IWA     == FORMNO_ARUSEUSU_IWA      );
    STATIC_ASSERT( POKETYPE_MUSHI   == FORMNO_ARUSEUSU_MUSHI    );
    STATIC_ASSERT( POKETYPE_GHOST   == FORMNO_ARUSEUSU_GHOST    );
    STATIC_ASSERT( POKETYPE_HAGANE  == FORMNO_ARUSEUSU_METAL    );
    STATIC_ASSERT( POKETYPE_HONOO   == FORMNO_ARUSEUSU_FIRE     );
    STATIC_ASSERT( POKETYPE_MIZU    == FORMNO_ARUSEUSU_WATER    );
    STATIC_ASSERT( POKETYPE_KUSA    == FORMNO_ARUSEUSU_KUSA     );
    STATIC_ASSERT( POKETYPE_DENKI   == FORMNO_ARUSEUSU_ELECTRIC );
    STATIC_ASSERT( POKETYPE_ESPER   == FORMNO_ARUSEUSU_ESPER    );
    STATIC_ASSERT( POKETYPE_KOORI   == FORMNO_ARUSEUSU_KOORI    );
    STATIC_ASSERT( POKETYPE_DRAGON  == FORMNO_ARUSEUSU_DRAGON   );
    STATIC_ASSERT( POKETYPE_AKU     == FORMNO_ARUSEUSU_AKU      );
    STATIC_ASSERT( POKETYPE_FAIRY   == FORMNO_ARUSEUSU_FAIRY    );
  }


  m_poke_data_ex = GFL_NEW( heap ) PokeTool::ExtendData( heap );
  m_poke_data_ex->Load( heap );
  
  m_mons_sort = GFL_NEW( heap ) app::sort::StringMonsName();
  m_mons_sort->Load( heap );
 
  // ファイル同期オープン＆ロード＆クローズ
  {
    gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

    // オープン
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId       = NIJI_ARCID_NUM(ARCID_ZUKAN_DATA);
    openReq.heapForFile = heap->GetLowerHandle();  // すぐにクローズするので、一時的にしか使われない。
    openReq.heapForReq  = heap->GetLowerHandle();
    fileManager->SyncArcFileOpen(openReq);

    // ロード
    const gfl2::fs::ArcFile* arc_file = fileManager->GetArcFile( NIJI_ARCID_NUM(ARCID_ZUKAN_DATA) );
    size_t size = 0;
    arc_file->GetDataSize( &size, GARC_zukan_data_zkn_sort_data_PACK, NULL );
    m_sort_binary = GflHeapAllocMemoryAlign( heap, size, 4 );
 
    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq loadReq;
    loadReq.arcId   = NIJI_ARCID_NUM(ARCID_ZUKAN_DATA);
    loadReq.datId   = GARC_zukan_data_zkn_sort_data_PACK;
    loadReq.pBuf    = m_sort_binary;
    loadReq.bufSize = size;
    fileManager->SyncArcFileLoadDataBuf(loadReq);
 
    // クローズ
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;  // 同期版だからヒープはNULLでよい。
    closeReq.arcId       = NIJI_ARCID_NUM(ARCID_ZUKAN_DATA);
    fileManager->SyncArcFileClose(closeReq);
  }

  m_bin_linker.Initialize( m_sort_binary );
  
  m_evo_data  = static_cast<const EvolutionData*>( m_bin_linker.GetData( BL_IDX_ZKN_SORT_DATA_ZUKAN_EVOLUTION_DAT ) );
}

/**
 * @brief デストラクタ
 */
PokemonSort::~PokemonSort()
{
  if( m_sort_binary )
  {
    GflHeapSafeFreeMemory( m_sort_binary );
  }
  GFL_SAFE_DELETE( m_mons_sort );
  GFL_SAFE_DELETE( m_poke_data_ex );
}

//-----------------------------------------------------------------------
// オリジナルデータのセット
//-----------------------------------------------------------------------
/**
 * @brief オリジナルデータ保持クラスをセット
 *
 * @param data_owner  CompareDataを継承したクラス
 */
void PokemonSort::SetOriginalDataOwner( IOriginalData *data_owner )
{
  m_original_data = data_owner;
}

//-----------------------------------------------------------------------
// ソート後データの取得
//-----------------------------------------------------------------------
/**
 * @brief ソート完了データを取得
 *
 * @param sorted_index インデックス
 *
 * @return  ソート完了データのオリジナルインデックス
 */
u32 PokemonSort::GetSortedData( u32 sorted_index ) const
{
  if( !(sorted_index < this->GetSortedDataMax()) )
  {
    GFL_ASSERT( 0 );  //@fix
    return 0;
  }
 
  if( m_original_data == NULL )
  {
    GFL_ASSERT( 0 );  //@fix
    return 0;
  }

  return m_sorted_data[ sorted_index ];
}

/**
 * @brief ソート完了データの配列長
 *
 * @return 配列長
 */
u32 PokemonSort::GetSortedDataMax( void ) const
{
  if( m_original_data == NULL )
  {
    GFL_ASSERT( 0 );  //@fix
    return 0;
  }

  return m_sorted_data_max;
}

//-----------------------------------------------------------------------
// オリジナルデータからソート後データへの操作
//-----------------------------------------------------------------------
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
void PokemonSort::OrderByBinary( u32 sort_bin_id, u32 start_index, u32 end_index )
{
  if( m_original_data == NULL
    || start_index >= end_index
    || start_index >= this->GetBinaryDataMax(sort_bin_id)
    || end_index > this->GetBinaryDataMax(sort_bin_id)
    )
  {
    GFL_ASSERT( 0 );  //@fix
    return;
  }


  this->ClearSortedData();

  for( u32 i=start_index; i < end_index; ++i )
  {
    for( u32 j=0; j < m_original_data->GetOriginalDataMax(); ++j )
    {
      MonsNo  monsno  = m_original_data->GetMonsNo( j );
      FormNo  formno  = m_original_data->GetFormNo( j );
      u16 data = this->GetBinaryData(sort_bin_id,i);
      
      if( ( (formno << 10) | monsno ) == data )
      {
        if( this->IsCondition( j ) )
        {
          this->AddSortedData( j );
        }
      }
    }
  }
}
/**
 * @brief ソート用のバイナリデータを使用して並び替えする（全て版）
 *
 * @param sort_bin_id ソート用のびんりんかーIDを指定してください
 *        (ARCID_ZUKAN_DATAのZUKAN_SORT_DATAビンリンカーに入っている必要があります)
 */
void PokemonSort::OrderByBinary( u32 sort_bin_id )
{
  this->OrderByBinary( sort_bin_id, 0, this->GetBinaryDataMax(sort_bin_id) );
}

/**
 * @brief ポケモン名にて全言語対応ソート
 *        言語はアーカイブによって変更される
 *
 * @param start_index 開始インデックス
 * @param end_index   終了インデックス
 */
void PokemonSort::OrderByMonsName( u32 start_index, u32 end_index )
{
  if( m_original_data == NULL
    || start_index == app::sort::StringBinary::SORT_HEADER_INDEX_NONE
    || end_index == app::sort::StringBinary::SORT_HEADER_INDEX_NONE
    || start_index > end_index
    )
  {
    GFL_ASSERT( 0 );  //@fix
    return;
  }

  this->ClearSortedData();

  for( u32 i=start_index; i <= end_index; ++i )
  {
    for( u32 j=0; j < m_original_data->GetOriginalDataMax(); ++j )
    {
      if( static_cast<u32>(m_original_data->GetMonsNo( j )) == m_mons_sort->GetSortOrder(i) )
      {
        if( this->IsCondition( j ) )
        {
          this->AddSortedData( j );
        }
      }
    }
  }
}


/**
 * @brief ポケモン名にて全言語対応ソート  （イニシャルインデックス版）
 *
 * @param initial_index 「あ」だったら「あから開始するのポケモン」ぜんぶ
 */
bool PokemonSort::OrderByMonsName( u32 initial_index )
{
  u32 start, end;
  if( m_mons_sort->GetInitialIndexToSortHeaderIndex( initial_index, start, end ) )
  {
    this->OrderByMonsName( start, end );
    return true;
  }
  
  return false;
}

/**
 * @brief ポケモン名にてソート（すべて版）
 */
void PokemonSort::OrderByMonsName( void )
{
  this->OrderByMonsName( 0, m_mons_sort->GetSortOrderMax()-1 );
}
/**
 * @brief 全国図鑑番号順に並び替えます
 *
 */
void PokemonSort::OrderByZenkokuNumber( void )
{
  this->ClearSortedData();
  
  for( int j=0; j < MONSNO_MAX; ++j )
  {
    for( u32 i=0; i < m_original_data->GetOriginalDataMax(); ++i )
    {
      if( j == m_original_data->GetMonsNo( i ) )
      {
        if( this->IsCondition( i ) )
        {
          this->AddSortedData( i );
        }
      }
    }
  }
}

/**
 * @brief 地方図鑑番号順に並び替えます
 * 
 * @param area 地方エリア
 */
void PokemonSort::OrderByLocalNumber( ExtendData::LocalArea area )
{
  this->ClearSortedData();
  
  //地方図鑑ごとにソート
  for( u32 j=1; j <= m_poke_data_ex->GetLocalAreaMonsMax( area ); ++j )  // @fix NMCat[1022] 完成していないのに王冠マークがついてしまう → 最大番号を返すので<=にした。
  {
    u32 zenkoku_num = m_poke_data_ex->GetZenkokuNumber( j, area );

    for( u32 i=0; i < m_original_data->GetOriginalDataMax(); ++i )
    {
      if( zenkoku_num == static_cast<u32>(m_original_data->GetMonsNo( i )) )
      {
        if( this->IsCondition( i ) )
        {
          this->AddSortedData( i );
        }
      }
    }
  }
}

/**
 * @brief 図鑑のモードごとの番号順（全国図鑑番号or地方番号）に並べ替えます
 */
void PokemonSort::OrderByZukanModeNumber( void )
{
  if( m_zukan_save->GetZukanMode() == Savedata::ZukanData::MODE_ZENKOKU )
  {
    this->OrderByZenkokuNumber();
  }
  else
  {
    this->OrderByLocalNumber( m_zukan_save->GetLocalAreaMode() );
  }
}

/**
 * @brief 図鑑リスト用に並び替えます。全国or地方は図鑑セーブの図鑑モードにより決定します。
 */
void PokemonSort::OrderByZukanList( void )
{
  m_tail_pokemon_no = m_zukan_save->GetZukanTailMonsNo( m_poke_data_ex );

  this->AddConditionFunction( &PokemonSort::IsConditionZukanList );

  this->OrderByZukanModeNumber();

  this->RemoveConditionFunction( &PokemonSort::IsConditionZukanList );
}


/**
 * @brief 体重が重い順に並び替えます。
 */
void PokemonSort::OrderByHeavy( void )
{
  this->OrderByBinary( BL_IDX_ZKN_SORT_DATA_ZKN_SORT_HEAVY_DAT );
}
/**
 * @brief 体重が低い順に並び替えます。
 */
void PokemonSort::OrderByLight( void )
{
  this->OrderByBinary( BL_IDX_ZKN_SORT_DATA_ZKN_SORT_LIGHT_DAT );
}
/**
 * @brief 身長が高い順に並び替えます。
 */
void PokemonSort::OrderByHigh( void )
{
  this->OrderByBinary( BL_IDX_ZKN_SORT_DATA_ZKN_SORT_HIGH_DAT );
}
/**
 * @brief 身長が低い順に並び替えます。
 */
void PokemonSort::OrderByLow( void )
{
  this->OrderByBinary( BL_IDX_ZKN_SORT_DATA_ZKN_SORT_SHORT_DAT );
}

/**
 * @brief 図鑑の条件を追加する
 *
 * @param condition 図鑑条件
 */
void PokemonSort::SetConditionZukan( ContidionZukan condition )
{
  //一旦クリア
  this->RemoveConditionFunction( &PokemonSort::IsConditionZukanSee );
  this->RemoveConditionFunction( &PokemonSort::IsConditionZukanCapture );
  this->RemoveConditionFunction( &PokemonSort::IsConditionZukanSeeOrCapture );

  switch( condition )
  {
  default:
    GFL_ASSERT(0);  //@check
  case CONDITION_ZUKAN_NONE:   ///<図鑑条件なし
    // 何も設定しない
    break;
  case CONDITION_ZUKAN_SEE:   ///<図鑑条件見た
    this->AddConditionFunction( &PokemonSort::IsConditionZukanSee );
    break;
  case CONDITION_ZUKAN_CAPTURE:///<図研条件捕まえた
    this->AddConditionFunction( &PokemonSort::IsConditionZukanCapture );
    break;
  case CONDITION_ZUKAN_SEE_OR_CAPTURE: ///<図研条件見たor捕まえた
    this->AddConditionFunction( &PokemonSort::IsConditionZukanSeeOrCapture );
    break;
  }
}

/**
 * @brief ポケタイプの条件を追加する
 *
 * @param type1  ポケタイプ１つめの条件
 * @param type2  ポケタイプ２つめの条件
 * 
 * @note  あくまでtype1,2はポケモンの２つのタイプです。
 *        （ほのお  むしを指定したらウルガモスがひっかかりガーディはひっかかりません）
 *        ほのおかむしのいずれか、という条件ではなりません。
 */
void PokemonSort::SetConditionPokeType( pml::PokeType type1, pml::PokeType type2 )
{
  if( type1 == POKETYPE_NULL && type2 == POKETYPE_NULL )
  {
    this->RemoveConditionFunction( &PokemonSort::IsConditionPokeType );
  }
  else
  {
    m_condition_type1 = type1;
    m_condition_type2 = type2;
    //NAGI_PRINT( "タイプセット1[%d] 2[%d]", type1, type2 );
    this->AddConditionFunction( &PokemonSort::IsConditionPokeType );
  }
}

/**
 * @brief 指定色条件を追加する
 *
 * @param color 何色のポケモンか　COLOR_MAXならば条件クリア
 */
void PokemonSort::SetConditionColor( pml::personal::Color color )
{
  m_condition_color = color;
  
  if( color == pml::personal::COLOR_MAX )
  {
    this->RemoveConditionFunction( &PokemonSort::IsConditionPokeColor );
  }
  else
  {
    this->AddConditionFunction( &PokemonSort::IsConditionPokeColor );
  }
}

/**
 * @brief 形状条件を追加する
 * 
 * @param style どの形状化  STYLE_TYPE_MAXならば条件クリア
 */
void PokemonSort::SetConditionStyle( ExtendData::StyleType style )
{
  m_condition_style = style;
  
  if( style == ExtendData::STYLE_TYPE_MAX )
  {
    this->RemoveConditionFunction( &PokemonSort::IsConditionPokeStyle );
  }
  else
  {
    this->AddConditionFunction( &PokemonSort::IsConditionPokeStyle );
  }
}

/**
 * @brief 頭文字ポケモンのみを抜き出し条件に追加
 *
 * @param index LANG_JAPANならばzkn_sort_aiueo_idx.hの定義値（ZKN_SORT_AIUEO_IDX_START_A～ZKN_SORT_AIUEO_IDX_ALL_END）
 * 
 */
void PokemonSort::SetConditionInitial( u32 initial_index )
{
  m_conditon_initial  = initial_index;
  
  if( initial_index == CONDITION_INITIAL_NONE )
  {
    this->RemoveConditionFunction( &PokemonSort::IsConditionPokeInitial );
  }
  else
  {
    this->AddConditionFunction( &PokemonSort::IsConditionPokeInitial );
  }
}

/**
 * @brief 指定モンスター番号の進化系かどうか
 *
 * @param mons モンスター番号
 */
void PokemonSort::SetConditionEvolution( MonsNo monsno )
{
  if( monsno == MONSNO_NULL )
  {
    this->RemoveConditionFunction( &PokemonSort::IsConditionPokeEvolution );
  }
  else
  {
    if( monsno > MONSNO_END )
    {
      GFL_ASSERT( 0 );  //@fix
      return;
    }
    GFL_ASSERT_STOP( m_evo_data );  //@fix
    m_evo_monsno  = monsno;

    this->AddConditionFunction( &PokemonSort::IsConditionPokeEvolution );
  }
}


/**
 * @brief 指定モンスター番号はOKとする
 *
 * @param monsno  指定モンスター番号
 */
void PokemonSort::SetConditionOnlyMonsNo( MonsNo monsno )
{
  m_only_monsno = monsno;
  if( monsno == MONSNO_NULL )
  {
    this->RemoveConditionFunction( &PokemonSort::IsConditionPokeOnly );
  }
  else
  {
    this->AddConditionFunction( &PokemonSort::IsConditionPokeOnly );
  }
}


/**
 * @brief 地方図鑑エリアのポケモン
 *
 * @param local_area  エリア  LOCAL_AREA_MAXだと全てのポケモン（全国）
 */
void PokemonSort::SetConditionLocalArea( PokeTool::ExtendData::LocalArea local_area )
{
  m_condition_local_area = local_area;
  if( local_area == PokeTool::ExtendData::LOCAL_AREA_MAX )
  {
    this->RemoveConditionFunction( &PokemonSort::IsConditionLocalArea );
  }
  else
  {
    this->AddConditionFunction( &PokemonSort::IsConditionLocalArea );
  }
}

/**
 * @brief 条件を完全クリア
 */
void PokemonSort::ClearAllCondition( void )
{
  for( int i=0; i<SET_CONDITION_NUM; ++i )
  {
    m_condition_function[ i ] = NULL;
  }
}

//-----------------------------------------------------------------------
// PRIVATE
//-----------------------------------------------------------------------
/**
 * @brief ソート後データを消去
 */
void PokemonSort::ClearSortedData( void )
{
  m_sorted_data_max = 0;
}
/**
 * @brief ソート後データに追加
 *
 * @param original_index  オリジナルデータインデックス
 */
void PokemonSort::AddSortedData( u32 original_index )
{
  m_sorted_data[ m_sorted_data_max++ ] = original_index;
}

/**
 * @brief 条件を追加
 *
 * @param function  条件関数
 */
void PokemonSort::AddConditionFunction( IsConditionFunction function )
{
  //重複チェック
  for( int i=0; i<SET_CONDITION_NUM; ++i )
  {
    if( m_condition_function[ i ] == function )
    {
      return;
    }
  }

  //空いている所にいれる
  for( int i=0; i<SET_CONDITION_NUM; ++i )
  {
    if( m_condition_function[ i ] == NULL )
    {
      m_condition_function[ i ] = function;
      return;
    }
  }
  GFL_ASSERT(0);  //追加できなかった。SET_CONDITION_NUMの数を増やす //@fix
}
/**
 * @brief 条件を削除
 *
 * @param function  条件関数
 */
void PokemonSort::RemoveConditionFunction( IsConditionFunction function )
{
  for( int i=0; i<SET_CONDITION_NUM; ++i )
  {
    if( m_condition_function[ i ] == function )
    {
      m_condition_function[ i ] = NULL;
      return;
    }
  }
}
/**
 * @brief 条件をチェック
 *
 * @param original_index  チェックするオリジナルデータインデックス
 *
 * @return trueすべての条件に合致 falseいずれかの条件を満たさなかった
 */
bool PokemonSort::IsCondition( u32 original_index )
{
  bool result = true;
  for( int i=0; i<SET_CONDITION_NUM; ++i )
  {
    if( m_condition_function[ i ] != NULL )
    {
      result &= (this->*m_condition_function[i])( original_index );
    }
  }
  return result;
}
/**
 * @brief 検索用バイナリデータからデータ取得
 *
 * @param bin_id  びんりんかーのID
 * @param index インデックス
 *
 * @return 下記データ構造
 * 
 *   |15 14 13 12 11 10|09 08 07 06 05 04 03 02 01 00|
 *   |     FORMNO      |           MONSNO            |
 *   |  0<=      <64   |        0<=      <1024       |
 */
u16 PokemonSort::GetBinaryData( u32 bin_id, u32 index ) const
{
  if( !( index < this->GetBinaryDataMax(bin_id) ) )
  {
    GFL_ASSERT(0);  //@fix
    return 0;
  }
  return static_cast<u16*>(m_bin_linker.GetData( bin_id ))[ index ];
}

/**
 * @brief 検索用バイナリデータのデータ総数取得
 *
 * @param bin_id  びんりんかーのID
 * @return データ総数
 */
u32 PokemonSort::GetBinaryDataMax( u32 bin_id ) const
{
  return m_bin_linker.GetDataSize( bin_id ) / sizeof(u16);
}

/**
 * @brief 条件：図鑑見たフラグがたっている
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return  true条件を満たした  false条件を満たしていない
 */
bool PokemonSort::IsConditionZukanSee( u32 original_index )
{
  return m_zukan_save->GetPokeSeeFlag( m_original_data->GetMonsNo( original_index ) ) && 
    !m_zukan_save->GetPokeGetFlag( m_original_data->GetMonsNo( original_index ) );
}
/**
 * @brief 条件：図鑑捕まえたフラグがたっている
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return  true条件を満たした  false条件を満たしていない
 */
bool PokemonSort::IsConditionZukanCapture( u32 original_index )
{
  return m_zukan_save->GetPokeGetFlag( m_original_data->GetMonsNo( original_index ) );
}

/**
 * @brief 条件：図鑑捕まえたor見たフラグがたっている
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return  true条件を満たした  false条件を満たしていない
 */
bool PokemonSort::IsConditionZukanSeeOrCapture( u32 original_index )
{
  return m_zukan_save->GetPokeSeeFlag( m_original_data->GetMonsNo( original_index ) )
    || m_zukan_save->GetPokeGetFlag( m_original_data->GetMonsNo( original_index ) );
}
/**
 * @brief 条件：指定のポケタイプ
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return  true条件を満たした  false条件を満たしていない
 */
bool PokemonSort::IsConditionPokeType( u32 original_index )
{
  pml::PokeType condition_type1  = m_condition_type1;
  pml::PokeType condition_type2  = m_condition_type2;

  //１を基準にチェックするので、１に入っていなかったら１と２をスワップ
  if( m_condition_type1 == POKETYPE_NULL )
  {
    condition_type1 = condition_type2;
    condition_type2 = POKETYPE_NULL;
  }
 
  pml::PokeType org_type1 = POKETYPE_NULL;
  pml::PokeType org_type2 = POKETYPE_NULL;
  if( ( m_original_data->GetMonsNo( original_index ) == MONSNO_ARUSEUSU ) || // アルセウスのパーソナルデータのタイプは全てPOKETYPE_NORMALなので特別対処が必要。
      ( m_original_data->GetMonsNo( original_index ) == MONSNO_GURIPUSU2 ) ) // グリプス２のパーソナルデータのタイプは全てPOKETYPE_NORMALなので特別対処が必要。
  {
    org_type1 = m_original_data->GetFormNo( original_index );  // poke_lib/pmlib/prog/include/poketype_def.h  PokeType_tag  POKETYPE_****  と
    org_type2 = org_type1;  // poke_lib/pmlib/prog/include/monsno_def.h  FORMNO_ARUSEUSU_****  の並び順は同じ
  }
  else
  {
    org_type1 = m_original_data->GetPokeType1( original_index );
    org_type2 = m_original_data->GetPokeType2( original_index );
  }

  if( condition_type2 == POKETYPE_NULL )
  {
    GFL_ASSERT( condition_type1 != POKETYPE_NULL ); //@check　引っかかっても条件にあてはまらないだけ
    //タイプが１つ、第一、第二のどちらかとあっていればOK
    return org_type1 == condition_type1
      || org_type2 == condition_type1;
  }
  else
  { 
    GFL_ASSERT( condition_type1 != POKETYPE_NULL ); //@check
    GFL_ASSERT( condition_type2 != POKETYPE_NULL ); //@check  引っかかっても条件にあてはまらないだけ
    
    return ( org_type1 == condition_type1 && org_type2 == condition_type2 )
      || ( org_type1 == condition_type2 && org_type2 == condition_type1 );
  }
}

/**
 * @brief 条件：ポケモンの色
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return  true条件を満たした  false条件を満たしていない
 */
bool PokemonSort::IsConditionPokeColor( u32 original_index )
{
  return m_condition_color == m_original_data->GetColor( original_index );
}
/**
 * @brief 条件：ポケモンの形状
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return  true条件を満たした  false条件を満たしていない
 */
bool PokemonSort::IsConditionPokeStyle( u32 original_index )
{
  SimpleParam simple_param;
  simple_param.monsNo = m_original_data->GetMonsNo( original_index );
  simple_param.formNo = m_original_data->GetFormNo( original_index );
  simple_param.sex    = pml::SEX_MALE;
  simple_param.isRare = false;
  simple_param.isEgg  = false;
  
  ExtendData::StyleType style = m_poke_data_ex->GetStyleType( simple_param );
  
  return style == m_condition_style;
}
/**
 * @brief 条件：ポケモンの頭文字
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return  true条件を満たした  false条件を満たしていない
 */
bool PokemonSort::IsConditionPokeInitial( u32 original_index )
{
  u32 initial_index = m_mons_sort->GetInitialIndex( m_original_data->GetMonsNo( original_index ) ); 
  
  return initial_index == m_conditon_initial;
}


/**
 * @brief 条件：指定ポケモンの進化系かどうか
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return true条件を満たした false条件を満たしていない 
 */
bool PokemonSort::IsConditionPokeEvolution( u32 original_index )
{
  // NAGI_PRINT( "MONSNO%d max%d\n", m_evo_monsno, m_evo_data[m_evo_monsno].evolution_num );
  for( int i = 0; i < m_evo_data[m_evo_monsno].evolution_num; ++i )
  {
    // NAGI_PRINT( " [%d] evo%d\n", i, m_evo_data[m_evo_monsno].evolution_mons[ i ] );
    if( m_evo_data[m_evo_monsno].evolution_mons[ i ] == m_original_data->GetMonsNo( original_index ) )
    {
      return true;
    }
  }
  return false;
}

/**
 * @brief 条件：指定したポケモンを含める
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return true条件を満たした false条件を満たしていない 
 */
bool PokemonSort::IsConditionPokeOnly( u32 original_index )
{
  return m_original_data->GetMonsNo( original_index ) == m_only_monsno;
}


/**
 * @brief 条件：指定した地方エリアのポケモンのみにする
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return  true条件を満たした  false条件を満たしていない
 */
bool PokemonSort::IsConditionLocalArea( u32 original_index )
{
  u32 zenkoku_num = static_cast<u32>(m_original_data->GetMonsNo( original_index ));
  return m_poke_data_ex->IsExistLocalZukan( zenkoku_num, m_condition_local_area );
}

/**
 * @brief 条件：最後尾のポケモン以内
 *
 * @param original_index  オリジナルデータインデックス
 *
 * @return  true条件を満たした  false条件を満たしていない
 */
bool PokemonSort::IsConditionZukanList( u32 original_index )
{
  MonsNo  monsno = m_original_data->GetMonsNo( original_index );
  
  if( m_zukan_save->GetZukanMode() == Savedata::ZukanData::MODE_ZENKOKU )
  {
    return static_cast<u32>(monsno) <= m_tail_pokemon_no;
  }
  else
  {
    return m_poke_data_ex->GetLocalNumber(monsno, m_zukan_save->GetLocalAreaMode() ) <= m_tail_pokemon_no;
  }
}

GFL_NAMESPACE_END( PokeTool )
