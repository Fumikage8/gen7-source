//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapDivLoad.cpp
 *	@brief  マップブロック分割読み込み
 *	@author	tomoya takahashi
 *	@date		2011.01.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <grp/map/gfl_MapDivLoad.h>


namespace gfl{
namespace grp{
namespace map{

#define GFL_MAP_DIVLOAD_THREAD (1)

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
DivLoad::DivLoad( void ) : 
  m_Priority(0),
  m_CompFlag(false),
  m_pLoadBuff(NULL),
  m_pBuff(NULL),
  m_BuffSize(0),
  m_pArcFile(NULL),
  m_OneLoadSize(0),
  m_MemoryOffset(0),
  m_FileOffset(0),
  m_FileSize(0),
  m_pTmpHeap(NULL)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
DivLoad::~DivLoad()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	p_buff      バッファ
 *	@param	buffSize    バッファサイズ
 *	@param  p_tmp_heap  スレッド動作に必要なテンポラリワーク確保用
 *	@param  mode        動作モード
 *	@param  priority    スレッド優先順位
 */
//-----------------------------------------------------------------------------
void DivLoad::Initialize( void* p_buff, u32 buffSize, gfl::heap::HeapBase* p_tmp_heap, Mode mode, gfl::base::Thread::Priority priority )
{
  // 読み込み中チェック
  if( IsLoading() ){
    GFL_ASSERT( 0 );
    u32 count = 0;
    while( (ResetFileLoad() == false) && (count < 10000)  ){ 
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMicroSeconds( 10000 ) );
      ++count;
    }
    GFL_ASSERT_STOP( count < 10000 );
  }
  
  m_pLoadBuff   = p_buff;
  m_pBuff       = p_buff;
  m_BuffSize    = buffSize;
  m_pTmpHeap    = p_tmp_heap;
  if( mode < MODE_MAX ) {
    m_Mode      = mode;
    m_Priority  = priority;
  }else{
    GFL_ASSERT( mode < MODE_MAX ); //@check
    m_Mode      = MODE_PEACE_LOAD;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄
 */
//-----------------------------------------------------------------------------
void DivLoad::Finalize( void )
{
  // 特に破棄するものはなし。
}


//----------------------------------------------------------------------------
/**
 *	@brief  ファイル読み込み開始
 *
 *	@param	  p_arc         アーカイブファイル
 *	@param    dataID        データID
 *	@param    one_load_size 1回の読み込みサイズ
 */
//-----------------------------------------------------------------------------
void DivLoad::StartFileLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size )
{
  if(m_Mode == MODE_THREAD_LOAD){
    this->startFileSyncLoad( p_arc, dataID, one_load_size );
  }else{
    this->startFilePieceLoad( p_arc, dataID, one_load_size );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  圧縮の掛かったファイルの読み込み開始
 *
 *	@param	p_arc         アーカイブファイル
 *	@param  dataID        データＩＤ
 *	@param  one_load_size 一度に読み込むサイズ
 *	@param  gfl::heap::HeapBase* p_tmp_heap テンポラリバッファ生成用ヒープ
 */
//-----------------------------------------------------------------------------
void DivLoad::StartCompFileLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap )
{
  if(m_Mode == MODE_THREAD_LOAD){
    this->startFileSyncLoadComp( p_arc, dataID, one_load_size, p_tmp_heap );
  }else{
    this->startFilePieceLoadComp( p_arc, dataID, one_load_size, p_tmp_heap );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ファイル読み込み1回分実行
 *
 *  @retval true    ファイル読み込み完了
 *  @retval false   ファイル読み込み途中
 */
//-----------------------------------------------------------------------------
b32 DivLoad::ContinueFileLoad( void )
{
  if(m_Mode == MODE_THREAD_LOAD){
   return this->continueFileSyncLoad();
  }
  return this->continueFilePieceLoad();
}


//----------------------------------------------------------------------------
/**
 *	@brief  ファイル読み込み　リセット
 */
//-----------------------------------------------------------------------------
bool DivLoad::ResetFileLoad( void )
{
  if(m_Mode == MODE_THREAD_LOAD){
    return this->resetFileSyncLoad();
  }
  else{
    this->resetFilePieceLoad();
  }
  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  読み込み中かチェック
 *
 *	@retval true    読み込み中
 *	@retval false   読み込んでない
 */
//-----------------------------------------------------------------------------
b32 DivLoad::IsLoading( void ) const
{
  if(m_Mode == MODE_THREAD_LOAD){
    return this->isLoadingFileSyncLoad();
  }
  return this->isLoadingFilePieceLoad();
}




//----------------------------------------------------------------------------
/**
 *	@brief  分割読み込み開始
 *
 *	@param	p_arc             アーカイブファイル
 *	@param	dataID            データID
 *	@param	one_load_size     １回の読み込みサイズ
 */
//-----------------------------------------------------------------------------
void DivLoad::startFilePieceLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size )
{
  if(one_load_size == 0){
    GFL_ASSERT( one_load_size ); //@check
    one_load_size = 1;
  }

  
  m_pArcFile    = p_arc;
  m_DataID      = dataID;
  m_FileSize    = p_arc->GetDataSize( dataID );
  m_OneLoadSize = one_load_size;
  m_FileOffset  = 0;

  // シーク
  m_pArcFile->SeekDataPtr( m_DataID, m_FileOffset );

  // バッファオーバーチェック
  GFL_ASSERT_STOP( (m_FileSize + m_MemoryOffset) < m_BuffSize );

  m_CompFlag = false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  分割読み込み　圧縮ファイルの場合
 *
 *	@param	p_arc               アーカイブファイル
 *	@param	dataID              データＩＤ
 *	@param	one_load_size       １回の読み込みサイズ
 *	@param	p_tmp_heap          圧縮データ読み込みよう一時バッファ確保先
 */
//-----------------------------------------------------------------------------
void DivLoad::startFilePieceLoadComp( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap )
{
  if(one_load_size == 0){
    GFL_ASSERT( one_load_size ); //@check
    one_load_size = 1;
  }

  
  m_pArcFile    = p_arc;
  m_DataID      = dataID;
  m_FileSize    = p_arc->GetDataSize( dataID );
  m_OneLoadSize = one_load_size;
  m_FileOffset  = 0;

  // シーク
  m_pArcFile->SeekDataPtr( m_DataID, m_FileOffset );

  // 読み込み用バッファ生成
  m_pLoadBuff = GflHeapAllocMemoryLow( p_tmp_heap, m_FileSize );
  

  m_CompFlag = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  分割読み込みの更新
 *
 *	@retval true  完了
 *	@retval false 継続
 */
//-----------------------------------------------------------------------------
b32 DivLoad::continueFilePieceLoad(void)
{
  //読み込み途中チェック
  if( m_FileOffset >= m_FileSize ){
    return true;
  }

  b32 ret = false;
  u32 loadSize = 0;
  uptr buffAddr = reinterpret_cast<uptr>(m_pLoadBuff);
  u32 memory_offset;

  // ライスと読み込みチェック
  if( (m_FileOffset + m_OneLoadSize) >= m_FileSize ){
    loadSize  = m_FileSize - m_FileOffset;
    ret = true;
  }else{
    loadSize = m_OneLoadSize;
  }

  //GFL_PRINT( "Offset 0x%x loadOne 0x%x\n", m_FileOffset, loadSize );

  // バッファの読み込み先を求める
  if( m_CompFlag ){
    memory_offset = m_FileOffset;
  }else{
    memory_offset = m_MemoryOffset;
  }

  // 読み込み
  m_pArcFile->LoadDataPtr( loadSize, reinterpret_cast<void*>(buffAddr + memory_offset) );
  m_FileOffset += loadSize;

  if( !m_CompFlag ){
    m_MemoryOffset += loadSize;
  }


  if( ret ){
    // 圧縮データ解凍
    if( m_CompFlag ){

      uptr destAddr = reinterpret_cast<uptr>(m_pBuff);
      m_FileSize = gfl::cx::GetUncompressedSize( m_pLoadBuff );
      // バッファオーバーチェック
      GFL_ASSERT_STOP( (m_FileSize + m_MemoryOffset) < m_BuffSize );

      gfl::cx::Uncompress( m_pLoadBuff, reinterpret_cast<void*>(destAddr + m_MemoryOffset) );  // 解凍
      GflHeapFreeMemory( m_pLoadBuff );
      m_pLoadBuff = m_pBuff;
      m_CompFlag = false;
    }
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  分割読み込みのリセット
 */
//-----------------------------------------------------------------------------
void DivLoad::resetFilePieceLoad(void)
{
  m_pArcFile    = NULL;
  m_DataID      = 0;
  m_OneLoadSize = 0;
  m_MemoryOffset= 0;
  m_FileSize    = 0;
  m_FileOffset  = 0;

  if( m_CompFlag ){
    GflHeapFreeMemory( m_pLoadBuff );
    m_pLoadBuff = NULL;
    m_pLoadBuff = m_pBuff;
    m_CompFlag = false;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  読み込みチェック
 *
 *	@retval true    読み込み中
 *	@retval false   読み込み完了
 */
//-----------------------------------------------------------------------------
b32 DivLoad::isLoadingFilePieceLoad(void) const
{
  if( m_FileSize > m_FileOffset ){
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  スレッド読み込み開始
 *
 *	@param	p_arc           アーカイブファイル
 *	@param	dataID          データID
 *	@param  one_load_size   １回の読み込みサイズ　（0=データサイズ)
 */
//-----------------------------------------------------------------------------
void DivLoad::startFileSyncLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size )
{
  bool result;

  m_pArcFile    = p_arc;
  m_FileSize    = p_arc->GetDataSize( dataID );
  m_DataID      = dataID;
  m_FileOffset  = 0;


  if(one_load_size == 0){
    one_load_size = m_FileSize;
  }
  m_OneLoadSize = one_load_size;

  // バッファオーバーチェック
  GFL_ASSERT_STOP( (m_FileSize + m_MemoryOffset) < m_BuffSize );

  //GFL_PRINT( "arc start data_id %d\n", dataID );
  m_pLoadBuff = m_pBuff;

  // 非同期読み込み開始
  uptr buffAddr = reinterpret_cast<uptr>(m_pLoadBuff);
  result = m_pArcFile->StartAsyncRead( m_pTmpHeap, dataID, 
      reinterpret_cast<void*>(buffAddr + m_MemoryOffset),
      m_Priority  );


  GFL_ASSERT_STOP( result );

  m_CompFlag = false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  圧縮形式のファイルをスレッド読み込み開始
 *
 *	@param	p_arc           アーカイブファイル
 *	@param	dataID          データＩＤ
 *	@param	one_load_size   １回の読み込みサイズ（0=データサイズ）
 *	@param  p_tmp_heap      テンポラリバッファ確保ヒープベース
 */
//-----------------------------------------------------------------------------
void DivLoad::startFileSyncLoadComp( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap )
{
  bool result;
  m_pArcFile    = p_arc;
  m_FileSize    = p_arc->GetDataSize( dataID );
  m_DataID      = dataID;
  m_FileOffset  = 0;


  if(one_load_size == 0){
    one_load_size = m_FileSize;
  }
  m_OneLoadSize = one_load_size;

  // 読み込みようメモリを確保
  m_pLoadBuff = GflHeapAllocMemoryLow( p_tmp_heap, m_FileSize );

  // 非同期読み込み開始
  result = m_pArcFile->StartAsyncRead( m_pTmpHeap, dataID, 
      m_pLoadBuff,
      m_Priority  );


  GFL_ASSERT_STOP( result );
  m_CompFlag = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  スレッド読み込み　終了まち
 *
 *	@retval true    終了
 *	@retval false   継続
 */
//-----------------------------------------------------------------------------
b32 DivLoad::continueFileSyncLoad(void)
{
  if( m_pArcFile->WaitAsyncRead() == false ){

    //GFL_PRINT( "arc end\n" );
  
    if( m_CompFlag ){

      uptr buffAddr = reinterpret_cast<uptr>(m_pBuff);

      // LoadBufferの内容を解凍して、m_pBufferに格納する。
      // m_pBufferのm_MemoryOffsetの先から格納する。
      m_FileSize = gfl::cx::GetUncompressedSize( m_pLoadBuff );
      
      // バッファオーバーチェック
      GFL_ASSERT_STOP( (m_FileSize + m_MemoryOffset) < m_BuffSize );
      gfl::cx::Uncompress( m_pLoadBuff, reinterpret_cast<void*>(buffAddr + m_MemoryOffset) );  // 解凍

      GflHeapFreeMemory( m_pLoadBuff );
      m_pLoadBuff = m_pBuff;
      m_CompFlag = false;
    }
    
    
    m_FileOffset    = m_FileSize;
    m_MemoryOffset += m_FileSize;

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  スレッド読み込み　中断
 *	＊one_load_size分読んだところで中断します。
 */
//-----------------------------------------------------------------------------
bool DivLoad::resetFileSyncLoad(void)
{
  if(m_pArcFile){
    // 読み込みきるべき。
    if( m_pArcFile->WaitAsyncRead() ){

      // 強制的にとめる。
      //m_pArcFile->CancelAsyncRead();
      TOMOYA_PRINT( "DivLoad cancel \n" );
      
      return false;
    }
  }

  m_pArcFile    = NULL;
  m_DataID      = 0;
  m_OneLoadSize = 0;
  m_MemoryOffset= 0;
  m_FileSize    = 0;
  m_FileOffset  = 0;


  if( m_CompFlag ){
    GflHeapFreeMemory( m_pLoadBuff );
    m_pLoadBuff = NULL;
    m_pLoadBuff = m_pBuff;
    m_CompFlag = false;
  }
  
  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  読み込みチェック
 *
 *	@retval true    読み込み中
 *	@retval false   読み込み完了
 */
//-----------------------------------------------------------------------------
b32 DivLoad::isLoadingFileSyncLoad(void) const
{
  if( m_FileSize > m_FileOffset ){
    return true;
  }
  return false;
}



} //namespace map{
} //namespace grp{  
} //namespace gfl{
