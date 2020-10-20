//======================================================================
/**
 * @file  PGLMediaCreator.cpp
 *
 * @author  suzuki_hisanao
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#include <NetStatic/NetLib/include/PGL/PGLMediaCreator.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(pgl)

//--------------------------------------------------------------------------------------------
/**
* @brief コンストラクタ
*/
//--------------------------------------------------------------------------------------------
PGLMediaCreator::PGLMediaCreator( void )
{
  m_isIconCreated    = false;
  m_isGameSyncSignIn = false;
  m_isCreateIconMgr  = false;
  m_isCameraCreated  = false;
  m_nexUniqueId      = 0;
  //
  //m_playerIconMgr = NULL;
  //m_playerIconTex = NULL;
  //m_playerIcon    = NULL;
  //
  m_iconData      = NULL;
  //
  m_seq           = E_SEQ_CREATE_NONE;
  //
  m_cameraDate    = NULL;
  //m_cameraSave    = NULL;
}


//--------------------------------------------------------------------------------------------
/**
* @brief デストラクタ
*/
//--------------------------------------------------------------------------------------------
PGLMediaCreator::~PGLMediaCreator()
{
}


//--------------------------------------------------------------------------------------------
/**
* @brief 初期化処理（ブロック関数）
*
* @param heap        データ生成に利用されるヒープ
* @param devHeap     データ生成に利用されるデバイスヒープ
* @param fileReadMgr ファイルリードマネージャ
* @param myStatus    プレイヤーステータス
* @param iconTex     外部で生成されたアイコンを使用する際に指定する
*
* @return 成功ならtrue、失敗したならfalseを返却
*/
//--------------------------------------------------------------------------------------------
bool PGLMediaCreator::Initialize( gfl2::heap::HeapBase *heap, gfl2::heap::HeapBase *devHeap,
                                  //gfl2::fs::FileReadManager* fileReadMgr,
                                  const Savedata::MyStatus* myStatus,
                                  bool isGameSyncSignIn
                                  //xy_system::player_icon::PlayerIconManager* iconMgr 
                                  )
{
  /*

  GameSys::GameManager*  gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  // テクスチャデータなので128byteアライメント
  if( m_iconData == NULL )
  {
    m_iconData = (u8*)GflHeapAllocMemoryAlign( devHeap, IMAGE_SIZE_RGB5551_64_64, 128 );
  }

  // 自分でアイコンを生成
  if( iconMgr == NULL )
  {
    if( m_playerIconMgr == NULL )
    {
      m_playerIconMgr = GFL_NEW(heap) xy_system::player_icon::PlayerIconManager();
      m_playerIconMgr->Create( fileReadMgr, heap, 1, 1 );
      m_isCreateIconMgr = true;
    }
  }
  else
  {
    if( m_playerIconMgr == NULL )
    {
      m_playerIconMgr   = iconMgr;
      m_isCreateIconMgr = false;;
    }
  }

  if( m_playerIconTex == NULL ){
    m_playerIconTex = GFL_NEW(heap) common::resource::PlayerIconTex( heap, devHeap, 1 );
  }
  if( m_playerIcon == NULL )
  {
    m_playerIcon = GFL_NEW(heap) app::ui::UIPlayerIcon( heap, devHeap, NULL, m_playerIconMgr, 0, 0, 0 );

    //xy_system::dress_up::SaveFashionBuffer fashionBuffer;
    //myStatus->GetDressUpData( &fashionBuffer );
    //netapp::pss::PssPersonalData* pMydata = GFL_SINGLETON_INSTANCE(netapp::pss::PssPersonalDataManager)->GetMyData();
    m_playerIcon->ReplacePlayerIconTexture( myStatus->GetPssIconForStranger() , m_playerIconTex->GetImageAddress() );
    //m_playerIcon->ReplacePlayerIconTexture( fashionBuffer, m_playerIconTex->GetImageAddress() );
  }

  if( m_cameraDate == NULL )
  {
    m_cameraDate = GFL_NEW(heap) gfl2::system::Date();
  }

  if( m_cameraSave == NULL )
  {
    m_cameraSave = GFL_NEW(heap) extSavedata::CameraExtSaveData( heap );
  }

  m_nexUniqueId      = myStatus->GetNexUniqueID();
  m_seq              = E_SEQ_CREATE_ICON;
  m_isGameSyncSignIn = isGameSyncSignIn;

  */

  return true;
}


//--------------------------------------------------------------------------------------------
/**
* @brief 終了処理
*/
//--------------------------------------------------------------------------------------------
void PGLMediaCreator::Finalize( void )
{
  if( m_iconData != NULL)
  {
    GflHeapFreeMemory( m_iconData );
    m_iconData = NULL;
  }
  if( m_isCreateIconMgr )
  {
    //GFL_SAFE_DELETE( m_playerIconMgr );
  }
  //GFL_SAFE_DELETE( m_playerIcon    );
  //GFL_SAFE_DELETE( m_playerIconTex );
  GFL_SAFE_DELETE( m_cameraDate    );
  //GFL_SAFE_DELETE( m_cameraSave    );
}


//--------------------------------------------------------------------------------------------
/**
* @brief 更新処理
*/
//--------------------------------------------------------------------------------------------
bool PGLMediaCreator::Update( void )
{
  //if( m_playerIconMgr != NULL ){
  //  m_playerIconMgr->Update();
  //}
/*
  switch(m_seq)
  {
    case  E_SEQ_CREATE_ICON:
      if( !m_playerIcon->IsLoading() )
      {
        ConvertBlock2LinierIcon();
        m_seq = E_SEQ_CONVERT_ICON_B2L;
      }
      break;

    case  E_SEQ_CONVERT_ICON_B2L:
      ResizeIcon();
      m_seq = E_SEQ_LOAD_CAMERA;
      break;

    case  E_SEQ_LOAD_CAMERA:
      {
        m_cameraSave->LoadCameraData( &m_cameraData );
        m_seq = E_SEQ_WAIT_CAMERA;
      }
      break;

    case  E_SEQ_WAIT_CAMERA:
      {
        AnalyzeCameraData();
        m_seq = E_SEQ_FINISH;
      }
      break;

    default:
      return false;
  }*/

  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ＰＧＬへのサインイン状態の変更
 *
 * @param isSignIn サインイン状態に設定するならtrueを指定
 */
//--------------------------------------------------------------------------------------------
void PGLMediaCreator::ChageGameSyncSignIn( bool isSignIn )
{
  m_isGameSyncSignIn = isSignIn;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief ブロックフォーマットのアイコンをリニアフォーマットに変更
 */
//--------------------------------------------------------------------------------------------
void  PGLMediaCreator::ConvertBlock2LinierIcon( void )
{
 /* u32 width  = 64;
  u32 height = 64;

  nngxFlush3DCommand();
  nngxAddB2LTransferCommand( m_playerIconTex->GetImageAddress(), width, height, gfl2::grp::RENDER_COLOR_FORMAT_RGB5_A1,
                             m_iconData,                         width, height, gfl2::grp::RENDER_COLOR_FORMAT_RGB5_A1,
                             NN_GX_ANTIALIASE_NOT_USED, GL_FALSE, 8 );*/
}


//--------------------------------------------------------------------------------------------
/**
 * @brief カメラデータの解析
 */
//--------------------------------------------------------------------------------------------
void PGLMediaCreator::AnalyzeCameraData( void )
{
  //m_isCameraCreated = false;

  //if( m_cameraSave->IsSuccessLoad() )
  //{
  //  if( m_cameraData.pglSendFlag != 0 )
  //  {//送信済みの写真
  //    return;
  //  }

  //  // 日付設定
  //  m_cameraDate->SetYear( 2000+m_cameraData.dateTime[0] );
  //  m_cameraDate->SetMonth( m_cameraData.dateTime[1] );
  //  m_cameraDate->SetDay( m_cameraData.dateTime[2] );
  //  m_cameraDate->SetHour(0);
  //  m_cameraDate->SetMinute(0);
  //  m_cameraDate->SetSecond(0);

  //  m_isCameraCreated = true;
  //}
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 64x64アイコンデータを40x40のデータに変換
 */
//--------------------------------------------------------------------------------------------
void PGLMediaCreator::ResizeIcon( void )
{
  u16*  src = reinterpret_cast<u16*>(m_iconData);
  u16*  dst = reinterpret_cast<u16*>(m_iconData);

  for( u32 y = 0; y < 40; y++ )
  {
    ::std::memcpy( dst, src, 40*sizeof(u16) );    // 1LINEのデータ転送
    src += 64;
    dst += 40;
  }
  m_isIconCreated = true;
}


//--------------------------------------------------------------------------------------------
/**
* @brief プレイヤーアイコンデータ(40x40, RGB5A1)を取得
*
* @return プレイヤーアイコンデータへのポインタ。存在しない場合はNULLを返却。
*/
//--------------------------------------------------------------------------------------------
const void* PGLMediaCreator::GetPlayerIconData( void ) const
{
  if( !m_isIconCreated ){
    return NULL;
  }
  return m_iconData;
}



//--------------------------------------------------------------------------------------------
/**
* @brief カメラデータを取得
*
* @return カメラデータへのポインタ。存在しない場合はNULLを返却
*/
//--------------------------------------------------------------------------------------------
const void* PGLMediaCreator::GetCameraData( void ) const
{
  //if( !m_isGameSyncSignIn || !m_isCameraCreated ){
  //  return NULL;   // データ未生成または未サインイン
  //}

  //if( m_cameraData.pglSendFlag == 1 )
  //{//送信済み
  //  return NULL;
  //}

  //if( m_cameraData.isReal == 1 )
  //{//実写だったら送らない
  //  return NULL;
  //}

  //return &m_cameraData;

  return NULL;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief カメラ撮影時間を取得
 *
 * @return 日付情報
 */
//--------------------------------------------------------------------------------------------
gfl2::system::Date* PGLMediaCreator::GetDateOfCameraData( void ) const
{
  if( !m_isGameSyncSignIn || !m_isCameraCreated ){
    return NULL;   // データ未生成または未サインイン
  }

  return m_cameraDate;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief 写真データ送信済み情報をセーブ
 */
//--------------------------------------------------------------------------------------------
void PGLMediaCreator::StartCameraSave( void )
{
  //if( m_cameraData.isCreated && m_isCameraCreated )
  //{//作成済み
  //  m_cameraData.pglSendFlag = 1;//送信済みにする
  //  m_cameraSave->SaveCameraData( &m_cameraData );
  //}
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 送信済みフラグを巻き戻し
 */
//--------------------------------------------------------------------------------------------
void PGLMediaCreator::RollbackCameraSave( void )
{
  //if( m_cameraData.isCreated && m_isCameraCreated && m_cameraData.pglSendFlag == 1 )
  //{//作成済み
  //  m_cameraData.pglSendFlag = 0;//未送信にする
  //  m_cameraSave->SaveCameraData( &m_cameraData );
  //  m_isCameraCreated = false;
  //}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief 写真データ送信完了（以後はrollbackしても巻き戻らない）
 */
//--------------------------------------------------------------------------------------------
void PGLMediaCreator::CommitCameraSave( void )
{
  m_isCameraCreated = false;
}


GFL_NAMESPACE_END(pgl)
GFL_NAMESPACE_END(netapp)
#endif