//=============================================================================
/**
 *
 *  @file   ScriptMoniterTrialModel.cpp
 *  @brief  スクリプト監視用
 *  @author Miyuki Iwasawa 
 *  @data   2015.12.14
 *
 */
//=============================================================================

#include <util/include/gfl2_std_string.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"
#include "System/include/GameStrLen.h"
#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptMoniter.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/PlacementData/FieldPlacementDataManager.h"

//#include "../subscreen/fieldmenu.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

#include "prog/Savedata/include/EventWorkDef.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//-----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 *
 *  @param  GameSys::GameEvent* event イベントが起動された元のイベント
 */
//-----------------------------------------------------------------------------
ScriptMoniterTrialModel::ScriptMoniterTrialModel( GameSys::GameManager* p_gman ) :
  IScriptMoniterBase( p_gman ),
{
  m_AlertMsg = "TrialModel";
}

//-----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
ScriptMoniterTrialModel::~ScriptMoniterTrialModel( void )
{
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 登録 
 */
/* ------------------------------------------------------------------------- */
void ScriptMoniterTrialModel::Regist( void )
{
  this->ClearBase();
  m_bIsRegist = true;
}
/* ------------------------------------------------------------------------- */
/**
 * @brief 登録解除
 */
/* ------------------------------------------------------------------------- */
void ScriptMoniterTrialModel::Unregist( void )
{
  this->ClearBase();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプト終了時チェック
 */
/* ------------------------------------------------------------------------- */
bool ScriptMoniterTrialModel::CheckEnd( void )
{
  if( this->IsRegist() == false ) return true;

  this->Alert( m_AlertMsg );
  this->Unregist();
}
/* ------------------------------------------------------------------------- */
/**
 * @brief フィールドクローズチェック
 */
/* ------------------------------------------------------------------------- */
bool ScriptMoniterTrialModel::CheckFieldClose( void )
{
  return true;
}
/* ------------------------------------------------------------------------- */
/**
 * @brief フィールド復帰チェック
 */
/* ------------------------------------------------------------------------- */
bool ScriptMoniterTrialModel::CheckFieldRecover( void )
{
  return true;
}
/* ------------------------------------------------------------------------- */
/**
 * @brief ゲームオーバー時チェック
 */
/* ------------------------------------------------------------------------- */
bool ScriptMoniterTrialModel::CheckGameOver( void )
{
  return true;
}



GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

