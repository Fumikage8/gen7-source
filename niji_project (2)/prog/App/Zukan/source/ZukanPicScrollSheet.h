#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLSHEET_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLSHEET_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanPicScrollSheet.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.11
 @brief   図鑑の絵巻シート
 @note    絵巻の１レイアウトをシート(ページ、１画面)と呼ぶことにする
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

// 図鑑のインクルード
#include "ZukanPicScrollDataAccessor.h"
#include "ZukanPicScrollUtil.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class MsgData;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)
  class WordSet;
GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(PokeTool)
  class ExtendData;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class G2DUtil;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  //class ZukanPicScrollDataAccessor;
  //class ZukanPicScrollUtil;
  class ZukanContents;
  class ZukanPokePicManager;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPicScrollSheet
 @brief     図鑑の絵巻シート
 */
//==============================================================================
class ZukanPicScrollSheet
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPicScrollSheet);

public:
  ZukanPicScrollSheet(
    app::util::Heap*                    pAppHeap,
    app::util::G2DUtil*                 pG2DUtil,
    gfl2::lyt::LytMultiResID            lytRes,
    gfl2::lyt::LytWk*                   pLytWk,
    u32                                 lytWkId,
    const ZukanPicScrollDataAccessor*   pPicScrollDataAccessor,
    const PokeTool::ExtendData*         pPokeExtendData,
    gfl2::str::MsgData*                 pMsgData,  // <niji_conv_header/message/msg_zukan.h>
    print::WordSet*                     pWordSet,
    ZukanContents*                      pContents,
    ZukanPokePicManager*                pPokePicManager
  );
  virtual ~ZukanPicScrollSheet();

  void InitSheet(u32 placeGroupTopAlolaNo);
  void UnuseSheet(void);
  void SetPosY(f32 posY);
  
  void UpdateSheet(void);

  void ReqInAnim(void);
  void ReqOutAnim(void);

private:
  enum State
  {
    STATE_WAIT_INIT,    // 初期化待ち
    STATE_WAIT_REQ_IN,  // インアニメのリクエストが来るのを待っている
    STATE_WAIT_IN,      // インアニメが始まるのを待っている
    STATE_IN,           // インアニメ中
    STATE_LOOP,         // ループ
    STATE_OUT,          // アウトアニメ中
  };

private:
  void termSheet(void);
  void setupPokeSpot( ZukanPicScrollUtil::PlaceInfo* placeInfo, u32 pokeNum );
  void startInAnim(void);
  void startOutAnim(void);
  void updatePokePicPane(void);

private:
  app::util::G2DUtil*                 m_pG2DUtil;
  gfl2::lyt::LytMultiResID            m_lytRes;
  gfl2::lyt::LytWk*                   m_pLytWk;
  u32                                 m_lytWkId;
  //gfl2::lyt::LytWk*                   m_pBackmostLytWk;
  //u32                                 m_backmostLytWkId;
  const ZukanPicScrollDataAccessor*   m_pPicScrollDataAccessor;
  const PokeTool::ExtendData*         m_pPokeExtendData;
  gfl2::str::MsgData*                 m_pMsgData;
  print::WordSet*                     m_pWordSet;
  ZukanContents*                      m_pContents;
  ZukanPokePicManager*                m_pPokePicManager;

  //ZukanPicScrollUtil::BgInfo*         m_bgInfo;  // @fix cov_ctr[11514]: 未使用変数削除
  ZukanPicScrollUtil::PlaceInfo*      m_placeInfo;
  ZukanPicScrollUtil::SheetInfo*      m_sheetInfo;  // m_sheetInfo->placeInfoにm_placeInfoを代入しておくが、解放はm_placeInfoで行う。

  u32                                 m_placeGroupTopAlolaNo;  // 未使用時は0

  State                               m_state;
  bool                                m_waitInFlag;
  bool                                m_waitOutFlag;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLSHEET_H_INCLUDED

