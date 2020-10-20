//==========================================================================
/**
 * @file   gfl_Ro_Manager.cpp
 * @brief  動的モジュール管理クラス
 * @author Hisanao Suzuki
 */
//==========================================================================
#include <ro/gfl_RoManager.h>


#if GFL_ENABLE_RO


//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(gfl::ro::RoManager);
template class gfl::base::SingletonAccessor<gfl::ro::RoManager>;




namespace gfl{
namespace ro{


/** ---------------------------------------------------------------------
//! @brief  動的モジュールのロード（関数版）
//! @param  croFileName CROファイル名
//! @return 動的モジュールへのポインタ
// ---------------------------------------------------------------------*/
nn::ro::Module* StartModuleFunc( const char* croFileName, bool is_lower )
{
  RoManager*      roMgr   = GFL_SINGLETON_INSTANCE( RoManager );
  nn::ro::Module* pModule;
  
  if( is_lower ){
    pModule = roMgr->LoadModuleLower( croFileName );
  }else{
    pModule = roMgr->LoadModule( croFileName );
  }

  roMgr->StartModule( pModule );

  return pModule;
}


/** ---------------------------------------------------------------------
//! @brief  コンストラクタ
//! @param  無し
//! @return 無し
// ---------------------------------------------------------------------*/
RoManager::RoManager( gfl::heap::HeapBase *modulesHeap ) :
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
void* RoManager::LoadFile( gfl::heap::HeapBase* heap, const char* fileName, size_t *size )
{
  gfl::fs::ReadFile file;
  bool              open_ret = file.Open(fileName);
  void*             buffer   = NULL;
  size_t            allocSize;

  GFL_ASSERT_STOP( open_ret );

  u32 fileSize = file.GetSize();

  // 確保するメモリは4096アラインする
  allocSize = nn::math::RoundUp(fileSize, 0x1000);
  buffer    = GflHeapAllocMemoryAlign4096( heap, allocSize );
  GFL_ASSERT_STOP( buffer );

  s32 readSize = file.Read( buffer, fileSize );
  file.Close();

  if( readSize != fileSize )
  {
    GflHeapFreeMemory(buffer);
    buffer = NULL;
    *size  = 0;
  }
  else{
    *size  = readSize;
  }

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
void RoManager::Initialize( gfl::heap::HeapBase* heap, const char* crsFileName, const char* crrFileName, u32 moduleMax )
{
  if( !m_isInit )
  {
    size_t crsFileSize = 0;
    size_t crrFileSize = 0;

    m_crsBuffer = LoadFile( heap, crsFileName, &crsFileSize );
    GFL_ASSERT_STOP( m_crsBuffer );
    m_crrBuffer = LoadFile( heap, crrFileName, &crrFileSize );
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
nn::ro::Module* RoManager::LoadModule( const char* croFileName, nn::ro::FixLevel fixLevel )
{
  if( m_moduleMax <= m_registNum )
  {
    GFL_PRINT( "RoManager::LoadModule() module is Buffer Over!\n" );
    GFL_ASSERT_STOP( false );
    return NULL;
  }

  size_t               croFileSize;
  gfl::heap::HeapBase* heap       = m_heapForModules;
  void*                croDataPtr = LoadFile( heap, croFileName, &croFileSize );
  uptr                 roUseAddr  = NULL;

  GFL_ASSERT_STOP( croDataPtr );

  // CRO 用のバッファを確保
  nn::ro::SizeInfo sizeInfo;
  nn::Result sizeResult = nn::ro::GetSizeInfo(&sizeInfo, croDataPtr);

  GFL_ASSERT_STOP(sizeResult.IsSuccess());

  // BSS用バッファ確保(8バイトアライメント）
  bit8* bufferPtr = (bit8*)GflHeapAllocMemoryAlign( heap, sizeInfo.bufferSize, 8 );

  // モジュールをロード
  nn::ro::Module* pModule = nn::ro::LoadModule(croDataPtr, croFileSize, bufferPtr, sizeInfo.bufferSize, true, fixLevel);
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
nn::ro::Module* RoManager::LoadModuleLower( const char* croFileName, nn::ro::FixLevel fixLevel )
{
  if( m_moduleMax <= m_registNum )
  {
    GFL_PRINT( "RoManager::LoadModule() module is Buffer Over!\n" );
    GFL_ASSERT_STOP( false );
    return NULL;
  }

  size_t               croFileSize;
  gfl::heap::HeapBase* heap       = m_heapForModules->GetLowerHandle();
  void*                croDataPtr = LoadFile( heap, croFileName, &croFileSize );
  uptr                 roUseAddr  = NULL;

  GFL_ASSERT_STOP( croDataPtr );

  // CRO 用のバッファを確保
  nn::ro::SizeInfo sizeInfo;
  nn::Result sizeResult = nn::ro::GetSizeInfo(&sizeInfo, croDataPtr);

  GFL_ASSERT_STOP(sizeResult.IsSuccess());

  // BSS用バッファ確保(8バイトアライメント）
  bit8* bufferPtr = (bit8*)GflHeapAllocMemoryAlign( heap, sizeInfo.bufferSize, 8 );

  // モジュールをロード
  nn::ro::Module* pModule = nn::ro::LoadModule(croDataPtr, croFileSize, bufferPtr, sizeInfo.bufferSize, true, fixLevel);
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
//! @brief  動的モジュールの開始
//! @param  targetModule 開始対象となる動的モジュール
//! @param  isLinkCheck  シンボルリンクが解決されているかを厳密にチェックするならtrueを指定
//! @attention 実行に失敗する条件である場合はASSERTで停止させる
// ---------------------------------------------------------------------*/
void RoManager::StartModule( nn::ro::Module* targetModule, bool isLinkCheck )
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
void RoManager::DisposeModule( nn::ro::Module* targetModule )
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
s32 RoManager::GetModuleIndex( nn::ro::Module* targetModule )
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


} // namespace ro
} // namespace gfl




#endif // GFL_ENABLE_RO

