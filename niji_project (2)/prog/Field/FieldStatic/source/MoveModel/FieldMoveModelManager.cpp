//=================================================================================================
/**
*  @file   FieldMoveModelManager.cpp
*  @brief  フィールド用 動作モデルマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <heap/include/gfl2_Heap.h>
#include <arc_def_index/arc_def.h>
#include "GameSys/include/GameManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "niji_reference_files/script/FieldPawnTypes.h"
#include "../../include/MoveModel/FieldMoveModelManager.h"
#include "../../include/MoveCode/FieldMoveCode_define.h"
#include "../../include/MoveCode/FieldMoveCodeManager.h"
#include "../../include/MoveCode/FieldMoveCodeBase.h"

// フィールドレジデント
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

// デバッグメニュー
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"

// savedata
#include "Savedata/include/FieldMoveModelSave.h"

// RTTIデバッグシステム
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"

// ジョインフェスタの定義
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
// バトル施設の定義
#include "Field/FieldStatic/include/BattleInst/BattleInstDefine.h"
// バトルフェス定義
#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"

#include <System/include/DressUp.h>
#include <System/include/GflUse.h>

// fureai
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"


#if PM_DEBUG
// debug
#include <fs/include/gfl2_Fs.h>
#include <fs/include/gfl2_FsPcUtil.h>
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldRo/include/Debug/FieldDebugMapJump.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

namespace Field{ namespace MoveModel {

#if defined(GF_PLATFORM_WIN32)
  static const u32 USUALLY_PLAYER_MODEL_HEAP_SIZE   = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;   // 通常フィールドでのプレイヤー動作モデル用のヒープサイズ
  static const u32 USUALLY_NPC_MODEL_HEAP_SIZE      = 1024 * 128;                                  // 通常フィールドでのNPC       動作モデル用のヒープサイズ
  static const u32 USUALLY_SUB_MODEL_HEAP_SIZE      = 1024 *  96;                                  // 通常フィールドでのサブオブジェクト    用のヒープサイズ
  static const u32 USUALLY_NPC_MODEL_HEAP_CNT       = FIELD_MOVE_MODEL_MAX - 2;                    // 通常フィールドでのNPC       動作モデル用のヒープ数( -2はプレイヤーとライドを引いた分 )
  static const u32 USUALLY_SUB_MODEL_HEAP_CNT       = FIELD_SUB_OBJECT_MAX;                        // 通常フィールドでのサブオブジェクト    用のヒープ数
#else
  static const u32 USUALLY_PLAYER_MODEL_HEAP_SIZE   = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;   // 通常フィールドでのプレイヤー動作モデル用のヒープサイズ
  static const u32 USUALLY_NPC_MODEL_HEAP_SIZE      = 1024 * 128;                                  // 通常フィールドでのNPC       動作モデル用のヒープサイズ
  static const u32 USUALLY_SUB_MODEL_HEAP_SIZE      = 1024 *  96;                                  // 通常フィールドでの動作モデルのサブオブジェクト用のヒープサイズ
  static const u32 USUALLY_NPC_MODEL_HEAP_CNT       = FIELD_MOVE_MODEL_MAX - 2;                    // 通常フィールドでのNPC       動作モデル用のヒープ数( -2はプレイヤーとライドを引いた分 )
  static const u32 USUALLY_SUB_MODEL_HEAP_CNT       = FIELD_SUB_OBJECT_MAX;                        // 通常フィールドでの動作モデルのサブオブジェクト用のヒープ数
#endif

#if defined(GF_PLATFORM_WIN32)
  static const u32 JOINFESTA_PLAYER_MODEL_HEAP_SIZE = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;   // ジョインフェスタでのプレイヤー動作モデル用のヒープサイズ
  static const u32 JOINFESTA_NPC_MODEL_HEAP_SIZE    = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;   // ジョインフェスタでのNPC       動作モデル用のヒープサイズ
  static const u32 JOINFESTA_SUB_MODEL_HEAP_SIZE    = 0;                          // ジョインフェスタでのサブオブジェクト    用のヒープサイズ
  static const u32 JOINFESTA_NPC_MODEL_HEAP_CNT     = NetApp::JoinFesta::JoinFestaDefine::JFMODEL_DISP_MAX + 10;  // ジョインフェスタでのNPC       動作モデル用のヒープ数
  static const u32 JOINFESTA_SUB_MODEL_HEAP_CNT     = 0;                          // ジョインフェスタでのサブオブジェクト    用のヒープ数
#else
  static const u32 JOINFESTA_PLAYER_MODEL_HEAP_SIZE = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;   // ジョインフェスタでのプレイヤー動作モデル用のヒープサイズ
  static const u32 JOINFESTA_NPC_MODEL_HEAP_SIZE    = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;   // ジョインフェスタでのNPC       動作モデル用のヒープサイズ
  static const u32 JOINFESTA_SUB_MODEL_HEAP_SIZE    = 0;                          // ジョインフェスタでのサブオブジェクト    用のヒープサイズ
  static const u32 JOINFESTA_NPC_MODEL_HEAP_CNT     = NetApp::JoinFesta::JoinFestaDefine::JFMODEL_DISP_MAX + 10;  // ジョインフェスタでのNPC       動作モデル用のヒープ数
  static const u32 JOINFESTA_SUB_MODEL_HEAP_CNT     = 0;                          // ジョインフェスタでのサブオブジェクト    用のヒープ数
#endif

  static const u32 BATTLEINST_PLAYER_MODEL_HEAP_SIZE = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;   // バトル施設でのプレイヤー動作モデル用のヒープサイズ
  static const u32 BATTLEINST_NPC_MODEL_HEAP_SIZE    = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;   // バトル施設でのNPC       動作モデル用のヒープサイズ
  static const u32 BATTLEINST_SUB_MODEL_HEAP_SIZE    = USUALLY_SUB_MODEL_HEAP_SIZE;                 // バトル施設でのサブオブジェクト    用のヒープサイズ
  static const u32 BATTLEINST_NPC_MODEL_HEAP_CNT     = BattleInst::BattleInstDefine::BINST_MODEL_DISP_MAX + 8;  // バトル施設でのNPC       動作モデル用のヒープ数
  static const u32 BATTLEINST_SUB_MODEL_HEAP_CNT     = BattleInst::BattleInstDefine::BINST_MODEL_DISP_MAX;  // バトル施設でのサブオブジェクト    用のヒープ数

  static const u32 BATTLEFES_PLAYER_MODEL_HEAP_SIZE = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;        // バトルフェスでのプレイヤー動作モデル用のヒープサイズ
  static const u32 BATTLEFES_NPC_MODEL_HEAP_SIZE = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;           // バトルフェスでのNPC       動作モデル用のヒープサイズ
  static const u32 BATTLEFES_SUB_MODEL_HEAP_SIZE = USUALLY_SUB_MODEL_HEAP_SIZE;                         // バトルフェスでのサブオブジェクト    用のヒープサイズ
  static const u32 BATTLEFES_NPC_MODEL_HEAP_CNT = BattleFes::BattleFesDefine::BFES_MODEL_DISP_MAX + 3;  // バトルフェスでのNPC       動作モデル用のヒープ数
  static const u32 BATTLEFES_SUB_MODEL_HEAP_CNT = BattleFes::BattleFesDefine::BFES_MODEL_DISP_MAX;      // バトルフェスでのサブオブジェクト    用のヒープ数

  static const u32 NPC_LARGE_HEAP_PLAYER_MODEL_HEAP_SIZE = System::DressUp::FIELD_HEAP_SIZE_INSTANCE;        // 使用メモリの大きいNPCを配置する世界でのプレイヤー動作モデル用のヒープサイズ
  static const u32 NPC_LARGE_HEAP_RIDE_MODEL_HEAP_SIZE = USUALLY_NPC_MODEL_HEAP_SIZE;                        // 使用メモリの大きいNPCを配置する世界でのライド動作モデル用のヒープサイズ
  static const u32 NPC_LARGE_HEAP_NPC_MODEL_HEAP_SIZE = 0x40000;                                             // 使用メモリの大きいNPCを配置する世界でのNPC       動作モデル用のヒープサイズ
  static const u32 NPC_LARGE_HEAP_SUB_MODEL_HEAP_SIZE = USUALLY_SUB_MODEL_HEAP_SIZE;                         // 使用メモリの大きいNPCを配置する世界でのサブオブジェクト    用のヒープサイズ
  static const u32 NPC_LARGE_HEAP_NPC_MODEL_HEAP_CNT = 15;                                                   // 使用メモリの大きいNPCを配置する世界でのNPC       動作モデル用のヒープ数
  static const u32 NPC_LARGE_HEAP_SUB_MODEL_HEAP_CNT = FIELD_SUB_OBJECT_MAX;                                 // 使用メモリの大きいNPCを配置する世界でのサブオブジェクト    用のヒープ数


static const u32 DYNAMIC_RES_LOAD_SIZE[FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_MAX] = 
{
  1024 * 544,   // DYNAMIC_RES_LOAD_INDEX_RIDE
  1024 *  96,   // DYNAMIC_RES_LOAD_INDEX_SUBOBJECT
  1024 * 400,   // DYNAMIC_RES_LOAD_INDEX_DUMMY
};

static const c8* DYNAMIC_RES_LOAD_HEAP_NAME[FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_MAX] = 
{
  "MoveModelDynamicRide",
  "MoveModelDynamicSubObject",
  "MoveModelDynamicDummy"
};

#if PM_DEBUG
  #define TERMINATE_TICK_DUMP 1
  #define ENABLE_POSITION_CHECK_FORCE true  //バミリチェックを全体に適用
#else
  #define TERMINATE_TICK_DUMP 0
#endif

enum
{
  STATIC_SUBOBJECT = FIELD_MOVE_MODEL_SUB_OBJECT_STATIC,
  DYNAMIC_SUBOBJECT = FIELD_MOVE_MODEL_SUB_OBJECT_DYNAMIC,
};

static const u8 MAX_EVENT_REQUEST_CNT = 255;

// 挙動設定
FieldMoveModelManager::SETTING FieldMoveModelManager::m_Setting =
{
   200.0f, //f32 cullingBoxSizeHalf;             // 通常カリングBOXのサイズ
   440.0f, //f32 cullingBoxSizeHalfForTrainer;   // トレーナー用カリングBOXのサイズ
   4200.0f * 1.41421f, //f32 cullingDispLength;              // 距離
};

#if PM_DEBUG

#if defined(GF_PLATFORM_WIN32)
  #include <shlwapi.h>
  #include <locale.h>
  #pragma comment( lib, "Shlwapi.lib" ) 
#endif

/* ------------------------------------------------------------------------- */
/**
* @brief  イベント終了時の座標チェック機能を利用するかどうか取得 static関数
*
* @return trueは利用する、falseなら利用しない
*/
/* ------------------------------------------------------------------------- */
b32 FieldMoveModelManager::CheckEnablePositionForEventScript( void )
{
#if defined(ENABLE_POSITION_CHECK_FORCE)

  return true;  //バミリチェックを全体に適用

#else  //defined(ENABLE_POSITION_CHECK_FORCE)

#if defined(GF_PLATFORM_CTR)
  bool bExist = false;
  #if GFL_FS_HIO_ENABLE
  gfl2::fs::PcUtil::CheckFileExist( &bExist, "R:/home/momiji/resource/field_data/script/CheckPlayerPosition.dat" );
  #endif
  if( bExist )
#elif defined(GF_PLATFORM_WIN32)
  if(PathFileExists(L"R:/home/momiji/resource/field_data/script/CheckPlayerPosition.dat"))
#endif
  {
    return true;
  }
  return false;
#endif  //defined(ENABLE_POSITION_CHECK_FORCE)
}

#endif  //PM_DEBUG

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::FieldMoveModelManager()
: m_pInstanceRootNode     (NULL)
, m_pFieldMoveModelRoot   (NULL)
, m_pMyRenderingPipeLine  (NULL)
, m_isUseCulling          (true)
, m_trafficCullingPriority(TRAFFIC_CULLING_PRIORITY_MIDDLE)
, m_dispMaxCnt            (DEFAULT_DISP_MAX_CNT)
, m_charaEventRequestCnt  (1) // @fix NMCat[1824] 関連 セーブ復帰や最初のフィールド生成時にイベント状態になっていない。
, m_pLocalAllocatorForDressModelResource(NULL)
, m_isSetupDressUpModelResourceManager(false)
, m_isPadding             (NULL)
, m_pAllocator            (NULL)
, m_pParentHeap           (NULL)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    m_pFieldMoveModel[i]        = NULL;
    m_pLocalHeapForMoveModel[i] = NULL;
  }

  for( u32 i=0 ; i<FIELD_SUB_OBJECT_MAX ; i++ )
  {
    m_pFieldSubobject[i]        = NULL;
    m_pLocalHeapForSubobject[i] = NULL;
  }

  for( int i=0; i<DYNAMIC_RES_LOAD_INDEX_MAX; ++i )
  {
    m_pLocalHeapForModelResource[i] = NULL;
    m_pLocalAllocatorForModelResource[i] = NULL;
    m_pLocalAllocatorForModelResourceID[i] = -1;
    m_pLocalAllocatorForIgnoreID[i]        = -1;
  }

  // 着せ替え用ヒープの作成
  m_pLocalHeapForDressModelResource      = GFL_CREATE_LOCAL_HEAP_NAME( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_BATTLE_TRANSITION ), System::DressUp::FIELD_HEAP_SIZE_MODEL + System::DressUp::FIELD_HEAP_SIZE_ALLOCATOR , gfl2::heap::HEAP_TYPE_EXP, false, "DressModelResource" );
  m_pLocalAllocatorForDressModelResource = GFL_NEW( m_pLocalHeapForDressModelResource ) System::nijiAllocator( m_pLocalHeapForDressModelResource );
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::~FieldMoveModelManager()
{
  TerminateMoveModelWork();
  TerminateMoveModelResource();
  TerminateSubobject();
  TerminateManagerWork();
  TerminateManagerResource();

  // 着せ替えリソースマネージャーの解放
  TerminateDressUpResourceManager();
  // 着せ替え用ヒープの破棄
  GFL_SAFE_DELETE( m_pLocalAllocatorForDressModelResource )
  GFL_DELETE_HEAP( m_pLocalHeapForDressModelResource );
  m_pLocalAllocatorForDressModelResource = NULL;
  m_pLocalHeapForDressModelResource      = NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(マネージャーのワーク)
*
*  @param  pInstanceRootNode    ツリーを形成するためのルートノード
*  @param  pFileHeap            ワークで作成に利用されるヒープ
*  @param  pAllocator           ワークで作成に利用されるアロケータ
*
*  @note   ここで利用されるヒープ/アロケータのアドレスは内部で保持してはいけません。どのアプリか関係なくいつもいきているためゲーム起動時のみ利用可
*
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::InitializeManagerWork(gfl2::renderingengine::scenegraph::instance::InstanceNode* pInstanceRootNode,
                                                  gfl2::heap::HeapBase*                                      pHeap,
                                                  gfl2::gfx::IGLAllocator*                                   pAllocator)
{
  // 動作モデルツリーの作成
  m_pFieldMoveModelRoot = GLNew(pAllocator) FieldMoveModel();
  m_pInstanceRootNode   = pInstanceRootNode;
  m_pInstanceRootNode->AddChild( m_pFieldMoveModelRoot );

  // 管理する動作モデルの作成
  m_pFieldMoveModel[FIELD_MOVE_MODEL_PLAYER]       = GLNew(pAllocator) FieldMoveModelPlayer(); 
  m_pFieldMoveModel[FIELD_MOVE_MODEL_RIDE_POKEMON] = GLNew(pAllocator) FieldMoveModel();
  for( u32 i=FIELD_MOVE_MODEL_NPC_START ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    m_pFieldMoveModel[i] = GLNew(pAllocator) FieldMoveModel();
  }

  // 管理するサブオブジェクトの作成
  for( u32 i=0 ; i<FIELD_SUB_OBJECT_MAX ; i++ )
  {
    m_pFieldSubobject[i] = GLNew(pAllocator) FieldSubobject();
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(マネージャーのリソース)
*
*  @param  MyRenderingPipeLine  レンダリングパイプライン
*  @param  pAllocator           アロケータ
*  @param  pHeap                ヒープ
*  @param  pFileManager         ファイルマネージャー
*  @param  dressUpParam         着せ替えパラメーター
*  @param  isExecuteJoinFesta   ジョインフェスタが起動中かどうか
*  @param  isExecuteBattleInst  バトル施設が起動中かどうか
*  @param  isExecuteBattleFes   バトルフェスが起動中かどうか
*  @param  isNpcHeapLargeZone   メモリ使用量の大きい動作モデルを配置した世界どうか（例：アクジキングの世界）
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::InitializeManagerResource(Field::MyRenderingPipeLine*   pMyRenderingPipeLine,
                                                      gfl2::gfx::IGLAllocator*      pAllocator,
                                                      gfl2::heap::HeapBase*         pHeap,
                                                      gfl2::fs::AsyncFileManager*   pFileManager,
                                                      poke_3d::model::DressUpParam  dressUpParam,
                                                      b32                           isExecuteJoinFesta,
                                                      b32                           isExecuteBattleInst,
                                                      b32                           isExecuteBattleFes,
                                                      b32                           isNPCHeapLargeZone )
{
  // 各ヒープの初期化
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    m_pLocalHeapForMoveModel[i] = NULL;
  }
   for( u32 i=0 ; i<FIELD_SUB_OBJECT_MAX ; i++ )
  {
    m_pLocalHeapForSubobject[i] = NULL;
  }

  // バトルフェス時
  if (isExecuteBattleFes)
  {
    m_pLocalHeapForMoveModel[0] = GFL_CREATE_LOCAL_HEAP_NAME(pHeap, BATTLEFES_PLAYER_MODEL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "PLAYER");  // プレイヤー
    m_pLocalHeapForMoveModel[1] = GFL_CREATE_LOCAL_HEAP_NAME(pHeap, BATTLEFES_NPC_MODEL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "RIDE");  // ライド
    for (u32 i = 0; i < BATTLEFES_NPC_MODEL_HEAP_CNT; i++)
    {
      m_pLocalHeapForMoveModel[i + 2] = GFL_CREATE_LOCAL_HEAP_NAME(pHeap, BATTLEFES_NPC_MODEL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "NPC");  // NPC
    }
    for (u32 i = 0; i < BATTLEFES_SUB_MODEL_HEAP_CNT; i++)
    {
      m_pLocalHeapForSubobject[i] = GFL_CREATE_LOCAL_HEAP_NAME(pHeap, BATTLEFES_SUB_MODEL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "SUB");  // サブオブジェクト
    }
  }
  // ジョインフェスタ時
  else if( isExecuteJoinFesta )
  {
    m_pLocalHeapForMoveModel[0]       = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, JOINFESTA_PLAYER_MODEL_HEAP_SIZE , gfl2::heap::HEAP_TYPE_EXP, false, "PLAYER" );  // プレイヤー
    m_pLocalHeapForMoveModel[1]       = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, JOINFESTA_NPC_MODEL_HEAP_SIZE    , gfl2::heap::HEAP_TYPE_EXP, false, "RIDE"   );  // ライド
    for( u32 i=0 ; i<JOINFESTA_NPC_MODEL_HEAP_CNT ; i++ )
    {
      m_pLocalHeapForMoveModel[i+2]   = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, JOINFESTA_NPC_MODEL_HEAP_SIZE    , gfl2::heap::HEAP_TYPE_EXP, false, "NPC" );  // NPC
    }
  }
  else if( isExecuteBattleInst )
  {
    m_pLocalHeapForMoveModel[0]       = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, BATTLEINST_PLAYER_MODEL_HEAP_SIZE , gfl2::heap::HEAP_TYPE_EXP, false, "PLAYER" );  // プレイヤー
    m_pLocalHeapForMoveModel[1]       = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, BATTLEINST_NPC_MODEL_HEAP_SIZE    , gfl2::heap::HEAP_TYPE_EXP, false, "RIDE"   );  // ライド
    for( u32 i=0 ; i<BATTLEINST_NPC_MODEL_HEAP_CNT ; i++ )
    {
      m_pLocalHeapForMoveModel[i+2]   = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, BATTLEINST_NPC_MODEL_HEAP_SIZE    , gfl2::heap::HEAP_TYPE_EXP, false, "NPC" );  // NPC
    }
    for( u32 i=0 ; i<BATTLEINST_SUB_MODEL_HEAP_CNT ; i++ )
    {
      m_pLocalHeapForSubobject[i] = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, BATTLEINST_SUB_MODEL_HEAP_SIZE , gfl2::heap::HEAP_TYPE_EXP, false, "SUB" );  // サブオブジェクト
    }
  }
  else if ( isNPCHeapLargeZone )
  {
    m_pLocalHeapForMoveModel[0]       = GFL_CREATE_LOCAL_HEAP_NAME( pHeap , NPC_LARGE_HEAP_PLAYER_MODEL_HEAP_SIZE   , gfl2::heap::HEAP_TYPE_EXP, false, "PLAYER" );  // プレイヤー
    m_pLocalHeapForMoveModel[1]       = GFL_CREATE_LOCAL_HEAP_NAME( pHeap , NPC_LARGE_HEAP_RIDE_MODEL_HEAP_SIZE , gfl2::heap::HEAP_TYPE_EXP, false, "RIDE" );  // ライド
    for( u32 i=0 ; i<NPC_LARGE_HEAP_NPC_MODEL_HEAP_CNT-1 ; i++ )//-1分は後方確保する（ロトム用）
    {
      m_pLocalHeapForMoveModel[i+2]   = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, NPC_LARGE_HEAP_NPC_MODEL_HEAP_SIZE , gfl2::heap::HEAP_TYPE_EXP, false, "NPC" );  // NPC
    }
    m_pLocalHeapForMoveModel[FIELD_MOVE_MODEL_EVENT_START] = GFL_CREATE_LOCAL_HEAP_NAME(pHeap, NPC_LARGE_HEAP_NPC_MODEL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "NPC");  // NPC ロトム用
    
    for( u32 i=0 ; i<NPC_LARGE_HEAP_SUB_MODEL_HEAP_CNT ; i++ )
    {
      m_pLocalHeapForSubobject[i] = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, NPC_LARGE_HEAP_SUB_MODEL_HEAP_SIZE , gfl2::heap::HEAP_TYPE_EXP, false, "SUB" );  // サブオブジェクト
    }
  }
  // 通常時
  else
  {
    m_pLocalHeapForMoveModel[0]       = GFL_CREATE_LOCAL_HEAP_NAME( pHeap       , USUALLY_PLAYER_MODEL_HEAP_SIZE                                       , gfl2::heap::HEAP_TYPE_EXP, false, "PLAYER" );  // プレイヤー
    m_pLocalHeapForMoveModel[1]       = GFL_CREATE_LOCAL_HEAP_NAME( pHeap       , USUALLY_NPC_MODEL_HEAP_SIZE                                          , gfl2::heap::HEAP_TYPE_EXP, false, "RIDE" );  // ライド
    for( u32 i=0 ; i<USUALLY_NPC_MODEL_HEAP_CNT ; i++ )
    {
      m_pLocalHeapForMoveModel[i+2]   = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, USUALLY_NPC_MODEL_HEAP_SIZE , gfl2::heap::HEAP_TYPE_EXP, false, "NPC" );  // NPC
    }
    for( u32 i=0 ; i<USUALLY_SUB_MODEL_HEAP_CNT ; i++ )
    {
      m_pLocalHeapForSubobject[i] = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, USUALLY_SUB_MODEL_HEAP_SIZE , gfl2::heap::HEAP_TYPE_EXP, false, "SUB" );  // サブオブジェクト
    }
  }

  for( int i=0; i<DYNAMIC_RES_LOAD_INDEX_MAX; ++i )
  {
    m_pLocalHeapForModelResource[i] = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, DYNAMIC_RES_LOAD_SIZE[i] , gfl2::heap::HEAP_TYPE_EXP, false, DYNAMIC_RES_LOAD_HEAP_NAME[i] );

    // 動的用リソースノード作成時に利用するアロケータを作成
    m_pLocalAllocatorForModelResource[i] = GFL_NEW( m_pLocalHeapForModelResource[i] ) System::nijiAllocator( m_pLocalHeapForModelResource[i] );
  }

  m_pParentHeap           = pHeap;
  m_pMyRenderingPipeLine  = pMyRenderingPipeLine;
  m_pAllocator            = pAllocator;
  m_charaModelFactory.Initialize(pFileManager, m_pParentHeap, ARCID_CHARA_MODEL_FIELD_NORMAL);
  m_charaModelFactory.SetupRandomFunc(System::GflUse::GetPublicRand);

  if( m_isSetupDressUpModelResourceManager )
  {
    m_dressUpModelResourceManager.Resume();
  }
  // 現在の服装の状況と着せ替えマネージャーに登録されている内容が違えば作成
  if( m_isSetupDressUpModelResourceManager && !m_dressUpModelResourceManager.IsSetupDressUpParts(dressUpParam) )
  {
    TerminateDressUpResourceManager();
  }

  // 着せ替えマネージャーが作成されていなければ作成
  InitializeDressUpResourceManager(pFileManager);
}

//-----------------------------------------------------------------------------
/**
*  @brief  解放(マネージャーのワーク)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateManagerWork(void)
{
  // 管理する動作モデルの削除
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    GFL_SAFE_DELETE( m_pFieldMoveModel[i] );
  }

  // 動作モデルツリーを解放
  m_pInstanceRootNode->RemoveChild( m_pFieldMoveModelRoot );
  GFL_SAFE_DELETE( m_pFieldMoveModelRoot );
  m_pInstanceRootNode = NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief  解放(マネージャーのリソース)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateManagerResource(void)
{
  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_LOCAL_HEAP_FOR_MOVEMODEL,
    TICK_LOCAL_HEAP_FOR_SUBOBJECT,
    TICK_CHARA_MODEL_FACTORY,
    TICK_LOCAL_HEAP_FOR_RIDE_MODEL_RESOURCE,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP   (FieldMoveModelManager,TICK_MAX);
  GFL_DEBUG_TICK_SET_FLAG(FieldMoveModelManager,true);
  GFL_DEBUG_TICK_RESET   (FieldMoveModelManager);
  //-----------------------------------------------

  // 動作モデル用ローカルヒープの削除
  GFL_DEBUG_TICK_START(FieldMoveModelManager, TICK_LOCAL_HEAP_FOR_MOVEMODEL, "LocalHeapForMoveModel");
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pLocalHeapForMoveModel[i] )
    {
      GFL_DELETE_HEAP( m_pLocalHeapForMoveModel[i] );
      m_pLocalHeapForMoveModel[i] = NULL;
    }
  }
  GFL_DEBUG_TICK_END(FieldMoveModelManager, TICK_LOCAL_HEAP_FOR_MOVEMODEL);

  // サブオブジェクト用ローカルヒープの削除
  GFL_DEBUG_TICK_START(FieldMoveModelManager, TICK_LOCAL_HEAP_FOR_SUBOBJECT, "LocalHeapForSubobject");
  for( u32 i=0 ; i<FIELD_SUB_OBJECT_MAX ; i++ )
  {
    if( m_pLocalHeapForSubobject[i] )
    {
      GFL_DELETE_HEAP( m_pLocalHeapForSubobject[i] );
      m_pLocalHeapForSubobject[i] = NULL;
    }
  }
  GFL_DEBUG_TICK_END(FieldMoveModelManager, TICK_LOCAL_HEAP_FOR_SUBOBJECT);

  // キャラモデルファクトリーの解放
  GFL_DEBUG_TICK_START(FieldMoveModelManager, TICK_CHARA_MODEL_FACTORY, "CharaModelFactory");
  m_charaModelFactory.Finalize();
  m_dressUpModelResourceManager.Suspend();
  GFL_DEBUG_TICK_END(FieldMoveModelManager, TICK_CHARA_MODEL_FACTORY);

  GFL_DEBUG_TICK_START(FieldMoveModelManager, TICK_LOCAL_HEAP_FOR_RIDE_MODEL_RESOURCE, "LocalHeapForRideModelResource");
  for( int i=0; i<DYNAMIC_RES_LOAD_INDEX_MAX; ++i )
  {
    if( m_pLocalHeapForModelResource[i] )
    {
      GFL_SAFE_DELETE( m_pLocalAllocatorForModelResource[i] );
      m_pLocalAllocatorForModelResource[i] = NULL;

      GFL_DELETE_HEAP( m_pLocalHeapForModelResource[i]  );
      m_pLocalHeapForModelResource[i]  = NULL;
    }
    m_pLocalAllocatorForModelResourceID[i] = -1;
    m_pLocalAllocatorForIgnoreID[i] = -1;
  }
  GFL_DEBUG_TICK_END(FieldMoveModelManager, TICK_LOCAL_HEAP_FOR_RIDE_MODEL_RESOURCE);

#if TERMINATE_TICK_DUMP
  gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
  GFL_DEBUG_TICK_DUMP_ALL(FieldMoveModelManager);
  gfl2::debug::ResetPrintFlag();
#endif

}

//-----------------------------------------------------------------------------
/**
*  @brief  初期化(管理している全ての動作モデルのワーク)
*
*  @param  fieldMoveModelId       動作モデルID
*  @param  pFieldMoveModelHeader  動作モデルリクエスト
*  @return 成否
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::CREATE_STATE FieldMoveModelManager::InitializeMoveModelWork(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeaderWork* pFieldMoveModelHeaderWork)
{
  // ワーク初期化
  if( !m_pFieldMoveModel[ (u32)fieldMoveModelId ]->IsSetupWork() )
  {
    // 動作モデルのワークを初期化
    m_pFieldMoveModel[ (u32)fieldMoveModelId ]->SetupWork( pFieldMoveModelHeaderWork );
    // 動作モデルをツリーに登録
    m_pFieldMoveModelRoot->AddChild( m_pFieldMoveModel[ (u32)fieldMoveModelId ] );
    // エッジIDを設定
    m_pFieldMoveModel[ (u32)fieldMoveModelId ]->SetEdgeId( (u32)fieldMoveModelId%4 * 2 ); // ID被りを成るべくすくなくするために2ずつ増やしている
    // 動作モデルにイベントリクエストを適用
    ApplyEventRequest(fieldMoveModelId);
    return CREATE_SUCCESS;
  }
  return CREATE_ERROR;
}

//-----------------------------------------------------------------------------
/**
*  @brief  初期化(管理している全ての動作モデルのリソース)
*
*  @param  fieldMoveModelId       動作モデルID
*  @param  pFieldMoveModelHeader  動作モデルリクエスト
*  @param  pDressUpModelResourceManager    着せ替えリソースマネージャ（通常はNULLでOK。ジョインフェスタのみ渡す。)
*  @return 成否
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::CREATE_STATE FieldMoveModelManager::InitializeMoveModelResource(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeaderResource* pFieldMoveModelHeaderResource, poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager)
{
  // リソース初期化
  if( !m_pFieldMoveModel[ (u32)fieldMoveModelId ]->IsSetupResource() )
  {
    // 着せ替えリソースマネージャがNULLの場合はメンバのものを使う
    //  memo:着せ替えリソースマネージャを外側が持っている場合がある。ichiraku
    if( pDressUpModelResourceManager == NULL )
    {
      pDressUpModelResourceManager = &m_dressUpModelResourceManager;
    }

    // 動作モデルのリソースを初期化
    m_pFieldMoveModel[ (u32)fieldMoveModelId ]->SetupResource( m_pLocalHeapForMoveModel[ (u32)fieldMoveModelId ] , m_pMyRenderingPipeLine , &m_charaModelFactory , pDressUpModelResourceManager , pFieldMoveModelHeaderResource );

    // 初期化した動作モデルがNPCなら
    Field::MoveModel::ApplicationDataForCommon* pCommonData = (Field::MoveModel::ApplicationDataForCommon*)( GetApplicationData( pFieldMoveModelHeaderResource->characterId ) );
    if( pCommonData->type == Field::MoveModel::TYPE_NPC )
    {
      // NPCにアタッチするサブオブジェクトが設定されているかどうか
      Field::MoveModel::ApplicationDataForNPC* pNpcData = (Field::MoveModel::ApplicationDataForNPC*)( pCommonData );
      if( pNpcData->subobjectId != 0 )
      {
        Field::MoveModel::FieldSubobjectHeader header;
        header.characterId = pNpcData->subobjectId;
        if( !SetSubobjectToMoveModel( m_pFieldMoveModel[ (u32)fieldMoveModelId ] , STATIC_SUBOBJECT , &header, false ) )
        {
          GFL_ASSERT(0);
        }

#if PM_DEBUG
        // サブオブジェクトの残ワーク数を取得
        u32 cnt = 0;
        for( u32 i=FIELD_SUB_OBJECT_AUTO ; i<FIELD_SUB_OBJECT_MAX ; i++ ) 
        { 
          if( !m_pFieldSubobject[i]->IsSetup() ) 
          {
            cnt++;
          } 
        }

        // 企画の方が想定したワークよりも少なくなったかどうか
        if( cnt <= 4 )
        {
          GFL_PRINT("=======================================================\n");
          GFL_PRINT("Dynamic用のSubobjectが%d個しか出せない状態になっています!!!\n",cnt);
          GFL_PRINT("=======================================================\n");
          GFL_ASSERT(0);
        }
#endif
      }
    }

    return CREATE_SUCCESS;
  }
  return CREATE_ERROR;
}

//-----------------------------------------------------------------------------
/**
*  @brief  初期化(管理している全てのサブオブジェクト)
*
*  @param  fieldSubobjectId       サブオブジェクトID
*  @param  pFieldSubobjectHeader  サブオブジェクトリクエスト
*  @return 成否
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::CREATE_STATE FieldMoveModelManager::InitializeSubobject(FIELD_SUB_OBJECT_ID fieldSubobjectId,const FieldSubobjectHeader* pFieldSubobjectHeader)
{
  if( !m_pFieldSubobject[ (u32)fieldSubobjectId ]->IsSetup() )
  {
    // サブオブジェクトを初期化
    m_pFieldSubobject[ (u32)fieldSubobjectId ]->Setup( m_pLocalHeapForSubobject[ (u32)fieldSubobjectId ] , m_pMyRenderingPipeLine , &m_charaModelFactory ,pFieldSubobjectHeader );
    // サブオブジェクトをツリーに登録
    m_pFieldMoveModelRoot->AddChild( m_pFieldSubobject[ (u32)fieldSubobjectId ] );
    return CREATE_SUCCESS;
  }
  return CREATE_ERROR;
}

//-----------------------------------------------------------------------------
/**
*  @brief  解放(管理している全ての動作モデルのワーク)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateMoveModelWork(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetupWork() )
    {
      // 動作モデルをツリーから削除
      m_pFieldMoveModelRoot->RemoveChild( m_pFieldMoveModel[i] );
      // 動作モデルワークを破棄
      m_pFieldMoveModel[i]->ClearWork();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  解放(管理している全ての動作モデルのリソース)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateMoveModelResource(void)
{
  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_MOVEMODEL_CLEAR_RESOURCE_FIELD,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP   (FieldMoveModelManager,TICK_MAX);
  GFL_DEBUG_TICK_SET_FLAG(FieldMoveModelManager,true);
  GFL_DEBUG_TICK_RESET   (FieldMoveModelManager);
  //-----------------------------------------------

  GFL_DEBUG_TICK_START(FieldMoveModelManager, TICK_MOVEMODEL_CLEAR_RESOURCE_FIELD, "MoveModelClearResource");
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if(  m_pFieldMoveModel[i]->IsSetupResource() )
    {
      // 動作モデルリソースを破棄
      m_pFieldMoveModel[i]->ClearResource();
    }
  }
  GFL_DEBUG_TICK_END(FieldMoveModelManager, TICK_MOVEMODEL_CLEAR_RESOURCE_FIELD);

#if TERMINATE_TICK_DUMP
  gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
  GFL_DEBUG_TICK_DUMP_ALL(FieldMoveModelManager);
  gfl2::debug::ResetPrintFlag();
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  解放(管理している全てのサブオブジェクト)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateSubobject(void)
{
  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_MOVEMODEL_SUBOBJECT_CLEAR_RESOURCE_FIELD,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP   (FieldMoveModelManager,TICK_MAX);
  GFL_DEBUG_TICK_SET_FLAG(FieldMoveModelManager,true);
  GFL_DEBUG_TICK_RESET   (FieldMoveModelManager);
  //-----------------------------------------------

  GFL_DEBUG_TICK_START(FieldMoveModelManager, TICK_MOVEMODEL_SUBOBJECT_CLEAR_RESOURCE_FIELD, "SubobjectClear");
  for( u32 i=0 ; i<FIELD_SUB_OBJECT_MAX ; i++ )
  {
    if(  m_pFieldSubobject[i]->IsSetup() )
    {
      // サブオブジェクトをツリーから削除
      m_pFieldMoveModelRoot->RemoveChild( m_pFieldSubobject[i] );
      // サブオブジェクトを破棄
      m_pFieldSubobject[i]->Clear();
    }
  }
  GFL_DEBUG_TICK_END(FieldMoveModelManager, TICK_MOVEMODEL_SUBOBJECT_CLEAR_RESOURCE_FIELD);

#if TERMINATE_TICK_DUMP
  gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
  GFL_DEBUG_TICK_DUMP_ALL(FieldMoveModelManager);
  gfl2::debug::ResetPrintFlag();
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  指定動作モデルの削除(リソースのみ)
*
*  @param  fieldMoveModelId  動作モデルID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateMoveModelResource(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
  if( m_pFieldMoveModel[ (u32)fieldMoveModelId ]->IsSetupResource() )
  {
    // 動作モデルリソースを破棄
    m_pFieldMoveModel[ (u32)fieldMoveModelId ]->ClearResource();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  指定動作モデルの削除(リソース/ワーク共に)
*
*  @param  fieldMoveModelId  動作モデルID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateMoveModelWorkResource(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
  if( m_pFieldMoveModel[ (u32)fieldMoveModelId ]->IsSetupResource() )
  {
    // 動作モデルリソースを破棄
    m_pFieldMoveModel[ (u32)fieldMoveModelId ]->ClearResource();
  }
  if( m_pFieldMoveModel[ (u32)fieldMoveModelId ]->IsSetupWork() )
  {
    // 動作モデルをツリーから削除
    m_pFieldMoveModelRoot->RemoveChild( m_pFieldMoveModel[ (u32)fieldMoveModelId ] );
    // 動作モデルワークを破棄
    m_pFieldMoveModel[ (u32)fieldMoveModelId ]->ClearWork();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  指定サブオブジェクトの削除
*
*  @param  fieldSubobjectId  サブオブジェクトID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateSubobject(FIELD_SUB_OBJECT_ID fieldSubobjectId)
{
  if( m_pFieldSubobject[ (u32)fieldSubobjectId ]->IsSetup() )
  {
    // サブオブジェクトをツリーから削除
    m_pFieldMoveModelRoot->RemoveChild( m_pFieldSubobject[ (u32)fieldSubobjectId ] );
    // サブオブジェクトを破棄
    m_pFieldSubobject[ (u32)fieldSubobjectId ]->Clear();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  初期化(着せ替えマネージャー)
*
*  @param  pFileManager  ファイルマネージャー
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::InitializeDressUpResourceManager(gfl2::fs::AsyncFileManager* pFileManager)
{
  if( !m_isSetupDressUpModelResourceManager )
  {
    m_dressUpModelResourceManager.Initialize(pFileManager, m_pLocalHeapForDressModelResource, System::DressUp::GetDressUpArcIdListField());
    m_isSetupDressUpModelResourceManager   = true;
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  解放(着せ替えマネージャー)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateDressUpResourceManager(void)
{
  if( m_isSetupDressUpModelResourceManager )
  {
    m_dressUpModelResourceManager.Finalize();
    m_isSetupDressUpModelResourceManager   = false;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  前更新(コリジョン以外の座標/姿勢の変更を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::PrevUpdate(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      m_pFieldMoveModel[i]->PrevUpdate();

      // 動的にモンスターボールをアタッチする必要があるか
      if     ( m_pFieldMoveModel[i]->IsNeedMonsterBall() )
      {
        if( IsLoaded( CHARA_MODEL_ID_OB0204_00 )  && m_charaModelFactory.GetApplicationData(CHARA_MODEL_ID_OB0204_00) )
        {
          Field::MoveModel::FieldSubobjectHeader header;
          header.characterId = CHARA_MODEL_ID_OB0204_00;
          SetSubobjectToMoveModel( m_pFieldMoveModel[i] , DYNAMIC_SUBOBJECT , &header, false );
        }
      }
      // 動的にライドギアをアタッチする必要があるか
      else if( m_pFieldMoveModel[i]->IsNeedRideGear() )
      {
        if( IsLoaded( CHARA_MODEL_ID_OB0001_00 ) && m_charaModelFactory.GetApplicationData(CHARA_MODEL_ID_OB0001_00) )
        {
          Field::MoveModel::FieldSubobjectHeader header;
          header.characterId = CHARA_MODEL_ID_OB0001_00;
          SetSubobjectToMoveModel( m_pFieldMoveModel[i] , DYNAMIC_SUBOBJECT , &header, false );
        }
      }
      // 動的にアタッチされたサブオブジェクトを外す
      else
      {
        m_pFieldMoveModel[i]->SetSubobject( DYNAMIC_SUBOBJECT,NULL,NULL );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  後更新(座標の決定後に必要な処理を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::PostUpdate(void)
{
  // 動作モデルの後更新
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      m_pFieldMoveModel[i]->PostUpdate(&m_charaModelFactory);

      // 動作モデルが持っているサブオブジェクトに描画状況を流し込む
      m_pFieldMoveModel[i]->PostUpdateSubobject();
    }
  }

  // サブオブジェクトを保持している動作モデルが削除されるとサブオブジェクト自体も削除する
  for( u32 i=0 ; i<FIELD_SUB_OBJECT_MAX ; i++ )
  {
    if( m_pFieldSubobject[i]->IsSetup() && m_pFieldSubobject[i]->IsEnableDelete() )
    {
      TerminateSubobject( (FIELD_SUB_OBJECT_ID)i );
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  サブオブジェクト更新
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::UpdateSubobject(void)
{
  // 親のアニメーション情報をサブオブジェクトに適用する
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      m_pFieldMoveModel[i]->UpdateSubobject();
    }
  }

  // サブオブジェクトの更新
  for( u32 i=0 ; i<FIELD_SUB_OBJECT_MAX ; i++ )
  {
    if( m_pFieldSubobject[i]->IsSetup() )
    {
      m_pFieldSubobject[i]->Update();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  カリング
*
*  @param  ビュープロジェクション行列
*  @param  カメラ座標
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::UpdateCulling(gfl2::math::Matrix44 viewProj, gfl2::math::Vector3 cameraPos, b32 isFieldCamera, s32 zoneId )
{
#if PM_DEBUG
  // デバッグメニューから
  Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
  if( pFieldDebugMenu )
  {
    m_isUseCulling = (b8)pFieldDebugMenu->isUseCullingForMoveModel();
  }
#endif

  // カリングをする
  if( m_isUseCulling )
  {
    // カリング用行列の作成
    gfl2::math::Matrix44 cullingMatrixTable[4];
    gfl2::math::AABB::Util::CalcFrustumPlanes( cullingMatrixTable, viewProj );

    // カメラから近い順に動作モデル配列を並びかえる
    static FieldMoveModel* pCullingSrc[FIELD_MOVE_MODEL_MAX];
    static FieldMoveModel* pCullingDst[FIELD_MOVE_MODEL_MAX];
    for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
    {
      if ( IsCullingTarget(i) )
      { 
        pCullingSrc[i] = m_pFieldMoveModel[i];
      }
      else
      {
        // プレイヤー・ライドポケモンはカリング対象から除外
        pCullingSrc[i] = NULL;
      }
      pCullingDst[i] = NULL;
    }

    s32 dstCnt = 0;

    // プレイヤーから一定距離以内の描画オンなトレーナーを先に集める
    for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
    {
      s32 enableTrainerIndex = GetEnableTrainerFromPlayer(&pCullingSrc[0],true);
      if( enableTrainerIndex >= 0 )
      {
        pCullingDst[dstCnt]             = pCullingSrc[enableTrainerIndex];
        pCullingSrc[enableTrainerIndex] = NULL;
        dstCnt++;
      }
    }

    // トレーナーが近場に配置されすぎている
    if( dstCnt > m_dispMaxCnt )
    {
      GFL_ASSERT_MSG(0,"error: Too put the trainer");
    }

    if( m_trafficCullingPriority == (u8)TRAFFIC_CULLING_PRIORITY_MIDDLE )
    {
      // 次にカメラから近い順で描画オンなNPC/トレーナーを集める
      for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
      {
        s32 nearestIndex = GetNearestMoveModelFromCamera(cameraPos,&pCullingSrc[0],true,false);
        if( nearestIndex >= 0 )
        {
          pCullingDst[dstCnt]       = pCullingSrc[nearestIndex];
          pCullingSrc[nearestIndex] = NULL;
          dstCnt++;
        }
      }
    }
    else if( m_trafficCullingPriority == (u8)TRAFFIC_CULLING_PRIORITY_HIGH )
    {
      // 次にカメラから近い順で描画オンなNPC(往来以外)/トレーナーを集める
      for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
      {
        s32 nearestIndex = GetNearestMoveModelFromCamera(cameraPos,&pCullingSrc[0],true,true);
        if( nearestIndex >= 0 )
        {
          pCullingDst[dstCnt]       = pCullingSrc[nearestIndex];
          pCullingSrc[nearestIndex] = NULL;
          dstCnt++;
        }
      }

      // 次にカメラから近い順で描画オンなNPC(往来)を集める
      for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
      {
        s32 nearestIndex = GetNearestMoveModelFromCamera(cameraPos,&pCullingSrc[0],true,false);
        if( nearestIndex >= 0 )
        {
          pCullingDst[dstCnt]       = pCullingSrc[nearestIndex];
          pCullingSrc[nearestIndex] = NULL;
          dstCnt++;
        }
      }
    }
    else
    {
      GFL_ASSERT(0);
    }

    // 次にプレイヤーから一定距離以内の描画オフなトレーナーを先に集める
    for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
    {
      s32 enableTrainerIndex = GetEnableTrainerFromPlayer(&pCullingSrc[0],false);
      if( enableTrainerIndex >= 0 )
      {
        pCullingDst[dstCnt]             = pCullingSrc[enableTrainerIndex];
        pCullingSrc[enableTrainerIndex] = NULL;
        dstCnt++;
      }
    }

    // 次にカメラから近い順で描画オフなNPC/トレーナーを集める
    for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
    {
      s32 nearestIndex = GetNearestMoveModelFromCamera(cameraPos,&pCullingSrc[0],false,false);
      if( nearestIndex >= 0 )
      {
        pCullingDst[dstCnt]       = pCullingSrc[nearestIndex];
        pCullingSrc[nearestIndex] = NULL;
        dstCnt++;
      }
    }

    // フラスタムカリング
    s32 dispCnt = 0;
    for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
    {
      if( !pCullingDst[i] || !pCullingDst[i]->IsSetup() )
      {
        continue;
      }

      // モデルのサイズやアニメーションを含め十分な大きさのAABBを作成する
      gfl2::math::Vector3 HALF_SIZE( m_Setting.cullingBoxSizeHalf, m_Setting.cullingBoxSizeHalf, m_Setting.cullingBoxSizeHalf );
      

      if( pCullingDst[i]->GetMoveCodeId() == FIELD_MOVE_CODE_TRAINER_PATH
       || pCullingDst[i]->GetMoveCodeId() == FIELD_MOVE_CODE_TRAINER_EYE )
      {
        // note：トレーナーの視線はEyeJointに関連しているので視線>AABBだと視線判定がおかしくなる
        //        400cm(視線の奥行き) + 40cm(プレイヤーの最大移動量)
        f32 cullingBoxSizeHalfForTrainer = GetCullingBoxSizeHalfForTrainer( isFieldCamera, zoneId );
        HALF_SIZE.x = cullingBoxSizeHalfForTrainer;
        HALF_SIZE.y = cullingBoxSizeHalfForTrainer;
        HALF_SIZE.z = cullingBoxSizeHalfForTrainer;
      }

      // ルナアーラはサイズがでかく、モーションも原点から大きく外れているので特別に大きなAABBを作成
      if( pCullingDst[i]->GetCharacterId() == CHARA_MODEL_ID_PM0874_00
       || pCullingDst[i]->GetCharacterId() == CHARA_MODEL_ID_PM0874_01 )
      {
        HALF_SIZE.x = 400.0f;
        HALF_SIZE.y = 700.0f;
        HALF_SIZE.z = 400.0f;
      }
      //momiji追加　光ドラゴンはサイズがでかく、モーションも原点から大きく外れているので特別に大きなAABBを作成
      if (pCullingDst[i]->GetCharacterId() == CHARA_MODEL_ID_PM0865_14)
      {
        HALF_SIZE.x = 700.0f;
        HALF_SIZE.y = 700.0f;
        HALF_SIZE.z = 400.0f;
      }

      // @fix MMcat[283]:ナッシー・アイランドにて、特定の位置でAナッシーが表示されなくなる対処。カリングAABBをAナッシーにあわせたものに変更
      if (pCullingDst[i]->GetCharacterId() == CHARA_MODEL_ID_PM0103_61)
      {
        // Aナッシーのサイズ+移動バッファ(200.0f)
        HALF_SIZE.x = 350.0f + 200.0f;
        HALF_SIZE.y = 800.0f + 200.0f;
        HALF_SIZE.z = 400.0f + 200.0f;
      }

      // 最大表示距離
      f32 DISP_MAX_LENGTH = GetCullingDispLength( isFieldCamera, zoneId );

      // フェローチェマップの岩が画面内で描画対象からはずれてしまうので特別に調整する（momijiで追加された処理）
      if (pCullingDst[i]->GetCharacterId() == CHARA_MODEL_ID_IT0008_00_RIDEROCK23
        || pCullingDst[i]->GetCharacterId() == CHARA_MODEL_ID_IT0009_00_RIDEROCK24)
      {
        HALF_SIZE.x += 400.0f;
        HALF_SIZE.y += 300.0f;
        HALF_SIZE.z += 400.0f;
        DISP_MAX_LENGTH += 1350.0f; // 表示距離制限を伸ばす（小さい岩も一番遠い配置状態で影響するのであわせて伸ばす）
      }

      //
      // 処理負荷対応
      //

      // カリング用AABBを強制的に小さくする
      if (pCullingDst[i]->CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_CULLING_AABB_MINIMUM))
      {
        f32 cullingBoxSizeHalf = 25.0f;
        HALF_SIZE.x = cullingBoxSizeHalf;
        HALF_SIZE.y = cullingBoxSizeHalf;
        HALF_SIZE.z = cullingBoxSizeHalf;
      }

      // 距離カリング用の長さを強制的に元に戻す
      if (pCullingDst[i]->CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_CULLING_LENGTH_OPT_DISABLE))
      {
        DISP_MAX_LENGTH = m_Setting.cullingDispLength;
      }

      gfl2::math::Vector3 maxOffset( pCullingDst[i]->GetLocalSRT().translate + HALF_SIZE );
      gfl2::math::Vector3 minOffset( pCullingDst[i]->GetLocalSRT().translate - HALF_SIZE );
      gfl2::math::AABB    aabb     ( minOffset,maxOffset );

      // カメラと自身との距離を取得
      gfl2::math::Vector3 def( pCullingDst[i]->GetLocalSRT().translate - cameraPos );

      b32 isCulling = false;

      if (pCullingDst[i]->CheckStateBit(FIELD_MOVE_MODEL_APP_CULLING_DISABLE))
      {
        // APPカリング無効

      }
      else
      {
        if ( !pCullingDst[i]->CheckStateBit(FIELD_MOVE_MODEL_DISP_CNT_CULLING_DISABLE ) && dispCnt >= m_dispMaxCnt) // 最大表示数制限
        {
          isCulling = true;
        }

        if ( def.Length() >= DISP_MAX_LENGTH ) // 最大表示距離制限
        {
          isCulling = true;
        }
      }
      if ( !isCulling
       && gfl2::math::AABB::Util::IsIntersectFrustum( cullingMatrixTable, aabb ) ) // フラスタムカリング
      {
        // カリングオフ
        pCullingDst[i]->DisableCulling();

        // 対象が持っているサブオブジェクトもカリングオフにする
        for( u32 j=0 ; j<FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; j++ )
        {
          if( pCullingDst[i]->GetSubobject(j) )
          {
            pCullingDst[i]->GetSubobject(j)->DisableCulling();
          }
        }

        if (!pCullingDst[i]->CheckStateBit(FIELD_MOVE_MODEL_DISP_CNT_CULLING_DISABLE) && !pCullingDst[i]->CheckStateBit(FIELD_MOVE_MODEL_APP_CULLING_DISABLE))
        {
          dispCnt++;
        }
      }
      else
      {
        // カリングオン
        pCullingDst[i]->EnableCulling();

        // 対象が持っているサブオブジェクトもカリングオンにする
        for( u32 j=0 ; j<FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; j++ )
        {
          if( pCullingDst[i]->GetSubobject(j) )
          {
            pCullingDst[i]->GetSubobject(j)->EnableCulling();
          }
        }
      }
    }
  }
  // カリングをしない
  else
  {
    for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
    {
      if( !m_pFieldMoveModel[i]->IsSetup() )
      {
        continue;
      }

      // カリングオフ
      m_pFieldMoveModel[i]->DisableCulling();

      // 対象が持っているサブオブジェクトもカリングオフにする
      for( u32 j=0 ; j<FIELD_MOVE_MODEL_SUB_OBJECT_CNT ; j++ )
      {
        if( m_pFieldMoveModel[i]->GetSubobject(j) )
        {
          m_pFieldMoveModel[i]->GetSubobject(j)->DisableCulling();
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  カメラビューから一番近い動作モデルのインデックスを返す
*
*  @param  カメラ座標
*  @param  動作モデル配列
*  @param  対象とする描画状況
*  @param  往来を無視するかどうか
*/
//-----------------------------------------------------------------------------
s32 FieldMoveModelManager::GetNearestMoveModelFromCamera(gfl2::math::Vector3 cameraPos,FieldMoveModel** ppFieldMoveModelSrc,b32 isTargetVisible,b32 isIgnoreTraffic)
{
  s32 nearestIndex  = -1;
  f32 nearestLength = 0.0f;

  for( s32 i=0; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( ppFieldMoveModelSrc[i] 
     && ppFieldMoveModelSrc[i]->IsSetup() 
     && ppFieldMoveModelSrc[i]->GetCharaDrawInstance()->IsVisible() == isTargetVisible )
    {
      // 往来を無視する
      if( isIgnoreTraffic && ppFieldMoveModelSrc[i]->GetMoveCodeId() == FIELD_MOVE_CODE_TRAFFIC )
      {
        continue;
      }

      gfl2::math::Vector3 def( ppFieldMoveModelSrc[i]->GetLocalSRT().translate - cameraPos );
      if( nearestIndex == -1 || def.Length() < nearestLength )
      {
        nearestIndex  = i;
        nearestLength = def.Length();
      }
    }
  }

  return nearestIndex;
}

//-----------------------------------------------------------------------------
/**
*  @brief  有効な動作モデル(視線トレーナー)のインデックスを返す
*
*  @param  動作モデル配列
*  @param  動作モデル配列の描画動作状況
*/
//-----------------------------------------------------------------------------
s32 FieldMoveModelManager::GetEnableTrainerFromPlayer(FieldMoveModel** ppFieldMoveModelSrc,b32 isVisibleActive)
{
  for( s32 i=0; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if(  ppFieldMoveModelSrc[i] 
     &&  ppFieldMoveModelSrc[i]->IsSetup()
     &&  ppFieldMoveModelSrc[i]->GetCharaDrawInstance()->IsVisible() == isVisibleActive
     && (ppFieldMoveModelSrc[i]->GetMoveCodeId() == FIELD_MOVE_CODE_TRAINER_PATH || ppFieldMoveModelSrc[i]->GetMoveCodeId() == FIELD_MOVE_CODE_TRAINER_EYE) )
    {
      const f32 DISTANCE_THRESHOLD = 500.0f;
      gfl2::math::Vector3 def( ppFieldMoveModelSrc[i]->GetLocalSRT().translate - m_pFieldMoveModel[FIELD_MOVE_MODEL_PLAYER]->GetLocalSRT().translate );
      if( def.Length() < DISTANCE_THRESHOLD )
      {
        return i;
      }
    }
  }

  return -1;
}

//-----------------------------------------------------------------------------
/**
*  @brief  カリングする距離を取得する
*/
//-----------------------------------------------------------------------------
f32 FieldMoveModelManager::GetCullingDispLength(b32 isFieldCamera, s32 zoneId) const
{
  if( isFieldCamera )
  {
    // 処理負荷対策
    switch (zoneId)
    {
    case Z_F0102_C0101: return 2900.0f;
    case Z_F0102_S0401: return 2900.0f;
    case Z_F0201_T0201: return 2900.0f;
    case Z_F0201_C0401: return 2500.0f;
    case Z_F0301_S0701: return 2900.0f;
    case Z_F0201I1701: return 2900.0f;
    case Z_F0201_S0901: return 2900.0f;
    case Z_F0102_S0101: return 2900.0f;
    case Z_F0102_S0201: return 2900.0f;
    case Z_F0102_T0101: return 2900.0f;
    case Z_F0201_C0201: return 2900.0f;
    case Z_F0102_C0102: return 2900.0f;
    case Z_F0201_S1001: return 2500.0f;
    case Z_F0201_S0201: return 2500.0f;
    case Z_F0201_C0301: return 2900.0f;
    case Z_F0301D0501: return 2900.0f;
    }

    return m_Setting.cullingDispLength;
  }
  else
  {
    return m_Setting.cullingDispLength;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  カリング判定用BOXサイズを取得する
*/
//-----------------------------------------------------------------------------
f32 FieldMoveModelManager::GetCullingBoxSizeHalfForTrainer(b32 isFieldCamera, s32 zoneId) const
{
  if (isFieldCamera)
  {
    // 処理負荷対策
    switch (zoneId)
    {
    case Z_F0201_S0201: return 200.0f;
    }

    return m_Setting.cullingBoxSizeHalfForTrainer;
  }
  else
  {
    return m_Setting.cullingBoxSizeHalfForTrainer;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  地面にキャラを引っ付ける
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::AdjustGround(void)
{
  // プレイヤーとライドポケモンは強制的に地面接地
  for( u32 i=FIELD_MOVE_MODEL_PLAYER ; i<FIELD_MOVE_MODEL_RIDE_POKEMON ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      m_pFieldMoveModel[i]->AdjustGround();
    }
  }

#if PM_DEBUG
  // 処理負荷チェックのためプレイヤー以外の接地処理をオフに
  Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
  if( pFieldDebugMenu && !pFieldDebugMenu->IsEnableAdjustGround() )
  {
    return;
  }
#endif
  
  // プレイヤー座標  
  gfl2::math::Vector3 playerPos2D( m_pFieldMoveModel[FIELD_MOVE_MODEL_PLAYER]->GetLocalSRT().translate.x,
                                   0.0f,
                                   m_pFieldMoveModel[FIELD_MOVE_MODEL_PLAYER]->GetLocalSRT().translate.z);

  // プレイヤーからこれだけはなれていれば確実に地面がない
  const f32 BLOCK_X_2_R = (4200 * 1.41421f) * 2.f;

  // NPCはプレイヤーと一定距離離れていたら地面接地を行わない
  for( u32 i=FIELD_MOVE_MODEL_NPC_START ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      gfl2::math::Vector3 npcPos2D( m_pFieldMoveModel[i]->GetLocalSRT().translate.x,
                                    0.0f,
                                    m_pFieldMoveModel[i]->GetLocalSRT().translate.z);

      gfl2::math::Vector3 def2D( playerPos2D - npcPos2D );
      if( def2D.Length() > BLOCK_X_2_R )
      {
        continue;
      }

      m_pFieldMoveModel[i]->AdjustGround();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルの座標/姿勢をダイナミックアクターの座標/姿勢に適用する
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::ApplyMoveModelToActor(FieldMoveModelCollisionType colType)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      m_pFieldMoveModel[i]->ApplyMoveModelToActor(colType);
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルの座標/姿勢をスタティックアクターの座標/姿勢に適用する
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::ApplyMoveModelToActor(FieldMoveModelStaticCollisionType colType)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      m_pFieldMoveModel[i]->ApplyMoveModelToActor(colType);
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  コリジョンアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::ApplyActorToMoveModel(FieldMoveModelCollisionType colType)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      m_pFieldMoveModel[i]->ApplyActorToMoveModel(colType);
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  全てのコリジョンアクターが衝突を解決しているかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::CheckCollisionSolved(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      m_pFieldMoveModel[i]->CheckCollisionSolved();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  使われていない動作モデルの要素を返す 
*
*  @return 動作モデルの要素(FIELD_MOVE_MODEL_MAXの場合はエラー)
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModelManager::GetFieldMoveModelIndexFromFreeSpace(void) 
{
  int start = FIELD_MOVE_MODEL_NPC_START;
  int end = FIELD_MOVE_MODEL_NPC_END;

  // テストゾーンでは、30体を担保する。
#if PM_DEBUG
  if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident() )
  {
    ZoneID zoneID = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident()->DebugGetZone();
/*
    if(zoneID >= Z_TESTPL1701)
    {
      end = FIELD_MOVE_MODEL_EVENT_END;
    }
*/
  }
#endif

  for( u32 i=start ; i<=end; i++ ) 
  { 
    if( !(m_pFieldMoveModel[i]->IsSetupWork() || m_pFieldMoveModel[i]->IsSetupResource()) ) 
    { 
      return i; 
    } 
  }
  return FIELD_MOVE_MODEL_MAX; 
}


//-----------------------------------------------------------------------------
/**
*  @brief  使われていないイベント用動作モデルの要素を返す 
*
*  @return 動作モデルの要素(FIELD_MOVE_MODEL_MAXの場合はエラー)
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModelManager::GetFieldMoveEventModelIndexFromFreeSpace(void)
{
  for( u32 i=FIELD_MOVE_MODEL_EVENT_START ; i<=FIELD_MOVE_MODEL_EVENT_END; i++ ) 
  { 
    if( !(m_pFieldMoveModel[i]->IsSetupWork() || m_pFieldMoveModel[i]->IsSetupResource()) ) 
    { 
      return i; 
    } 
  }
  return FIELD_MOVE_MODEL_MAX; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  使われていないサブオブジェクトの要素を返す 
*
*  @return 動作モデルの要素(FIELD_SUB_OBJECT_MAXの場合はエラー)
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModelManager::GetFieldSubobjectIndexFromFreeSpace(void) 
{ 
  for( u32 i=FIELD_SUB_OBJECT_AUTO ; i<FIELD_SUB_OBJECT_MAX ; i++ ) 
  { 
    if( !m_pFieldSubobject[i]->IsSetup() ) 
    { 
      return i; 
    } 
  }
  return FIELD_SUB_OBJECT_MAX; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  イベントID(識別用のユニークなID)から動作モデルの要素を返す 
*
*  @param  eventId               イベントID
*  @return 動作モデルの要素(FIELD_MOVE_MODEL_MAXの場合はエラー)
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModelManager::GetFieldMoveModelIndexFromEventId(u32 eventId) 
{ 
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( m_pFieldMoveModel[i]->IsSetupWork() || m_pFieldMoveModel[i]->IsSetupResource() )
    {
      if( m_pFieldMoveModel[i]->GetEventId() == eventId ) 
      { 
        return i; 
      } 
    }
  }
  return FIELD_MOVE_MODEL_MAX; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルにサブオブジェクトをセットする
*
*  @param  pMoveModel               サブオブジェクトをセットしたい動作モデル
*  @param  setIndex                 動作モデルの何番目のサブオブジェクトとするか
*  @param  header                   動作モデルにセットしたいサブオブジェクトのヘッダー
*  @param  isManual                 専用ワークを使うかどうか
*  @return trueならセット成功、falseならセット失敗
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelManager::SetSubobjectToMoveModel(FieldMoveModel* pMoveModel,u32 setIndex,Field::MoveModel::FieldSubobjectHeader* pHeader,b32 isManual)
{
  if(!pHeader)
  {
    return false;
  }

  u32 subobjectIndex = 0;
  if( isManual )
  {
    // 専用ワークを取得
    subobjectIndex = FIELD_SUB_OBJECT_MANUAL;
    if( m_pFieldSubobject[subobjectIndex]->IsSetup() ) 
    { 
      return false; 
    } 
  }
  else
  {
    // まだ利用されていないワークを検索
    subobjectIndex = GetFieldSubobjectIndexFromFreeSpace();
    if( subobjectIndex == FIELD_SUB_OBJECT_MAX )
    {
      return false;
    }
  }

  // 動作モデルが持てるサブオブジェクト数を超えている
  if( FIELD_MOVE_MODEL_SUB_OBJECT_CNT <= setIndex )
  {
    return false;
  }

  // 既に同内容がアタッチされていればスルーする
  if( pMoveModel->GetSubobject(setIndex)
   && pMoveModel->GetSubobject(setIndex)->GetHeader().characterId == pHeader->characterId )
  {
    return false;
  }

  // 指定されたキャラIDがサブオブジェクトとして登録されているかどうか
  Field::MoveModel::ApplicationDataForCommon* pCommonData = (Field::MoveModel::ApplicationDataForCommon*)( GetApplicationData( pHeader->characterId ) );
  if( pCommonData->type != Field::MoveModel::TYPE_SUBOBJECT )
  {
    return false;
  }

  // サブオブジェクトの設定
  InitializeSubobject( (Field::MoveModel::FIELD_SUB_OBJECT_ID)subobjectIndex , pHeader );

  // アタッチするジョイント名を取得
  Field::MoveModel::ApplicationDataForSubobject* pSubobjectData = (Field::MoveModel::ApplicationDataForSubobject*)( pCommonData );
  c8 jointName[Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE];
  for( u32 i=0; i<Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE; i++ ){ Field::MoveModel::utf16_to_ascii(pSubobjectData->jointName[i],&jointName[i]); }

  // 動作モデルに持たせる
  pMoveModel->SetSubobject( setIndex,GetFieldSubobject(subobjectIndex),jointName );
  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  有効な全動作モデルをイベント状態(アクションコマンド、動作コードを共に動かさない)にする
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::AddEventRequestAll(void)
{
  m_charaEventRequestCnt++;

  // 明らかにおかしい
  if( m_charaEventRequestCnt >= MAX_EVENT_REQUEST_CNT )
  {
    GFL_ASSERT(0);
  }

  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  {
    if( m_pFieldMoveModel[i]->IsSetup() )
    {
      ApplyEventRequest((FIELD_MOVE_MODEL_ID)i);
      // イベントの実行タイミングを考慮し、リクエストがきたら即時に動作コードをサスペンド状態にしなければいけない
      // でなければイベント開始時、"イベント実行->動作コードのサスペンド->イベント実行 ..."というようにサスペンドが1フレーム遅れる
      m_pFieldMoveModel[i]->MoveCodeResumeSuspend();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  有効な全動作モデルをイベント状態(アクションコマンド、動作コードを共に動かさない)から解除する
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::PullEventRequestAll(void)
{
  if( m_charaEventRequestCnt >= 1 )
  {
    m_charaEventRequestCnt--;
  }

  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    ApplyEventRequest((FIELD_MOVE_MODEL_ID)i);
    // 動作コードのレジューム状態は即座にしなくともよい
    //m_pFieldMoveModel[i]->MoveCodeResumeSuspend();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルにイベントリクエストを適用
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::ApplyEventRequest(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
  for( u32 cnt=0 ; cnt<(u32)MAX_EVENT_REQUEST_CNT ; cnt++ )
  {
    if( !m_pFieldMoveModel[fieldMoveModelId] 
     || !m_pFieldMoveModel[fieldMoveModelId]->IsSetupWork() )
    {
      break;
    }

    if     ( m_pFieldMoveModel[fieldMoveModelId]->GetEventRequestCnt() > m_charaEventRequestCnt )
    {
      m_pFieldMoveModel[fieldMoveModelId]->PullEventRequest();
    }
    else if( m_pFieldMoveModel[fieldMoveModelId]->GetEventRequestCnt() < m_charaEventRequestCnt )
    {
      m_pFieldMoveModel[fieldMoveModelId]->AddEventRequest();
    }
    else
    {
      break;
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  プレイヤーのモーションIDをライド状況にあったモーションIDに変換する
*
*  @param  プレイヤーのモーションID
*  @param  ライドポケモンのキャラID
*  @param  指定ライドポケモンに合ったモーションID
*  @param  指定ライドポケモンに合ったモーションIDの再生終了フレーム
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::ConvertPlayerMotionIdToRideMotionId(u32 playerMotionId,u32 rideCharaId,u32* pOutRideMotionId,s32* pOutRideMotionEndFrame)
{
  RideMotionConvertData * _pRideMotionConvertData = NULL;

  // 未初期化
  {
    // ライド中のモーションIDの変換リストを初期化
    s32* pRideMotionConvertData = (s32*)( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident()->GetResource( BL_IDX_FIELD_RESIDENT__RIDE_MOTION_CONVERT_DATA_BIN ) );
    pRideMotionConvertData++;
    _pRideMotionConvertData = reinterpret_cast<RideMotionConvertData*>( pRideMotionConvertData );
  }

  // リストに設定がなければ変換をせずにそのまま再生する
  *pOutRideMotionId       = playerMotionId;
  *pOutRideMotionEndFrame = -1;

  // 送られてきたキャラIDがライドポケモンかどうか
  u32 rideIndex =  FieldMoveModelPlayer::GetRideIndex( rideCharaId );
  if( rideIndex == FieldMoveModelPlayer::RIDE_POKEMON_MAX_CNT )
  {
    return;
  }

  // ライドポケモンに合ったモーションIDに変換する
  for( u32 i=0 ; i<RIDE_MOTION_CONVER_DATA_CNT ; i++ )
  {
    if( _pRideMotionConvertData[i].playerMotionId != -1
     && _pRideMotionConvertData[i].playerMotionId == playerMotionId )
    {
      *pOutRideMotionId       = _pRideMotionConvertData[i].rideMotionId      [ rideIndex ];
      *pOutRideMotionEndFrame = _pRideMotionConvertData[i].rideMotionEndFrame[ rideIndex ];
      return;
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタに自身のインスタンスを登録
*
*  @param  エディタに表示したい動作モデルID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::CreateRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
#if USE_RTTI_DEBUG
  if( fieldMoveModelId == FIELD_MOVE_MODEL_PLAYER ) { m_pFieldMoveModel[ (u32)fieldMoveModelId ]->CreateRttiEditorPage( L"プレイヤー"); }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタから自身のインスタンスを削除
*
*  @param  エディタから削除したい動作モデルID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::DeleteRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
#if USE_RTTI_DEBUG
  if( fieldMoveModelId == FIELD_MOVE_MODEL_PLAYER ) { m_pFieldMoveModel[ (u32)fieldMoveModelId ]->DeleteRttiEditorPage(); }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルの状況をセーブデータに書き込む
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::SerializeSaveData( Savedata::FieldMoveModelSave* pFieldMoveModelSave )
{
  // フィールドマップがないとセーブをしない
  // note:不思議な贈り物でフィールドマップがない状態でのセーブがあり
  if( !GFL_SINGLETON_INSTANCE( GameSys::GameManager ) )
  {
    return;
  }

  FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( m_pFieldMoveModel[ FIELD_MOVE_MODEL_PLAYER ] );
  if( pPlayer && pPlayer->IsSetupWork() )
  {
    const FieldMoveCodeBase *pMoveCode = Field::MoveModel::FieldMoveCodeManager::GetMoveCode( pPlayer->GetMoveCodeId() );
    pMoveCode->Retirement( pPlayer->GetMoveCodeWork() ,true );
    pFieldMoveModelSave->returnDataForMoveModel.player.moveCodeId       = pPlayer->GetMoveCodeId();
    pFieldMoveModelSave->returnDataForMoveModel.player.eventId          = pPlayer->GetEventId();
    pFieldMoveModelSave->returnDataForMoveModel.player.posForReturn     = pPlayer->GetLocalSRT().translate;
    pFieldMoveModelSave->returnDataForMoveModel.player.quaForReturn     = pPlayer->GetLocalSRT().rotation;
    pFieldMoveModelSave->returnDataForMoveModel.player.zoneId           = pPlayer->GetZoneId();
    if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap() )
    {
      pFieldMoveModelSave->returnDataForMoveModel.player.rideEventId      = pPlayer->IsRideMoveModel() ? pPlayer->GetOnMoveModel()->GetEventId()     : Field::FIELD_EVENTID_NULL;
      pFieldMoveModelSave->returnDataForMoveModel.player.rideCharacterId  = pPlayer->IsRideMoveModel() ? pPlayer->GetOnMoveModel()->GetCharacterId() : 0;
    }
    else
    {
      pFieldMoveModelSave->returnDataForMoveModel.player.rideEventId      = pPlayer->IsRideMoveModelPrevClearResource() ? pPlayer->GetRideMoveModelHeaderWorkPrevClearResource().eventId         : Field::FIELD_EVENTID_NULL;
      pFieldMoveModelSave->returnDataForMoveModel.player.rideCharacterId  = pPlayer->IsRideMoveModelPrevClearResource() ? pPlayer->GetRideMoveModelHeaderResourcePrevClearResource().characterId : 0;
      if( pPlayer->IsRideMoveModelPrevClearResource() )
      {
        const u32 RIDE_OFFSET = FieldMoveModelPlayer::GetRideIndex( pPlayer->GetRideMoveModelHeaderResourcePrevClearResource().characterId );
        pFieldMoveModelSave->returnDataForMoveModel.player.moveCodeId  = (FIELD_MOVE_CODE_ID)( FieldMoveModelPlayer::RIDE_POKEMON_MOVE_CODE_ARRAY[ RIDE_OFFSET ] );
      }
    }

    for( u32 i=0 ; i<FIELD_MOVE_CODE_WORK_SIZE/4 ; i++ )
    {
      pFieldMoveModelSave->returnDataForMoveModel.player.moveCodeWork[i] = pPlayer->GetMoveCodeWork()->work[i];
    }
  }

  const u32 START_NPC_INDEX = FIELD_MOVE_MODEL_NPC_START; // @fix GFNMcat[1393] Rotomイベント用のワークを確保した為変更。
  for( u32 i=START_NPC_INDEX ; i<=FIELD_MOVE_MODEL_NPC_END ; i++ )
  {
    if(  m_pFieldMoveModel[i] 
     &&  m_pFieldMoveModel[i]->IsSetupWork() 
     && !m_pFieldMoveModel[i]->CheckStateBit( FIELD_MOVE_MODEL_STATEBIT_NOSERIALIZE ) )
    {
      const FieldMoveCodeBase *pMoveCode = Field::MoveModel::FieldMoveCodeManager::GetMoveCode( m_pFieldMoveModel[i]->GetMoveCodeId() );
      pMoveCode->Retirement( m_pFieldMoveModel[i]->GetMoveCodeWork() ,true );
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].moveCodeId   = m_pFieldMoveModel[i]->GetMoveCodeId();
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].eventId      = m_pFieldMoveModel[i]->GetEventId();
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].posForReturn = m_pFieldMoveModel[i]->GetLocalSRT().translate;
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].quaForReturn = m_pFieldMoveModel[i]->GetLocalSRT().rotation;
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].zoneId       = m_pFieldMoveModel[i]->GetZoneId();
      for( u32 j=0 ; j<FIELD_MOVE_CODE_WORK_SIZE/4 ; j++ )
      {
        pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].moveCodeWork[j] = m_pFieldMoveModel[i]->GetMoveCodeWork()->work[j];
      }
    }
    else
    {
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].moveCodeId   = FIELD_MOVE_CODE_NONE;
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].eventId      = Field::FIELD_EVENTID_NULL;
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].posForReturn = gfl2::math::Vector3(0.0f,0.0f,0.0f);
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].quaForReturn = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);
      pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].zoneId       = ZONEID_ERROR;
      for( u32 j=0 ; j<FIELD_MOVE_CODE_WORK_SIZE/4 ; j++ )
      {
        pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].moveCodeWork[j] = 0;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルの状況をセーブデータから受け取る
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::DeserializeSaveData( Savedata::FieldMoveModelSave* pFieldMoveModelSave )
{
  // 動作モデルワークを完全に初期化する
  TerminateMoveModelResource();
  TerminateMoveModelWork();

  // セーブデータからのプレイヤー
  {
    // セーブデータからワークヘッダーの作成
    Field::MoveModel::FieldMoveModelHeaderWork headerWork;
    headerWork = Savedata::FieldMoveModelHeaderFactoryFromReturnData::CreatePlayerFieldMoveModelHeader(&pFieldMoveModelSave->returnDataForMoveModel.player);
    headerWork.moveCodeId = FIELD_MOVE_CODE_PLAYER;

    // ヘッダーからワークの初期化(既に初期化されているのならスルーされます)
    InitializeMoveModelWork( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER , &headerWork );

    // 動作コードワークを復元
    for( u32 i=0 ; i<FIELD_MOVE_CODE_WORK_SIZE/4 ; i++ )
    { 
      GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER )->GetMoveCodeWork()->work[i] = pFieldMoveModelSave->returnDataForMoveModel.player.moveCodeWork[i];
    }
    GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER )->GetMoveCodeWork()->isSetupWorkFromSaveData = true;

    // 動作モデル内のワークからリソースを復帰させる処理を強制的に走らせる
    //  note: ワークを利用したリソースの復帰は、"ワークとリソースが両方を作成"→"リソースだけを削除"→"再度リソースを作成" した場合に限り走る
    GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER )->EnableReturnFromWork();

    // ライド復元
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( m_pFieldMoveModel[ FIELD_MOVE_MODEL_PLAYER ] );
    if( pFieldMoveModelSave->returnDataForMoveModel.player.moveCodeId != FIELD_MOVE_CODE_PLAYER )
    {
      Field::MoveModel::FieldMoveModelHeaderWork     headerWork;
      Field::MoveModel::FieldMoveModelHeaderResource headerResource;
      headerWork.eventId         = pFieldMoveModelSave->returnDataForMoveModel.player.rideEventId;
      headerWork.moveCodeId      = Field::MoveModel::FIELD_MOVE_CODE_NONE;
      headerResource.characterId = pFieldMoveModelSave->returnDataForMoveModel.player.rideCharacterId;
      pPlayer->SetRideMoveModelHeader( headerWork,headerResource );
    }
  }

  const u32 START_NPC_INDEX = FIELD_MOVE_MODEL_NPC_START; 
  for( u32 i=START_NPC_INDEX ; i<=FIELD_MOVE_MODEL_NPC_END ; i++ )// @fix GFNMcat[1393] Rotomイベント用のワークを確保した為変更。
  {
    // セーブデータからワークヘッダーの作成
    Field::MoveModel::FieldMoveModelHeaderWork headerWork;
    headerWork = Savedata::FieldMoveModelHeaderFactoryFromReturnData::CreateNpcFieldMoveModelHeader(&pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX]);
    if( !((headerWork.eventId == Field::FIELD_EVENTID_NULL) && (headerWork.moveCodeId == FIELD_MOVE_CODE_NONE) && (headerWork.zoneId == ZONEID_ERROR)) )
    {
      // ヘッダーからの初期化(既に初期化されているのならスルーされます)
      u32 modelIndex = GetFieldMoveModelIndexFromFreeSpace();
      InitializeMoveModelWork( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( modelIndex ) , &headerWork );

      // 動作コードワークを復元
      for( u32 j=0 ; j<FIELD_MOVE_CODE_WORK_SIZE/4 ; j++ )
      {
        m_pFieldMoveModel[ modelIndex ]->GetMoveCodeWork()->work[j] = pFieldMoveModelSave->returnDataForMoveModel.npc[i-START_NPC_INDEX].moveCodeWork[j];
      }
      m_pFieldMoveModel[ modelIndex ]->GetMoveCodeWork()->isSetupWorkFromSaveData = true;

      // 動作モデル内のワークからリソースを復帰させる処理を強制的に走らせる
      //  note: ワークを利用したリソースの復帰は、"ワークとリソースが両方を作成"→"リソースだけを削除"→"再度リソースを作成" した場合に限り走る
      Field::MoveModel::FieldMoveModel* pFieldMoveModel = GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( modelIndex ) );
      if( pFieldMoveModel )
      {
        pFieldMoveModel->EnableReturnFromWork();
      }

    }
  }
}


//=====================================================================================================================
//! @brief poke_3d::model::CharaModelManagerのラッパー部分
//=====================================================================================================================

//-----------------------------------------------------------------------------
/**
*  @brief  指定キャラクターリソースの非同期ロード開始  動的確保版
*
*  @param  dynamic_load_index ダイナミック読み込みインデックス
*  @param  characterId       キャラクターID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::LoadDynamicAsync(DYNAMIC_RES_LOAD_INDEX dynamic_load_index, u32 characterId)
{
  if( dynamic_load_index < DYNAMIC_RES_LOAD_INDEX_MAX )
  {
    if( IsLoaded(characterId) )
    {
      m_pLocalAllocatorForIgnoreID[dynamic_load_index] = (s32)characterId;
    }
    else
    {
      m_pLocalAllocatorForModelResourceID[dynamic_load_index] = characterId;
      m_charaModelFactory.LoadModelAsync(m_pLocalHeapForModelResource[dynamic_load_index], characterId);
    }
  }
  else
  {
    GFL_ASSERT(dynamic_load_index < DYNAMIC_RES_LOAD_INDEX_MAX);
    if( IsLoaded(characterId) )
    {
      m_pLocalAllocatorForIgnoreID[DYNAMIC_RES_LOAD_INDEX_EVENT] = (s32)characterId;
    }
    else
    {
      m_pLocalAllocatorForModelResourceID[DYNAMIC_RES_LOAD_INDEX_EVENT] = characterId;
      m_charaModelFactory.LoadModelAsync(m_pLocalHeapForModelResource[DYNAMIC_RES_LOAD_INDEX_EVENT], characterId);
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  指定キャラクターリソースノードの作成
*
*  @param  characterId       キャラクターID
*  @return 成否
*/
//-----------------------------------------------------------------------------
bool FieldMoveModelManager::SetupModel(u32 characterId)
{
  for( s32 i=0; i<DYNAMIC_RES_LOAD_INDEX_MAX ; i++ )
  {
    if( m_pLocalAllocatorForModelResourceID[i] != -1 && m_pLocalAllocatorForModelResourceID[i] == (s32)characterId )
    {
      return m_charaModelFactory.SetupModel( m_pLocalAllocatorForModelResource[i] , characterId );
    }
  }
  return m_charaModelFactory.SetupModel( m_pAllocator , characterId ); 
}

/**
* @brief カリングを行う対象か取得します
*
* @param index チェック対象となる動作モデルのデータindexです
*/
b32 FieldMoveModelManager::IsCullingTarget(s32 index) const
{
  switch(index)
  {
    case FIELD_MOVE_MODEL_PLAYER:
    case FIELD_MOVE_MODEL_RIDE_POKEMON:
    {
      return false;
    }
  }
  return true;
}

FieldMoveModel* FieldMoveModelManager::GetInterestFureaiPokemon(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource() )
    {
      if (m_pFieldMoveModel[i]->GetMoveCodeId() != MoveModel::FIELD_MOVE_CODE_FUREAI_A)
      {
        continue;
      }

      if (FieldMoveCodeFureaiTypeA::IsPlayerInterest(m_pFieldMoveModel[i]->GetMoveCodeWork()))
      {
        return m_pFieldMoveModel[i];
      }
    }
  }

  return NULL;
}

#if PM_DEBUG
//-----------------------------------------------------------------------------
/**
* @brief 着せ替えパラメータのランダム取得
*
* @param pOutDressUpParam 受け取る着せ替えパラメータ
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::GetDressUpParamRand(poke_3d::model::DressUpParam* pOutDressUpParam)
{
  System::DressUp::Debug_GetDressUpParamRandom( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus()->GetSex(),
                                                GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus()->GetSex() == PM_MALE ? m_dressUpModelResourceManager.GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::SEX_MALE) : m_dressUpModelResourceManager.GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::SEX_FEMALE),
                                                pOutDressUpParam,
                                                NULL );
}

/**
* @brief  モデル種類数を取得します
*
* @return モデルの種類数
*/
s32 FieldMoveModelManager::GetCountOfModelTypes( void ) //const
{
  return m_charaModelFactory.GetModelLoadedCount();
}

/**
* @brief  存在する動作モデルの数を取得します
*
* @return 動作モデル数
*/
s32 FieldMoveModelManager::GetCountOfSetupCompleteModel( void ) const
{
  s32 count = 0;

  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource() )
    {
      count++;
    }
  }

  return count;
}

/**
* @brief  描画中の動作モデルの数を取得します
*
* @return 動作モデル数
*/
s32 FieldMoveModelManager::GetCountOfDrawingModel( void ) const
{
  s32 count = 0;

  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource() && m_pFieldMoveModel[i]->GetCharaDrawInstance() && m_pFieldMoveModel[i]->GetCharaDrawInstance()->IsVisible() && !m_pFieldMoveModel[i]->IsEnableCulilng() )
    {
      count++;
    }
  }

  return count;
}

/**
* @brief  描画する動作モデルの数を取得します
*
* @return 描画する動作モデルの数
*/
s32 FieldMoveModelManager::GetCountOfDrawableModel( void ) const
{
  s32 count = 0;

  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource() && m_pFieldMoveModel[i]->GetCharaDrawInstance() && m_pFieldMoveModel[i]->GetCharaDrawInstance()->IsVisible() )
    {
      count++;
    }
  }

  return count;
}

/**
* @brief  存在する配置データの動作モデルの数を取得します
*
* @return 動作モデル数
*/
s32 FieldMoveModelManager::GetCountOfSetupCompletePlacementDataModel( void ) const
{
  s32 count = 0;

  for( u32 i = FIELD_MOVE_MODEL_NPC_START; i<=FIELD_MOVE_MODEL_NPC_END ; i++ )
  { 
    if( m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource() )
    {
      count++;
    }
  }

  return count;
}

/**
* @brief  配置データの動作モデルの最大数を取得します
*
* @return 動作モデル数
*/
s32 FieldMoveModelManager::GetCountOfPlacementDataModelMax( void ) const
{
  return (FIELD_MOVE_MODEL_NPC_END-FIELD_MOVE_MODEL_NPC_START+1);
}

/**
* @brief  カリングターゲットで描画する動作モデルの数を取得します
*
* @return 描画する動作モデルの数
*/
s32 FieldMoveModelManager::GetCountOfDrawableCullingTragetModel(void) const
{
  s32 count = 0;

  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( IsCullingTarget(i) && m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource() && m_pFieldMoveModel[i]->GetCharaDrawInstance() && m_pFieldMoveModel[i]->GetCharaDrawInstance()->IsVisible() )
    {
      count++;
    }
  }

  return count;
}

/**
* @brief  カリングターゲットで描画中の動作モデルの数を取得します
*
* @return 動作モデル数
*/
s32 FieldMoveModelManager::GetCountOfDrawingCullingTargetModel(void) const
{
  s32 count = 0;

  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( IsCullingTarget(i) && m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource() && m_pFieldMoveModel[i]->GetCharaDrawInstance() && m_pFieldMoveModel[i]->GetCharaDrawInstance()->IsVisible() && !m_pFieldMoveModel[i]->IsEnableCulilng() )
    {
      count++;
    }
  }

  return count;
}

/**
* @brief  スクリプト操作があった動作モデルの座標を取得
*/
void FieldMoveModelManager::OutPutsEndPositionForEventScript( u16 zoneId, u16 areaId)
{
  bool enableFlag;
  gfl2::fs::Result result = gfl2::fs::PcUtil::IsPcAccessEnableFlag(&enableFlag);
  if (result.IsSuccess() == false || enableFlag == false)
  {
    return;
  }

  char buffer[2048];
  u32 start = 0;
  
#if defined(GF_PLATFORM_WIN32)
#define CAMERA_PUTS_TEXT( text, ... ) start += sprintf_s( &buffer[ start ], sizeof( buffer ) - start, text, __VA_ARGS__ )
#elif defined(GF_PLATFORM_CTR)
#define CAMERA_PUTS_TEXT( ... ) start += sprintf( &buffer[ start ], __VA_ARGS__ )
#endif

  // ヘッダー情報
  CAMERA_PUTS_TEXT("%d,%d\r\n", zoneId, areaId );

  for (u32 i = 0; i<FIELD_MOVE_MODEL_MAX; i++)
  {
    if (m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource() )
    {
      // 動いた
      if (m_pFieldMoveModel[i]->IsFreeMoveForEventScript() || m_pFieldMoveModel[i]->IsPointMoveForEventScript())
      {
        // レジストされていないNPCならスキップ
        if (i >= FIELD_MOVE_MODEL_NPC_START && i < FIELD_MOVE_MODEL_NPC_END)
        {
          PlacementDataManager* pPlacementData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident()->GetPlacementDataManager();
          if (pPlacementData->SearchZoneEventIDModel(m_pFieldMoveModel[i]->GetEventId()) == NULL)
          {
            continue;
          }
        }

        gfl2::math::Vector3 pos(m_pFieldMoveModel[i]->GetLocalSRT().translate);

        // 情報出力
        CAMERA_PUTS_TEXT("%d,%f,%f,%f\r\n", m_pFieldMoveModel[i]->GetEventId(), pos.x, pos.y, pos.z );
      }
    }
  }

  buffer[start] = '\0';
  gfl2::fs::PcUtil::WriteFile("R:/home/momiji/exec/direct_sdmc/field/evend_movemodel_info.txt", buffer, start);
}

/**
* @brief  バミリアサートチェック
*/
void FieldMoveModelManager::CheckSafetyPositionForEventScript(void)
{
  for (u32 i = 0; i<FIELD_MOVE_MODEL_MAX; i++)
  {
    if (m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource())
    {
      // 動いた
      if (m_pFieldMoveModel[i]->IsFreeMoveForEventScript())
      {
        // レジストされていないNPCならスキップ
        if (i >= FIELD_MOVE_MODEL_NPC_START && i < FIELD_MOVE_MODEL_NPC_END)
        {
          PlacementDataManager* pPlacementData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident()->GetPlacementDataManager();
          if (pPlacementData->SearchZoneEventIDModel(m_pFieldMoveModel[i]->GetEventId()) == NULL)
          {
            continue;
          }
        }

        gfl2::math::Vector3 pos(m_pFieldMoveModel[i]->GetLocalSRT().translate);
        u16 zoneId = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetZoneID();
        GFL_ASSERT_MSG(0,
          "zoneId[%s] position[%f,%f,%f] バミリ移動で終わる必要があります",
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldDebugMenu()->GetDebugMapJump()->GetZoneName(zoneId),
          pos.x,
          pos.y,
          pos.z);
      }
    }
  }
}

/**
* @brief  アクション終了アサートチェック
*/
void FieldMoveModelManager::CheckActionEndForEventScript(void)
{
  for (u32 i = 0; i<FIELD_MOVE_MODEL_MAX; i++)
  {
    if (m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource())
    {
      // 動いている
      if (m_pFieldMoveModel[i]->IsExecuteActionCommand())
      {
        // レジストされていないNPCならスキップ
        if (i >= FIELD_MOVE_MODEL_NPC_START && i < FIELD_MOVE_MODEL_NPC_END)
        {
          PlacementDataManager* pPlacementData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident()->GetPlacementDataManager();
          if (pPlacementData->SearchZoneEventIDModel(m_pFieldMoveModel[i]->GetEventId()) == NULL)
          {
            continue;
          }

          // 移動命令以外はスキップ
          FieldActionCommandId actionCommandId = m_pFieldMoveModel[i]->GetActionCommandId();
          if( actionCommandId != FIELD_ACTION_COMMAND_TARGET_MOVE
            && actionCommandId != FIELD_ACTION_COMMAND_TARGET_MOVE_WITH_OPTIONAL_MOTION
            && actionCommandId != FIELD_ACTION_COMMAND_STEP_MOVE
            && actionCommandId != FIELD_ACTION_COMMAND_STEP_MOVE_WITHOUT_START_END )
          {
            continue;
          }
        }

        GFL_ASSERT_MSG(0, "アクションの終了を待つ必要があります(%d)", m_pFieldMoveModel[i]->GetEventId());
      }
    }
  }
}

#endif

/**
* @brief  バミリアサートチェック、クリア
*/
void FieldMoveModelManager::ClearCheckSafetyPositionForEventScript(void)
{
  for (u32 i = 0; i<FIELD_MOVE_MODEL_MAX; i++)
  {
    if (m_pFieldMoveModel[i]->IsSetupWork() && m_pFieldMoveModel[i]->IsSetupResource())
    {
      m_pFieldMoveModel[i]->ClearPositionCheckForEventScript();
    }
  }
}


}; //end of namespace MoveModel
}; //end of namespace Field
