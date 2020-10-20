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

#if !defined __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWBASE_H_INCLUDED__
#define __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWBASE_H_INCLUDED__
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_std_string.h>

// nijiのインクルード
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowDataManager.h"



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
  class InfoWindowDataManager;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class  InfoWindowBase
 @brief  インフォウィンドウ　ベース
 */
class InfoWindowBase : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(InfoWindowBase);

  friend class InfoWindowDataManager;

public:
  InfoWindowBase(app::util::Heap* appHeap);
  virtual ~InfoWindowBase();

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
  virtual bool IsOpen(void) const = 0;

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
  virtual bool IsDisplay(void) const = 0;

  //! @brief  クローズアニメなしで直ちにクローズを開始する
  //! @note  クローズの完了はInfoWindowBookTypeListener::OnCloseで検知するか、IsOpenで判定して下さい。
  //! @note  「直ちに」といってもこの関数を呼び出したフレームではクローズできないので、必ず上記方法にてクローズ確認をして下さい。
  virtual void StartCloseImmediately(void) = 0;

  //! @brief  手動クローズ
  //! @note  本当に「直ちに」クローズします。この関数を呼び出したフレームでクローズします。
  //! @note  他のクローズは安全にクローズし破棄処理に移行できるようにしていますが、
  //         このクローズは安全性を呼び出し元に保証してもらわなければなりません。
  //         従って、使用許可をもらったところでしか使用しないで下さい。
  //! @note  この後破棄する場合は、呼び出し元で1フレーム待ってから破棄して下さい。これで安全性が保証されます。
  virtual void CloseManually(void) = 0;

  //! @brief  背景の表示/非表示を設定する(デフォルト非表示)
  //! @param[in]  visible  trueのとき表示、falseのとき非表示
  void SetBgVisible(bool visible);
  //! @brief  背景が表示されるか
  //! @return  trueのとき表示、falseのとき非表示
  bool IsBgVisible(void) const;

  //! @brief  SEのオン/オフを設定する(デフォルトオン)
  //! @param[in]  isOn  trueのときSEあり、falseのときSEなし
  virtual void SetSEOn(bool isOn);
  //! @brief  SEがオンか
  //! @return  trueのときSEあり、falseのときSEなし
  bool IsSEOn(void) const;

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
  virtual InfoWindowDataManager::CreateType GetDataManagerSelfCreateType(void) const = 0;
  //@}

protected:
  virtual void destructa(void);

private:
  enum CreateState
  {
    CREATE_STATE_NONE,
    CREATE_STATE_DATA_MANAGER_CREATING,
    CREATE_STATE_LAYOUT_CREATING,
    CREATE_STATE_CREATED,
  };

  void startAsyncCreateDataManager(InfoWindowDataManager* dataManager);
  void syncCreateDataManager(InfoWindowDataManager* dataManager);
  void destroyDataManager(void);

  virtual void createLayout(void) = 0;
  virtual void destroyLayout(void) = 0;

  void localUpdate(void);
  void createLayoutFromCreatingToCreated(void);

protected:
  // Createの引数
  app::util::Heap*                 m_appHeap;  // 「StartAsyncCreateが呼ばれたとき」から「IsCreatedがtrueを返すようになるとき」まで保持しておく。その後はNULLにしておく。
  app::util::AppRenderingManager*  m_appRenderingManager;
  InfoWindowDataManager*           m_dataManager;
  
  // 内部変数
  bool                             m_isEndFuncCalled;  // EndFuncが呼び出されていたらtrue
  CreateState                      m_createState;
  bool                             m_isDataManagerSelfCreated;   // m_dataManagerを内部で生成したらtrue、外部からもらっていたらfalse
  gfl2::lyt::LytPane*              m_openStartPosPane;  // オープン開始位置を設定するペイン
  gfl2::lyt::LytPane*              m_openEndPosPane;    // オープン終了位置を設定するペイン(m_openStartPaneの子ペインなので、座標もm_openStartPane内の座標で指定する)
  gfl2::lyt::LytPane*              m_bgPane;            // 背景ペイン
  bool                             m_bgVisible;
  bool                             m_SEIsOn;
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif // __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWBASE_H_INCLUDED__

