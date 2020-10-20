#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANQRDISPLAY_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANQRDISPLAY_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanQRDisplay.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.06
 @brief   図鑑で行うQRコード表示
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <NetStatic/NetAppLib/include/QR/QRDefine.h>

// 図鑑のインクルード
#include "ZukanContents.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppQRTextureCreateUtility;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanQRDisplay
 @brief     図鑑で行うQRコード表示
 */
//==============================================================================
class ZukanQRDisplay
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanQRDisplay);

public:
  ZukanQRDisplay(
      gfl2::heap::HeapBase*   heap,
      gfl2::lyt::LytPicture*  picPane
  );
  virtual ~ZukanQRDisplay();

  void Update(void);

  bool IsCreated(void) const;
  bool CanDestroy(void) const;

  void ReqCreateQR(gfl2::heap::HeapBase* tempHeap, const ZukanContents::PokeData* pokeData, const PokeTool::SimpleParam& dispSp, bool dispGenderMaleFemaleFlag, const u32* perRand/*[NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX]*/);  // tempHeapはこの関数内で一時的に使用するヒープ
  void SetVisible(bool visible);

private:
  static const u32 ENCODE_DATA_BUFFER_SIZE = 512;
  
  enum State
  {
    STATE_NONE,      // 何もなし
    STATE_CREATING,  // 生成中
    STATE_CREATED,   // 生成済み
  };

private:
  gfl2::lyt::LytPicture*                 m_picPane;
  app::util::AppQRTextureCreateUtility*  m_QRCreateUtil;

  bool m_visible;     // 外部からの表示非表示設定
  State m_state;      // 生成状態
  const ZukanContents::PokeData* m_pokeDataOfQR;  // m_qrValの元になったデータへのポインタ
  PokeTool::SimpleParam m_dispSp;
  NetApp::QR::QR_ZUKAN m_qrVal;   // 現在生成済み、現在生成中のQRコード値
  
  bool m_createReq;    // 生成リクエストが出ているならtrue
  const ZukanContents::PokeData* m_reqPokeDataOfQR;  // m_reqQrValの元になったデータへのポインタ
  PokeTool::SimpleParam m_reqDispSp;
  NetApp::QR::QR_ZUKAN m_reqQRVal;  // 生成リクエスト中のQRコード値
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANQRDISPLAY_H_INCLUDED

