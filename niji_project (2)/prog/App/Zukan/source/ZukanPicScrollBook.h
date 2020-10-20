#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLBOOK_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLBOOK_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanPicScrollBook.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.11
 @brief   図鑑の絵巻ブック
 @note    複数のシート(ページ、１画面)を管理するものをブックと呼ぶことにする
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

// 図鑑のインクルード
#include "ZukanDefine.h"


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
  class ZukanPicScrollDataAccessor;
  class ZukanPicScrollSheetInfo;
  class ZukanPicScrollBgInfo;
  class ZukanContents;
  class ZukanPokePicManager;
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  class ZukanPrismSheet;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPicScrollBook
 @brief     図鑑の絵巻ブック
 */
//==============================================================================
class ZukanPicScrollBook
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPicScrollBook);

public:
  ZukanPicScrollBook(
    app::util::Heap*                    pAppHeap,
    app::util::G2DUtil*                 pG2DUtil,
    gfl2::lyt::LytMultiResID            lytRes,
    gfl2::lyt::LytWk*                   pLytWkSheet0,
    gfl2::lyt::LytWk*                   pLytWkSheet1,
    gfl2::lyt::LytWk*                   pLytWkSheet2,
    u32                                 lytWkIdSheet0,
    u32                                 lytWkIdSheet1,
    u32                                 lytWkIdSheet2,
    gfl2::lyt::LytWk*                   pBackmostLytWk,
    gfl2::lyt::LytWk*                   pCurtainLytWk,
    u32                                 backmostLytWkId,
    u32                                 curtainLytWkId,
    const ZukanPicScrollDataAccessor*   pPicScrollDataAccessor,
    const PokeTool::ExtendData*         pPokeExtendData,
    gfl2::str::MsgData*                 pMsgData,  // <niji_conv_header/message/msg_zukan.h>
    print::WordSet*                     pWordSet,
    ZukanContents*                      pContents,
    ZukanPokePicManager*                pPokePicManager
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    , ZukanPrismSheet*                  pPrismSheet
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  );
  virtual ~ZukanPicScrollBook();

  void Update(void);

  // 直ちにalolaNoを表示したいときはimmediateFlagにtrueを渡す
  void SetSheet(u32 alolaNo, bool immediateFlag=false);

public:
  // なしのときのシートのインデックス
  static const s32 SHEET_INDEX_NONE = 0xFFFF;  // u32のZukanPicScrollDataAccessor::PLACE_GROUP_NO_NONEとは違うので注意すること！

private:
  // LytWkポインタ  // G2DUtilに渡すIDと勘違いされないようにポインタという名前にした。G2DUtilに渡すIDとしては使えない。
  enum LytWkPtr
  {
    LYT_WK_PTR_SHEET_0,
    LYT_WK_PTR_SHEET_1,
    LYT_WK_PTR_SHEET_2,
    LYT_WK_PTR_NUM,
  };

  enum State
  {
    STATE_STAY,  // 静止中
    STATE_MOVE,  // 移動中
  };

private:
  void setup(app::util::Heap* pAppHeap);

  //! @brief  アローラ番号alolaNoのポケモンが入っているシートのインデックスを得る。
  s32 getSheetIndex(u32 alolaNo) const;  // なしのときはZukanPicScrollBook::SHEET_INDEX_NONE

  //! @brief  sheetIndexの画面中央からの現在のY距離を計算する。画面中央より上方にあるときは正、下方にあるときは負。
  f32 calcDistFromCenter(s32 sheetIndex) const;

  // 初期化する必要があるとき、シートを初期化し初期状態の表示をする
  void initSheet(s32 prevSheetIndex);  // 前の画面中央のシートのインデックス

  void changeState(State state);

private:
  app::util::G2DUtil*                 m_pG2DUtil;
  gfl2::lyt::LytMultiResID            m_lytRes;
  gfl2::lyt::LytWk*                   m_pLytWk[LYT_WK_PTR_NUM];
  u32                                 m_lytWkId[LYT_WK_PTR_NUM];
  gfl2::lyt::LytWk*                   m_pBackmostLytWk;
  gfl2::lyt::LytWk*                   m_pCurtainLytWk;
  u32                                 m_backmostLytWkId;
  u32                                 m_curtainLytWkId;
  const ZukanPicScrollDataAccessor*   m_pPicScrollDataAccessor;
  const PokeTool::ExtendData*         m_pPokeExtendData;
  gfl2::str::MsgData*                 m_pMsgData;
  print::WordSet*                     m_pWordSet;
  ZukanContents*                      m_pContents;
  ZukanPokePicManager*                m_pPokePicManager;
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  ZukanPrismSheet*                    m_pPrismSheet;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

  s32                        m_sheetNum;
  u32                        m_alolaNo_end;               // 1<=alolaNo<=alolaNo_end

  u8*                        m_sheetLytWkPtrList;           // m_sheetList[m_sheetNum]  m_sheetList[i]に、i番目のインデックスのシートはどのLytWkPtrを使うか、を覚えておく。
  u32*                       m_placeGroupTopAlolaNoList;    // m_placeGroupTopAlolaNoList[m_sheetNum]  m_placeGroupTopAlolaNoList[i]に、i番目のインデックスのシートの「配置グループ先頭のアローラ図鑑番号」、を覚えておく。
  s32*                       m_sheetIndexList;              // m_sheetIndexList[alolaNo_end+1]  m_sheetIndexList[alolaNo(1<= <=alolaNo_end)]に、alolaNoのポケモンが入っているシートのインデックス(0始まり)、を覚えておく。
                                                            // なしのときはZukanPicScrollBook::SHEET_INDEX_NONE
  ZukanPicScrollSheetInfo*   m_sheetInfo;                   // m_sheetInfo[LYT_WK_PTR_NUM];

  ZukanPicScrollBgInfo*  m_bgInfo;

  State m_state;
  u32   m_stateSeq;

  s32  m_goalSheetIndex;     // 画面中央に持って行きたいシートのインデックス
  f32  m_currSheetIndexF32;  // 現在の画面中央のシートのインデックス
  s32  m_currSheetIndex;     // m_currSheetIndexの整数版。例：1.5<=m_currSheetIndexF32<2.5のとき2になる。

  s32  m_currMoveDir;    // 現在動いていたら+1or-1、動いていなかったら0
  f32  m_currMoveSpeed;  // 現在のスピード(m_currMoveFlagがtrueのときのみ有効)
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLBOOK_H_INCLUDED

