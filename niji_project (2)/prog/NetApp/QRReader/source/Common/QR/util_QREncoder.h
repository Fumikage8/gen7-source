

#if defined(GF_PLATFORM_CTR)


#if !defined(UTIL_QRENCODER_H_INCLUDED)
#define UTIL_QRENCODER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   util_QREncoder.h
 * @date   20120911 Tue.
 * @date   2016/01/18 Mon. 17:51:14
 * @author muto_yu
 * @brief  QRエンコーダ
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
#include  <Layout/include/gfl2_Layout.h>
#include  <Layout/include/gfl2_LytSys.h>


#include  <mw/qre/qre.h>


namespace NetApp    {
namespace QRReader  {
namespace QRUtil    {
/*  ********************************************************************
    define
  *******************************************************************/
/**
  QRエンコーダ
  任意のデータからQR画像を生成します。
  QRコードに格納できるデータサイズは、フォーマットバージョンと誤り訂正レベルで決定します。
  
  @note  ・QRコードを構成する一つの四角い領域を「セル」と呼びます。
         ・QRコードには誤り訂正機能があるため多少の汚れには対応できますが、
           縦横比が変わったり（セルが正方形でなくなる）Pixel補間がかかるなど変形したものは
           認識率が低下します。
         ・コントラストが不鮮明なコードは認識率が低下します。
         ・コード領域とは別に、周囲にそれぞれ4セルずつ（上下8セルずつ）のマージンが必要です。
         ・コード領域に任意の画像を埋め込むことが可能（デザインQRコード）です。
         ・データを分割して複数のQRコードとすることが可能です。

  @note  QRコードに関する詳細はこちらを参照して下さい。
         http://www.qrcode.com/index.html
*/
class QREncoder {
public:
  /*  version  */
  static const u32      QRVER_AUTO  = 0;    /**<  QRバージョン自動  */
  static const u32      QRVER_MIN   = 1;    /**<  QRバージョン最小  */
  static const u32      QRVAR_MAX   = 40;   /**<  QRバージョン最大  */

  /*  分割数  */
  static const u32      QRDIV_AUTO  = 0;    /**<  QR分割数自動      */
  static const u32      QRDIV_MIN   = 1;    /**<  QR分割数最小      */
  static const u32      QRDIV_MAX   = 16;   /**<  QR分割数最大      */

  /** 誤り訂正レベル  */
  enum  EccLV  {
    ECCLV_L = mw::qre::ECC_LEVEL_L,         /**<  誤り訂正レベルL。7%のデータを復元することが可能です。                              */
    ECCLV_M = mw::qre::ECC_LEVEL_M,         /**<  誤り訂正レベルM。15%のデータを復元することが可能です。※通常の推奨値               */
    ECCLV_Q = mw::qre::ECC_LEVEL_Q,         /**<  誤り訂正レベルQ。25%のデータを復元することが可能です。                             */
    ECCLV_H = mw::qre::ECC_LEVEL_H          /**<  誤り訂正レベルH。30%のデータを復元することが可能です。※デザインQRコード時の推奨値 */
  };

  /** setupパラメータ  */
  typedef struct  Config {
    gfl2::heap::HeapBase* pAppHeap;         /**<  アプリケーションヒープ  */
    gfl2::heap::HeapBase* pDevHeap;         /**<  デバイスヒープ          */
    u32                   srcDataSize;      /**<  入力データサイズ        */
    u32                   pixelPerCell;     /**<  セルのピクセルサイズ    */
    u32                   divideNum;        /**<  分割数                  */

    /** ctor  */
    Config(void)  :
       pAppHeap(NULL)
      ,pDevHeap(NULL)
      ,srcDataSize(0)
      ,pixelPerCell(0)
      ,divideNum(0)
    {
    }
  }Config;


  QREncoder(void);                          /**<  ctor  */
  virtual ~QREncoder();                     /**<  dtor  */

  void  Setup(const Config& rConfig);       /**<  使用準備          */
  void  Cleanup(void);                      /**<  使用終了          */

  /*-----------------------------------------------------------------*//**
    エンコード
    @note   デフォルト引数は
              ・ver 18
              ・分割なし
              ・セルサイズ  2x2
              ・EccLV       M
            としています。
            セル数は89x89なので、生成されるQR画像は一辺(89+8)[cell] * 2[pixel] の194x194[pixel]となり、
            格納可能なデータ量は
              ビット数  4,504
              数字      1,346
              英数字      816
              バイナリ    560
              漢字        345
            となります。
    @param  [in]  dataSize      データサイズ
    @param  [in]  pData         データ
    @param  [in]  qrVersion     QRバージョン [1, 40]、0で自動
    @param  [in]  qrDivTotal    QR分割数 [1, 16]、0で自動
    @param  [in]  pixelPerCell  QRセルのピクセル数
    @param  [in]  eccLv         QRの誤り訂正レベル
    @retval    true   : 成功
    @retval    false  : 失敗
  *//*------------------------------------------------------------------*/
  bool  Encode(
          u32           dataSize,
          const void*   pData,
          u32           qrVersion     = 18,
          u32           qrDivTotal    = 1,
          u32           pixelPerCell  = 2,
          EccLV         eccLv         = ECCLV_M
        );

  /*-----------------------------------------------------------------*//**
    エンコード（イメージ埋め込み）
    @note   デフォルト引数は
              ・ver 18
              ・分割なし
              ・セルサイズ  2x2
              ・EccLV       H
            としています。
            セル数は89x89なので、生成されるQR画像は一辺(89+8)*2の194x194[pixel]となり、
            格納可能なデータ量は
              ビット数  2,504
              数字        746
              英数字      452
              バイナリ    310
              漢字        191
            となります。
    @param  [in]  dataSize      データサイズ
    @param  [in]  pData         データ
    @param  [in]  imageW        埋め込み画像幅
    @param  [in]  imageH        埋め込み画像高
    @param  [in]  pRGB888Image  埋め込み画像（RGB888）
    @param  [in]  qrVersion     QRバージョン [1, 40]、0で自動
    @param  [in]  qrDivTotal    QR分割数 [1, 16]、0で自動
    @param  [in]  pixelPerCell  QRセルのピクセル数
    @param  [in]  eccLv         QRの誤り訂正レベル
    @retval    true   : 成功
    @retval    false  : 失敗
  *//*------------------------------------------------------------------*/
  bool  EncodeWithImage(
          u32           dataSize,
          const void*   pData,
          u16           imageW,
          u16           imageH,
          const void*   pRGB888Image,
          u32           qrVersion     = 18,         /*  8以上推奨  */
          u32           qrDivTotal    = 1,
          u32           pixelPerCell  = 2,
          EccLV         eccLv         = ECCLV_H     /*  H推奨      */
        );

  void  BuildQRTexture(void);           /**<  描画用テクスチャの構築  ※Linear->Block変換をCPUで行うため、時間がかかる可能性があります  */
  void  ReleaseQRTexture(void);         /**<  描画用テクスチャの解体  */


  void  GetImage(u8** ppRGB888, u32 divIndex = 0, u16* pWidth = NULL, u16* pHeight = NULL);     /**<  QR画像の取得          */
  bool  GetBMP(u8* pDstBuf, u32 bufSize, u32 divIndex = 0, u32* pDataSize = NULL);              /**<  QR画像をBMPとして取得 */


  /*-----------------------------------------------------------------*//**
    描画用テクスチャの物理アドレスを取得
    @return   描画テクスチャの物理アドレス
  *//*------------------------------------------------------------------*/
  uptr  GetTexture(
          u32   divIndex,
          u32*  pTexW = NULL,
          u32*  pTexH = NULL,
          u32*  pImageW = NULL,
          u32*  pImageH = NULL
        );

  /*-----------------------------------------------------------------*//**
    描画用テクスチャをPicturePaneに設定
  *//*------------------------------------------------------------------*/
  inline void  AssignTexture(gfl2::lyt::LytWk* pLayoutWork, const gfl2::lyt::LytPaneIndex picturePaneIndex, const u32 textureIndex = 0, const u32 divIndex = 0) {
                  GFL_ASSERT_STOP(pLayoutWork);
                  this->AssignTexture(pLayoutWork->GetPicturePane(picturePaneIndex), textureIndex, divIndex);
               }

  /*-----------------------------------------------------------------*//**
    描画用テクスチャをPicturePaneに設定
  *//*------------------------------------------------------------------*/
  void  AssignTexture(gfl2::lyt::LytPicture* pPicturePane, const u32 textureIndex = 0, const u32 divIndex = 0);


protected:
  /** QR画像表示用テクスチャ情報  */
  class  QRTextureInfo {
  public:
    void*   mpTexture;            /**<  QR画像テクスチャ（RGB888-block）      */
    uptr    mpTexture_Physical;   /**<  QR画像テクスチャ（物理アドレス）      */
    u32     mBytes;               /**<  QR画像テクスチャデータサイズ          */
    u32     mPixelSize[2];        /**<  QR画像テクスチャpixelサイズ           */
    u32     mImagePixelSize[2];   /**<  QR画像テクスチャの有効部分pixelサイズ */

    /** ctor  */
    QRTextureInfo(void);
    /** dtor  */
    virtual~ QRTextureInfo();

    /**  テクスチャ生成 */
    void     Setup(
               gfl2::heap::HeapBase*  pDevHeap,
               void*                  pInputImage,
               u32                    imputImageW,
               u32                    imputImageH
             );

    /**  テクスチャ解放 */
    void     Cleanup(void);

  private:
  };


  gfl2::heap::HeapBase*             mpAppHeap;                /**<  アプリケーションヒープ          */
  gfl2::heap::HeapBase*             mpDevHeap;                /**<  デバイスヒープ                  */
  mw::qre::QREncoder::QREncoder     mMWQREncoder;             /**<  エンコーダ                      */
  void*                             mpQRWorkBuffer;           /**<  エンコーダワーク                */

  mw::qre::ImageInfo*               mpMWQRImageInfoArray;     /**<  QR画像情報                      */
  u32                               mMWQRImageInfoArrayNum;   /**<  QR画像情報数                    */

  QRTextureInfo*                    mpQRTextureInfoArray;     /**<  QR画像テクスチャ                */
  u32                               mQRTextureInfoArrayNum;   /**<  QR画像テクスチャ数              */


  void    BuildQRImage(void);
  void    ReleaseQRImage(void);

private:
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
#endif  /*  #if !defined(UTIL_QRENCODER_H_INCLUDED)  */

#endif  /*  #if defined(GF_PLATFORM_CTR)  */
