//======================================================================
/**
 * @file app_tool_InfoWindow.h
 * @date 2015/07/02 15:47:18
 * @author kawada_koji
 * @brief インフォウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWBOOKTYPE_H_INCLUDED__
#define __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWBOOKTYPE_H_INCLUDED__
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_std_string.h>

// nijiのインクルード
#include "AppLib/include/ui/UIView.h"
#include "AppLib/source/Tool/InfoWindow/app_tool_InfoWindowBase.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class StrBuf;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)
  class WordSet;
GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  class InfoWindowBookTypeListener;
  class InfoWindowBookTypeLocalListener;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class  InfoWindowBookType
 @brief  インフォウィンドウ　ブックタイプ　(lrg)
 */
class InfoWindowBookType : public InfoWindowBase
{
  GFL_FORBID_COPY_AND_ASSIGN(InfoWindowBookType);
  
  friend class InfoWindowDataManager;
  friend class InfoWindowBookTypeLocalListener;

public:
  //! @brief  オープンの種類
  enum OpenType
  {
    OPEN_TYPE_SCALE_ANIM,  //!< スケールアニメありオープン
    OPEN_TYPE_NO_ANIM,     //!< アニメなしオープン(すぐオープン)
  };

public:
  InfoWindowBookType(app::util::Heap* appHeap);
  virtual ~InfoWindowBookType();

  //! @brief  終了処理
  //! @note  trueを返すまで必ず呼んで下さい。毎フレーム呼んで下さい。trueを返したら破棄できます。
  virtual bool EndFunc(void);

  //! @brief  非同期生成　開始
  //! @param[in]  appHeap  ヒープ。m_pDevHeap, m_pDevHeapAllocatorしか使わない。完了するまでに随時メモリを確保する。一時的に後方確保することもある。
  //! @param[in]  appRenderingManager  レンダリングマネージャ。
  //! @param[in]  pageNumMax  ページ数の最大値
  //! @param[in]  dataManager  データマネージャ。NULL可。NULLのときは内部でデータマネージャを生成する。
  //! @note  親から呼ばれるUpdateが毎フレーム回っていないと非同期処理は進みません。
  void StartAsyncCreate(
      app::util::Heap* appHeap,
      app::util::AppRenderingManager* appRenderingManager,
      u32 pageNumMax,
      InfoWindowDataManager* dataManager
  );
  //! @brief  同期生成
  //! @param[in]  appHeap  ヒープ。m_pDevHeap, m_pDevHeapAllocatorしか使わない。完了するまでに随時メモリを確保する。一時的に後方確保することもある。
  //! @param[in]  appRenderingManager  レンダリングマネージャ。
  //! @param[in]  pageNumMax  ページ数の最大値
  //! @param[in]  dataManager  データマネージャ。NULL可。NULLのときは内部でデータマネージャを生成する。
  void SyncCreate(
      app::util::Heap* appHeap,
      app::util::AppRenderingManager* appRenderingManager,
      u32 pageNumMax,
      InfoWindowDataManager* dataManager
  );
   //! @brief  生成が完了しているか
  //! @return  生成が完了したらtrueを返す。
  //!          生成を開始していないときはfalseを返す。生成中はfalseを返す。
  //! @note  親から呼ばれるUpdateが毎フレーム回っていないと非同期処理は進みません。
  bool IsCreated(void) const;

  //! @brief  オープン中か
  //! @return  オープン中ならtrueを返す。
  //! @note  クローズの完了をこれで判定してもいいです。
  virtual bool IsOpen(void) const;

  //! @brief  表示中か
  //! @return  表示中ならtrueを返す。
  //! @caution  IsDisplayをオープン中か否かの判定に使わないで下さい。IsOpenを使うかリスナーのOnCloseで検知して下さい。
  //! @caution  IsDisplayは特別な場合にしか使わないで下さい。通常はIsOpenを使うかリスナーのOnCloseで検知して下さい。
  //! @note  IsOpenとIsDisplayの違いに注意して下さい。
  //         フレーム  状況                  IsOpen  IsDisplay  状態
  //         --------------------------------------------------------------------------------------------------------------------
  //         t0        オープン前            false   false      何も表示されていない
  //         t1        StartOpen             true    false      何も表示されていないがオープン扱いになる
  //         t2                              true    true       オープン演出中の絵が表示される(今のところStartOpenの1フレーム後)
  //         t3                              true    true       完全にオープンし切った状態の絵が表示される
  //         t4        クローズボタンを押す  true    true       クローズ演出中の絵が表示される(今のところクローズ演出はない)
  //         t5                              true    false      何も表示されていないがオープン扱いになる
  //         t6        クローズ後            false   false      何も表示されていない(今のところ何も表示されなくなって1フレーム後)
  virtual bool IsDisplay(void) const;

  //! @brief  クローズアニメなしで直ちにクローズを開始する
  //! @note  クローズの完了はInfoWindowBookTypeListener::OnCloseで検知するか、IsOpenで判定して下さい。
  //! @note  「直ちに」といってもこの関数を呼び出したフレームではクローズできないので、必ず上記方法にてクローズ確認をして下さい。
  virtual void StartCloseImmediately(void);

  //! @brief  手動クローズ
  //! @note  本当に「直ちに」クローズします。この関数を呼び出したフレームでクローズします。
  //! @note  他のクローズは安全にクローズし破棄処理に移行できるようにしていますが、
  //         このクローズは安全性を呼び出し元に保証してもらわなければなりません。
  //         従って、使用許可をもらったところでしか使用しないで下さい。
  //! @note  この後破棄する場合は、呼び出し元で1フレーム待ってから破棄して下さい。これで安全性が保証されます。
  virtual void CloseManually(void);

  //! @brief  SEのオン/オフを設定する(デフォルトオン)
  //! @param[in]  isOn  trueのときSEあり、falseのときSEなし
  virtual void SetSEOn(bool isOn);

  //! @brief  コマンドボタン(Aボタン対応のボタン)をアクティブ/パッシブにする(デフォルトアクティブ)
  //! @param[in]  isActive  trueのときアクティブ、falseのときパッシブ。
  //! @note  SetXxxContent関数で渡すcommandStrBufがNULLでないかNULLかはコマンドボタンの表示/非表示(非表示のときは入力無効)に関係します。
  //         SetCommandButtonActiveで設定した値は、ページが変わってもコマンドボタンの表示/非表示が変わっても使われ続けます。
  void SetCommandButtonActive(bool isActive);

  //! @brief  オープン開始
  //! @param[in]  listener  リスナー。クローズが完了するまで呼び出し元は破棄しないようにして下さい。NULLでもOK。
  //! @param[in]  pos       オープンを開始する位置。レイアウトの座標(画面の中心が(0,0,0)、下画面なら右下が(160,-120,0))。
  //! @param[in]  openType  オープンの種類
  //! @note  クローズの完了はInfoWindowBookTypeListener::OnCloseで検知するか、IsOpenで判定して下さい。
  void StartOpen(
      InfoWindowBookTypeListener*   listener,
      const gfl2::math::Vector3&    pos,
      OpenType                      openType = OPEN_TYPE_SCALE_ANIM
  );
  
  //! @brief  クローズ開始
  //! @note  クローズの完了はInfoWindowBookTypeListener::OnCloseで検知するか、IsOpenで判定して下さい。
  void StartClose(void);

  //! @brief  コンテンツをリセットする
  //! @param[in]  pageNum   これからセットするコンテンツのページ数。pageNumMax以下にして下さい。
  //! @param[in]  keepFlag  ページが変わるときに直前に表示していたコンテンツをクリアするときfalse、
  //!                       クリアせず保持するときtrue。trueのときはStartOpen前に全ページを設定しておいて下さい。
  void ResetContents(u32 pageNum, bool keepFlag=false);
  
  //! @brief  コンテンツ設定
  //! @param[in]  pageNo         0スタート(「表示上の1ページ目」が「pageNo=0」となる)。ResetContentsで設定したpageNum以下にして下さい。
  //! @param[in]  textStrBuf     本文。ページが変わるまで呼び出し元は破棄しないようにして下さい。
  //! @param[in]  textureInfo    テクスチャ。ページが変わるまで呼び出し元は破棄しないようにして下さい。NULL可。NULLのときはピクチャペインを表示しません。
  //!                            lytdatにテクスチャを含めたとき: LytResTextureInfoはgfl2::lyt::LytResクラスのGetTexture関数でlytdatから取得できます。
  //!                            bflimファイルを直接読み込んだとき: LytResTextureInfoはnw::lyt::LoadTexture関数にbflimファイルを読み込んだバッファを渡すことでセットアップできます。
  //! @param[in]  qrTextureInfo  QRコードのテクスチャ。ページが変わるまで呼び出し元は破棄しないようにして下さい。
  //! @param[in]  commandStrBuf  コマンド。Aボタンのところに表示する文字列。NULLのときは表示しない。ページが変わるまで呼び出し元は破棄しないようにして下さい。
  void SetTextContent(
      u32                      pageNo,
      const gfl2::str::StrBuf* textStrBuf,
      const gfl2::str::StrBuf* commandStrBuf
  );
  void SetTextureAndTextContent(
      u32                                  pageNo,
      const gfl2::lyt::LytResTextureInfo*  textureInfo,
      const gfl2::str::StrBuf*             textStrBuf,
      const gfl2::str::StrBuf*             commandStrBuf
  );
  void SetQRContent(
      u32                                  pageNo,
      const gfl2::lyt::LytResTextureInfo*  qrTextureInfo,
      const gfl2::str::StrBuf*             commandStrBuf
  );


public:
  //! @brief  親から呼ばれるUpdate
  //! @note  親子関係を持たず単独利用なら直接呼んでも問題ないです。
  virtual void Update(void);

  //! @brief  親から呼ばれるDraw
  //! @note  親子関係を持たず単独利用なら直接呼んでも問題ないです。
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);  // ←は内部で u8 order = gfl2::lyt::ORDER_NORMAL, u32 layerNo = 0 という指定をしています。
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo, u8 order, u32 layerNo);


  //! @name  friendクラスInfoWindowDataManagerからアクセスするメンバ
  //@{
private:
  virtual InfoWindowDataManager::CreateType GetDataManagerSelfCreateType(void) const
  {
    return InfoWindowDataManager::CREATE_TYPE_BOOK;
  }
  //@}

  //! @name  friendクラスInfoWindowSheetBookTypeLocalListenerからアクセスするメンバ
  //@{
private:
  enum
  {
    BUTTON_ID_B,
    BUTTON_ID_A,
    BUTTON_ID_L,
    BUTTON_ID_R,
    BUTTON_ID_NUM
  };
  
  void ReqClose(void);
  void ReqCommand(void);
  void ReqTouchTrigger(u16 x, u16 y);
  void ReqTouchHold(u16 x, u16 y);
  void ReqTouchRelease(u16 x, u16 y);
  void ReqGoToPrevPage(void);
  void ReqGoToNextPage(void);
  //@}


private:
  virtual void createLayout(void);
  virtual void destroyLayout(void);

private:
  // フレーム(枠付き1ページのことをフレームと呼ぶことにする)
  enum FramePos
  {
    FRAME_POS_L,   // 前のページ
    FRAME_POS_C,   // 現在のページ
    FRAME_POS_R,   // 次のページ
    FRAME_POS_NUM  // フレームの配置場所の個数
  };
  enum FrameContent
  {
    FRAME_CONTENT_FRONT,  // 現在のページ
    FRAME_CONTENT_BACK,   // 前か次のページ
    FRAME_CONTENT_NUM     // フレームの中身の個数
  };
  class FrameContentProperty
  {
  public:
    gfl2::lyt::LytPane*    frameTopPane;
    gfl2::lyt::LytPane*    contentTopPane;
    gfl2::lyt::LytPane*    contentVisiblePane;
    gfl2::lyt::LytTextBox* pageNoTextPane;   // aa/bbのaa
    gfl2::lyt::LytTextBox* pageNumTextPane;  // aa/bbのbb
    gfl2::lyt::LytTextBox* slashTextPane;    // aa/bbの/
    gfl2::lyt::LytTextBox* textPane;
    gfl2::lyt::LytPicture* texturePane;
    gfl2::lyt::LytPicture* qrTexturePane;
    u32                    animNoText;
    u32                    animNoTextureAndText;
    u32                    animNoQR;            
  public:
    FrameContentProperty(void)
      : frameTopPane(NULL),
        contentTopPane(NULL),
        contentVisiblePane(NULL),
        pageNoTextPane(NULL),
        pageNumTextPane(NULL),
        slashTextPane(NULL),
        textPane(NULL),
        texturePane(NULL),
        qrTexturePane(NULL),
        animNoText(0),
        animNoTextureAndText(0),
        animNoQR(0)
    {}
  };

  // コンテンツ
  class Content
  {
  public:
    struct TextContent
    {
      const gfl2::str::StrBuf* textStrBuf;
      const gfl2::str::StrBuf* commandStrBuf;
    };
    struct TextureAndTextContent
    {
      const gfl2::lyt::LytResTextureInfo*  textureInfo;
      const gfl2::str::StrBuf*             textStrBuf;
      const gfl2::str::StrBuf*             commandStrBuf;
    };
    struct QRContent
    {
      const gfl2::lyt::LytResTextureInfo*  qrTextureInfo;
      const gfl2::str::StrBuf*             commandStrBuf;
    };

    // 初期化用の最大サイズのコンテンツ
    struct InitContent
    {
      static const u32 FOUR_BYTE_BUF_SIZE = 3;  // 4バイトのバッファの個数
      u32 buf[FOUR_BYTE_BUF_SIZE];
    };

    // 共用体
    union ContentUnion
    {
      TextContent           textContent;
      TextureAndTextContent textureAndTextContent;
      QRContent             qrContent;
      InitContent           initContent;
    };

    // コンテンツのタイプ
    enum ContentType
    {
      CONTENT_TYPE_NONE,
      CONTENT_TYPE_TEXT,
      CONTENT_TYPE_TEXTURE_AND_TEXT,
      CONTENT_TYPE_QR,
    };

  public:
    ContentType   contentType;
    ContentUnion  contentUnion;
  public:
    Content(void)
    {
      contentType = CONTENT_TYPE_NONE;
      gfl2::std::MemFill(&contentUnion.initContent, 0, sizeof(InitContent));
    }
  };
  void setupContent(bool noneIsOk, FramePos framePos);  // セットアップ時にコンテンツがなくてもOKのときはnoneIsOkをtrueにする。
  void setNumberToPageTextPane(gfl2::lyt::LytTextBox* textPane, u32 number);
  void resetContentsExceptCurrPage(void);  // 現在のページ(m_pageNo)以外のコンテンツをリセットする。

  // 共通オブジェクト(フレーム外にあり1個分しか領域がないものを共通オブジェクトと呼ぶことにする)
  void setupCommonObj(void);
  bool setStringToCommandTextPane(const gfl2::str::StrBuf* commandStrBuf);  // コマンドのボタンを表示するときはtrueを返す(commandStrBufがNULLのとき表示するのでtrueを返す)
  void setButtonInputEnableAndVisible(u32 buttonId, bool enable);
 
  // フレームとコンテンツの接続
  void connectFrameContent(FramePos framePos, FrameContent frameContent);  // framePosにframeContentを接続する
  void swapFrameContent(void);  // FRAME_CONTENT_BACKの内容とFRAME_CONTENT_FRONTの内容を入れ替える
  void initFrameContent(FrameContent frameContent);  // FRAME_CONTENT_...の内容を空にする

  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,                    //<! 表示していない
    DISP_STATE_WAIT_ONE_FRAME_BEFORE,  //<! 表示する前に1フレーム待っている最中
    DISP_STATE_ON_OPEN,                //<! 表示している(オープン中)
    DISP_STATE_ON_WORK,                //<! 表示している(稼動中)
    DISP_STATE_ON_TOUCH,               //<! 表示している(タッチ操作中)
    DISP_STATE_ON_GO_TO_CURR_PAGE,     //<! 表示している(今のページが真ん中に来るように移動中)
    DISP_STATE_ON_GO_TO_PREV_PAGE,     //<! 表示している(前のページへ移動中)
    DISP_STATE_ON_GO_TO_NEXT_PAGE,     //<! 表示している(後のページへ移動中)
    DISP_STATE_WAIT_ONE_FRAME_AFTER,   //<! 表示をやめて1フレーム待っている最中
  };
  void changeDispState(DispState);

  // Update処理の一部をまとめた関数
  void updateCloseReq(void);
  void updateDispStateWaitOneFrameAfter(void);

  // クローズ後にレイアウトを初期化する(生成し直すことなく使い回してもうまくいくように)
  void initLayoutAfterClose(void);

  // チェック関数 
  bool areAllPageContentsSet(void) const;  // コンテンツが全ページ分設定されているか(ページ数が0のときはfalseを返す)
  bool canGoToPrevPage(void) const;  // 前のページに移動できるか
  bool canGoToNextPage(void) const;  // 次のページに移動できるか

  // タッチ操作
  struct TouchVector
  {
    u16 x;
    u16 y;
  };
  enum
  {
    TOUCH_POS_NUM_MAX  = 10
  };
  enum TouchMoveDir
  {
    TOUCH_MOVE_DIR_STOP,   // 静止
    TOUCH_MOVE_DIR_LEFT,   // 左方向へ動く
    TOUCH_MOVE_DIR_RIGHT,  // 右方向へ動く
  };
  void setTouchTrigger(u16 x, u16 y);
  TouchMoveDir calcTouchMoveDir(void) const;
  FramePos calcFrameRootPanePosSection(void) const;  // m_frameRootPaneが画面の左が中央か右のどのあたりにいるか

  // SE
  void setButtonManagerSEOn(void);

  // フラグ反映
  void updateCommandButtonActive(void);

private:
  // 代入するまで保持しておくCreateの引数  // 代入し終わったら未使用値に戻しておく。
  u32      m_argPageNumMax;

  // Createの引数
  u32      m_pageNumMax;

  // Openの引数
  InfoWindowBookTypeListener*  m_listener;
  gfl2::math::Vector3          m_pos;

  // コンテンツ
  Content* m_contents;  // 配列の個数はm_pageNumMax  // 0スタート(「表示上の1ページ目」が「m_contents[0]」となる)。
  u32      m_pageNum;   // pageNumMax以下
  u32      m_pageNo;    // 0スタート(「表示上の1ページ目」が「pageNo=0」となる)。
  bool     m_keepFlag;  // ページが変わるときに直前に表示していたコンテンツをクリアするときfalse、
                        // クリアせず保持するときtrue。

  // フラグ
  bool     m_commandButtonIsActive;

  // 内部変数
  InfoWindowBookTypeLocalListener*  m_localListener;
  DispState                         m_dispState;
  u32                               m_dispStateCount;
  bool                              m_openReq;    // trueのときオープンリクエストが出ている
  bool                              m_closeReq;   // trueのときクローズリクエストが出ている
  OpenType                          m_openType;
  u32                               m_openAnim;
  print::WordSet*                   m_wordSet;
  gfl2::lyt::LytTextBox*            m_commandPane;
  gfl2::lyt::LytPane*               m_frameRootPane;
  gfl2::math::Vector3               m_frameRootFixPos;
  gfl2::lyt::LytPane*               m_framePosPane[FRAME_POS_NUM];
  gfl2::math::Vector3               m_framePosFixPos[FRAME_POS_NUM];  // 画面にちょうどおさまるオリジナル位置
  FrameContentProperty              m_frameContentProperty[FRAME_CONTENT_NUM];

  // タッチ操作関連の変数↓
  TouchVector                       m_touchTriggerPos;
  TouchVector                       m_touchPos[TOUCH_POS_NUM_MAX];  // 0=最新; 1=１つ前; ...; TOUCH_POS_NUM_MAX-1=記憶している中で最も古い;
  s32                               m_touchPosNum;  // TOUCH_POS_NUM_MAXまでしかカウントしない
  bool                              m_setupWhenGoToPageByTouchFlag;  // タッチをリリースしたときの前次ページへの移動中で
                                                                     // セットアップが必要かもしれないならtrueにしておく。
  // タッチ操作関連の変数↑

  bool  m_touchNow;  // タッチ中はtrue
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif // __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWBOOKTYPE_H_INCLUDED__

