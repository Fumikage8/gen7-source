//================================================================================
/**
 * @file   gfl_ImageDBAccessor.h
 * @brief  ImageDBを利用したSDカードへの写真入出力用クラス
 * @author hisanao suzuki
 * @date   2012.2.13
 */
//================================================================================
#include <imagedb/gfl_ImageDBAccessor.h>


#ifdef GFL_USE_IMAGE_DB


namespace gfl {
namespace imagedb {


/**---------------------------------------------------------------------------
  @param 独自アロケータ
 *---------------------------------------------------------------------------*/
class MyAllocator : public nn::fnd::IAllocator
{
  public:
    void Initialize( void *buf, int size ){ m_Heap.Initialize( reinterpret_cast<uptr>( buf ), size ); }
    void Finalize(){ m_Heap.Finalize(); }

    virtual void* Allocate( size_t size, s32 alignment ){
      return m_Heap.Allocate( size, alignment );
    }
    virtual void Free( void* memory ){
      m_Heap.Free( memory );
    }

  private:
    nn::fnd::ExpHeap m_Heap;
};



//-------------------------------------------------------------------------------
/**
 * @@brief コンストラクタ
 * @param  heap        ヒープ
 * @param  deviceHeap  デバイスヒープ
 * @param  appUniqueId アプリケーション固有ID(20bit)
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ImageDBAccessor( gfl::heap::HeapBase *heap, gfl::heap::HeapBase *deviceHeap, u32 appUniqueId ) :
  m_imgDbWork( NULL ),
  m_tmpAllocator( NULL ),
  m_heap( heap ),
  m_deviceHeap( deviceHeap ),
  m_topBuf( NULL ),
  m_topRightBuf( NULL),
  m_appUniqueId(appUniqueId),
  m_init( false ),
  m_resultDetail( imgdb::ResultSuccess )
{
  m_targetWidth  = 0;
  m_targetHeight = 0;
  m_format       = IMAGE_FORMAT;
  m_filePath[0]  = '\0';
}

//-------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::~ImageDBAccessor()
{
  Finalize();
}

//-------------------------------------------------------------------------------
/**
 * @brief ImageDbライブラリの初期化処理（開始時に１度だけコール）
 *
 * @param useHeapSize  ImageDbライブラリに割り当てるヒープサイズ
 * @param targetWidth  出力元ソースとなる画面の横幅
 * @param targetHeight 出力元ソースとなる画面の縦幅
 * @param isSimpleMode イメージユーティリティーを使用するためだけの初期化を行うならtrueを指定する
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ResultCode ImageDBAccessor::Initialize( u32 useHeapSize, u32 targetWidth, u32 targetHeight, bool isSimpleMode )
{
  if( !m_init )
  {
    m_targetWidth  = targetWidth;
    m_targetHeight = targetHeight;

    if( m_imgDbWork == NULL )
    {
      m_imgDbWork = GflHeapAllocMemory( m_heap, useHeapSize );

      if( m_imgDbWork == NULL ){
        return E_RET_OUT_OF_MEMORY;
      }
    }

    if( m_tmpAllocator == NULL )
    {
      m_tmpAllocator = GFL_NEW( m_heap ) MyAllocator();

      if( m_tmpAllocator == NULL ){
        return E_RET_OUT_OF_MEMORY;
      }
      m_tmpAllocator->Initialize( m_imgDbWork, useHeapSize );
    }
    m_init = true;

    imgdb::Initialize( *m_tmpAllocator );
  }
  if( isSimpleMode ){
    return E_RET_SUCCESS;
  }

  return AllocateImageBuffer();
}


//-------------------------------------------------------------------------------
/**
 * @brief  イメージバッファの確保
 * @return エラーコード
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ResultCode ImageDBAccessor::AllocateImageBuffer( void )
{
  // JPEG保存の各種設定を行う
  size_t nImageSize   = GetBufferSize();

  FreeImageBuffer();

  GFL_PRINT( "ImageDBAccessor allocated size[%d]\n", nImageSize );

  m_topBuf = GflHeapAllocMemoryAlign( m_deviceHeap , nImageSize, IMAGE_ALIGN_SIZE );
  if( m_topBuf == NULL ){
    return E_RET_OUT_OF_MEMORY;
  }

  m_topRightBuf = GflHeapAllocMemoryAlign( m_deviceHeap , nImageSize, IMAGE_ALIGN_SIZE );
  if( m_topRightBuf == NULL ){
    return E_RET_OUT_OF_MEMORY;
  }
  return E_RET_SUCCESS;
}

//-------------------------------------------------------------------------------
/**
 * @brief イメージバッファの解放
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::FreeImageBuffer( void )
{
  if( m_topBuf != NULL )
  {
    GflHeapFreeMemory( m_topBuf );
    m_topBuf = NULL;
  }
  if( m_topRightBuf != NULL ){
    GflHeapFreeMemory( m_topRightBuf );
    m_topRightBuf = NULL;
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ImageDbライブラリの終了処理（終了時に１度だけコール）
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::Finalize( void )
{
  if( m_init )
  {
    imgdb::Finalize();

    FreeImageBuffer();

    if( m_tmpAllocator != NULL )
    {
      m_tmpAllocator->Finalize();
      GFL_SAFE_DELETE( m_tmpAllocator );
    }

    if( m_imgDbWork != NULL )
    {
      GflHeapFreeMemory( m_imgDbWork );
      m_imgDbWork = NULL;
    }
    m_init = false;
  }
}


//-------------------------------------------------------------------------------
/**
 * @brief ブロックイメージからリニアイメージへ変換する
 *        ※本APIはGPUコマンドのため、実行完了に1フレームを要する！
 *
 * @param pSrcImg   変換元となるブロックイメージ（デバイスメモリ必須！）
 * @param pDstImg   リニアイメージ格納先（デバイスメモリ必須！）
 * @param width     イメージ横幅
 * @param height    イメージ縦幅
 * @param blockSize ブロックイメージのサイズ（8、32のいずれか）
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::ConvertImageBlock2Liner( const void * pSrcImg, void* pDstImg, u32 width, u32 height, u32 blockSize )
{
  nngxFlush3DCommand();
  nngxAddB2LTransferCommand( pSrcImg, width, height, gfl::grp::RENDER_COLOR_FORMAT_RGBA8,
                             pDstImg, width, height, gfl::grp::RENDER_COLOR_FORMAT_RGBA8,
                             NN_GX_ANTIALIASE_NOT_USED, GL_FALSE, blockSize );
}

//-------------------------------------------------------------------------------
/**
 * @brief リニアイメージからブロックイメージへ変換する
 *        ※本APIはGPUコマンドのため、実行完了に1フレームを要する！
 *
 * @param pSrcImg   変換元となるリニアイメージ（デバイスメモリ必須！）
 * @param pDstImg   ブロックイメージ格納先（デバイスメモリ必須！）
 * @param width     イメージ横幅
 * @param height    イメージ縦幅
 * @param blockSize ブロックイメージのサイズ（8、32のいずれか）
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::ConvertImageLiner2Block( const void * pSrcImg, void* pDstImg, u32 width, u32 height, u32 blockSize )
{
  nngxFlush3DCommand();
  nngxAddL2BTransferCommand( pSrcImg, pDstImg, width, height, gfl::grp::RENDER_COLOR_FORMAT_RGBA8, blockSize );
}

//-------------------------------------------------------------------------------
/**
 * @brief SDカードへの写真保存
 *        スクリーンショットのPixelイメージを渡すことで
 *        写真データとしてSDカードへ保存される。
 *
 *        ※引数で引き渡すPixelデータはリニアイメージである必要がある。
 *          ブロックイメージからリニアイメージへの変換はConvertImageBlock2Liner()を
 *          使用すること
 *
 * @param pImageTop      画面Pixelデータ(RGBA)
 * @param pImageTopRight 立体視用右画面Pixelデータ(RGBA)。NULLを指定することも可能。
 * @param width          イメージデータの横幅(2のべき乗であること！）
 * @param height         イメージデータの縦幅(2のべき乗であること！）
 * @param pUserMakerNote ユーザーノート用文字列
 * @param noteSize       ユーザーノート用文字列サイズ
 * @param jpegQuality    Jpegの画像品質（1〜100)
 *
 * @return リザルトコード
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ResultCode ImageDBAccessor::SaveToSDCard( const void* pImageTop, const void *pImageTopRight, u32 width, u32 height, const void* pUserMakerNote, u32 noteSize, int jpegQuality )
{
  if( !nn::fs::IsSdmcInserted() ){
     return E_RET_SDCARD_NOEXIST;
  }
  if( !nn::fs::IsSdmcWritable() ){
    return E_RET_SDCARD_READONLY;
  }
  if( (jpegQuality < 1) || (jpegQuality > 100) ){
    GFL_PRINT( "invalid jpegQuality(%d)!!\n", jpegQuality );
    return E_RET_FAILED;  // 品質指定が不正
  }

  CopyStereoView2Memory( const_cast<void*>(pImageTop),      const_cast<void*>(m_topBuf),      width, height, m_targetWidth, m_targetHeight );
  CopyStereoView2Memory( const_cast<void*>(pImageTopRight), const_cast<void*>(m_topRightBuf), width, height, m_targetWidth, m_targetHeight );

  if( pImageTopRight != NULL )
  {
    GFL_PRINT( "use MpSaver()\n" );
    imgdb::MpSaver* saver = GFL_NEW( m_heap ) imgdb::MpSaver();
    saver->SetJpegQuality( jpegQuality );
    m_resultDetail = SaveScreenshotToSDCard( saver, m_topBuf, m_topRightBuf, pUserMakerNote, noteSize );
    GFL_DELETE( saver );
  }
  else
  {
    GFL_PRINT( "use JpegSaver()\n" );
    imgdb::JpegSaver *saver = GFL_NEW( m_heap ) imgdb::JpegSaver();
    saver->SetJpegQuality( jpegQuality );
    m_resultDetail = SaveScreenshotToSDCard( saver, m_topBuf, pUserMakerNote, noteSize );
    GFL_DELETE( saver );
  }

  if( m_resultDetail != imgdb::ResultSuccess ){
    return E_RET_FAILED;
  }

  return E_RET_SUCCESS;
}

//-------------------------------------------------------------------------------
/**
 * @brief SDカードからの写真を読み込む
 *        格納先のバッファサイズは、GetBufferSize()にて取得すること
 *
 *        ※取得されるPixelデータはリニアイメージである。
 *          ブロックイメージへの変換はConvertImageLiner2Block()を使用すること
 *
 * @param filePath       読み込み対象となる写真のファイルパス
 * @param pImageTop      データ格納先バッファ
 * @param pImageTopRight 立体視用右画面Pixelデータ格納先バッファ。NULLを指定することも可能。
 * @param width          格納先バッファの画像横幅(2のべき乗であること！）
 * @param height         格納先バッファの画像縦幅(2のべき乗であること！）
 *
 * @return リザルトコード
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ResultCode ImageDBAccessor::LoadFromSDCard( const wchar_t * filePath, void* pImageTop, void *pImageTopRight, u32 width, u32 height )
{
  if( !nn::fs::IsSdmcInserted() ){
     return E_RET_SDCARD_NOEXIST;
  }
  if( !nn::fs::IsSdmcWritable() ){
    return E_RET_SDCARD_READONLY;
  }

  if( pImageTopRight != NULL )
  {
    GFL_PRINT( "use MpLoader()\n" );
    imgdb::MpLoader* loader = GFL_NEW( m_heap ) imgdb::MpLoader();
    m_resultDetail = LoadScreenshotFromSDCard( loader, filePath, m_topBuf, m_topRightBuf );
    GFL_DELETE( loader );
  }
  else
  {
    GFL_PRINT( "use JpegLoader()\n" );
    imgdb::JpegLoader *loader = GFL_NEW( m_heap ) imgdb::JpegLoader();
    m_resultDetail = LoadScreenshotFromSDCard( loader, filePath, m_topBuf );
    GFL_DELETE( loader );
  }
  CopyMemory2StereoView( m_topBuf,      pImageTop,      width, height, m_targetWidth, m_targetHeight );
  CopyMemory2StereoView( m_topRightBuf, pImageTopRight, width, height, m_targetWidth, m_targetHeight );

  return E_RET_SUCCESS;
}

//-------------------------------------------------------------------------------
/**
 * @brief 画面のPixelイメージを出力用バッファへコピー
 * @param srcBuf       転送元用PiexelImage
 * @param dstBuf       転送先用PiexelImage
 * @param srcWidth     入力PiexlImageの横幅(2のべき乗であること）
 * @param srcHeight    入力PiexlImageの縦幅(2のべき乗であること）
 * @param dstWidth     出力先イメージバッファの横幅
 * @param dstHeight    出力先イメージバッファの縦幅
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::CopyStereoView2Memory( void* srcBuf, void* dstBuf, u32 srcWidth, u32 srcHeight, u32 dstWidth, u32 dstHeight )
{
  if( srcBuf != NULL )
  {
    u8* base    = static_cast<u8*>(srcBuf);
    u8* copy    = static_cast<u8*>(dstBuf);
    u32 offsetX = srcWidth-dstWidth;

    for( int x=0;x<dstWidth;x++)
    {
      u32 baseAdr = (x+offsetX)*srcHeight;
      for( int y=0;y<dstHeight;y++)
      {
        const u32 copyAdr = (dstWidth*dstHeight)-((dstWidth-x)+y*dstWidth);
        gfl::std::MemCopy( &base[(baseAdr<<2)], &copy[(copyAdr<<2)], 4 );
        baseAdr++;
      }
    }
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief イメージバッファから画面のPixelイメージへコピー
 * @param srcBuf       転送元用PiexelImage
 * @param dstBuf       転送先用PiexelImage
 * @param srcWidth     入力PiexlImageの横幅(512を推奨）
 * @param srcHeight    入力PiexlImageの縦幅(256を推奨）
 * @param dstWidth     出力先イメージバッファの横幅
 * @param dstHeight    出力先イメージバッファの縦幅
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::CopyMemory2StereoView( void* srcBuf, void* dstBuf, u32 srcWidth, u32 srcHeight, u32 dstWidth, u32 dstHeight )
{
  if( dstBuf != NULL )
  {
    u8 *base    = static_cast<u8*>(srcBuf);
    u8 *copy    = static_cast<u8*>(dstBuf);

    for( int y=0;y<dstHeight;y++)
    {
      u32 copyAdr = (dstHeight-(y+1))*srcWidth;
      for( int x=0;x<dstWidth;x++)
      {
        const u32 baseAdr = (dstWidth*dstHeight)-((dstWidth-x)+y*dstWidth);
        gfl::std::MemCopy( &base[(baseAdr<<2)], &copy[(copyAdr<<2)], 4 );
        copyAdr++;
      }
    }
  }
}

//-------------------------------------------------------------------------------
 /**
  * @brief  SDカードからの読み込み処理(立体視用）
  * @param  loader         ロードクラス
  * @param  filePath       読み込み対象ファイルパス
  * @param  memNormalBuf   データ格納先バッファ
  * @param  memRightBuf    立体視用右画面Pixelデータ格納先バッファ。NULLを指定することも可能。
  * @return imgdb::Result  エラーコード
  */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::LoadScreenshotFromSDCard( imgdb::MpLoader* loader, const wchar_t* filePath, void* memNormalBuf, void* memRightBuf )
{
  loader->SetFilePath( filePath );
  loader->SetFormat( m_format );
  loader->SetBuffer(memNormalBuf,memRightBuf, GetBufferSize() );

  imgdb::Result ret = loader->Load();

  #if GFL_DEBUG
  if( ret == imgdb::ResultSuccess ){
    GFL_PRINT( "LoadFromSDCard() imgdb::MpLoader : success[%d][%d]\n", loader->GetRawWidth(), loader->GetRawHeight() );
  }
  else{
    GFL_PRINT( "LoadFromSDCard() imgdb::MpLoader : faild\n" );
  }
  #endif

  return ret;
}

//-------------------------------------------------------------------------------
 /**
  * @brief  SDカードからの読み込み処理(非立体視用）
  * @param  loader         ロードクラス
  * @param  filePath       読み込み対象ファイルパス
  * @param  memNormalBuf   データ格納先バッファ
  * @return imgdb::Result  エラーコード
  */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::LoadScreenshotFromSDCard( imgdb::JpegLoader* loader, const wchar_t* filePath, void* memNormalBuf )
{
  loader->SetFilePath( filePath );
  loader->SetFormat( m_format );

  GFL_PRINT( "BufferSize[%u][%u]\n", GetBufferSize(), loader->GetBufferSize() );

  loader->SetBuffer(memNormalBuf, GetBufferSize() );

  imgdb::Result ret = loader->Load();

  #if GFL_DEBUG
  if( ret == imgdb::ResultSuccess ){
    GFL_PRINT( "LoadFromSDCard() imgdb::JpegLoader : success[%d][%d]\n", loader->GetRawWidth(), loader->GetRawHeight() );
  }
  else{
    GFL_PRINT( "LoadFromSDCard() imgdb::JpegLoader : faild\n" );
  }
  #endif

  return ret;
}

//-------------------------------------------------------------------------------
 /**
  * @brief SDカード保存処理(立体視用）
  * @param  saver          セーブクラス
  * @param  memNormalBuf   画像データバッファ(ARGBフォーマット）
  * @param  memRightBuf    右画像データバッファ(ARGBフォーマット）
  * @param  pUserMakerNote ユーザーノート文字列(NULL指定時は未設定として扱う)
  * @param  noteSize       ユーザーノート文字列サイズ（０指定時は未設定として扱う)
  * @return imgdb::Result  エラーコード
  */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::SaveScreenshotToSDCard( imgdb::MpSaver* saver, const void* memNormalBuf, const void* memRightBuf, const void* pUserMakerNote, u32 noteSize )
{
  saver->SetRaw(memNormalBuf,memRightBuf, m_targetWidth, m_targetHeight, m_format);
  saver->SetTitleUniqueId( m_appUniqueId );
  saver->SetUserMakerNote(pUserMakerNote, noteSize );
  saver->SetScreenshotFlag( true );

  imgdb::Result ret = saver->Save();

  if( ret == imgdb::ResultSuccess ){
    ::std::wcscpy( m_filePath, saver->GetSavedFilePath() );
    GFL_PRINT( "SaveToSDCard() imgdb::MpSaver : success[%ls]\n", m_filePath );
  }
  else{
    GFL_PRINT( "SaveToSDCard() imgdb::MpSaver : faild\n" );
    m_filePath[0] = '\0';
  }
  return ret;
}

//-------------------------------------------------------------------------------
 /**
  * @brief SDカード保存処理(非立体視用）
  * @param  saver          セーブクラス
  * @param  memNormalBuf   画像データバッファ(ARGBフォーマット）
  * @param  pUserMakerNote ユーザーノート文字列(NULL指定時は未設定として扱う)
  * @param  noteSize       ユーザーノート文字列サイズ（０指定時は未設定として扱う)
  * @return imgdb::Result  エラーコード
  */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::SaveScreenshotToSDCard( imgdb::JpegSaver* saver, const void* memNormalBuf, const void* pUserMakerNote, u32 noteSize )
{
  saver->SetRaw( memNormalBuf, m_targetWidth, m_targetHeight, m_format);
  saver->SetTitleUniqueId( m_appUniqueId );
  saver->SetUserMakerNote(pUserMakerNote, noteSize );
  saver->SetScreenshotFlag( true );

  imgdb::Result ret = saver->Save();

  if( ret == imgdb::ResultSuccess )
  {
    ::std::wcscpy( m_filePath, saver->GetSavedFilePath() );
    GFL_PRINT( "SaveToSDCard() imgdb::JpegSaver : success[%ls]\n", m_filePath );
  }
  else{
    GFL_PRINT( "SaveToSDCard() imgdb::JpegSaver : faild\n" );
    m_filePath[0] = '\0';
  }
  return ret;
}

//-------------------------------------------------------------------------------
/**
 * @brief  SaveToSDCard()実行結果の詳細なエラーコードを取得する
 * @return エラー列挙定数
 */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::GetResultDetail( void )
{
  return m_resultDetail;
}

//-------------------------------------------------------------------------------
/**
 * @brief  SaveToSDCard()でSDカードに保存された写真ファイルの絶対パスを取得する
 *         未保存、または保存失敗時は、空文字が返却される。
 * @return 写真ファイルのファイル名（絶対パス）
 */
 //-------------------------------------------------------------------------------
const wchar_t * ImageDBAccessor::GetSavedFilePath() const
{
  return m_filePath;
}

//-------------------------------------------------------------------------------
/**
 * @brief  読み込んだPiexelデータを格納する為に必要なバッファのサイズを取得する。
 * @return 読み込みに必要となるバッファサイズ（立体視時は１画面分のバッファサイズ）。
 */
 //-------------------------------------------------------------------------------
size_t ImageDBAccessor::GetBufferSize() const
{
  return imgdb::JpegMpBaseLoader::ComputeImageSize(m_targetWidth, m_targetHeight, m_format);
}


//-------------------------------------------------------------------------------
/**
 * @brief ImageDbが利用するSDカードアーカイブがマウントされているかチェック
 *
 * @return SDカードがマウントされているならtrueを返却
 */
//-------------------------------------------------------------------------------
bool ImageDBAccessor::IsMountSdmc( void ) const
{
  return imgdb::mnt::IsMountSdmc();
}


//-------------------------------------------------------------------------------
/**
 * @brief ImageDbが利用するSDカードアーカイブをマウントする
 *
 * @return SDカードのマウントに成功したならtrueを返却
 */
//-------------------------------------------------------------------------------
bool ImageDBAccessor::MountSdmc( void )
{
  return imgdb::mnt::MountSdmc();
}


//-------------------------------------------------------------------------------
/**
 * @brief ImageDbが利用するSDカードアーカイブをアンマウントする
 *
 * @note  SDカードが抜かれたらアプリ側で本関数を呼び出してアンマウントしてください
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::UnmountSdmc( void )
{
  imgdb::mnt::UnmountSdmc();
}


//-------------------------------------------------------------------------------
/**
 * @brief  管理されているSDカード内の写真枚数を取得する
 *
 * @return 写真枚数。SDカードがマウントされていないなら負の値を返す
 */
 //-------------------------------------------------------------------------------
int ImageDBAccessor::GetRegisteredPictureNum( void ) const
{
  if(!IsMountSdmc()){
    return -1;
  }
  return imgdb::util::GetRegisteredPictureNum();
}


//-------------------------------------------------------------------------------
/**
 * @brief  SDカードヘ保存可能な写真枚数を見積もる
 *
 * @param  pResult 撮影可能ならResultSuccess()が、撮影不可能ならそれ以外を格納
 *
 * @note   pResultに格納される値は、以下のとおり。
 * @note   ・ResultErrorOverNumImages
 * @note   ・ResultErrorFileOperationFailure
 * @note   ・ResultErrorNotExistStorage
 * @note   ・ResultErrorWriteProtect
 * @note   ・ResultErrorInsufficientStorageSpace
 * @note   ・ResultErrorReadOnlyDbFile
 * @note   ・ResultErrorNotAvailableStorage
 *
 * @return 写真の残り撮影枚数
 */
 //-------------------------------------------------------------------------------
int ImageDBAccessor::EstimateRemainPictureNum( imgdb::Result* pResult )
{
  return imgdb::util::EstimateRemainPictureNum(pResult);
}


//-------------------------------------------------------------------------------
/**
 * @brief  SDカード内の写真を削除する
 * @param  pPath 削除対象となる写真へのファイルパス
 * @return エラー列挙定数
 */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::DeletePicture( const wchar_t* pPath )
{
  return imgdb::util::DeletePicture( pPath, m_appUniqueId );
}




} // namespace imagedb
} // namespace gfl

#endif

