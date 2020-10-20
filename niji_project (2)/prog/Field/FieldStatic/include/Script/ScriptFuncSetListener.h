//=============================================================================
/**
 * @file  ScriptFuncSetListener.cpp
 * @brief スクリプトコマンドが用いるリスナー系常駐クラス配置場所
 */
//=============================================================================

#ifndef __SCRIPTFUNCSETLISTENER_H__
#define __SCRIPTFUNCSETLISTENER_H__

#include "App/AppEvent/include/Box/BoxEventListener.h"
#include "App/Box/include/BoxAppParam.h"

namespace GameSys{
  class GameData;
}
namespace Field{
  namespace FieldScript{
    class FieldScriptSystem;
  }
}

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( FieldScript );

/*
 * ボックスコール
 */
class ScriptBoxEvenListener : public App::Event::BoxEventListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ScriptBoxEvenListener); // コピーコンストラクタ＋代入禁止
public:
  ScriptBoxEvenListener( FieldScriptSystem* p_sys, App::Box::BOX_CALL_TYPE type ) :
    m_pScriptSystem(p_sys), m_Type(type) {}

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTermination
   * @brief   終了の通知
   * @param   outJumpMode  ジャンプモード
   * @param   outTray  選んだトレイ番号 ※BoxAppParam.hの特殊な戻り値参照
   * @param   outPos   選んだ場所番号
   */
  //-----------------------------------------------------------------------------
  virtual void OnTermination(u8 outJumpMode, u8 outTray,u8 outPos );

private:
  FieldScriptSystem*  m_pScriptSystem;
  App::Box::BOX_CALL_TYPE m_Type;
};

GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );

#endif // __SCRIPTFUNCSETLISTENER_H__
