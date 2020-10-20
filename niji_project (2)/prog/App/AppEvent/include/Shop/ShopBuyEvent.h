//=============================================================================
/**
 * @file    ShopBuyEvent.h
 * @brief   ショップ購入画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.10.02
 */
//=============================================================================
#if !defined( SHOP_BUY_EVENT_H_INCLUDED )
#define SHOP_BUY_EVENT_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "App/Shop/include/ShopAppParam.h"
#include "AppLib/include/Util/app_util_Heap.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Shop)
class ShopFrame;
GFL_NAMESPACE_END(Shop)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
 * @class ShopBuyEvent
 * @brief ショップ購入画面イベントクラス
 * @date  2015.10.02
 */
//=============================================================================
class ShopBuyEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(ShopBuyEvent);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  ShopBuyEvent( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ShopBuyEvent();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2015.10.02
   *
   * @param   man     ゲームマネージャクラス
   *
   * @return  ShopBuyEventクラス
   */
  //-----------------------------------------------------------------------------
  static ShopBuyEvent * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupParam
   * @brief   パラメータ設定
   * @date    2015.10.02
   *
   * @param   type              ショップタイプ
   * @param   id                ショップID
   * @param   howmany_msg       メッセージ：いくつ買う？
   * @param   really_msg        メッセージ：xxコでxx円です
   * @param   really_msg2       メッセージ：xxコでxx円です（複数形）
   * @param   hereitis_msg      メッセージ：まいど
   * @param   youcanttake_msg   メッセージ：持ちきれない
   * @param   nomoney_msg       メッセージ：お金が足りない
   * @param   selected_msg      メッセージ：今日は終了しました（フェスショップ用）
   */
  //-----------------------------------------------------------------------------
  void SetupParam(
        App::Shop::ShopType type,
        u32 id,
        u32 howmany_msg,
        u32 really_msg,
        u32 really_msg2,
        u32 hereitis_msg,
        u32 youcanttake_msg,
        u32 nomoney_msg,
        u32 selected_msg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupParamForFC
   * @brief   パラメータ設定(FesCircle用)
   * @date    2015.10.02
   *
   * @param   type              ショップタイプ
   * @param   id                ショップID
   * @param   howmany_msg       メッセージ：いくつ買う？
   * @param   really_msg        メッセージ：xxコでxx円です
   * @param   really_msg2       メッセージ：xxコでxx円です（複数形）
   * @param   hereitis_msg      メッセージ：まいど
   * @param   youcanttake_msg   メッセージ：持ちきれない
   * @param   nomoney_msg       メッセージ：お金が足りない
   * @param   selected_msg      メッセージ：今日は終了しました（フェスショップ用）
   * @param   pShopFeatureDeleagte  Shop機能委譲
   */
  //-----------------------------------------------------------------------------
  void SetupParamForFC(
        App::Shop::ShopType type,
        u32 id,
        u32 howmany_msg,
        u32 really_msg,
        u32 really_msg2,
        u32 hereitis_msg,
        u32 youcanttake_msg,
        u32 nomoney_msg,
        u32 selected_msg,
        App::Shop::IShopFeatureDelegate* pShopFeatureDeleagte
  );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupHeap
   * @brief   Procが使用するヒープをセット
   * @date    2015.10.10
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void SetupHeap( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @date    2015.10.02
   *
   * @param   gmgr  ゲームマネージャークラス
   *
   * @retval  true  = 起動してよい
   * @retval  false = 起動しない
   */
  //-----------------------------------------------------------------------------
  virtual bool BootChk( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @date    2015.10.02
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.10.02
   *
   * @param   gmgr  ゲームマネージャークラス
   *
   * @return  イベント制御関数の戻り値
   */
  //-----------------------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.10.02
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    PreDrawFunc
   * @brief   描画下準備
   * @date    2015.10.22
   */
  //-----------------------------------------------------------------------------
  void PreDrawFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   描画処理
   * @date    2015.10.22
   *
   * @param   displayNo   描画対象画面
   */
  //-----------------------------------------------------------------------------
  void DrawFunc( gfl2::gfx::CtrDisplayNo displayNo );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    InitFrame
   * @brief   フレーム初期化
   * @date    2015.10.22
   *
   * @retval  true  = 処理中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool InitFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MainFrame
   * @brief   フレームメイン処理
   * @date    2015.10.22
   *
   * @retval  true  = 処理中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool MainFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFrame
   * @brief   フレーム終了処理
   * @date    2015.10.22
   *
   * @retval  true  = 処理中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool EndFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateHeap
   * @brief   ヒープ生成
   * @date    2015.10.22
   */
  //-----------------------------------------------------------------------------
  void CreateHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteHeap
   * @brief   ヒープ削除
   * @date    2015.10.22
   */
  //-----------------------------------------------------------------------------
  void DeleteHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRenderManager
   * @brief   描画マネージャ生成
   * @date    2015.10.22
   */
  //-----------------------------------------------------------------------------
  void CreateRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRenderManager
   * @brief   描画マネージャ削除
   * @date    2015.10.22
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateFrame
   * @brief   フレーム生成
   * @date    2015.10.22
   */
  //-----------------------------------------------------------------------------
  void CreateFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteFrame
   * @brief   フレーム削除
   * @date    2015.10.22
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupFieldMap
   * @brief   イベントをフィールドマップにセット
   * @date    2015.10.22
   *
   * @param   p   イベントのポインタ
   */
  //-----------------------------------------------------------------------------
  void SetupFieldMap( ShopBuyEvent * p );


private:
  App::Shop::APP_PARAM m_appParam;    //!< バッグ画面の外部設定パラメータ
  gfl2::heap::HeapBase * m_parentHeap;  //!< 親ヒープ

  gfl2::heap::HeapBase * m_pManagerHeap;    //!< マネージャ用ヒープ

  app::util::Heap m_heap;   //!< ローカルヒープ

  gfl2::ro::Module * m_roShopMobule;  //!< フレームDLL

  app::util::AppRenderingManager * m_pRenderingManager;   //!< 描画マネージャ
  applib::frame::Manager * m_pFrameManager;               //!< フレームマネージャ

  App::Shop::ShopFrame * m_pFrame;  //!< フレーム

  bool m_isDrawEnable;  //!< 描画フラグ
  u32 m_frameSeq;       //!< フレーム生成関連のシーケンス
};


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif  // SHOP_BUY_EVENT_H_INCLUDED
