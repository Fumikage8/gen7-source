//======================================================================
/**
* @file FieldEventDebugCharaLayoutEditorMode.cpp
* @brief キャラエディターの動作モード
* @author saita_kazuki
* @data 2015.10.22
*/
//======================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Event/FieldEventDebugCharaLayoutEditorMode.h"

#if defined(GF_ENABLE_DEBUG_EDITOR)
#include <locale.h> // Win32のみなので使う

// gfl2
#include <Debug/include/gfl2_DebugPrint.h>
#include <Animation/include/gfl2_AnimationId.h>

// niji
#include "Sound/include/Sound.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldFacialAnime.h"


// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugTopView.h"
#include <model/include/gfl2_BaseCamera.h>


// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinLocal.h>

// field script
#include "FieldScript/include/FieldScriptSystem.h"
#include "Field/FieldRo/include/Window/FieldTalkWindow.h"

// conv_header
#include <niji_conv_header/message/debug/msg_debug_menu.h>
#include <niji_conv_header/field/chara/p2_base_fi.h>
#include <niji_conv_header/message/debug/msg_debug_field_chara_motion.h>
#include <niji_conv_header/message/debug/msg_debug_field_chara_dynamic_motion.h>


GFL_NAMESPACE_BEGIN( Field )


//------------------------------------------------------------------------------
// DebugCharaLayoutEditorCommonWork
//------------------------------------------------------------------------------

/**
 * @brief コンストラクタ
 */ 
DebugCharaLayoutEditorCommonWork::DebugCharaLayoutEditorCommonWork()
  : m_editCharaNum( 0)
  , m_pEditCharaList( NULL)
  , m_selectCharaIndex( 0)
  , m_pCharaWork( NULL)
{
}

/**
 * @brief デストラクタ
 */ 
DebugCharaLayoutEditorCommonWork::~DebugCharaLayoutEditorCommonWork()
{
}

//------------------------------------------------------------------------------
// DebugMotionMessageManager
//------------------------------------------------------------------------------

/**
 * @brief コンストラクタ
 */ 
DebugMotionMessageManager::DebugMotionMessageManager(
  gfl2::heap::HeapBase* pHeap,
  gfl2::str::MsgData* pMsgDataMotion,
  gfl2::str::MsgData* pMsgDataDynamicMotion,
  void*               pDynamicMotionMsgIDData
  )
  : m_pHeap( pHeap)
  , m_pMsgDataMotion( pMsgDataMotion) 
  , m_pMsgDataDynamicMotion( pMsgDataDynamicMotion) 
  , m_pDynamicMotionMsgIDData( reinterpret_cast<DynamicMotionMsgIDData*>( pDynamicMotionMsgIDData))
{
  m_pStrBuf = GFL_NEW( m_pHeap) gfl2::str::StrBuf( 64, m_pHeap);
}

/**
 * @brief デストラクタ
 */ 
DebugMotionMessageManager::~DebugMotionMessageManager()
{
  GFL_SAFE_DELETE( m_pStrBuf);
}

/**
 * @brief 常駐モーションの文字列取得
 */ 
const gfl2::str::STRCODE* DebugMotionMessageManager::GetMotionString( u32 motionID)
{
  m_pStrBuf->Clear();

  m_pMsgDataMotion->GetString( motionID, *m_pStrBuf);
  return m_pStrBuf->GetPtr();
}

/**
 * @brief 非常駐モーションの文字列取得
 */
const gfl2::str::STRCODE* DebugMotionMessageManager::GetDynamicMotionString( u32 packID, u32 motionID)
{
  m_pStrBuf->Clear();

  u32 startStrID = this->GetDynamicMotionStartMsgID( packID);

  if( startStrID != INVALID_START_MSG_ID)
  {
    m_pMsgDataDynamicMotion->GetString( startStrID + motionID, *m_pStrBuf);
    return m_pStrBuf->GetPtr();
  }
  return L"Not Found Motion Name\n";
}

/**
 * @brief 目モーションの文字列取得
 */ 
const gfl2::str::STRCODE* DebugMotionMessageManager::GetEyeMotionString( u32 motionID)
{
  m_pStrBuf->Clear();

  m_pMsgDataMotion->GetString( msg_dfc_motion_eye_00 + motionID, *m_pStrBuf);
  return m_pStrBuf->GetPtr();
}

/**
 * @brief 口モーションの文字列取得
 */ 
const gfl2::str::STRCODE* DebugMotionMessageManager::GetMouthMotionString( u32 motionID)
{
  m_pStrBuf->Clear();

  m_pMsgDataMotion->GetString( msg_dfc_motion_mouth_00 + motionID, *m_pStrBuf);
  return m_pStrBuf->GetPtr();
}

/**
 * @brief 非常駐モーションメッセージの開始番号取得
 */
u32 DebugMotionMessageManager::GetDynamicMotionStartMsgID( u32 packID) const
{
  for( u32 i = 0; i < m_pDynamicMotionMsgIDData->num; ++i)
  {
    if( packID == m_pDynamicMotionMsgIDData->core[i].packID)
    {
      return m_pDynamicMotionMsgIDData->core[i].msgIDOffset;
    }
  }
  return INVALID_START_MSG_ID;
}

//------------------------------------------------------------------------------
// DebugCharaLayoutEditorModeManager
//------------------------------------------------------------------------------

/**
 * @brief コンストラクタ
 */ 
DebugCharaLayoutEditorModeManager::DebugCharaLayoutEditorModeManager()
  : m_pHeap( NULL)
  , m_pDebugGroup( NULL)
  , m_pMsgData( NULL)
  , m_pFieldmap( NULL)
  , m_state( STATE_NONE)
  , m_pCurrentMode( NULL)
  , m_commonWork()
{
}

/**
 * @brief デストラクタ
 */ 
DebugCharaLayoutEditorModeManager::~DebugCharaLayoutEditorModeManager()
{
}

/**
 * @brief 初期化
 * @param desc 初期化設定
 */ 
void DebugCharaLayoutEditorModeManager::Initialize( const InitializeDescription& desc)
{
  m_pHeap         = desc.pHeap;
  m_pMsgData      = desc.pMsgData;
  m_pFieldmap     = desc.pFieldmap;
  m_pScriptSystem = desc.pScriptSystem;

  // 共通ワーク初期化
  m_commonWork.m_pEditCharaList = m_pScriptSystem->DebugGetChrEditList( &m_commonWork.m_editCharaNum);
  m_commonWork.m_pCharaWork = GFL_NEW_ARRAY( m_pHeap) CharaWork[ m_commonWork.m_editCharaNum ];
  for( s32 i = 0; i < m_commonWork.m_editCharaNum; ++i)
  {
    u32 moveModelIndex = m_pFieldmap->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( m_commonWork.m_pEditCharaList[ i ].event_id);
    MoveModel::FieldMoveModel* pMoveModel = m_pFieldmap->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);
    poke_3d::model::CharaModel* pCharaModel = pMoveModel->GetCharaDrawInstance();

    m_commonWork.m_pCharaWork[i].pManager    = this;
    m_commonWork.m_pCharaWork[i].pSelf       = NULL;
    m_commonWork.m_pCharaWork[i].index       = i;
    m_commonWork.m_pCharaWork[i].walkStep    = 0;
    m_commonWork.m_pCharaWork[i].runStep     = 0;
    m_commonWork.m_pCharaWork[i].defaultPos  = pCharaModel->GetPosition();
    m_commonWork.m_pCharaWork[i].defaultRot  = pCharaModel->GetRotationQuat();

    m_commonWork.m_pCharaWork[i].eyeIndex    = pCharaModel->GetEyeIndex();
    m_commonWork.m_pCharaWork[i].mouthIndex  = pCharaModel->GetMouthIndex();

    //m_commonWork.m_pCharaWork[i].defaultEyeIndex    = pCharaModel->GetEyeIndex();
    //m_commonWork.m_pCharaWork[i].defaultMouthIndex  = pCharaModel->GetMouthIndex();

    m_commonWork.m_pCharaWork[i].dynamicMotionCount = 0;
    m_commonWork.m_pCharaWork[i].dynamicMotionStartIndex = 0;
    m_commonWork.m_pCharaWork[i].dynamicMotionEndIndex = 0;
    m_commonWork.m_pCharaWork[i].dynamicMotionIndex = 0;
 
    s32 dynamicMotionPackID = m_commonWork.m_pEditCharaList[ i ].dynamic_mot_id;
    if( dynamicMotionPackID != -1)
    {
      if( m_pFieldmap->GetMoveModelManager()->IsDynamicAnimationLoaded( dynamicMotionPackID) )
      {
        // 非常駐モーション関連初期化
        gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId( m_commonWork.m_pEditCharaList[ i ].dynamic_mot_id);
        const u32 DYNAMIC_MOTION_OFFSET = 0x00010000;
        u32 motionCount = pCharaModel->GetAnimationPackResouceCount( packedDynamicAnimationId.packId);
        u32 startIndex = DYNAMIC_MOTION_OFFSET * ( packedDynamicAnimationId.packId );
        m_commonWork.m_pCharaWork[i].dynamicMotionCount = motionCount;
        m_commonWork.m_pCharaWork[i].dynamicMotionStartIndex = startIndex;
        m_commonWork.m_pCharaWork[i].dynamicMotionEndIndex = startIndex + motionCount - 1;
        m_commonWork.m_pCharaWork[i].dynamicMotionIndex = startIndex;
   
        if( pMoveModel->GetCharacterId() != packedDynamicAnimationId.charaId)
        {
          GFL_PRINT( "CharacterID [%d] : MotionPackCharacterID [%d] : does not match.", pMoveModel->GetCharacterId(), packedDynamicAnimationId.charaId);
          GFL_ASSERT( 0);
          return;
        }
      }
    }
  }

  // 動作モード生成
  m_pModeTable[ MODE_DISP_MODEL_LIST ]        = GFL_NEW( m_pHeap) DebugCharaLayoutEditorModeDispModelList();
  m_pModeTable[ MODE_PLACEMENT_EDIT ]         = GFL_NEW( m_pHeap) DebugCharaLayoutEditorModePlacementEdit();
  m_pModeTable[ MODE_PLAY_MOTION ]            = GFL_NEW( m_pHeap) DebugCharaLayoutEditorModePlayMotion();
  m_pModeTable[ MODE_PLAY_DYNAMIC_MOTION ]    = GFL_NEW( m_pHeap) DebugCharaLayoutEditorModePlayDynamicMotion();

  // 動作モード初期化
  DebugCharaLayoutEditorModeBase::InitializeDescription initDesc;
  initDesc.pHeap        = m_pHeap;
  initDesc.pMsgData     = m_pMsgData;
  initDesc.pFieldmap    = m_pFieldmap;
  initDesc.pManager     = this;
  for( u32 i = 0; i < MODE_MAX; ++i)
  {
    m_pModeTable[i]->Initialize( initDesc);
  }

  m_pMotionMessageManager = GFL_NEW( m_pHeap) DebugMotionMessageManager(
    m_pHeap,
    desc.pMsgDataMotion,
    desc.pMsgDataDynamicMotion,
    desc.pDynamicMotionMsgIDData
    );
}

/**
 * @brief 破棄
 */ 
void DebugCharaLayoutEditorModeManager::Terminate()
{
  GFL_SAFE_DELETE( m_pMotionMessageManager);
  for( u32 i = 0; i < MODE_MAX; ++i)
  {
    if( m_pModeTable[i])
    {
      m_pModeTable[i]->Terminate();
    }
    GFL_SAFE_DELETE( m_pModeTable[i]);
  }
}

/**
 * @brief 更新
 * @return 更新後の状態
 */ 
DebugCharaLayoutEditorModeManager::State DebugCharaLayoutEditorModeManager::Update()
{
  DebugCharaLayoutEditorModeBase::State state;
  state = m_pCurrentMode->Update( this);

  // モードが終了チェック
  if( state == DebugCharaLayoutEditorModeBase::STATE_END)
  {
    m_pCurrentMode->Clean();
    return STATE_END;
  }

  return STATE_CONTINUE;
}

/**
 * @brief セットアップ
 */ 
void DebugCharaLayoutEditorModeManager::Setup( Mode mode, gfl2::debug::DebugWinGroup* pDebugGroup)
{
  m_pDebugGroup = pDebugGroup;

  m_pCurrentMode = m_pModeTable[ mode ];
  m_pCurrentMode->Setup( m_pDebugGroup);
}

/**
 * @brief モード取得
 * @param mode モード
 * @return 指定したモード
 */ 
DebugCharaLayoutEditorModeBase* DebugCharaLayoutEditorModeManager::GetMode( Mode mode) const
{
  return m_pModeTable[ mode ];
}

/**
 * @brief 共通ワーク取得
 * @return 共通ワーク(書き換え可)
 */ 
DebugCharaLayoutEditorCommonWork* DebugCharaLayoutEditorModeManager::GetCommonWork()
{
  return &m_commonWork;
}

//------------------------------------------------------------------------------
// DebugCharaLayoutEditorModeBase
//------------------------------------------------------------------------------

/**
 * @brief コンストラクタ
 */ 
DebugCharaLayoutEditorModeBase::DebugCharaLayoutEditorModeBase()
  : m_pHeap( NULL)
  , m_pDebugGroup( NULL)
  , m_pMsgData( NULL)
  , m_pFieldmap( NULL)
  , m_pManager( NULL)
  , m_isUpdate( false)
{
}

/**
 * @brief デストラクタ
 */ 
DebugCharaLayoutEditorModeBase::~DebugCharaLayoutEditorModeBase()
{
}

/**
 * @brief 初期化
 * @param desc 初期化設定
 */ 
void DebugCharaLayoutEditorModeBase::Initialize( const InitializeDescription& desc)
{
  m_pHeap         = desc.pHeap;
  m_pDebugGroup   = desc.pDebugGroup;
  m_pMsgData      = desc.pMsgData;
  m_pFieldmap     = desc.pFieldmap;
  m_pManager      = desc.pManager;

  this->initialize();
}


//------------------------------------------------------------------------------
// DebugCharaLayoutEditorModeDispModelList
//------------------------------------------------------------------------------

void UpdateFunc_DispModelList( void* userWork, gfl2::debug::DebugWinItem* item )
{
  CharaWork* pWork = (CharaWork*)userWork;

  DebugCharaLayoutEditorCommonWork* pCommonWork = pWork->pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return ;
  }

  u32 moveModelIndex = pWork->pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pWork->index ].event_id);
  MoveModel::FieldMoveModel* pMoveModel = pWork->pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pCommonWork->m_selectCharaIndex = pWork->index;
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) || pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    pMoveModel->SetVisible( !pMoveModel->GetVisible());
  }
}

wchar_t* DrawFunc_DispModelList( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  CharaWork* pWork = (CharaWork*)userWork;

  DebugCharaLayoutEditorCommonWork* pCommonWork = pWork->pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  u32 moveModelIndex = pWork->pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pWork->index ].event_id);
  MoveModel::FieldMoveModel* pMoveModel = pWork->pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);

  bool isVisible = pMoveModel->GetVisible();
  bool isSelectChara = (pCommonWork->m_selectCharaIndex == pWork->index);
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%s]%s", item->GetName(), isVisible ? L"ON" : L"OFF", isSelectChara ? L" <-": L"" );

  return workStr;
}

/**
 * @brief コンストラクタ
 */ 
DebugCharaLayoutEditorModeDispModelList::DebugCharaLayoutEditorModeDispModelList()
{
}

/**
 * @brief デストラクタ
 */ 
DebugCharaLayoutEditorModeDispModelList::~DebugCharaLayoutEditorModeDispModelList()
{
}

/**
 * @brief 破棄
 */ 
void DebugCharaLayoutEditorModeDispModelList::Terminate()
{
}

/**
 * @brief 更新
 * @return 更新後のモードの状態
 */ 
DebugCharaLayoutEditorModeBase::State DebugCharaLayoutEditorModeDispModelList::Update( DebugCharaLayoutEditorModeManager* pManager)
{
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) && gfl2::debug::DebugWin_IsOpen() )
  {
    return STATE_END;
  }

  if( m_isUpdate == false)
  {
    return STATE_CONTINUE;
  }

  return STATE_CONTINUE;
}

/**
 * @brief セットアップ
 */ 
void DebugCharaLayoutEditorModeDispModelList::Setup( gfl2::debug::DebugWinGroup* pDebugGroup)
{
  m_pDebugGroup = pDebugGroup;

  DebugCharaLayoutEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  for( s32 i = 0; i < pCommonWork->m_editCharaNum; ++i)
  {
    wchar_t name[ Field::FieldScript::DEBUG_CHR_EDIT_NAME_LEN ];
    mbstowcs( name, pCommonWork->m_pEditCharaList[ i ].name, Field::FieldScript::DEBUG_CHR_EDIT_NAME_LEN);
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, name, reinterpret_cast<void*>( &pCommonWork->m_pCharaWork[i]), UpdateFunc_DispModelList, DrawFunc_DispModelList );

    pCommonWork->m_pCharaWork[i].pSelf = this;
  }

  m_isUpdate = true;
}

/**
 * @brief クリーン
 */ 
void DebugCharaLayoutEditorModeDispModelList::Clean()
{
  DebugCharaLayoutEditorCommonWork* pCommonWork =m_pManager->GetCommonWork();

  m_isUpdate = false;

  for( s32 i = 0; i < pCommonWork->m_editCharaNum; ++i)
  {
    pCommonWork->m_pCharaWork[i].pSelf = NULL;
  }

  m_pDebugGroup->RemoveAllItem();
}

/**
 * @brief 初期化
 */ 
void DebugCharaLayoutEditorModeDispModelList::initialize()
{
}

//------------------------------------------------------------------------------
// DebugCharaLayoutEditorModePlacementEdit
//------------------------------------------------------------------------------

f32 CalcQuaternionToRadian( const gfl2::math::Quaternion& qt)
{
  gfl2::math::Vector3 v1( 0.0f, 0.0f, 1.0f);
  gfl2::math::Matrix34 mtx;
  qt.QuaternionToMatrix( mtx);
  v1 = (mtx * v1).Normalize();

  f32 radianY = 0.0f;
  if( gfl2::math::IsAlmostZero( v1.z) )
  {
    radianY = ( v1.x > 0.0f ) ? (PI/2.0f) : -(PI/2.0f);
  }
  else
  {
    if( gfl2::math::IsAlmostZero( v1.x) )
    {
      radianY = ( v1.z > 0.0f ) ? 0.0f : PI;
    }
    else
    {
      radianY = atan( v1.x / v1.z );
      if( v1.z < 0.0f )
      {
        radianY += ( v1.x > 0.0f ) ? PI : -PI;
      }
    }
  }
  return radianY;
}

wchar_t* DrawFunc_PlacementEdit_Name( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugCharaLayoutEditorModePlacementEdit* pSelf = (DebugCharaLayoutEditorModePlacementEdit*)userWork;
  DebugCharaLayoutEditorCommonWork* pCommonWork = pSelf->GetModeManager()->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%S]", item->GetName(), pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].name );

  return workStr;
}

wchar_t* DrawFunc_PlacementEdit_Pos( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugCharaLayoutEditorModePlacementEdit* pSelf = (DebugCharaLayoutEditorModePlacementEdit*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pSelf->GetModeManager();
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  u32 moveModelIndex = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  MoveModel::FieldMoveModel* pMoveModel = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);
  gfl2::math::Vector3 pos = pMoveModel->GetCharaDrawInstance()->GetPosition();

  gfl2::math::Vector3 printPos = pManager->GetScriptSystem()->CalcEventPositionOffset( pos);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %.1f, %.1f, %.1f ]", item->GetName(), printPos.x, printPos.y, printPos.z );

  return workStr;
}

wchar_t* DrawFunc_PlacementEdit_Rot( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugCharaLayoutEditorModePlacementEdit* pSelf = (DebugCharaLayoutEditorModePlacementEdit*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pSelf->GetModeManager();
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  u32 moveModelIndex = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  MoveModel::FieldMoveModel* pMoveModel = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);
  gfl2::math::Quaternion rot = pMoveModel->GetCharaDrawInstance()->GetRotationQuat();

  f32 radianY = CalcQuaternionToRadian( rot);
  f32 degY = gfl2::math::ConvRadToDeg( radianY );

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %.1f ]", item->GetName(), degY );

  return workStr;
}

wchar_t* DrawFunc_PlayMotionEye( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  CharaWork* pWork = (CharaWork*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->pManager;
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  u32 eyeIndex = pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].eyeIndex;
  const wchar_t* strwEye = pManager->GetMotionMessageManager()->GetEyeMotionString( eyeIndex);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %s ]", item->GetName() , strwEye );

  return workStr;
}

wchar_t* DrawFunc_PlayMotionMouth( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  CharaWork* pWork = (CharaWork*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->pManager;
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  u32 mouthIndex = pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].mouthIndex;
  const wchar_t* strwMouth = pManager->GetMotionMessageManager()->GetMouthMotionString( mouthIndex);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %s ]", item->GetName() , strwMouth );

  return workStr;
}



/**
 * @brief コンストラクタ
 */ 
DebugCharaLayoutEditorModePlacementEdit::DebugCharaLayoutEditorModePlacementEdit()
  : m_pControlMoveModel( NULL)
  , m_addPos( 1.0f)
  , m_addRot( 1.0f)
  , m_isTopViewMode( false)
  , m_pSwapCameraController( NULL)
  , m_pTopViewCameraController( NULL)
{
  gfl2::std::MemClear( m_isTalkWindowVisibleDefault, sizeof( m_isTalkWindowVisibleDefault));
}

/**
 * @brief デストラクタ
 */ 
DebugCharaLayoutEditorModePlacementEdit::~DebugCharaLayoutEditorModePlacementEdit()
{
}

/**
 * @brief 破棄
 */ 
void DebugCharaLayoutEditorModePlacementEdit::Terminate()
{
}

/**
 * @brief 更新
 * @return 更新後のモードの状態
 */ 
DebugCharaLayoutEditorModeBase::State DebugCharaLayoutEditorModePlacementEdit::Update( DebugCharaLayoutEditorModeManager* pManager)
{
  DebugCharaLayoutEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  FieldScript::FieldScriptSystem* pScriptSystem = m_pManager->GetScriptSystem();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) && gfl2::debug::DebugWin_IsOpen() )
  {
    return STATE_END;
  }

  if( m_isUpdate == false)
  {
    return STATE_CONTINUE;
  }

  poke_3d::model::BaseModel* pModel = m_pControlMoveModel->GetCharaDrawInstance();

  // 移動速度加算
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ) )
    {
      m_addPos *= 10.0f;
      if( m_addPos >= 100.0f)
      {
        m_addPos = 100.0f;
      }
      m_addRot *= 10.0f;
      if( m_addRot >= 100.0f)
      {
        m_addRot = 100.0f;
      }
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) )
    {
      m_addPos /= 10.0f;
      if( m_addPos <= 1.0f)
      {
        m_addPos = 1.0f;
      }
      m_addRot /= 10.0f;
      if( m_addRot <= 1.0f)
      {
        m_addRot = 1.0f;
      }
    }
  }
  // 移動
  else
  {
    Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
    const Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Field::Camera::UnitNo::EVENT);
    const poke_3d::model::BaseCamera* pCamera = pCameraUnit->GetBaseCamera();
    gfl2::math::Vector3 camPos = pCamera->GetPosition();
    gfl2::math::Vector3 camTarget = pCamera->GetTargetPosition();
    gfl2::math::Vector3 camVec;
    if( m_isTopViewMode)
    {
      camVec = gfl2::math::Vector3( 0.0f, 0.0f, 1.0f);
    }
    else
    {
      camVec = (camTarget - camPos).Normalize();
      camVec.y = 0.0f;
    }

    gfl2::math::Vector3 addPos = gfl2::math::Vector3::GetZero();
    if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
    {
      addPos += camVec * m_addPos;
    }
    if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
    {
      addPos += -camVec * m_addPos;
    }
    if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    {
      gfl2::math::Quaternion qt( gfl2::math::Vector3( 0.0f, 1.0f, 0.0f), gfl2::math::ConvDegToRad( 90.0f));
      gfl2::math::Matrix34 rotMat;
      qt.QuaternionToMatrix( rotMat );
      addPos += rotMat.TransformNormal( camVec) * m_addPos;
    }
    if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      gfl2::math::Quaternion qt( gfl2::math::Vector3( 0.0f, 1.0f, 0.0f), gfl2::math::ConvDegToRad( -90.0f));
      gfl2::math::Matrix34 rotMat;
      qt.QuaternionToMatrix( rotMat );
      addPos += rotMat.TransformNormal( camVec) * m_addPos;
    }

    if( m_pControlMoveModel->IsForceLanding() == false)
    {
      if( pVecDevice->GetY() >= 0.5f)
      {
        addPos.y += m_addPos;
      }
      else if( pVecDevice->GetY() <= -0.5f)
      {
        addPos.y -= m_addPos;
      }
    }

    pModel->SetPosition( pModel->GetPosition() + addPos);
  }



  // 回転
  gfl2::math::Quaternion qt ( pModel->GetRotationQuat());
  if( pButton->IsRepeat( gfl2::ui::BUTTON_ZL ) )
  {
    f32 radianY = CalcQuaternionToRadian( qt);
    radianY = gfl2::math::NormalizeRadianAngle( radianY + gfl2::math::ConvDegToRad( -m_addRot) );
    gfl2::math::Quaternion rot( gfl2::math::Vector3( 0.0f, 1.0f, 0.0f), radianY);
    pModel->SetRotationQuat( rot);
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_ZR ) )
  {
    f32 radianY = CalcQuaternionToRadian( qt);
    radianY = gfl2::math::NormalizeRadianAngle( radianY + gfl2::math::ConvDegToRad( m_addRot) );
    gfl2::math::Quaternion rot( gfl2::math::Vector3( 0.0f, 1.0f, 0.0f), radianY);
    pModel->SetRotationQuat( rot);
  }


  // 会話ウインドウ表示ON/OFF
  FieldScript::SystemSingletones* pSystemSingletones = pScriptSystem->GetSystemWork()->GetSingletones();
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    for( u32 i = 0; i < FIELDTALKWINDOW_MAX; ++i)
    {
      if( pSystemSingletones->m_pFieldTalkWindow[i] && m_isTalkWindowVisibleDefault[i])
      {
        if( pSystemSingletones->m_pFieldTalkWindow[i]->IsVisible())
        {
          pSystemSingletones->m_pFieldTalkWindow[i]->SetVisible( false);
        }
      }
    }
  }
  else
  {
    for( u32 i = 0; i < FIELDTALKWINDOW_MAX; ++i)
    {
      if( pSystemSingletones->m_pFieldTalkWindow[i] && m_isTalkWindowVisibleDefault[i])
      {
        if( pSystemSingletones->m_pFieldTalkWindow[i]->IsVisible() == false)
        {
          pSystemSingletones->m_pFieldTalkWindow[i]->SetVisible( true);
        }
      }
    }
  }


  // 動作モデルの座標と回転を現在の位置に Set (更新) -> 現在の動作モデルの情報のみコマンドを出力
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    // Apply
    pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultPos = pModel->GetPosition();
    pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultRot = pModel->GetRotationQuat();

    GFL_PRINT( "Command Output Start\n");
    GFL_PRINT( "------------------------------\n");
    this->Print( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].name,
      pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultPos,
      pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultRot
      );
    GFL_PRINT( "------------------------------\n");
    GFL_PRINT( "Command Output End\n");
  }


  // 全登録モデルのデバッグログ出力
  if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
  {
    GFL_PRINT( "Command Output Start\n");
    GFL_PRINT( "------------------------------\n");
    for( s32 i = 0; i < pCommonWork->m_editCharaNum; ++i)
    {
      this->Print( pCommonWork->m_pEditCharaList[ i ].name,
        pCommonWork->m_pCharaWork[ i ].defaultPos,
        pCommonWork->m_pCharaWork[ i ].defaultRot
        );
      GFL_PRINT( "------------------------------\n");
    }
    GFL_PRINT( "Command Output End\n");
  }


  // トップビュー切り替え
  if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
    Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);
    poke_3d::model::BaseCamera* pCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera());

    if( m_pSwapCameraController)
    {
      pCameraUnit->SwapController( m_pSwapCameraController);
      pCameraUnit->SetUpdateFlag( false);
      m_pSwapCameraController = NULL;
      m_isTopViewMode = false;

      // カメラパラメーターを元に戻す
      pCamera->SetPosition( m_cameraPos);
      pCamera->SetTargetPosition( m_cameraTarget);
      pCamera->SetRotationQuat( m_cameraRot);
    }
    else
    {
      m_pSwapCameraController = pCameraUnit->SwapController( m_pTopViewCameraController);
      pCameraUnit->SetUpdateFlag( true);
      m_isTopViewMode = true;
    }
  }

  return STATE_CONTINUE;
}

/**
 * @brief セットアップ
 */ 
void DebugCharaLayoutEditorModePlacementEdit::Setup( gfl2::debug::DebugWinGroup* pDebugGroup)
{
  m_pDebugGroup = pDebugGroup;

  DebugCharaLayoutEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"*Character", reinterpret_cast<void*>( this), NULL, DrawFunc_PlacementEdit_Name )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"*Pos", reinterpret_cast<void*>( this), NULL, DrawFunc_PlacementEdit_Pos )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"*Rot", reinterpret_cast<void*>( this), NULL, DrawFunc_PlacementEdit_Rot )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"*Motion", reinterpret_cast<void*>( this), NULL, NULL )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"*Eye", reinterpret_cast<void*>( &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ]), NULL, DrawFunc_PlayMotionEye )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"*Mouth", reinterpret_cast<void*>( &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ]), NULL, DrawFunc_PlayMotionMouth )->SetIsSelectSkip( true);

  pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].pSelf = this;

  u32 moveModelIndex = m_pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  m_pControlMoveModel = m_pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);


  // 会話ウインドウの元の表示状態を保持しておく
  FieldScript::FieldScriptSystem* pScriptSystem = m_pManager->GetScriptSystem();
  FieldScript::SystemSingletones* pSystemSingletones = pScriptSystem->GetSystemWork()->GetSingletones();
  for( u32 i = 0; i < FIELDTALKWINDOW_MAX; ++i)
  {
    if( pSystemSingletones->m_pFieldTalkWindow[i])
    {
      m_isTalkWindowVisibleDefault[i] = pSystemSingletones->m_pFieldTalkWindow[i]->IsVisible();
    }
  }

  // トップビュー用カメラコントローラーを生成
  m_pTopViewCameraController = GFL_NEW( m_pHeap )Camera::Controller::ControllerTypeDebugTopView();
  m_pTopViewCameraController->SetTargetObject( m_pControlMoveModel);
  m_pTopViewCameraController->SetDistance( 1000.0f);

  // 現在のカメラパラメーターを保持しておく
  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);
  const poke_3d::model::BaseCamera* pCamera = pCameraUnit->GetBaseCamera();
  m_cameraPos = pCamera->GetPosition();
  m_cameraTarget = pCamera->GetTargetPosition();
  m_cameraRot = pCamera->GetRotationQuat();

  m_isTopViewMode = false;
  m_isUpdate = true;
}

/**
 * @brief クリーン
 */ 
void DebugCharaLayoutEditorModePlacementEdit::Clean()
{
  DebugCharaLayoutEditorCommonWork* pCommonWork =m_pManager->GetCommonWork();

  m_isUpdate = false;
  m_isTopViewMode = false;

  // トップビューに切り替えていたらカメラを戻す
  if( m_pSwapCameraController)
  {
    Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
    Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);
    poke_3d::model::BaseCamera* pCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera());

    pCameraUnit->SwapController( m_pSwapCameraController);
    pCameraUnit->SetUpdateFlag( false);
    m_pSwapCameraController = NULL;

    // カメラパラメーターを元に戻す
    pCamera->SetPosition( m_cameraPos);
    pCamera->SetTargetPosition( m_cameraTarget);
    pCamera->SetRotationQuat( m_cameraRot);
  }

  // トップビュー用カメラコントローラーを破棄
  GFL_SAFE_DELETE( m_pTopViewCameraController);

  // 会話ウインドウの元の表示状態を元に戻す
  FieldScript::FieldScriptSystem* pScriptSystem = m_pManager->GetScriptSystem();
  FieldScript::SystemSingletones* pSystemSingletones = pScriptSystem->GetSystemWork()->GetSingletones();
  for( u32 i = 0; i < FIELDTALKWINDOW_MAX; ++i)
  {
    if( pSystemSingletones->m_pFieldTalkWindow[i])
    {
      pSystemSingletones->m_pFieldTalkWindow[i]->SetVisible( m_isTalkWindowVisibleDefault[i]);
    }
  }

  m_pControlMoveModel->GetCharaDrawInstance()->SetPosition( pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultPos);
  m_pControlMoveModel->GetCharaDrawInstance()->SetRotationQuat( pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultRot);
  m_pControlMoveModel = NULL;

  pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].pSelf = NULL;

  m_pDebugGroup->RemoveAllItem();
}

/**
 * @brief 初期化
 */ 
void DebugCharaLayoutEditorModePlacementEdit::initialize()
{
}

/**
 * @brief ログ出力
 */
void DebugCharaLayoutEditorModePlacementEdit::Print( const char* name, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rot)
{
  gfl2::math::Vector3 printPos = m_pManager->GetScriptSystem()->CalcEventPositionOffset( pos);

  f32 radianY = CalcQuaternionToRadian( rot);
  f32 degY = gfl2::math::ConvRadToDeg( radianY );

  GFL_PRINT( "ChrDisp( %s, true );\n", name );
  GFL_PRINT( "ChrSetPosAng( %s, %.1f, %.1f, %.1f );\n",
    name,
    printPos.x,
    printPos.z,
    degY
    );
}


//------------------------------------------------------------------------------
// DebugCharaLayoutEditorModePlayMotion
//------------------------------------------------------------------------------

void UpdateFunc_PlayMotion_Walk( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugCharaLayoutEditorModePlayMotion* pWork = (DebugCharaLayoutEditorModePlayMotion*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->GetModeManager();
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();
  CharaWork* pCharaWork = &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ];

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return ;
  }

  u32 moveModelIndex = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  MoveModel::FieldMoveModel* pMoveModel = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pMoveModel->SetActionCommandStepMove( (MoveModel::FieldActionCommandId)(MoveModel::FIELD_ACTION_COMMAND_STEP_MOVE), true, pCharaWork->walkStep, true, 1.0f );
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ))
  {
    pCharaWork->walkStep--;
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ))
  {
    pCharaWork->walkStep++;
  }
}

wchar_t* DrawFunc_PlayMotion_Walk( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugCharaLayoutEditorModePlayMotion* pWork = (DebugCharaLayoutEditorModePlayMotion*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->GetModeManager();
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();
  CharaWork* pCharaWork = &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ];

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pCharaWork->walkStep );

  return workStr;
}

void UpdateFunc_PlayMotion_Run( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugCharaLayoutEditorModePlayMotion* pWork = (DebugCharaLayoutEditorModePlayMotion*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->GetModeManager();
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();
  CharaWork* pCharaWork = &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ];

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return ;
  }

  u32 moveModelIndex = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  MoveModel::FieldMoveModel* pMoveModel = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pMoveModel->SetActionCommandStepMove( (MoveModel::FieldActionCommandId)(MoveModel::FIELD_ACTION_COMMAND_STEP_MOVE), false, pCharaWork->runStep, true, 1.0f );
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ))
  {
    pCharaWork->runStep--;
  }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ))
  {
    pCharaWork->runStep++;
  }
}

wchar_t* DrawFunc_PlayMotion_Run( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugCharaLayoutEditorModePlayMotion* pWork = (DebugCharaLayoutEditorModePlayMotion*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->GetModeManager();
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();
  CharaWork* pCharaWork = &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ];

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), pCharaWork->runStep );

  return workStr;
}

void UpdateFunc_PlayMotion( void* userWork, gfl2::debug::DebugWinItem* item )
{
  CharaWork* pWork = (CharaWork*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->pManager;
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return;
  }

  u32 moveModelIndex = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  MoveModel::FieldMoveModel* pMoveModel = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  u32 add = 1;
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    add *= 10;
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    //モーションを補間ありで再生
    pMoveModel->GetCharaDrawInstance()->ChangeAnimationSmooth( pWork->motionIndex, 0);
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pWork->motionIndex += add;
    if( pWork->motionIndex >= 270)
    {
      pWork->motionIndex -= 270;
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    s32 tmp = static_cast<s32>( pWork->motionIndex);
    tmp -= add;
    if( tmp < 0)
    {
      tmp += 270;
    }
    pWork->motionIndex = static_cast<u32>( tmp);
  }
}

wchar_t* DrawFunc_PlayMotion( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  CharaWork* pWork = (CharaWork*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->pManager;
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  const wchar_t* strw = pManager->GetMotionMessageManager()->GetMotionString( pWork->motionIndex);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %s ]", item->GetName() , strw );

  return workStr;
}

void UpdateFunc_PlayMotionEye( void* userWork, gfl2::debug::DebugWinItem* item )
{
  CharaWork* pWork = (CharaWork*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->pManager;
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return;
  }

  u32 moveModelIndex = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  MoveModel::FieldMoveModel* pMoveModel = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = pMoveModel->GetFacialAnimeEye();
    pAnimeEye->ChangeAnime( static_cast<poke_3d::model::CharaModel::EyeIndex>( pWork->eyeIndex) );
  }
  u32 eyeIndexMax = poke_3d::model::CharaModel::EYE_ANIMATION_SLOT + 1;
  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pWork->eyeIndex++;
    if( pWork->eyeIndex >= eyeIndexMax)
    {
      pWork->eyeIndex = 0;
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( pWork->eyeIndex <= 0)
    {
      pWork->eyeIndex = eyeIndexMax - 1;
    }
    else
    {
      pWork->eyeIndex--;
    }
  }
}

void UpdateFunc_PlayMotionMouth( void* userWork, gfl2::debug::DebugWinItem* item )
{
  CharaWork* pWork = (CharaWork*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->pManager;
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return;
  }

  u32 moveModelIndex = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  MoveModel::FieldMoveModel* pMoveModel = pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = pMoveModel->GetFacialAnimeMouth();
    pAnimeMouth->ChangeAnime( static_cast<poke_3d::model::CharaModel::MouthIndex>( pWork->mouthIndex) );
  }
  u32 mouthIndexMax = poke_3d::model::CharaModel::MOUTH_ANIMATION_SLOT + 1;
  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pWork->mouthIndex++;
    if( pWork->mouthIndex >= mouthIndexMax)
    {
      pWork->mouthIndex = 0;
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( pWork->mouthIndex <= 0)
    {
      pWork->mouthIndex = mouthIndexMax - 1;
    }
    else
    {
      pWork->mouthIndex--;
    }
  }
}




/**
 * @brief コンストラクタ
 */ 
DebugCharaLayoutEditorModePlayMotion::DebugCharaLayoutEditorModePlayMotion()
{
}

/**
 * @brief デストラクタ
 */ 
DebugCharaLayoutEditorModePlayMotion::~DebugCharaLayoutEditorModePlayMotion()
{
}

/**
 * @brief 破棄
 */ 
void DebugCharaLayoutEditorModePlayMotion::Terminate()
{
}

/**
 * @brief 更新
 * @return 管理クラスの状態
 */ 
DebugCharaLayoutEditorModeBase::State DebugCharaLayoutEditorModePlayMotion::Update( DebugCharaLayoutEditorModeManager* pManager)
{
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) && gfl2::debug::DebugWin_IsOpen() )
  {
    return STATE_END;
  }

  DebugCharaLayoutEditorCommonWork* pCommonWork =m_pManager->GetCommonWork();

  if( m_isUpdate == false)
  {
    return STATE_CONTINUE;
  }

  // デバッグログ出力
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GFL_PRINT( "Command Output Start\n");
    GFL_PRINT( "------------------------------\n");
    this->PrintMotion(
        pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].name,
        pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].motionIndex
        );
    GFL_PRINT( "\n");
    this->PrintFace(
      pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].name,
      pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].eyeIndex,
      pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].mouthIndex
      );
    GFL_PRINT( "\n");
    this->PrintWalk(
        pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].name,
        pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].walkStep
        );
    GFL_PRINT( "\n");
    this->PrintRun(
        pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].name,
        pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].runStep
        );
    GFL_PRINT( "------------------------------\n");
    GFL_PRINT( "Command Output End\n");
  }

  // 全登録モデルのデバッグログ出力
  if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
  {
    GFL_PRINT( "Command Output Start\n");
    GFL_PRINT( "------------------------------\n");
    for( s32 i = 0; i < pCommonWork->m_editCharaNum; ++i)
    {
      this->PrintMotion(
          pCommonWork->m_pEditCharaList[ i ].name,
          pCommonWork->m_pCharaWork[ i ].motionIndex
          );
      GFL_PRINT( "\n");
      this->PrintFace(
        pCommonWork->m_pEditCharaList[ i ].name,
        pCommonWork->m_pCharaWork[ i ].eyeIndex,
        pCommonWork->m_pCharaWork[ i ].mouthIndex
        );
      GFL_PRINT( "\n");
      this->PrintWalk(
          pCommonWork->m_pEditCharaList[ i ].name,
          pCommonWork->m_pCharaWork[ i ].walkStep
          );
      GFL_PRINT( "\n");
      this->PrintRun(
          pCommonWork->m_pEditCharaList[ i ].name,
          pCommonWork->m_pCharaWork[ i ].runStep
          );

      GFL_PRINT( "------------------------------\n");
    }
    GFL_PRINT( "Command Output End\n");
  }


  return STATE_CONTINUE;
}

/**
 * @brief セットアップ
 */ 
void DebugCharaLayoutEditorModePlayMotion::Setup( gfl2::debug::DebugWinGroup* pDebugGroup)
{
  m_pDebugGroup = pDebugGroup;

  DebugCharaLayoutEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, m_pMsgData, msg_dmenu_chara_layout_editor_play_motion_walk, reinterpret_cast<void*>( this), UpdateFunc_PlayMotion_Walk, DrawFunc_PlayMotion_Walk );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, m_pMsgData, msg_dmenu_chara_layout_editor_play_motion_run, reinterpret_cast<void*>( this), UpdateFunc_PlayMotion_Run, DrawFunc_PlayMotion_Run );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"Motion", reinterpret_cast<void*>( &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ]), UpdateFunc_PlayMotion, DrawFunc_PlayMotion );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"Eye", reinterpret_cast<void*>( &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ]), UpdateFunc_PlayMotionEye, DrawFunc_PlayMotionEye );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"Mouth", reinterpret_cast<void*>( &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ]), UpdateFunc_PlayMotionMouth, DrawFunc_PlayMotionMouth );

  pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].pSelf = this;

  u32 moveModelIndex = m_pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  m_pControlMoveModel = m_pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);

  //pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].eyeIndex = pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultEyeIndex;
  //pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].mouthIndex = pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultMouthIndex;

  m_isUpdate = true;
}

/**
 * @brief クリーン
 */ 
void DebugCharaLayoutEditorModePlayMotion::Clean()
{
  DebugCharaLayoutEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  m_isUpdate = false;


  // 表情書き戻し
  //Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = m_pControlMoveModel->GetFacialAnimeEye();
  //pAnimeEye->ChangeAnime( static_cast<poke_3d::model::CharaModel::EyeIndex>( pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultEyeIndex) );
  //Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = m_pControlMoveModel->GetFacialAnimeMouth();
  //pAnimeMouth->ChangeAnime( static_cast<poke_3d::model::CharaModel::MouthIndex>( pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultMouthIndex) );


  m_pControlMoveModel = NULL;

  pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].pSelf = NULL;

  m_pDebugGroup->RemoveAllItem();
}

/**
 * @brief 初期化
 */ 
void DebugCharaLayoutEditorModePlayMotion::initialize()
{
}


/**
 * @brief 
 */
void DebugCharaLayoutEditorModePlayMotion::PrintMotion( const char* name, u32 motionIndex)
{
  const wchar_t* strw = m_pManager->GetMotionMessageManager()->GetMotionString( motionIndex);

  char str[ 64 ];
  wcstombs( str, strw, 64);

  GFL_PRINT( "ChrMotionPlayFrameBlend( %s, %s, 0, 8, false );\n", name, str );
}

/**
 * @brief 
 */
void DebugCharaLayoutEditorModePlayMotion::PrintFace( const char* name, u32 eyeIndex, u32 mouthIndex)
{
  const wchar_t* strwEye = m_pManager->GetMotionMessageManager()->GetEyeMotionString( eyeIndex);

  char eyeStr[ 64 ];
  wcstombs( eyeStr, strwEye, 64);

  const wchar_t* strwMouth = m_pManager->GetMotionMessageManager()->GetMouthMotionString( mouthIndex);

  char mouthStr[ 64 ];
  wcstombs( mouthStr, strwMouth, 64);

  GFL_PRINT( "ChrChangeFace( %s, %s, %s);\n", name, eyeStr, mouthStr );
}

/**
 * @brief 
 */
void DebugCharaLayoutEditorModePlayMotion::PrintWalk( const char* name, u32 walkStep)
{
  GFL_PRINT( "ChrMove( %s, CHR_MOVE_CMD_WALK, %d, false );\n", name, walkStep );
  GFL_PRINT( "ChrMoveWait( %s );\n", name);
}

/**
 * @brief 
 */
void DebugCharaLayoutEditorModePlayMotion::PrintRun( const char* name, u32 runStep)
{
  GFL_PRINT( "ChrMove( %s, CHR_MOVE_CMD_RUN, %d, false );\n", name, runStep );
  GFL_PRINT( "ChrMoveWait( %s );\n", name);
}




//------------------------------------------------------------------------------
// DebugCharaLayoutEditorModePlayDynamicMotion
//------------------------------------------------------------------------------

void UpdateFunc_PlayDynamicMotion( void* userWork, gfl2::debug::DebugWinItem* item )
{
  CharaWork* pWork = (CharaWork*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->pManager;
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return;
  }

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  DebugCharaLayoutEditorModePlayDynamicMotion* pSelf = reinterpret_cast<DebugCharaLayoutEditorModePlayDynamicMotion*>( pWork->pSelf);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pSelf->m_pControlMoveModel->GetCharaDrawInstance()->ChangeAnimationSmooth( pWork->dynamicMotionIndex, 0);
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pWork->dynamicMotionIndex++;
    if( pWork->dynamicMotionIndex > pWork->dynamicMotionEndIndex)
    {
      pWork->dynamicMotionIndex = pWork->dynamicMotionStartIndex;
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    pWork->dynamicMotionIndex--;
    if( pWork->dynamicMotionIndex < pWork->dynamicMotionStartIndex)
    {
      pWork->dynamicMotionIndex = pWork->dynamicMotionEndIndex;
    }
  }
}

wchar_t* DrawFunc_PlayDynamicMotion( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  CharaWork* pWork = (CharaWork*)userWork;
  DebugCharaLayoutEditorModeManager* pManager = pWork->pManager;
  DebugCharaLayoutEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  if( pCommonWork->m_pEditCharaList == NULL)
  {
    return workStr;
  }

  u32 packID = pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].dynamic_mot_id;
  u32 motionIndexOrigin0 = pWork->dynamicMotionIndex - pWork->dynamicMotionStartIndex;

  const wchar_t* strw = pManager->GetMotionMessageManager()->GetDynamicMotionString( packID, motionIndexOrigin0);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %s ]", item->GetName() , strw );

  return workStr;
}


/**
 * @brief コンストラクタ
 */ 
DebugCharaLayoutEditorModePlayDynamicMotion::DebugCharaLayoutEditorModePlayDynamicMotion()
  : m_pControlMoveModel( NULL)
{
}

/**
 * @brief デストラクタ
 */ 
DebugCharaLayoutEditorModePlayDynamicMotion::~DebugCharaLayoutEditorModePlayDynamicMotion()
{
}

/**
 * @brief 破棄
 */ 
void DebugCharaLayoutEditorModePlayDynamicMotion::Terminate()
{
}

/**
 * @brief 更新
 * @return 管理クラスの状態
 */ 
DebugCharaLayoutEditorModeBase::State DebugCharaLayoutEditorModePlayDynamicMotion::Update( DebugCharaLayoutEditorModeManager* pManager)
{
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) && gfl2::debug::DebugWin_IsOpen() )
  {
    return STATE_END;
  }

  if( m_isUpdate == false)
  {
    return STATE_CONTINUE;
  }

  DebugCharaLayoutEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  // デバッグログ出力
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GFL_PRINT( "Command Output Start\n");
    GFL_PRINT( "------------------------------\n");
    this->PrintMotion(
        pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].name,
        pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].dynamicMotionIndex,
        pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].dynamic_mot_id,
        pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].dynamicMotionStartIndex
        );
    this->PrintFace(
        pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].name,
        pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].eyeIndex,
        pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].mouthIndex
        );
    GFL_PRINT( "------------------------------\n");
    GFL_PRINT( "Command Output End\n");
  }

  // 全登録モデルのデバッグログ出力
  if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
  {
    GFL_PRINT( "Command Output Start\n");
    GFL_PRINT( "------------------------------\n");
    for( s32 i = 0; i < pCommonWork->m_editCharaNum; ++i)
    {
      this->PrintMotion(
          pCommonWork->m_pEditCharaList[ i ].name,
          pCommonWork->m_pCharaWork[ i ].dynamicMotionIndex,
          pCommonWork->m_pEditCharaList[ i ].dynamic_mot_id,
          pCommonWork->m_pCharaWork[ i ].dynamicMotionStartIndex
          );
      this->PrintFace(
        pCommonWork->m_pEditCharaList[ i ].name,
        pCommonWork->m_pCharaWork[ i ].eyeIndex,
        pCommonWork->m_pCharaWork[ i ].mouthIndex
        );
    }
    GFL_PRINT( "------------------------------\n");
    GFL_PRINT( "Command Output End\n");
  }


  return STATE_CONTINUE;
}

/**
 * @brief セットアップ
 */ 
void DebugCharaLayoutEditorModePlayDynamicMotion::Setup( gfl2::debug::DebugWinGroup* pDebugGroup)
{
  m_pDebugGroup = pDebugGroup;

  DebugCharaLayoutEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  s32 dynamicMotionPackID = pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].dynamic_mot_id;
  if( dynamicMotionPackID == -1)
  {
    // DEBUG_CHR_EDIT_LIST_ADDに非常駐モーションが設定されていなかったら何もしない
    GFL_PRINT( "DEBUG_CHR_EDIT_LIST_ADDに非常駐モーションパックIDを設定してください\n");
    return;
  }
  if( m_pManager->GetFieldmap()->GetMoveModelManager()->IsDynamicAnimationLoaded( dynamicMotionPackID) == false )
  {
    // 非常駐モーションが読まれていなかったら何もしない
    GFL_PRINT( "非常駐モーションパックを読み込んでください\n");
    return;
  }

  // ワーク設定
  pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].pSelf = this;


  // モデル取得
  u32 moveModelIndex = m_pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModelIndexFromEventId( pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].event_id);
  m_pControlMoveModel = m_pManager->GetFieldmap()->GetMoveModelManager()->GetFieldMoveModel( moveModelIndex);


  // アイテム登録
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"Motion", reinterpret_cast<void*>( &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ]), UpdateFunc_PlayDynamicMotion, DrawFunc_PlayDynamicMotion );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"Eye", reinterpret_cast<void*>( &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ]), UpdateFunc_PlayMotionEye, DrawFunc_PlayMotionEye );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"Mouth", reinterpret_cast<void*>( &pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ]), UpdateFunc_PlayMotionMouth, DrawFunc_PlayMotionMouth );


  //pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].eyeIndex = pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultEyeIndex;
  //pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].mouthIndex = pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultMouthIndex;


  m_isUpdate = true;
}

/**
 * @brief クリーン
 */ 
void DebugCharaLayoutEditorModePlayDynamicMotion::Clean()
{
  DebugCharaLayoutEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  s32 dynamicMotionPackID = pCommonWork->m_pEditCharaList[ pCommonWork->m_selectCharaIndex ].dynamic_mot_id;
  if( dynamicMotionPackID == -1)
  {
    // DEBUG_CHR_EDIT_LIST_ADDに非常駐モーションが設定されていなかったら何もしない
    return;
  }
  if( m_pManager->GetFieldmap()->GetMoveModelManager()->IsDynamicAnimationLoaded( dynamicMotionPackID) == false )
  {
    // 非常駐モーションが読まれていなかったら何もしない
    return;
  }

  // 表情書き戻し
  //Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = m_pControlMoveModel->GetFacialAnimeEye();
  //pAnimeEye->ChangeAnime( static_cast<poke_3d::model::CharaModel::EyeIndex>( pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultEyeIndex) );
  //Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = m_pControlMoveModel->GetFacialAnimeMouth();
  //pAnimeMouth->ChangeAnime( static_cast<poke_3d::model::CharaModel::MouthIndex>( pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].defaultMouthIndex) );

  m_pDebugGroup->RemoveAllItem();

  m_pControlMoveModel = NULL;


  pCommonWork->m_pCharaWork[ pCommonWork->m_selectCharaIndex ].pSelf = NULL;

}

/**
 * @brief 初期化
 */ 
void DebugCharaLayoutEditorModePlayDynamicMotion::initialize()
{
}

/**
 * @brief 
 */
void DebugCharaLayoutEditorModePlayDynamicMotion::PrintMotion( const char* name, u32 motionIndex, s32 packID, u32 startIndex)
{
  if( packID == -1)
  {
    return;
  }

  u32 motionIndexOrigin0 = motionIndex - startIndex;
  const wchar_t* strw = m_pManager->GetMotionMessageManager()->GetDynamicMotionString( packID, motionIndexOrigin0);

  char str[ 64 ];
  wcstombs( str, strw, 64);

  GFL_PRINT( "ChrMotionPlayFrameBlend( %s, %s, 0, 8, false );\n", name, str );
}

/**
 * @brief 
 */
void DebugCharaLayoutEditorModePlayDynamicMotion::PrintFace( const char* name, u32 eyeIndex, u32 mouthIndex)
{
  const wchar_t* strwEye = m_pManager->GetMotionMessageManager()->GetEyeMotionString( eyeIndex);

  char eyeStr[ 64 ];
  wcstombs( eyeStr, strwEye, 64);

  const wchar_t* strwMouth = m_pManager->GetMotionMessageManager()->GetMouthMotionString( mouthIndex);

  char mouthStr[ 64 ];
  wcstombs( mouthStr, strwMouth, 64);

  GFL_PRINT( "ChrChangeFace( %s, %s, %s);\n", name, eyeStr, mouthStr );
}

GFL_NAMESPACE_END( Field )

#endif // GF_ENABLE_DEBUG_EDITOR
#endif // PM_DEBUG

