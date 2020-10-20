//=================================================================================================
/**
*  @file   FieldMoveCodeManager.cpp
*  @brief  フィールド用 動作コードマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../../include/MoveCode/FieldMoveCodeManager.h"
#include "../../include/MoveCode/FieldMoveCodeBase.h"
#include "../../include/MoveCode/FieldMoveCodePlayer.h"
#include "../../include/MoveCode/FieldMoveCodeNpc.h"
#include "../../include/MoveCode/FieldMoveCodeTrainerEye.h"
#include "../../include/MoveCode/FieldMoveCodeRideKentaros.h"
#include "../../include/MoveCode/FieldMoveCodeRideSamehader.h"
#include "../../include/MoveCode/FieldMoveCodeRideMuurando.h"
#include "../../include/MoveCode/FieldMoveCodeTraffic.h"
#include "../../include/MoveCode/FieldMoveCodeTrainerPath.h"
#include "../../include/MoveCode/FieldMoveCodeGimmickEncountChase.h"
#include "../../include/MoveCode/FieldMoveCodeGimmickEncountPatrol.h"
#include "../../include/MoveCode/FieldMoveCodeGimmickEncountEscape.h"
#include "../../include/MoveCode/FieldMoveCodeRideLaplace.h"
#include "../../include/MoveCode/FieldMoveCodeRideKairiky.h"
#include "../../include/MoveCode/FieldMoveCodeRideRoba2.h"

// RTTIデバッグシステム
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"

namespace Field{ namespace MoveModel {

//=====================================================================================================================
// マネージャー自身に関する操作
//=====================================================================================================================

const FieldMoveCodeBase * FieldMoveCodeManager::m_MoveCodeTable[ FIELD_MOVE_CODE_NUM ] ={};

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeManager::FieldMoveCodeManager()
{
  for (int i = 0; i < FIELD_MOVE_CODE_NUM; ++ i )
  {
    m_MoveCodeTable[i] = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeManager::~FieldMoveCodeManager()
{
  Terminate();
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化
*
*  @param  pHeap  ワーク作成時に利用するヒープ
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeManager::Initialize(gfl2::heap::HeapBase * pHeap)
{
  m_MoveCodeTable[FIELD_MOVE_CODE_NONE]                   = GFL_NEW( pHeap ) FieldMoveCodeBase();
  m_MoveCodeTable[FIELD_MOVE_CODE_PLAYER]                 = GFL_NEW( pHeap ) FieldMoveCodePlayer();
  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_RETURN]             = GFL_NEW( pHeap ) FieldMoveCodeNpcReturn();
  m_MoveCodeTable[FIELD_MOVE_CODE_TRAINER_EYE]            = GFL_NEW( pHeap ) FieldMoveCodeTrainerEye();
  m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_KENTAROS]          = GFL_NEW( pHeap ) FieldMoveCodeRideKentaros();
  m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_SAMEHADER]         = GFL_NEW( pHeap ) FieldMoveCodeRideSamehader();
  m_MoveCodeTable[FIELD_MOVE_CODE_TRAFFIC]                = GFL_NEW( pHeap ) FieldMoveCodeTraffic();
  m_MoveCodeTable[FIELD_MOVE_CODE_TRAINER_PATH]           = GFL_NEW( pHeap ) FieldMoveCodeTrainerPath();
  m_MoveCodeTable[FIELD_MOVE_CODE_GIMMICK_ENCOUNT_CHASE]  = GFL_NEW( pHeap ) FieldMoveCodeGimmickEncountChase();
  m_MoveCodeTable[FIELD_MOVE_CODE_GIMMICK_ENCOUNT_PATROL] = GFL_NEW( pHeap ) FieldMoveCodeGimmickEncountPatrol();
  m_MoveCodeTable[FIELD_MOVE_CODE_GIMMICK_ENCOUNT_ESCAPE] = GFL_NEW( pHeap ) FieldMoveCodeGimmickEncountEscape();
  m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_MUURAND]           = GFL_NEW( pHeap ) FieldMoveCodeRideMuurando();
  m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_LAPLACE]           = GFL_NEW( pHeap ) FieldMoveCodeRideLaplace();
  m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_KAIRIKY]           = GFL_NEW( pHeap ) FieldMoveCodeRideKairiky();
  m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_ROBA2]           = GFL_NEW( pHeap ) FieldMoveCodeRideRoba2();
  
  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_MEET]             = GFL_NEW( pHeap ) FieldMoveCodeNpcMeet();
  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_MEET_RETURN]      = GFL_NEW( pHeap ) FieldMoveCodeNpc();
  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_LOOK]             = GFL_NEW( pHeap ) FieldMoveCodeNpcLook();
  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_LOOK_RETURN]      = GFL_NEW( pHeap ) FieldMoveCodeNpcLookReturn();

  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_MEET_SHORT]        = GFL_NEW( pHeap ) FieldMoveCodeNpcMeetShort();
  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_MEET_SHORT_RETURN] = GFL_NEW( pHeap ) FieldMoveCodeNpcMeetShortReturn();
  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_LOOK_SHORT]        = GFL_NEW( pHeap ) FieldMoveCodeNpcLookShort();
  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_LOOK_SHORT_RETURN] = GFL_NEW( pHeap ) FieldMoveCodeNpcLookShortReturn();

  // @fix NMcat[1248]:タマゴが発生した際に、一定ラインを超えないとNPC動作が発生しない、対処。NPC動作制御をintrPosから動作コードに変更
  m_MoveCodeTable[FIELD_MOVE_CODE_NPC_AZUKARIYA]         = GFL_NEW( pHeap ) FieldMoveCodeNpcAzukariya();
  m_MoveCodeTable[FIELD_MOVE_CODE_FUREAI_A]              = GFL_NEW( pHeap ) FieldMoveCodeFureaiTypeA();


#if USE_RTTI_DEBUG
  FieldMoveCodePlayer* pFieldMoveCodePlayer = (FieldMoveCodePlayer*)m_MoveCodeTable[FIELD_MOVE_CODE_PLAYER];
  gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( L"プレイヤーコントロール" , &typeid( FieldMoveCodePlayer ) , pFieldMoveCodePlayer );

  FieldMoveCodeRideKentaros* pFieldMoveCodeKentaros = (FieldMoveCodeRideKentaros*)m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_KENTAROS];
  gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( L"ケンタロスコントロール" , &typeid( FieldMoveCodeRideKentaros ) , pFieldMoveCodeKentaros );

  FieldMoveCodeRideSamehader* pFieldMoveCodeSamehader = (FieldMoveCodeRideSamehader*)m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_SAMEHADER];
  gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( L"サメハダーコントロール" , &typeid( FieldMoveCodeRideSamehader ) , pFieldMoveCodeSamehader );

  FieldMoveCodeRideMuurando* pFieldMoveCodeMuurando = (FieldMoveCodeRideMuurando*)m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_MUURAND];
  gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( L"ムーランドコントロール" , &typeid( FieldMoveCodeRideMuurando ) , pFieldMoveCodeMuurando );
  
  FieldMoveCodeRideLaplace* pFieldMoveCodeLaplace = (FieldMoveCodeRideLaplace*)m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_LAPLACE];
  gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( L"ラプラスコントロール" , &typeid( FieldMoveCodeRideSamehader ) , pFieldMoveCodeLaplace );

  FieldMoveCodeRideKairiky* pFieldMoveCodeKairiky = (FieldMoveCodeRideKairiky*)m_MoveCodeTable[FIELD_MOVE_CODE_RIDE_KAIRIKY];
  gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( L"カイリキーコントロール" , &typeid( FieldMoveCodeRideKairiky ) , pFieldMoveCodeKairiky );
#endif
}

//----------------------------------------------------------------------------
/**
*  @brief  解放
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeManager::Terminate(void)
{
  for (int i = 0; i < FIELD_MOVE_CODE_NUM; ++ i )
  {
    GFL_SAFE_DELETE( m_MoveCodeTable[i] );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コードの取得
*
*  @param  fieldMoveCodeId   動作コードID
*  @return 動作コード
*/
//-----------------------------------------------------------------------------
const FieldMoveCodeBase * FieldMoveCodeManager::GetMoveCode(FIELD_MOVE_CODE_ID fieldMoveCodeId)
{
  const FieldMoveCodeBase * move_code = m_MoveCodeTable[static_cast<u32>(fieldMoveCodeId)];
  if ( move_code )
  {
    return move_code;
  }
  return m_MoveCodeTable[0];  //MOVE_CODE_NONE
}

//=====================================================================================================================
// 動作コードのワークに関する操作
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
*  @brief  動作コードのワークを初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  pModel         動作モデル
*  @param  extendWorkSize 拡張メモリのサイズ
*  @param  pExtendWork    拡張メモリ
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeManager::InitializeMoveCodeWork(FieldMoveCodeWork* pMoveCodeWork, FieldMoveModel* pModel, u32 extendWorkSize, u32* pExtendWork)
{
  pMoveCodeWork->pModel         = pModel;
  pMoveCodeWork->moveCodeId     = FIELD_MOVE_CODE_NONE;
  pMoveCodeWork->extendWorkSize = extendWorkSize;
  pMoveCodeWork->pExtendWork    = pExtendWork;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コードのワークを取得
*    
*  @param  pMoveCodeWork  動作コード動作コードワーク
*  @param  size           必要なサイズ
*  @return 動作コードワーク
*/
//-----------------------------------------------------------------------------
void* FieldMoveCodeManager::GetMoveCodeWork(FieldMoveCodeWork* pMoveCodeWork, u32 size)
{
  GFL_ASSERT( size <= FIELD_MOVE_CODE_WORK_SIZE );
  return reinterpret_cast<void*>(pMoveCodeWork->work);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コードのワークを取得(const版)
*    
*  @param  pMoveCodeWork  動作コード動作コードワーク
*  @param  size           必要なサイズ
*  @return 動作コードワーク
*/
//-----------------------------------------------------------------------------
const void* FieldMoveCodeManager::GetMoveCodeWork( const FieldMoveCodeWork* pMoveCodeWork, u32 size)
{
  GFL_ASSERT( size <= FIELD_MOVE_CODE_WORK_SIZE );
  return reinterpret_cast<const void*>(pMoveCodeWork->work);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コードの拡張ワークを取得
*    
*  @param  pMoveCodeWork  動作コードワーク
*  @param  size           必要なサイズ
*  @return 動作コード拡張ワーク
*/
//-----------------------------------------------------------------------------
void* FieldMoveCodeManager::GetMoveCodeExtendWork(FieldMoveCodeWork* pMoveCodeWork, u32 size)
{
  GFL_ASSERT( size <= pMoveCodeWork->extendWorkSize );
  return reinterpret_cast<void*>(pMoveCodeWork->pExtendWork);
}

//----------------------------------------------------------------------------
/**
*  @brief  NPC用の動作コードか取得
*
*  @param  fieldMoveCodeId   動作コードID
*  @return true NPC用
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeManager::IsNpcMoveCode(FIELD_MOVE_CODE_ID fieldMoveCodeId)
{
  switch( fieldMoveCodeId )
  {
    case FIELD_MOVE_CODE_NPC_RETURN:
    case FIELD_MOVE_CODE_NPC_MEET:
    case FIELD_MOVE_CODE_NPC_MEET_RETURN:
    case FIELD_MOVE_CODE_NPC_LOOK:
    case FIELD_MOVE_CODE_NPC_LOOK_RETURN:
    case FIELD_MOVE_CODE_NPC_MEET_SHORT:
    case FIELD_MOVE_CODE_NPC_MEET_SHORT_RETURN:
    case FIELD_MOVE_CODE_NPC_LOOK_SHORT:
    case FIELD_MOVE_CODE_NPC_LOOK_SHORT_RETURN:
    // @fix NMcat[1248]:タマゴが発生した際に、一定ラインを超えないとNPC動作が発生しない、対処。NPC動作制御をintrPosから動作コードに変更
    case FIELD_MOVE_CODE_NPC_AZUKARIYA:
      {
        return true;
      }
  }

  return false;
}

}; //end of namespace MoveModel
}; //end of namespace Field
