

#if defined(GF_PLATFORM_CTR)

//==============================================================================
/**
 * @file   util_QRDecoder.cpp
 * @date   20120907 Fri.
 * @date   2016/01/18 Mon. 17:43:17
 * @author muto_yu
 * @brief  QR読み込み
 * @note   過去のプロジェクトより移植
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


/*	********************************************************************
    include
  *******************************************************************/
#include  "./util_QRDecoder.h"


namespace NetApp    {
namespace QRReader  {
namespace QRUtil    {
/* ********************************************************************
    define
  *******************************************************************/
namespace
{
  static const u32    ThreadPriority  = NN_OS_DEFAULT_THREAD_PRIORITY + 1;    /**<  デコードスレッドプライオリティ  */
  static const u32    StackSize       = 0x1000;                               /**<  デコードスレッドスタックサイズ  */
}


//==============================================================================
/**
  *  @class  
  *  @brief  QRデコードスレッド
  *  @date   2016/01/18 Mon. 18:41:30
  */
//==============================================================================
class QRDecodeThreadProcedure
  : public  gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(QRDecodeThreadProcedure);

public:
  /** ctor  */
  QRDecodeThreadProcedure(gfl2::heap::HeapBase* pHeapBase, const u16 decodeImageMaxW, const u16 decodeImageMaxH);
  virtual ~QRDecodeThreadProcedure();

  /** デコード要求  */
  bool                        DecodeRequest(
                                const void* pQRImage,
                                const u16   pImageSize[2],
                                const s32   pTargetImageRect[4] = NULL,
                                const bool  isNoWait            = false
                              );
  mw::qrd::Result             GetResult(void);
  const mw::qrd::DecodeData&  GetDecodeData(void);

  bool                        IsDecodeEnd(void)       {return mEvent_DecodeEnd.TryWait();}
  void                        WaitForDecodeEnd(void)  {mEvent_DecodeEnd.Wait();}

  void                        KillRequest(void);


protected:
private:
  void  WaitForRequest(void)  {mEvent_Request.Wait(); mEvent_Request.Clear();}

  /*  implement gfl2::thread::ThreadInterface  */
  virtual void  Function(void);

  gfl2::thread::Event       mEvent_Request;
  gfl2::thread::Event       mEvent_DecodeEnd;
  gfl2::thread::Event       mEvent_KillRequest;

  mw::qrd::QRDecoder        mMWQRDecoder;                   /**<  QRデコーダ                */
  mw::qrd::ImageInfo        mMWQRImageInfo;                 /**<  QR画像情報                */
  mw::qrd::DecodeData       mMWQRDecodeData;                /**<  デコードデータ            */
  mw::qrd::Result           mMWQRResult;                    /**<  処理結果                  */
  void*                     mpDecoderWork;                  /**<  デコーダ用ワークバッファ  */
  u16                       mDocodeImageSizeMax[2];         /**<  入力画像サイズ最大(w/h)   */

  inline void   WaitForThreadPause(void);                   /**<  スレッドがpause状態になるまで待つ  */
};


/* ********************************************************************
  global
    *******************************************************************/


/* ********************************************************************
    static
  *******************************************************************/


/* ********************************************************************
    prototype
  *******************************************************************/


/* ********************************************************************
    global function
  *******************************************************************/


/*-----------------------------------------------------------------*//**
  ctor
*//*------------------------------------------------------------------*/
QRDecoder::QRDecoder(void)
  : mpDecodeThread(NULL)
  , mpDecodeThreadProcedure(NULL)
  , mDecodeDataCache()
  , mState(STATE_INITIAL)
  , mLastResult(RESULT_NODATA)
{
}


/*-----------------------------------------------------------------*//**
  dtor
*//*------------------------------------------------------------------*/
QRDecoder::~QRDecoder() {
  Cleanup();
}


/*-----------------------------------------------------------------*//**
  使用準備
*//*------------------------------------------------------------------*/
void  QRDecoder::Setup(const Config& rConfig) {
  GFL_ASSERT_STOP(!mpDecodeThread);             /*  未Setupであること  */
  GFL_ASSERT_STOP(!mpDecodeThreadProcedure);    /*  未Setupであること  */

  gfl2::heap::HeapBase* pHeapBase = rConfig.pHeapBase;


  mpDecodeThreadProcedure = GFL_NEW(pHeapBase) QRDecodeThreadProcedure(pHeapBase, rConfig.decodeImageSizeMax[0], rConfig.decodeImageSizeMax[1]);

  mpDecodeThread          = GFL_NEW(pHeapBase) gfl2::thread::Thread(mpDecodeThreadProcedure, pHeapBase, true, StackSize);
  mpDecodeThread->Start(ThreadPriority);
}


/*-----------------------------------------------------------------*//**
  使用終了
*//*------------------------------------------------------------------*/
void  QRDecoder::Cleanup(void)  {
  /*  join  */
  if(mpDecodeThread)
  {
    mpDecodeThreadProcedure->KillRequest(); /*  リクエスト待ちを解除し、スレッド関数から抜ける  */
    mpDecodeThread->Kill();
    mpDecodeThread->Wait();
  }
  GFL_SAFE_DELETE(mpDecodeThread);
  GFL_SAFE_DELETE(mpDecodeThreadProcedure);
}


/*-----------------------------------------------------------------*//**
  状態の更新
  @note   デコード結果は本関数内で更新されます
*//*------------------------------------------------------------------*/
QRDecoder::State QRDecoder::Update(void) {
  switch(mState)  {
    case  STATE_INITIAL:
      mState  = STATE_IDLE;
      break;

    case  STATE_IDLE:
      break;

    case  STATE_PROCESSING:
      if(mpDecodeThreadProcedure->IsDecodeEnd())  {
        if(this->DecodeResultHandler() == RESULT_SUCCESS) {
          memcpy(&mDecodeDataCache, &mpDecodeThreadProcedure->GetDecodeData(), sizeof(mw::qrd::DecodeData));
#if 0
  {
    const u8*  pdata = reinterpret_cast<const u8*>(mDecodeDataCache.data);

    GFL_PRINT("[%s] dump :\n  ", __FUNCTION__);
    for(u32 num = 0; num < 16; ++num) {
      GFL_PRINT("%02x", pdata[num]);
      if(num && !(num%4))   GFL_PRINT(" ");
    }
    GFL_PRINT("\n");
  }
#endif
        }
        mState  = STATE_IDLE;
      }
      break;

    default:
      GFL_ASSERT(0);      /*  invalid state  */
      break;
  }

  return mState;
}



/*-----------------------------------------------------------------*//**
  デコード開始要求
*//*------------------------------------------------------------------*/
bool  QRDecoder::StartDecode(
  const void* pQRImage,
  const u16   pImageSize[2],
  const s32   pTargetImageRect[4],
  const bool  isNoWait
)  {
  bool  isRequested = mpDecodeThreadProcedure->DecodeRequest(pQRImage, pImageSize, pTargetImageRect, isNoWait);

  if(isRequested) {
    mState  = STATE_PROCESSING;
  }

  return  isRequested;
}


/*-----------------------------------------------------------------*//**
  結果の取得
  @caution  IsSuccessがfalseの場合の内容は未定義
  @caution  データが文字列の場合、QRコードの仕様上ではNULターミネートは保障されないため注意
  @note       分割QRは本関数では扱わないので、GetDecodeDataRawを使用してアプリケーション側で対応
  @param  pDataSize   [out]   データサイズ格納先（不要の場合はNULL）
  @return デコードデータ
*//*------------------------------------------------------------------*/
const void*   QRDecoder::GetDecodeData(u32* pDataSize) const  {
  const void*                 pData     = NULL;
  const mw::qrd::DecodeData*  pRawData  = this->GetDecodeDataRaw();

  pData = pRawData->data;
  if(pDataSize)
    *pDataSize  = pRawData->size;

  return pData;
}

/*	********************************************************************
    local function
  *******************************************************************/


/*-----------------------------------------------------------------*//**
*//*------------------------------------------------------------------*/
QRDecoder::Result  QRDecoder::DecodeResultHandler(void)  {
  switch(mpDecodeThreadProcedure->GetResult()) {
    case  mw::qrd::RESULT_SUCCESS:                   /*  処理成功  */
      mLastResult = RESULT_SUCCESS;
      break;

    case  mw::qrd::RESULT_ILLEGAL_PARAMETER:         /*  不正なパラメータ  */
      mLastResult = RESULT_ERROR;
      GFL_PRINT("%s : RESULT_ILLEGAL_PARAMETER : 不正なパラメータ\n", __FUNCTION__);
      break;

    case  mw::qrd::RESULT_NODATA:                    /*  QRコード認識失敗  */
      mLastResult = RESULT_NODATA;
      break;

    case  mw::qrd::RESULT_NOMEMORY:                  /*  作業用バッファが不足  */
      mLastResult = RESULT_ERROR;
      GFL_PRINT("%s : RESULT_NOMEMORY : 作業用バッファが不足\n", __FUNCTION__);
      break;

    case  mw::qrd::RESULT_VERSION:                   /*  QRコードのバージョンサポート対象外  */
      mLastResult = RESULT_ERROR;
      GFL_PRINT("%s : RESULT_VERSION : QRコードのバージョンサポート対象外\n", __FUNCTION__);
      break;

    case  mw::qrd::RESULT_ILLEGAL_IMAGESIZE:         /*  イメージサイズが不正  */
      mLastResult = RESULT_ERROR;
      GFL_PRINT("%s : RESULT_ILLEGAL_IMAGESIZE : イメージサイズが不正\n", __FUNCTION__);
      break;

    case  mw::qrd::RESULT_NOT_INITIALIZED:           /*  ライブラリが初期化されていない  */
      mLastResult = RESULT_ERROR;
      GFL_PRINT("%s : RESULT_NOT_INITIALIZED : ライブラリが初期化されていない\n", __FUNCTION__);
      break;

    default:
      GFL_PRINT("%s : illegal result (%d)\n", __FUNCTION__, (s32)mpDecodeThreadProcedure->GetResult());
      GFL_ASSERT(0);    /*  illigal result  */
      break;
  }

  return mLastResult;
}


/*-----------------------------------------------------------------*//**
  ctor
*//*------------------------------------------------------------------*/
QRDecodeThreadProcedure::QRDecodeThreadProcedure(gfl2::heap::HeapBase* pHeapBase, const u16 decodeImageMaxW, const u16 decodeImageMaxH)
  : mEvent_Request(pHeapBase)
  , mEvent_DecodeEnd(pHeapBase)
  , mEvent_KillRequest(pHeapBase)
  , mMWQRDecoder()
  , mMWQRImageInfo()
  , mMWQRDecodeData()
  , mMWQRResult(mw::qrd::RESULT_NODATA)
  , mpDecoderWork(NULL)
{
  GFL_ASSERT(pHeapBase);
  GFL_ASSERT(decodeImageMaxW <= 640);
  GFL_ASSERT(decodeImageMaxH <= 480);

  /*  event  */
  {
    mEvent_Request.Initialize();
    mEvent_DecodeEnd.Initialize();
    mEvent_KillRequest.Initialize();

    mEvent_DecodeEnd.Signal();
  }

  /*  qr  */
  {
    const u32   workSize  = mw::qrd::QRDecoder::GetDecodeBufferSize(decodeImageMaxW, decodeImageMaxH);

    /*
      QRコードのデコード処理で使用する作業用バッファです。
      4バイトアライメントでなければいけません。
    */
    mpDecoderWork = GflHeapAllocMemoryAlign(pHeapBase, workSize, 4);
    mMWQRDecoder.Initialize(mpDecoderWork, workSize);
  }
  mMWQRImageInfo.width              = 0;   /*  入力画像            */
  mMWQRImageInfo.height             = 0;
  mMWQRImageInfo.yuv422Data         = NULL;
  mMWQRImageInfo.qrPosition.top     = 
  mMWQRImageInfo.qrPosition.left    = 
  mMWQRImageInfo.qrPosition.right   = 
  mMWQRImageInfo.qrPosition.bottom  = 0;   /*  all0でイメージ全体  */

  mDocodeImageSizeMax[0] = decodeImageMaxW;
  mDocodeImageSizeMax[1] = decodeImageMaxH;
}


/*-----------------------------------------------------------------*//**
  dtor
*//*------------------------------------------------------------------*/
QRDecodeThreadProcedure::~QRDecodeThreadProcedure()
{
  mMWQRDecoder.Finalize();
  GflHeapFreeMemory(mpDecoderWork);
  mpDecoderWork = NULL;

  /*  event  */
  {
    mEvent_KillRequest.Finalize();
    mEvent_DecodeEnd.Finalize();
    mEvent_Request.Finalize();
  }
}


/*-----------------------------------------------------------------*//**
  デコード要求（メインスレッドからのコール）
  @param  [in]  pQRImage      デコード対象QR画像
  @param  [in]  pImageSize[2] 入力画像サイズ（w, h）
  @param  [in]  pImageRect[4] デコード対象領域(top, left, right, bottom)
  @param  [in]  isImmediate   false : 処理終了までブロックする / true : しない
  @return デコード要求が発行されたか（isImmediate == false 時は常にtrue）
*//*------------------------------------------------------------------*/
bool  QRDecodeThreadProcedure::DecodeRequest(
  const void* pQRImage,
  const u16   pImageSize[2],
  const s32   pTargetImageRect[4],
  const bool  isImmediate
) {
  GFL_ASSERT_STOP(pImageSize);

  bool  isEnableRequest = false;    /*  リクエスト可能か    */
  bool  isRequested     = false;    /*  リクエストをしたか  */

  if(isImmediate)  {
    /*  即時復帰  */
    isEnableRequest = IsDecodeEnd();
  }else {
    /*  処理終了までブロック  */
    WaitForDecodeEnd();
    isEnableRequest = true;
  }

  /*  リクエスト可能な状態であればデコード開始 */
  if(isEnableRequest)  {
    /*  入力画像サイズ  */
    mMWQRImageInfo.width  = pImageSize[0];
    mMWQRImageInfo.height = pImageSize[1];

    /*  対象矩形  */
    if(pTargetImageRect)  {
      mMWQRImageInfo.qrPosition.top     = pTargetImageRect[0];
      mMWQRImageInfo.qrPosition.left    = pTargetImageRect[1];
      mMWQRImageInfo.qrPosition.right   = pTargetImageRect[2];
      mMWQRImageInfo.qrPosition.bottom  = pTargetImageRect[3];
    }else
    {
      mMWQRImageInfo.qrPosition.top     = 
      mMWQRImageInfo.qrPosition.left    = 
      mMWQRImageInfo.qrPosition.right   = 
      mMWQRImageInfo.qrPosition.bottom  = 0;    /*  入力画像全体  */
    }

    mMWQRImageInfo.yuv422Data = reinterpret_cast<u32*>(const_cast<void*>(pQRImage));
    mEvent_Request.Signal();
    isRequested = true;
  }

  return isRequested;
}


/*-----------------------------------------------------------------*//**
  デコード結果を取得
  @note   デコード処理中にコールした場合、デコード終了までブロックします
*//*------------------------------------------------------------------*/
mw::qrd::Result   QRDecodeThreadProcedure::GetResult(void) {
  WaitForDecodeEnd();
  return mMWQRResult;
}


/*-----------------------------------------------------------------*//**
  デコードデータ取得
  @note   デコード処理中にコールした場合、デコード終了までブロックします
*//*------------------------------------------------------------------*/
const mw::qrd::DecodeData&   QRDecodeThreadProcedure::GetDecodeData(void) {
  WaitForDecodeEnd();
  return mMWQRDecodeData;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  QRDecodeThreadProcedure::KillRequest(void)
{
  mEvent_KillRequest.Signal();  /*  kill要求            */
  mEvent_Request.Signal();      /*  リクエスト待ち解除  */
}


/*-----------------------------------------------------------------*//**
  デコードスレッド処理
  @note スレッドからコールされる
*//*------------------------------------------------------------------*/
void  QRDecodeThreadProcedure::Function(void)  {
  
  WaitForRequest(); /*  リクエスト待ち*/

  /*  デコード処理開始  */
  mEvent_DecodeEnd.Clear();

  if(!mEvent_KillRequest.TryWait())   /*  kill要求がある場合は処理せず抜ける  */
  {
    if(mMWQRImageInfo.yuv422Data)
    {
      mMWQRResult  = mMWQRDecoder.ExecuteDecode(&mMWQRDecodeData, &mMWQRImageInfo);
    }
  }

  /*  デコード処理終了  */
  mEvent_DecodeEnd.Signal();
}





} /*  namespace QRUtil    */
} /*  namespace QRReader  */
} /*  namespace NetApp    */

#endif  /*  #if defined(GF_PLATFORM_CTR)  */

