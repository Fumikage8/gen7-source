// ============================================================================
/*
 * @file NetApplicationView.h
 * @brief UIViewに入力フラグと描画フラグを持ったクラスです。
 *        入力フラグと描画フラグは外部から操作してもらいます。
 *        内部ではフラグを参照してUIViewのUpdateとDrawを呼び出します。
 * @date 2015.04.08
 */
// ============================================================================
#if !defined( NETAPPLICATIONVIEWBASE_H_INCLUDE )
#define NETAPPLICATIONVIEWBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/Ui/UIView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)
class ApplicationWorkBase;
GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Message)
class MessageUtility;
GFL_NAMESPACE_END(Message)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


class NetApplicationViewBase : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( NetApplicationViewBase );
public:
  NetApplicationViewBase( app::util::Heap* pHeap );

  NetApplicationViewBase(
    NetAppLib::System::ApplicationWorkBase* pWork,
    NetAppLib::System::ResourceID layoutResourceID,
    gfl2::lyt::LytArcIndex layoutArcIndex,
    u32 animationMax,
    app::util::G2DUtil::SETUP_DISPLAY_TYPE setupDisplayType,
    NetAppLib::Message::MessageUtility* pMessageUtility,
    gfl2::fs::ArcFile::ARCDATID garcID );

  virtual~NetApplicationViewBase();


  //------------------------------------------------------------------
  /**
    *  @func     Create2DOneLayout
    *  @brief    ひとつのLayoutリソースに特化するかわりに必要なパラメータを減らしたCreate2D簡易版
    */
  //------------------------------------------------------------------
  virtual void  Create2DOneLayout(
    NetAppLib::System::ApplicationWorkBase* pWork,
    NetAppLib::System::ResourceID           layoutResourceID,
    gfl2::lyt::LytArcIndex                  layoutArcIndex,
    u32                                     animationMax,
    app::util::G2DUtil::SETUP_DISPLAY_TYPE  setupDisplayType,
    NetAppLib::Message::MessageUtility*     pMessageUtility,
    gfl2::fs::ArcFile::ARCDATID             messageArcDatID
  );

protected:
  void setupLayout( u32 layoutID, u32 animationID );

  app::util::G2DUtil::LytResData    m_LayoutResourceData;
  app::util::G2DUtil::LytwkData     m_LayoutWorkData;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPPLICATIONVIEWBASE_H_INCLUDE
