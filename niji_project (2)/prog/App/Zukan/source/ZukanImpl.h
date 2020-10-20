#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANIMPL_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANIMPL_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanImpl.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑の実装
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>
#include <AppLib/include/Tool/RotomToneGenerator/app_tool_RotomToneGenerator.h>

// 図鑑のインクルード
#include "ZukanType.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(g2d)
GFL_NAMESPACE_BEGIN(lyt)
  class LytSys;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(g2d)

GFL_NAMESPACE_BEGIN(System)
  class nijiAllocator;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanProcParam;
  class ZukanDataManager;
  class ZukanPokePicManager;
  class ZukanFrameListener;
  class ZukanFrameCell;
  class ZukanView;
  class ZukanTool;
  class ZukanContents;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDebugWin;
  class ZukanSaveDataDebugWin;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanImpl
 @brief     図鑑の実装
 */
//==============================================================================
class ZukanImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanImpl);

public:
  //! @brief  コンストラクタ
  //! @param[in]  param        paramのポインタを保持するだけ。
  //!                          呼び出し元はparamを破棄しないように。
  //! @param[in]  appHeap      使ってもいいヒープ。
  //! @param[in]  gameManager  ゲームマネージャ。
  //! @param[in]  debugInfo    ZukanDebugInfo
  ZukanImpl(ZukanProcParam* param, app::util::Heap* appHeap, GameSys::GameManager* gameManager, void* debugInfo=NULL);

  virtual ~ZukanImpl();

  gfl2::proc::Result InitFunc(void);
  gfl2::proc::Result UpdateFunc(void);
  void PreDrawFunc(void);
  void DrawFunc(gfl2::gfx::CtrDisplayNo displayNo);
  gfl2::proc::Result EndFunc(void);
private:
  void updateSys(void);
  void changeSeq(s32 seq);
  gfl2::proc::Result finishFunc(void);

#if PM_DEBUG
  void dumpMem(void);
#endif

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanProcParam*       m_param;
  app::util::Heap*      m_appHeap;  // implAppHeap
  GameSys::GameManager* m_gameManager;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // メモリ
  gfl2::heap::HeapBase*  m_implInstanceDevMem;
  gfl2::heap::HeapBase*  m_dataDevMem;
  gfl2::heap::HeapBase*  m_frameManagerDevMem;
  gfl2::heap::HeapBase*  m_vramAllocatorDevMem;
  gfl2::heap::HeapBase*  m_contentsDevMem;
  app::util::Heap*       m_commonAppHeap;
  gfl2::heap::HeapBase*  m_pokeModelCompDevMem;
  gfl2::heap::HeapBase*  m_langMsgDataDevMem;
  gfl2::heap::HeapBase*  m_langFontDevMem;
  app::util::Heap*       m_instanceAppHeap;
  gfl2::heap::HeapBase*  m_debugDevMem;
  
  // VRAMアロケータ
  System::nijiAllocator* m_nijiAllocatorForEdgeMapTexture;
  
  // フェード色
  gfl2::math::Vector4  m_fadeColor;

  // シーケンス制御
  s32  m_seq;
  s32  m_seqCount;

  // データマネージャ
  ZukanDataManager*        m_dataManager;

  // 図鑑の絵巻のポケモン画像のマネージャ
  ZukanPokePicManager* m_pokePicManager;

  // コンテンツ
  ZukanContents*  m_contents;

  // フレーム
  applib::frame::Manager*  m_frameManager;
  ZukanFrameListener*      m_frameListener;
  ZukanFrameCell*          m_frameCell;
  bool                     m_frameCreated;     // trueのときフレームが生成された
  bool                     m_frameReqDestroy;  // trueのときフレームが破棄をリクエストしている
  bool                     m_frameDestroying;  // trueのときフレームが破棄されている最中
  
  // レンダリングマネージャ
  app::util::AppRenderingManager*  m_appRenderingManager;

  // レイアウト
  gfl2::lyt::LytSys*  m_lytSys;

  // フレーム外で更新、描画するもの
  ZukanView*  m_view;  // rootView
  ZukanTool*  m_tool;

  // セットアップ状況
  ZukanType::SetupState  m_setupState;

  // 図鑑のデバッグウィンドウ
  ZukanDebugWin*  m_zukanDebugWin;
  ZukanSaveDataDebugWin*  m_zukanSaveDataDebugWin;
  // 図鑑デバッグ情報
  void*           m_debugInfo;  // ZukanDebugInfo*

  // ロトムトーン（玩具連動）
  app::tool::RotomToneGenerator  m_RotomToneGenerator;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANIMPL_H_INCLUDED

