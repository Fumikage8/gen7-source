// ============================================================================
/*
 * @file NBRUpperViewBase.h
 * @brief 通信対戦受付アプリ　上画面ビュー
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBRUPPERVIEWBASE_H_INCLUDE )
#define NBRUPPERVIEWBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "AppLib/include/Ui/UIView.h"

#include "NetApp/NBR/source/NBRSoundDefine.h"
#include "NetApp/NBR/source/View/NBRViewDefine.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)
class ApplicationWorkBase;
GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(Savedata)
class MyStatus;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

class NBRUpperViewBase : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRUpperViewBase );
public:
  // コンストラクタ
  NBRUpperViewBase( NetAppLib::System::ApplicationWorkBase* pWork, NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility );
  // デストラクタ
  virtual~NBRUpperViewBase();

  // UIView
  //virtual void Update(){}; ←UIViewのUpdateを回したいだけならオーバーライドしなくてよい
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){};

public:

  /// タイトル文字
  void SetTitleTextOneLine( u32 msg_title );
  void SetTitleTextTwoLine( u32 msg_title1, u32 msg_title2 );

public:
  NetAppLib::System::ApplicationWorkBase* m_pWork;
  app::util::Heap*    m_pHeap;      ///< ヒープメモリ
  NetAppLib::Util::NetAppTrainerIconUtility&      m_NetAppTrainerIconUtility;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBRUPPERVIEWBASE_H_INCLUDE
