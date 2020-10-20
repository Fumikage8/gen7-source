// ============================================================================
/*
 * @file IApplicationSystemBuilder.h
 * @brief アプリケーションシステムを構築する為のインターフェースクラスです。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( IAPPLICATIONSYSTEMBUILDER_H_INCLUDE )
#define IAPPLICATIONSYSTEMBUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemDefine.h"
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "AppLib/include/Util/app_util_2d.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)
class ApplicationWorkBase;
GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


class IApplicationSystemBuilder
{
public:

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher() = 0;
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory() = 0;


  virtual u32 CreateAppRenderingManagerCount(){ return NetAppLib::System::DEFAULT_RENDERING_MANAGER_COUNT; }
  // @note : GFL_NEW で生成した物を返す。CreateAppRenderingManagerDescription か CreateAppRenderingManagerDescriptions のどちらかを使用する
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription(){ return NULL; }
  // @note : GFL_NEW_ARRAY で生成した物を返す
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescriptions(){ return NULL; }


  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork() = 0;
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID() = 0;
  virtual u32 GetApplicationResourceCount() = 0;


  virtual bool IsCreateMessageUtility(){ return false; }
  virtual void GetMessageUtilityParameter( u32& dataNum, u32& wordMax, u32& bufLen ){ dataNum = NetAppLib::System::DEFAULT_MESSAGE_DATA_COUNT; wordMax = NetAppLib::System::DEFAULT_WORD_SET_WORD_MAX; bufLen = NetAppLib::System::DEFAULT_WORD_SET_BUFFER_LENGTH; }


  virtual bool IsCreateNetAppMessageMenu(){ return false; }
  
  // トレーナーアイコンを使う
  virtual bool IsCreateAppToolTrainerIconRendering(){ return false; }
  //  トレーナーNPCのみの場合は true
  virtual bool IsCreateAppToolTrainerIconRenderingByNPCOnly() const { return false; }

  // トレーナーアイコンを使う個数
  virtual u32  GetTrainerIconNum(){ return 0; }
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType(){ return app::util::G2DUtil::SETUP_LOWER; }

  virtual bool IsCreateAppCountryRegionNameUtility(){ return false; }

  // UIViewマネージャのボタン監視モード
  virtual bool IsEnableInputExclusiveKeyMode(){ return false; }

  // 共通アイコン置き換えクラスを使う
  virtual bool IsCreateAppCommonGrpIconData(){ return false; }

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // IAPPLICATIONSYSTEMBUILDER_H_INCLUDE
