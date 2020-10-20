//=============================================================================
/**
 *
 *  @file   ScriptObject.h
 *  @brief  フィールドスクリプト：インスタンス
 *  @author hosaka genya
 *  @date   2012.03.23
 *
 */
//=============================================================================
#ifndef __SCRIPTOBJECT_H__
#define __SCRIPTOBJECT_H__
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <fs/include/gfl2_Fs.h>


#include "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"
#include "Pawn/include/PawnObject.h" // for PawnObject

/* ------------------------------------------------------------------------- */
/**
 * @brief クラス前方宣言
 */
/* ------------------------------------------------------------------------- */
namespace GameSys{
  class GameManager;
  class GameEvent;
}

namespace Field{
  namespace MoveModel{
    class FieldMoveModel;
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief フィールド
 */
/* ------------------------------------------------------------------------- */
GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

/// クラス前方宣言
class Work;

/* ------------------------------------------------------------------------- */
/**
 * @class スクリプトオブジェクト
 * @brief スクリプトインスタンス基底クラス
 */
/* ------------------------------------------------------------------------- */
class ScriptObject : public PawnScript::PawnObject
{
  GFL_FORBID_COPY_AND_ASSIGN( ScriptObject ); ///< コピーコンストラクタ + 代入禁止
public:
  static const int AMX_ID_NULL = 0xffff;  ///< AMD ID 無効値
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   *
   * @param p_heap ヒープへのポインタ
   * @param p_heap_debug デバッグ用ヒープへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  ScriptObject( gfl2::heap::HeapBase* p_heap, gfl2::heap::HeapBase* p_heap_debug = NULL );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ~ScriptObject();
  
  void SetWork( Work* p_work ) { m_pWork = p_work; }
  Work* GetWork( void ) const { return m_pWork; }
  void  SetGameManager( GameSys::GameManager* _pGameManager ){  m_pGameManager = _pGameManager; }
  GameSys::GameManager* GetGameManager() const {  return m_pGameManager;  }

  Work* ScriptWorkCreate( GameSys::GameEvent* p_event, MoveModel::FieldMoveModel* p_talk_model );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  スクリプトワーク　破棄
   */
  //-----------------------------------------------------------------------------
  void DeleteWork( void );

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
  static u32 GetRegistID( ZoneID zone_id, gfl2::fs::ArcFile::ARCDATID resident_id = AMX_ID_NULL );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 指定レジストIDの関数をレジスト
   *
   * @param reg_id レジストID（ビット演算）
   */
  /* ------------------------------------------------------------------------- */
  void RegistFunc( u32 reg_id );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 命令セットをレジスト
   *
   * @param zone_id ゾーンID
   * @param resident_id ARCID
   */
  /* ------------------------------------------------------------------------- */
  void RegistFunc( ZoneID zone_id, gfl2::fs::ArcFile::ARCDATID resident_id = AMX_ID_NULL );

private:
  Work* m_pWork;
  GameSys::GameManager* m_pGameManager;
};

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )


#endif // __SCRIPTOBJECT_H__
