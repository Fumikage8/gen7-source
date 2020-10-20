//===================================================================
/**
 * @file    GameData.cpp
 * @brief   ゲーム進行データ管理
 * @author  Miyuki Iwasawa 
 * @date    2015.02.28
*/
//===================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <System/include/GflvUse.h>
#include "GameSystem/include/GameData.h"

// キャラクターシステム
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel_define.h"
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModelManager.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodeManager.h"
///#include "ViewerProc/include/FieldRo/ActionCommand/FieldActionCommandManager.h"

#include "ViewerProc/include/FieldRo/FieldRootNode.h"

GFL_NAMESPACE_BEGIN(GameSystem)

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief セーブデータに生成するファイル数
 * //@note  kujiraではリアル地域ポケモン情報、通常進行データの2件
 * @note  2013.03.20  通常進行データの1件のみに修正
 */
//--------------------------------------------------------------
enum {
  //SAVE_FILES_MAX = 1 + 1,
  SAVE_FILES_MAX = 1,
};

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------
GameData::GameData( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* deviceHeap, gfl2::ui::DeviceManager* uiDeviceManager )
{
  // アロケータ作成
  m_pHeapAllocator       = GFL_NEW(heap) gfl2::util::GLHeapAllocator(heap);
  m_pDeviceHeapAllocator = GFL_NEW(deviceHeap) gfl2::util::GLHeapAllocator(deviceHeap);

  // 動作モデル用
  m_pFieldRootNode            = GFL_NEW(deviceHeap) Field::FieldRootNode();
  m_pFieldCharaModelManager   = GFL_NEW(deviceHeap) Field::MoveModel::FieldMoveModelManager();
  m_pFieldMoveCodeManager     = GFL_NEW(deviceHeap) Field::MoveModel::FieldMoveCodeManager();
  ///m_pFieldActionComandManager = GFL_NEW(deviceHeap) Field::MoveModel::FieldActionCommandManager();

  // 動作モデルマネージャーのリソース部以外を初期化
  // *補足* 動作モデルマネージャーはフィールドプロック内でメモリを動的に確保する箇所があるので、フィールドプロック用ヒープを割り当てて別途初期化する必要があります
  m_pFieldRootNode->Initialize( m_pDeviceHeapAllocator );
	m_pFieldCharaModelManager->InitializeWork(m_pFieldRootNode->GetInstanceRootNode(),deviceHeap);
  m_pFieldMoveCodeManager->Initialize(deviceHeap);
  ///m_pFieldActionComandManager->Initialize(deviceHeap);

}


//--------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------
GameData::~GameData()
{
  // 動作モデル用
  GFL_SAFE_DELETE(m_pFieldCharaModelManager);
  GFL_SAFE_DELETE(m_pFieldMoveCodeManager);
  ///GFL_SAFE_DELETE(m_pFieldActionComandManager);
  GFL_SAFE_DELETE(m_pFieldRootNode);

  // アロケータ削除
  GFL_SAFE_DELETE(m_pHeapAllocator);
  GFL_SAFE_DELETE(m_pDeviceHeapAllocator);
}



//============================================================================================
//
//
//      初期化処理
//
//
//============================================================================================
//----------------------------------------------------------------------------
  /**
   * @brief 「さいしょから」ゲーム開始用データ設定
   */
//----------------------------------------------------------------------------
void GameData::SetFirstData( gfl2::heap::HeapBase * pTempHeap, gfl2::heap::HeapBase * pTempDevHeap )
{
  GFL_PRINT("GameData::SetFirstData\n");
}

#if PM_DEBUG
//----------------------------------------------------------------------------
  /**
   * @brief デバッグ開始用データ設定
   */
//----------------------------------------------------------------------------
void GameData::SetDebugData( DebugDataType type, gfl2::heap::HeapBase * pTempHeap, gfl2::heap::HeapBase * pTempDevHeap, int sex )
{
  GFL_PRINT("GameData::SetDebugData\n");

  gfl2::heap::HeapBase * heap = pTempHeap;
  //デバッグ開始時は一旦セーブデータを初期化する
  this->ClearSaveData( heap );
}

//----------------------------------------------------------------------------
  /**
   * @brief デバッグバトル開始用データ設定
   */
//----------------------------------------------------------------------------
void GameData::SetDebugBattleData( gfl2::heap::HeapBase * pTempHeap )
{
  GFL_PRINT("GameData::SetDebugBattleData\n");

  gfl2::heap::HeapBase * heap = pTempHeap;
  //デバッグ開始時は一旦セーブデータを初期化する
  this->ClearSaveData( heap );
}
#endif //PM_DEBUG

//============================================================================================
//
//
//    アクセス関数
//
//
//============================================================================================

GFL_NAMESPACE_END(GameSystem)


