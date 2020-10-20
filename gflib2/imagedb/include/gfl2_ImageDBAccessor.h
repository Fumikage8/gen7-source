//================================================================================
/**
 * @file   gfl2_ImageDBAccessor.h
 * @brief  ImageDBを利用したSDカードへの写真入出力用クラス
 * @author hisanao suzuki
 * @date   2012.2.13
 * @date   2015.11.30 niji用に移植 takahashi tomoya 
 */
//================================================================================
#ifndef __GFL2_IMAGE_DB_ACCESSOR_H__
#define __GFL2_IMAGE_DB_ACCESSOR_H__
#pragma once

#include <imagedb/include/gfl2_ImageDBTypes.h>


#if GFL2_ENABLE_IMAGE_DB

namespace gfl2 {
namespace imagedb {

class MyAllocator;

//-------------------------------------------------------------------------------
/**
* @brief ImageDBアクセス用クラス
*/
//-------------------------------------------------------------------------------
class ImageDBAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( ImageDBAccessor );  // コピーを禁止

public:
  static const u32 IMGDB_HEAP_SIZE = 0x130000;    //!< ImageDbライブラリに割り当てる推奨ヒープメモリ量
  static const u32 REGISTERD_IMAGE_MAX = 3000;    //!< ImageDbが扱える最大写真枚数

  /**
   * @breif リザルトコード
   */
  typedef enum
  {
     E_RET_SUCCESS,         //!< 成功
     E_RET_OUT_OF_MEMORY,   //!< メモリ不足
     E_RET_SDCARD_NOEXIST,  //!< SDカード未挿入
     E_RET_SDCARD_READONLY, //!< SDカード書込み禁止
     E_RET_FAILED,          //!< 失敗。GetErrorDetail()で詳細なエラーを取得すること
  }ResultCode;

  /**
   *  @note imagedb::Resultを参照するソースは、CTRで囲ってください。
   */
#if defined(GF_PLATFORM_CTR)
  typedef imgdb::Result imgDbResult;
#elif defined(GF_PLATFORM_WIN32)
  typedef u32 imgDbResult;
#endif

public:
  /**
   * @brief  保存に使用されるデフォルト画像フォーマット
   */
#if defined(GF_PLATFORM_CTR)
  static const nn::jpeg::CTR::PixelFormat IMAGE_FORMAT = nn::jpeg::CTR::PIXEL_FORMAT_ABGR8;
#elif defined(GF_PLATFORM_WIN32)
  static const u32 IMAGE_FORMAT = 0;  // 無意味な値を入れる。
#endif

  static const int IMAGE_UPEER_WIDTH  = 400;        //!< 保存に使用される上画面用の画像横幅
  static const int IMAGE_UPEER_HEIGHT = 240;        //!< 保存に使用される上画面用の画像縦幅
  static const int IMAGE_LOWER_WIDTH  = 320;        //!< 保存に使用される下画面用の画像横幅
  static const int IMAGE_LOWER_HEIGHT = 240;        //!< 保存に使用される下画面用の画像縦幅
  static const int IMAGE_ALIGN_SIZE   = 4;          //!< イメージバッファ用メモリアラインサイズ
  static const int IMAGE_DEFAULT_JPEG_QUALITY = 80; //!< Jpeg保存時の品質デフォルト値

  /**
   *  @brief ファイルパスの最大サイズ
   */
#if defined(GF_PLATFORM_CTR)
  static const int FILEPATH_SIZE = imgdb::imgio::c_nMaxPathSize;
#elif defined(GF_PLATFORM_WIN32)
  static const int FILEPATH_SIZE = 128;
#endif

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param[in]  heap         ヒープ
   * @param[in]  deviceHeap   デバイスヒープ
   * @param[in]  appUniqueId  アプリケーション固有ID(20bit)
   */
  //-------------------------------------------------------------------------------
  ImageDBAccessor( gfl2::heap::HeapBase *heap, gfl2::heap::HeapBase *deviceHeap, u32 appUniqueId );

  //-------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-------------------------------------------------------------------------------
  virtual ~ImageDBAccessor();

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDbライブラリの初期化処理（開始時に１度だけコール）
   *
   * @param[in] useHeapSize  ImageDbライブラリに割り当てるヒープサイズ
   * @param[in] targetWidth  出力元ソースとなる画面の横幅
   * @param[in] targetHeight 出力元ソースとなる画面の縦幅
   * @param[in] isSimpleMode イメージユーティリティーを使用するためだけの初期化を行うならtrueを指定する
   */
  //-------------------------------------------------------------------------------
  ResultCode Initialize( u32 useHeapSize = IMGDB_HEAP_SIZE,
                         u32 targetWidth = IMAGE_UPEER_WIDTH, u32 targetHeight = IMAGE_UPEER_HEIGHT,
                         bool isSimpleMode=false );

  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDbライブラリの終了処理（終了時に１度だけコール）
   */
  //-------------------------------------------------------------------------------
  void Finalize( void );

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDbが利用するSDカードアーカイブがマウントされているかチェック
   *
   * @return SDカードがマウントされているならtrueを返却
   */
  //-------------------------------------------------------------------------------
  bool IsMountSdmc( void ) const;

  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDbが利用するSDカードアーカイブをマウントする
   *
   * @return SDカードのマウントに成功したならtrueを返却
   */
  //-------------------------------------------------------------------------------
  bool MountSdmc( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief ImageDbが利用するSDカードアーカイブをアンマウントする
   *
   * @note  SDカードが抜かれたらアプリ側で本関数を呼び出してアンマウントしてください
   */
  //-------------------------------------------------------------------------------
  void UnmountSdmc( void );

public:

  //-------------------------------------------------------------------------------
  /**
   * @brief  管理されているSDカード内の写真枚数を取得する
   *
   * @return 写真枚数。SDカードがマウントされていないなら負の値を返す
   */
   //-------------------------------------------------------------------------------
  int GetRegisteredPictureNum( void ) const;

  //-------------------------------------------------------------------------------
  /**
   * @brief  SDカードヘ保存可能な写真枚数を見積もる
   *
   * @param  pResult 撮影可能ならResultSuccess()が、撮影不可能ならそれ以外を格納
   *
   * @par   pResultに格納される値は、以下のとおり。
   *        - ResultErrorOverNumImages
   *        - ResultErrorFileOperationFailure
   *        - ResultErrorNotExistStorage
   *        - ResultErrorWriteProtect
   *        - ResultErrorInsufficientStorageSpace
   *        - ResultErrorReadOnlyDbFile
   *        - ResultErrorNotAvailableStorage
   *
   * @return 写真の残り撮影枚数
   */
   //-------------------------------------------------------------------------------
  int EstimateRemainPictureNum( imgDbResult* pResult );

  //-------------------------------------------------------------------------------
  /**
   * @brief  SDカード内の写真を削除する
   *
   * @param[in]  pPath 削除対象となる写真へのファイルパス
   *
   * @return エラー列挙定数
   */
   //-------------------------------------------------------------------------------
  imgDbResult DeletePicture( const wchar_t * pPath );


public:
  //-------------------------------------------------------------------------------
  /**
   * @brief SDカードへ写真を保存する。
   *        スクリーンショットのPixelイメージを渡すことで写真データとしてSDカードへ保存される。
   *
   * @attention  引数で引き渡すPixelデータはリニアイメージである必要がある。
   * @attention  ブロックイメージからリニアイメージへの変換はConvertImageBlock2Liner()を使用すること
   *
   * @param[in] pImageTop      画面Pixelデータ(RGBA)
   * @param[in] pImageTopRight 立体視用右画面Pixelデータ(RGBA)。NULLを指定することも可能。
   * @param[in] width          イメージデータの横幅(2のべき乗であること！）
   * @param[in] height         イメージデータの縦幅(2のべき乗であること！）
   * @param[in] pUserMakerNote ユーザーノート用文字列
   * @param[in] noteSize       ユーザーノート用文字列サイズ
   * @param[in] jpegQuality    Jpegの画像品質（1～100)
   *
   * @return リザルトコード
   */
  //-------------------------------------------------------------------------------
  ResultCode SaveToSDCard( const void* pImageTop, const void *pImageTopRight, u32 width=512, u32 height=256, const void* pUserMakerNote=NULL, u32 noteSize=0, int jpegQuality=IMAGE_DEFAULT_JPEG_QUALITY );

  //-------------------------------------------------------------------------------
  /**
   * @brief SDカードからの写真を読み込む。
   *        格納先のバッファサイズは、GetBufferSize()にて取得すること
   *
   * @attention  取得されるPixelデータはリニアイメージである。
   * @attention  ブロックイメージへの変換はConvertImageLiner2Block()を使用すること
   *
   * @param[in]  filePath       読み込み対象となる写真のファイルパス
   * @param[out] pImageTop      データ格納先バッファ
   * @param[out] pImageTopRight 立体視用右画面Pixelデータ格納先バッファ。NULLを指定することも可能。
   * @param[in]  width          格納先バッファの画像横幅(2のべき乗であること！）
   * @param[in]  height         格納先バッファの画像縦幅(2のべき乗であること！）
   * @param[in]  thumbnail      サムネイルフラグ(サムネイル使用時はtrue)
   *
   * @return リザルトコード
   */
  //-------------------------------------------------------------------------------
  ImageDBAccessor::ResultCode LoadFromSDCard( const wchar_t * filePath, void* pImageTop, void *pImageTopRight, u32 width, u32 height, bool thumbnail = false );

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief  SaveToSDCard()実行結果の詳細なエラーコードを取得する
   *
   * @return エラー列挙定数
   */
   //-------------------------------------------------------------------------------
  imgDbResult GetResultDetail( void );

  //-------------------------------------------------------------------------------
  /**
   * @brief  SaveToSDCard()でSDカードに保存された写真ファイルの絶対パスを取得する
   *
   * @retval 空文字     未保存、または保存失敗
   * @retval 空文字以外 写真ファイルのファイル名（絶対パス）
   */
   //-------------------------------------------------------------------------------
  const wchar_t * GetSavedFilePath() const;

  //-------------------------------------------------------------------------------
  /**
   * @brief  読み込んだPiexelデータを格納する為に必要なバッファのサイズを取得する
   *
   * @return 読み込みに必要となるバッファサイズ（立体視時は１画面分のバッファサイズ）
   */
   //-------------------------------------------------------------------------------
  size_t GetBufferSize() const;

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief ブロックイメージからリニアイメージへ変換する
   *
   * @note  本APIはGPUコマンドのため、実行完了に1フレームを要する
   *
   * @param[in]  pSrcImg   変換元となるブロックイメージ（デバイスメモリ必須！）
   * @param[out] pDstImg   リニアイメージ格納先（デバイスメモリ必須！）
   * @param[in]  width     イメージ横幅
   * @param[in]  height    イメージ縦幅
   * @param[in]  blockSize ブロックイメージのサイズ（8、32のいずれか）
   */
  //-------------------------------------------------------------------------------
  static void ConvertImageBlock2Liner( const void * pSrcImg, void* pDstImg, u32 width, u32 height, u32 blockSize );

  //-------------------------------------------------------------------------------
  /**
   * @brief リニアイメージからブロックイメージへ変換する
   *
   * @note  本APIはGPUコマンドのため、実行完了に1フレームを要する
   *
   * @param[in]  pSrcImg   変換元となるリニアイメージ（デバイスメモリ必須！）
   * @param[out] pDstImg   ブロックイメージ格納先（デバイスメモリ必須！）
   * @param[in]  width     イメージ横幅
   * @param[in]  height    イメージ縦幅
   * @param[in]  blockSize ブロックイメージのサイズ（8、32のいずれか）
   */
  //-------------------------------------------------------------------------------
  static void ConvertImageLiner2Block( const void * pSrcImg, void* pDstImg, u32 width, u32 height, u32 blockSize );

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief   読み込んだ画像の横幅を取得
   */
  //-------------------------------------------------------------------------------
  u32 GetLoadImageWidth( void ) const { return m_loadImageWidth; }

  //-------------------------------------------------------------------------------
  /**
   * @brief   読み込んだ画像の縦幅を取得
   */
  //-------------------------------------------------------------------------------
  u32 GetLoadImageHeight( void ) const { return m_loadImageHeight; }

public:
  //-------------------------------------------------------------------------------
  /**
   * @brief   タイトルユニークIDの一致チェック
   *
   * @param[in]   filepath    比較する写真のパス
   *
   * @retval  "true"なら一致
   * @retval  "false"なら不一致または写真が読み込めないなどの理由で確認出来ない時
   *
   * @fix MMCat[641]
   */
  //-------------------------------------------------------------------------------
  bool IsSameTitleUniqueID( const wchar_t* filepath );

private:
  void                      *m_imgDbWork;                               //!< ImageDbライブラリ用ヒープメモリ
  MyAllocator               *m_tmpAllocator;                            //!< 独自アロケータ
  gfl2::heap::HeapBase       *m_heap;                                    //!< ヒープ
  gfl2::heap::HeapBase       *m_deviceHeap;                              //!< デバイスヒープ

#if defined(GF_PLATFORM_CTR)
  nn::jpeg::CTR::PixelFormat m_format;                                  //!< 出力画像のフォーマット
#endif

  void                      *m_topBuf;                                  //!< 上部画面用バッファ
  void                      *m_topRightBuf;                             //!< 上部画面立体視用バッファ
  u32                        m_targetWidth;                             //!< 出力画像の横幅
  u32                        m_targetHeight;                            //!< 出力画像の縦幅
  u32                        m_appUniqueId;                             //!< アプリケーション固有ID(20Bit)

  bool                       m_init;                                    //!< 初期化フラグ
  imgDbResult                m_resultDetail;                            //!< SaveToSDCard()実行結果
  wchar_t                    m_filePath[FILEPATH_SIZE];                 //!< 保存されたファイル名

  u32                        m_loadImageWidth;                          //!< ロードした画像の横幅
  u32                        m_loadImageHeight;                         //!< ロードした画像の縦幅


private:
  ResultCode    AllocateImageBuffer( void );
  void          FreeImageBuffer( void );
  void          CopyStereoView2Memory( void* srcBuf, void* dstBuf, u32 srcWidth, u32 srcHeight, u32 dstWidth, u32 dstHeight );
  void          CopyMemory2StereoView( void* srcBuf, void* dstBuf, u32 srcWidth, u32 srcHeight, u32 dstWidth, u32 dstHeight );

#if defined(GF_PLATFORM_CTR)
  imgdb::Result SaveScreenshotToSDCard( imgdb::MpSaver* saver,   const void* memNormalBuf, const void* memRightBuf, const void* pUserMakerNote, u32 noteSize );
  imgdb::Result SaveScreenshotToSDCard( imgdb::JpegSaver* saver, const void* memNormalBuf, const void* pUserMakerNote, u32 noteSize );
  imgdb::Result LoadScreenshotFromSDCard( imgdb::MpLoader* loader, const wchar_t* filePath, void* memNormalBuf, void* memRightBuf );
  imgdb::Result LoadScreenshotFromSDCard( imgdb::JpegLoader* loader, const wchar_t* filePath, void* memNormalBuf, bool thumbnail );
#endif
};

} // namespace imagedb
} // namespace gfl2


#endif
#endif

