#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANSEARCHPOKEICONDISPLAY_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANSEARCHPOKEICONDISPLAY_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanSearchPokeIconDisplay.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.08
 @brief   図鑑の検索のポケアイコン表示クラス
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class G2DUtil;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
  class ZukanContents;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanSearchPokeIconDisplay
 @brief     図鑑の検索のポケアイコン表示クラス
 */
//==============================================================================
class ZukanSearchPokeIconDisplay
{
public:
  enum
  {
    POKE_ICON_X_NUM              = 13,
    POKE_ICON_Y_NUM              = 15,
    POKE_ICON_NUM                = POKE_ICON_X_NUM * POKE_ICON_Y_NUM,        // 総数
    POKE_ICON_NUM_IN_SCREEN      = POKE_ICON_X_NUM * (POKE_ICON_Y_NUM - 4),  // 画面内におさまっている個数(上2段と下2段がおさまらないので-4)
    POKE_ICON_NUM_WITHOUT_BOTTOM = POKE_ICON_X_NUM * (POKE_ICON_Y_NUM - 2),  // 下2段を除いた個数
    POKE_ICON_NUM_AT_TOP         = POKE_ICON_X_NUM * 2,                      // 上2段の個数
    
    POKE_ICON_DIFF_Y             = 21,  // ポケアイコンペインとポケアイコンペインの間のY距離
  };

  struct PokeIconPartsConstInfo
  {
    u32 partsPaneIndex;
    u32 picPaneIndex;
    u32 ballPaneIndex;
    u32 kiraPaneIndex;
    u32 animIndex;
  };

  class PokeIconPartsInfo
  {
  public:
    gfl2::lyt::LytParts*   partsPane;
    gfl2::lyt::LytPicture* picPane;
    gfl2::lyt::LytPane*    ballPane;
    gfl2::lyt::LytPane*    kiraPane;
  public:
    PokeIconPartsInfo(void)
      : partsPane(NULL),
        picPane(NULL),
        ballPane(NULL),
        kiraPane(NULL)
    {}
  };

public:
  ZukanSearchPokeIconDisplay(
    gfl2::heap::HeapBase*           devHeap,      // デバイスメモリ(何度もポケアイコンをつくり直すので専用のヒープ渡して下さい)。pokeDataNumMax個分のtextureInfoとポケアイコンをつくれるサイズ渡して下さい。
    ZukanDataManager*               dataManager,
    ZukanContents*                  contents,
    app::util::G2DUtil*             pG2DUtil,
    u32                             lytWkId,
    gfl2::lyt::LytWk*               lytWk,
    const gfl2::lyt::LytMultiResID& lytRes
  );

  virtual ~ZukanSearchPokeIconDisplay();

  // 表示を開始する(1個ずつ)
  void StartDisplay(
    const u8*  drawFigureDataIndexList,
    u16        dispPokeDataIndexNum,
    const u16* dispPokeDataIndexList
  );
  // 表示を開始する(1ページ分は全部表示し、その後1個ずつ表示する)
  void StartDisplayPage(
    const u8*  drawFigureDataIndexList,
    u16        dispPokeDataIndexNum,
    const u16* dispPokeDataIndexList
  );
  // 表示をやめる
  void CancelDisplay(void);
  // 毎フレーム呼ぶ
  void Update(void);
  // 一時停止リクエスト(キリのいいところまで進まないと一時停止できないので、リクエスト形式にした)
  void RequestPause(void);
  // 一時停止(強制的に一時停止させるので、呼び出し元でキリがいいことが分かっているときしか使わないで下さい)
  void ForcePause(void);
  // 表示が進行中か(表示中かつ進行中のときtrueを返す、表示していないときfalse、表示していても一時停止中のときfalse)
  bool IsDisplayProgressing(void) const;
  // 一時停止を解除する
  void ReleasePause(void);

private:
  enum State
  {
    STATE_NOT_DISPLAY,
    STATE_DISPLAY_CREATE,
    STATE_DISPLAY_SCROLL_UP,
    STATE_DISPLAY_SCROLL_DOWN,
    STATE_DISPLAY_WAIT_AT_TOP,
    STATE_DISPLAY_WAIT_AT_BOTTOM,
    STATE_DISPLAY_WAIT,
  };

  enum
  {
    WAIT_COUNT_MAX      = 60,                    // トップやボトムで待っているフレーム数
    RAISE_SCREEN_SPEED  = POKE_ICON_DIFF_Y / 7,  // スクリーンを1段上へ上げるスピード
    SCROLL_SCREEN_SPEED = 1,
  };

private:
  void startDisplay(
    const u8*  drawFigureDataIndexList,
    u16        dispPokeDataIndexNum,
    const u16* dispPokeDataIndexList
  );

  // m_dispPokeDataIndexList[m_index]のポケアイコンを生成し、表示する
  void createPokeIconTexture(void);
  // ポケアイコンを1段ずらす
  void shiftPokeIconTexture(s32 upOrDown);  // +1で上へずらす、-1で下へずらす
  // m_pokeIconPartsInfo[x][y]を表示する(描かれているのはm_dispPokeDataIndexList[index])
  void setPaneVisible(u32 x, u32 y, u32 index);
  // スクリーンを1段上へ上げる
  bool raiseScreen(void);  // 上へ上げ中のときはtrueを返す。上げなかったら、もしくは、上げるのが終了したらfalseを返す。

  // スクリーンをスクロールする
  bool scrollScreen(s32 upOrDown, bool* isShifted);  // +1で上へずらす、-1で下へずらす。スクロールが終了したらtrueを返す。
                                                      // ちょうど1段ずらしたらときだけisShiftedにtrueを入れて返す。

  void changeState(State state);

private:
  static const PokeIconPartsConstInfo  POKE_ICON_PARTS_CONST_INFO[POKE_ICON_Y_NUM][POKE_ICON_X_NUM];
  static const u32                     POKE_ICON_PARENT_PANE_ID[POKE_ICON_Y_NUM];

private:
  PokeIconPartsInfo                    m_pokeIconPartsInfo[POKE_ICON_Y_NUM][POKE_ICON_X_NUM];
 
  gfl2::heap::HeapBase*     m_devHeap;
  const ZukanDataManager*   m_dataManager;
  const ZukanContents*      m_contents;
  app::util::G2DUtil*       m_pG2DUtil;
  u32                       m_lytWkId;
  gfl2::lyt::LytWk*         m_lytWk;
  gfl2::lyt::LytMultiResID  m_lytRes;

  const u8*  m_drawFigureDataIndexList;
  u16        m_dispPokeDataIndexNum;
  const u16* m_dispPokeDataIndexList;  

  gfl2::lyt::LytResTextureInfo* m_textureInfo;  // gfl2::lyt::LytResTextureInfo[pokeDataNumMax]
  void**                        m_texBuf;       // void*[pokeDataNumMax]

  gfl2::lyt::LytPane*           m_pokeIconParentPane[POKE_ICON_Y_NUM];
  f32                           m_pokeIconPosY[POKE_ICON_Y_NUM];

  gfl2::lyt::LytPane*           m_topPane;      // 全体の表示/非表示や移動の制御に使うペイン

  bool m_pauseReq;  // 一時停止リクエストが出ているときtrue
  bool m_pause;     // 一時停止中ならtrue

  State m_state;
  u32   m_stateCount;
  s32   m_index;      // m_dispPokeDataIndexListのインデックス。今は(m_index-1)まで描いてあり、次にm_indexに描く。
  s32   m_zeroIndex;  // m_dispPokeDataIndexListのインデックス何番がm_pokeIconPartsInfo[0][0]に描かれているか
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANSEARCHPOKEICONDISPLAY_H_INCLUDED

