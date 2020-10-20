//=================================================================================================
/**
 * @file   EventListDataManager.cpp
 * @brief  イベントリスト　データマネージャークラス
 * @author miyuki iwasawa
 * @date   2015.08.27
 */
//=================================================================================================

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/EventWork.h"

#include "Field/FieldStatic/include/EventList/EventListDataManager.h"


#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(EventList)

//--------------------------------------------------------------
/**
 * @brief   イベントリストデータ コンストラクタ
 */
//--------------------------------------------------------------
EventListData::EventListData() :
  m_pEventWork( NULL )
 ,m_pHeap( NULL )
 ,m_pDetailInfo( NULL )
 ,m_pDetailField( NULL )
{
  gfl2::std::MemClear( &m_Param, sizeof(EventListData::Param) ); 
}
//--------------------------------------------------------------
/**
 * @brief   イベントリストデータ デストラクタ
 */
//--------------------------------------------------------------
EventListData::~EventListData()
{
  GFL_SAFE_DELETE_ARRAY(m_pDetailInfo);  
  GFL_SAFE_DELETE_ARRAY(m_pDetailField);  
}

//--------------------------------------------------------------
/**
 * @brief   イベントリストデータ データセット 
 */
//--------------------------------------------------------------
void EventListData::SetupParam(
  EventWork* pEventWork, gfl2::heap::HeapBase* heap, EventListData::Param* p_param, EventListData::DetailInfo* p_info, EventListData::DetailField* p_field)
{
  m_pEventWork = pEventWork;
  m_pDetailInfo  = GFL_NEW_ARRAY( heap ) EventListData::DetailInfo[p_param->detail_num];
  m_pDetailField = GFL_NEW_ARRAY( heap ) EventListData::DetailField[p_param->detail_num];
  
  gfl2::std::MemCopy( p_param, &m_Param, sizeof(EventListData::Param));
  gfl2::std::MemCopy( p_info, m_pDetailInfo, sizeof(EventListData::DetailInfo)*p_param->detail_num);
  gfl2::std::MemCopy( p_field, m_pDetailField, sizeof(EventListData::DetailField)*p_param->detail_num);
}

//--------------------------------------------------------------
/**
 * @brief   イベントリスト詳細データ取得 
 */
//--------------------------------------------------------------
const EventListData::DetailInfo* EventListData::GetDetailInfo( u32 index ) const
{
  if( index < m_Param.detail_num ) return &m_pDetailInfo[ index ];
  GFL_ASSERT_MSG(0,"index over %d/%d\n", index, m_Param.detail_num );
  return &m_pDetailInfo[0];
}

const EventListData::DetailInfo* EventListData::GetDetailInfoCurrent() const
{
  return &m_pDetailInfo[this->GetCurrentIndex()];
}

//--------------------------------------------------------------
/**
 * @brief   イベントリスト フィールド詳細データ取得 
 */
//--------------------------------------------------------------
const EventListData::DetailField* EventListData::GetDetailField( u32 index ) const
{
  if( index < m_Param.detail_num ) return &m_pDetailField[ index ];
  GFL_ASSERT_MSG(0,"index over %d/%d\n", index, m_Param.detail_num );
  return &m_pDetailField[0];
}
const EventListData::DetailField* EventListData::GetDetailFieldCurrent() const
{
  return &m_pDetailField[this->GetCurrentIndex()];
}

//--------------------------------------------------------------
/**
 * @brief このイベントがアクティブか？
 *
 * @retval  true   アクティブ（開始されている）
 * @retval  false  パッシブ
 */
//--------------------------------------------------------------
bool EventListData::IsActive() const
{
  u32 value = this->GetWorkValue();

  if( value > 0 ) return true;
  return false;
}

//--------------------------------------------------------------
/**
 * @brief このイベントをクリア済みか？ 
 *
 * @retval  true   クリアしている
 * @retval  false  クリアしていない
 */
//--------------------------------------------------------------
bool EventListData::IsClear() const
{
  u32 value = this->GetWorkValue();

  if( value > m_pDetailInfo[m_Param.detail_num-1].work_value ) return true;
  return false;
}

//--------------------------------------------------------------
/**
 * @brief このイベントのカレントIndexを返す
 *
 * @com イベントオープン前は0を、イベントクリア後は末尾を返す
 */
//--------------------------------------------------------------
u32 EventListData::GetCurrentIndex() const
{
  u32 value = this->GetWorkValue();

  if( !this->IsActive() ) return 0; //未開放
  if( this->IsClear() ) return m_Param.detail_num-1; //クリア時は末尾

  for( u32 i = 0; i < m_Param.detail_num; i++ ){
    if( value <= m_pDetailInfo[i].work_value ) return i;
  }
  GFL_ASSERT(0);
  return 0;
}

//--------------------------------------------------------------
/**
 * @brief このイベントの現在のワーク値を返す 
 */
//--------------------------------------------------------------
u32 EventListData::GetWorkValue() const
{
  u16* p_work = m_pEventWork->GetEventWorkAdrs( WK_EVLIST_MAIN_001 ); //仕様変更で固定値になった
  return static_cast<u32>(*p_work);
}

//--------------------------------------------------------------
/**
 * @brief   イベントデータマネージャー コンストラクタ
 * @param   heap         workをとるためのメモリクラス
 */
//--------------------------------------------------------------
EventListDataManager::EventListDataManager( GameSys::GameManager* pGameManager, gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager* pAsyncFileManager) :
  m_pGameManager( pGameManager )
 ,m_pHeap( heap )
 ,m_pAsyncFileManager( pAsyncFileManager )
 ,m_Type( EVENT_LIST_DATA_TYPE_APP )
 ,m_ListNumMain( 0 )
 //,m_ListNumSub( 0 )
 ,m_pListMain( NULL )
 //,m_pListSub( NULL )
 ,m_SeqInitialize(0)
{

}

//--------------------------------------------------------------
/**
 * @brief イベントデータマネージャー デストラクタ
 */
//--------------------------------------------------------------
EventListDataManager::~EventListDataManager()
{
  this->Terminate();
}

//--------------------------------------------------------------
/**
 * @brief イベントデータマネージャー データロード開始
 */
//--------------------------------------------------------------
void EventListDataManager::Initialize( EventListDataType type )
{
  m_Type = type;

  //---------------------------------------------------
  // ARCファイルオープン
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_SCRIPT_EVENT_DATA;
    openReq.heapForFile = m_pHeap->GetLowerHandle();
    openReq.heapForArcSrc = m_pHeap->GetLowerHandle();
    openReq.heapForReq = m_pHeap->GetLowerHandle();
    m_pAsyncFileManager->AddArcFileOpenReq( openReq );
  }
  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    loadReq.arcId = ARCID_SCRIPT_EVENT_DATA;
    loadReq.datId = GARC_script_event_data_event_list_main_BIN;
    loadReq.ppBuf = &m_pBufferMain;
    loadReq.heapForBuf = m_pHeap->GetLowerHandle();
    loadReq.heapForReq = m_pHeap->GetLowerHandle();
    loadReq.heapForCompressed = NULL;//m_pHeap->GetLowerHandle();
    loadReq.pBufSize = NULL;
    loadReq.align = 4;
    m_pAsyncFileManager->AddArcFileLoadDataReq( loadReq );  
  }
}

//--------------------------------------------------------------
/**
 * @brief イベントデータマネージャー データロード待ち
 *
 * @retval  true   ロード終了
 * @retval  false  ロード待ち中
 */
//--------------------------------------------------------------
bool EventListDataManager::InitializeWait()
{
  enum
  {
    SEQ_LOADWAIT,
    SEQ_FILECLOSE,
    SEQ_END,
  };

  switch(m_SeqInitialize)
  {
  case SEQ_LOADWAIT:
    {
      // 読み込み完了待ち
      if( !m_pAsyncFileManager->IsArcFileOpenFinished( ARCID_SCRIPT_EVENT_DATA ) ){
        return false;
      }
      if( !m_pAsyncFileManager->IsArcFileLoadDataFinished( &m_pBufferMain ) ){
        return false;
      }
     
      //データ格納
      m_pListMain = this->SetupData(m_pBufferMain, &m_ListNumMain );

      //バッファは開放
      GflHeapSafeFreeMemory(m_pBufferMain);  

      //アーカイブ閉じる
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.arcId = ARCID_SCRIPT_EVENT_DATA;
        closeReq.heapForReq = m_pHeap->GetLowerHandle();
        m_pAsyncFileManager->AddArcFileCloseReq( closeReq );
      }

      m_SeqInitialize ++;
    }
    // break through

  case SEQ_FILECLOSE:
    if( !m_pAsyncFileManager->IsArcFileCloseFinished( ARCID_SCRIPT_EVENT_DATA ) )
    {
      return false;
    }
    m_SeqInitialize ++;
    // break; through

  case SEQ_END:
    return true;
  }

  return false;
}

//--------------------------------------------------------------
/**
 * @brief イベントデータマネージャー データ開放
 */
//--------------------------------------------------------------
void EventListDataManager::Terminate()
{
  GFL_SAFE_DELETE_ARRAY( m_pListMain );
  //GFL_SAFE_DELETE_ARRAY( m_pListSub );
  
  m_ListNumMain = 0;
  //m_ListNumSub = 0;
  m_SeqInitialize = 0;
}

//--------------------------------------------------------------
/**
 * @brief イベントデータマネージャー データ格納
 */
//--------------------------------------------------------------
EventListData* EventListDataManager::SetupData( void* p_buffer, u32* p_num )
{
  EventWork* pEventWork = m_pGameManager->GetGameData()->GetEventWork();
  EventListData* pData = NULL;
  EventListData::Param* pParam = NULL;
  EventListData::DetailInfo*  pDInfo = NULL;
  EventListData::DetailField* pDField = NULL;
  int offset = 0;

  //ヘッダを取得
  EventListData::BinHeader* pHeader = static_cast<EventListData::BinHeader*>( p_buffer );

  u32 param_num = 0;

  pData = GFL_NEW_ARRAY( m_pHeap ) EventListData[pHeader->param_num]( );
  pParam  = reinterpret_cast<EventListData::Param*>( reinterpret_cast<u8*>(p_buffer)+pHeader->param_offset );
  pDInfo  = reinterpret_cast<EventListData::DetailInfo*>( reinterpret_cast<u8*>(p_buffer)+pHeader->detail_info_offset );
  pDField = reinterpret_cast<EventListData::DetailField*>( reinterpret_cast<u8*>(p_buffer)+pHeader->detail_field_offset );

  for( u32 i = 0; i < pHeader->param_num; i++ ){
    if( this->IsEnableParam(&pParam[i]) ){
      pData[param_num++].SetupParam( pEventWork, m_pHeap, &pParam[i], &pDInfo[offset], &pDField[offset] );
    }
    offset += pParam[i].detail_num;
  }
  *p_num = param_num;
  return pData;
}

//--------------------------------------------------------------
/**
 * @brief   パラメータ取得
 */
//--------------------------------------------------------------
const EventListData* EventListDataManager::GetMainList( u32 index ) const
{
  if( index < m_ListNumMain ) return &m_pListMain[index]; 

  GFL_ASSERT(0);
  return &m_pListMain[0]; 
} 

//--------------------------------------------------------------
/**
 * @brief このイベントが有効か？（分岐用。falseのイベントはリストに表示しない）
 *
 * @retval  true   有効
 * @retval  false  無効
 */
//--------------------------------------------------------------
bool EventListDataManager::IsEnableParam( EventListData::Param* pParam ) const
{
  return true;  //現状無条件 150830
}

//--------------------------------------------------------------
/**
 * @brief 　カレントのメインイベントIndex
 *
 * @retval  -1    カレントがない（全クリア）
 * @retval  0-    MainListのIndex
 */
//--------------------------------------------------------------
s32 EventListDataManager::GetTargetIndexForMain( void ) const
{
  for( u32 i = 0; i < m_ListNumMain; i++)
  {
    if( m_pListMain[i].IsClear() == false ){
      //ターゲットイベントがオープンされていない状態もチェック
      if( m_pListMain[i].IsActive() == false ){
        return -1;
      }
      return static_cast<s32>(i);
    }
  }
  return -1;
}


GFL_NAMESPACE_END(EventList)
GFL_NAMESPACE_END(Field)

