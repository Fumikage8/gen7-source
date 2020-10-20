//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		poke_tool_ExtendData.cpp
 *	@brief  パーソナル外にあるポケモンごとのデータ
 *	@author	Toru=Nagihashi
 *	@date		2012.05.24
 *	@author	kawada_koji
 *	@date		2015.08.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/zukan_data.gaix>
#include <niji_conv_header/app/zukan_data/zkn_extend_data.h>

// niji
#include <System/include/ArcIdGetter.h>
#include <GameSys/include/GameManager.h>
#include <PokeTool/include/PokeToolExtendData.h>


GFL_NAMESPACE_BEGIN(PokeTool)


#include <niji_conv_header/app/zukan_data/zkn_chihoua_no.cdat>  // static u32 GetLocalNumberStatic( u32 zenkoku_num, LocalArea local_area ); 用


//--------------------------------------------------------------------
// 生成・破棄
//--------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param heap  ヒープ
 */
ExtendData::ExtendData( gfl2::heap::HeapBase *heap ) :
  m_bin_linker(),
  m_bin(NULL),  // @fix cov_ctr[12097]: 未初期化をNULLで初期化
  m_next_form_table(NULL),
  m_next_form_table_size(0),
  m_style_table(NULL),
  m_style_table_size(0)
{
  // zero初期化
  {
    for(s32 local_area=LOCAL_AREA_START; local_area<=LOCAL_AREA_END; ++local_area)
    {
      m_local_number_table[local_area-1] = NULL;
      m_local_number_table_size[local_area-1] = 0;
      m_local_number_max[local_area-1] = 0;
    }
  }

  // ファイル同期オープン
  {
    gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
 
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId       = NIJI_ARCID_NUM(ARCID_ZUKAN_DATA);
    openReq.heapForFile = heap;
    openReq.heapForReq  = heap->GetLowerHandle();
 
    fileManager->SyncArcFileOpen(openReq);
  }
}

/**
 * @brief デストラクタ
 */
ExtendData::~ExtendData()
{
  if( m_bin )
  {
    GflHeapSafeFreeMemory( m_bin );
  }

  // ファイル同期クローズ
  {
    gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
    
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;  // 同期版だからヒープはNULLでよい。
    closeReq.arcId       = NIJI_ARCID_NUM(ARCID_ZUKAN_DATA);

    fileManager->SyncArcFileClose(closeReq);
  }
}

/**
 * @brief 同期読み込み
 *
 * @param heap  ヒープ
 */
void ExtendData::Load( gfl2::heap::HeapBase *heap )
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  const gfl2::fs::ArcFile* arc_file = fileManager->GetArcFile( NIJI_ARCID_NUM(ARCID_ZUKAN_DATA) );

  size_t size = 0;
  arc_file->GetDataSize( &size, GARC_zukan_data_zkn_extend_data_PACK, NULL );
  m_bin = GflHeapAllocMemoryAlign( heap, size, 4 );

  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq loadReq;
  loadReq.arcId   = NIJI_ARCID_NUM(ARCID_ZUKAN_DATA);
  loadReq.datId   = GARC_zukan_data_zkn_extend_data_PACK;
  loadReq.pBuf    = m_bin;
  loadReq.bufSize = size;
  fileManager->SyncArcFileLoadDataBuf(loadReq);
  
  m_bin_linker.Initialize( m_bin );

  //全国地方テーブル
  static const s32 ZKN_CHIHOU_NO_DAT_BL_IDX_TABLE[] =
  {
    BL_IDX_ZKN_EXTEND_DATA_ZKN_CHIHOUA_NO_DAT,  // LOCAL_AREA_A
    BL_IDX_ZKN_EXTEND_DATA_ZKN_CHIHOUB_NO_DAT,  // LOCAL_AREA_B
    BL_IDX_ZKN_EXTEND_DATA_ZKN_CHIHOUC_NO_DAT,  // LOCAL_AREA_C
    BL_IDX_ZKN_EXTEND_DATA_ZKN_CHIHOUD_NO_DAT,  // LOCAL_AREA_D
    BL_IDX_ZKN_EXTEND_DATA_ZKN_CHIHOUE_NO_DAT,  // LOCAL_AREA_E
  };
  for(s32 local_area=LOCAL_AREA_START; local_area<=LOCAL_AREA_END; ++local_area)
  {
    m_local_number_table[local_area-1]      = static_cast<u16*>(m_bin_linker.GetData( ZKN_CHIHOU_NO_DAT_BL_IDX_TABLE[local_area-1] ) );
    m_local_number_table_size[local_area-1] = m_bin_linker.GetDataSize( ZKN_CHIHOU_NO_DAT_BL_IDX_TABLE[local_area-1] );
    u16 local_number_max = 0;
    for(u32 i=1; i<=MONSNO_END; ++i)
    {
      if(local_number_max < m_local_number_table[local_area-1][i])
      {
        local_number_max = m_local_number_table[local_area-1][i];
      }
    }
    m_local_number_max[local_area-1] = local_number_max;
  }

  m_style_table = static_cast<u8*>( m_bin_linker.GetData( BL_IDX_ZKN_EXTEND_DATA_ZKN_STYLE_DAT ) );
  m_style_table_size  = m_bin_linker.GetDataSize( BL_IDX_ZKN_EXTEND_DATA_ZKN_STYLE_DAT );
      
  m_next_form_table = static_cast<u16*>( m_bin_linker.GetData( BL_IDX_ZKN_EXTEND_DATA_ZKN_NEXT_FORM_POS_DAT ) );
  m_next_form_table_size  = m_bin_linker.GetDataSize( BL_IDX_ZKN_EXTEND_DATA_ZKN_NEXT_FORM_POS_DAT );
}

//--------------------------------------------------------------------
// 取得
//--------------------------------------------------------------------
bool ExtendData::IsExistLocalZukanStatic( u32 zenkoku_num, LocalArea local_area )
{
  GFL_ASSERT( local_area == LOCAL_AREA_A );  // 開発中に気付かせるためのASSERT。

  return GetLocalNumberStatic( zenkoku_num, local_area ) != LOCAL_NUMBER_NONE;
}

/**
 * @brief 地方図鑑にいるかどうか
 *
 * @return true地方図鑑にいる falseいない
 */
bool ExtendData::IsExistLocalZukan( u32 zenkoku_num, LocalArea local_area ) const
{
  return this->GetLocalNumber( zenkoku_num, local_area ) != LOCAL_NUMBER_NONE;
}
bool ExtendData::IsExistLocalZukan( const pml::pokepara::CoreParam *core_param, LocalArea local_area ) const
{
  return this->IsExistLocalZukan( static_cast<u32>(core_param->GetMonsNo()), local_area );
}

u32 ExtendData::GetLocalNumberStatic( u32 zenkoku_num, LocalArea local_area )
{
  GFL_ASSERT( local_area == LOCAL_AREA_A );  // 開発中に気付かせるためのASSERT。

  u32 local_number = LOCAL_NUMBER_NONE;
  if( local_area == LOCAL_AREA_A )
  {
    if( zenkoku_num < GFL_NELEMS(ZKN_CHIHOUA_NO) )
    {
      local_number = ( ZKN_CHIHOUA_NO[zenkoku_num] != 0 ) ? ( ZKN_CHIHOUA_NO[zenkoku_num] ) : ( LOCAL_NUMBER_NONE );
    }
  }
  return local_number;
}
  
/**
 * @brief 地方図鑑番号を取得
 *
 * @return 地方図鑑番号 いない場合はLOCAL_NUMBER_NONE
 * 
 * @note あえて「全国図鑑番号」という意味合いのためMonsNo定義にしていません
 */
u32 ExtendData::GetLocalNumber( u32 zenkoku_num, LocalArea local_area ) const
{
  if( !(LOCAL_AREA_START<=local_area && local_area<=LOCAL_AREA_END) )
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    local_area = LOCAL_AREA_A;
  }

  if( !( zenkoku_num < m_local_number_table_size[local_area-1]/2 ) )
  {
    GFL_ASSERT( 0 );  //@fix
    return 0;
  }
  return m_local_number_table[local_area-1][ zenkoku_num ];
}
u32 ExtendData::GetLocalNumber( const pml::pokepara::CoreParam *core_param, LocalArea local_area ) const
{
  return this->GetLocalNumber(core_param->GetMonsNo(), local_area );
}

/**
 * @brief 地方図鑑のモンスター番号最大を取得
 *
 * @param local_area  ローカルエリア
 *
 * @return 最大数
 */
u32 ExtendData::GetLocalAreaMonsMax( LocalArea local_area ) const
{
  if( !(LOCAL_AREA_START<=local_area && local_area<=LOCAL_AREA_END) )
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    local_area = LOCAL_AREA_A;
  }

  return m_local_number_max[local_area-1];
}

/**
 * @brief 全国図鑑番号を取得
 *
 * @return 全国図鑑番号
 */
u32 ExtendData::GetZenkokuNumber( u32 local_num, LocalArea local_area ) const
{
  if( !(LOCAL_AREA_START<=local_area && local_area<=LOCAL_AREA_END) )
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    local_area = LOCAL_AREA_A;
  }

  for(u32 i=1; i<=MONSNO_END; ++i)
  {
    if( (static_cast<u32>(m_local_number_table[local_area-1][i])) == local_num )
    {
      return i;
    }
  }
  
  return MONSNO_NULL;
}

/**
 * @brief   形状取得
 *
 * @param simple_param    シンプルパラム版
 *
 * @return  形状
 */
ExtendData::StyleType ExtendData::GetStyleType( const SimpleParam & simple_param ) const
{
  u32 index = this->GetTableIndex( simple_param.monsNo, simple_param.formNo );
  
  if( !(index < m_style_table_size) )
  {
    GFL_ASSERT( 0 );  //@fix
    return STYLE_TYPE_CIRCLE;
  }
  return static_cast<StyleType>( m_style_table[ index ] );

}
ExtendData::StyleType ExtendData::GetStyleType( const pml::pokepara::CoreParam *core_param ) const
{
  SimpleParam simple_param;
  GetSimpleParam( &simple_param, core_param );
  return this->GetStyleType( simple_param );
}

//--------------------------------------------------------------------
// 取得
//--------------------------------------------------------------------
/**
 * @brief フォルム込みテーブルのインデックスを求める
 *
 * @param monsno  モンスター番号
 * @param formno  フォルム番号
 *
 * @return  インデックス
 * 
 * @note  フォルムが０のポケモンでもフォルムが１以上入っていることがある
 *        コフキムシ、コフウライ
 */
u32 ExtendData::GetTableIndex( MonsNo monsno, FormNo formno ) const
{  

  u16 formno_count = 0;
  u16 pos = static_cast<u16>(monsno);

  while( formno_count != formno )
  {
    pos = m_next_form_table[pos];
    if( pos == 0 ) break;
    formno_count++;
  }
  
  if( pos == 0 )
  {
    pos = static_cast<u16>(monsno);
    GFL_PRINT( "▼▼▼▼▼▼フォルム番号が異常 コフキムシならOK▼▼▼▼▼▼▼ mosno=%d form=%d pos=%d\n", monsno, formno, pos );
  }

  return pos;
}
  
GFL_NAMESPACE_END(PokeTool)

