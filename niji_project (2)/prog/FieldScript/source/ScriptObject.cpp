//=============================================================================
/**
 *
 *  @file       ScriptObject.cpp
 *  @brief  フィールドスクリプト：インスタンス
 *  @author hosaka genya
 *  @data       2012.03.23
 *
 */
//=============================================================================
#include "FieldScript/include/ScriptObject.h"

#include "FieldScript/include/ScriptWork.h"

//script funcset
#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "NetApp/JoinFesta/include/Script/ScriptFuncSetJoinFesta.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetInit.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetFieldInit.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetAppCall.h"
#include "Field/FieldRo/include/Script/ScriptFuncSetSodateya.h"     //FuncSetSodateya::GetTable
#include "Field/FieldRo/include/Script/ScriptFuncSetBattleInst.h"     //FuncSetSodateya::GetTable

#include "arc_index/FieldScript.gaix" // for GARC_field_script_

// conv_header
#include <niji_conv_header/field/zone/zone_id.h>

/* ------------------------------------------------------------------------- */
/**
 * @brief フィールド
 */
/* ------------------------------------------------------------------------- */
GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//#include "assets/field/script/funcset_table_resident.cdat" // sc_funcset_table_resident
//#include "assets/field/script/funcset_table_zone.cdat" // sc_funcset_table_zone

static const u32 sc_funcset_table_resident[] = {
  0xFFFFFFFF,
};
static const u32 sc_funcset_table_zone[] = {
  0xFFFFFFFF,
};

/* ------------------------------------------------------------------------- */
/**
 * @brief コンストラクタ
 *
 * @param p_heap ヒープへのポインタ
 * @param p_heap_debug デバッグ用ヒープへのポインタ
 */
/* ------------------------------------------------------------------------- */
ScriptObject::ScriptObject( gfl2::heap::HeapBase* p_heap, gfl2::heap::HeapBase* p_heap_debug ) : 
  PawnScript::PawnObject( p_heap, p_heap_debug ), 
  m_pWork( NULL ),
  m_pGameManager( NULL )
{

}

/* ------------------------------------------------------------------------- */
/**
 * @brief デストラクタ
 */
/* ------------------------------------------------------------------------- */
ScriptObject::~ScriptObject()
{

}

//-----------------------------------------------------------------------------
/**
 *  @brief  スクリプトワーク　破棄
 */
//-----------------------------------------------------------------------------
void ScriptObject::DeleteWork( void )
{
  GFL_SAFE_DELETE( m_pWork );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 命令セットをレジスト
 *
 * @param zone_id ゾーンID
 * @param resident_id ARCID
 */
/* ------------------------------------------------------------------------- */
void ScriptObject::RegistFunc( ZoneID zone_id, gfl2::fs::ArcFile::ARCDATID resident_id )
{
  u32 reg_id = GetRegistID( zone_id, resident_id );
  RegistFunc( reg_id );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief レジストIDを取得
 *
 * @param zone_id ゾーンID
 * @param resident_id ARCID
 *
 * @return レジストID
 */
/* ------------------------------------------------------------------------- */
u32 ScriptObject::GetRegistID( ZoneID zone_id, gfl2::fs::ArcFile::ARCDATID resident_id )
{
  const u32 * p_data = NULL;

  // ジョインフェスタか
  bool isJoinFesta = false;

  switch( resident_id ){
  case GARC_FieldScript_join_festa_field_AMX:
  case GARC_FieldScript_join_festa_room_AMX:
  case GARC_FieldScript_join_festa_attraction_AMX:
  case GARC_FieldScript_join_festa_facilities_AMX:
  case GARC_FieldScript_join_festa_tutorial_AMX:
  case GARC_FieldScript_battle_fes_scr_AMX:
    {
      isJoinFesta = true;
      ICHI_PRINT(">ICHI ジョインフェスタゾーン\n");
    }
    break;
  }
  
  //@attention nijiでは関数登録テーブル生成を行っていない。将来は復活させるかも by iwasawa 160225
  zone_id = 0;
  resident_id = 0;

  if( resident_id == AMX_ID_NULL )
  {
    p_data = &sc_funcset_table_zone[ zone_id ];
  }
  else 
  {
    p_data = &sc_funcset_table_resident[resident_id];
  }

  // ジョインフェスタじゃない場合は、ジョインフェスタコマンドテーブルを読み込まない
  if( isJoinFesta == false )
  {
    return (*p_data & ~(FUNCSET_BIT_JOIN_FESTA));
  }

  return *p_data;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 指定レジストIDの関数をレジスト
 *
 * @param reg_id レジストID（ビット演算）
 */
/* ------------------------------------------------------------------------- */
void ScriptObject::RegistFunc( u32 reg_id )
{
  const GFL_PAWN_FUNC_TABLE* tbl;

  if( reg_id & FUNCSET_BIT_INIT )
  {
    RegisterPawnFunction( FuncSetInit::GetTable() );
    RegisterPawnFunction( FuncSetFieldInit::GetTable() );
  }

  if( reg_id & FUNCSET_BIT_COMMON )
  {
    RegisterPawnFunction( FuncSetCommon::GetTable() );
    RegisterPawnFunction( FuncSetAppCall::GetTable() );
  }

  if( reg_id & FUNCSET_BIT_JOIN_FESTA )
  {
    RegisterPawnFunction( FuncSetJoinFesta::GetTable() );
  }
  if( reg_id & FUNCSET_BIT_SODATEYA )
  {
    RegisterPawnFunction( FuncSetSodateya::GetTable() );

  }

  if( reg_id & FUNCSET_BIT_BATTLE_INST )
  {
    RegisterPawnFunction( FuncSetBattleInst::GetTable() );
  }
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

