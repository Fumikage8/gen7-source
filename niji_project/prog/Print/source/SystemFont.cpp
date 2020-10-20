//======================================================================
/**
 * @file    SystemFont.cpp
 * @brief   システムフォントの初期化・取得
 * @author  ariizumi
 * @author  Hiroyuki Nakamura
 * @author  k.ohno
 * @date    11/04/07
 *
 * @li  sangoのsystem_font.cppを移植
 */
//======================================================================
#include <Print/include/SystemFont.h>
#include <base/include/gfl2_Singleton.h>

#include <System/include/PokemonVersion.h>
#include <System/include/PokemonLanguage.h>

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
SINGLETON_INSTANCE_DEFINITION(print::SystemFont)
template class gfl2::base::SingletonAccessor<print::SystemFont>;
#endif

GFL_NAMESPACE_BEGIN(print)
//--------------------------------------------------------------
//
//--------------------------------------------------------------

SystemFont::SystemFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem ,bool bJpnFontLoadRegion)
  : mMainFont(NULL)
  , mStdFont(NULL)
  , mKujiraFont(NULL)
  , mKujiraFontBuffer(NULL)
  , mNumFont(NULL)
  , mNumFontBuffer(NULL)
  , mBattleFont(NULL)
  , mBattleFontBuffer(NULL)
{
  bool bSuccess;
  mMainFontNo=-1;
  
  mpHeapDev = deviceMem;

  mStdFont = NULL;

  size_t fileSize;

  SyncOpen( heapMem );  // ファイルオープン
  
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  const gfl2::fs::ArcFile * arc_file = man->GetArcFile( ARCID_FONT );  //@attention 現在フォントが大きいので、メモリーが確保できなければ動かないように書き換えた、k.ohno

  { //フォント用シェーダー読み込み
    gfl2::fs::Result fs_result = arc_file->GetDataSize( &fileSize, GARC_Font_nwfont2_RectDrawerShader_SHBIN, heapMem );
     void* pBuff = GflHeapAllocMemoryAlign( deviceMem, fileSize, nw::font::GlyphDataAlignment );

    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.fileName          = NULL;                                         // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    req.arcId             = ARCID_FONT;                                   // ARCID。ARCID_NULLのときはfileNameが使われる。
    req.datId             = GARC_Font_nwfont2_RectDrawerShader_SHBIN;                                        // アーカイブファイル内データID
    req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // リクエストの実行優先度(値が小さい方が優先度が高い)
    req.pBuf              = pBuff;                            // 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
    req.bufSize           = fileSize;                                     // pBufのサイズ。データサイズ(パディングを含む)以上にして下さい。
    req.pRealReadSize     = NULL;                                         // 実際に読み込んだサイズ。compressedがtrueのとき解凍後のサイズ。NULLでもよい。
    req.heapForReq        = NULL;                                         // リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
    req.heapForCompressed = NULL;                                         // 解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。
    man->SyncArcFileLoadDataBuf( req );

    mpRectDrawerShaderRes = GFL_NEW(deviceMem) gfl2::lyt::LytShaderResource();
    mpRectDrawerShaderRes->SetData(pBuff, fileSize);
  }

  ChangeMainFont(GFL_SINGLETON_INSTANCE(System::Language)->GetMsgLangId() ,heapMem, deviceMem);  //GFNMCat:866

}

SystemFont::~SystemFont()
{
  Terminate();


  GflHeapSafeFreeMemory( mKujiraFontBuffer );
  GFL_SAFE_DELETE( mKujiraFont );

  GFL_SAFE_DELETE( mpRectDrawerShaderRes );
  
  SyncClose();  // ファイルクローズ
}

void SystemFont::Initialize(void)
{
  // font2で機能が消えたのでやることなくなった
}

void SystemFont::Terminate()
{
  // font2で機能が消えたのでやることなくなった
}


//-----------------------------------------------------------------------------
/**
 * @func    SyncOpen
 * @brief   アークファイルオープン（同期）
 * @date    2016.05.02
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont::SyncOpen( gfl2::heap::HeapBase * heap )
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.fileName    = NULL;                                         // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
  req.arcId       = ARCID_FONT;                                   // ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
  req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // リクエストの実行優先度(値が小さい方が優先度が高い)
  req.heapForFile = heap;                                         // ヒープ。ファイルインスタンスの作成と索引テーブルの作成に使用。Closeするまで左記のものが保持される。
  req.heapForReq  = heap->GetLowerHandle();                       // リクエスト終了まで一時的に使用するヒープ。索引テーブルの作成のためにも使用する。ArcSrcのインスタンスやソーステーブルの作成のためにも使用する。
  gfl2::fs::System::GetAsyncFileManagerForRomDevice()->SyncArcFileOpen( req );
}

//-----------------------------------------------------------------------------
/**
 * @func    SyncOpen
 * @brief   アークファイルクローズ（同期）
 * @date    2016.05.02
 */
//-----------------------------------------------------------------------------
void SystemFont::SyncClose(void)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.fileName   = NULL;                                        // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
  req.arcId      = ARCID_FONT;                                  // ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
  req.prio       = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL; // リクエストの実行優先度(値が小さい方が優先度が高い)
  req.heapForReq = NULL;                                        // リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
  gfl2::fs::System::GetAsyncFileManagerForRomDevice()->SyncArcFileClose( req );
}


//拡張フォント関連

//-----------------------------------------------------------------------------
/**
 * @func    CreateFontBuffer
 * @brief   フォントのバッファを生成
 * @date    2015.07.17
 *
 * @param   heapMem     サイズ取得作業用ヒープ
 * @param   deviceMem   バッファ生成先ヒープ
 * @param   dat_id      フォントのデータID
 * @param   size        生成したバッファのサイズ格納場所
 *
 * @return  生成したバッファ
 */
//-----------------------------------------------------------------------------
void * SystemFont::CreateFontBuffer( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem, gfl2::fs::ArcFile::ARCDATID dat_id, size_t * size )
{
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  const gfl2::fs::ArcFile * arc_file = man->GetArcFile( ARCID_FONT );
  gfl2::fs::Result fs_result = arc_file->GetDataSize( size, dat_id, heapMem );
  if( fs_result.IsSuccess() == false )
  {
    GFL_ASSERT(0);
  }
  return GflHeapAllocMemoryAlign( deviceMem, *size, nw::font::GlyphDataAlignment );
}




void SystemFont::ChangeMainFont( int no, gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem )
{
  if( mMainFontNo == no ){
    return;
  }
  mMainFontNo=no;

//  bool bSuccess;
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  const gfl2::fs::ArcFile * arc_file = man->GetArcFile( ARCID_FONT );
  size_t fileSize;

  
  GflHeapSafeFreeMemory( mKujiraFontBuffer );
  GFL_SAFE_DELETE( mKujiraFont );

  {
    u32 arcId;

    switch(no){
    case System::MSGLANGID_CHINA:
      arcId = GARC_Font_cbf_ch_BFFNT;
      break;
		case System::MSGLANGID_TAIWAN:
      arcId = GARC_Font_cbf_tw_BFFNT;
      break;
    default:
      arcId = GARC_Font_cbf_std_kujira_BFFNT;
      break;
    }

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
      bool setRes = mKujiraFont->SetResource( mKujiraFontBuffer );
      GFL_ASSERT( setRes );
    }
    else{
      GFL_ASSERT(0);
    }
  }

  mMainFont = mKujiraFont;

}


//-----------------------------------------------------------------------------
/**
 * @func    SetNumFontResource
 * @brief   NUMフォントのリソースをセット
 * @date    2015.07.18
 *
 * @param   heapMem   ResFont作成ヒープ
 *
 * @note  下にグローバル関数版があります。そっちを使ってください。
 */
//-----------------------------------------------------------------------------
void SystemFont::SetNumFontResource( gfl2::heap::HeapBase * heapMem )
{
  mNumFont = GFL_NEW(heapMem) gfl2::str::ResFont();
  const bool bSuccess = mNumFont->SetResource( mNumFontBuffer );
  NW_ASSERT( bSuccess );
}

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
    size_t fileSize;
    mNumFontBuffer = CreateFontBuffer( heapMem, deviceMem, GARC_Font_num_font_BFFNT, &fileSize );

    gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

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

    SetNumFontResource( heapMem );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadNumFont
 * @brief   NUMフォントの読み込み（非同期）
 * @date    2015.07.17
 *
 * @param   heapMem     作業用ヒープ
 * @param   deviceMem   バッファ生成先ヒープ
 *
 * @note  下にグローバル関数版があります。そっちを使ってください。
 */
//-----------------------------------------------------------------------------
void SystemFont::LoadNumFont( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem )
{
  if( mNumFont == NULL )
  {
    size_t fileSize;
    mNumFontBuffer = CreateFontBuffer( heapMem, deviceMem, GARC_Font_num_font_BFFNT, &fileSize );

    gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.fileName          = NULL;                                         // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    req.arcId             = ARCID_FONT;                                   // ARCID。ARCID_NULLのときはfileNameが使われる。
    req.datId             = GARC_Font_num_font_BFFNT;                     // アーカイブファイル内データID
    req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // リクエストの実行優先度(値が小さい方が優先度が高い)
    req.pBuf              = mNumFontBuffer;                               // 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
    req.bufSize           = fileSize;                                     // pBufのサイズ。データサイズ(パディングを含む)以上にして下さい。
    req.pRealReadSize     = NULL;                                         // 実際に読み込んだサイズ。compressedがtrueのとき解凍後のサイズ。NULLでもよい。
    req.heapForReq        = heapMem->GetLowerHandle();                    // リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
    req.heapForCompressed = NULL;                                         // 解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。

    man->AddArcFileLoadDataBufReq( req );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsLoadNumFont
 * @brief   NUMフォントの読み込み終了チェック
 * @date    2015.07.17
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 *
 * @note  下にグローバル関数版があります。そっちを使ってください。
 */
//-----------------------------------------------------------------------------
bool SystemFont::IsLoadNumFont(void)
{
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  return man->IsArcFileLoadDataBufFinished( mNumFontBuffer );
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
    GFL_DELETE mNumFont;
    GflHeapFreeMemory( mNumFontBuffer );

    mNumFont = NULL;
    mNumFontBuffer = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetBattleFontResource
 * @brief   バトルフォントのリソースをセット
 * @date    2015.07.18
 *
 * @param   heapMem   ResFont作成ヒープ
 *
 * @note  下にグローバル関数版があります。そっちを使ってください。
 */
//-----------------------------------------------------------------------------
void SystemFont::SetBattleFontResource( gfl2::heap::HeapBase * heapMem )
{
  if ( mBattleFont == NULL )
  {
    mBattleFont = GFL_NEW(heapMem) gfl2::str::ResFont();
    const bool bSuccess = mBattleFont->SetResource( mBattleFontBuffer );
    GFL_ASSERT_STOP( bSuccess );
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
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  if( mBattleFont == NULL )
  {
    size_t fileSize;
    mBattleFontBuffer = CreateFontBuffer( heapMem, deviceMem, GARC_Font_kujira_battle_BFFNT, &fileSize );

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

    SetBattleFontResource( heapMem );
  }


}

//-----------------------------------------------------------------------------
/**
 * @func    LoadBattleFont
 * @brief   バトルフォントの読み込み（非同期）
 * @date    2015.07.18
 *
 * @param   heapMem     作業用ヒープ
 * @param   deviceMem   バッファ生成先ヒープ
 *
 * @note  下にグローバル関数版があります。そっちを使ってください。
 */
//-----------------------------------------------------------------------------
void SystemFont::LoadBattleFont( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem )
{
  if( mBattleFont == NULL )
  {
    size_t fileSize;
    mBattleFontBuffer = CreateFontBuffer( heapMem, deviceMem, GARC_Font_kujira_battle_BFFNT, &fileSize );

    gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.fileName          = NULL;                                         // ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
    req.arcId             = ARCID_FONT;                                   // ARCID。ARCID_NULLのときはfileNameが使われる。
    req.datId             = GARC_Font_kujira_battle_BFFNT;                // アーカイブファイル内データID
    req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // リクエストの実行優先度(値が小さい方が優先度が高い)
    req.pBuf              = mBattleFontBuffer;                            // 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
    req.bufSize           = fileSize;                                     // pBufのサイズ。データサイズ(パディングを含む)以上にして下さい。
    req.pRealReadSize     = NULL;                                         // 実際に読み込んだサイズ。compressedがtrueのとき解凍後のサイズ。NULLでもよい。
    req.heapForReq        = heapMem->GetLowerHandle();                    // リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
    req.heapForCompressed = NULL;                                         // 解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。

    man->AddArcFileLoadDataBufReq( req );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsLoadBattleFont
 * @brief   バトルフォントの読み込み終了チェック
 * @date    2015.07.18
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 *
 * @note  下にグローバル関数版があります。そっちを使ってください。
 */
//-----------------------------------------------------------------------------
bool SystemFont::IsLoadBattleFont(void)
{
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  return man->IsArcFileLoadDataBufFinished( mBattleFontBuffer );
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

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_LoadNumFont
 * @brief   NUMフォントの読み込み（非同期）
 * @date    2015.07.18
 *
 * @param   heapMem     作業用ヒープ
 * @param   deviceMem   バッファ生成先ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont_LoadNumFont( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem )
{
  GFL_SINGLETON_INSTANCE(SystemFont)->LoadNumFont(heapMem,deviceMem);
}

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_IsLoadNumFont
 * @brief   NUMフォントの読み込み終了チェック
 * @date    2015.07.18
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool SystemFont_IsLoadNumFont(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->IsLoadNumFont();
}

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_SetLoadNumFont
 * @brief   NUMフォントのリソースをセット
 * @date    2015.07.18
 *
 * @param   heapMem   ResFont作成ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont_SetLoadNumFont( gfl2::heap::HeapBase * heapMem )
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->SetNumFontResource(heapMem);
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
  return; // 常駐化されたため、以下の処理を呼び出せないようにした
//  GFL_SINGLETON_INSTANCE(SystemFont)->DeleteBattleFont();
}

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_LoadBattleFont
 * @brief   バトルフォントの読み込み（非同期）
 * @date    2015.07.18
 *
 * @param   heapMem     作業用ヒープ
 * @param   deviceMem   バッファ生成先ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont_LoadBattleFont( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem )
{
  return; // 常駐化されたため、以下の処理を呼び出せないようにした
//  GFL_SINGLETON_INSTANCE(SystemFont)->LoadBattleFont(heapMem,deviceMem);
}

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_IsLoadBattleFont
 * @brief   バトルフォントの読み込み終了チェック
 * @date    2015.07.18
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool SystemFont_IsLoadBattleFont(void)
{
  return true;  // 常駐化されたため、以下の処理を呼び出せないようにした
//  return GFL_SINGLETON_INSTANCE(SystemFont)->IsLoadBattleFont();
}

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_SetLoadBattleFont
 * @brief   バトルフォントのリソースをセット
 * @date    2015.07.18
 *
 * @param   heapMem   ResFont作成ヒープ
 */
//-----------------------------------------------------------------------------
void SystemFont_SetLoadBattleFont( gfl2::heap::HeapBase * heapMem )
{
  return; // 常駐化されたため、以下の処理を呼び出せないようにした
//  GFL_SINGLETON_INSTANCE(SystemFont)->SetBattleFontResource(heapMem);
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
 *  @brief  defaultフォントを０番目にセットする
 */
//-----------------------------------------------------------------------------
void SystemFont_SetLytSystemDefaultFont(void)
{
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetFont( ), LAYOUT_DEFAULT_FONT, print::DEFAULT_FONT_IDX_KUJIRA_DEFAULT );
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
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetBattleFont(), "kujira_battle.bffnt", print::DEFAULT_FONT_LAYOUT_BATTLE_FONT );
//  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetBattleFont(), print::LAYOUT_BATTLE_FONT, print::DEFAULT_FONT_LAYOUT_BATTLE_FONT );
}


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


GFL_NAMESPACE_END(print)
