#if PM_DEBUG
#if ENABLE_DEBUG_TERRAIN_BLOCK_CONTROL
/**
* @brief  デバッグ用カメラ位置調整イベント
* @file   FieldEventDebugTerrainBlockControl.cpp
* @author ikuta_junya
* @data   2015.05.23
*/

// gfl2
#include <Debug/include/gfl2_DebugPrint.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"

// field
#include "Field/FieldRo/include/Event/FieldEventDebugTerrainBlockControl.h"
#include "Field/FieldRo/include/FieldMap.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainBlock.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainLayoutData.h"

// field debug
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Debug/Menu/FieldDebugMenuTerrain.h"
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Event );


/**
* @brief コンストラクタ
*
* @param  pSystemHeap GameEventに渡すヒープ
*
* @return 無し
*/ 
EventDebugTerrainBlockControl::EventDebugTerrainBlockControl( gfl2::heap::HeapBase* pSystemHeap ) :

  GameSys::GameEvent( pSystemHeap )

  // 内部制御
  ,m_X( 0 )
  ,m_Z( 0 )
  ,m_Lod( 0 )

  // 内部システム
  // 外部オブジェクト
  // 外部システム
  ,m_pFieldMap( NULL )
  ,m_pSystemHeap( NULL )

{

}

/**
* @brief デストラクタ
*
* @return 無し
*/ 
EventDebugTerrainBlockControl::~EventDebugTerrainBlockControl()
{

}

/**
* @brief 初期化処理(インスタンス生成直後に実行する)
*
* @param  rDesc   初期化設定
*
* @return 無し
*/ 
void EventDebugTerrainBlockControl::Initialize( const InitializeDescription& rDesc )
{
  m_pSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  // m_pSystemHeap = rDesc.m_pHeap;
}

//--------------------------------------------------------------
/**
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @retval  true  起動してよい
* @retval  false 起動しない
*/
//--------------------------------------------------------------
bool EventDebugTerrainBlockControl::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventDebugTerrainBlockControl::InitFunc(GameSys::GameManager* gmgr)
{
  m_pFieldMap = gmgr->GetFieldmap();
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventDebugTerrainBlockControl::MainFunc(GameSys::GameManager* gmgr)
{
  // シーケンス制御値
  struct Seq
  {
    enum Value
    {
      INIT = 0,
      MAIN,
      TERM,
      TERM_WAIT,
      END,
    };
  };

  switch( this->GetSeqNo() )
  {
  case Seq::INIT:
    {
      Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();

      // 地形制御更新OFF
      pTerrainManager->SetBlockControlSkip( true );
      
      // 全部LOD
      ReqBlockLodAll();
      
      // 全部OFF
      SetBlockInvisibleAll( true );
      

      // デバッグメニューに追加
      Field::Debug::Menu::Terrain* pDebugMenuTerrain;
      pDebugMenuTerrain = m_pFieldMap->GetFieldDebugMenu()->GetTerrainMenu();
      gfl2::str::MsgData* pMsgData = m_pFieldMap->GetFieldDebugMenu()->GetMessageData();

      m_pBlockControlGrop = gfl2::debug::DebugWin_AddGroup( m_pHeap, pMsgData, msg_dmenu_field_terrain_block_ctrl, pDebugMenuTerrain->GetTopGroup() );

      gfl2::debug::DebugWin_AddItemValueInt( m_pBlockControlGrop, m_pHeap, pMsgData, msg_dmenu_field_terrain_block_ctrl_x, &m_X, 0, pTerrainManager->GetLayoutData()->GetBlockNumOfXAxis() - 1, 1 );
      gfl2::debug::DebugWin_AddItemValueInt( m_pBlockControlGrop, m_pHeap, pMsgData, msg_dmenu_field_terrain_block_ctrl_z, &m_Z, 0, pTerrainManager->GetLayoutData()->GetBlockNumOfZAxis() - 1, 1 );
      gfl2::debug::DebugWin_AddItemValueInt( m_pBlockControlGrop, m_pHeap, pMsgData, msg_dmenu_field_terrain_block_ctrl_lod, &m_Lod, 0, Field::Terrain::Block::LodType::MAX - 1, 1 );


      this->SetSeqNo( Seq::MAIN );
      break;
    } // case Seq::INIT:

  case Seq::MAIN:
    {
      GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
      gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
      gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

      if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
      {
        this->SetSeqNo( Seq::TERM );
        break;
      }

      // リクエスト更新
      SetBlockInvisibleAll( true );
      ReqBlockLodAll();

      // 選択中のモデルを制御
      Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
      pTerrainManager->GetBlock( m_X, m_Z )->ChangeLod( static_cast<Field::Terrain::Block::LodType::Value>( m_Lod ));

      // 望みのモデルになっていたら表示
      if( pTerrainManager->GetBlock( m_X, m_Z )->GetLod() == m_Lod )
      {
        pTerrainManager->GetBlock( m_X, m_Z )->SetDebugInvisibleBit( Field::Terrain::TerrainBlock::INVISIBLE_DEBUG_001, false );
      }

      break;
    } // case Seq::MAIN:

   case Seq::TERM:
    {
      // デバッグメニューから削除
      Field::Debug::Menu::Terrain* pDebugMenuTerrain;
      pDebugMenuTerrain = m_pFieldMap->GetFieldDebugMenu()->GetTerrainMenu();
      pDebugMenuTerrain->GetTopGroup()->RemoveItem( m_pBlockControlGrop );
      m_pBlockControlGrop = NULL;


      // 全部ON
      SetBlockInvisibleAll( true );

      // 全部LOD
      ReqBlockLodAll();
      
      // 地形制御更新ON
      Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
      pTerrainManager->SetBlockControlSkip( false );

      
      this->SetSeqNo( Seq::TERM_WAIT );
      break;

    } // case Seq::TERM

   case Seq::TERM_WAIT:
     {
       this->SetSeqNo( Seq::END );
       break;
     } // case Seq::TERM_WAIT

   case Seq::END:
     {
       // 制御点の上書きをクリア
       Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
       pTerrainManager->ClearExternalBlockControlPoint();
       return GameSys::GMEVENT_RES_FINISH;
     } // case Seq::END:

  } // switch( this->GetSeqNo() )

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventDebugTerrainBlockControl::EndFunc(GameSys::GameManager* gmgr)
{

}

void EventDebugTerrainBlockControl::SetBlockInvisibleAll( b32 flag )
{
  Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
  for( s32 x = 0; x < pTerrainManager->GetLayoutData()->GetBlockNumOfXAxis(); x++ )
  {
    for( s32 z = 0; z < pTerrainManager->GetLayoutData()->GetBlockNumOfZAxis(); z++ )
    {
      pTerrainManager->GetBlock( x, z )->SetDebugInvisibleBit( Field::Terrain::TerrainBlock::INVISIBLE_DEBUG_001, flag );
    }
  }
}
void EventDebugTerrainBlockControl::ReqBlockLodAll( void )
{
  Field::Terrain::TerrainManager* pTerrainManager = m_pFieldMap->GetTerrainManager();
  for( s32 x = 0; x < pTerrainManager->GetLayoutData()->GetBlockNumOfXAxis(); x++ )
  {
    for( s32 z = 0; z < pTerrainManager->GetLayoutData()->GetBlockNumOfZAxis(); z++ )
    {
      pTerrainManager->GetBlock( x, z )->ChangeLod( Field::Terrain::Block::LodType::MIDDLE );
    }
  }
}

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Field );

#endif // ENABLE_DEBUG_TERRAIN_BLOCK_CONTROL
#endif