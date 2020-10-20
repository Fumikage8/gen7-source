#if !defined(__SCRIPTENDCHECK_H__) // 重複定義防止
#define __SCRIPTENDCHECK_H__  // 重複定義防止マクロ
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "pawn/include/gfl2_Pawn.h"


//前方宣言
namespace GameSys{
  class GameManager;
}

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

// 1フレームで終了するチェック
void CheckOntime( GameSys::GameManager* pGameManager );

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

#endif
