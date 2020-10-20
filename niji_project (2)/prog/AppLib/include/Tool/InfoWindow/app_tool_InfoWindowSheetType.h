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

#if !defined __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWSHEETTYPE_H_INCLUDED__
#define __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWSHEETTYPE_H_INCLUDED__
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

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  class InfoWindowSheetTypeListener;
  class InfoWindowSheetTypeLocalListener;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class  InfoWindowSheetType
 @brief  インフォウィンドウ　シートタイプ　(sml)
 */
class InfoWindowSheetType : public InfoWindowBase
{
  GFL_FORBID_COPY_AND_ASSIGN(InfoWindowSheetType);
  
  friend class InfoWindowDataManager;
  friend class InfoWindowSheetTypeLocalListener;

public:
  InfoWindowSheetType(app::util::Heap* appHeap);
  virtual ~InfoWindowSheetType();

  //! @brief  終了処理
  //! @note  trueを返すまで必ず呼んで下さい。毎フレーム呼んで下さい。trueを返したら破棄できます。
  virtual bool EndFunc(void);
 
  //! @brief  非同期生成　開始
  //! @param[in]  appHeap  ヒープ。m_pDevHeap, m_pDevHeapAllocatorしか使わない。完了するまでに随時メモリを確保する。一時的に後方確保することもある。
  //! @param[in]  appRenderingManager  レンダリングマネージャ。
  //! @param[in]  dataManager  データマネージャ。NULL可。NULLのときは内部でデータマネージャを生成する。
  //! @note  親から呼ばれるUpdateが毎フレーム回っていないと非同期処理は進みません。
  void StartAsyncCreate(
      app::util::Heap* appHeap,
      app::util::AppRenderingManager* appRenderingManager,
      InfoWindowDataManager* dataManager 
  );
  //! @brief  同期生成
  //! @param[in]  appHeap  ヒープ。m_pDevHeap, m_pDevHeapAllocatorしか使わない。完了するまでに随時メモリを確保する。一時的に後方確保することもある。
  //! @param[in]  appRenderingManager  レンダリングマネージャ。
  //! @param[in]  dataManager  データマネージャ。NULL可。NULLのときは内部でデータマネージャを生成する。
  void SyncCreate(
      app::util::Heap* appHeap,
      app::util::AppRenderingManager* appRenderingManager,
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

  //! @brief  オープン開始
  //! @param[in]  listener     リスナー。クローズが完了するまで呼び出し元は破棄しないようにして下さい。NULLでもOK。
  //! @param[in]  pos          オープンを開始する位置。レイアウトの座標(画面の中心が(0,0,0)、下画面なら右下が(160,-120,0))。
  //! @param[in]  titleStrBuf  タイトル。クローズが完了するまで呼び出し元は破棄しないようにして下さい。
  //! @param[in]  textStrBuf   本文。クローズが完了するまで呼び出し元は破棄しないようにして下さい。
  //! @note  クローズの完了はInfoWindowSheetTypeListener::OnCloseで検知するか、IsOpenで判定して下さい。
  void StartOpen(
      InfoWindowSheetTypeListener*  listener,
      const gfl2::math::Vector3&    pos,
      const gfl2::str::StrBuf*      titleStrBuf,
      const gfl2::str::StrBuf*      textStrBuf
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
    return InfoWindowDataManager::CREATE_TYPE_SHEET;
  }
  //@}

  //! @name  friendクラスInfoWindowSheetTypeLocalListenerからアクセスするメンバ
  //@{
private:
  void ReqClose(void);
  //@}


private:
  virtual void createLayout(void);
  virtual void destroyLayout(void);

private:
  static const u32 TEXT_LINE_NUM_MIN = 1;  // MIN<= <=MAX
  static const u32 TEXT_LINE_NUM_MAX = 5;

  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,                    //<! 表示していない
    DISP_STATE_WAIT_ONE_FRAME_BEFORE,  //<! 表示する前に1フレーム待っている最中
    DISP_STATE_ON_OPEN,                //<! 表示している(オープン中)
    DISP_STATE_ON_WORK,                //<! 表示している(稼動中)
    DISP_STATE_WAIT_ONE_FRAME_AFTER,   //<! 表示をやめて1フレーム待っている最中
  };
  void changeDispState(DispState);

  // Update処理の一部をまとめた関数
  void updateCloseReq(void);
  void updateDispStateWaitOneFrameAfter(void);

private:
  // Openの引数
  InfoWindowSheetTypeListener*  m_listener;
  gfl2::math::Vector3           m_pos;
  const gfl2::str::StrBuf*      m_titleStrBuf;
  const gfl2::str::StrBuf*      m_textStrBuf;

  // 内部変数
  InfoWindowSheetTypeLocalListener*  m_localListener;
  DispState                          m_dispState;
  u32                                m_dispStateCount;
  bool                               m_openReq;    // trueのときオープンリクエストが出ている
  bool                               m_closeReq;   // trueのときクローズリクエストが出ている
  gfl2::lyt::LytTextBox*             m_titlePane;
  gfl2::lyt::LytTextBox*             m_textPane;
  u32                                m_textLineNum;
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif // __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWSHEETTYPE_H_INCLUDED__

