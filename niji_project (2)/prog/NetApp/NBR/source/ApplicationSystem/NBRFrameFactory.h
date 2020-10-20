// ============================================================================
/*
 * @file NBRFrameFactory.h
 * @brief 通信対戦受付アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_FRAMEFACTORY_H_INCLUDE )
#define NBR_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
class NBRNet;
GFL_NAMESPACE_BEGIN(View)
class NBRParentView;
GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class NBRFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRFrameFactory );
public:
  NBRFrameFactory( View::NBRParentView* pParentView, NBRWork* pNBRWork, NBRNet* pNet );
  virtual~NBRFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  View::NBRParentView*        m_pParentView;    //! 親ビュー
  NBRWork*                    m_pWork;          //! ワーク
  NBRNet*                     m_pNet;           //! 通信管理クラス
};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_FRAMEFACTORY_H_INCLUDE
