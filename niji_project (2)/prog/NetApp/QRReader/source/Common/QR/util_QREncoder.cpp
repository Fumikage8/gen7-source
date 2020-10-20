
#if defined(GF_PLATFORM_CTR)

//==============================================================================
/**
 * @file      util_QREncoder.cpp
 * @date      20120911 Tue.
 * @date      2016/01/18 Mon. 17:49:16
 * @author    muto_yu
 * @brief     QRエンコーダ
 * @note      過去Projectより移植
 * @attention ミドルウェアライブラリに依存する為CTR専用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


/*	********************************************************************
    include
  *******************************************************************/
#include  "./util_QREncoder.h"
#include  "./texFormatConverter/demo_TextureConverter.h"



namespace NetApp    {
namespace QRReader  {
namespace QRUtil    {
/* ********************************************************************
    define
  *******************************************************************/


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
QREncoder::QREncoder(void)  :
   mpAppHeap(NULL)
  ,mpDevHeap(NULL)
  ,mMWQREncoder()
  ,mpQRWorkBuffer(NULL)
  ,mpMWQRImageInfoArray(NULL)
  ,mMWQRImageInfoArrayNum(0)
  ,mpQRTextureInfoArray(NULL)
  ,mQRTextureInfoArrayNum(0)

{
}


/*-----------------------------------------------------------------*//**
  dtor
*//*------------------------------------------------------------------*/
QREncoder::~QREncoder() {
  this->Cleanup();
}


/*-----------------------------------------------------------------*//**
  使用準備
*//*------------------------------------------------------------------*/
void  QREncoder::Setup(const Config& rConfig) {
  GFL_ASSERT(!mpQRWorkBuffer);     /*  2重セットアップ  */

  mpAppHeap  = rConfig.pAppHeap;
  mpDevHeap  = rConfig.pDevHeap;

  /*  work  */
  {
    const u32   workSize  = mw::qre::QREncoder::GetEncodeBufferSize(rConfig.srcDataSize, rConfig.pixelPerCell, rConfig.divideNum);

//    mpQRWorkBuffer = GflHeapAllocMemoryAlign(mpAppHeap, workSize, 4);
    mpQRWorkBuffer = GflHeapAllocMemoryAlign(mpDevHeap, workSize, 4);
    GFL_ASSERT(mpQRWorkBuffer);

    mMWQREncoder.InitMemory(mpQRWorkBuffer, workSize);
  }
}


/*-----------------------------------------------------------------*//**
  使用完了
*//*------------------------------------------------------------------*/
void  QREncoder::Cleanup(void) {
  this->ReleaseQRImage();
  this->ReleaseQRTexture();

  GflHeapFreeMemory(mpQRWorkBuffer);
  mpQRWorkBuffer = NULL;
}


/*-----------------------------------------------------------------*//**
  エンコード
*//*------------------------------------------------------------------*/
bool  QREncoder::Encode(
  u32           dataSize,
  const void*   pData,
  u32           qrVersion,
  u32           qrDivTotal,
  u32           pixelPerCell,
  EccLV         eccLv
) {
  bool                  isEncoded = false;
  mw::qre::EncodeData   encodeData;

  encodeData.size       = dataSize;
  encodeData.data       = reinterpret_cast<u8*>((const_cast<void*>(pData)));
  encodeData.version    = qrVersion;
  encodeData.total      = qrDivTotal;
  encodeData.cell_size  = pixelPerCell;
  encodeData.ecc_level  = static_cast<mw::qre::EccLevel>(eccLv);


#if 0
  {
    const u8*  pdata = reinterpret_cast<const u8*>(encodeData.data);

    GFL_PRINT("[%s] dump :\n  ", __FUNCTION__);
    for(u32 num = 0; num < 16; ++num) {
      GFL_PRINT("%02x", pdata[num]);
      if(num && !(num%4))   GFL_PRINT(" ");
    }
    GFL_PRINT("\n");
  }
#endif

  isEncoded = mMWQREncoder.Encode(&encodeData);
  this->BuildQRImage();

  return isEncoded;
}


/*-----------------------------------------------------------------*//**
  イメージ付きエンコード
*//*------------------------------------------------------------------*/
bool  QREncoder::EncodeWithImage(
  u32           dataSize,
  const void*   pData,
  u16           imageW,
  u16           imageH,
  const void*   pRGB888Image,
  u32           qrVersion,
  u32           qrDivTotal,
  u32           pixelPerCell,
  EccLV         eccLv
) {
  bool                  isEncoded = false;
  mw::qre::EncodeData   encodeData;
  mw::qre::ImageInfo    imageInfo;

  encodeData.size       = dataSize;
  encodeData.data       = reinterpret_cast<u8*>((const_cast<void*>(pData)));
  encodeData.version    = qrVersion;
  encodeData.total      = qrDivTotal;
  encodeData.cell_size  = pixelPerCell;
  encodeData.ecc_level  = static_cast<mw::qre::EccLevel>(eccLv);

  imageInfo.width       = imageW;
  imageInfo.height      = imageH;
  imageInfo.rgb888Data  = reinterpret_cast<u8*>(const_cast<void*>(pRGB888Image));
//  imageInfo.imgPos      /*  未使用  */

  isEncoded = mMWQREncoder.EncodeWithImage(&encodeData, &imageInfo);
  this->BuildQRImage();

  return isEncoded;
}


/*-----------------------------------------------------------------*//**
  描画用テクスチャをPicturePaneに設定
*//*------------------------------------------------------------------*/
void  QREncoder::AssignTexture(gfl2::lyt::LytPicture* pPicturePane, const u32 textureIndex, const u32 divIndex) {
  if(!pPicturePane)
  {
    GFL_ASSERT(pPicturePane);
    return;
  }

  gfl2::lyt::LytMaterial*  pMaterial  = pPicturePane->GetMaterial(0);   /*  0のみ */

  if(!pMaterial)
  {
    GFL_ASSERT(pMaterial);
    return;
  }

  {
    u32                   texW, texH, imageW, imageH;
    uptr                  pTexture  = this->GetTexture(divIndex, &texW, &texH, &imageW, &imageH);
    nw::lyt::TextureInfo  texInfo;

    if(!pTexture)
    {
      GFL_ASSERT(pTexture);
      return;
    }

    pMaterial->GetTexMap(0).GetTextureInfo(&texInfo);

    texInfo.Set(
      texInfo.GetTextureObject(),
      pTexture,
      nw::lyt::TexSize(imageW, imageH),
      nw::lyt::TexSize(texW, texH),
      nw::lyt::TEXFORMAT_RGB8,          /*  texInfo.GetFormat(),  */
      nw::lyt::TEXELROTATION_NONE
    );

    pMaterial->SetTexMap(0, &texInfo);
  }
}


/*-----------------------------------------------------------------*//**
*//*------------------------------------------------------------------*/
void  QREncoder::BuildQRImage(void) {
  this->ReleaseQRImage();

  mMWQRImageInfoArrayNum  = mMWQREncoder.GetDivCount();
  GFL_ASSERT(mMWQRImageInfoArrayNum);    /*  エンコードを行っていない可能性  */

  mpMWQRImageInfoArray  = GFL_NEW(mpAppHeap) mw::qre::ImageInfo[mMWQRImageInfoArrayNum];
  for(u32 num = 0; num < mMWQRImageInfoArrayNum; ++num) {
    const u32            qrImageSize  = mMWQREncoder.GetQRSize(num, false);

    /*  バッファを確保してデータを取得  */
    mpMWQRImageInfoArray[num].rgb888Data = (u8*)GflHeapAllocMemoryAlign(mpDevHeap, qrImageSize, 4);
    mMWQREncoder.GetQRData(&mpMWQRImageInfoArray[num], num);
  }
}


/*-----------------------------------------------------------------*//**
*//*------------------------------------------------------------------*/
void  QREncoder::ReleaseQRImage(void)  {
  if(mpMWQRImageInfoArray)  {
    for(u32 num = 0; num < mMWQRImageInfoArrayNum; ++num) {
      GflHeapFreeMemory(mpMWQRImageInfoArray[num].rgb888Data);
      mpMWQRImageInfoArray[num].rgb888Data = NULL;
    }
    GFL_SAFE_DELETE_ARRAY(mpMWQRImageInfoArray);
  }
  mMWQRImageInfoArrayNum  = 0;
}


/*-----------------------------------------------------------------*//**
*//*------------------------------------------------------------------*/
void  QREncoder::BuildQRTexture(void) {
  GFL_ASSERT(mMWQRImageInfoArrayNum);     /*  コードデータが存在しない  */
  this->ReleaseQRTexture();

  mQRTextureInfoArrayNum  = mMWQRImageInfoArrayNum;
  mpQRTextureInfoArray    = GFL_NEW(mpAppHeap) QRTextureInfo[mQRTextureInfoArrayNum];
  for(u32 num = 0; num < mQRTextureInfoArrayNum; ++num)  {
    mpQRTextureInfoArray[num].Setup(
      mpDevHeap,
      mpMWQRImageInfoArray[num].rgb888Data,
      mpMWQRImageInfoArray[num].width,
      mpMWQRImageInfoArray[num].height
    );
  }
}


/*-----------------------------------------------------------------*//**
*//*------------------------------------------------------------------*/
void  QREncoder::ReleaseQRTexture(void)  {
  if(mpQRTextureInfoArray)  {
    for(u32 num = 0; num < mQRTextureInfoArrayNum; ++num)  {
      mpQRTextureInfoArray[num].Cleanup();
    }
    GFL_SAFE_DELETE_ARRAY(mpQRTextureInfoArray);
  }
  mQRTextureInfoArrayNum  = NULL;
};


/*-----------------------------------------------------------------*//**
  QR画像の取得
*//*------------------------------------------------------------------*/
void  QREncoder::GetImage(u8** ppRGB888, u32 divIndex, u16* pWidth, u16* pHeight) {
  GFL_ASSERT_STOP(ppRGB888);

  if(divIndex < mMWQRImageInfoArrayNum) {
    *ppRGB888 = mpMWQRImageInfoArray[divIndex].rgb888Data;
    if(pWidth)    *pWidth   = mpMWQRImageInfoArray[divIndex].width;
    if(pHeight)   *pHeight  = mpMWQRImageInfoArray[divIndex].height;

  }else {
    GFL_WARNING("イメージ情報数%dに対し、index %dが要求されています。\n", mMWQRImageInfoArrayNum, divIndex);
  }
}


/*-----------------------------------------------------------------*//**
  QR画像をBMPとして取得
*//*------------------------------------------------------------------*/
bool  QREncoder::GetBMP(u8* pDstBuf, u32 bufSize, u32 divIndex, u32* pDataSize) {
  GFL_ASSERT_STOP(pDstBuf);

  bool  isSuccess = false;

  if(divIndex < mMWQRImageInfoArrayNum) {
    isSuccess = mMWQREncoder.GetQRBMPData(pDstBuf, bufSize, divIndex);

  }else {
    GFL_WARNING("イメージ情報数%dに対し、index %dが要求されています。\n", mMWQRImageInfoArrayNum, divIndex);

  }

  if(isSuccess) {
    if(pDataSize) *pDataSize  = mMWQREncoder.GetQRSize(divIndex, true);
  }else {
    GFL_WARNING("失敗しました。\n");
    
  }



  return isSuccess;
}


/*-----------------------------------------------------------------*//**
*//*------------------------------------------------------------------*/
uptr  QREncoder::GetTexture(u32 divIndex, u32* pTexW, u32 *pTexH, u32* pImageW, u32 *pImageH) {
  GFL_ASSERT_STOP(divIndex < mQRTextureInfoArrayNum);

  uptr  pPhysicalAddr = NULL;

  {
    QRTextureInfo&  rTexInfo  = mpQRTextureInfoArray[divIndex];

    pPhysicalAddr = rTexInfo.mpTexture_Physical;

    if(pTexW)     *pTexW    = rTexInfo.mPixelSize[0];
    if(pTexH)     *pTexH    = rTexInfo.mPixelSize[1];
    if(pImageW)   *pImageW  = rTexInfo.mImagePixelSize[0];
    if(pImageH)   *pImageH  = rTexInfo.mImagePixelSize[1];
  }

  return pPhysicalAddr;
}


/*-----------------------------------------------------------------*//**
  ctor
  テクスチャデータ管理
*//*------------------------------------------------------------------*/
QREncoder::QRTextureInfo::QRTextureInfo(void) :
    mpTexture(NULL)
  , mpTexture_Physical(NULL)
  , mBytes(0)
{
  mPixelSize[0]       = 
  mPixelSize[1]       = 0;
  mImagePixelSize[0]  = 
  mImagePixelSize[1]  = 0;
}


/*-----------------------------------------------------------------*//**
  dtor
  テクスチャデータ管理
*//*------------------------------------------------------------------*/
QREncoder::QRTextureInfo::~QRTextureInfo()  {
  this->Cleanup();
}


/*-----------------------------------------------------------------*//**
*//*------------------------------------------------------------------*/
void  QREncoder::QRTextureInfo::Setup(
  gfl2::heap::HeapBase*  pDevHeap,
  void*                  pInputImage,
  u32                    imputImageW,
  u32                    imputImageH
) {
  const u32   bytesPerPixel = 3;

  this->Cleanup();

  mImagePixelSize[0]  = imputImageW;
  mImagePixelSize[1]  = imputImageH;
  mPixelSize[0]  =
  mPixelSize[1]  = 32;

  /*  テクスチャサイズを算出  */
  while(mPixelSize[0] < mImagePixelSize[0]) {
    mPixelSize[0] *= 2;
  }
  GFL_ASSERT(mPixelSize[0] <= 1024);

  while(mPixelSize[1] < mImagePixelSize[1]) {
    mPixelSize[1] *= 2;
  }
  GFL_ASSERT(mPixelSize[1] <= 1024);


  {
    const u32   qrLineBytes   = mImagePixelSize[0] * bytesPerPixel; /*  qr画像のラインサイズ    */
    const u32   texLineBytes  = mPixelSize[0] * bytesPerPixel;      /*  テクスチャラインサイズ  */
    const u32   strideBytes   = texLineBytes - qrLineBytes;         /*  余白部分のラインサイズ  */
    u32         line          = 0;

    /*  バッファ確保  */
    u8*   pLinearSrc  = (u8*)GflHeapAllocMemoryAlign(pDevHeap->GetLowerHandle(), mPixelSize[0] * mPixelSize[1] * bytesPerPixel, 128);   /**<  一時バッファ  */
    mpTexture         = GflHeapAllocMemoryAlign(pDevHeap, mPixelSize[0] * mPixelSize[1] * bytesPerPixel, 128);

    GFL_ASSERT(pLinearSrc);
    GFL_ASSERT(mpTexture);

    /*  変換元を準備（ConvertGLTextureToNativeで上下が逆になるようなので、ここでも反転）  */
    {
      const u8*   pSrc  = reinterpret_cast<const u8*>(pInputImage);

      /*  有効部分  */
      for(line = 0; line < mImagePixelSize[1]; ++line)  {
        u8*   pDst  = pLinearSrc + texLineBytes * (mPixelSize[1] - 1 - line);

        std::memcpy(pDst, pSrc, qrLineBytes);   /*  qr部分  */
        pDst  += qrLineBytes;
        pSrc  += qrLineBytes;

        std::memset(pDst, 0x44, strideBytes);   /*  右余白  */
        pDst  += strideBytes;
      }

      /*  下余白  */
      std::memset(pLinearSrc, 0x0f, texLineBytes * (mPixelSize[1] - mImagePixelSize[1]));
    }

    /*  linear->block変換  */
    QRUtil::ConvertGLTextureToNative(GL_RGB_NATIVE_DMP, mPixelSize[0], mPixelSize[1], pLinearSrc, mpTexture);

    /*  一時バッファの解放  */
    GflHeapFreeMemory(pLinearSrc);
  }

  mpTexture_Physical  = nn::gx::CTR::GetPhysicalAddr((uptr)mpTexture);
}


/*-----------------------------------------------------------------*//**
*//*------------------------------------------------------------------*/
void  QREncoder::QRTextureInfo::Cleanup(void) {
  GflHeapFreeMemory(mpTexture);
  mpTexture           = NULL;
  mpTexture_Physical  = NULL;
  mBytes              = 0;
  mPixelSize[0]       =
  mPixelSize[1]       = 0;
  mImagePixelSize[0]  = 
  mImagePixelSize[1]  = 0;
}


} /*  namespace QRUtil    */
} /*  namespace QRReader  */
} /*  namespace NetApp    */

#endif  /*  #if defined(GF_PLATFORM_CTR)  */
