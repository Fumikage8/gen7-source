//==============================================================================
/**
 * @file    LayoutArcTest.h
 * @author  fukushima_yuya
 * @date    2015.02.09
 * @brief   レイアウトのリソース読み込みテスト
 */
//==============================================================================
#if PM_DEBUG

#if !defined(NIJI_PROJECT_LAYOUT_ARC_TEST_H_INCLUDED)
#define NIJI_PROJECT_LAYOUT_ARC_TEST_H_INCLUDED
#pragma once

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>  // @todo これはgfl2_Heap.hにまとめて欲しい
#include <Layout/include/gfl2_Layout.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

// niji
#include <GameSys/include/GameProc.h>

#define DISABLE_PIPELINE_20151214 (1)    // 開発初期の描画パイプライン削除に伴い、ビルドを通すため未使用コードをコメントアウトしました
#if !defined(DISABLE_PIPELINE_20151214)
#include <system/include/MyRenderingPipeLine.h>
#endif
// AppLib
#include <AppLib/include/Util/app_util_AppLytAccessor.h>


GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(LytArc)


//==============================================================================
/**
 * @brief    LayoutArcProc
 */
//==============================================================================
class LayoutArcProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( LayoutArcProc );

public:
  //! @brief  コンストラクタ
  LayoutArcProc( void );
  //! @brief  デストラクタ
  virtual ~LayoutArcProc( void );

public:
  //! @brief  PROCの初期化
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  //! @brief  PROCのメイン更新
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  //! @brief  PROCのメイン描画
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  //! @brief  PROCのメインを行う
  virtual gfl2::proc::Result NextFrameMainFunc( gfl2::proc::Manager* pManager );
  //! @brief  PROCの終了を行う
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

public:
  // 初期化処理
  void Initialize( void );
  // 終了処理
  void Finalize( void );
  // 更新処理
  void Update( void );

private:
  // レイアウトシステムの初期化
  void InitLytSys( void );
  // フォントの読み込み
  void LoadFont( gfl2::heap::HeapBase* pTempHeap );

  // GARCの読み込み
  void LoadGARC( gfl2::heap::HeapBase* pTempHeap );
  // AppLytの読み込み
  void LoadAppLyt( gfl2::heap::HeapBase* pTempHeap );
  // リソースの読み込み
  void LoadResource( void );
  // リソースの読み込み(バイナリ指定版)
  void LoadResource( const void* pBinary );
  // 部品レイアウトの読み込み
  void LoadLytParts();
  // GARCの破棄
  void DeleteGARC( void );

  // レイアウトワークの生成
  void CreateLytWk( void );

  // 環境の生成
  void CreateEnv( void );

private:
  s32    m_count;

private:
  gfl2::heap::HeapBase*        m_pMemHeap;
  gfl2::heap::HeapBase*        m_pDevHeap;
  gfl2::heap::NwAllocator*    m_pMemAllocator;
  gfl2::heap::NwAllocator*    m_pDevAllocator;

  gfl2::lyt::LytSys*          m_pLytSys;
  gfl2::lyt::LytWk*            m_pLytWk;
  gfl2::lyt::LytMultiResID*    m_pResID;
  gfl2::lyt::Env*              m_pEnv;
  gfl2::str::ResFont*          m_pFont;
  gfl2::util::DrawUtilText* mpDrawUtilText;

  app::util::AppLytAccessor*  m_pLytAccessor;

#if !defined(DISABLE_PIPELINE_20151214)
  MyRenderingPipeLine*        m_pRenderingPipeLine;
#endif
  gfl2::util::UtilTextRenderingPipeLine*    m_pUtilTextRenderingPipeLine;

  System::nijiAllocator*      m_pNijiAllocator;
};

GFL_NAMESPACE_END(LytArc)
GFL_NAMESPACE_END(Test)

#endif  // NIJI_PROJECT_LAYOUT_ARC_TEST_H_INCLUDED

#endif // PM_DEBUG
