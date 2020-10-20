//==============================================================================
/**
 * @file        ScriptFuncSetJoinFesta.h
 * @brief       ジョインフェスタ用コマンド
 * @author      ichiraku_katsuhiko
 * @date        2015/10/21
 */
//==============================================================================

#if !defined(__SCRIPTFUNCSETJOINFESTA_H__) // 重複定義防止
#define __SCRIPTFUNCSETJOINFESTA_H__   // 重複定義防止マクロ
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "pawn/include/gfl2_Pawn.h"

//前方宣言
namespace GameSys{
  class GameManager;
}
namespace Field{
  namespace Camera{
    class CameraManager;
  }
  namespace FieldScript{
    class FieldScriptSystem;
  }
}

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )

//------------------------------------------------------------------------------
/**
 * @class       FuncSetJoinFesta
 */
//------------------------------------------------------------------------------
class FuncSetJoinFesta {
  GFL_FORBID_COPY_AND_ASSIGN(FuncSetJoinFesta);
public:
  /// 命令テーブルセットを取得.
  static const GFL_PAWN_FUNC_TABLE* GetTable(void);
  
private:
  // スクリプトに渡す関数

  // フェスコイン使用のショップ
  static cell AMX_NATIVE_CALL CallJoinFestaShop_(AMX *, const cell * );

};


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

#endif // __SCRIPTFUNCSETJOINFESTA_H__ 重複定義防止
