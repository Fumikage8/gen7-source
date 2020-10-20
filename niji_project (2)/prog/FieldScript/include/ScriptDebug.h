//============================================================================
/**
 *
 *  @file   ScriptDebug.h
 *  @brief  スクリプト デバッグ機能関連ワーク
 *  @author Miyuki Iwasawa
 *  @date   2015.10.22
 *
 */
//============================================================================
#ifndef __SCRIPTDEBUG_H__
#define __SCRIPTDEBUG_H__ 

#if PM_DEBUG

//system
#include "GameSys/include/GameEvent.h"

#include "Print/include/PrintSystem.h"
#include "Print/include/WordSet.h"

#include "niji_reference_files/script/FieldPawnTypes.h"   //SCRID
#include "FieldScript/include/ScriptTypes.h"  //SCRIPT_TYPE
#include "FieldScript/include/ScriptInfo.h"
#include "FieldScript/include/ScriptAssert.h"
#include "Pawn/include/PawnObject.h"

#include "niji_conv_header/field/flagwork/WorkDefine.h"

/* ------------------------------------------------------------------------- */
/**
 * @brief フィールド
 */
/* ------------------------------------------------------------------------- */
GFL_NAMESPACE_BEGIN( Field );
/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプト
 */
/* ------------------------------------------------------------------------- */
GFL_NAMESPACE_BEGIN( FieldScript );

enum{
  DEBUG_CHR_EDIT_LIST_MAX = 32,
  DEBUG_CHR_EDIT_NAME_LEN = 32,
};

/// デバッグ キャラエディットリスト
struct DebugChrEditList{
  char name[DEBUG_CHR_EDIT_NAME_LEN];
  u32 event_id;
  int dynamic_mot_id;
};

/// デバッグ シーンエディットパラメータ
enum{
  DEBUG_SCENE_EDIT_PARAM_SKIP_CODE_START,
  DEBUG_SCENE_EDIT_PARAM_SKIP_CODE_END,
  DEBUG_SCENE_EDIT_PARAM_SKIP_POINT_MAX,
  DEBUG_SCENE_EDIT_PARAM_MAX,
};

struct DebugSceneEditParam{
  int skip_code_start;
  int skip_code_end;
  int skip_point_max;
};

GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );

#endif //PM_DEBUG
#endif // __SCRIPTDEBUG_H__
