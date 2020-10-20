//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		StringBinary.cpp
 *	@brief  文字ソート用のバイナリ管理クラス
 *	@author	Toru=Nagihashi
 *	@date		2012.09.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// module
#include "AppLib/include/Sort/StringBinary.h"
// gflib2
#include <debug/include/gfl2_Assert.h>
// niji
#include "System/include/PokemonVersion.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "GameSys/include/GameManager.h"
// resource
#include "arc_def_index/arc_def.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)

//-----------------------------------------------------------------------
// 生成、破棄
//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
StringBinary::StringBinary() :
  m_sort_table(NULL),
  m_sort_table_size(0),
  m_initial_index(NULL),
  m_initial_index_size(0),
  m_initial_to_sort(NULL),
  m_heap(NULL),
  m_workHeap(NULL),
  m_sortTableID(gfl2::fs::ArcFile::ARCDATID_NULL),
  m_initialIndexID(gfl2::fs::ArcFile::ARCDATID_NULL),
  m_initialToSortID(gfl2::fs::ArcFile::ARCDATID_NULL),
  m_tmpBuffer(NULL),
  m_tmpSize(0),
  m_seq(0)
{
}
/**
 * @brief デストラクタ
 */
StringBinary::~StringBinary()
{
  GflHeapSafeFreeMemory( m_sort_table );
  GflHeapSafeFreeMemory( m_initial_index );
  GflHeapSafeFreeMemory( m_initial_to_sort );
}

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
 */
void StringBinary::LoadSync( gfl2::heap::HeapBase * heap, gfl2::fs::ArcFile::ARCDATID initial_index_datid, gfl2::fs::ArcFile::ARCDATID sort_table_datid, gfl2::fs::ArcFile::ARCDATID initial_to_sort_datid )
{
  GflHeapSafeFreeMemory( m_sort_table );
  GflHeapSafeFreeMemory( m_initial_index );
  GflHeapSafeFreeMemory( m_initial_to_sort );

  app::util::FileAccessor::FileOpenSync( ARCID_STRING_SORT_BINARY, heap, true );
  {
    gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
    void * buffer;
    size_t size;

    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId = ARCID_STRING_SORT_BINARY;
    req.ppBuf = &buffer;
    req.pBufSize = &size;
    req.heapForBuf = heap;
    req.align = 4;

    req.datId = initial_index_datid;
    man->SyncArcFileLoadData( req );
    m_initial_index = static_cast<u16*>(buffer);
    m_initial_index_size = size;

    req.datId = sort_table_datid;
    man->SyncArcFileLoadData( req );
    m_sort_table = static_cast<u16*>(buffer);
    m_sort_table_size = size;

    req.datId = initial_to_sort_datid;
    man->SyncArcFileLoadData( req );
    m_initial_to_sort = static_cast<InitialToSortHeader*>(buffer);
  }
  app::util::FileAccessor::FileCloseSync( ARCID_STRING_SORT_BINARY );
}

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
void StringBinary::LoadSetup(
      gfl2::heap::HeapBase * heap,
      gfl2::heap::HeapBase * work_heap,
      gfl2::fs::ArcFile::ARCDATID initial_index_datid,
      gfl2::fs::ArcFile::ARCDATID sort_table_datid,
      gfl2::fs::ArcFile::ARCDATID initial_to_sort_datid )
{
  m_heap = heap;
  m_workHeap = work_heap;
  m_sortTableID = sort_table_datid;
  m_initialIndexID = initial_index_datid;
  m_initialToSortID = initial_to_sort_datid;
  m_seq = 0;
}

/**
 * @brief 非同期読み込み
 * 
 * @retval  true  終了
 * @retval  false それ以外
 */
bool StringBinary::LoadMain(void)
{
  switch( m_seq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( ARCID_STRING_SORT_BINARY, m_workHeap );
    m_seq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen(ARCID_STRING_SORT_BINARY) == false )
    {
      break;
    }
    LoadRequest( m_initialIndexID );
    m_seq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad(&m_tmpBuffer) == false )
    {
      break;
    }
    m_initial_index = static_cast<u16*>(m_tmpBuffer);
    m_initial_index_size = m_tmpSize;
    LoadRequest( m_sortTableID );
    m_seq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileLoad(&m_tmpBuffer) == false )
    {
      break;
    }
    m_sort_table = static_cast<u16*>(m_tmpBuffer);
    m_sort_table_size = m_tmpSize;
    LoadRequest( m_initialToSortID );
    m_seq++;
    /* FALL THROUGH */
  case 4:
    if( app::util::FileAccessor::IsFileLoad(&m_tmpBuffer) == false )
    {
      break;
    }
    m_initial_to_sort = static_cast<InitialToSortHeader*>(m_tmpBuffer);
    app::util::FileAccessor::FileClose( ARCID_STRING_SORT_BINARY, m_workHeap );
    m_seq++;
    /* FALL THROUGH */
  case 5:
    if( app::util::FileAccessor::IsFileClose(ARCID_STRING_SORT_BINARY) == false )
    {
      break;
    }
    m_seq = 0;
    return true;
  }

  return false;
}

/**
 * @brief 非同期読み込みリクエスト
 * 
 * @param dat_id  アークデータID
 */
void StringBinary::LoadRequest( gfl2::fs::ArcFile::ARCDATID dat_id )
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId      = ARCID_STRING_SORT_BINARY;
  req.datId      = dat_id;
  req.ppBuf      = &m_tmpBuffer;
  req.pBufSize   = &m_tmpSize;
  req.heapForBuf = m_heap;
  req.align      = 4;
  req.heapForReq = m_heap->GetLowerHandle();

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->AddArcFileLoadDataReq( req );
}


//-----------------------------------------------------------------------
// 取得
//-----------------------------------------------------------------------
/**
 * @brief ソート順番を取得する
 *
 * @param index 順番（0が優先度高い、MAXが優先度低い）
 *
 * @return 中身（例えば特性のテーブルならば特性NOが入っている）
 */
u32 StringBinary::GetSortOrder( u32 index ) const
{
  if( index >= this->GetSortOrderMax() )
  {
    GFL_ASSERT_MSG( 0, "%d<%d", index, this->GetSortOrderMax() ); //@fix
    return 0;
  }
  return m_sort_table[ index ];
}

/**
 * @brief ソート順番の最大を取得する
 *
 * @return 順番最大
 */
u32 StringBinary::GetSortOrderMax( void ) const
{
  return m_sort_table_size / sizeof(u16);
}

/**
 * @brief 頭文字インデックスを取得する
 *
 * @param index 中身（例えば特性テーブルならば特性NOを入れる）
 *
 * @return  頭文字インデックス（インデックスの定義はそれぞれのヘッダを見ること）
 */
u32 StringBinary::GetInitialIndex( u32 index ) const
{
  if( index >= this->GetInitialIndexLength() )
  {
    GFL_ASSERT( 0 );  //@fix
    return 0;
  }
  return m_initial_index[ index ];
}

/**
 * @brief 上記イニシャルインデクス取得用のバッファ最大をとる
 */
u32 StringBinary::GetInitialIndexLength( void ) const
{
  return m_initial_index_size / sizeof(u16);
}
/**
 * @brief 頭文字インデックスの最大を取得
 *
 * @return  頭文字インデックスの最大
 */
u32 StringBinary::GetInitialIndexMax( void ) const
{
  /*
    @note 2016/12/07 by nakahiro
      未使用なのでアサート。
      定義がkujira/sangoのままなので使用する際は変更する必要がある。
  */
  GFL_ASSERT( 0 );

  switch( System::GetLang() )
  {
    //  日本語
  case POKEMON_LANG_JAPAN:
    return INITIAL_INDEX_JPN_MAX;
    //  韓国語
  case POKEMON_LANG_KOREA:
    return INITIAL_INDEX_KOR_MAX;
    //  言語コード：簡体字（中国）      CHN
  case POKEMON_LANG_CHINA:
    return 24;
    //  言語コード：繁体字（台湾、香港）    TWN
  case POKEMON_LANG_TAIWAN:
    return 35;
    //  その他言語
  default:
    return INITIAL_INDEX_EFIGS_MAX;
  }
}


/**
 * @brief 頭文字インデックスから、ソート順番用の頭出しインデックスを取得する
 *
 * @param initial_index 頭出しインデックス
 * @param start SortOrder用 頭文字開始インデックス
 * @param end SortOrder用 頭文字終了インデックス
 * 
 * @retval true その頭文字に対応するインデックスがあった
 * @retval false  その頭文字に対応するインデックスがない
 */
bool StringBinary::GetInitialIndexToSortHeaderIndex( u32 initial_index, u32 & start, u32 & end ) const
{
  start = m_initial_to_sort[ initial_index ].start;
  end = m_initial_to_sort[ initial_index ].end;
  
  return start != SORT_HEADER_INDEX_NONE &&
    end != SORT_HEADER_INDEX_NONE;
}


GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)
