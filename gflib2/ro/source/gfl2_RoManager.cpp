//==========================================================================
/**
 * @file   gfl2_RoManager.cpp
 * @brief  動的モジュール管理クラス
 * @author Hisanao Suzuki
 *  150227 niji対応 ariizumi
 */
//==========================================================================
#include <fs/include/gfl2_Fs.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <ro/include/gfl2_RoManager.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(gfl2::ro::RoManager)
template class gfl2::base::SingletonAccessor<gfl2::ro::RoManager>;

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(ro)

/** ---------------------------------------------------------------------
//! @brief  動的モジュールのロード（関数版）
//! @param  croFileName CROファイル名
//! @return 動的モジュールへのポインタ
// ---------------------------------------------------------------------*/
Module* StartModuleFunc( gfl2::fs::AsyncFileManager *fileMng , const char* croFileName, bool is_lower )
{
#if GFL_RO_ENABLE
  RoManager*      roMgr   = GFL_SINGLETON_INSTANCE( RoManager );
  nn::ro::Module* pModule;
  
  if( is_lower ){
    pModule = roMgr->LoadModuleLower( fileMng , croFileName );
  }else{
    pModule = roMgr->LoadModule( fileMng , croFileName );
  }

  roMgr->StartModule( pModule );

  return pModule;
#else
  return NULL;
#endif

}

#if GFL_RO_ENABLE

/** ---------------------------------------------------------------------
//! @brief  コンストラクタ
//! @param  無し
//! @return 無し
// ---------------------------------------------------------------------*/
RoManager::RoManager( gfl2::heap::HeapBase *modulesHeap ) :
  m_registList(NULL),
  m_modules(NULL),
  m_bssBuffer(NULL),
  m_moduleBuffer(NULL),
  m_heapForModules(modulesHeap),
  m_moduleMax(0),
  m_registNum(0),
  m_isInit(false)
{
  ::std::memset( &m_crsBuffer, 0, sizeof(m_crsBuffer) );
  ::std::memset( &m_crrBuffer, 0, sizeof(m_crrBuffer) );
}


/** ---------------------------------------------------------------------
//! @brief  デストラクタ
//! @param  無し
//! @return 無し
// ---------------------------------------------------------------------*/
RoManager::~RoManager( void )
{
}


/** ---------------------------------------------------------------------
//! @brief  ファイル読み込み処理
//! @param  heap        ヒープ管理クラスへのポインタ
//! @param  fileName    読み込みたいファイル名
//! @param  size        ファイルサイズを格納する先のポインタ
//! @return 読み込まれたファイル内容を格納したバッファ情報
// ---------------------------------------------------------------------*/
void* RoManager::LoadFile( gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager *fileMng , const char* fileName, size_t *size )
{
  void*             buffer   = NULL;
  gfl2::fs::Result result;

  gfl2::fs::AsyncFileManager::FileEasyReadReq readReq;
  readReq.fileName = fileName;
  readReq.ppBuf = &buffer;
  readReq.pFileSize = size;
  readReq.heapForBuf = heap;
  readReq.align = 4096;
  readReq.heapForReq = heap;
  readReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
  readReq.result = &result;
  fileMng->SyncFileEasyRead( readReq );

  GFL_ASSERT_STOP( result.IsSuccess() );

  return buffer;
}


/** ---------------------------------------------------------------------
//! @brief  初期化処理
//! @param  heap        ヒープ管理クラスへのポインタ
//! @param  crsFileName CRSファイル名
//! @param  crrFileName CRRファイル名
//! @param  moduleMax   登録可能モジュール数
//! @return 無し
// ---------------------------------------------------------------------*/
void RoManager::Initialize( gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager *fileMng , const char* crsFileName, const char* crrFileName, u32 moduleMax )
{
  if( !m_isInit )
  {
    size_t crsFileSize = 0;
    size_t crrFileSize = 0;

    m_crsBuffer = LoadFile( heap, fileMng , crsFileName, &crsFileSize );
    GFL_ASSERT_STOP( m_crsBuffer );
    m_crrBuffer = LoadFile( heap, fileMng , crrFileName, &crrFileSize );
    GFL_ASSERT_STOP( m_crrBuffer );

    nn::ro::Initialize(m_crsBuffer, crsFileSize);

    m_registList = nn::ro::RegisterList(m_crrBuffer, crrFileSize);
    GFL_ASSERT_STOP( m_registList );

    m_registNum    = 0;
    m_moduleMax    = moduleMax;
    m_modules      = (nn::ro::Module**)GflHeapAllocMemory( heap, moduleMax * sizeof(nn::ro::Module*) );
    m_moduleBuffer = (void**)GflHeapAllocMemory( heap, moduleMax * sizeof(void*) );
    m_bssBuffer    = (bit8**)GflHeapAllocMemory( heap, moduleMax * sizeof(bit8*) );

    GFL_ASSERT_STOP( m_modules );
    GFL_ASSERT_STOP( m_moduleBuffer );
    GFL_ASSERT_STOP( m_bssBuffer );

    m_isInit = true;
  }
}


/** ---------------------------------------------------------------------
//! @brief  動的モジュールのロード
//! @param  heap        ヒープ管理クラスへのポインタ
//! @param  crsFileName CRSファイル名
//! @param  crrFileName CRRファイル名
//! @param  moduleMax   登録可能モジュール数
//! @param  fixLevel    モジュール解放時のメモリの解放段階。
//! @return 無し
// ---------------------------------------------------------------------*/
Module* RoManager::LoadModule( gfl2::fs::AsyncFileManager *fileMng , const char* croFileName, gfl2::heap::HeapBase* loadHeap ,FixLevel fixLevel )
{
  if( m_moduleMax <= m_registNum )
  {
    GFL_PRINT( "RoManager::LoadModule() module is Buffer Over!\n" );
    GFL_ASSERT_STOP( false );
    return NULL;
  }

  size_t               croFileSize;
  gfl2::heap::HeapBase* heap       = m_heapForModules;
  if( loadHeap )
  {
    heap = loadHeap;
  }
  void*                croDataPtr = LoadFile( heap, fileMng , croFileName, &croFileSize );

  GFL_ASSERT_STOP( croDataPtr );

  // CRO 用のバッファを確保
  nn::ro::SizeInfo sizeInfo;
  nn::Result sizeResult = nn::ro::GetSizeInfo(&sizeInfo, croDataPtr);

  GFL_ASSERT_STOP(sizeResult.IsSuccess());

  // BSS用バッファ確保(8バイトアライメント）
  bit8* bufferPtr = (bit8*)GflHeapAllocMemoryAlign( heap, sizeInfo.bufferSize, 8 );

  // モジュールをロード
  nn::ro::Module* pModule = nn::ro::LoadModule(croDataPtr, croFileSize, bufferPtr, sizeInfo.bufferSize, true, static_cast<nn::ro::FixLevel>(fixLevel));
  GFL_ASSERT_STOP(pModule);

  // 情報を格納
  m_modules[m_registNum]      = pModule;
  m_moduleBuffer[m_registNum] = croDataPtr;
  m_bssBuffer[m_registNum]    = bufferPtr;
  m_registNum++;

  GFL_PRINT( "RoManager::LoadModule(%s) [buffSize:0x%x][fileSize:0x%x]\n", croFileName, sizeInfo.bufferSize, croFileSize );

  return pModule;
}

//----------------------------------------------------------------------------
/**
//! @brief  動的モジュールのロード  ヒープの下から確保
//! @param  heap        ヒープ管理クラスへのポインタ
//! @param  crsFileName CRSファイル名
//! @param  crrFileName CRRファイル名
//! @param  moduleMax   登録可能モジュール数
//! @param  fixLevel    モジュール解放時のメモリの解放段階。
//! @return 無し
 */
//-----------------------------------------------------------------------------
Module* RoManager::LoadModuleLower( gfl2::fs::AsyncFileManager *fileMng , const char* croFileName,gfl2::heap::HeapBase* loadHeap, FixLevel fixLevel )
{
  if( m_moduleMax <= m_registNum )
  {
    GFL_PRINT( "RoManager::LoadModule() module is Buffer Over!\n" );
    GFL_ASSERT_STOP( false );
    return NULL;
  }

  size_t               croFileSize;
  gfl2::heap::HeapBase* heap       = m_heapForModules->GetLowerHandle();
  if( loadHeap )
  {
    heap = loadHeap;
  }
  void*                croDataPtr = LoadFile( heap, fileMng , croFileName, &croFileSize );

  GFL_ASSERT_STOP( croDataPtr );

  // CRO 用のバッファを確保
  nn::ro::SizeInfo sizeInfo;
  nn::Result sizeResult = nn::ro::GetSizeInfo(&sizeInfo, croDataPtr);

  GFL_ASSERT_STOP(sizeResult.IsSuccess());

  // BSS用バッファ確保(8バイトアライメント）
  bit8* bufferPtr = (bit8*)GflHeapAllocMemoryAlign( heap, sizeInfo.bufferSize, 8 );

  // モジュールをロード
  nn::ro::Module* pModule = nn::ro::LoadModule(croDataPtr, croFileSize, bufferPtr, sizeInfo.bufferSize, true, static_cast<nn::ro::FixLevel>(fixLevel));
  GFL_ASSERT_STOP(pModule);

  // 情報を格納
  m_modules[m_registNum]      = pModule;
  m_moduleBuffer[m_registNum] = croDataPtr;
  m_bssBuffer[m_registNum]    = bufferPtr;
  m_registNum++;

  GFL_PRINT( "RoManager::LoadModule(%s) [buffSize:0x%x][fileSize:0x%x]\n", croFileName, sizeInfo.bufferSize, croFileSize );

  return pModule;
}

/** ---------------------------------------------------------------------
//! @brief  動的モジュールのロード(バッファ渡し
//! @param  croDataPtr  モジュールのデータ
//! @param  croFileSize モジュールのデータサイズ
//! @param  loadHeap    DLLリンク用ヒープ(NULLでデフォルト)
//! @param  fixLevel    モジュール解放時のメモリの解放段階。
//! @return 無し
// ---------------------------------------------------------------------*/
Module* RoManager::LoadModuleSetBuffer( void* croDataPtr , size_t croFileSize, gfl2::heap::HeapBase* loadHeap ,FixLevel fixLevel )
{
  if( m_moduleMax <= m_registNum )
  {
    GFL_PRINT( "RoManager::LoadModule() module is Buffer Over!\n" );
    GFL_ASSERT_STOP( false );
    return NULL;
  }

  gfl2::heap::HeapBase* heap       = m_heapForModules;
  if( loadHeap )
  {
    heap = loadHeap;
  }

  GFL_ASSERT_STOP( croDataPtr );

  // CRO 用のバッファを確保
  nn::ro::SizeInfo sizeInfo;
  nn::Result sizeResult = nn::ro::GetSizeInfo(&sizeInfo, croDataPtr);

  GFL_ASSERT_STOP(sizeResult.IsSuccess());

  // BSS用バッファ確保(8バイトアライメント）
  bit8* bufferPtr = (bit8*)GflHeapAllocMemoryAlign( heap, sizeInfo.bufferSize, 8 );

  // モジュールをロード
  nn::ro::Module* pModule = nn::ro::LoadModule(croDataPtr, croFileSize, bufferPtr, sizeInfo.bufferSize, true, static_cast<nn::ro::FixLevel>(fixLevel));
  GFL_ASSERT_STOP(pModule);

  // 情報を格納
  m_modules[m_registNum]      = pModule;
  m_moduleBuffer[m_registNum] = croDataPtr;
  m_bssBuffer[m_registNum]    = bufferPtr;
  m_registNum++;

  //GFL_PRINT( "RoManager::LoadModule(%s) [buffSize:0x%x][fileSize:0x%x]\n", croFileName, sizeInfo.bufferSize, croFileSize );

  return pModule;
}

/** ---------------------------------------------------------------------
//! @brief  動的モジュールの開始
//! @param  targetModule 開始対象となる動的モジュール
//! @param  isLinkCheck  シンボルリンクが解決されているかを厳密にチェックするならtrueを指定
//! @attention 実行に失敗する条件である場合はASSERTで停止させる
// ---------------------------------------------------------------------*/
void RoManager::StartModule( Module* targetModule, bool isLinkCheck )
{
  GFL_ASSERT_STOP_MSG( (targetModule != NULL), "RoManager::StartModule() invalid argument[argument is NULL]! \n" );

  s32 index = GetModuleIndex( targetModule );

  GFL_ASSERT_STOP_MSG( (index >= 0), "RoManager::StartModule() invalid argument[argument is not Loaded]! \n" );

  if( isLinkCheck )
  {
    bool isLinked = targetModule->IsAllSymbolResolved();
    GFL_ASSERT_STOP_MSG( isLinked, "RoManager::StartModule() invalid argument[argument is not Linked]! \n" );
  }

  targetModule->DoInitialize();
}


/** ---------------------------------------------------------------------
//! @brief  登録されている全ての動的モジュールの解放
//! @param  無し
//! @return 無し
// ---------------------------------------------------------------------*/
void RoManager::DisposeAllModule( void )
{
  for( u32 i = 0; i < m_registNum; i++ )
  {
    if( m_modules[i] != NULL ){
      m_modules[i]->DoFinalize();
    }
  }
  for( u32 i = 0; i < m_registNum; i++ )
  {
    if( m_modules[i] != NULL )
    {
      m_modules[i]->Unload();
      m_modules[i] = NULL;
    }
    if( m_moduleBuffer[i] != NULL )
    {
      GflHeapFreeMemory( m_moduleBuffer[i] );
      m_moduleBuffer[i] = NULL;
    }
    if( m_bssBuffer[i] != NULL )
    {
      GflHeapFreeMemory( m_bssBuffer[i] );
      m_bssBuffer[i] = NULL;
    }
  }
  m_registNum = 0;
}


/** ---------------------------------------------------------------------
//! @brief  動的モジュールの解放
//! @param  targetModule 解放対象となる動的モジュール
//! @return 無し
// ---------------------------------------------------------------------*/
void RoManager::DisposeModule( Module* targetModule )
{
  s32 index = GetModuleIndex( targetModule );

  if( (index >= 0) && (m_registNum > 0) )
  {
    if( targetModule != NULL )
    {
      GFL_PRINT("Start DoFinalize()\n");
      targetModule->DoFinalize();
      GFL_PRINT("Start Unload(%s)\n", targetModule->GetName() );
      targetModule->Unload();
      GFL_PRINT("Finish Unload\n");

      if( m_moduleBuffer != NULL )
      {
        if( m_moduleBuffer[index] != NULL )
        {
          GflHeapFreeMemory( m_moduleBuffer[index] );
          m_moduleBuffer[index] = NULL;
        }
      }
    }
    if( m_bssBuffer != NULL )
    {
      if( m_bssBuffer[index] != NULL )
      {
        GflHeapFreeMemory( m_bssBuffer[index] );
        m_bssBuffer[index] = NULL;
      }
    }

    // リストを詰めましょう
    for( u32 i = index; i < (m_registNum-1); i++ )
    {
      if( m_bssBuffer != NULL ){
        m_bssBuffer[i]    = m_bssBuffer[i+1];
      }
      if( m_moduleBuffer != NULL ){
        m_moduleBuffer[i] = m_moduleBuffer[i+1];
      }
      if( m_modules != NULL ){
        m_modules[i]      = m_modules[i+1];
      }
    }

    // 念のためNULLに設定
    if( m_bssBuffer != NULL ){
      m_bssBuffer[m_registNum-1]    = NULL;
    }
    if( m_modules != NULL ){
      m_modules[m_registNum-1]      = NULL;
    }
    if( m_moduleBuffer != NULL ){
      m_moduleBuffer[m_registNum-1] = NULL;
    }
    m_registNum--;
  }
}


/** ---------------------------------------------------------------------
//! @brief  動的モジュールの解放
//! @param  targetModule 解放対象となる動的モジュール
//! @return 登録リストのインデックス値。0より小さい値の場合は未検出。
// ---------------------------------------------------------------------*/
s32 RoManager::GetModuleIndex( Module* targetModule )
{
  for( u32 i = 0; i < m_registNum; i++ )
  {
    if( m_modules[i] == targetModule ){
      return i;
    }
  }

  return -1;
}


/** ---------------------------------------------------------------------
//! @brief  終了処理
//! @param  無し
//! @return 無し
// ---------------------------------------------------------------------*/
void RoManager::Finalize( void )
{
  if( m_isInit )
  {
    DisposeAllModule();

    if( m_registList != NULL )
    {
      m_registList->Unregister();
      m_registList = NULL;
    }

    if( m_crsBuffer != NULL )
    {
      GflHeapFreeMemory( m_crsBuffer );
      m_crsBuffer = NULL;
    }
    if( m_crrBuffer != NULL )
    {
      GflHeapFreeMemory( m_crrBuffer );
      m_crrBuffer = NULL;
    }
    if( m_modules != NULL )
    {
      GflHeapFreeMemory( m_modules );
      m_modules = NULL;
    }
    if( m_moduleBuffer != NULL )
    {
      GflHeapFreeMemory( m_moduleBuffer );
      m_moduleBuffer = NULL;
    }
    if( m_bssBuffer != NULL )
    {
      GflHeapFreeMemory( m_bssBuffer );
      m_bssBuffer = NULL;
    }
    nn::ro::Finalize();

    m_isInit = false;
  }
}


#endif // GFL_RO_ENABLE

GFL_NAMESPACE_END(ro)
GFL_NAMESPACE_END(gfl2)


