// ============================================================================
/*
 * @file ApplicationSystemBase.h
 * @brief アプリケーションシステムの基底クラスです。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( APPLICATIONSYSTEMBASE_H_INCLUDE )
#define APPLICATIONSYSTEMBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_GFGL.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"

GFL_NAMESPACE_BEGIN(app)
  GFL_NAMESPACE_BEGIN(util)
    class AppRenderingManager;
    class AppCountryRegionNameUtility;
  GFL_NAMESPACE_END(util)

  GFL_NAMESPACE_BEGIN(tool)
    class AppCommonGrpIconData;
  GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
  GFL_NAMESPACE_BEGIN(frame)
    class Manager;
  GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(System)
  class nijiAllocator;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(NetAppLib)
  GFL_NAMESPACE_BEGIN(Message)
    class MessageUtility;
  GFL_NAMESPACE_END(Message)

  GFL_NAMESPACE_BEGIN(UI)
    class NetAppMessageMenu;
  GFL_NAMESPACE_END(UI)

  GFL_NAMESPACE_BEGIN(Util)
    class NetAppTrainerIconUtility;
  GFL_NAMESPACE_END(Util)

  GFL_NAMESPACE_BEGIN(System)
    class IApplicationSystemBuilder;
    class IApplicationFrameSwitcher;
    class IApplicationFrameFactory;
    class ResourceManager;
    class NetAppFrameBase;
    class ApplicationWorkBase;
  GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)

class ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ApplicationSystemBase );

public:
  ApplicationSystemBase();
  virtual~ApplicationSystemBase();

  //! @brief アプリケーションシステムの構築を行います。
  //! param  pApplicationSystemBuilder アプリケーションシステムビルダー
  virtual void Build( NetAppLib::System::IApplicationSystemBuilder* pApplicationSystemBuilder );
  
  //! @brief ローディング処理を行います。
  //! @retval true ロード中
  //! @retval false ロード完了
  virtual bool Loading();

  //! @brief 更新処理を行います。
  //! @retval true 実行しているFrameが存在しない(Procを終了させる)
  //! @retval false 実行しているFrameが存在している(Procを継続させる)
  virtual bool Update();

  //! @brief 描画処理(下準備)を行います。
  virtual void PreDraw();

  //! @brief 描画処理を行います。
  //! @param displayNo ディスプレイ番号
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //! @brief アプリケーションの終了処理を行います。
  //! @retval true 終了処理中
  //! @retval false 終了処理完了
  virtual bool End();

  //! @brief 一時停止
  virtual void Suspend();
  //! @brief 再起動
  virtual void Resume();


  //! @brief 強制でフェードを使用しない
  void NonUseFadeRequest();


private:

  //! @brief メンバの破棄を行います。
  //! @note 基本的にはオーバーライドする必要は無いと思いますが、何かしらの理由で解放手順を変えたい時はオーバーライドして下さい。
  virtual void Destroy();
  
  //! @brief リソースの読み込みリクエストを行います。
  virtual void LoadResourceRequest(){}

  //! @brief メッセージデータの作成を行います。
  virtual void CreateMessageData(){}

  //! @brief ビューの作成を行います。
  virtual void CreateView(){}

  //! @brief ビューの作成が完了したか
  virtual bool IsCreateViewComplete(){ return true; }

  //! @brief モデルの登録を行います。
  virtual void AddModel(){}

  //! @brief モデルの登録解除を行います。
  virtual void RemoveModel(){}

  //! @brief カメラの設定を行います。
  virtual void SetupCamera(){}

  virtual void UnsetCamera(){}

  //! @brief 独自Viewの更新
  virtual void UpdateView(){}
  //! @brief 独自Viewの描画
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo ){}
  //! @brief 独自Viewが破棄可能か
  virtual bool IsExitView(){ return true; }

  //! @brief 独自通信クラスの作成
  virtual void CreateNet(){}
  //! @brief 独自通信クラスが完了したか
  virtual bool IsCreateNetComplete(){ return true; }
  //! @brief 独自通信クラスの更新
  virtual void UpdateNet(){}

  void updateRenderingManagers();
  bool isCreatedRenderingManagers();
  void preDrawRenderingManagers();
  void drawRenderingManagers( gfl2::gfx::CtrDisplayNo displayNo );
  bool canDestroyRenderingManagers();

  void updateCountryRegionNameUtility();

#if PM_DEBUG
  void appHeapDebugPrint();
  void residentDeviceDebugPrint();
#endif // PM_DEBUG

private:

  enum LoadSequence
  {
    LOAD_SEQUENCE_APP_RENDER_MANAGER_CREATE_WAIT,
    LOAD_SEQUENCE_APP_TRAINER_ICON_CREATE_WAIT,

    LOAD_SEQUENCE_APP_COMMON_GRP_ICON_DATA_WAIT,

    LOAD_SEQUENCE_LOAD_START,
    LOAD_SEQUENCE_LOAD_WAIT,
    LOAD_SEQUENCE_CREATE_VIEW_WAIT
  };

  enum EndSequence
  {
    END_SEQUENCE_START,
    END_SEQUENCE_APP_TRAINER_ICON_TERMINATE_WAIT,
    END_SEQUENCE_APP_COUNTRY_REGION_NAME_TERMINATE_START,
    END_SEQUENCE_APP_COUNTRY_REGION_NAME_TERMINATE_WAIT,
    END_SEQUENCE_REMOVE_MODEL,
    END_SEQUENCE_APP_RENDERING_MANAGER_TERMINATE_WAIT
  };

protected:

  NetAppLib::System::IApplicationFrameSwitcher*         m_pFrameSwitcher;
  NetAppLib::System::IApplicationFrameFactory*          m_pFrameFactory;
  NetAppLib::System::ResourceManager*                   m_pResourceManager;
  ::System::nijiAllocator*                              m_pNijiAllocator;
  app::util::AppRenderingManager*                       m_pAppRenderingManager; // @note : テンポラリとして使用します。m_pAppRenderingManagers の先頭アドレスが設定されています
  app::util::AppRenderingManager*                       m_pAppRenderingManagers;
  applib::frame::Manager*                               m_pAppFrameManager;

  NetAppLib::Message::MessageUtility*                   m_pMessageUtility;
  NetAppLib::UI::NetAppMessageMenu*                     m_pNetAppMessageMenu;
  NetAppLib::Util::NetAppTrainerIconUtility*            m_pNetAppTrainerIconUtility;
  app::util::AppCountryRegionNameUtility*               m_pAppCountryRegionNameUtility;
  app::tool::AppCommonGrpIconData*                      m_pAppCommonGrpIconData;

private:

  LoadSequence                                          m_eLoadSequence;
  EndSequence                                           m_eEndSequence;
  NetAppLib::System::NetAppFrameBase*                   m_pExecuteFrame;
  NetAppLib::System::ApplicationWorkBase*               m_pWork;
  NetAppLib::System::FrameID                            m_StartFrameID;
  u32                                                   m_AppRenderingManagerCount;
  bool                                                  m_isSuspend;
  bool                                                  m_isNonUseFade;
  bool                                                  m_isEnableInputExclusiveKeyMode;
  bool                                                  m_isUIViewInputExclusiveKey;   //!< UIViewマネージャのボタン監視モード
  bool                                                  m_isFadeOutForNetworkError;   //! 通信エラー発生時用のフェードアウト

#if PM_DEBUG
  s64                                                   m_debugLoadStartTime;
#endif
};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // APPLICATIONSYSTEMBASE_H_INCLUDE
