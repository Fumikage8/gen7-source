// ============================================================================
/*
 * @file ApplicationWorkBase.h
 * @brief アプリを動かす為に必要なワークの基底クラスです。
 *        このクラスにはどのアプリでも必ず使うようなモジュールやデータを持つ事になります。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( APPLICATIONWORKBASE_H_INCLUDE )
#define APPLICATIONWORKBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <types/include/gfl2_Typedef.h>
#include "AppLib/include/Util/app_util_heap.h"
#include "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"
#include "AppLib/include/Util/AppCountryRegionNameUtility.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


class ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ApplicationWorkBase );
private:
  static const int  FRAME_MANAGER_HEAP_SIZE   = 0x1000;     //  4KB

public:
  ApplicationWorkBase( u32 heapSize, u32 deviceHeapSize );
  ApplicationWorkBase( gfl2::heap::HEAPID heapId, u32 heapSize, u32 deviceHeapSize );
  virtual~ApplicationWorkBase();

  gfl2::heap::HeapBase* GetAppFrameManagerHeap() const { return m_pAppFrameManagerHeap; }

  // ----------------------------------------------------------------------------------------------
  // アプリ側で使用する関数

  // ヒープ取得を取得します
  app::util::Heap* GetAppHeap() const { return m_pAppHeap; }

  // デバイスヒープ取得を取得します
  gfl2::heap::HeapBase* GetDeviceHeap() const { return m_pAppHeap->GetDeviceHeap(); }

  // フレームのリザルトを設定します
  void SetFrameResult( NetAppLib::System::FrameResult result ){ m_FrameResult = result; }

  //! @brief サスペンド状態か設定
  void SetSuspend( bool flag ){ m_bSuspend = flag; }
  //! @brief サスペンド状態か取得
  bool IsSuspend() const { return m_bSuspend; }

  // レンダリングマネージャを取得します
  app::util::AppRenderingManager* GetAppRenderingManager() const { return m_pAppRenderingManager; }
  app::util::AppRenderingManager* GetAppRenderingManager( u32 index ) const { return &m_pAppRenderingManager[ index ]; }

  // ----------------------------------------------------------------------------------------------
  // アプリ側で必要に応じて使用する関数

  // リソースマネージャを取得します
  NetAppLib::System::ResourceManager* GetResourceManager() const { return m_pResourceManager; }

  // ----------------------------------------------------------------------------------------------
  // ApplicationSystemBaseで使用する関数
  NetAppLib::System::FrameID GetExecuteFrameID() const { return m_ExecuteFrameID; }
  void SetExecuteFrameID( NetAppLib::System::FrameID frameID ) { m_ExecuteFrameID = frameID; }

  //! @note Procが終了した時に最後に実行していたFrameが何なのかを判定する為に使用します
  NetAppLib::System::FrameID GetLastExecuteFrameID() const { return m_LastExecuteFrameID; }
  void SetLastExecuteFrameID( NetAppLib::System::FrameID frameID ) { m_LastExecuteFrameID = frameID; }

  NetAppLib::System::FrameResult GetFrameResult() const { return m_FrameResult; }

  void SetResourceManager( NetAppLib::System::ResourceManager* pResourceManager ){ m_pResourceManager = pResourceManager; }
  void SetAppRenderingManager( app::util::AppRenderingManager* pAppRenderingManager ){ m_pAppRenderingManager = pAppRenderingManager; }

  // MessageUtility
  NetAppLib::Message::MessageUtility* GetMessageUtility() { return m_pMessageUtility; }
  void SetMessageUtility( NetAppLib::Message::MessageUtility* pMessageUtility ) { m_pMessageUtility = pMessageUtility; }

  // AppCountryRegionNameUtility
  app::util::AppCountryRegionNameUtility* GetAppCountryRegionNameUtility() { return m_pAppCountryRegionNameUtility; }
  void SetAppCountryRegionNameUtility( app::util::AppCountryRegionNameUtility* pAppCountryRegionNameUtility ) { m_pAppCountryRegionNameUtility = pAppCountryRegionNameUtility; }

  // AppCommonGrpIconData
  app::tool::AppCommonGrpIconData* GetAppCommonGrpIconData(){ return m_pAppCommonGrpIconData; }
  void SetAppCommonGrpIconData( app::tool::AppCommonGrpIconData* pAppCommonGrpIconData ){ m_pAppCommonGrpIconData = pAppCommonGrpIconData; }

  void CreateHeap( gfl2::heap::HEAPID heapId, u32 heapSize, u32 deviceHeapSize );
  void DeleteHeap();

private:

  gfl2::heap::HeapBase*                     m_pAppFrameManagerHeap;   //! AppFrameManagerを確保する為の専用ヒープ
  app::util::Heap*                          m_pAppHeap;               //! 各アプリで使用するヒープ
  gfl2::heap::HeapBase*                     m_pDevHeap;               //! デバイスヒープ
  gfl2::heap::NwAllocator*                  m_pDevHeapAllocator;      //! デバイスヒープアロケータ
  gfl2::util::GLHeapAllocator*              m_pDevGLHeapAllocator;    //! デバイスGLヒープアロケータ
  NetAppLib::System::FrameID                m_ExecuteFrameID;         //! 現在実行中のフレームID
  NetAppLib::System::FrameID                m_LastExecuteFrameID;     //! 最後に実行されたフレームID
  NetAppLib::System::FrameResult            m_FrameResult;            //! 各アプリのフレームのリザルトコード
  bool                                      m_bSuspend;               //! サスペンド状態フラグ

  // 外部(ApplicationSystemBase)で生成して設定して使うもの
  NetAppLib::System::ResourceManager*       m_pResourceManager;
  app::util::AppRenderingManager*           m_pAppRenderingManager;
  NetAppLib::Message::MessageUtility*       m_pMessageUtility;
  app::util::AppCountryRegionNameUtility*   m_pAppCountryRegionNameUtility;
  app::tool::AppCommonGrpIconData*          m_pAppCommonGrpIconData;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // APPLICATIONWORKBASE_H_INCLUDE
