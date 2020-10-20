// ============================================================================
/*
 * @file NBRLowerViewBase.h
 * @brief 通信対戦受付アプリ　下画面ビュー基底クラス
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBRLOWERVIEWBASE_H_INCLUDE )
#define NBRLOWERVIEWBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

#include "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"

#include "NetApp/NBR/source/NBRSoundDefine.h"
#include "NetApp/NBR/source/View/NBRViewDefine.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)
class ApplicationWorkBase;
GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

class NBRLowerViewBase :
  public NetAppLib::UI::NetAppCursorView
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRLowerViewBase );
public:
  // コンストラクタ
  NBRLowerViewBase( NetAppLib::System::ApplicationWorkBase* pWork );
  // デストラクタ
  virtual~NBRLowerViewBase();

  // ↓NetAppCursorViewのを回したいだけならオーバーライドしなくてよい
  //virtual void Update();
  //virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){};

public:
  NetAppLib::System::ApplicationWorkBase*               m_pWork;
  app::util::Heap*                                      m_pHeap;      ///< ヒープメモリ
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBRLOWERVIEWBASE_H_INCLUDE
