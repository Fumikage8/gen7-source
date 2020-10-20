
#if defined(GF_PLATFORM_CTR)

#if !defined(UTIL_QRDECODER_H_INCLUDED)
#define UTIL_QRDECODER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   util_QRDecoder.h
 * @date   20120907 Fri.
 * @date   2016/01/18 Mon. 17:45:33
 * @author muto_yu
 * @brief   QRデコーダ
 *          DeviceCameraの撮影画像（YUV422）の全部もしくは一部からQRコードを探索し、デコードします。
 *          デコード処理はメインスレッドより優先の低いデコードスレッドで行われます。
 *          スレッドは処理完了に毎回Pause状態に入り、デコードリクエストにより起床します。
 * @note   過去Projectより移植
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


/*  ********************************************************************
    include
  *******************************************************************/
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>
//#include  <fs/include/gfl2_Fs.h>
//#include  <str/include/gfl2_Str.h>
#include  <debug/include/gfl2_Assert.h>
#include  <debug/include/gfl2_DebugPrint.h>
#include  <thread/include/gfl2_Thread.h>

#include  <mw/qrd/qrd.h>


namespace NetApp    {
namespace QRReader  {
namespace QRUtil    {
  class QRDecodeThreadProcedure;
/*  ********************************************************************
    define
  *******************************************************************/
/**
  QRデコーダ
*/
class QRDecoder {
public:
  /** 状態  */
  enum  State {
    STATE_INITIAL,      /**<  初期状態        */
    STATE_IDLE,         /**<  データ待機中    */
    STATE_PROCESSING,   /**<  デコード処理中  */

    STATE_MAX
  };


  /** 結果  */
  enum  Result  {
    RESULT_SUCCESS,     /**<  成功              */
    RESULT_ERROR,       /**<  エラー            */
    RESULT_NODATA,      /**<  QRコード認識失敗  */

    RESULT_MAX
  };


  /** setupパラメータ  */
  typedef struct  Config {
    gfl2::heap::HeapBase* pHeapBase;                /**<  ヒープ      */
    u16                   decodeImageSizeMax[2];    /**<  入力画像（デコード範囲ではない）サイズ最大    */

    /** ctor  */
    Config(void)
      : pHeapBase(NULL)
    {
      decodeImageSizeMax[0]  =
      decodeImageSizeMax[1]  = 0;
    }
  }Config;


  QRDecoder(void);                            /**<  ctor        */
  virtual ~QRDecoder();                       /**<  dtor        */

  void  Setup(const Config& rConfig);         /**<  使用準備    */
  void  Cleanup(void);                        /**<  使用終了    */

  /*-----------------------------------------------------------------*//**
    状態の更新
    @note   デコード結果は本関数内で更新されます
  *//*------------------------------------------------------------------*/
  State Update(void);


  /*-----------------------------------------------------------------*//**
    デコード開始要求（非同期のため、IsEnd()で終了を検知）
    @param    [in]  pQRImage          QRデコード画像
    @param    [in]  pImageSize        QRデコード画像のPixelサイズ（w, h）
    @param    [in]  pTargetImageRect  QRデコード画像内の処理対象矩形（top, left, right, bottom）、NULLで全体を対象とする
    @param    [in]  isImmediate       デコード処理中だった場合に内部で完了を待たず、即時終了する
    @retval   true  : 開始に成功
    @retval   false : 開始に失敗（デコード処理中にisImmediate==trueでコールした場合）
  *//*------------------------------------------------------------------*/
  bool  StartDecode(
          const void* pQRImage,
          const u16   pImageSize[2],
          const s32   pTargetImageRect[4] = NULL,
          const bool  isImmediate         = false
        );

  inline bool                         IsEnd(void) const     {return (this->GetState() == STATE_IDLE);}      /**<  デコード処理が終了している  */
  inline bool                         IsSuccess(void) const {return (this->GetResult() == RESULT_SUCCESS);} /**<  デコード処理が成功した      */

  inline State                        GetState(void) const  {return mState;}                                /**<  状態の取得                  */
  inline Result                       GetResult(void) const {return mLastResult;}                           /**<  前回の処理の結果を取得      */

  const void*                         GetDecodeData(u32* pDataSize = NULL) const;                           /**<  結果の取得  */
  inline const mw::qrd::DecodeData*   GetDecodeDataRaw(void) const  {return &mDecodeDataCache;}             /**<  結果の取得  */


protected:
private:
  Result  DecodeResultHandler(void);                  /**<  APIの実行結果に応じた処理     */

  /*  members  */
  gfl2::thread::Thread*     mpDecodeThread;           /**<  デコードスレッド              */
  QRDecodeThreadProcedure*  mpDecodeThreadProcedure;  /**<  デコードスレッド処理部        */
  mw::qrd::DecodeData       mDecodeDataCache;         /**<  最後にデコードが成功した結果  */
  State                     mState;                   /**<  内部状態                      */
  Result                    mLastResult;              /**<  前回の処理結果                */

};


/*  ********************************************************************
    global
  *******************************************************************/


/*  ********************************************************************
    prototype
  *******************************************************************/


} /*  namespace QRUtil    */
} /*  namespace QRReader  */
} /*  namespace NetApp    */
#endif  /*  #if !defined(UTIL_QRDECODER_H_INCLUDED)  */


#endif  /*  #if defined(GF_PLATFORM_CTR)  */

