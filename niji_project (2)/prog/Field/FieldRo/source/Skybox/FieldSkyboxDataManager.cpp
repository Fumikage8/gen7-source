//======================================================================
/**
 * @file FieldSkyboxDataManager.cpp
 * @date 2016/2/19
 * @author saita_kazuki
 * @brief 天球データ管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Skybox/FieldSkyboxDataManager.h"
#include "Field/FieldRo/include/Skybox/FieldSkyboxPosDataAccessor.h"

#include <debug/include/gfl2_DebugPrint.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

#include "System/include/ArcIdGetter.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/skybox_data.gaix>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief コンストラクタ
 */
DataManager::DataManager()
  : m_desc()
  , m_seq( 0)
  , m_pPosData( NULL)
  , m_isPosDataExist( false)
  , m_posY( 0.0f)
  , m_isFixPos( false )
  , m_fixPos( gfl2::math::Vector3::GetZero() )
{
}

/**
 * @brief デストラクタ
 */
DataManager::~DataManager()
{
}

/**
 * @brief 初期化
 */
void DataManager::Initialize( const Description& desc)
{
  m_seq = 0;
  m_desc = desc;

  this->AddOpenReq();
}

/**
 * @brief 初期化待ち
 * @retval true 完了
 * @retval true 実行中
 */
bool DataManager::WaitInitialize()
{
  enum Sequence
  {
    WAIT_OPEN,
    WAIT_LOAD,
    WAIT_CLOSE,
    END,
  };

  switch( m_seq)
  {
  case WAIT_OPEN:
    if( m_desc.pAsyncFileManager->IsArcFileOpenFinished( NIJI_ARCID_NUM( ARCID_SKYBOX_DATA)) )
    {
      this->AddAsyncLoad(
        GARC_skybox_data_skybox_pos_BIN,
        &m_pPosData,
        NULL,
        4
        );
      m_seq++;
    }
    break;

  case WAIT_LOAD:
    if( m_desc.pAsyncFileManager->IsArcFileLoadDataFinished( &m_pPosData))
    {
      this->AddCloseReq();
      m_seq++;
    }
    break;

  case WAIT_CLOSE:
    if( m_desc.pAsyncFileManager->IsArcFileCloseFinished( NIJI_ARCID_NUM( ARCID_SKYBOX_DATA)) )
    {
      this->InitializePosData();
      m_seq++;

      return true;
    }
    break;

  case END:
    return true;
    // break;
  }

  return false;
}

/**
 * @brief 破棄
 * @retval true 完了
 * @retval true 実行中
 */
bool DataManager::Terminate()
{
  GflHeapSafeFreeMemory( m_pPosData);

  m_seq = 0;
  m_isPosDataExist = false;
  m_posY = 0.0f;
  m_isFixPos        = false;
  m_fixPos          = gfl2::math::Vector3::GetZero();

  return true;
}

/**
 * @brief 位置データが存在するか
 * @retval true 存在する
 * @retval false 存在しない
 */
bool DataManager::IsExistPosData()
{
  return m_isPosDataExist;
}

/**
 * @brief 位置変換
 * @return 位置情報に合わせた位置
 */
gfl2::math::Vector3 DataManager::ConvertPosition( const gfl2::math::Vector3& sourcePos)
{
  gfl2::math::Vector3 pos = sourcePos;
  pos.y = m_posY;
  return pos;
}

/**
 * @brief 位置を固定中か
 * @retval true 固定中
 * @retval false 固定中ではない
 */
bool DataManager::IsFixPosition() const
{
  return m_isFixPos;
}

/**
 * @brief 固定位置を取得
 * @return 位置情報に合わせた位置
 */
const gfl2::math::Vector3& DataManager::GetFixPosition() const
{
  GFL_ASSERT( this->IsFixPosition() );
  return m_fixPos;
}

/**
 * @brief 固定位置の登録
 * @param fixPos 固定したい位置
 */
void DataManager::RegistFixPosition( const gfl2::math::Vector3& fixPos )
{
  m_fixPos    = fixPos;
  m_isFixPos  = true;
}

/**
 * @brief 固定位置の解除
 */
void DataManager::UnRegistFixPosition()
{
  m_fixPos    = gfl2::math::Vector3::GetZero();
  m_isFixPos  = false;
}

/**
 * @brief 非同期オープンリクエスト
 */
void DataManager::AddOpenReq()
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.arcId        = NIJI_ARCID_NUM( ARCID_SKYBOX_DATA);
  req.prio         = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  req.heapForFile  = m_desc.pTempHeap;  // Closeするまで保持される
  req.heapForReq   = m_desc.pTempHeap;  // リクエスト終了まで一時的に保持される

  m_desc.pAsyncFileManager->AddArcFileOpenReq( req);
}

/**
 * @brief 非同期クローズリクエスト
 */
void DataManager::AddCloseReq()
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId        = NIJI_ARCID_NUM( ARCID_SKYBOX_DATA);
  req.prio         = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  req.heapForReq   = m_desc.pTempHeap;  // リクエスト終了まで一時的に保持される

  m_desc.pAsyncFileManager->AddArcFileCloseReq( req);
}

/**
 * @brief 非同期読み込みリクエスト
 * @param datId garcデータID
 * @param ppBuff 確保した読み込みバッファが渡されるポインタ
 * @param pSize サイズのポインタ
 * @param align アライメント
 */
void DataManager::AddAsyncLoad( u32 datId, void** ppBuff, u32* pSize, u32 align)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId              = NIJI_ARCID_NUM( ARCID_SKYBOX_DATA);
  req.datId              = datId;
  req.prio               = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL + 1;
  req.ppBuf              = ppBuff;
  req.pBufSize           = pSize;
  req.heapForBuf         = m_desc.pHeap;                   // ファイルから読み込んだ内容を入れておくバッファを確保する
  req.align              = align;
  req.heapForReq         = m_desc.pTempHeap;                   // リクエスト終了まで一時的に使用される

  m_desc.pAsyncFileManager->AddArcFileLoadDataReq( req);
}

/**
 * @brief 位置データ初期化
 */
void DataManager::InitializePosData()
{
  if( m_pPosData == NULL)
  {
    GFL_ASSERT( 0);
    return;
  }

  PosDataAccessor accessor( m_pPosData);

  m_isPosDataExist  = accessor.IsExistData( m_desc.worldID);
  m_posY            = accessor.GetPositionY( m_desc.worldID);
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( Field )

