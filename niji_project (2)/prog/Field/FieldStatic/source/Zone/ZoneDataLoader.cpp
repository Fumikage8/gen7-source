//===================================================================
/**
 * @file    ZoneDataLoader.cpp
 * @brief   ゾーンデータ読み込みクラス
 * @author  saita_kazuki
 * @date    2015.04.02
 */
//===================================================================

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "./ZoneDataFormat.h"

#include "GameSys/include/GameManager.h"  // AsyncFileManager取得のため
#include "System/include/ArcIdGetter.h"
#include "Savedata/include/EventWork.h"

// zone_data
#include <arc_def_index/arc_def.h>
#include <arc_index/zone_data.gaix>
#include <niji_conv_header/field/zone/zone_id.h>

GFL_NAMESPACE_BEGIN( Field )

/**
 * @brief ゾーンに設定されているNG項目チェック用ワーク
 */
struct ZoneNGCheckWork
{
  u16   zoneID;           ///< ゾーンID
  s16   flag;             ///< フラグ
  bool  isCapture;        ///< 捕獲可能か
  bool  isEncount;        ///< エンカウント可能か
};

/**
 * @brief NG項目チェック用ワーク配列
 */
static const ZoneNGCheckWork sc_zoneNGCheckWork[] =
{
  // ゾーンID、フラグID、捕獲できるかどうか、エンカウントできるかどうか
  { Z_F0102H0101, SYS_FLAG_WATCH_ZONE_NG_NORMAL, false, false },  // しげみの洞窟 ノーマル試練
  { Z_F0102H0102, SYS_FLAG_WATCH_ZONE_NG_NORMAL, false, false },  // しげみの洞窟 ノーマル試練ぬし
  { Z_F0201H0303, SYS_FLAG_WATCH_ZONE_NG_WATER,  false, false },  // せせらぎのおか深部 水試練
  { Z_F0201H0302, SYS_FLAG_WATCH_ZONE_NG_WATER,  false, false },  // せせらぎのおか深部 水試練
  { Z_F0201H0301, SYS_FLAG_WATCH_ZONE_NG_WATER,  false, false },  // せせらぎのおか深部 水試練
  { Z_F0201H0401, SYS_FLAG_WATCH_ZONE_NG_GRASS,  false, false },  // シェードジャングル:エリア1 草試練
  { Z_F0201H0402, SYS_FLAG_WATCH_ZONE_NG_GRASS,  false, false },  // シェードジャングル:エリア2 草試練
  { Z_F0201H0403, SYS_FLAG_WATCH_ZONE_NG_GRASS,  false, false },  // シェードジャングル:エリア3 草試練
  { Z_F0201H0404, SYS_FLAG_WATCH_ZONE_NG_GRASS,  false, false },  // シェードジャングル:エリア4 草試練
  { Z_F0201H0405, SYS_FLAG_WATCH_ZONE_NG_GRASS,  false, false },  // シェードジャングル:エリア5 草試練
  { Z_F0201H0202, SYS_FLAG_WATCH_ZONE_NG_FIRE,   false, false },  // アーカラ火山公園:火試練
  { Z_F0301I1301, SYS_FLAG_WATCH_ZONE_NG_DENKI,  false, false },  // ホクラニ天文台：エントランス 電気試練
  { Z_F0301I1302, SYS_FLAG_WATCH_ZONE_NG_DENKI,  false, false },  // ホクラニ天文台：廊下 電気試練
  { Z_F0301I1303, SYS_FLAG_WATCH_ZONE_NG_DENKI,  false, false },  // ホクラニ天文台：所長室 電気試練
  { Z_F0301I1304, SYS_FLAG_WATCH_ZONE_NG_DENKI,  false, false },  // ホクラニ天文台：所長室 電気試練
  { Z_F0301H0501, SYS_FLAG_WATCH_ZONE_NG_GHOST,  false, false },  // 廃墟：試練クリア前 ゴースト試練
  { Z_F0301H0502, SYS_FLAG_WATCH_ZONE_NG_GHOST,  false, false },  // 廃墟：ぬしの間 ゴースト試練
  { Z_F0401H0701, SYS_FLAG_WATCH_ZONE_NG_DRAGON, false, false },  // 廃墟：試練クリア前 ドラゴン試練
};

  /**
 * @brief   コンストラクタ
 */
ZoneDataLoader::ZoneDataLoader() :
  m_pHeap( NULL),
  m_pAsyncFileManager( NULL),
  m_pZoneDataBuff( NULL),
  m_zoneDataSize( 0),
  m_pZoneWorldIdTable( NULL),
  m_zoneWorldIdTableSize( 0)
#if PM_DEBUG
  , m_hioUseSetting( gfl2::fs::AsyncFileManager::HioUseSetting::SAME_AS_MANAGER)
#endif // PM_DEBUG
{
  m_pAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();


#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  {
    bool isExist = false;  // フルパスで有無を確認する
    gfl2::fs::PcUtil::CheckFileExist( &isExist, "R:/home/momiji/exec/data_ctr/debug_zonedata_hio.txt" );
    if( isExist )
    {
      m_hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO;
    }
  }
#endif // GFL_FS_HIO_ENABLE
#endif // GF_PLATFORM_CTR
#endif // PM_DEBUG
}

/**
 * @brief   デストラクタ
 */
ZoneDataLoader::~ZoneDataLoader()
{
}

/**
 * @brief   初期化
 * @param   pHeap 読み込みバッファ確保などに使用する
 */
void ZoneDataLoader::Initialize( gfl2::heap::HeapBase* pHeap)
{
  m_pHeap = pHeap;

  this->SyncOpenArcFile();
}

/**
 * @brief   終了
 */
void ZoneDataLoader::Terminate()
{
  if(m_pHeap){
    m_zoneDataSize = 0;
    GflHeapSafeFreeMemory( m_pZoneDataBuff);
    GflHeapSafeFreeMemory( m_pZoneWorldIdTable);

    this->SyncCloseArcFile();

    m_pHeap = NULL;
  }
}

/**
 * @brief   同期読み込み
 * @retval  true 成功
 * @retval  false 失敗
 */
bool ZoneDataLoader::SyncLoad()
{
  bool result[2];

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

  // 全ゾーンデータ
  this->SetArcFileLoadRequestCommon( &req);

  m_pAsyncFileManager->SyncArcFileLoadData( req);
  result[0] = this->CheckFileResult();

  // Zone WorldIDリスト
  this->SetArcFileLoadRequestWorldId( &req);
  m_pAsyncFileManager->SyncArcFileLoadData( req);
  result[1] = this->CheckFileResult();

  return result[0] && result[1];
}

/**
 * @brief   非同期読み込み開始
 * @retval  true 成功
 * @retval  false 失敗
 */
bool ZoneDataLoader::AsyncLoadStart()
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  this->SetArcFileLoadRequestCommon( &req);
  m_pAsyncFileManager->AddArcFileLoadDataReq( req);

  this->SetArcFileLoadRequestWorldId( &req);
  m_pAsyncFileManager->AddArcFileLoadDataReq( req);

  return true;
}

/**
 * @brief   非同期読み込み終了待ち
 */
bool ZoneDataLoader::WaitAsyncLoadFinished()
{
  if( this->IsArcFileOpenFinished())
  {
    return this->IsLoadFinished();
  }
  return false;
}

/**
 * @brief   読み込みが完了しているか
 * @retval  true 完了or読み込んでいない
 * @retval  false 実行中
 */
bool ZoneDataLoader::IsLoadFinished()
{

  bool result0 = false;
  bool result1 = false;

  // アーカイブのオープンを先にチェック
  if( m_pAsyncFileManager->IsArcFileOpenFinished( this->GetArcID() ) ){
    result0 = m_pAsyncFileManager->IsArcFileLoadDataFinished( &m_pZoneDataBuff);

    result1 = m_pAsyncFileManager->IsArcFileLoadDataFinished( reinterpret_cast<void**>(&m_pZoneWorldIdTable));
    if( result1 ){ result1 = false; }
  }


  return (result0 && result1);
}

/**
 * @brief   ZONE_DATA構造体のポインタを取得
 * @param   取得したいゾーンのID。zone_id.h参照
 * @return  指定ゾーンIDのZONE_DATA構造体のポインタ
 * @note    ここで返ってきたポインタを、ZoneDataAccessorにセットして使います。
 */
void* ZoneDataLoader::GetZoneData( ZoneID zoneID) const
{
  // @fix GFCHECK[842]:新規仕様箇所があるため、念のためNULLを返すように修正
  if (m_pZoneDataBuff && zoneID < (m_zoneDataSize / sizeof(ZONE_DATA)) )
  {
    c8* pSeek = reinterpret_cast<c8*>( m_pZoneDataBuff);
    pSeek += ( sizeof(ZONE_DATA) * zoneID );
    return pSeek;
  }
  GFL_ASSERT( 0);
  return NULL;
}

/**
 *  @brief  現在のゾーン総数を取得
 */
u32 ZoneDataLoader::GetZoneNum() const
{
  return (m_zoneDataSize / sizeof(ZONE_DATA));
}


/**
 * @brief   ゾーンが所属するワールドIDを取得
 */
WorldID ZoneDataLoader::GetZoneWorldId( ZoneID zoneID ) const
{
  GFL_ASSERT_MSG( zoneID < (m_zoneWorldIdTableSize / sizeof(u16)), "0x%x", m_zoneWorldIdTableSize);
  if( m_pZoneWorldIdTable)
  {
    return static_cast<WorldID>(m_pZoneWorldIdTable[zoneID]);
  }
  GFL_ASSERT( 0);
  return NULL;
}

/**
 * @brief   捕獲制限ゾーンかどうか？
 */
bool ZoneDataLoader::CheckCaptureNGZone( ZoneID zoneID, const Field::EventWork* pEventWork ) const
{
  if( zoneID >= (m_zoneWorldIdTableSize / sizeof(u16)) )
  {
    GFL_ASSERT_MSG( zoneID < (m_zoneWorldIdTableSize / sizeof(u16)), "0x%x", m_zoneWorldIdTableSize);
    return false;
  }
  for( int i = 0; i < GFL_NELEMS( sc_zoneNGCheckWork); i++ )
  {
    // ゾーンが一致したらチェックする
    if( sc_zoneNGCheckWork[i].zoneID == zoneID )
    {
      // フラグがON かつ 捕獲可否フラグがfalseなら捕獲制限ゾーン
      if( sc_zoneNGCheckWork[i].flag >= 0 &&
          this->CheckFlag( sc_zoneNGCheckWork[i].flag, pEventWork) &&
          sc_zoneNGCheckWork[i].isCapture == false
        )
      {
        return true;
      }
      return false;
    }
  }
  return false;
}

/**
 * @brief   エンカウント制限ゾーンかどうか？
 */
bool ZoneDataLoader::CheckEncountNGZone( ZoneID zoneID, const Field::EventWork* pEventWork ) const
{
  if( zoneID >= (m_zoneWorldIdTableSize / sizeof(u16)) )
  {
    GFL_ASSERT_MSG( zoneID < (m_zoneWorldIdTableSize / sizeof(u16)), "0x%x", m_zoneWorldIdTableSize);
    return false;
  }
  for( int i = 0; i < GFL_NELEMS( sc_zoneNGCheckWork); i++ )
  {
    // ゾーンが一致したらチェックする
    if( sc_zoneNGCheckWork[i].zoneID == zoneID )
    {
      // フラグがON かつ エンカウント可否フラグがfalseならエンカウント制限ゾーン
      if( sc_zoneNGCheckWork[i].flag >= 0 &&
          this->CheckFlag( sc_zoneNGCheckWork[i].flag, pEventWork) &&
          sc_zoneNGCheckWork[i].isEncount == false
        )
      {
        return true;
      }
      return false;
    }
  }
  return false;
}

//*******************************************************************
// private
//*******************************************************************

/**
 * @brief   読み込みリクエスト共通部分を設定
 * @param   pReq 設定するリクエスト
 */
void ZoneDataLoader::SetArcFileLoadRequestCommon( gfl2::fs::AsyncFileManager::ArcFileLoadDataReq* pReq)
{
  m_fsResult.Clear();

  pReq->arcId              = this->GetArcID();
  pReq->datId              = GARC_zone_data_zonetable_COMP;
  pReq->prio               = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  pReq->ppBuf              = &m_pZoneDataBuff;
  pReq->pBufSize           = &m_zoneDataSize;
  pReq->heapForBuf         = m_pHeap;                   // ファイルから読み込んだ内容を入れておくバッファを確保する
  pReq->align              = 4;
  pReq->heapForReq         = m_pHeap->GetLowerHandle(); // リクエスト終了まで一時的に使用される
  pReq->heapForCompressed  = m_pHeap->GetLowerHandle(); // 解凍時に一時的に使用される
  pReq->result             = &m_fsResult;
}

/**
 * @brief   オープンリクエスト共通部分を設定
 * @param   pReq 設定するリクエスト
 */
void ZoneDataLoader::SetArcFileOpenRequestCommon( gfl2::fs::AsyncFileManager::ArcFileOpenReq* pReq)
{
  m_fsResult.Clear();

  pReq->arcId        = this->GetArcID();
  pReq->prio         = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  pReq->heapForFile  = m_pHeap;                   // Closeするまで保持される
  pReq->heapForReq   = m_pHeap->GetLowerHandle(); // リクエスト終了まで一時的に保持される
  pReq->result       = &m_fsResult;
#if PM_DEBUG
  pReq->hioUseSetting = m_hioUseSetting;
#endif // PM_DEBUG
}

/**
 * @brief   クローズリクエスト共通部分を設定
 * @param   pReq 設定するリクエスト
 */
void ZoneDataLoader::SetArcFileCloseRequestCommon( gfl2::fs::AsyncFileManager::ArcFileCloseReq* pReq)
{
  m_fsResult.Clear();

  pReq->arcId       = this->GetArcID();
  pReq->prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  pReq->heapForReq  = m_pHeap->GetLowerHandle();  // リクエスト終了まで使用する
  pReq->result      = &m_fsResult;
}

/**
 * @brief   読み込みリクエスト共通部分を設定
 * @param   pReq 設定するリクエスト
 */
void ZoneDataLoader::SetArcFileLoadRequestWorldId( gfl2::fs::AsyncFileManager::ArcFileLoadDataReq* pReq)
{
  m_fsResult.Clear();

  pReq->arcId              = this->GetArcID();
  pReq->datId              = GARC_zone_data_zone_worldid_BIN;
  pReq->prio               = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  pReq->ppBuf              = reinterpret_cast<void**>(&m_pZoneWorldIdTable);
  pReq->pBufSize           = &m_zoneWorldIdTableSize;
  pReq->heapForBuf         = m_pHeap;                   // ファイルから読み込んだ内容を入れておくバッファを確保する
  pReq->align              = 4;
  pReq->heapForReq         = m_pHeap->GetLowerHandle(); // リクエスト終了まで一時的に使用される
  pReq->heapForCompressed  = NULL; // 解凍時に一時的に使用される
  pReq->result             = &m_fsResult;
}

/**
 * @brief   同期ARCファイルオープン
 * @retval  true 成功
 * @retval  false 失敗
 */
bool ZoneDataLoader::SyncOpenArcFile()
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  this->SetArcFileOpenRequestCommon( &req);

  m_pAsyncFileManager->SyncArcFileOpen( req);

  return this->CheckFileResult();
}

/**
 * @brief   同期ARCファイルクローズ
 * @retval  true 成功
 * @retval  false 失敗
 */
bool ZoneDataLoader::SyncCloseArcFile()
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  this->SetArcFileCloseRequestCommon( &req);

  m_pAsyncFileManager->SyncArcFileClose( req);

  return this->CheckFileResult();
}

/**
 * @brief   非同期ARCファイルオープン開始
 * @retval  true 成功
 * @retval  false 失敗
 */
bool ZoneDataLoader::AsyncOpenArcFileStart()
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  this->SetArcFileOpenRequestCommon( &req);

  m_pAsyncFileManager->AddArcFileOpenReq( req);

  return true;
}

/**
 * @brief   非同期ARCファイルクローズ開始
 * @retval  true 成功
 * @retval  false 失敗
 */
bool ZoneDataLoader::AsyncCloseArcFileStart()
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  this->SetArcFileCloseRequestCommon( &req);

  m_pAsyncFileManager->AddArcFileCloseReq( req);

  return true;
}

/**
 * @brief   ARCファイルがオープンが完了したか
 * @retval  true 完了orリクエストしていない
 * @retval  false 実行中
 */
bool ZoneDataLoader::IsArcFileOpenFinished()
{
  return m_pAsyncFileManager->IsArcFileOpenFinished( this->GetArcID());
}

/**
 * @brief   ARCファイルがクローズが完了したか
 * @retval  true 完了orリクエストしていない
 * @retval  false 実行中
 */
bool ZoneDataLoader::IsArcFileCloseFinished()
{
  return m_pAsyncFileManager->IsArcFileCloseFinished( this->GetArcID());
}

/**
 * @brief   ARCIDの取得
 * @return  zone_data.garcのARCID
 */
gfl2::fs::ArcFile::ARCID ZoneDataLoader::GetArcID()
{
  return NIJI_ARCID_NUM( ARCID_ZONE_DATA );
}

/**
 * @brief   ファイルリクエスト結果のチェック
 * @retval  true 成功
 * @retval  false 失敗。ログを出力してASSERT
 */
bool ZoneDataLoader::CheckFileResult()
{
  if( m_fsResult.IsSuccess() == false)
  {
    m_fsResult.Print();
    GFL_ASSERT( 0);
    return false;
  }
  return true;
}

/**
 * @brief フラグチェック
 */
bool ZoneDataLoader::CheckFlag( s16 flag, const Field::EventWork* pEventWork) const
{
  if( EventWork::CheckIDFlagRenge( flag) == false)
  {
    return false;
  }

  if( flag != 0)
  {
    if( pEventWork->CheckEventFlag( flag))
    {
      return true;
    }
  }

  return false;
}

GFL_NAMESPACE_END( Field )
