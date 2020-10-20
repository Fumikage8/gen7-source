//======================================================================
/**
 * @file    BoxSearchSortData.cpp
 * @date    2015/07/30 19:42:41
 * @author  fukushima_yuya
 * @brief   ボックス検索：ソートデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include <debug/include/gfl2_Assert.h>

// niji
#include "App/BoxSearch/source/parts/BoxSearchSortData.h"

#include <AppLib/include/Sort/StringBinary.h>
#include <AppLib/include/Sort/StringTokusei.h>
#include <AppLib/include/Sort/StringWaza.h>
#include <PokeTool/include/PokeToolPersonalSort.h>
#include <System/include/PokemonVersion.h>

#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 STRING_SORT_NONE = 0xffffffff;		//!< ソートデータなし

#include "App/BoxSearch/source/BoxSearchInitial.cdat"   //!< ボックス検索画面 50音リストデータ


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
SortData::SortData( gfl2::heap::HeapBase* pHeap, Savedata::ZukanData* pZukanData, u8 lang )
  : m_pDevHeap(pHeap)
  , m_pZukanData( pZukanData )
  , m_pPersonalSort( NULL )
  , m_pTokuseiSort( NULL )
  , m_pTokuseiNoSortData( NULL )
  , m_TokuseiSortDataMax( 0 )
  , m_pWazaSort( NULL )
  , m_pWazaNoSortData( NULL )
  , m_WazaSortDataMax( 0 )
  , m_Language( lang )
{
  for( u32 i=0; i<STR_HED_LIST2_MAX; ++i )
  {
    m_VowelItemMax[i] = 0;
  }

  CreatePersonalSort();
  CreateTokuseiSort();
  CreateWazaSort();

  FUKUSHIMA_PRINT( "SortData : Langague[%d]\n", m_Language );
}

//------------------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------------------
SortData::~SortData( void )
{
  DeleteWazaSort();
  DeleteTokuseiSort();
  DeletePersonalSort();
}


//==============================================================================
/**
 * @brief   ポケモン名ソートデータ
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief	  パーソナルソートの生成
 */
//------------------------------------------------------------------------------
void SortData::CreatePersonalSort( void )
{
  // パーソナルソートの生成
  m_pPersonalSort = GFL_NEW(m_pDevHeap) PokeTool::PersonalSort( m_Language, m_pZukanData, m_pDevHeap, PokeTool::PersonalSort::LOAD_TYPE_FULL );
  m_pPersonalSort->SetConditionZukan( PokeTool::PokemonSort::CONDITION_ZUKAN_SEE_OR_CAPTURE );
}

//------------------------------------------------------------------------------
/**
 * @brief	  パーソナルソートの破棄
 */
//------------------------------------------------------------------------------
void SortData::DeletePersonalSort( void )
{
  GFL_SAFE_DELETE( m_pPersonalSort );
}


//------------------------------------------------------------------------------
/**
 * @brief	  ポケモン名の頭文字ごとのソートデータ設定
 *
 * @param   con     子音
 * @param   vowMax  母音項目数
 */
//------------------------------------------------------------------------------
void SortData::InitPokemonSortItemMax( u32 con, u32 vowMax )
{
  FUKUSHIMA_PRINT( "InitPokemonSortItemMax( u32 con=[%d], u32 vowMax=[%d] )\n", con, vowMax );

  if( m_Language == POKEMON_LANG_JAPAN )
  {
    const u32* tbl = InitialSortTable[con];

    for( u32 i=0; i<vowMax; ++i )
    {
      if( m_pPersonalSort->OrderByMonsName( tbl[i] ) )
      {
        m_VowelItemMax[i] = m_pPersonalSort->GetSortedDataMax();
      }
      else {
        m_VowelItemMax[i] = 0;
      }

      FUKUSHIMA_PRINT( "m_VowelItemMax[%d] = %d\n", i, m_VowelItemMax[i] );
    }
  }
  else {
    m_VowelItemMax[0] = 0;    //!< "-" ダミーで 0

    for( u32 i=1; i<vowMax; ++i )
    {
      if( m_pPersonalSort->OrderByMonsName( i - 1 ) )
      {
        m_VowelItemMax[i] = m_pPersonalSort->GetSortedDataMax();
      }
      else {
        m_VowelItemMax[i] = 0;
      }

      FUKUSHIMA_PRINT( "m_VowelItemMax[%d] = %d\n", i - 1, m_VowelItemMax[i] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモン名の母音項目数の取得
 *
 * @param   index   母音インデックス
 *
 * @return  母音項目数
 */
//------------------------------------------------------------------------------
u32 SortData::GetPokemonSortVowelNum( u32 index ) const
{
  return m_VowelItemMax[index];
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモン名ソートデータの取得
 *
 * @param   pos   位置
 * @return  モンスターNo
 */
//------------------------------------------------------------------------------
MonsNo SortData::GetPokemonSortItemData( u32 pos )
{
  GFL_ASSERT( m_pPersonalSort );

  return m_pPersonalSort->GetSortedMonsNo( pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモン名ソートデータの頭文字を設定
 *
 * @param   con   子音
 * @param   vow   母音
 */
//------------------------------------------------------------------------------
void SortData::SetInitialPokemonSortData( u32 con, u32 vow )
{
  FUKUSHIMA_PRINT( "SetInitialPokemonSortData( u32 con=[%d], u32 vow=[%d] )\n", con, vow );
  GFL_ASSERT( m_pPersonalSort );

  if( m_Language == POKEMON_LANG_JAPAN )
  {
    if( con >= GFL_NELEMS(InitialSortTable) )
    {
      GFL_ASSERT(0);
      con = 0;
    }

    // 頭文字を変更
    const u32* tbl = InitialSortTable[con];
    m_pPersonalSort->OrderByMonsName( tbl[vow] );
  }
  else {
    m_pPersonalSort->OrderByMonsName( vow );
    FUKUSHIMA_PRINT( "ItemMax[%d] = %d\n", vow, m_pPersonalSort->GetSortedDataMax() );
  }
}



//==============================================================================
/**
 * @brief   特性ソートデータ
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief	  特性ソートデータの生成
 */
//------------------------------------------------------------------------------
void SortData::CreateTokuseiSort( void )
{
  m_pTokuseiSort = GFL_NEW(m_pDevHeap) app::sort::StringTokusei();
  m_pTokuseiSort->Load( m_pDevHeap );
}

//------------------------------------------------------------------------------
/**
 * @brief	  特性ソートデータの破棄
 */
//------------------------------------------------------------------------------
void SortData::DeleteTokuseiSort( void )
{
  GFL_SAFE_DELETE( m_pTokuseiSort );
}


//------------------------------------------------------------------------------
/**
 * @brief	  特性の頭文字ごとのソートデータ設定
 *
 * @param   con     子音
 * @param   vowMax  母音項目数
 */
//------------------------------------------------------------------------------
void SortData::InitTokuseiSortItemMax( u32 con, u32 vowMax )
{
  FUKUSHIMA_PRINT( "InitTokuseiSortItemMax( u32 con=[%d], u32 vowMax=[%d] )\n", con, vowMax );

  if( m_Language == POKEMON_LANG_JAPAN )
  {
    for( u32 i=0; i<vowMax; ++i )
    {
      CreateTokuseiSortData( con, i );
      m_VowelItemMax[i] = m_TokuseiSortDataMax;
      DeleteTokuseiSortData();

      FUKUSHIMA_PRINT( "m_VowelItemMax[%d] = %d\n", i, m_VowelItemMax[i] );
    }
  }
  else {
    m_VowelItemMax[0] = 0;    //!< "-" ダミーで 0

    for( u32 i=1; i<vowMax; ++i )
    {
      CreateTokuseiSortData( con, (i - 1) );
      m_VowelItemMax[i] = m_TokuseiSortDataMax;
      DeleteTokuseiSortData();

      FUKUSHIMA_PRINT( "m_VowelItemMax[%d] = %d\n", i - 1, m_VowelItemMax[i] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief	  母音の項目数の取得
 *
 * @param   index   母音インデックス
 *
 * @return  母音項目数
 */
//------------------------------------------------------------------------------
u32 SortData::GetTokuseiSortVowelNum( u32 index ) const
{
  return m_VowelItemMax[index];
}


//------------------------------------------------------------------------------
/**
 * @brief	  特性データテーブル生成
 *
 * @param   con   子音
 * @param   vow   母音
 */
//------------------------------------------------------------------------------
void SortData::CreateTokuseiSortData( u32 con, u32 vow )
{
  FUKUSHIMA_PRINT( "CreateTokuseiSortData( u32 con=[%d], u32 vow=[%d] )\n", con, vow );
  m_TokuseiSortDataMax = 0;

  u32 max = GetTokuseiSortMax( con, vow );
  m_pTokuseiNoSortData = GFL_NEW_LOW_ARRAY(m_pDevHeap) TokuseiNo[max];

  for( u32 i=0; i<max; ++i )
  {
    TokuseiNo tokusei = GetTokuseiSortData( con, vow, i );

    if( m_pTokuseiSort->IsSecretTokusei(tokusei) ) continue;
    
    m_pTokuseiNoSortData[m_TokuseiSortDataMax] = tokusei;
    FUKUSHIMA_PRINT( "m_pTokuseiNoSortData[%d] = %d\n", m_TokuseiSortDataMax, tokusei );
    m_TokuseiSortDataMax++;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief	  特性データテーブル削除
 */
//------------------------------------------------------------------------------
void SortData::DeleteTokuseiSortData( void )
{
  if( m_pTokuseiNoSortData )
  {
    GFL_DELETE_ARRAY m_pTokuseiNoSortData;
    m_pTokuseiNoSortData = NULL;
    m_TokuseiSortDataMax = 0;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   特性IDの取得
 *
 * @param   index   ソートデータの配列インデックス
 *
 * @return  特性ID
 */
//------------------------------------------------------------------------------
TokuseiNo SortData::GetTokuseiNo( u32 index ) const
{
  return m_pTokuseiNoSortData[index];
}

//------------------------------------------------------------------------------
/**
 * @brief   特性ソートデータ数取得
 *
 * @return  特性ソートデータ数
 */
//------------------------------------------------------------------------------
u32 SortData::GetTokuseiCount( void ) const
{
  return m_TokuseiSortDataMax;
}


//------------------------------------------------------------------------------
/**
 * @brief	  特性ソートデータの取得
 * @param   con   子音
 * @param   vow   母音
 * @param   pos   リスト位置
 * @return  特性
 */
//------------------------------------------------------------------------------
TokuseiNo SortData::GetTokuseiSortData( u32 con, u32 vow, u32 pos )
{
  u32 ret = 0;

  if( m_Language == POKEMON_LANG_JAPAN )
  {
    if( con >= GFL_NELEMS(InitialSortTable) )
    {
      GFL_ASSERT( 0 );
      con = 0;
    }

    const u32* tbl = InitialSortTable[con];
    ret = GetStringSortData( m_pTokuseiSort, tbl[vow], pos );
  }
  else {
    ret = GetStringSortData( m_pTokuseiSort, vow, pos );
  }

  if( ret == STRING_SORT_NONE )
  {
    GFL_ASSERT( 0 );
    return TOKUSEI_NULL;
  }

  return static_cast<TokuseiNo>( ret );
}

//------------------------------------------------------------------------------
/**
 * @brief	  特性ソートデータ数取得
 * @param   con   子音
 * @param   vow   母音
 */
//------------------------------------------------------------------------------
u32 SortData::GetTokuseiSortMax( u32 con, u32 vow )
{
  if( m_Language == POKEMON_LANG_JAPAN )
  {
    if( con >= GFL_NELEMS(InitialSortTable) )
    {
      GFL_ASSERT( 0 );
      con = 0;
    }

    const u32* tbl = InitialSortTable[con];
    return GetStringSortMax( m_pTokuseiSort, tbl[vow] );
  }
  else {
    return GetStringSortMax( m_pTokuseiSort, vow );
  }
}


//==============================================================================
/**
 * @brief   技名ソートデータ
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief	  技ソートデータ生成
 */
//------------------------------------------------------------------------------
void SortData::CreateWazaSort( void )
{
  m_pWazaSort = GFL_NEW(m_pDevHeap) app::sort::StringWaza();
  m_pWazaSort->Load( m_pDevHeap );
}

//------------------------------------------------------------------------------
/**
 * @brief	  技ソートデータ破棄
 */
//------------------------------------------------------------------------------
void SortData::DeleteWazaSort( void )
{
  GFL_SAFE_DELETE( m_pWazaSort );
}


//------------------------------------------------------------------------------
/**
 * @brief	  技の頭文字ごとのソートデータ設定
 *
 * @param   con     子音
 * @param   vowMax  母音項目数
 */
//------------------------------------------------------------------------------
void SortData::InitWazaSortItemMax( u32 con, u32 vowMax )
{
  FUKUSHIMA_PRINT( "InitWazaSortItemMax( u32 con=[%d], u32 vowMax=[%d] )\n", con, vowMax );

  if( m_Language == POKEMON_LANG_JAPAN )
  {
    for( u32 i=0; i<vowMax; ++i )
    {
      CreateWazaSortData( con, i );
      m_VowelItemMax[i] = m_WazaSortDataMax;
      DeleteWazaSortData();

      FUKUSHIMA_PRINT( "m_VowelItemMax[%d] = %d\n", i, m_VowelItemMax[i] );
    }
  }
  else {
    m_VowelItemMax[0] = 0;    //!< "-" ダミーで 0

    for( u32 i=1; i<vowMax; ++i )
    {
      CreateWazaSortData( con, (i - 1) );
      m_VowelItemMax[i] = m_WazaSortDataMax;
      DeleteWazaSortData();

      FUKUSHIMA_PRINT( "m_VowelItemMax[%d] = %d\n", i - 1, m_VowelItemMax[i] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief	  母音の項目数の取得
 *
 * @param   index   母音のインデックス
 *
 * @return  母音項目数
 */
//------------------------------------------------------------------------------
u32 SortData::GetWazaSortVowelNum( u32 index ) const
{
  return m_VowelItemMax[index];
}


//------------------------------------------------------------------------------
/**
 * @brief	  技データテーブル生成
 *
 * @param   con   子音
 * @param   vow   母音
 */
//------------------------------------------------------------------------------
void SortData::CreateWazaSortData( u32 con, u32 vow )
{
  FUKUSHIMA_PRINT( "CreateWazaSortData( u32 con=[%d], u32 vow=[%d] )\n", con, vow );

  m_WazaSortDataMax = 0;

  u32 max = GetWazaSortMax( con, vow );
  m_pWazaNoSortData = GFL_NEW_LOW_ARRAY(m_pDevHeap) WazaNo[max];

  for( u32 i=0; i<max; ++i )
  {
    WazaNo waza = GetWazaSortData( con, vow, i );

    if( m_pWazaSort->IsSecretWaza(waza) ) continue;

    m_pWazaNoSortData[m_WazaSortDataMax] = waza;
    FUKUSHIMA_PRINT( "m_pWazaNoSortData[%d] = %d\n", m_WazaSortDataMax, waza );
    m_WazaSortDataMax++;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief	  技データテーブル破棄
 */
//------------------------------------------------------------------------------
void SortData::DeleteWazaSortData( void )
{
  if( m_pWazaNoSortData )
  {
    GFL_DELETE_ARRAY m_pWazaNoSortData;
    m_pWazaNoSortData = NULL;
    m_WazaSortDataMax = 0;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   技Noの取得
 *
 * @param   index   ソートデータの配列インデックス
 *
 * @return  技No
 */
//------------------------------------------------------------------------------
WazaNo SortData::GetWazaNo( u32 index ) const
{
  return m_pWazaNoSortData[index];
}

//------------------------------------------------------------------------------
/**
 * @brief   技ソートデータ数取得
 *
 * @return  技ソートデータ数
 */
//------------------------------------------------------------------------------
u32 SortData::GetWazaCount( void ) const
{
  return m_WazaSortDataMax;
}


//------------------------------------------------------------------------------
/**
 * @brief	  技ソートデータ取得
 *
 * @param   con   子音
 * @param   vow   母音
 * @param   pos   リスト位置
 *
 * @return  技
 */
//------------------------------------------------------------------------------
WazaNo SortData::GetWazaSortData( u32 con, u32 vow, u32 pos )
{
  u32 ret = 0;

  if( m_Language == POKEMON_LANG_JAPAN )
  {
    if( con >= GFL_NELEMS(InitialSortTable) )
    {
      GFL_ASSERT( 0 );
      con = 0;
    }

    const u32* tbl = InitialSortTable[con];
    ret = GetStringSortData( m_pWazaSort, tbl[vow], pos );
  }
  else {
    ret = GetStringSortData( m_pWazaSort, vow, pos );
  }

  if( ret == STRING_SORT_NONE )
  {
    GFL_ASSERT( 0 );
    return WAZANO_NULL;
  }

  return static_cast<WazaNo>( ret );
}

//------------------------------------------------------------------------------
/**
 * @brief	  技ソートデータ数取得
 *
 * @param   con   子音
 * @param   vow   母音
 *
 * @return  データ数
 */
//------------------------------------------------------------------------------
u32 SortData::GetWazaSortMax( u32 con, u32 vow )
{
  if( m_Language == POKEMON_LANG_JAPAN )
  {
    if( con >= GFL_NELEMS(InitialSortTable) )
    {
      GFL_ASSERT( 0 );
      con = 0;
    }

    const u32* tbl = InitialSortTable[con];
    return GetStringSortMax( m_pWazaSort, tbl[vow] );
  }
  else {
    return GetStringSortMax( m_pWazaSort, vow );
  }
}



//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
u32 SortData::GetStringSortData( app::sort::StringBinary* sort, u32 id, u32 pos )
{
  u32 start;    //!< 開始位置
  u32 end;      //!< 終了位置

  if( !sort->GetInitialIndexToSortHeaderIndex( id, start, end ) )
  {
    return STRING_SORT_NONE;
  }

  return sort->GetSortOrder( start + pos );
}

//------------------------------------------------------------------------------
/**
 * @brief	  頭文字ソートテーブルからデータ数を取得
 *
 * @param   sort    ソートデータ
 * @param   id      頭文字ID
 *
 * @return  データ数
 */
//------------------------------------------------------------------------------
u32 SortData::GetStringSortMax( app::sort::StringBinary* sort, u32 id )
{
  u32 start;    //!< 開始位置
  u32 end;      //!< 終了位置

  if( !sort->GetInitialIndexToSortHeaderIndex( id, start, end ) )
  {
    return 0;
  }

  return ( end - start + 1 );
}


GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )
