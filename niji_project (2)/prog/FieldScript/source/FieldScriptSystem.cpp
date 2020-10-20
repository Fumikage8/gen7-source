//=============================================================================
/**
 *
 *  @file   FieldScriptSystem.cpp
 *  @brief  フィールドスクリプトシステム
 *  @author Miyuki Iwasawa 
 *  @date   2015.02.28
 *
 */
//=============================================================================

#include <heap/include/gfl2_Heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <System/include/HeapDefine.h>
#include <System/include/HeapSize.h>

#include "System/include/HeapDefine.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"
#include "fieldScript/include/ScriptObject.h"
#include "FieldScript/include/EventScriptCall.h"

#include "FieldScript/include/ScriptObject.h" // for ScriptObject
#include "FieldScript/include/ScriptInfo.h"
#include "FieldScript/include/ScriptBufferManager.h"
#include "FieldScript/include/ScriptTrialModel.h"
#include "FieldScript/include/ScriptAssert.h"

#include "niji_conv_header/field/flagwork/FlagDefine.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"
#include "niji_reference_files/script/init_script_define.inc"

#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetInit.h"        //FuncSetInit::GetTable
#include "Field/FieldStatic/include/Script/ScriptFuncSetFieldInit.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetAppCall.h"     //FuncSetAppCall::GetTable
#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"          //FuncSetCommon::GetTable
#include "NetApp/JoinFesta/include/Script/ScriptFuncSetJoinFesta.h"    //FuncSetJoinFesta::GetTable

#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

#include "niji_conv_header/field/zone/zone_id.h"
#include "niji_conv_header/field/script/inc/init_scr.inc"
#include "niji_conv_header/field/script/inc/scene_work_const.inc"
#include "arc_def_index/arc_def.h"

#include "arc_index/FieldScript.gaix" // for GARC_field_script_
#include "niji_conv_header\field\script\inc\debug_iwasawa.inc"
#include "arc_index/message.gaix"

#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldStatic/include/Debug/DebugStartSettingAccessor.h"

#include "FieldScript/include/SceneChangeControl.h"
#include "FieldScript/include/RegularObject.h"



#if PM_DEBUG
  // 処理負荷プリント有効フラグ
  #define SCRIPT_TICK_PRINT (0)

  #if GFL_FS_HIO_ENABLE
    // PM_DEBUG有効かつGFL_HIO_ENABLE有効時にスクリプトのHIO有効
    #define SCRIPT_HIO_ENABLE (1) //(1)
  #else
    #if defined(GF_PLATFORM_CTR)
      #define SCRIPT_HIO_ENABLE (0)
    #else
      #define SCRIPT_HIO_ENABLE (1)                   ///< WIN32 では必ず有効.
    #endif
  #endif

#else

  #define SCRIPT_TICK_PRINT (0)
  #define SCRIPT_HIO_ENABLE (0)

#endif

//--------------------------------------------------------------
/// 静的メンバ初期化
//--------------------------------------------------------------
int ScriptAssert::ScriptAssertEnableFlag = false;

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプト
 */
/* ------------------------------------------------------------------------- */
GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//--------------------------------------------------------------
/// 静的メンバ初期化
//--------------------------------------------------------------
#if PM_DEBUG
int FieldScriptSystem::DebugHioEnableFlag = false;
int FieldScriptSystem::DebugEndPositionOutPutEnableFlag = false;
int FieldScriptSystem::DebugEndPositionCheckEnableFlag = false;

int FieldScriptSystem::DebugSceneEditEnableFlag = false;
int FieldScriptSystem::DebugRegdentHeapCheckDisableFlag = false;
int FieldScriptSystem::DebugHeapCheckOffEnableFlag = false;
#endif


//--------------------------------------------------------------
/// 速度計測用定義
//--------------------------------------------------------------
enum 
{ 
  DEBUG_TICK_PRINT_SCRIPT_HIO,
  DEBUG_TICK_PRINT_SCRIPT_DOING,
  DEBUG_TICK_PRINT_SCRIPT_LOAD,
  DEBUG_TICK_PRINT_SCRIPT_REGIST,
  DEBUG_TICK_PRINT_SCRIPT_SET_SCENEC,

  DEBUG_TICK_PRINT_SCRIPT_MAX,
};
/// 速度測定セットアップ
//GFL_DEBUG_TICK_SETUP(SCRIPT, DEBUG_TICK_PRINT_SCRIPT_MAX);

//=============================================================================
//=============================================================================

FieldScriptSystem* FieldScriptSystem::pThis = NULL; ///< 自分自身へのポインタ保持ワーク

//FieldScript.garオープン用領域
static const u32 HEAP_FIELD_SCRIPT_ARC_FILE_SIZE = 0x3000; //160531現在 0x1838

//ZoneScript用Amx(Pawn)オブジェクト展開領域
static const u32 HEAP_FIELD_SCRIPT_PAWN_OBJECT_ZONE_SIZE = 0x7800;  ///<30k
//GeneralScript用Amx(Pawn)オブジェクト展開領域
static const u32 HEAP_FIELD_SCRIPT_PAWN_OBJECT_GENERAL_SIZE = 0x20800;  ///<1つ65k*2=130k

static const u32 HEAP_FIELD_SCRIPT_MSGNAMELIST_SIZE = 0xa00 + 0xa00;  ///< JPN_KANJI\namelist.dat のサイズは 1444(2016/5/18現在) momiji加増分を増やした、最新サイズは2156(2017/2/15現在)
/*  0xa00 確保で実際のローカルヒープの使用容量は、0x8cc 
    namelist.dat と MsgDataインスタンス(0x50)を合わせた分を除くと 0x288(648) の余裕.
*/

//=============================================================================
//
//
//    定義等
//
//
//=============================================================================

//=============================================================================
//=============================================================================
/* ------------------------------------------------------------------------- */
/**
 * @brief ヒープの状態をプリント
 *
 * @param pHeap
 */
/* ------------------------------------------------------------------------- */
static void PrintHeap( gfl2::heap::HeapBase * pHeap )
{
  SCRIPT_PRINT( "********ScriptHeapSize*********\n");
  SCRIPT_PRINT( "TOTAL %06x\n", pHeap->GetTotalSize() );
  SCRIPT_PRINT( "FREE  %06x\n", pHeap->GetTotalFreeSize() );
  SCRIPT_PRINT( "ALLOC %06x\n", pHeap->GetTotalAllocatableSize() );
  SCRIPT_PRINT( "********ScriptHeapSize*********\n");
}

#if SCRIPT_HIO_ENABLE

/* ------------------------------------------------------------------------- */
/**
 * @brief F7で転送したファイルが存在した場合、ロードする
 *
 * @param p_heap ヒープ
 * @param amxname ロードするAMXの名前
 * @param out_size ロードしたサイズ 出力先へのポインタ
 *
 * @return 生成したバッファ
 */
/* ------------------------------------------------------------------------- */
#if defined(GF_PLATFORM_WIN32)
#include <shlwapi.h>
#include <locale.h>
#pragma comment( lib, "Shlwapi.lib" ) 
#endif
static void* CreateTempBuffer( gfl2::fs::AsyncFileManager* pAsyncFileManager, gfl2::heap::HeapBase* p_heap, const char* amxname, u32* out_size )
{
  void* buffer = NULL;
  u32 size = 0;
  char path[256];

  if( !FieldScriptSystem::DebugHioEnableFlag ){
    return NULL;
  }

#if defined(GF_PLATFORM_CTR)
  char t_path[256];
  nw::ut::snprintf( path, 256, 255, "../../convert/field_convert/script/amx/%s", amxname );
  nw::ut::snprintf( t_path, 256, 255, "R:/home/momiji/convert/field_convert/script/amx/%s", amxname );
  
  bool bExist = false;
  // フルパスで有無を確認する
  gfl2::fs::PcUtil::CheckFileExist( &bExist, t_path );
  if( bExist )
#elif defined(GF_PLATFORM_WIN32)
  wchar_t t_path[256];
  wchar_t t_name[64];
  sprintf(path,"../../convert/field_convert/script/amx/%s",amxname);
  mbstowcs(t_name,amxname,64);
  wsprintf(t_path,L"R:/home/momiji/convert/field_convert/script/amx/%s",t_name);
  if(PathFileExists(t_path))
#endif
  {
    // ファイルを読み込みバッファに格納
    gfl2::fs::AsyncFileManager::FileEasyReadReq req;

    req.fileName   = path;
    req.ppBuf      = &buffer;
    req.pFileSize  = &size;
    req.heapForBuf = p_heap;
    req.align      = 128;
    req.heapForReq = p_heap->GetLowerHandle();
    req.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO;
    pAsyncFileManager->SyncFileEasyRead(req);
  }

  *out_size = size;
  return buffer;
}

#endif // #if SCRIPT_HIO_ENABLE




//=============================================================================
/**
 * @class InitObject
 * @brief フィールド初期化スクリプトシステム
 */
//=============================================================================
class InitObject : public ScriptObject
{
  GFL_FORBID_COPY_AND_ASSIGN( InitObject ); ///< コピーコンストラクタ + 代入禁止
public:
  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   *
   * @param p_heap ヒープへのポインタ
   * @param p_gman ゲームマネージャへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  InitObject( gfl2::heap::HeapBase* p_heap, GameSys::GameManager* p_gman );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  virtual ~InitObject( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief ロード処理
   *
   * @param buffer AMXバッファ
   * @param size AMXバッファサイズ
   * @param amxname AMX名
   */
  /* ------------------------------------------------------------------------- */
  void LoadExt( const void* buffer, const size_t size, const c8* const amxname, const int init_level );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 主処理
   *
   * @return pawn主処理の戻り値
   */
  /* ------------------------------------------------------------------------- */
  virtual s32 Update( void );

};

/* ------------------------------------------------------------------------- */
/**
 * @brief コンストラクタ
 *
 * @param p_heap ヒープへのポインタ
 * @param p_gman ゲームマネージャへのポインタ
 */
/* ------------------------------------------------------------------------- */
InitObject::InitObject( gfl2::heap::HeapBase* p_heap, GameSys::GameManager* p_gman ) : ScriptObject( p_heap )
{
  this->SetGameManager( p_gman );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
InitObject::~InitObject( void )
{

}

/* ------------------------------------------------------------------------- */
/**
 * @brief ロード処理
 *
 * @param buffer AMXバッファ
 * @param size AMXバッファサイズ
 * @param amxname AMX名
 */
/* ------------------------------------------------------------------------- */
void InitObject::LoadExt( const void * buffer, const size_t size, const char* amxname, const int init_level )
{
  ScriptObject::Load( buffer, size, amxname );
 
  // Initで使えるFUNCSETを登録
  RegisterPawnFunction( FuncSetInit::GetTable() );
  RegisterPawnFunction( FuncSetFieldInit::GetTable() );

  if( init_level < 2 ) return;

  RegisterPawnFunction( FuncSetCommon::GetTable() );
  RegisterPawnFunction( FuncSetAppCall::GetTable() );
  RegisterPawnFunction( FuncSetJoinFesta::GetTable() );
#if 0
  RegisterPawnFunction( FuncSetSodateya::GetTable() );
  RegisterPawnFunction(        FuncSetItem::GetTable() );
  RegisterPawnFunction( FuncSetInteractive::GetTable() );
  RegisterPawnFunction(     FuncSetGimmick::GetTable() );
  RegisterPawnFunction(   FuncSetProgField::GetTable() ); // @note sango 追加 秘密基地ギルドのため
#endif
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 主処理
 *
 * @return pawn主処理の戻り値
 */
/* ------------------------------------------------------------------------- */
s32 InitObject::Update( void )
{
  s32 ret;
  
  // 実行関数
  ret = gfl2::pawn::Pawn::Update();

  return ret;
}


//=============================================================================
/**
 * @class オブジェクトコンテナ
 */
//=============================================================================
class ObjectContainer
{
private:
  static const int HOLD_OBJECT_MAX = 3; ///< 保持するオブジェクト最大数
  static const int ZONE_OBJECT_ID = 0;  ///< ゾーンオブジェクトのID
  RegularObject* m_pRegularObject;  ///< カレントオブジェクトへのポインタ
  RegularObject* m_pObjects[ HOLD_OBJECT_MAX ]; ///< オブジェクト配列
  RegularObject* m_pUseObjects[ HOLD_OBJECT_MAX ];  ///< 使用オブジェクト配列
  u32 m_UseObjCount;  ///< 使用オブジェクトカウンタ

public:
  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   *
   * @param p_sys スクリプトシステムへのポインタ
   * @param p_heap ヒープへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  ObjectContainer( FieldScriptSystem * p_sys, gfl2::heap::HeapBase * p_heap, gfl2::heap::HeapBase * p_heap_zone, gfl2::heap::HeapBase * p_heap_general )
  {
#if GFL_PAWN_DEBUG
    // @note 容量が余っているDEVICEを使う
    gfl2::heap::HeapBase* p_debug = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG)->GetLowerHandle();

    // @note MapChangeContinue時のゾーンオブジェクト断片化を防ぐため、ゾーンとGeneral用でヒープを分ける
    for ( int i = 0; i < HOLD_OBJECT_MAX; i ++ )
    {
      if( i==ZONE_OBJECT_ID ){
        m_pObjects[i] = GFL_NEW( p_heap ) RegularObject( p_sys, p_heap_zone, p_debug );
      }else{
        m_pObjects[i] = GFL_NEW( p_heap ) RegularObject( p_sys, p_heap_general, p_debug );
      }
      
      m_pUseObjects[i] = NULL;
    }
#else
    for ( int i = 0; i < HOLD_OBJECT_MAX; i ++ )
    {
      if( i==ZONE_OBJECT_ID ){
        m_pObjects[i] = GFL_NEW( p_heap ) RegularObject( p_sys, p_heap_zone );
      }else{
        m_pObjects[i] = GFL_NEW( p_heap ) RegularObject( p_sys, p_heap_general );
      }
      m_pUseObjects[i] = NULL;
    }
#endif
    m_UseObjCount = 0;
    m_pRegularObject = m_pObjects[ZONE_OBJECT_ID];
  }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ~ObjectContainer()
  {
    for ( int i = 0; i < HOLD_OBJECT_MAX; i ++ )
    {
      GFL_SAFE_DELETE( m_pObjects[i] );
    }
  }
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief ゾーンオブジェクトを取得
   *
   * @return ゾーンオブジェクトへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  RegularObject * GetZoneObject( void )
  {
    return m_pRegularObject;
  }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 指定オブジェクトが使用されているかチェック
   *
   * @param p_obj オブジェクト
   *
   * @return true:使用している, false:使用していない
   */
  /* ------------------------------------------------------------------------- */
  bool IsObjUse( const RegularObject * p_obj ) const
  {
    for (u32 i = 0; i < m_UseObjCount; i ++ )
    {
      if ( m_pUseObjects[i] == p_obj ) return true;
    }
    return false;
  }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief Object管理機構から取得
   *
   * @param scrtype スクリプトタイプ
   *
   * @return オブジェクトへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  RegularObject * AcquireObject( SCRIPT_TYPE scrtype )
  {
    RegularObject * p_obj = NULL;

    if ( scrtype == SCRIPT_TYPE_ZONE )
    {
      //ZoneScript用オブジェクトは唯一
      GFL_ASSERT( IsObjUse(m_pRegularObject) == false );
      p_obj = m_pRegularObject;
    }
    else
    {
      for (int i = 0; i < HOLD_OBJECT_MAX; i ++ )
      {
        if ( IsObjUse( m_pObjects[i] ) == false )
        {
          p_obj = m_pObjects[i];
        }
      }
    }
    GFL_ASSERT( p_obj != NULL );
    m_pUseObjects[ m_UseObjCount ] = p_obj;
    m_UseObjCount ++;
    SCRIPT_PRINT("AcquireScrObj:idx=%d(%08p)\n", m_UseObjCount, p_obj );
    return p_obj;
  }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief Object管理機構に返す
   *
   * @param obj オブジェクトへのポインタ
   *
   * @return 次にカレントになるオブジェクトへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  RegularObject * ReleaseObject( RegularObject * p_obj )
  {
    GFL_ASSERT( m_UseObjCount > 0 );
    GFL_ASSERT( m_pUseObjects[m_UseObjCount - 1 ] == p_obj );

    m_pUseObjects[ m_UseObjCount - 1 ] = NULL;
    m_UseObjCount --;
    RegularObject * nextObj = NULL;
    if ( m_UseObjCount )
    { //積まれているOBJはHalt状態のはずなのでResumeする
      nextObj = m_pUseObjects[ m_UseObjCount - 1 ];
      nextObj->Resume();
      p_obj->GetWork()->CopyGlobalParamsToParent( nextObj->GetWork() );
    }
    p_obj->Finalize();

    SCRIPT_PRINT("ReleaseScrObj:idx=%d(%08p)\n", m_UseObjCount, p_obj );
    return nextObj;
  }
};

#if PM_DEBUG

/* ------------------------------------------------------------------------- */
/**
 * @class デバッグ名マネージャ
 */
/* ------------------------------------------------------------------------- */
class DebugNameManager
{
private:

  static const int ONE_ZONE_CHAR_NUM = 16; // 1Zone名の文字数
  static const int ONE_ZONE_CHAR_BUF = 32; // 1Zone名の文字数
private:
  char* m_pZoneNameTable;
  char  m_ZoneName[ONE_ZONE_CHAR_BUF];
  s32 m_ZoneMax;
  size_t m_ZoneNameDataSize;

public:
  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   *
   * @param pHeap ヒープへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  DebugNameManager( gfl2::fs::AsyncFileManager* pAsyncFileManager, gfl2::heap::HeapBase * pHeap, gfl2::heap::HeapBase * pHeapWork )
  {
    //---------------------------------------------------
    // ARCでゾーン名データ読み込み
    //---------------------------------------------------
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_ZONE_DATA_D;
      openReq.heapForFile = pHeapWork->GetLowerHandle();
      openReq.heapForArcSrc = pHeapWork->GetLowerHandle();
      openReq.heapForReq = pHeapWork->GetLowerHandle();
      pAsyncFileManager->SyncArcFileOpen( openReq );
    }
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
      loadReq.arcId = ARCID_ZONE_DATA_D;
      loadReq.datId = 0;
      loadReq.ppBuf = reinterpret_cast<void**>(&m_pZoneNameTable);
      loadReq.heapForBuf = pHeap->GetLowerHandle(); //デバッグ時にしか載らないメモリなのであえて後方確保にしている
      loadReq.pBufSize = &m_ZoneNameDataSize;
      loadReq.align = 4;
      pAsyncFileManager->SyncArcFileLoadData( loadReq );
    }
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_ZONE_DATA_D;
      closeReq.heapForReq = pHeapWork->GetLowerHandle();
      pAsyncFileManager->SyncArcFileClose( closeReq );
    }

    // ZoneNameの最大値を求める。
    m_ZoneMax = m_ZoneNameDataSize / ONE_ZONE_CHAR_NUM;
  }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ~DebugNameManager()
  {
    GflHeapSafeFreeMemory( m_pZoneNameTable );
  }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief AMX名を取得
   *
   * @param type スクリプトタイプ
   * @param index スクリプトインデックス
   *
   * @return AMX名
   */
  /* ------------------------------------------------------------------------- */
  const char * GetAmxName( SCRIPT_TYPE type, u16 index )
  {
    if( index >= m_ZoneMax )
    {
      GFL_ASSERT_MSG(0, "index=%d, zonemax=%d, size=%d\n", index, m_ZoneMax, m_ZoneNameDataSize );
      index = 0; 
    }
    char * name = &m_pZoneNameTable[index * ONE_ZONE_CHAR_NUM];
    char zoneName[ONE_ZONE_CHAR_NUM+1] = {0};

    #if defined(GF_PLATFORM_CTR)
      nw::ut::strncpy( zoneName, ONE_ZONE_CHAR_NUM+1, name, ONE_ZONE_CHAR_NUM );
    #elif defined(GF_PLATFORM_WIN32)
      strncpy(zoneName, name,ONE_ZONE_CHAR_NUM);
    #endif

    switch (type )
    {
    case SCRIPT_TYPE_ZONE:
      #if defined(GF_PLATFORM_CTR)
        nw::ut::snprintf( m_ZoneName, ONE_ZONE_CHAR_BUF, ONE_ZONE_CHAR_BUF-1, "%s.amx", zoneName );
      #elif defined(GF_PLATFORM_WIN32)
        _snprintf(m_ZoneName, ONE_ZONE_CHAR_BUF, "%s.amx",zoneName);
      #endif
      break;
    case SCRIPT_TYPE_ZONEINIT:
      #if defined(GF_PLATFORM_CTR)
        nw::ut::snprintf( m_ZoneName, ONE_ZONE_CHAR_BUF, ONE_ZONE_CHAR_BUF-1, "init_%s.amx", zoneName );
      #elif defined(GF_PLATFORM_WIN32)
      _snprintf(m_ZoneName, ONE_ZONE_CHAR_BUF, "init_%s.amx",zoneName);
      #endif
      break;
    default:
      GFL_ASSERT( 0 );
      #if defined(GF_PLATFORM_CTR)
        nw::ut::snprintf( m_ZoneName, ONE_ZONE_CHAR_BUF, ONE_ZONE_CHAR_BUF-1, "dummy.amx");
      #elif defined(GF_PLATFORM_WIN32)
        _snprintf(m_ZoneName, ONE_ZONE_CHAR_BUF, "init_dummy.amx");
      #endif
    }
    return m_ZoneName;
  }
};

#endif // PM_DEBUG

/* ------------------------------------------------------------------------- */
/**
 * @brief AMXの名前を取得(デバッグ時のみ)
 *
 * @param type スクリプトタイプ
 * @param index スクリプトインデックス
 *
 * @return AMXの名前
 */
/* ------------------------------------------------------------------------- */
const char * FieldScriptSystem::GetAmxName( SCRIPT_TYPE type, u16 index )
{
#if PM_DEBUG
  return m_DebugNameManager->GetAmxName( type, index );
#else
  return "";
#endif
}


//=============================================================================
// FieldScriptSystem
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 *  
 *  @param  gfl2::heap::HeapBase* p_heap   ヒープ
 */
//-----------------------------------------------------------------------------
FieldScriptSystem::FieldScriptSystem( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* p_heap )
: m_pHeap(NULL),
  m_pHeapFileRead(NULL),
  m_pHeapWork(NULL),
  m_pHeapNameList(NULL),
  m_pGameManager(p_gman),
  m_pSystemWork(NULL),
  m_SceneChangeControl(NULL),
  m_ScriptBufferManager(NULL),
  m_ObjectContainer(NULL),
  m_pScriptEffectManager( NULL ),
  m_pScriptTrialModel( NULL ),
  m_pScriptMoveModelManager( NULL ),
  m_pMsgDataNameList( NULL ),
  m_pMsgDataNameListBuffer( NULL ),
  m_eMsgDataNameListInit( INIT_STATE_NONE ),
  m_pEffect( NULL ),
  m_bCreated(false),
  m_bForceEnd(false),
  m_pNowObject(NULL),
  m_iScriptArcTableSize( 0 )
  ,m_IsTerrainBlockControlSet( false )
  ,m_TerrainBlockControlPositon( 0.0f, 0.0f, 0.0f )
  ,m_bLastKeyXMenuOpen( NULL )
#if PM_DEBUG
  ,m_DebugScriptEventAfterCheckFlag(false)
#endif
{
  // @note ゲーム起動時に1度だけ呼ばれる

  ChrClearActionAfterRotAll();
  ChrMotionEndWatchClearAll();

  gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::Tag arcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::SAME_AS_MANAGER;
  gfl2::fs::AsyncFileManager::ArcSrcUseSetting::Tag arcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::SAME_AS_MANAGER;

#if PM_DEBUG
  u32 arcid_scroffset = ARCID_SCROFFSETD;

  const Field::Debug::StartSettingAccessor* pStartSettingAccessor = p_gman->GetGameData()->GetDebugStartSettingAccessor();

  if (pStartSettingAccessor->HasScriptEndPositionCheckEnable()){
    FieldScriptSystem::DebugEndPositionCheckEnableFlag = pStartSettingAccessor->IsScriptEndPositionCheckEnable();
  }
  if (pStartSettingAccessor->HasScriptEndPositionOutputEnable()){
    FieldScriptSystem::DebugEndPositionOutPutEnableFlag = pStartSettingAccessor->IsScriptEndPositionOutPutEnable();
  }

  //スクリプトHIOの有効無効を評価
#if SCRIPT_HIO_ENABLE
  {
    if( pStartSettingAccessor->HasScriptHioEnable() ){
      FieldScriptSystem::DebugHioEnableFlag = pStartSettingAccessor->IsScriptHioEnable();
    }

    if( FieldScriptSystem::DebugHioEnableFlag )
    {
      // @note 不要？ デバッグメニューで操作されるケアが入ってるはず
      arcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
      arcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
    }
  }
#endif //SCRIPT_HIO_ENABLE

//スクリプトアサート制御
#if SCRIPT_HIO_ENABLE
  {
    if( pStartSettingAccessor->HasScriptAssertEnable() ){
      ScriptAssert::ScriptAssertEnableFlag = pStartSettingAccessor->IsScriptAssertEnable();
    }
  }
#endif //SCRIPT_HIO_ENABLE

#if PM_DEBUG
  {
    if( pStartSettingAccessor->HasScriptEditModeEnable() ){
      FieldScriptSystem::DebugSceneEditEnableFlag = pStartSettingAccessor->IsScriptEditModeEnable();
    }
  }
#endif

#else
  u32 arcid_scroffset = ARCID_SCROFFSET;
#endif

  for( u32 i = 0; i < BG_PARTS_MAX; i++ )
  {
    m_BgPartsInfo.paramTable[i].use = 0;
  }

  GFL_ASSERT(pThis==NULL);
  pThis=this;  //自分を格納
  

  m_pEntryNextEvent = NULL;
  
  // 初期無効
  m_ReservedScrID = SCRID_DUMMY;
  m_WeakReservedScrID = SCRID_DUMMY;
  m_CallBackType = SCR_CALLBACK_TYPE_NOTHING;
  m_CallBackScriptID = SCRID_DEFAULT;
  
  // スクリプト親ヒープ生成
  m_pHeap = GFL_CREATE_HEAP(
      p_heap,
      HEAPID_FIELD_SCRIPT,
      System::HeapSize::HEAP_FIELD_SCRIPT_SIZE,
      gfl2::heap::HEAP_TYPE_EXP,
      false );
  
  m_pHeapFileRead = GFL_CREATE_HEAP(
      m_pHeap,
      HEAPID_FIELD_SCRIPT_FILEREAD,
      System::HeapSize::HEAP_FIELD_SCRIPT_FILEREAD_SIZE,
      gfl2::heap::HEAP_TYPE_EXP,
      false );
  
  m_pHeapWork = GFL_CREATE_HEAP(
      m_pHeap,
      HEAPID_FIELD_SCRIPT_WORK,
      System::HeapSize::HEAP_FIELD_SCRIPT_WORK_SIZE,
      gfl2::heap::HEAP_TYPE_EXP,
      false );

  m_pHeapArcFile = GFL_CREATE_LOCAL_HEAP_NAME(
    m_pHeap,
    HEAP_FIELD_SCRIPT_ARC_FILE_SIZE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "ScriptArc"
    );

  m_pHeapPawnZone = GFL_CREATE_LOCAL_HEAP_NAME(
    m_pHeap,
    HEAP_FIELD_SCRIPT_PAWN_OBJECT_ZONE_SIZE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "ScriptZoneAmx"
    );

  m_pHeapPawnGeneral = GFL_CREATE_LOCAL_HEAP_NAME(
    m_pHeap,
    HEAP_FIELD_SCRIPT_PAWN_OBJECT_GENERAL_SIZE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "ScriptGeneralAmx"
    );

  m_pHeapNameList = GFL_CREATE_LOCAL_HEAP_NAME(
      m_pHeap,
      HEAP_FIELD_SCRIPT_MSGNAMELIST_SIZE,
      gfl2::heap::HEAP_TYPE_EXP,
      false,
      "ScriptNameList"
      );
  
  //  従来の ScrOffset.cdat をバイナリー化したものをここで読む. ScriptInfo::GetCommonAmxNum() で返る値が含まれるのでそれより前に.
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId       = arcid_scroffset;
    req.heapForFile = p_heap;
    req.heapForArcSrc = p_heap;
    req.heapForReq = p_heap->GetLowerHandle();
    req.arcSrcCreateSetting = arcCreateSetting;
    m_pGameManager->GetAsyncFileManager()->SyncArcFileOpen(req);
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId      = arcid_scroffset;
    req.datId      = 0; ///< 0 固定.
    req.ppBuf      = (void**)&m_pScriptArcTable;
    req.pBufSize   = &m_iScriptArcTableSize;
    req.heapForBuf = p_heap;
    req.align      = 4;
    req.heapForReq = p_heap->GetLowerHandle();
    req.arcSrcUseSetting = arcUseSetting;
    m_pGameManager->GetAsyncFileManager()->SyncArcFileLoadData(req);
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.fileName   = NULL;
    req.arcId       = arcid_scroffset;
    req.heapForReq = p_heap;
    m_pGameManager->GetAsyncFileManager()->SyncArcFileClose(req); 
  }
  
  
  // 各初期化
  m_ScriptBufferManager = GFL_NEW( m_pHeap ) ScriptBufferManager( m_pHeap, ScriptInfo::GetCommonAmxNum() );
 
  m_pSystemWork = GFL_NEW( m_pHeapWork ) SystemWork( p_gman, m_pHeapWork );
  
  m_SceneChangeControl = GFL_NEW( m_pHeap ) SceneChangeControl();

  m_ObjectContainer = GFL_NEW( m_pHeap ) ObjectContainer( this, m_pHeap, m_pHeapPawnZone, m_pHeapPawnGeneral );

  m_pScriptEffectManager = GFL_NEW( m_pHeap ) ScriptEffectManager( m_pHeap, p_gman );
  m_pScriptTrialModel = GFL_NEW( m_pHeap ) ScriptTrialModel( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ), p_gman );
  m_pScriptMoveModelManager = GFL_NEW( m_pHeap ) ScriptMoveModelManager( p_gman );

  //m_ScriptMoveModelController = GFL_NEW( m_pHeap ) ScriptMoveModelController( m_pHeap, m_pGameManager->GetGameData()->GetFieldMoveModelManager(), m_pEngineWorkManager );
  // シーン実行ワーク初期化
  m_SceneChangeControl->Clear();

#if PM_DEBUG
  m_DebugNameManager = GFL_NEW( m_pHeap ) DebugNameManager( m_pGameManager->GetAsyncFileManager(), m_pHeap, m_pHeapFileRead );
  m_DebugBufferFlag = false;
  m_sDebugMsgLabel[ 0 ] = '\0';
  m_uIsDebugMsgLabel = 0;


#endif // PM_DEBUG
  SCRIPT_PRINT("sizeof RegularObject = %d\n", sizeof(RegularObject) );
  SCRIPT_PRINT("sizeof SystemWork = %d\n", sizeof(SystemWork) );

  PrintHeap( m_pHeap );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
FieldScriptSystem::~FieldScriptSystem( void )
{
#if PM_DEBUG
  GFL_SAFE_DELETE( m_DebugNameManager );
#endif // PM_DEBUG

  GFL_SAFE_DELETE( m_pSystemWork );
  GFL_SAFE_DELETE( m_ObjectContainer );
  GFL_SAFE_DELETE( m_ScriptBufferManager );
  GFL_SAFE_DELETE( m_SceneChangeControl );
  GFL_SAFE_DELETE( m_pScriptEffectManager );
  GFL_SAFE_DELETE( m_pScriptTrialModel );
  GFL_SAFE_DELETE( m_pScriptMoveModelManager );
  GFL_SAFE_DELETE( m_pMsgDataNameList );
  
  //バッファ開放
  GflHeapSafeFreeMemory(m_pMsgDataNameListBuffer);
  
  // ヒープ解放
  GFL_DELETE_HEAP(m_pHeapNameList);
  GFL_DELETE_HEAP(m_pHeapPawnGeneral);
  GFL_DELETE_HEAP(m_pHeapPawnZone);
  GFL_DELETE_HEAP(m_pHeapArcFile);
  gfl2::heap::Manager::DeleteHeap( m_pHeapWork );
  gfl2::heap::Manager::DeleteHeap( m_pHeapFileRead );
  gfl2::heap::Manager::DeleteHeap( m_pHeap );
  m_pHeapWork = NULL;
  m_pHeapFileRead = NULL;
  m_pHeap = NULL;

  pThis = NULL;
}



//----------------------------------------------------------------------------
/**
 *  @brief  MsgDataNameListデータロード
 */
//-----------------------------------------------------------------------------
void FieldScriptSystem::MsgDataNameList_Initialize()
{
  gfl2::fs::AsyncFileManager* pAsyncFileManager = m_pGameManager->GetAsyncFileManager();

  //---------------------------------------------------
  // ARCファイルオープン
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = print::GetMessageArcId();
    openReq.heapForFile = m_pHeap->GetLowerHandle();
    openReq.heapForArcSrc = m_pHeap->GetLowerHandle();
    openReq.heapForReq = m_pHeap->GetLowerHandle();
    pAsyncFileManager->AddArcFileOpenReq( openReq );
  }
  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    loadReq.arcId = print::GetMessageArcId();
    loadReq.datId = GARC_message_namelist_DAT;
    loadReq.ppBuf = &m_pMsgDataNameListBuffer;
    loadReq.heapForBuf = m_pHeapNameList;
    loadReq.heapForReq = m_pHeap->GetLowerHandle();
    loadReq.heapForCompressed = NULL;
    loadReq.pBufSize = NULL;
    loadReq.align = 4;
    pAsyncFileManager->AddArcFileLoadDataReq( loadReq );
  }
  
  m_eMsgDataNameListInit = INIT_STATE_LOADWAIT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  MsgDataNameListデータ待ち
 */
//-----------------------------------------------------------------------------
bool FieldScriptSystem::MsgDataNameList_Update()
{
  gfl2::fs::AsyncFileManager* pAsyncFileManager = m_pGameManager->GetAsyncFileManager();

  switch( m_eMsgDataNameListInit )
  {
    case INIT_STATE_LOADWAIT:
      // 読み込み完了待ち
      if( !pAsyncFileManager->IsArcFileOpenFinished( print::GetMessageArcId() ) ){
        return false;
      }
      if( !pAsyncFileManager->IsArcFileLoadDataFinished( &m_pMsgDataNameListBuffer ) ){
        return false;
      }
      
      //アーカイブ閉じる
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.arcId = print::GetMessageArcId();
        closeReq.heapForReq = m_pHeap->GetLowerHandle();
        pAsyncFileManager->AddArcFileCloseReq( closeReq );
      }
      m_eMsgDataNameListInit = INIT_STATE_CLOSEWAIT;
      return false;
    
    case INIT_STATE_CLOSEWAIT:
      // クローズ完了待ち
      if( !pAsyncFileManager->IsArcFileCloseFinished( print::GetMessageArcId() ) ){
        return false;
      }
      
      m_pMsgDataNameList = GFL_NEW(m_pHeapNameList) gfl2::str::MsgData( m_pMsgDataNameListBuffer, m_pHeapNameList );
      
      m_eMsgDataNameListInit = INIT_STATE_COMPLITE;
      break;
    
    default:
      break;
  }
  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  MsgDataNameListデータ解放
 */
//-----------------------------------------------------------------------------
void FieldScriptSystem::MsgDataNameList_Terminate()
{
  /// GFNMCat[4999]デバッグ機能ResetField後に、再度ゲームを始めようとすると停止.
  GFL_SAFE_DELETE( m_pMsgDataNameList );
  //バッファ開放
  GflHeapSafeFreeMemory(m_pMsgDataNameListBuffer);
}

//-----------------------------------------------------------------------------
/**
 *  @brief フィールド生成処理：開始
 *  @param  gfl2::heap::HeapBase* p_heap 
 *  @param  p_fileread ファイル読み込みクラスへのポインタ
 */
//-----------------------------------------------------------------------------
void FieldScriptSystem::Create( gfl2::heap::HeapBase* p_heap, gfl2::fs::AsyncFileManager* p_fileread )
{
  GFL_ASSERT( m_bCreated == false ); // 二重生成禁止
  

  bool b_hio = false;
  
#if PM_DEBUG
  if( DebugHioEnableFlag ) 
  {
    b_hio = true;
  }
#endif // PM_DEBUG
  
  // 断片化するのでファイル読込常駐領域からとる
  m_bCreated = false;
  m_ScriptBufferManager->Setup( p_fileread, m_pHeapArcFile, m_pHeapFileRead, b_hio );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  生成主処理
 */
//-----------------------------------------------------------------------------
bool FieldScriptSystem::UpdateCreate( void )
{
  if( m_ScriptBufferManager->SetupWait() )
  {
    PrintHeap( m_pHeap );
    m_bCreated = true;
  }
  return m_bCreated;
}

bool FieldScriptSystem::IsCreated( void ) const { return m_bCreated; }

//-----------------------------------------------------------------------------
/**
 *  @brief フィールド削除
 */
//-----------------------------------------------------------------------------
void FieldScriptSystem::Delete( void )
{
  m_bCreated = false;

  m_ScriptBufferManager->Cleanup();
  
  if( m_pScriptTrialModel )  m_pScriptTrialModel->Tarminate();
}

/* ------------------------------------------------------------------------- */
/**
* @brief フィールドマップが削除される際の破棄チェック
*/
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::FinalizeFieldmapTerminateTop( void )
{
  FieldScript::FuncSetCommon::ScriptFuncTerminateFieldTop( m_pGameManager );
}

/* ------------------------------------------------------------------------- */
/**
* @brief フィールドマップが削除される際の破棄
*/
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::FinalizeFieldmapTerminateBottom( void )
{
  FieldScript::FuncSetCommon::ScriptFuncTerminateFieldBottom( m_pGameManager );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ゾーンスクリプト初期化処理
 *
 * @param data ゾーンスクリプトのバッファ
 * @param size ゾーンスクリプトのバッファサイズ
 * @param zone_id ゾーンID
 *
 * @note  「つづきから」でも実行される
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::InitializeZoneScript( void* data, u32 size, ZoneID zone_id )
{
  RegularObject * zoneObj = m_ObjectContainer->GetZoneObject();

  SCRIPT_PRINT("InitializeZoneScript=%d\n", zone_id);
  // NULLだったら停止
  GFL_ASSERT_STOP( zoneObj );
  
  // ゾーンが走っているときはロードしない
  if ( zoneObj->IsFinished() == false )
  {
    GFL_ASSERT(0);
    return;
  } 

#if PM_DEBUG
  if( m_DebugBufferFlag && m_pZoneAmx ){
    GflHeapSafeFreeMemory( m_pZoneAmx );
    m_pZoneAmx = NULL;
  }
  m_DebugBufferFlag = false;
#endif // PM_DEBUG

  m_ZoneID = zone_id;
  m_ZoneAmxSize = size;
  m_pZoneAmx = data;

  //const char* amxname = ScriptInfo::GetAmxName( SCRIPT_TYPE_ZONE, zone_id );
  const char* amxname = this->GetAmxName( SCRIPT_TYPE_ZONE, zone_id );

#if SCRIPT_HIO_ENABLE
  void * temp_buffer = NULL;
  size_t temp_size;

  // @note 容量が余っているDEVICEを使う
  gfl2::heap::HeapBase* p_debug = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER)->GetLowerHandle();
  temp_buffer = CreateTempBuffer( m_pGameManager->GetAsyncFileManager(), p_debug/*m_pHeap*/, amxname, &temp_size );

  if ( temp_buffer )
  {
    m_pZoneAmx  = temp_buffer;
    m_ZoneAmxSize  = temp_size;
#if PM_DEBUG
    m_DebugBufferFlag = true;
#endif // PM_DEBUG
  }
#endif // if SCRIPT_HIO_ENABLE

#if SCRIPT_TICK_PRINT
  GFL_DEBUG_TICK_SET_FLAG( SCRIPT, true );
  GFL_DEBUG_TICK_RESET( SCRIPT );
#endif
  
  GFL_ASSERT( m_pZoneAmx != NULL );
  zoneObj->SetRegistData( m_ZoneID, ScriptObject::AMX_ID_NULL );
  zoneObj->LoadNormal( m_pZoneAmx, m_ZoneAmxSize, amxname );

//  GFL_DEBUG_TICK_START( SCRIPT, DEBUG_TICK_PRINT_SCRIPT_SET_SCENEC, "set_scene_c" );

  zoneObj->SetSceneChangeData( m_SceneChangeControl );
  m_CallBackType = SCR_CALLBACK_TYPE_NOTHING;
  m_CallBackScriptID = SCRID_DEFAULT;

//  GFL_DEBUG_TICK_END( SCRIPT, DEBUG_TICK_PRINT_SCRIPT_SET_SCENEC );

#if SCRIPT_TICK_PRINT
  s32 tick1 = GFL_DEBUG_TICK_US_GET(SCRIPT, DEBUG_TICK_PRINT_SCRIPT_LOAD);
  s32 tick2 = GFL_DEBUG_TICK_US_GET(SCRIPT, DEBUG_TICK_PRINT_SCRIPT_REGIST);
  s32 tick3 = GFL_DEBUG_TICK_US_GET(SCRIPT, DEBUG_TICK_PRINT_SCRIPT_SET_SCENEC);
  GFL_PRINT( "script init load: %d ms\n",  tick1 );
  GFL_PRINT( "script init regist: %d ms\n",  tick2 );
  GFL_PRINT( "script init set scene change: %d ms\n", tick3 );

#endif // SCRIPT_TICK_PRINT
  PrintHeap( m_pHeap );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ゾーンスクリプト廃棄処理
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::FinalizeZoneScript( void )
{
#if PM_DEBUG
  if( m_DebugBufferFlag && m_pZoneAmx ){
    GflHeapSafeFreeMemory( m_pZoneAmx );
  }
  m_DebugBufferFlag = false;
#endif // PM_DEBUG
  m_pZoneAmx = NULL;
  m_ZoneAmxSize = 0;

}


/* ------------------------------------------------------------------------- */
/**
 * @brief ゾーンローカルワーク初期化
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::ClearZoneLocalWork( void )
{
  GameSys::GameData* p_gdata = m_pGameManager->GetGameData();

  // マップチェンジ時にローカルワークをクリア
  EventWork* evwork = p_gdata->GetEventWork();

  // ローカルフラグのクリア
  evwork->ClearEventFlags( FZ_AREA_START, FZ_AREA_END );
  // ローカルワークのクリア
  evwork->ClearEventWorks( ZONEWORKAREA_START, ZONEWORKAREA_END );
  // 可変OBJワークのクリア
  evwork->ClearEventWorks( OBJCHRWORKAREA_START, OBJCHRWORKAREA_END );
#if 0
  //ジガルデセルの表示状態制御
  if( evwork->CheckEventFlag(SYS_FLAG_ZIGARUDE_CELL_ON) )
  {
    u16 day_value = SEQ_ZIGARUDE_CELL_ON;
    u16 night_value = SEQ_ZIGARUDE_CELL_OFF;
    int i = 0;

    if( Field::EvTime::IsAlolaDayTime(p_gdata) == false ){
      day_value = SEQ_ZIGARUDE_CELL_OFF;
      night_value = SEQ_ZIGARUDE_CELL_ON;
    }
    for( i = 0; i < GFL_NELEMS(sc_DataZigarudeCellTableDay); i++ ){
      u32 id = sc_DataZigarudeCellTableDay[i];
      if( evwork->GetEventWork(id) < SEQ_ZIGARUDE_CELL_GET ){
        evwork->SetEventWork(id, day_value);
      }
    }
    for( i = 0; i < GFL_NELEMS(sc_DataZigarudeCellTableNight); i++ ){
      u32 id = sc_DataZigarudeCellTableNight[i];
      if( evwork->GetEventWork(id) < SEQ_ZIGARUDE_CELL_GET ){
        evwork->SetEventWork(id,night_value);
      }
    }      
  }
#endif //momijiではヌシールという仕様に変更。昼夜で表示が変わることはない
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 新しいオブジェクトを生成&取得
 *
 * @param scrid スクリプトID
 * @param p_event 実行元イベントへのポインタ
 * @param target_actor_id   対象のフィールドアクターのEventID(起動時に設定)
 * @param target_actor_type 対象のフィールドアクターのType(起動時に設定)
 * @param p_talk_model   対象の動作モデルへのポインタ(話しかけ起動などの時に設定)
 *
 * @return 新しいオブジェクト
 */
/* ------------------------------------------------------------------------- */
RegularObject * FieldScriptSystem::GetNewObject( SCRIPTID scrid, GameSys::GameEvent* p_event, u32 tareget_event_id, FieldActorType target_actor_type, MoveModel::FieldMoveModel* p_talk_model )
{
  Work* work = CreateWork( p_event, tareget_event_id, target_actor_type, p_talk_model, scrid, ACMDLIST_MAX );
  ScriptInfo * scrInfo = work->GetScriptInfo();
 
#if 0
  { // データ読み込み
    ZoneDataAccessor zoneDataAccessor( m_pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_ZoneID ) );
    //ZoneDataLoader* zone = m_pGameManager->GetGameData()->GetZoneDataLoader();
    scrInfo->SetScriptInfo( m_ZoneID, scrid, &zoneDataAccessor );
  }
#endif

  RegularObject * newObj = m_ObjectContainer->AcquireObject( scrInfo->m_ScriptType );
  newObj->SetGameManager( m_pGameManager );
  newObj->SetWork( work );
  
  if (newObj->IsLoaded() )
  { // 通常 起動
    newObj->SetGlobalParameterInteger( "g_mode", scrInfo->m_ScriptId );
  }
  else
  { // ロード開始
    SCRIPT_PRINT("Script Read Req(%d)\n", scrInfo->m_AmxId );
    
    // 断片化するのでファイル読込常駐領域を使用
    m_ScriptBufferManager->RequestReadFile( m_pHeapFileRead, scrInfo->m_AmxId, scrInfo->m_AmxFileName );
  }

  return newObj;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプト実行リクエスト
 *
 * @param scrid 実行するスクリプトID
 * @param p_event 実行元イベントへのポインタ
 * @param p_target_actor 対象のアクターへのポインタ(話しかけ起動などの時に設定)
 * @param p_talk_model   対象の動作モデルへのポインタ(話しかけ起動などの時に設定)
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::RunRequest( SCRIPTID scrid, GameSys::GameEvent* p_event, IFieldActorBase* p_target_actor, MoveModel::FieldMoveModel* p_talk_model )
{
  u32 tareget_event_id = Work::TARGET_ACTOR_ID_NONE;
  FieldActorType target_actor_type = FIELD_ACTOR_TYPE_MAX;

  if( p_target_actor != NULL ){
    tareget_event_id = p_target_actor->GetDataEventID();
    target_actor_type = p_target_actor->GetType();
  }
  RunRequest( scrid, p_event, tareget_event_id, target_actor_type, p_talk_model );
}

void FieldScriptSystem::RunRequest( SCRIPTID scrid, GameSys::GameEvent* p_event, u32 tareget_event_id, FieldActorType target_actor_type, MoveModel::FieldMoveModel* p_talk_model )
{
  SCRIPT_PRINT( "script run req scrid=%d zone=%d \n", scrid, m_ZoneID );

  GFL_ASSERT( IsRunning() == false );

  // 共通スクリプトロード中は禁止
  if( m_ScriptBufferManager->IsReadAllFinished() == false )
  {
    SCRIPT_PRINT( "RunRequest : CommonLoading !\n" );
    return;
  }

  m_pNowObject = GetNewObject( scrid, p_event, tareget_event_id, target_actor_type, p_talk_model );
#if PM_DEBUG
  SCRIPT_PRINT("ScriptRun  type(%d):id(%5d):%s\n", 
    m_pNowObject->GetWork()->GetScriptInfo()->m_ScriptType, m_pNowObject->GetWork()->GetScriptInfo()->m_ScriptId, m_pNowObject->GetWork()->GetScriptInfo()->m_AmxFileName );
#endif
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 子スクリプトを起動
 *
 * @param parent 親スクリプトオブジェクトへのポインタ
 * @param scrid 実行するスクリプトID
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::CallChildScript( ScriptObject * parent, SCRIPTID scrid )
{
  GFL_ASSERT( IsRunning() == true );
  GFL_ASSERT( m_pNowObject == parent );
  Work* pWork = m_pNowObject->GetWork();

  m_pNowObject = GetNewObject( scrid, pWork->GetBaseEvent(), pWork->GetTargetActorID(), pWork->GetTargetActorType(), pWork->GetTargetModel() );
  parent->GetWork()->CopyGlobalParamsToChild( m_pNowObject->GetWork() );
  parent->Halt();
#if PM_DEBUG
  SCRIPT_PRINT("ScriptCall type(%d):id(%5d):%s\n", pWork->GetScriptInfo()->m_ScriptType, pWork->GetScriptInfo()->m_ScriptId, pWork->GetScriptInfo()->m_AmxFileName );
#endif
}

/* ------------------------------------------------------------------------- */
/**
* @brief 並列スクリプトを起動
*
* @param parent 親スクリプトオブジェクトへのポインタ
* @param scrid 実行するスクリプトID
* @param pHeap 使用するヒープ
* @param param0 引き数として渡すワーク値 0 SCWK_PARAM0 にはいる
* @param param1 引き数として渡すワーク値 1 SCWK_PARAM1 にはいる
* @param param2 引き数として渡すワーク値 2 SCWK_PARAM2 にはいる
* @param param3 引き数として渡すワーク値 3 SCWK_PARAM3 にはいる
*
* @return 登録ハンドル
*/
/* ------------------------------------------------------------------------- */
s32 FieldScriptSystem::CallParallelScript( RegularObject * parent, SCRIPTID scrid, gfl2::heap::HeapBase* pHeap, u32 param0, u32 param1, u32 param2, u32 param3 )
{
  size_t heap_prev = pHeap->GetTotalFreeSize();
  Work* pWork = m_pNowObject->GetWork();
  Work* pNewWork = CreateWork( pWork->GetBaseEvent(), pWork->GetTargetActorID(), pWork->GetTargetActorType(), pWork->GetTargetModel(), scrid, ACMDLIST_MAX );
  ScriptInfo * scrInfo = pNewWork->GetScriptInfo();

#if 0
  { // データ読み込み
    ZoneDataAccessor zoneDataAccessor( m_pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_ZoneID ) );
    //ZoneDataLoader* zone = m_pGameManager->GetGameData()->GetZoneDataLoader();
    scrInfo->SetScriptInfo( m_ZoneID, scrid, &zoneDataAccessor );
  }
#endif

  RegularObject * newObj = GFL_NEW( pHeap ) RegularObject( this, pHeap, pHeap );
  newObj->SetGameManager( m_pGameManager );
  newObj->SetWork( pNewWork );
  pNewWork->SetParameters( param0, param1, param2, param3 );

  // 本流と同じamxファイルならシェアする
  if( scrInfo->m_AmxId == pWork->GetScriptInfo()->m_AmxId )
  {
    newObj->LoadShare( parent );
  }

  if (newObj->IsLoaded() )
  { // 通常 起動
    newObj->SetGlobalParameterInteger( "g_mode", scrInfo->m_ScriptId );
  }
  else
  { // ロード開始
    SCRIPT_PRINT("Script Read Req(%d)\n", scrInfo->m_AmxId );

    // 断片化するのでファイル読込常駐領域を使用
    m_ScriptBufferManager->RequestReadFile( m_pHeapFileRead, scrInfo->m_AmxId, scrInfo->m_AmxFileName );
  }

  parent->GetWork()->CopyGlobalParamsToChild( newObj->GetWork() ); // 必要？
  // parent->Halt();

  s32 result = parent->RegistParallelObject( newObj, pHeap );
  size_t heap_after = pHeap->GetTotalFreeSize();
  GFL_PRINT("CallParallelScript id=%d size=%d byte\n", scrid, heap_prev - heap_after );
  return result;
}


#if PM_DEBUG

/* ------------------------------------------------------------------------- */
/**
 * @brief 子スクリプトを起動 Debug用Onetime実行 
 *
 * @param parent 親スクリプトオブジェクトへのポインタ
 * @param scrid 実行するスクリプトID
 * 
 * すべてを同期読み込みで処理するので処理負荷がん無視。ロック上等
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::DebugCallChildScriptOnetime( ScriptObject * parent, SCRIPTID scrid )
{
  GFL_ASSERT( IsRunning() == true );
  GFL_ASSERT( m_pNowObject == parent );
  Work* pParentWork = m_pNowObject->GetWork();
  void* pBuffer = NULL;
  u32 size = 0;

  //m_pNowObject = GetNewObject( scrid, pParentWork->GetBaseEvent(), pParentWork->GetTargetActor(), pParentWork->GetTargetModel() );

  Work* pWork = CreateWork( pParentWork->GetBaseEvent(), pParentWork->GetTargetActorID(), pParentWork->GetTargetActorType(), pParentWork->GetTargetModel(), scrid, ACMDLIST_MAX );
  ScriptInfo * scrInfo = pWork->GetScriptInfo();

  //SCRIPT_PRINT("DebugScriptCall type(%d):id(%5d):%s\n", pWork->GetScriptInfo()->m_ScriptType, pWork->GetScriptInfo()->m_ScriptId, pWork->GetScriptInfo()->m_AmxFileName );

  RegularObject * newObj = m_ObjectContainer->AcquireObject( scrInfo->m_ScriptType );
  newObj->SetGameManager( m_pGameManager );
  newObj->SetWork( pWork );
  
  // ロード開始
  if( newObj->IsLoaded() == false )
  {  
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

      #if PM_DEBUG
      req.arcId = ARCID_FIELD_SCRIPTD;
      #else
      req.arcId = ARCID_FIELD_SCRIPT;
      #endif

      req.datId = scrInfo->m_AmxId;
      req.heapForReq = m_pHeapFileRead->GetLowerHandle();
      req.heapForBuf = m_pHeapFileRead->GetLowerHandle();
      req.align = 4;
      req.ppBuf = &pBuffer;
      req.pBufSize = &size;

#if SCRIPT_HIO_ENABLE
      if( FieldScriptSystem::DebugHioEnableFlag ){
        req.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
      }
#endif
      m_pGameManager->GetAsyncFileManager()->SyncArcFileLoadData(req);

    }
    newObj->SetRegistData( scrInfo->m_RegFuncId );
    newObj->LoadNormal( pBuffer, size, scrInfo->m_AmxFileName );
  }


  pParentWork->CopyGlobalParamsToChild( newObj->GetWork() );
  newObj->SetGlobalParameterInteger( "g_mode", scrid );

  parent->Halt();
  newObj->Update();
  if( newObj->IsFinished() == false ){
    GFL_ASSERT_MSG(0,"Waitが必要なコマンドは使えません\n");
  }

  m_ObjectContainer->ReleaseObject( newObj );

  if( pBuffer != NULL ){
    GflHeapSafeFreeMemory( pBuffer );
  }

}
#endif


/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプト実行中判定
 *
 * @return ture:実行中, false:実行中でない
 */
/* ------------------------------------------------------------------------- */
bool FieldScriptSystem::IsRunning( void ) const
{
  return m_pNowObject != NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトロード中判定
 *
 * @return true:ロード中, false:ロード中でない
 */
/* ------------------------------------------------------------------------- */
bool FieldScriptSystem::IsLoading( void ) const
{
  return !m_ScriptBufferManager->IsReadAllFinished();
}


/* ------------------------------------------------------------------------- */
/**
 * @brief ワーク生成
 *
 * @param p_event 実行元イベントへのポインタ
 * @param target_actor_id   対象のフィールドアクターのEventID(起動時に設定)
 * @param target_actor_type 対象のフィールドアクターのType(起動時に設定)
 * @param p_talk_model   対象の動作モデルへのポインタ(話しかけ起動などの時に設定)
 * @param scr_id ワーク生成元のスクリプトID
 * @param acmdlist_max アクションコマンドリストの最大数
 *
 * @return 生成したスクリプトワークへのポインタ
 */
/* ------------------------------------------------------------------------- */
Work* FieldScriptSystem::CreateWork( GameSys::GameEvent* p_event, u32 tareget_event_id, FieldActorType target_actor_type, MoveModel::FieldMoveModel* p_talk_model, SCRIPTID scr_id /*= SCRID_DEFAULT*/, u16 acmdlist_max /*= 128*/ )
{
  Work* work;
  
  work = GFL_NEW( m_pHeapWork ) Work( m_pGameManager, m_pHeapWork, acmdlist_max );

  if ( scr_id != SCRID_DEFAULT )
  {
    ZoneDataAccessor zoneDataAccessor( m_pGameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_ZoneID ) );
    work->GetScriptInfo()->SetScriptInfo( m_ZoneID, scr_id, &zoneDataAccessor );
  }

  work->SetBaseEvent( p_event );
  work->SetTargetModel( p_talk_model );
  work->SetTargetActor( tareget_event_id, target_actor_type );
  work->SetFadeFlag( m_bFade );

  return work;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief シングルトンであるスクリプトシステムへのポインタを取得
 *
 * @return スクリプトシステムへのポインタ
 */
/* ------------------------------------------------------------------------- */
FieldScriptSystem* FieldScriptSystem::GetInstance( void )
{
  GFL_ASSERT(pThis);
  return pThis;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトのアップデート処理
 *
 * @return pawnのupdate実行結果
 */
/* ------------------------------------------------------------------------- */
s32 FieldScriptSystem::Update( void )
{
  s32 ret = gfl2::pawn::PawnBase::GFL_PAWN_NOT_AVAILABLE;
  
  RegularObject * targetObj = m_pNowObject;
  if ( targetObj == NULL ) return ret;

  Work * work = targetObj->GetWork();
  if ( work == NULL )
  {
    GFL_ASSERT( 0 );
    return ret;
  }

  ScriptInfo * scrInfo = work->GetScriptInfo();
  if (!targetObj->IsLoaded())
  { //利用できない→ロードされていない
    if ( m_ScriptBufferManager->IsReadFinished( scrInfo->m_AmxId ) == false )
    { // 共通スクリプトのロード中はスクリプトを走らせない
      return ret;
    }
    void * buffer = m_ScriptBufferManager->GetBuffer( scrInfo->m_AmxId );
    size_t size   = m_ScriptBufferManager->GetBufferSize( scrInfo->m_AmxId );

    targetObj->SetRegistData( scrInfo->m_RegFuncId );
    targetObj->LoadNormal( buffer, size, scrInfo->m_AmxFileName );
    targetObj->SetGlobalParameterInteger( "g_mode", scrInfo->m_ScriptId );
    m_ScriptBufferManager->ReleaseBuffer( scrInfo->m_AmxId );
  }

  // 実行関数
  if( !m_bForceEnd )
  {
    CheckChrActionEnd();

    ret = targetObj->Update();
  
    /// 
    if( m_pScriptTrialModel )  m_pScriptTrialModel->Update();

    // メッセージ更新処理
    if( m_MsgUpdateFunc ) 
    {
      if( m_MsgUpdateFunc( m_MsgUpdateLabel, m_pMsgUpdateWork ) )
      {
        m_MsgUpdateFunc   = NULL;
        m_MsgUpdateLabel  = 0;
        m_pMsgUpdateWork  = NULL;
      }
    }

    // モーション終了監視処理
    CheckChrMotionEnd();
  }

  // 実行終了チェック
  if( targetObj->IsFinished() || m_bForceEnd )
  {
    // 強制終了の実行
    if( m_bForceEnd )
    {
      //GlobalCallで多段積んだ状態でForceEndをかけた際に、カレント以外の終了処理が呼ばれていなかったのを修正
      //関連バグ BTS4039
      m_pNowObject = targetObj;
      do {
        m_pNowObject->ClearSleepAndRestart();
        m_pNowObject = m_ObjectContainer->ReleaseObject( m_pNowObject );
      }while ( m_pNowObject != NULL );
      m_bForceEnd = false;
    }
    else
    {
      m_pNowObject = m_ObjectContainer->ReleaseObject( targetObj );
    }

    // モーション監視クリア
    ChrMotionEndWatchClearAll();

    // アクション監視クリア
    ChrClearActionAfterRotAll();

  }

#if PM_DEBUG
  if( GetIsDebugMsgLabel() && Debug::DebugTypes::s_pDrawUtilTextUpper && m_sDebugMsgLabel[0] != '\0' )
  {
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextColor( gfl2::math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextScale( 1.0f, 1.0f );
  #if defined(GF_PLATFORM_WIN32)
    Debug::DebugTypes::s_pDrawUtilTextUpper->DrawTextW( 2, 2, m_sDebugMsgLabel );
  #else
    Debug::DebugTypes::s_pDrawUtilTextUpper->DrawText( 2, 2, m_sDebugMsgLabel );
  #endif
      
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextScale( 1.0f, 1.0f );
  #if defined(GF_PLATFORM_WIN32)
    Debug::DebugTypes::s_pDrawUtilTextUpper->DrawTextW( 0, 0, m_sDebugMsgLabel );
  #else
    Debug::DebugTypes::s_pDrawUtilTextUpper->DrawText( 0, 0, m_sDebugMsgLabel );
  #endif
  }
#endif //PM_DEBUG

  return ret;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  スクリプト実行を強制的に終了させる。
 */
//-----------------------------------------------------------------------------
void FieldScriptSystem::ForceEnd(void)
{
  // 強制終了設定
  m_bForceEnd = true;

  // スクリプト：待機モードへ
  RegularObject * zoneObj = m_ObjectContainer->GetZoneObject();
  zoneObj->SetGlobalParameterInteger( "g_mode", SCRID_DEFAULT );
  
  SCRIPT_PRINT( "Script Force End\n" );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief　ゾーンスクリプト単発呼び出し
 *
 * @param buffer スクリプトバッファ
 * @param size スクリプトバッファサイズ 
 * @param zone_id ゾーンID
 * @param script_id 実行するスクリプトID
 * @param p_heap ヒープへのポインタ
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::CallZoneScriptOnetime( gfl2::fs::BinLinkerAccessor* pBinLinker, u32 index, ZoneID zone_id, SCRIPTID script_id, gfl2::heap::HeapBase* p_heap )
{
  m_initScriptTiming = true;
  if ( p_heap == NULL ){ p_heap = m_pHeap; }
  p_heap = p_heap->GetLowerHandle();

  const char* amxname = this->GetAmxName( SCRIPT_TYPE_ZONE, zone_id );
  size_t size = pBinLinker->GetDataSize(index);
  void* buffer = pBinLinker->GetData(index);
  
  SCRIPT_PRINT("Call ZoneScriptOneTime = %s scrid=%d \n", amxname, script_id );
  
#if SCRIPT_HIO_ENABLE

  void * temp_buffer = NULL;
  size_t temp_size;

  // @note 容量が余っているDEVICEを使う
  gfl2::heap::HeapBase* p_debug = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER)->GetLowerHandle();
  temp_buffer = CreateTempBuffer( m_pGameManager->GetAsyncFileManager(), p_debug/*p_heap*/, amxname, &temp_size );

  if ( temp_buffer )
  {
    buffer  = temp_buffer;
    size    = temp_size;
  }
  
#endif // #if SCRIPT_HIO_ENABLE

  InitObject* p_init = GFL_NEW( p_heap ) InitObject( p_heap, m_pGameManager );
  Work* work = GFL_NEW( p_heap ) Work( m_pGameManager, p_heap );
  //work->SetEventWorkValue( ZONEWORK_MAPCHG, type );
  p_init->SetWork( work );
  p_init->LoadExt( buffer, size, "amx", 0 );
  p_init->SetGlobalParameterInteger( "g_mode", script_id );
  p_init->Update();
  GFL_ASSERT( p_init->IsFinished() == true );
  GFL_SAFE_DELETE( work );
  GFL_SAFE_DELETE( p_init );

#if SCRIPT_HIO_ENABLE
  {
    GflHeapSafeFreeMemory( temp_buffer );
  }
#endif // #if SCRIPT_HIO_ENABLE
  m_initScriptTiming = false;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief 初期化スクリプト呼び出し：ゾーンチェンジ
 *
 * @param buffer スクリプトバッファ
 * @param size スクリプトバッファサイズ 
 * @param zone_id ゾーンID
 * @param mapchange_type マップ遷移タイプ
 * @param p_heap ヒープへのポインタ
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::CallZoneChange( gfl2::fs::BinLinkerAccessor* pBinLinker, u32 index, ZoneID zone_id, ScrZoneInitType type, gfl2::heap::HeapBase* p_heap )
{
  m_initScriptTiming = true;
  if ( p_heap == NULL ){ p_heap = m_pHeap; }
  p_heap = p_heap->GetLowerHandle();

  u16 script_id = SCRID_INIT_ZONECHANGE;
  int level = 0;

  const char* amxname = this->GetAmxName( SCRIPT_TYPE_ZONEINIT, zone_id );
  size_t size = pBinLinker->GetDataSize(index);
  void* buffer = pBinLinker->GetData(index);
  
  if( type == SCR_ZONEINIT_WALK_OBJECT ){
    script_id = SCRID_INIT_ZONECHANGE_SEAMLESS;
    level = 1;
  }

  SCRIPT_PRINT("Call ZoneChange Script = %s init_type=%d \n", amxname, type );
  
#if SCRIPT_HIO_ENABLE

  void * temp_buffer = NULL;
  size_t temp_size;

  // @note 容量が余っているDEVICEを使う
  gfl2::heap::HeapBase* p_debug = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER)->GetLowerHandle();
  temp_buffer = CreateTempBuffer( m_pGameManager->GetAsyncFileManager(), p_debug/*p_heap*/, amxname, &temp_size );

  if ( temp_buffer )
  {
    buffer  = temp_buffer;
    size    = temp_size;
  }
  
#endif // #if SCRIPT_HIO_ENABLE

  InitObject* p_init = GFL_NEW( p_heap ) InitObject( p_heap, m_pGameManager );
  Work* work = GFL_NEW( p_heap ) Work( m_pGameManager, p_heap );
  work->SetEventWorkValue( ZONEWORK_MAPCHG, type );
  p_init->SetWork( work );
  p_init->LoadExt( buffer, size, "amx", level );
  p_init->SetGlobalParameterInteger( "g_mode", script_id );
  p_init->Update();
  GFL_ASSERT( p_init->IsFinished() == true );
  GFL_SAFE_DELETE( work );
  GFL_SAFE_DELETE( p_init );

#if SCRIPT_HIO_ENABLE
  {
    GflHeapSafeFreeMemory( temp_buffer );
  }
#endif // #if SCRIPT_HIO_ENABLE
  m_initScriptTiming = false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 初期化スクリプト呼び出し：フィールド復帰（コンティニュー時も）
 *
 * @param pTempHeap 一時ヒープへのポインタ
 * @param buffer スクリプトバッファ
 * @param size スクリプトバッファサイズ 
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::CallFieldRecover( gfl2::fs::BinLinkerAccessor* pBinLinker, u32 index, gfl2::heap::HeapBase * pTempHeap )
{
  m_initScriptTiming = true;
  const char* amxname = this->GetAmxName( SCRIPT_TYPE_ZONEINIT, m_ZoneID );
  size_t size = pBinLinker->GetDataSize(index);
  void* buffer = pBinLinker->GetData(index);
  
  SCRIPT_PRINT("Call FieldRecover Script %d = %s \n", index, "amx" );
  PrintHeap( pTempHeap );

#if SCRIPT_HIO_ENABLE
  void * temp_buffer = NULL;
  u32    temp_size;

  gfl2::heap::HeapBase* p_debug = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER)->GetLowerHandle();
  temp_buffer = CreateTempBuffer( m_pGameManager->GetAsyncFileManager(), p_debug/*pTempHeap*/, amxname, &temp_size );

  if ( temp_buffer )
  {
    buffer  = temp_buffer;
    size    = temp_size;
  }
#endif // #if SCRIPT_HIO_ENABLE

  InitObject* p_init = GFL_NEW( pTempHeap ) InitObject( pTempHeap, m_pGameManager );
  Work* work = GFL_NEW( m_pHeapWork ) Work( m_pGameManager, m_pHeapWork );
  p_init->SetWork( work );
  p_init->LoadExt( buffer, size, "amx", 1 );
  p_init->SetGlobalParameterInteger( "g_mode", SCRID_INIT_RECOVER );
  p_init->Update();
  GFL_ASSERT( p_init->IsFinished() == true );
  GFL_SAFE_DELETE( work );
  GFL_SAFE_DELETE( p_init );

#if SCRIPT_HIO_ENABLE
  {
    GflHeapSafeFreeMemory( temp_buffer );
  }
#endif // if SCRIPT_HIO_ENABLE

#if PM_DEBUG
  {
    MoveModel::FieldMoveModelManager* pMoveModelManager = m_pGameManager->GetGameData()->GetFieldCharaModelManager();
    Field::MoveModel::FieldMoveModelPlayer* pPlayer = 
      reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

    // フェスサークルのスクリプトもバミリチェック回避する
    if( m_pGameManager->GetGameData()->IsNowJoinFestaFieldData() )
    {
      pMoveModelManager->ClearCheckSafetyPositionForEventScript();
    }
  }
#endif //PM_DEBUG
  m_initScriptTiming = false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 初期化スクリプト呼び出し：フィールド初期化
 *
 * @param pTempHeap 一時ヒープへのポインタ
 * @param buffer スクリプトバッファ
 * @param size スクリプトバッファサイズ 
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::CallFieldInit(  gfl2::fs::BinLinkerAccessor* pBinLinker, u32 index, gfl2::heap::HeapBase * pTempHeap )
{
  m_initScriptTiming = true;
  const char* amxname = this->GetAmxName( SCRIPT_TYPE_ZONEINIT, m_ZoneID );
  size_t size = pBinLinker->GetDataSize(index);
  void* buffer = pBinLinker->GetData(index);

  SCRIPT_PRINT("Call FieldInit Script %d = %s \n", index, "amx" );

#if SCRIPT_HIO_ENABLE
  void * temp_buffer = NULL;
  u32    temp_size;

  gfl2::heap::HeapBase* p_debug = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER)->GetLowerHandle();
  temp_buffer = CreateTempBuffer( m_pGameManager->GetAsyncFileManager(), p_debug/*pTempHeap*/, amxname, &temp_size );

  if ( temp_buffer )
  {
    buffer  = temp_buffer;
    size    = temp_size;
  }
#endif // #if SCRIPT_HIO_ENABLE

  InitObject* p_init = GFL_NEW( pTempHeap ) InitObject( pTempHeap, m_pGameManager );
  Work* work = GFL_NEW( m_pHeapWork ) Work( m_pGameManager, m_pHeapWork );
  p_init->SetWork( work );
  p_init->LoadExt( buffer, size, "amx", 1 );
  p_init->SetGlobalParameterInteger( "g_mode", SCRID_INIT_INIT );
  p_init->Update();
  GFL_ASSERT( p_init->IsFinished() == true );
  GFL_SAFE_DELETE( work );
  GFL_SAFE_DELETE( p_init );

#if SCRIPT_HIO_ENABLE
  {
    GflHeapSafeFreeMemory( temp_buffer );
  }
#endif // #if SCRIPT_HIO_ENABLE

#if PM_DEBUG
  {
    MoveModel::FieldMoveModelManager* pMoveModelManager = m_pGameManager->GetGameData()->GetFieldCharaModelManager();
    Field::MoveModel::FieldMoveModelPlayer* pPlayer = 
      reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

    // フェスサークルのスクリプトはバミリチェック回避する
    if( m_pGameManager->GetGameData()->IsNowJoinFestaFieldData() )
    {
      pMoveModelManager->ClearCheckSafetyPositionForEventScript();
    }
  }
#endif //PM_DEBUG

  m_initScriptTiming = false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 初期化スクリプト呼び出し：ゲーム開始
 *
 * @param p_heap ヒープへのポインタ
 * @param type 初期化タイプ
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::CallGameInit( gfl2::heap::HeapBase* p_heap, GAME_INIT_TYPE type )
{
  m_initScriptTiming = true;
  void *p_buffer = NULL;
  u32 size;
  SCRIPTID scrid = 0;
  
  switch( type )
  {
  case GAME_INIT_TYPE_INIT:
    scrid = SCRID_INIT_SCRIPT;
    break;
  case GAME_INIT_TYPE_DEBUG:
    scrid = SCRID_INIT_DEBUG_SCRIPT;
    break;
  case GAME_INIT_TYPE_GAMECLEAR:
    scrid = SCRID_INIT_GAMECLEAR_SCRIPT;
    break;
  default : GFL_ASSERT(0);
    return;
  }
  Work* p_work = CreateWork( NULL, 0, FIELD_ACTOR_TYPE_MAX, NULL, scrid );
  ScriptInfo * scrInfo = p_work->GetScriptInfo();

  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;

    req.arcId = ARCID_FIELD_SCRIPT;
    req.heapForFile = p_heap;
    req.heapForArcSrc = p_heap;
    req.heapForReq = p_heap->GetLowerHandle();
    req.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::SAME_AS_MANAGER;
    
#if SCRIPT_HIO_ENABLE
    if( FieldScriptSystem::DebugHioEnableFlag ){
      req.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
    }
#endif

    m_pGameManager->GetAsyncFileManager()->SyncArcFileOpen(req);
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

    req.arcId      = ARCID_FIELD_SCRIPT;
    req.datId      = scrInfo->m_AmxId;
    req.ppBuf      = &p_buffer;
    req.pBufSize   = &size;
    req.heapForBuf = p_heap;
    req.align      = 128;
    req.heapForReq = p_heap->GetLowerHandle();
    req.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::SAME_AS_MANAGER;

#if SCRIPT_HIO_ENABLE
    if( FieldScriptSystem::DebugHioEnableFlag ){
      req.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
    }
#endif
    m_pGameManager->GetAsyncFileManager()->SyncArcFileLoadData(req);
  }
  InitObject* p_init = GFL_NEW( p_heap ) InitObject( p_heap, m_pGameManager );
  p_init->SetWork( p_work );
  p_init->LoadExt( p_buffer, size, ScriptInfo::GetAmxName( SCRIPT_TYPE_COMMON, scrInfo->m_AmxId ), 0 );
  p_init->SetGlobalParameterInteger( "g_mode", scrid );
  p_init->Update();
  GFL_ASSERT( p_init->IsFinished() == true );
  
  
  GFL_SAFE_DELETE( p_work );
  GFL_SAFE_DELETE( p_init );
  GflHeapSafeFreeMemory( p_buffer );

  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.fileName   = NULL;
    req.arcId = ARCID_FIELD_SCRIPT;
    req.heapForReq = p_heap;
    m_pGameManager->GetAsyncFileManager()->SyncArcFileClose(req); 
  }
  m_initScriptTiming = false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief INTRスクリプト呼び出し(即時実行スクリプト)
 *
 * @param scrid 実行するスクリプトID
 * @param pos_event_id 実行元POSイベントのID
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::CallIntrScript( SCRIPTID scrid, int pos_event_id )
{
  GFL_ASSERT( IsRunning() == false );
  RegularObject * zoneObj = m_ObjectContainer->GetZoneObject();

  Work* work = CreateWork( NULL, 0, FIELD_ACTOR_TYPE_MAX, NULL, scrid );

  // @fix GFNMcat[3619]:IntrPosイベントコール時にスクリプトオブジェクトにGameManagerが設定されていない、対処
  zoneObj->SetGameManager( m_pGameManager );

  zoneObj->SetWork( work );
  zoneObj->GetWork()->SetPosID( pos_event_id );
  zoneObj->SetGlobalParameterInteger( "g_mode", scrid );
  zoneObj->Update();
  GFL_ASSERT( zoneObj->IsFinished() == true );
  zoneObj->Finalize();
  // GFL_SAFE_DELETE( work );
}


/* ------------------------------------------------------------------------- */
/**
 * @brief 起動パラメータをスクリプトワークにセット
 *
 * @param p0 パラメータ0
 * @param p1 パラメータ1
 * @param p2 パラメータ2
 * @param p3 パラメータ3
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::SetParameters( u16 p0, u16 p1, u16 p2, u16 p3 )
{
  GFL_ASSERT( m_pNowObject );
  m_pNowObject->GetWork()->SetParameters( p0, p1, p2, p3 );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief BGIDをセット
 *
 * @param bgid BGID
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::SetBGID( u32 bgid )
{
  GFL_ASSERT( m_pNowObject );
  m_pNowObject->GetWork()->SetBGID( bgid );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief POSIDをセット
 *
 * @param posid POSID
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::SetPosID( u32 posid )
{
  GFL_ASSERT( m_pNowObject );
  m_pNowObject->GetWork()->SetPosID( posid );
}
  
//-----------------------------------------------------------------------------
/**
 * @brief ワークに通知するフェードフラグを設定
 *
 * @param is_fade フェードフラグ
 */
//-----------------------------------------------------------------------------
void FieldScriptSystem::SetWorkToFadeFlag( bool is_fade )
{
  m_bFade = is_fade;
}
  
/* ------------------------------------------------------------------------- */
/**
 * @brief 現在のカレント スクリプトオブジェクトを取得
 *
 * @return スクリプトオブジェクト
 */
/* ------------------------------------------------------------------------- */
ScriptObject* FieldScriptSystem::GetScriptObject( void ) const
{ 
  GFL_ASSERT( m_pNowObject != NULL );
  return m_pNowObject;
}

// 地形ブロック制御点情報の記憶
void FieldScriptSystem::SetTerrainBlockControlPosition( const gfl2::math::Vector3& rPos )
{
  m_IsTerrainBlockControlSet = true;
  m_TerrainBlockControlPositon = rPos;
}

void FieldScriptSystem::ClearTerrainBlockControlPosition( void )
{
  m_IsTerrainBlockControlSet = false;
}

b32 FieldScriptSystem::IsTerrainBlockControlSet( void ) const
{
  return m_IsTerrainBlockControlSet;
}

void FieldScriptSystem::GetTerrainBlockControlPosition( gfl2::math::Vector3* pPos ) const
{
  *pPos = m_TerrainBlockControlPositon;
}

// BGパーツの記憶
FieldScriptSystem::BG_PARTS_INFO* FieldScriptSystem::GetBgPartsInfo( void )
{
  return &m_BgPartsInfo;
}
// BGパーツの記憶クリア
void FieldScriptSystem::ClearBgPartsParam( void )
{
  gfl2::std::MemClear( &m_BgPartsInfo, sizeof(BG_PARTS_INFO) );
}

void FieldScriptSystem::SetBgPartsLoad( u32 no, b32 useEventHeap )
{
  if( GetBgPartsParam( no ) != NULL )
  {
    return;
  }

  for( u32 i = 0; i < BG_PARTS_MAX; i++ )
  {
    if( m_BgPartsInfo.paramTable[i].use == 1 )
    {
      continue;
    }

    m_BgPartsInfo.paramTable[i].partsNo = no;
    m_BgPartsInfo.paramTable[i].visible = 1;
    m_BgPartsInfo.paramTable[i].use = 1;
    m_BgPartsInfo.paramTable[i].useEventHeap = useEventHeap;
    break;
  }
 
}
void FieldScriptSystem::SetBgPartsUnLoad( u32 no )
{
  BG_PARTS_PARAM* pParam = GetBgPartsParam( no );
  if( pParam )
  {
    pParam->use = 0;
  }
}
void FieldScriptSystem::SetBgPartsVisible( u32 no, b32 flag )
{
  BG_PARTS_PARAM* pParam = GetBgPartsParam( no );
  if( pParam )
  {
    if( flag )
    {
      pParam->visible = 1;
    }
    else
    {
      pParam->visible = 0;
    }
  }
}
FieldScriptSystem::BG_PARTS_PARAM* FieldScriptSystem::GetBgPartsParam( u32 no )
{
  for( u32 i = 0; i < BG_PARTS_MAX; i++ )
  {
    if( m_BgPartsInfo.paramTable[i].use == 0 )
    {
      continue;
    }

    if( m_BgPartsInfo.paramTable[i].partsNo == no )
    {
      return &m_BgPartsInfo.paramTable[i];
    }
  }

  return NULL;
}

u32 FieldScriptSystem::GetBgPartsCount( void )
{
  u32 count = 0;

  for( u32 i = 0; i < BG_PARTS_MAX; i++ )
  {
    if( m_BgPartsInfo.paramTable[i].use == 0 )
    {
      continue;
    }
    ++count;
  }

  return count;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief イベントポジション(イベント相対座標システム基準点)
 * セットされていない時は(0,0,0)が返るので、有効無効で扱いを変える必要はない
 */
/* ------------------------------------------------------------------------- */
gfl2::math::Vector3 FieldScriptSystem::GetEventPosition()
{
  gfl2::math::Vector3 pos(0,0,0);
  RegularObject *targetObj = m_pNowObject;

  if( targetObj == NULL ) return pos;

  Work * work = targetObj->GetWork();
  if ( work == NULL ){
    GFL_ASSERT( 0 );
    return pos;
  }
  return work->GetEventPosition();
}

void FieldScriptSystem::SetEventPosition( const gfl2::math::Vector3& pos )
{
  RegularObject *targetObj = m_pNowObject;

  if( targetObj == NULL ) return;

  Work * work = targetObj->GetWork();
  if ( work == NULL ){
    GFL_ASSERT( 0 );
    return;
  }
  work->SetEventPosition(pos);
}

void FieldScriptSystem::SetEventPosition( float x, float y, float z )
{
  gfl2::math::Vector3 pos(x,y,z);
  this->SetEventPosition(pos);
}

void FieldScriptSystem::ResetEventPosition()
{
  RegularObject *targetObj = m_pNowObject;

  if( targetObj == NULL ) return;

  Work * work = targetObj->GetWork();
  if ( work == NULL ){
    GFL_ASSERT( 0 );
    return;
  }
  work->ResetEventPosition();
}

bool FieldScriptSystem::CheckEventPositionEnable()
{
  RegularObject *targetObj = m_pNowObject;

  if( targetObj == NULL ) return false;

  Work * work = targetObj->GetWork();
  if ( work == NULL ){
    GFL_ASSERT( 0 );
    return false;
  }
  return work->CheckEventPositionEnable();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief イベントポジション(イベント相対座標システム基準点) オフセット計算
 * 
 * ワールド座標を与えると、イベントポジションからの相対座標を返すユーティリティ
 */
/* ------------------------------------------------------------------------- */
gfl2::math::Vector3 FieldScriptSystem::CalcEventPositionOffset( const gfl2::math::Vector3& targetPos )
{
  gfl2::math::Vector3 eventPos(0,0,0);
  RegularObject *targetObj = m_pNowObject;

  if( targetObj == NULL ) return targetPos;

  Work * work = targetObj->GetWork();
  if ( work == NULL ){
    GFL_ASSERT( 0 );
    return targetPos;
  }
  eventPos = work->GetEventPosition();
  return targetPos-eventPos;
}

gfl2::math::Vector3 FieldScriptSystem::CalcEventPositionOffset( float x, float y, float z )
{
  gfl2::math::Vector3 targetPos(x,y,z);
  return CalcEventPositionOffset( targetPos );
}


/* ------------------------------------------------------------------------- */
/**
 * @brief イベントポジション(イベント相対座標システム基準点) オフセット計算
 * 
 * 相対座標座標を与えると、イベントポジションからワールド座標を計算して返すユーティリティ
 */
/* ------------------------------------------------------------------------- */
gfl2::math::Vector3 FieldScriptSystem::CalcEventPositionWorld( const gfl2::math::Vector3& targetPos )
{
  gfl2::math::Vector3 eventPos(0,0,0);
  RegularObject *targetObj = m_pNowObject;

  if( targetObj == NULL ) return targetPos;

  Work * work = targetObj->GetWork();
  if ( work == NULL ){
    GFL_ASSERT( 0 );
    return targetPos;
  }
  eventPos = work->GetEventPosition();
  return targetPos+eventPos;
}

gfl2::math::Vector3 FieldScriptSystem::CalcEventPositionWorld( float x, float y, float z )
{
  gfl2::math::Vector3 targetPos(x,y,z);
  return CalcEventPositionWorld( targetPos );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  スクリプト終了後に、遷移するイベントを設定
 */
//-----------------------------------------------------------------------------
void FieldScriptSystem::EntryNextEvent( GameSys::GameEvent* next_event )
{
  GFL_ASSERT( m_pEntryNextEvent == NULL );
  m_pEntryNextEvent = next_event;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  エントリーされたイベントを登録
 */
//-----------------------------------------------------------------------------
void FieldScriptSystem::CallEntryNextEvent( void )
{
  if( m_pEntryNextEvent ){
    // イベントをコール
    m_pGameManager->GetGameEventManager()->CallEvent( m_pEntryNextEvent );
    m_pEntryNextEvent = NULL;
  }
}
//-----------------------------------------------------------------------------
/**
 *  @brief  エントリーされたイベントを登録
 */
//-----------------------------------------------------------------------------
void FieldScriptSystem::ChangeEntryNextEvent( void )
{
  if( m_pEntryNextEvent ){
    // イベントを切り替え
    m_pGameManager->GetGameEventManager()->ChangeEvent( m_pEntryNextEvent );
    m_pEntryNextEvent = NULL;
  }
}
//-----------------------------------------------------------------------------
/**
 * @brief 条件を満たすシーン遷移スクリプトがあるか調べる
 * @return  bool  trueのとき存在する
 */
//-----------------------------------------------------------------------------
bool FieldScriptSystem::IsEnableSceneScript( void )
{
  EventWork* ev = m_pGameManager->GetGameData()->GetEventWork();
  SCRIPTID scrid = m_SceneChangeControl->Search( ev );
  if ( scrid == SCRID_DUMMY ) return false;

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 条件を満たすシーン遷移スクリプトを検索、実行させる
 * @return  bool  trueのとき実行する
 */
//-----------------------------------------------------------------------------
bool FieldScriptSystem::SearchRunSceneScript( void )
{
  EventWork* ev = m_pGameManager->GetGameData()->GetEventWork();
  SCRIPTID scrid = m_SceneChangeControl->Search( ev );
  if ( scrid == SCRID_DUMMY ) return false;
  EventScriptCall::CallScriptScene( m_pGameManager, scrid );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 条件を満たすシーン遷移スクリプトを検索、フェードオプションを返す
 */
//-----------------------------------------------------------------------------
SceneChangeFadeMode FieldScriptSystem::CheckSceneScriptFadeMode( void )
{
  EventWork* ev = m_pGameManager->GetGameData()->GetEventWork();
  return m_SceneChangeControl->CheckFadeMode( ev );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 予約スクリプトをセットする
 *
 * @param scrid スクリプトID
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::SetReserveScript( SCRIPTID scrid, u32 p0, u32 p1, u32 p2, u32 p3 )
{
#if PM_DEBUG
#if 0
  // 予約スクリプト起動しないデバッグ
  if (!FieldDebug::DebugEnableReserbScript)
  {
    return;
  }
#endif
#endif

  if( scrid != SCRID_DUMMY ){
    GFL_ASSERT_MSG( m_ReservedScrID == SCRID_DUMMY, "リザーブスクリプトが多重予約されました\n new_scrid=%d old_scrid=%d\n", scrid, m_ReservedScrID );
  }
  m_ReservedScrID = scrid;
  m_ReservedScrParam[0] = p0;
  m_ReservedScrParam[1] = p1;
  m_ReservedScrParam[2] = p2;
  m_ReservedScrParam[3] = p3;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 予約スクリプトが既にセットされているか？
 *
 * @return true されている, false されていない
 */
/* ------------------------------------------------------------------------- */
bool FieldScriptSystem::IsReserveScriptSetting( void ) const
{
  return m_ReservedScrID != SCRID_DUMMY; 
}

//-----------------------------------------------------------------------------
/**
 * @brief 予約スクリプトの発動チェック＆発動
 *
 * @return true:発動, false:発動なし
 */
//-----------------------------------------------------------------------------
bool FieldScriptSystem::CheckRunReserveScript( void )
{
  if( m_ReservedScrID != SCRID_DUMMY )
  {
    EventScriptCall::CallScriptScene(
        m_pGameManager,
        m_ReservedScrID,
        m_ReservedScrParam[0],
        m_ReservedScrParam[1],
        m_ReservedScrParam[2],
        m_ReservedScrParam[3]
      );
    m_ReservedScrID = SCRID_DUMMY;
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 弱い予約スクリプトの発動予約
 *
 * @param scrid スクリプトID
 * @param p0 パラメータ0
 * @param p1 パラメータ1
 * @param p2 パラメータ2
 * @param p3 パラメータ3
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::SetWeakReservedScript( SCRIPTID scrid, u32 p0, u32 p1, u32 p2, u32 p3 )
{
  m_WeakReservedScrID = scrid; 
  m_WeakReservedScrParam[0] = p0;
  m_WeakReservedScrParam[1] = p1;
  m_WeakReservedScrParam[2] = p2;
  m_WeakReservedScrParam[3] = p3;
}

//-----------------------------------------------------------------------------
/**
 * @brief 弱い予約スクリプトの発動チェック＆発動
 *
 * @return true:発動した, false:発動しなかった
 */
//-----------------------------------------------------------------------------
bool FieldScriptSystem::CheckRunWeakReserveScript( void )
{
  if( m_WeakReservedScrID != SCRID_DUMMY )
  {
    EventScriptCall::CallScriptScene( 
        m_pGameManager,
        m_WeakReservedScrID,
        m_WeakReservedScrParam[0],
        m_WeakReservedScrParam[1],
        m_WeakReservedScrParam[2],
        m_WeakReservedScrParam[3]
    );
    
    m_WeakReservedScrID = SCRID_DUMMY;
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief コールバックスクリプトを設定 
 *
 * @param type コールバックタイプ
 * @param scrid スクリプトID
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::RegisterCallBackScript( SCR_CALLBACK_TYPE type, SCRIPTID scrid )
{
  GFL_ASSERT( type < SCR_CALLBACK_TYPE_NUM );
  GFL_ASSERT( m_CallBackType == SCR_CALLBACK_TYPE_NOTHING || m_CallBackType == type );
  GFL_ASSERT( m_CallBackScriptID == SCRID_DEFAULT || m_CallBackScriptID == scrid );
  m_CallBackType = type;
  m_CallBackScriptID = scrid;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief コールバックスクリプトIDを取得
 *
 * @param type 取得するコールバックタイプ
 *
 * @return スクリプトID
 */
/* ------------------------------------------------------------------------- */
SCRIPTID FieldScriptSystem::GetCallBackScriptID( SCR_CALLBACK_TYPE type ) const
{
  if ( m_CallBackType == type ) return m_CallBackScriptID;
  return SCRID_DEFAULT;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ウェイト関数を設定
 *
 * @param amx 実行元のAMXへのポインタ
 * @param func ウェイト関数へのポインタ
 * @param label ユーザーラベル
 * @param p_work ユーザーワーク
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::SetWaitFunc( AMX* amx, WAIT_FUNC func, u32 label, void* p_work ) 
{
  RegularObject* p_obj = ScriptObject::GetClass<RegularObject*>(amx);

  if ( m_pNowObject != p_obj )
  {
    GFL_ASSERT_MSG(0, "通常スクリプト以外のインスタンスからウェイト命令が呼ばれました。\n");
    return;
  }
  
  m_pNowObject->SetWaitFunc( func, label, p_work );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief メッセージUpdate関数を設定
 *
 * @param func ウェイト関数へのポインタ
 * @param label ユーザーラベル
 * @param p_work ユーザーワーク
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::SetMsgUpdateFunc( MSG_UPDATE_FUNC func, u32 label, void* p_work )
{
  if(m_MsgUpdateFunc == NULL)
  {
    m_MsgUpdateFunc     = func;
    m_MsgUpdateLabel    = label;
    m_pMsgUpdateWork    = p_work;
  }
  else
  {
    GFL_ASSERT_STOP_MSG( 0, "MsgUpdateFunc overwrite\n" );
  }
}


/* ------------------------------------------------------------------------- */
/**
 * @brief メッセージUpdate関数の完了をチェック
 *
 *  @retval true  完了
 *  @retval false 途中
 */
/* ------------------------------------------------------------------------- */
bool FieldScriptSystem::IsMsgUpdateFuncEnd()const
{
  if( m_MsgUpdateFunc )
  {
    return false;
  }
  return true;
}
  
/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトを一旦中断させる(次のUpdateで途中から実行される)
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::Suspend( void )
{
  amx_RaiseError( m_pNowObject->GetAmx(), AMX_ERR_SLEEP ); 
}


#if PM_DEBUG
/* ------------------------------------------------------------------------- */
/**
 * @brief 登録されているリザーブスクリプトをクリア(debug用)
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::DebugClearReserveScript( void )
{
  m_ReservedScrID = SCRID_DUMMY;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief キャラエディットリストを取得
 */
/* ------------------------------------------------------------------------- */
const DebugChrEditList* FieldScriptSystem::DebugGetChrEditList( int* p_num )
{
  if( m_pNowObject == NULL ){
    if( p_num != NULL ){
      *p_num = 0;
    }
    return NULL;
  }

  if( p_num != NULL ){
    *p_num = m_pNowObject->GetWork()->DebugChrEditListNumGet();
  }
  return m_pNowObject->GetWork()->DebugChrEditListGet();
}

/* ------------------------------------------------------------------------- */
 /**
 * @brief シーンエディットの有効無効をゲット
 */
/* ------------------------------------------------------------------------- */
bool FieldScriptSystem::DebugCheckSceneEditEnable( int scriptID )
{
  if( !FieldScript::FieldScriptSystem::DebugSceneEditEnableFlag ){
    return false;
  }
  if( scriptID >= 30000 && scriptID < 60000 ){ //main-subのID
    return true;
  }
  if( scriptID >= SCRID_DEBUG_IWASAWA && scriptID < (SCRID_DEBUG_IWASAWA+100) ){
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief シーンエディットのスキップコードをセット
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::DebugSetSceneSkipCode( int start, int end,  int max )
{
  RegularObject * targetObj = m_pNowObject;
  if( targetObj == NULL ) return;

  if(targetObj->IsLoaded())
  {
    targetObj->SetGlobalParameterInteger( "g_skip_point", start );
    targetObj->SetGlobalParameterInteger( "g_skip_point_end", end );
    targetObj->SetGlobalParameterInteger( "g_skip_point_max", max );
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief シーンエディットのスキップコードをゲット
 */
/* ------------------------------------------------------------------------- */
int FieldScriptSystem::DebugGetSceneEditParam( DebugSceneEditParam * param )
{
  RegularObject * targetObj = m_pNowObject;
  if( targetObj == NULL ) return 0;

  Work * work = targetObj->GetWork();
  if ( work == NULL ){
    GFL_ASSERT( 0 );
    return 0;
  }
  ScriptInfo * scrInfo = work->GetScriptInfo();
 
  if (!targetObj->IsLoaded())
  { //利用できない→ロードされていない
    
    if ( m_ScriptBufferManager->IsReadFinished( scrInfo->m_AmxId ) == false )
    { // 共通スクリプトのロード中はスクリプトを走らせない
      return -1;
    }
    void * buffer = m_ScriptBufferManager->GetBuffer( scrInfo->m_AmxId );
    size_t size   = m_ScriptBufferManager->GetBufferSize( scrInfo->m_AmxId );

    targetObj->SetRegistData( scrInfo->m_RegFuncId );
    targetObj->LoadNormal( buffer, size, scrInfo->m_AmxFileName );
    targetObj->SetGlobalParameterInteger( "g_mode", scrInfo->m_ScriptId );
    m_ScriptBufferManager->ReleaseBuffer( scrInfo->m_AmxId );
  }
  return targetObj->DebugGetSceneEditParam(param);
}

#endif // PM_DEBUG



/* ------------------------------------------------------------------------- */
/**
* @brief　キャラクターモーション終了チェック開始
*/
/* ------------------------------------------------------------------------- */
b32 FieldScriptSystem::ChrMotionEndWatchStart( s32 event_id, u32 anime_id )
{
  for( u32 i = 0; i < MOTION_END_WATCH_DATA_MAX; i++ )
  {
    MOTION_END_WATCH_DATA* pData = &m_ChrMotionEndWatchData[ i ];
    if( pData->event_id != 0 )
    {
      continue;
    }

    pData->event_id = event_id;
    pData->anime_id = anime_id;
    pData->disable_count = 1;
    return true;
  }

  return false;
}

/* ------------------------------------------------------------------------- */
/**
* @brief　キャラクターモーション終了チェック終了
*/
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::ChrMotionEndWatchClear( s32 event_id )
{
  for( u32 i = 0; i < MOTION_END_WATCH_DATA_MAX; i++ )
  {
    MOTION_END_WATCH_DATA* pData = &m_ChrMotionEndWatchData[ i ];
    if( pData->event_id != event_id )
    {
      continue;
    }

    pData->event_id = 0;
  }
}



/* ------------------------------------------------------------------------- */
/**
* @brief　キャラクターモーション終了のチェック
*/
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::CheckChrMotionEnd( void )
{
  // @fix GFNMcat[2374]:ポケモンライド中に小さなNPCやポケモンに話しかけると、モーションが停止する、対処。モーションの終了を監視し、次のモーションを発行する機能を追加。(回転アクション中にモーションを発行しているため、モーションをつむアクションコマンドは使えなかった)
  for( u32 i = 0; i < MOTION_END_WATCH_DATA_MAX; i++ )
  {
    MOTION_END_WATCH_DATA* pData = &m_ChrMotionEndWatchData[ i ];
    if( pData->event_id == 0 )
    {
      continue;
    }

    GameSys::GameData* p_gdata = m_pGameManager->GetGameData();
    Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gdata->GetFieldCharaModelManager();
    u32 moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(  pData->event_id );

    if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX )
    {
      pData->event_id = 0;
      continue;
    }
    

    // 対象となる動作モデルを取得する
    Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( moveModelIndex );

    if( pCharaModel == NULL )
    {
      pData->event_id = 0;
      continue;
    }

    if (pCharaModel->IsAnimationStopByUnRegist())
    {
      GFL_ASSERT_MSG( 0, "UnRegistされているキャラクターのモーション終了を監視している(eventId=%d)", pCharaModel->GetEventId() );
      pData->event_id = 0;
      continue;
    }

    // @fix GFMcat[4386]:話しかけを行うと時々モーション再生を行わなくなる対処。リクエスト系のモーション変更リクエストが実行される前に監視が実行されてしまっていた。監視リクエスト発効の1フレーム後から監視を行うように修正
    if( pData->disable_count > 0 )
    {
      pData->disable_count--;
      continue;
    }

    // 動作モデルの現在のアニメーションが再生中かどうか
    if( pCharaModel->GetCharaDrawInstance()->GetAnimationId() < 0 || pCharaModel->GetCharaDrawInstance()->IsAnimationLastFrame() )
    {
      s32 endFrame = -1;
      u32 anime_id = pData->anime_id;

      // プレイヤーのモーションIDをライド状況にあったモーションIDに変換する
      if( pData->event_id == FIELD_EVENTID_PLAYER )
      {
        Field::MoveModel::FieldMoveModelPlayer* pPlayer = reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>(pCharaModel);
        if( pPlayer && pPlayer->GetOnMoveModel() )
        {
          pFieldMoveModelManager->ConvertPlayerMotionIdToRideMotionId( anime_id, pPlayer->GetOnMoveModel()->GetCharacterId() , &anime_id, &endFrame );
          //デザイナー指定のデフォルト補間フレームでライドモーション再生
          pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( anime_id );

          // 終了フレームがリソースから設定されている場合
          if( endFrame >= 0 )
          {
            pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->SetAnimationEndFrame((f32)endFrame);
          }
        }
      }

      //デザイナー指定のデフォルト補間フレームで再生
      pCharaModel->GetCharaDrawInstance()->ChangeAnimation( anime_id );

      // 終了フレームがリソースから設定されている場合
      if( endFrame >= 0 )
      {
        pCharaModel->GetCharaDrawInstance()->SetAnimationEndFrame((f32)endFrame);
      }

      // 実行したのでデータをクリア
      pData->event_id = 0;
    }
  }
}

/* ------------------------------------------------------------------------- */
/**
* @brief　キャラクターモーション終了のチェックを全てクリア
*/
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::ChrMotionEndWatchClearAll( void )
{
  for( u32 i = 0; i < MOTION_END_WATCH_DATA_MAX; i++ )
  {
    MOTION_END_WATCH_DATA* pData = &m_ChrMotionEndWatchData[ i ];
    pData->anime_id = 0;
    pData->event_id = 0;
    pData->disable_count = 0;
  }
}



/* ------------------------------------------------------------------------- */
/**
 * @brief　アクション終了後の回転リクエスト
 */
/* ------------------------------------------------------------------------- */
b32 FieldScriptSystem::ChrReqActionAfterRot(s32 event_id, u32 rot_frame, f32 target_angle, b32 is_enable_idle_init, b32 is_compress_motion, b32 is_enable_turn_motion_init, b32 is_enable_idle_wait )
{
  ChrClearActionAfterRot( event_id ); // 上書き

  FieldScriptSystem::ACTION_END_WATCH_DATA* pData = FieldScriptSystem::GetActionEndWatchEmptyWork();
  if (pData == NULL)
  {
    return false;
  }


  pData->is_taget_mode = false;
  pData->event_id = event_id;
  pData->rot_frame = rot_frame;
  pData->target_angle = target_angle;
  pData->is_enable_idle_init = is_enable_idle_init;
  pData->is_compress_motion = is_compress_motion;
  pData->is_enable_turn_motion_init = is_enable_turn_motion_init;
  pData->is_enable_idle_wait = is_enable_idle_wait;
  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief　アクション終了後の回転リクエスト
 */
/* ------------------------------------------------------------------------- */
b32 FieldScriptSystem::ChrReqActionAfterRotTarget(s32 event_id, u32 rot_frame, s32 target_event_id, b32 is_enable_idle_init, b32 is_compress_motion, b32 is_enable_turn_motion_init, b32 is_enable_idle_wait )
{
  ChrClearActionAfterRot( event_id ); // 上書き
  
  FieldScriptSystem::ACTION_END_WATCH_DATA* pData = FieldScriptSystem::GetActionEndWatchEmptyWork();
  if (pData == NULL)
  {
    return false;
  }

  pData->event_id = event_id;
  pData->rot_frame = rot_frame;
  pData->is_taget_mode = true;
  pData->target_event_id = target_event_id;
  pData->is_enable_idle_init = is_enable_idle_init;
  pData->is_compress_motion = is_compress_motion;
  pData->is_enable_turn_motion_init = is_enable_turn_motion_init;
  pData->is_enable_idle_wait = is_enable_idle_wait;
  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief　アクション終了後の回転リクエスト、無効化
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::ChrClearActionAfterRot(s32 event_id)
{
  for (u32 i = 0; i < ACTION_END_WATCH_DATA_MAX; i++)
  {
    ACTION_END_WATCH_DATA* pData = &m_ChrActionEndWatchData[i];
    if (pData->event_id == 0)
    {
      continue;
    }

    if (pData->event_id == event_id)
    {
      pData->event_id = 0;
    }
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief　アクション終了後の回転リクエスト、無効化(全部)
 */
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::ChrClearActionAfterRotAll(void)
{
  for (u32 i = 0; i < ACTION_END_WATCH_DATA_MAX; i++)
  {
    ACTION_END_WATCH_DATA* pData = &m_ChrActionEndWatchData[i];
    pData->event_id = 0;
  }
}


/* ------------------------------------------------------------------------- */
/**
* @brief　キャラクターアクション終了のチェック
*/
/* ------------------------------------------------------------------------- */
void FieldScriptSystem::CheckChrActionEnd( void )
{
  for (u32 i = 0; i < ACTION_END_WATCH_DATA_MAX; i++)
  {
    ACTION_END_WATCH_DATA* pData = &m_ChrActionEndWatchData[i];
    if (pData->event_id == 0)
    {
      continue;
    }

    GameSys::GameData* p_gdata = m_pGameManager->GetGameData();
    Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gdata->GetFieldCharaModelManager();
    u32 moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(pData->event_id);

    if (moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX)
    {
      pData->event_id = 0;
      continue;
    }

    // 対象となる動作モデルを取得する
    Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(moveModelIndex);

    if (pCharaModel == NULL)
    {
      pData->event_id = 0;
      continue;
    }

    // 終了チェック
    if(pCharaModel->IsExecuteActionCommand())
    {
      continue;
    }

    if (pData->is_taget_mode)
    {
      FuncSetCommon::ChrRotTargetImpl( pData->event_id, pData->rot_frame, pData->target_event_id, pData->is_enable_idle_init, pData->is_compress_motion, pData->is_enable_turn_motion_init, pData->is_enable_idle_wait ); 
    }
    else
    {
      pCharaModel->SetActionCommandRotation(Field::MoveModel::FIELD_ACTION_COMMAND_ROTATION, pData->target_angle, 0.0f, 0.0f, pData->rot_frame, pData->is_enable_idle_init, pData->is_compress_motion, pData->is_enable_turn_motion_init, pData->is_enable_idle_wait );
    }
  
    // 無効化
    pData->event_id = 0;
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief　キャラクターアクション終了監視用データ取得
 *
 * @return 空データがない場合はNULL

 */
/* ------------------------------------------------------------------------- */
FieldScriptSystem::ACTION_END_WATCH_DATA* FieldScriptSystem::GetActionEndWatchEmptyWork(void)
{
  for (u32 i = 0; i < ACTION_END_WATCH_DATA_MAX; i++)
  {
    ACTION_END_WATCH_DATA* pData = &m_ChrActionEndWatchData[i];
    if (pData->event_id == 0)
    {
      return pData;
    }
  }

  return NULL;
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

