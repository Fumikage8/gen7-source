//======================================================================
/**
 * @file    PokeToolPersonalSort.cpp
 * @author  Toru=Nagihashi => niji:fukushima_yuya
 * @date    2012.07.06 => 2015/07/30 18:15:37
 * @brief   パーソナルデータソートクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "PokeTool/include/PokeToolPersonalSort.h"


GFL_NAMESPACE_BEGIN( PokeTool )

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
PersonalSort::PersonalSort( u32 language, const Savedata::ZukanData *zukan_save, gfl2::heap::HeapBase * heap, LoadType load_type, Mode mode ) :
  PokemonSort( language, zukan_save, heap ),
  m_draw_data( NULL ),
  CreatePersonalData( NULL ),
  DeletePersonalData( NULL ),
  GetPersonalData( NULL ),
  m_mode( mode ),
  m_load_type( load_type )
{
  this->SetOriginalDataOwner( this );
  
  this->SetModeFunction( load_type );
  
  (this->*CreatePersonalData)( heap );
  
}
/**
 * @brief デストラクタ
 */
PersonalSort::~PersonalSort()
{
  (this->*DeletePersonalData)();
}
//-----------------------------------------------------------------------
// 取得
//-----------------------------------------------------------------------
/**
 * @brief ソートされた結果受け取り
 *
 * @param index インデックス（０～GetSortedDataMax() ）
 *
 * @return モンスターナンバー
 */
MonsNo PersonalSort::GetSortedMonsNo( u32 index )
{
  return this->GetMonsNo( this->GetSortedData( index ) );
}

/**
 * @brief LoadTypeがFullだった際、再読み込みをする
 */
void PersonalSort::ReloadFullData( void )
{
  if( m_load_type == LOAD_TYPE_FULL )
  {
    for( u32 i = 0; i < this->GetOriginalDataMax(); ++i )
    {
      {
        MonsNo  monsno  = this->GetMonsNo(i);
        m_draw_data[ i ].formno = 0;
        if( m_mode == MODE_ZUKAN )
        {
          pml::Sex  sex;
          bool rare;
          m_zukan_save->GetDrawData( monsno, &sex, &rare, &m_draw_data[ i ].formno );
        }
        
        if( m_personal_array[ i ]->GetMonsNo() != monsno 
            || m_personal_array[ i ]->GetFormNo() != m_draw_data[ i ].formno )
        {
          m_personal_array[ i ]->LoadData( monsno, m_draw_data[ i ].formno );
        }
      }
    }
  }
}
//-----------------------------------------------------------------------
// IOriginalDataクラス実装
//-----------------------------------------------------------------------
/**
 * @brief モンスター番号取得
 *
 * @param original_index  オリジナルインデックス
 *
 * @return  モンスター番号
 */
MonsNo PersonalSort::GetMonsNo( u32 original_index ) 
{
  
  return static_cast<MonsNo>(original_index + 1);//(this->*GetPersonalData)( original_index )->GetMonsNo();
}
/**
 * @brief ポケモンタイプ１取得
 *
 * @param original_index  オリジナルインデックス
 *
 * @return  タイプ１
 */
pml::PokeType PersonalSort::GetPokeType1( u32 original_index ) 
{
  return static_cast<pml::PokeType>( (this->*GetPersonalData)( original_index )->GetParam( pml::personal::PARAM_ID_type1 ) );
}
/**
 * @brief ポケモンタイプ２取得
 *
 * @param original_index  オリジナルインデックス
 *
 * @return  タイプ２
 */
pml::PokeType PersonalSort::GetPokeType2( u32 original_index ) 
{
  return static_cast<pml::PokeType>( (this->*GetPersonalData)( original_index )->GetParam( pml::personal::PARAM_ID_type2 ) );
}
/**
 * @brief フォルム取得
 *
 * @param original_index  オリジナルインデックス
 *
 * @return  フォルム
 */
FormNo PersonalSort::GetFormNo( u32 original_index )
{
  if( m_load_type == LOAD_TYPE_FULL )
  {
    return m_draw_data[ original_index ].formno;
  }
  else
  {
    return (this->*GetPersonalData)( original_index )->GetFormNo();
  }
}
/**
 * @brief 色取得
 *
 * @param original_index  オリジナルインデックス
 *
 * @return 色
 */
pml::personal::Color PersonalSort::GetColor( u32 original_index )
{
  return static_cast<pml::personal::Color>( (this->*GetPersonalData)( original_index )->GetParam( pml::personal::PARAM_ID_color ) );
}
/**
 * @brief ソート前データ総数を取得
 *
 * @return  データ総数
 * 
 */
u32 PersonalSort::GetOriginalDataMax( void )
{
  //0オリジンで１～MONSNO_ENDまで
  return MONSNO_MAX-1-1;
}

/**
 * @brief モード別処理設定
 *
 * @param load_type モード別処理
 */
void PersonalSort::SetModeFunction( LoadType load_type )
{
  if( load_type == LOAD_TYPE_FULL )
  {
    CreatePersonalData  = &PersonalSort::CreatePersonalDataFull;
    DeletePersonalData  = &PersonalSort::DeletePersonalDataFull;
    GetPersonalData     = &PersonalSort::GetPersonalDataFull;
  }
  else
  {
    CreatePersonalData  = &PersonalSort::CreatePersonalDataPart;
    DeletePersonalData  = &PersonalSort::DeletePersonalDataPart;
    GetPersonalData     = &PersonalSort::GetPersonalDataPart;
  }
}

/**
 * @brief モード別処理  パーソナルを読み込み
 *
 * @param heap  ヒープ
 */
void PersonalSort::CreatePersonalDataFull( gfl2::heap::HeapBase * heap )
{
  m_personal_array  = static_cast<pml::personal::PersonalData**>(GflHeapAllocMemory( heap, sizeof(pml::personal::PersonalData*)*(this->GetOriginalDataMax()) ) );
  m_draw_data = static_cast<DrawData*>(GflHeapAllocMemory( heap, sizeof(DrawData)*(this->GetOriginalDataMax()) ) );
    
  for( u32 i = 0; i < this->GetOriginalDataMax(); ++i )
  {
    m_personal_array[ i ] = GFL_NEW( heap ) pml::personal::PersonalData(heap);
    
    MonsNo  monsno  = this->GetMonsNo(i);
    m_draw_data[ i ].formno = 0;
    if( m_mode == MODE_ZUKAN )
    {
      pml::Sex  sex;
      bool rare;
      m_zukan_save->GetDrawData( monsno, &sex, &rare, &m_draw_data[ i ].formno );
    }

    m_personal_array[ i ]->LoadData( monsno, m_draw_data[ i ].formno );
  }
}
/**
 * @brief モード別処理  パーソナル破棄
 */
void PersonalSort::DeletePersonalDataFull( void )
{
  for( u32 i = 0; i < this->GetOriginalDataMax(); ++i )
  {
    GFL_SAFE_DELETE( m_personal_array[i] );
  }
  GflHeapFreeMemory( m_draw_data );
  GflHeapFreeMemory( m_personal_array );
}

/**
 * @brief モード別処理  パーソナル取得
 *
 * @param index インデックス
 *
 * @return  取得
 */
pml::personal::PersonalData * PersonalSort::GetPersonalDataFull( u32 index )
{
  return m_personal_array[ index ];
}

/**
 * @brief モード別処理  パーソナル生成
 *
 * @param heap
 */
void PersonalSort::CreatePersonalDataPart( gfl2::heap::HeapBase * heap )
{
  m_personal_data  = GFL_NEW(heap) pml::personal::PersonalData(heap);
}
/**
 * @brief モード別処理  パーソナル破棄
 */
void PersonalSort::DeletePersonalDataPart( void )
{
  GFL_DELETE( m_personal_data );
}
/**
 * @brief モード別処理  パーソナル取得
 *
 * @param index インデックス
 *
 * @return  取得
 */
pml::personal::PersonalData * PersonalSort::GetPersonalDataPart( u32 index )
{
  MonsNo  monsno  = this->GetMonsNo( index );
  FormNo  formno  = 0;
  if( m_personal_data->GetMonsNo() != monsno )
  {
    if( m_mode == MODE_ZUKAN )
    {
      pml::Sex  sex;
      bool rare;
      m_zukan_save->GetDrawData( monsno, &sex, &rare, &formno );
    }

    m_personal_data->LoadData( monsno, formno );
  }
  return m_personal_data;
}

GFL_NAMESPACE_END( PokeTool )
