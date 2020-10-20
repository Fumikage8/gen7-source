//======================================================================
/**
 * @file    SystemFont.cpp
 * @brief   システムフォントの初期化・取得
 * @author  ariizumi
 * @author  Hiroyuki Nakamura
 * @date    11/04/07
 *
 * @li  sangoのsystem_font.cppを移植
 */
//======================================================================
#include <../include/SystemFont.h>
#include <base/include/gfl2_Singleton.h>

#include <../../System/include/PokemonVersion.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/Font.gaix>

#if defined(GF_PLATFORM_CTR)
#include <nn/pl.h>
#endif


//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(view_print::SystemFont);
template class gfl2::base::SingletonAccessor<view_print::SystemFont>;
#endif

GFL_NAMESPACE_BEGIN(view_print)
//--------------------------------------------------------------
//
//--------------------------------------------------------------

SystemFont::SystemFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem ,bool bJpnFontLoadRegion)
  : mMainFont(NULL)
  , mStdFont(NULL)
  , mKujiraFontBuffer(NULL)
  , mKujiraFont(NULL)
  , mNumFont(NULL)
  , mNumFontBuffer(NULL)
  , mBattleFont(NULL)
  , mBattleFontBuffer(NULL)
{
  bool bSuccess;

  mpHeapDev = deviceMem;

#if defined(GF_PLATFORM_CTR)
  //内蔵の共有フォントの読み込み
  if (nn::pl::GetSharedFontLoadState() != nn::pl::SHARED_FONT_LOAD_STATE_LOADED)
  {
    NN_UTIL_PANIC_IF_FAILED(nn::pl::InitializeSharedFont());

    // 共有フォントのロードが完了するまで待機
    while (nn::pl::GetSharedFontLoadState() != nn::pl::SHARED_FONT_LOAD_STATE_LOADED)
    {
      // 共有フォントのロードに失敗していないか確認
      if (nn::pl::GetSharedFontLoadState() == nn::pl::SHARED_FONT_LOAD_STATE_FAILED)
      {
          NN_TPANIC_("failed to load shared font!\n");
      }
      nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(10));
      ARIIZUMI_PRINT("loading SharedFont ...\n");
    }
    ARIIZUMI_PRINT("load SharedFont\n");
  }
  mStdFontBuffer = nn::pl::GetSharedFontAddress();
  // フォントリソースをセットします
  mStdFont = GFL_NEW(heapMem)gfl2::str::ResFont();
  bSuccess = mStdFont->SetResource( mStdFontBuffer );
  NW_ASSERT( bSuccess );
#elif defined(GF_PLATFORM_WIN32)
  // @todo PCはどうするか？
  mStdFont = NULL;
#endif

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  { // ファイルオープン
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.fileName    = NULL;                                         // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
    req.arcId       = ARCID_FONT;                                   // ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
    req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // リクエストの実行優先度(値が小さい方が優先度が高い)
    req.heapForFile = heapMem;                                      // ヒープ。ファイルインスタンスの作成と索引テーブルの作成に使用。Closeするまで左記のものが保持される。
    req.heapForReq  = heapMem;                                      // リクエスト終了まで一時的に使用するヒープ。索引テーブルの作成のためにも使用する。ArcSrcのインスタンスやソーステーブルの作成のためにも使用する。
    man->SyncArcFileOpen( req );
  }

  {
    u32 arcId;
    // 日本語が積まれていない本体による分岐
    if(!bJpnFontLoadRegion)
    {
      arcId = GARC_Font_cbf_std_kujira_for_jpuseu_BFFNT;
    }
    else
    {
#if 1
      arcId = GARC_Font_cbf_std_kujira_for_jpuseu_BFFNT;
#else
      GFL_ASSERT( 0 );
      arcId = GARC_font_kujira_for_jpuseu_BCFNT;  // @todo 未実装
#endif
    }
    const gfl2::fs::ArcFile * arc_file = man->GetArcFile( ARCID_FONT );  //@todo 現在フォントが大きいので、メモリーが確保できなければ動かないように書き換えた、k.ohno
    size_t fileSize;
    {
      gfl2::fs::Result fs_result = arc_file->GetDataSize( &fileSize, arcId, heapMem );
      if( fs_result.IsSuccess() == false )
      {
        GFL_ASSERT(0);
      }
    }
    mKujiraFontBuffer = GflHeapAllocMemoryAlign( deviceMem, fileSize, nw::font::GlyphDataAlignment );
    if(mKujiraFontBuffer){ // 読み込み
      gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
      req.fileName          = NULL;                                         // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
      req.arcId             = ARCID_FONT;                                   // ARCID。ARCID_NULLのときはfileNameが使われる。
      req.datId             = arcId;                                        // アーカイブファイル内データID
      req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // リクエストの実行優先度(値が小さい方が優先度が高い)
      req.pBuf              = mKujiraFontBuffer;                            // 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
      req.bufSize           = fileSize;                                     // pBufのサイズ。データサイズ(パディングを含む)以上にして下さい。
      req.pRealReadSize     = NULL;                                         // 実際に読み込んだサイズ。compressedがtrueのとき解凍後のサイズ。NULLでもよい。
      req.heapForReq        = NULL;                                         // リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
      req.heapForCompressed = NULL;                                         // 解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。
      man->SyncArcFileLoadDataBuf( req );
      mKujiraFont = GFL_NEW(heapMem) gfl2::str::ResFont();
      bSuccess = mKujiraFont->SetResource( mKujiraFontBuffer );
      NW_ASSERT( bSuccess );
    }
  }

  //kujira用フォント
  //mKujiraFont = gfl::str::CreateResFont( "rom:/font/kujira_base.bcfnt", heapMem, deviceMem );

  //日韓の共有フォント作成
#if defined(GF_PLATFORM_CTR)
  if(mKujiraFont){
    mMainFont = GFL_NEW(heapMem) nw::font::PairFont(mKujiraFont,mStdFont);
  }
#elif defined(GF_PLATFORM_WIN32)
  // @todo PCはどうするか？
  mMainFont = mKujiraFont;
#endif
  //mMainFont = mSharedFont;
  //mMainFont = GFL_NEW(heapMem)nw::font::PairFont(mKujiraFont,mStdFont);
  //mMainFont = mStdFont;

/*
  nw::font::CharWidths charData;
  charData = mKujiraFont->GetDefaultCharWidths();
  ARIIZUMI_PRINT("Kujira[%d][%d][%d][%d][%d]\n",charData.left,charData.glyphWidth ,charData.charWidth,mKujiraFont->GetWidth(),mKujiraFont->GetHeight() );
  ARIIZUMI_PRINT("Kujira[%d][%d][%d]\n",mKujiraFont->GetBaselinePos(),mKujiraFont->GetCellHeight(),mKujiraFont->GetCellWidth() );
  ARIIZUMI_PRINT("Kujira[%d]\n",mKujiraFont->GetTextureFormat() );
  charData = mStdFont->GetDefaultCharWidths();
  ARIIZUMI_PRINT("Shared[%d][%d][%d][%d][%d]\n",charData.left,charData.glyphWidth ,charData.charWidth,mStdFont->GetWidth(),mStdFont->GetHeight() );
  ARIIZUMI_PRINT("Shared[%d][%d][%d]\n",mStdFont->GetBaselinePos(),mStdFont->GetCellHeight(),mStdFont->GetCellWidth() );
  ARIIZUMI_PRINT("Shared[%d]\n",mStdFont->GetTextureFormat() );
  charData = mMainFont->GetDefaultCharWidths();
  ARIIZUMI_PRINT("Pair  [%d][%d][%d][%d][%d]\n",charData.left,charData.glyphWidth ,charData.charWidth,mMainFont->GetWidth(),mMainFont->GetHeight() );
  ARIIZUMI_PRINT("Pair  [%d][%d][%d]\n",mMainFont->GetBaselinePos(),mMainFont->GetCellHeight(),mMainFont->GetCellWidth() );
  ARIIZUMI_PRINT("Pair  [%d]\n",mMainFont->GetTextureFormat() );
  ARIIZUMI_PRINT("[%x][%d]\n",mFontArc,mFontArc->GetDataCount());
*/  

}

SystemFont::~SystemFont()
{
  Terminate();
  GflHeapSafeFreeMemory(mKujiraFontBuffer);
  ///GFL_SAFE_DELETE( mKujiraFontBuffer );
  ///GFL_SAFE_DELETE( mMainFont );
  GFL_SAFE_DELETE( mKujiraFont );
  GFL_SAFE_DELETE( mStdFont );

  { // ファイルクローズ
    gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.fileName   = NULL;                                        // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
    req.arcId      = ARCID_FONT;                                  // ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
    req.prio       = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL; // リクエストの実行優先度(値が小さい方が優先度が高い)
    req.heapForReq = NULL;                                        // リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
    man->SyncArcFileClose( req );
  }
}

void SystemFont::Initialize(void)
{
/* @todo font2で消えた機能

  // 描画用バッファの設定。 (処理速度高速化のため。)
  // 内部でgx系の関数が必要な為、nngxInitialize後(g3d::GraphicSystem初期化後)
  gfl2::heap::HeapBase *heap = mpHeapDev;

  {
    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mStdFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( heap , drawBufferSize, 128 );
    ARIIZUMI_PRINT("SystemFont Alloc drawbuffer[%d]byte\n",drawBufferSize);
    OHNO_PRINT("SystemFont Alloc drawbuffer[%d]byte\n",drawBufferSize);
    NW_NULL_ASSERT(drawBuffer);
    mStdFont->SetDrawBuffer( drawBuffer );
  }
  {
    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mKujiraFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( heap , drawBufferSize, 128 );
    ARIIZUMI_PRINT("SystemFont Alloc drawbuffer[%d]byte\n",drawBufferSize);
    OHNO_PRINT("SystemFont Alloc drawbuffer[%d]byte\n",drawBufferSize);
    NW_NULL_ASSERT(drawBuffer);
    mKujiraFont->SetDrawBuffer( drawBuffer );
  }
*/
}

void SystemFont::Terminate()
{
/* @todo font2で消えた機能

  // 描画用バッファの解除と解放
  {
    void *const drawBuffer = mKujiraFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
  }
  {
    void *const drawBuffer = mStdFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
  }
*/
}

//拡張フォント関連
//----------------------------------------------------------------------------
/**
 *  @brief  numフォントの作成
 *          下にグローバル関数版があります。そっちを使ってください。
 *
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont::CreateNumFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  if( mNumFont == NULL )
  {
    gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

    const gfl2::fs::ArcFile * arc_file = man->GetArcFile( ARCID_FONT );
    size_t fileSize;
    {
      gfl2::fs::Result fs_result = arc_file->GetDataSize( &fileSize, GARC_Font_num_font_BFFNT, heapMem );
      if( fs_result.IsSuccess() == false )
      {
        GFL_ASSERT(0);
      }
    }
    mNumFontBuffer = GflHeapAllocMemoryAlign( deviceMem, fileSize, nw::font::GlyphDataAlignment );

    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.fileName          = NULL;                                         // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    req.arcId             = ARCID_FONT;                                   // ARCID。ARCID_NULLのときはfileNameが使われる。
    req.datId             = GARC_Font_num_font_BFFNT;                     // アーカイブファイル内データID
    req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // リクエストの実行優先度(値が小さい方が優先度が高い)
    req.pBuf              = mNumFontBuffer;                               // 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
    req.bufSize           = fileSize;                                     // pBufのサイズ。データサイズ(パディングを含む)以上にして下さい。
    req.pRealReadSize     = NULL;                                         // 実際に読み込んだサイズ。compressedがtrueのとき解凍後のサイズ。NULLでもよい。
    req.heapForReq        = NULL;                                         // リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
    req.heapForCompressed = NULL;                                         // 解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。
    man->SyncArcFileLoadDataBuf( req );

    mNumFont = GFL_NEW(heapMem) gfl2::str::ResFont();
    const bool bSuccess = mNumFont->SetResource( mNumFontBuffer );
    NW_ASSERT( bSuccess );

/* @todo font2で消えた機能
    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mNumFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( deviceMem , drawBufferSize, 128 );
    ARIIZUMI_PRINT("NumFontSize[%d] buffer[%d]\n",fileSize,drawBufferSize );
    NW_NULL_ASSERT(drawBuffer);
    mNumFont->SetDrawBuffer( drawBuffer );
*/
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  numフォントの破棄
 *          下にグローバル関数版があります。そっちを使ってください。
 */
//-----------------------------------------------------------------------------
void SystemFont::DeleteNumFont(void)
{
  if( mNumFont )
  {
/* @todo font2で消えた機能
    void *const drawBuffer = mNumFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
*/
    GFL_DELETE mNumFont;
    GflHeapFreeMemory( mNumFontBuffer );

    mNumFont = NULL;
    mNumFontBuffer = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントの作成
 *          下にグローバル関数版があります。そっちを使ってください。
 *
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont::CreateBattleFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  if( mBattleFont == NULL )
  {
    gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

    const gfl2::fs::ArcFile * arc_file = man->GetArcFile( ARCID_FONT );
    size_t fileSize;
    {
      gfl2::fs::Result fs_result = arc_file->GetDataSize( &fileSize, GARC_Font_kujira_battle_BFFNT, heapMem );
      if( fs_result.IsSuccess() == false )
      {
        GFL_ASSERT(0);
      }
    }
    mBattleFontBuffer = GflHeapAllocMemoryAlign( deviceMem, fileSize, nw::font::GlyphDataAlignment );

    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.fileName          = NULL;                                         // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    req.arcId             = ARCID_FONT;                                   // ARCID。ARCID_NULLのときはfileNameが使われる。
    req.datId             = GARC_Font_kujira_battle_BFFNT;                // アーカイブファイル内データID
    req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // リクエストの実行優先度(値が小さい方が優先度が高い)
    req.pBuf              = mBattleFontBuffer;                            // 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
    req.bufSize           = fileSize;                                     // pBufのサイズ。データサイズ(パディングを含む)以上にして下さい。
    req.pRealReadSize     = NULL;                                         // 実際に読み込んだサイズ。compressedがtrueのとき解凍後のサイズ。NULLでもよい。
    req.heapForReq        = NULL;                                         // リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
    req.heapForCompressed = NULL;                                         // 解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。
    man->SyncArcFileLoadDataBuf( req );

    mBattleFont = GFL_NEW(heapMem) gfl2::str::ResFont();
    const bool bSuccess = mBattleFont->SetResource( mBattleFontBuffer );
    NW_ASSERT( bSuccess );

/* @todo font2で消えた機能
    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mBattleFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( deviceMem , drawBufferSize, 128 );
    ARIIZUMI_PRINT("BattleFontSize[%d] buffer[%d]\n",fileSize,drawBufferSize );
    NW_NULL_ASSERT(drawBuffer);
    mBattleFont->SetDrawBuffer( drawBuffer );
*/
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントの破棄
 *          下にグローバル関数版があります。そっちを使ってください。
 */
//-----------------------------------------------------------------------------
void SystemFont::DeleteBattleFont(void)
{
  if( mBattleFont )
  {
/* @todo font2で消えた機能
    void *const drawBuffer = mBattleFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
*/
    GFL_DELETE mBattleFont;
    GflHeapFreeMemory( mBattleFontBuffer );

    mBattleFont = NULL;
    mBattleFontBuffer = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  点字フォントの作成
 *          下にグローバル関数版があります。そっちを使ってください。
 *
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
/*
void SystemFont::CreateTenjiFont(gfl::heap::HeapBase *heapMem , gfl::heap::HeapBase *deviceMem)
{
  if( mTenjiFont == NULL )
  {
    u32 fileSize = mFontArc->GetDataSize(GARC_font_tenji_font_BCFNT);
    ARIIZUMI_PRINT("[%x][%d]\n",mFontArc,mFontArc->GetDataCount());
    mTenjiFontBuffer = GflHeapAllocMemoryAlign( deviceMem, fileSize, nw::font::GlyphDataAlignment );
    mFontArc->LoadData( GARC_font_tenji_font_BCFNT , mTenjiFontBuffer );

    mTenjiFont = GFL_NEW(heapMem)gfl::str::ResFont();
    const bool bSuccess = mTenjiFont->SetResource( mTenjiFontBuffer );
    NW_ASSERT( bSuccess );

    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mTenjiFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( deviceMem , drawBufferSize, 128 );
    ARIIZUMI_PRINT("TenjiFontSize[%d] buffer[%d]\n",fileSize,drawBufferSize );
    NW_NULL_ASSERT(drawBuffer);
    mTenjiFont->SetDrawBuffer( drawBuffer );
  }
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  点字フォントの破棄
 *          下にグローバル関数版があります。そっちを使ってください。
 */
//-----------------------------------------------------------------------------
/*
void SystemFont::DeleteTenjiFont(void)
{
  if( mTenjiFont )
  {
    void *const drawBuffer = mTenjiFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
    GFL_DELETE mTenjiFont;
    GflHeapFreeMemory( mTenjiFontBuffer );

    mTenjiFont = NULL;
    mTenjiFontBuffer = NULL;
  }
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  アプリ毎の初期化
 *          ※要gx初期化
 *
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont_Initialize(void)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->Initialize();
}

//----------------------------------------------------------------------------
/**
 *  @brief  アプリ毎の解放
 */
//-----------------------------------------------------------------------------
void SystemFont_Terminate()
{
  GFL_SINGLETON_INSTANCE(SystemFont)->Terminate();
}

//----------------------------------------------------------------------------
/**
 *  @brief  フォントの取得
 *
 *  @return gfl::str::Font フォント
 */
//-----------------------------------------------------------------------------
gfl2::str::Font* SystemFont_GetFont()
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  共有フォントバッファの取得
 *          ResFontじゃないと取得できないので、日米欧フォントのバッファ
 *
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
void* SystemFont_GetFontBuffer()
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetFontBuffer();
}

//内蔵フォントのみ取得(実験用)
gfl2::str::Font* SystemFont_GetSharedFont()
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetSharedFont();
}


//----------------------------------------------------------------------------
/**
 *  @brief  numフォントの作成
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont_CreateNumFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->CreateNumFont(heapMem,deviceMem);
}

//----------------------------------------------------------------------------
/**
 *  @brief  numフォントの破棄
 */
//-----------------------------------------------------------------------------
void SystemFont_DeleteNumFont(void)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->DeleteNumFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  numフォントの取得
 *  @return gfl::str::Font フォント
 */
//-----------------------------------------------------------------------------
gfl2::str::Font* SystemFont_GetNumFont(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetNumFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  num共有フォントバッファの取得
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
void* SystemFont_GetNumFontBuffer(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetNumFontBuffer();
}

//----------------------------------------------------------------------------
/**
 *  @brief  追加されたnumフォント情報を消す
 */
//-----------------------------------------------------------------------------
void SystemFont_ResetNumFontInfo(void)
{
  gfl2::lyt::LytSys::ResetDefaultFontInfo(print::DEFAULT_FONT_LAYOUT_NUM_FONT);
}

//拡張フォント関連
//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントの作成
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont_CreateBattleFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->CreateBattleFont(heapMem,deviceMem);
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントの破棄
 */
//-----------------------------------------------------------------------------
void SystemFont_DeleteBattleFont(void)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->DeleteBattleFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントの取得
 *  @return gfl::str::Font フォント
 */
//-----------------------------------------------------------------------------
gfl2::str::Font* SystemFont_GetBattleFont(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetBattleFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトル共有フォントバッファの取得
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
void* SystemFont_GetBattleFontBuffer(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetBattleFontBuffer();
}

//----------------------------------------------------------------------------
/**
 *  @brief  追加されたバトルフォント情報を消す
 */
//-----------------------------------------------------------------------------
void SystemFont_ResetBattleFontInfo(void)
{
  gfl2::lyt::LytSys::ResetDefaultFontInfo(print::DEFAULT_FONT_LAYOUT_BATTLE_FONT);
}

//----------------------------------------------------------------------------
/**
 *  @brief  点字フォントの作成
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
/*
void SystemFont_CreateTenjiFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->CreateTenjiFont(heapMem,deviceMem);
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  点字フォントの破棄
 */
//-----------------------------------------------------------------------------
/*
void SystemFont_DeleteTenjiFont(void)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->DeleteTenjiFont();
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  点字フォントの取得
 *  @return gfl::str::Font フォント
 */
//-----------------------------------------------------------------------------
/*
gfl2::str::Font* SystemFont_GetTenjiFont(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetTenjiFont();
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  点字共有フォントバッファの取得
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
/*
void* SystemFont_GetTenjiFontBuffer(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetTenjiFontBuffer();
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  defaultフォントを０番目にセットする
 */
//-----------------------------------------------------------------------------
void SystemFont_SetLytSystemDefaultFont(void)
{
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetFont( ), print::LAYOUT_DEFAULT_FONT, print::DEFAULT_FONT_IDX_KUJIRA_DEFAULT );
}

//----------------------------------------------------------------------------
/**
 *  @brief  numフォントを１番目にセットする
 */
//-----------------------------------------------------------------------------
void SystemFont_SetLytSystemNumFont(void)
{
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetNumFont(), print::LAYOUT_NUM_FONT, print::DEFAULT_FONT_LAYOUT_NUM_FONT );
}


//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントを２番目にセットする
 */
//-----------------------------------------------------------------------------
void SystemFont_SetLytSystemBattleFont(void)
{
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetBattleFont(), print::LAYOUT_BATTLE_FONT, print::DEFAULT_FONT_LAYOUT_BATTLE_FONT );
}

//----------------------------------------------------------------------------
/**
 *  @brief  tenjiフォントを３番目にセットする
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
/*
void SystemFont_SetLytSystemTenjiFont(void)
{
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetTenjiFont(), print::LAYOUT_TENJI_FONT, print::DEFAULT_FONT_LAYOUT_TENJI_FONT );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  追加されたフォント情報を消す
 */
//-----------------------------------------------------------------------------
void SystemFont_ResetDefaultFontInfo(void)
{
  for(int i = (print::DEFAULT_FONT_IDX_KUJIRA_DEFAULT+1) ;i < print::DEFAULT_FONT_IDX_MAX ; i++){
    gfl2::lyt::LytSys::ResetDefaultFontInfo(i);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  追加された点字フォントを消す
 */
//-----------------------------------------------------------------------------
/*
void SystemFont_ResetTenjiFont(void)
{
  gfl2::lyt::LytSys::ResetDefaultFontInfo(print::DEFAULT_FONT_LAYOUT_TENJI_FONT);
}
*/


GFL_NAMESPACE_END(view_print)
