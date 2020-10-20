//=============================================================================
/**
 * @file    PokeToolRemaindSkill.cpp
 * @brief   指定ポケモンが思い出せるかを取得する
 * @author  Toru=Nagihashi
 * @date    2012.07.27
 *
 * @author  Hiroyuki Nakamura
 * @date    2015.12.14
 * @note    sangoから移植
 */
//=============================================================================

// mobule
#include "PokeTool/include/PokeToolRemaindSkill.h"
// gflib2
#include <debug/include/gfl2_Assert.h>


GFL_NAMESPACE_BEGIN(PokeTool)


//---------------------------------------------------------------------
// コンストラクタ・デストラクタ
//---------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param heap ヒープ
 * @param param ポケモン情報
 */
RemaindSkill::RemaindSkill( gfl2::heap::HeapBase * heap, const pml::pokepara::CoreParam * param )
{
  m_waza_oboe = GFL_NEW( heap ) pml::personal::WazaOboeData( heap );
  m_waza_oboe->LoadData( param->GetMonsNo(), param->GetFormNo() );
  SetupRemaindWaza( param );
}

/**
 * @brief   コンストラクタ（非同期読み込み用）
 *
 * @param   heap  ヒープ
 */
RemaindSkill::RemaindSkill( gfl2::heap::HeapBase * heap )
  : m_seq( 0 )
{
  m_waza_oboe = GFL_NEW( heap ) pml::personal::WazaOboeData( heap );
}

/**
 * @brief デストラクタ
 */
RemaindSkill::~RemaindSkill()
{
  GFL_SAFE_DELETE( m_waza_oboe );
}

//---------------------------------------------------------------------
// システム
//---------------------------------------------------------------------
/**
 * @brief リスト最大
 *
 * @return  リスト最大取得
 */
u32 RemaindSkill::GetListMax( void ) const
{
  return m_list_max;
}

/**
 * @brief インデックスから技番号取得
 *
 * @param index インデックス
 *
 * @return 技番号
 */
WazaNo RemaindSkill::GetWazaNo( u32 index ) const
{
  GFL_ASSERT( index < m_list_max );
  return m_list_data[ index ];
}

/**
 * @brief 思い出せる技データをリストにセット
 *
 * @param heap  ヒープ
 * @param param 思い出すポケモン
 */
void RemaindSkill::SetupRemaindWaza( const pml::pokepara::CoreParam * param )
{
  m_list_max = 0;

  // 覚えている技のテーブルを生成
  WazaNo now_waza[pml::MAX_WAZA_NUM];
  for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ )
  {
    now_waza[i] = param->GetWazaNo( i );
  }
  
  // タマゴ技思い出し
  for( int i=0; i<pml::MAX_WAZA_NUM; i++ )
  {
    WazaNo waza = param->GetTamagoWazaNo( i );

    // ないチェック
    if( waza == WAZANO_NULL )
    {
      continue;
    }

    // すでに覚えている技かチェック
    if( IsSame( waza, now_waza, pml::MAX_WAZA_NUM ) != false )
    {
      continue;
    }

    //テーブルに重複がないかチェック
    if( IsSame( waza, m_list_data, m_list_max ) != false )
    {
      continue;
    }
    
    m_list_data[ m_list_max++ ] = waza;
    GFL_ASSERT( m_list_max < REMAIND_WAZA_MAX_BUFFER );
  }

  // 各覚え技
  for( int i=0; i<pml::personal::WazaOboeData::OBOEWAZA_KIND_NUM; i++ )
  {
    for( int j=0; j<m_waza_oboe->GetValidCodeNum(); j++ )
    {
      // 進化/基本習得技
      if( i != pml::personal::WazaOboeData::OBOEWAZA_KIND_LEVEL )
      {
        if( m_waza_oboe->GetLevel(j) != i )
        {
          continue;
        }
      }
      // レベル習得技
      else
      {
        if( m_waza_oboe->GetLevel(j) < i )
        {
          continue;
        }
      }

      WazaNo waza = m_waza_oboe->GetWazaNo( j );

      // すでに覚えている技かチェック
      if( IsSame( waza, now_waza, pml::MAX_WAZA_NUM ) != false )
      {
        continue;
      }

      //テーブルに重複がないかチェック
      if( IsSame( waza, m_list_data, m_list_max ) != false )
      {
        continue;
      }

      m_list_data[ m_list_max++ ] = waza;
      GFL_ASSERT( m_list_max < REMAIND_WAZA_MAX_BUFFER );
    }
  }
}

/**
 * @brief   データを読み込んでリストを生成
 *
 * @param   param   ポケモン情報
 * @param   heap    テンポラリヒープ
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
bool RemaindSkill::LoadRemaindWaza( const pml::pokepara::CoreParam * param, gfl2::heap::HeapBase * heap )
{
  switch( m_seq )
  {
  case 0:
    // 読み込み済み
    if( m_waza_oboe->StartAsyncRead( param->GetMonsNo(), param->GetFormNo(), heap ) == false )
    {
      break;
    }
    m_seq++;
    /* FALL THROUGH */
  case 1:
    if( m_waza_oboe->WaitAsyncReadFinish() == false )
    {
      return false;
    }
    m_seq = 0;
  }
  SetupRemaindWaza( param );
  return true;
}

/**
 * @brief   重複チェック
 *
 * @param   waza  検索する技
 * @param   tbl   検索場所
 * @param   len   検索場所の要素数
 *
 * @retval  true  = 重複している
 * @retval  false = それ以外
 */
bool RemaindSkill::IsSame( WazaNo waza, WazaNo * tbl, u32 len )
{
  for( int i=0; i<len; i++ )
  {
    if( tbl[i] == waza )
    {
      return true;
    }
  }
  return false;
}


GFL_NAMESPACE_END(PokeTool)
