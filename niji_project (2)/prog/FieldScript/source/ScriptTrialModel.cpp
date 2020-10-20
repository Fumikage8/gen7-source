//==============================================================================
/**
 * @file        ScriptTrialModel.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/06/11(木) 18:22:17
 * @date        2015/09/09(水) 18:15:06 N.Takeda  更新.
 */
//==============================================================================

#include "FieldScript/include/ScriptTrialModel.h"


#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
// GameSys
#include "GameSys/include/GameManager.h"
#include "System/include/HeapSize.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"

// TrialModel
#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"

// arc
#include <arc_def_index/arc_def.h>



GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )


//------------------------------------------------------------------------------
/**
 * @func        ScriptTrialModel
 * @brief       コンストラクタ.
 * @author      N.Takeda
 * @date        2015/09/09(水) 18:06:23
 *
 * @param[in,out] gfl2::heap::HeapBase *  p_heap    .
 * @param[in,out] GameSys::GameManager*   pGameMan  .
 */
//------------------------------------------------------------------------------
ScriptTrialModel::ScriptTrialModel( gfl2::heap::HeapBase * p_heap, GameSys::GameManager *pGameMan )
 : m_pGameMan( pGameMan )
 , m_pHeap( NULL )
 , m_pSysHeap( NULL )
 , m_pFactory( NULL )
 , m_eState( STATE_DONE )
 , m_bInitialized( false )
 , m_iResourceNum( 0 )
{
  
  for( int ii=0 ; ii<RESOURCE_MAX ; ii++)
  {
    m_stResource[ ii ].m_pModel = NULL;
    m_stResource[ ii ].m_pBuff = NULL;
    m_stResource[ ii ].m_pAllocator = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @func        ~ScriptTrialModel
 * @brief       デストラクタ.
 * @author      N.Takeda
 * @date        2015/09/10(木) 17:24:31
 */
//------------------------------------------------------------------------------
ScriptTrialModel::~ScriptTrialModel()
{
  Tarminate();
}

//------------------------------------------------------------------------------
/**
 * @func        Tarminate
 * @brief       破棄.
 * @author      N.Takeda
 * @date        2015/09/10(木) 17:50:44
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptTrialModel::Tarminate()
{
  if( m_bInitialized )
  {
   /**
   * @note  N.Takeda
   * 残したまま削除された場合の対応. 基本的にはここが実行される事があってはならない
   * テスト時に、途中でマップジャンプしたりしても止まらないようにする為.
   */
    for( s32 ii=0 ; ii<RESOURCE_MAX ; ii++ )
    {
      if( m_stResource[ ii ].m_pModel )
      {
        m_stResource[ ii ].m_pModel->SetDisp( false );
        Del( ii );
      }
    }
    
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closr;
    closr.arcId = ARCID_TRIAL_EVENT;
    closr.heapForReq = m_pHeap;
    m_pGameMan->GetAsyncFileManager()->AddArcFileCloseReq( closr );
    
    m_pFactory->DeleteSystem();
    while( !m_pFactory->IsDelete() );
    m_pFactory->FinalizeSystem();
    
    m_bInitialized = false;
  }
}


//------------------------------------------------------------------------------
/**
 * @func        Load
 * @brief       読み込み関数.
 * @author      N.Takeda
 * @date        2015/09/09(水) 19:26:29
 *
 * @param[in] const s32 _iDatId   ID.
 * @param[in] const s32 _iMonsNo  モンスターNo
 * @param[in] const s32 _iFormNo  フォルムNo.
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
s32  ScriptTrialModel::Load( const s32 _iId, const s32 _iDatId, const s32 _iMonsNo, const s32 _iFormNo )
{
  m_iResourceNum = _iId;
  GFL_ASSERT( m_iResourceNum < RESOURCE_MAX );
  m_stResource[ m_iResourceNum ].m_iDatId = _iDatId;
  m_stResource[ m_iResourceNum ].m_iMonsNo = _iMonsNo;
  m_stResource[ m_iResourceNum ].m_iFormNo = _iFormNo;
  
  m_eState = STATE_LOAD;
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        Del
 * @brief       Del関数.
 * @author      N.Takeda
 * @date        2015/09/10(木) 18:25:06
 *
 * @param[in] const s32 _iId  ID.
 *
 * @return      s32 
 */
//------------------------------------------------------------------------------
s32  ScriptTrialModel::Del( const s32 _iId )
{
  m_pFactory->DeleteTrialModel( m_stResource[ _iId ].m_pModel );
  m_stResource[ _iId ].m_pModel = NULL;
  
  if( m_stResource[ _iId ].m_pBuff != NULL )
  {
    GflHeapSafeFreeMemory( m_stResource[ _iId ].m_pBuff );  // Coverity.10964
  }
  m_stResource[ _iId ].m_pBuff = NULL;
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        UnLoad
 * @brief       Un読み込み関数.
 * @author      N.Takeda
 * @date        2015/09/10(木) 16:56:50
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptTrialModel::UnLoad()
{
  if( !m_bInitialized ) return;
  
  for( s32 ii=0 ; ii<RESOURCE_MAX ; ii++ )
  {
    if( m_stResource[ ii ].m_pModel )
    {
      m_stResource[ ii ].m_pModel->SetDisp( false );
    }
  }
  m_eState = STATE_UNLOAD_FREE;
  
}


//------------------------------------------------------------------------------
/**
 * @func        Disp
 * @brief       表示切替.
 * @author      N.Takeda
 * @date        2015/09/10(木) 11:42:36
 *
 * @param[in] const s32   _iId    ID.
 * @param[in] const bool  _bDisp  .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptTrialModel::Disp( const s32 _iId, const bool _bDisp )
{
  GFL_ASSERT( _iId < RESOURCE_MAX );
  GFL_ASSERT( m_stResource[ _iId ].m_pModel );
  m_stResource[ _iId ].m_pModel->SetDisp( _bDisp );
}

//------------------------------------------------------------------------------
/**
 * @func        SetShadowVisible
 * @brief       影の表示,非表示.
 * @author      N.Takeda
 * @date        2015/09/11(金) 13:14:11
 *
 * @param[in] const s32   _iId    ID.
 * @param[in] const bool  _bDisp  .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptTrialModel::SetShadowVisible( const s32 _iId, const bool _bDisp )
{
  GFL_ASSERT( _iId < RESOURCE_MAX );
  GFL_ASSERT( m_stResource[ _iId ].m_pModel );
  m_stResource[ _iId ].m_pModel->SetShadowVisible( _bDisp );
}

//------------------------------------------------------------------------------
/**
 * @func        PlayAnim
 * @brief       アニメーション切り替え.
 * @author      N.Takeda
 * @date        2015/09/10(木) 11:42:39
 *
 * @param[in] const s32 _iId    ID.
 * @param[in] const s32 _iAnim  .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptTrialModel::PlayAnim( const s32 _iId, const s32 _iAnim )
{
  GFL_ASSERT( _iId < RESOURCE_MAX );
  GFL_ASSERT( m_stResource[ _iId ].m_pModel );
  m_stResource[ _iId ].m_pModel->ChangeAnimation( _iAnim );
}


//------------------------------------------------------------------------------
/**
 * @func        PlayAnim
 * @brief       アニメーション切り替え.
 * @author      N.Takeda
 * @date        2015/09/10(木) 11:42:39
 *
 * @param[in] const s32 _iId    ID.
 * @param[in] const s32 _iAnim  .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
bool  ScriptTrialModel::PlayIsEnd( const s32 _iId )
{
  GFL_ASSERT( _iId < RESOURCE_MAX );
  GFL_ASSERT( m_stResource[ _iId ].m_pModel );
  return m_stResource[ _iId ].m_pModel->IsAnimationLastFrame();
}


//------------------------------------------------------------------------------
/**
 * @func        Update
 * @brief       更新.
 * @author      N.Takeda
 * @date        2015/09/09(水) 19:13:34
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  ScriptTrialModel::Update()
{
  
  
  switch( m_eState )
  {
    case STATE_LOAD:
      m_pSysHeap = m_pGameMan->GetFieldmap()->GetFieldExtHeap();
      //m_pSysHeap->Dump();
      
      if( m_bInitialized ){
        m_eState = STATE_OPENWAIT;
        break;
      }
      m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pSysHeap, System::HeapSize::HEAP_FIELD_BATTLE_MODEL_HEAP_SIZE-0x1000, gfl2::heap::HEAP_TYPE_EXP, false, "ScriptTrialModel" ); //ぎりぎりまでメモリ確保
      
      m_pFactory = m_pGameMan->GetFieldmap()->GetTrialModelFactory();
      m_pFactory->InitializeSystem( m_pHeap, m_pHeap, 1 );
      m_eState = STATE_INITWAIT;
      //break;//そのまま下へ
    
    case STATE_INITWAIT:
    {
      if( !m_pFactory->IsInitialize() ) break;
      
      m_pFactory->CreateSystem( m_pHeap );
      m_eState = STATE_OPEN;
      //break;//そのまま下へ
    }
    case STATE_OPEN:
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openr;
      openr.arcId       = ARCID_TRIAL_EVENT;
      openr.heapForFile = m_pHeap;
      openr.heapForReq  = m_pHeap->GetLowerHandle();
      m_pGameMan->GetAsyncFileManager()->AddArcFileOpenReq( openr );
      m_eState = STATE_OPENWAIT;
    }
      m_bInitialized = true;
      //break;//そのまま下へ
    
    case STATE_OPENWAIT:
      if( !m_pGameMan->GetAsyncFileManager()->IsArcFileOpenFinished( ARCID_TRIAL_EVENT ) ) break;
      
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
        loadr.arcId = ARCID_TRIAL_EVENT;
        loadr.datId = m_stResource[ m_iResourceNum ].m_iDatId;
        loadr.ppBuf = &m_stResource[ m_iResourceNum ].m_pBuff;
        loadr.heapForBuf = m_pHeap;
        loadr.heapForReq = m_pHeap->GetLowerHandle();
        loadr.heapForCompressed = m_pHeap->GetLowerHandle();
        loadr.align = 128;
        m_pGameMan->GetAsyncFileManager()->AddArcFileLoadDataReq( loadr );
        m_eState = STATE_LOADWAIT;
      }
      //break;//そのまま下へ
    
    case STATE_LOADWAIT:
      if( !m_pGameMan->GetAsyncFileManager()->IsArcFileLoadDataFinished( &m_stResource[ m_iResourceNum ].m_pBuff ) ) break;
      
      {
        PokeTool::SimpleParam param;
        param.monsNo = static_cast<MonsNo>(m_stResource[ m_iResourceNum ].m_iMonsNo);
        param.formNo = static_cast<FormNo>(m_stResource[ m_iResourceNum ].m_iFormNo);
        PokeTool::PokeModel::SetupOption opt;
        opt.dataHeap = m_pHeap;
        //opt.workHeap = m_pHeap->GetLowerHandle();
        opt.workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD );
        opt.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
        opt.useShadow = false;
        m_stResource[ m_iResourceNum ].m_pModel = m_pFactory->CreateTrialModel( m_pHeap, &param, opt );
      }
      m_eState = STATE_CREATEWAIT;
      //break;//そのまま下へ
    
    case STATE_CREATEWAIT:
      if( !m_stResource[ m_iResourceNum ].m_pModel->IsReady() ) break;
      
      m_stResource[ m_iResourceNum ].m_pAllocator = System::GflUse::GetEventDeviceAllocator();
      m_stResource[ m_iResourceNum ].m_pModel->CreateModel();
      m_stResource[ m_iResourceNum ].m_pModel->SetEnableAmbientColor( true );
      m_stResource[ m_iResourceNum ].m_pModel->LoadMotion( m_stResource[ m_iResourceNum ].m_pAllocator, m_pHeap, m_stResource[ m_iResourceNum ].m_pBuff );
      
      m_eState = STATE_DONE;
      break;
    
    
    
    
    case STATE_UNLOAD_FREE:
    {
      bool  bNotDel = false;
      for( s32 ii=0 ; ii<RESOURCE_MAX ; ii++ )
      {
        if( m_stResource[ ii ].m_pModel ){
          if( !m_stResource[ ii ].m_pModel->GetPokeModel()->CanDelete() )  bNotDel = true;
        }
      }
      if( !bNotDel )  m_eState = STATE_UNLOAD_FREEWAIT;
    }
      break;
    
    case STATE_UNLOAD_FREEWAIT:
    {
      for( s32 ii=0 ; ii<RESOURCE_MAX ; ii++ )
      {
        if( m_stResource[ ii ].m_pModel ) Del( ii );
      }
      
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closr;
      closr.arcId = ARCID_TRIAL_EVENT;
      closr.heapForReq = m_pHeap;
      m_pGameMan->GetAsyncFileManager()->AddArcFileCloseReq( closr );
    }
      m_eState = STATE_UNLOAD_DEL;
      //break;//そのまま下へ
    
    case STATE_UNLOAD_DEL:
      if( !m_pGameMan->GetAsyncFileManager()->IsArcFileCloseFinished( ARCID_TRIAL_EVENT ) ) break;
      m_pFactory->DeleteSystem();
      m_eState = STATE_UNLOAD_DELWAIT;
      //break;//そのまま下へ
    
    case STATE_UNLOAD_DELWAIT:
      if( !m_pFactory->IsDelete() ) break;
      
      m_pFactory->FinalizeSystem();
      m_bInitialized = false;
      m_eState = STATE_DONE;
      GFL_DELETE_HEAP(m_pHeap);
      //m_pSysHeap->Dump();
      break;
    
  }
  
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

