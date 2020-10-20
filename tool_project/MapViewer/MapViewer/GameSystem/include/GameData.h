///#if !defined( __GAMEDATA_H__ )
///#define __GAMEDATA_H__
//===================================================================
/**
 * @file    GameData.h
 * @brief   ゲーム進行データ管理
 * @author  k.ohno -> Miyuki Iwasawa
 * @date    15.2.28
 * @note
 * 基本的にゲーム進行データはセーブデータとして存在するが、
 * 通信時の複数プレイヤー情報やフィールドマップ情報などセーブデータに
 * のるとは限らない情報もある。
 * そのようなセーブデータ・非セーブデータへ並列にアクセスするインターフェイスを
 * 各パートごとに構成するとパートごとにアクセス手法が違ったり、また同じ機能の
 * ものが複数存在するなどプログラム全体の複雑さが増してしまう可能性が高い。
 * それを避けるため、共通インターフェイスを作ることによって簡略化する。
 */
//===================================================================
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <ui/include/gfl2_Ui.h>
//#include <pml_PokeParty.h>

#include "System/include/PokemonVersion.h"

//===================================================================
//
//
//  前方参照宣言
//
//
//  GameData/SaveDataはゲーム進行データの各クラスへのアクセスを
//  提供するだけなので、クラス詳細を知る必要はない。
//  カプセル化を進めるためにはそれぞれの利用箇所で、
//  該当クラスのヘッダをインクルードして利用する。
//
//===================================================================

namespace gfl2 {
  namespace util {
    class GLHeapAllocator;
  }
}

namespace Field{

  namespace MoveModel{
    class FieldMoveModelManager;
    class FieldMoveCodeManager;
    ///class FieldActionCommandManager;
  }

  class FieldRootNode;
}


GFL_NAMESPACE_BEGIN(GameSystem)

//--------------------------------------------------------------
/**
 * @class GameData
 */
//--------------------------------------------------------------
class GameData
{
  GFL_FORBID_COPY_AND_ASSIGN(GameData); //コピーコンストラクタ＋代入禁止

private:

  gfl2::util::GLHeapAllocator* m_pHeapAllocator;        ///< ヒープアロケータ
  gfl2::util::GLHeapAllocator* m_pDeviceHeapAllocator;  ///< デバイスヒープアロケータ

  gfl2::heap::HeapBase * mTempHeap;

  // 動作モデル用
  Field::MoveModel::FieldMoveModelManager*     m_pFieldCharaModelManager;
  Field::MoveModel::FieldMoveCodeManager*      m_pFieldMoveCodeManager;
  ///Field::MoveModel::FieldActionCommandManager* m_pFieldActionComandManager;
  Field::FieldRootNode*                        m_pFieldRootNode;


public:
  //コンストラクタ
  GameData(gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap, gfl2::ui::DeviceManager* uiDeviceManager );
  //デストラクタ
  ~GameData();

  ///「さいしょから」ゲーム開始用データ設定
  void SetFirstData( gfl2::heap::HeapBase * pTempHeap, gfl2::heap::HeapBase * pTempDevHeap );

#if PM_DEBUG
  ///デバッグ開始用データ設定
  enum DebugDataType
  {
    DEBUG_DATA_TYPE_CTRNAME,  ///<プレイヤー名にCTR本体名を使う
    DEBUG_DATA_TYPE_DEFAULTNAME,  ///<エル等デフォルト名を使う
  };
  void SetDebugData( DebugDataType type, gfl2::heap::HeapBase* pTempHeap, gfl2::heap::HeapBase* pTempDevHeap, int sex = PM_FEMALE );
  ///バトルデバッグ用データ設定（全アイテムを100ずつ持ちます）
  void SetDebugBattleData( gfl2::heap::HeapBase * pTempHeap );
#endif

  //============================================================================================
  //    セーブ処理
  //============================================================================================

  //============================================================================================
  //  動作モデル用アクセサ
  //============================================================================================

  //-----------------------------------------------------------------------------
  /**
   * @brief 動作モデルマネージャーを取得
   */
  //-----------------------------------------------------------------------------
  inline Field::MoveModel::FieldMoveModelManager* GetFieldCharaModelManager( void ) const { return m_pFieldCharaModelManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief 動作コードマネージャーを取得
   */
  //-----------------------------------------------------------------------------
  inline Field::MoveModel::FieldMoveCodeManager* GetFieldMoveCodeManager( void ) const { return m_pFieldMoveCodeManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief アクションコマンドマネージャーを取得
   */
  //-----------------------------------------------------------------------------
  ///inline Field::MoveModel::FieldActionCommandManager* GetFieldActionComandManager( void ) const { return m_pFieldActionComandManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief フィールド用ノードルートを取得
   */
  //-----------------------------------------------------------------------------
  inline Field::FieldRootNode* GetFieldRootNode( void ) const { return m_pFieldRootNode; }

private:

};  //GameData

GFL_NAMESPACE_END(GameSystem)



///#endif //__GAMEDATA_H__
