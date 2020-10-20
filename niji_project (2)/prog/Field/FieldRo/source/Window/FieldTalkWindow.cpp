//==============================================================================
/**
 * @file        FieldTalkWindow.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/04/21(火) 13:11:50
 */
//==============================================================================

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "Field/FieldRo/include/Window/FieldTalkWindow.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/msgwin_test.gaix>
#include <arc_index/fieldwindow.gaix>

#include <arc_index/script_message.gaix>
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"


/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldWindow )

enum{
  FIELDWIDOW_ARC_ID = ARCID_FIELDWINDOW,
  FIELDWINDOW_DAT_ID = GARC_fieldwindow_fieldwindow_applyt_COMP,
};

//------------------------------------------------------------------------------
/**
 * @func        FieldTalkWindow
 * @brief       コンストラクタ.
 * @author      N.Takeda
 * @date        2015/04/30(木) 16:16:55
 *
 * @param[in,out] Field::FieldWindow::FieldWindowManager* _pFieldWindowManager  .
 */
//------------------------------------------------------------------------------
FieldTalkWindow::FieldTalkWindow( Field::Fieldmap *_pFieldmap )
 : m_pTalkWindow( NULL )
 , m_pHeap( NULL )
 , m_pJoinFestaPlayerName( NULL )
 , m_uArcId( print::GetMessageArcId_Script() )
 , m_uGaixNo( 0 )
 , m_uMsgId( 0 )
 , m_pMsgData( NULL )
 , m_cPos()
 , m_eTalkWindowType( App::Tool::WIN_TYPE_SYS )
 , m_eMsgSpeed( Savedata::ConfigSave::MSG_SPEED_MAX )
 , m_iTime( 0 )
 , m_pFieldmap( _pFieldmap )
 , m_pFieldWindowManager( _pFieldmap->GetFieldWindowManager() )
 , m_ReadSeq( 0 )
 , m_pMsgBuff( NULL )
 , m_pArcReadBuff( NULL )
 , m_bDecide( false )
 , m_isMsgDataCreate( false )
 , m_isMsgBufferCreate( false )
 , m_iMouseId( 0 )
 , m_ResSize( 0 )
#ifdef DEBUG_MSGLABEL
 , m_pDebHeap( NULL )
 , m_pArcDebLabelBuff( NULL )
 , m_uArcDebLabelSize( 0 )
#endif
{
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
#ifdef DEBUG_MSGLABEL
  m_pDebHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
#endif

  m_pJoinFestaPlayerName = GFL_NEW( m_pHeap ) gfl2::str::StrBuf( ( ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ), m_pHeap );
}

//------------------------------------------------------------------------------
/**
 * @func        FieldTalkWindow
 * @brief       コンストラクタ(デバッグで使用).
 * @author      N.Takeda
 * @date        2015/06/02(火) 13:24:20
 *
 * @param[in,out] Field::FieldWindow::FieldWindowManager* _pFieldWindowManager  .
 */
//------------------------------------------------------------------------------
FieldTalkWindow::FieldTalkWindow( Field::FieldWindow::FieldWindowManager *_pFieldWindowManager )
 : m_pTalkWindow( NULL )
 , m_pHeap( NULL )
 , m_pJoinFestaPlayerName( NULL )
 , m_uArcId( print::GetMessageArcId_Script() )
 , m_uGaixNo( 0 )
 , m_uMsgId( 0 )
 , m_pMsgData( NULL )
 , m_cPos()
 , m_eTalkWindowType( App::Tool::WIN_TYPE_SYS )
 , m_eMsgSpeed( Savedata::ConfigSave::MSG_SPEED_MAX )
 , m_iTime( 0 )
 , m_pFieldmap( NULL )
 , m_pFieldWindowManager( _pFieldWindowManager )
 , m_ReadSeq( 0 )
 , m_pMsgBuff( NULL )
 , m_pArcReadBuff( NULL )
 , m_bDecide( false )
 , m_isMsgDataCreate( false )
 , m_isMsgBufferCreate( false )
 , m_iMouseId( 0 )
 , m_ResSize( 0 )
#ifdef DEBUG_MSGLABEL
 , m_pDebHeap( NULL )
 , m_pArcDebLabelBuff( NULL )
 , m_uArcDebLabelSize( 0 )
#endif
{
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
#ifdef DEBUG_MSGLABEL
  m_pDebHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
#endif

  m_pJoinFestaPlayerName = GFL_NEW( m_pHeap ) gfl2::str::StrBuf( ( ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ), m_pHeap );
}


//------------------------------------------------------------------------------
/**
 * @func        ~FieldTalkWindow
 * @brief       デストラクタ.
 * @author      N.Takeda
 * @date        2015/04/30(木) 17:48:56
 */
//------------------------------------------------------------------------------
FieldTalkWindow::~FieldTalkWindow()
{
  if( m_pTalkWindow ){
    m_pTalkWindow->RemoveFromSuperView();
    m_pTalkWindow->End();
  }
  GFL_DELETE m_pTalkWindow;

  if( m_isMsgDataCreate ){
    GFL_SAFE_DELETE(m_pMsgData);
  }
  if( m_isMsgBufferCreate ){
    GflHeapFreeMemory(m_pMsgBuff);
  }

  GflHeapFreeMemory( m_pArcReadBuff );
#ifdef DEBUG_MSGLABEL
  GflHeapFreeMemory( m_pArcDebLabelBuff );
#endif  // DEBUG_MSGLABEL
  m_appUtilHeap.Delete();

  GFL_SAFE_DELETE( m_pJoinFestaPlayerName );
}


//------------------------------------------------------------------------------
/**
 * @func        InitMsgData(バッファ引渡しバージョン)
 * @brief       デストラクタ.
 * @author      N.Takeda
 * @date        2015/04/30(木) 17:48:56
 */
//------------------------------------------------------------------------------
void FieldTalkWindow::InitMsgData( void * pMsgBuf, const u32 arcid, const u32 gaixno )
{
  m_pMsgData = GFL_NEW( m_pHeap )gfl2::str::MsgData( pMsgBuf, m_pHeap );
  m_uArcId = arcid;
  m_uGaixNo = gaixno;
  m_isMsgDataCreate = true;
}

void FieldTalkWindow::InitMsgData( const u32 arcid, const u32 gaixno )
{
  m_pMsgData = NULL; //後で作る
  m_uArcId = arcid;
  m_uGaixNo = gaixno;
  m_isMsgBufferCreate = true;
}
void FieldTalkWindow::InitMsgData( gfl2::str::MsgData* pMsgData, const u32 arcid, const u32 gaixno )
{
  m_pMsgData = pMsgData;
  m_uArcId = arcid;
  m_uGaixNo = gaixno;
}

//------------------------------------------------------------------------------
/**
 * @func        LoadResourceReq
 * @brief       リソース読み込みリクエスト
 * @author      Miyuki Iwasawa
 * @date        2016/04/17
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void FieldTalkWindow::LoadResourceReq()
{
  gfl2::fs::AsyncFileManager  *pAsyncFileMan = m_pFieldWindowManager->GetFileManager();

  //メッセージはもともと使っていたのと同じヒープから取るためにm_appUtilHeap->GetDeviceHeapを使う 161417
  gfl2::heap::HeapBase* pMsgHeap = m_appUtilHeap.GetDeviceHeap();


  //ウィンドウリソースの読み込み
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
    oreq.arcId       = FIELDWIDOW_ARC_ID;
    oreq.heapForFile = m_pHeap->GetLowerHandle();  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
    oreq.heapForReq  = m_pHeap->GetLowerHandle();  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
    pAsyncFileMan->AddArcFileOpenReq(oreq);
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    m_pArcReadBuff        = NULL;
    req.arcId             = FIELDWIDOW_ARC_ID;
    req.datId             = FIELDWINDOW_DAT_ID;
    req.ppBuf             = &m_pArcReadBuff;
    req.heapForBuf        = m_pHeap;                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
    req.heapForReq        = m_pHeap->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
    req.heapForCompressed = m_pHeap->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
    req.pBufSize          = &m_ResSize;
    req.align             = 128;
    pAsyncFileMan->AddArcFileLoadDataReq(req);
  }

  //メッセージリソースの読み込み
  if( m_isMsgBufferCreate == false ){
    return;
  }

  
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
    oreq.arcId       = m_uArcId;
    oreq.heapForFile = pMsgHeap->GetLowerHandle();  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
    oreq.heapForReq  = pMsgHeap->GetLowerHandle();  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
    pAsyncFileMan->AddArcFileOpenReq(oreq);
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    m_pMsgBuff            = NULL;
    req.arcId             = m_uArcId;
    req.datId             = m_uGaixNo;
    req.ppBuf             = &m_pMsgBuff;
    req.heapForBuf        = pMsgHeap;                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
    req.heapForReq        = pMsgHeap->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
    req.heapForCompressed = NULL;
    req.align             = 4;
    pAsyncFileMan->AddArcFileLoadDataReq(req);
  }
}

//------------------------------------------------------------------------------
/**
 * @func        LoadResourceWait
 * @brief       リソース読み込み待ち
 * @author      Miyuki Iwasawa
 * @date        2016/04/17
 *
 * @return      true 読み込み中
 */
//------------------------------------------------------------------------------
bool FieldTalkWindow::LoadResourceWait()
{
  gfl2::fs::AsyncFileManager  *pAsyncFileMan = m_pFieldWindowManager->GetFileManager();

  //メッセージはもともと使っていたのと同じヒープから取るためにm_appUtilHeap->GetDeviceHeapを使う 161417
  gfl2::heap::HeapBase* pMsgHeap = m_appUtilHeap.GetDeviceHeap();


  if( pAsyncFileMan->IsArcFileOpenFinished( FIELDWIDOW_ARC_ID ) == false )
  {
    return true;
  }
  if( pAsyncFileMan->IsArcFileLoadDataFinished(&m_pArcReadBuff) == false )
  {
    return true;
  }
  //メッセージリソースの読み込み
  if( m_isMsgBufferCreate == false ){
    return false;
  }

  if( pAsyncFileMan->IsArcFileOpenFinished( m_uArcId ) == false )
  {
    return true;
  }
  if( pAsyncFileMan->IsArcFileLoadDataFinished(&m_pMsgBuff) == false )
  {
    return true;
  }
  m_pMsgData = GFL_NEW( pMsgHeap )gfl2::str::MsgData( m_pMsgBuff, pMsgHeap );
  m_isMsgDataCreate = true;

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        UnoadResourceReq
 * @brief       リソース破棄
 * @author      Miyuki Iwasawa
 * @date        2016/04/17
 *
 * @return      true 読み込み中
 */
//------------------------------------------------------------------------------
void FieldTalkWindow::UnloadResourceReq()
{
  gfl2::fs::AsyncFileManager  *pAsyncFileMan = m_pFieldWindowManager->GetFileManager();

  //メッセージはもともと使っていたのと同じヒープから取るためにm_appUtilHeap->GetDeviceHeapを使う 161417
  gfl2::heap::HeapBase* pMsgHeap = m_appUtilHeap.GetDeviceHeap();

  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
    creq.arcId       = FIELDWIDOW_ARC_ID;
    creq.heapForReq  = m_pHeap->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。
    pAsyncFileMan->AddArcFileCloseReq(creq);
  }

  //メッセージリソースの読み込み
  if( m_isMsgBufferCreate == false ){
    return;
  }

  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
    creq.arcId       = m_uArcId;
    creq.heapForReq  = pMsgHeap->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。
    pAsyncFileMan->AddArcFileCloseReq(creq);
  }

}

//------------------------------------------------------------------------------
/**
 * @func        UnoadResourceWait
 * @brief       リソース破棄待ち
 * @author      Miyuki Iwasawa
 * @date        2016/04/17
 *
 * @return      true 読み込み中
 */
//------------------------------------------------------------------------------
bool FieldTalkWindow::UnloadResourceWait()
{
  gfl2::fs::AsyncFileManager  *pAsyncFileMan = m_pFieldWindowManager->GetFileManager();

  if( pAsyncFileMan->IsArcFileCloseFinished(FIELDWIDOW_ARC_ID) == false){
    return true;
  }
  //メッセージリソースの読み込み
  if( m_isMsgBufferCreate == false ){
    return false;
  }
  if( pAsyncFileMan->IsArcFileCloseFinished(m_uArcId) == false){
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        Update
 * @brief       更新.
 * @author      N.Takeda
 * @date        2015/04/30(木) 16:54:13
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
bool  FieldTalkWindow::Update()
{
  GFL_ASSERT( m_pFieldWindowManager );  ///< デバッグモードから呼び出しているなど( Test/TakedaTest/source/TestBagFrame.cpp(79): デバッグランチャーでのテスト起動ではFieldMapは作られないので、以下のメソッドを gamemanager をに追加する.
  if( !m_pFieldWindowManager )  return false;
  
  gfl2::fs::AsyncFileManager  *pAsyncFileMan = m_pFieldWindowManager->GetFileManager();

  switch(m_ReadSeq){  ///読み込み開始をここに記述
  case LOCAL_OPEN_START:
    // ローカルヒープ生成
    m_appUtilHeap.LocalHeapCreate( m_pHeap, m_pHeap, App::Tool::TalkWindow::SIZE_TALKWINDOW_APP, App::Tool::TalkWindow::SIZE_TALKWINDOW_APPDEV );

    //リソースロードリクエスト
    this->LoadResourceReq();
    m_ReadSeq = LOCAL_READ_WAIT;
    //break throw;

  case LOCAL_READ_WAIT:
    if( this->LoadResourceWait())
    {
      break;
    }
    {
      // ウィンドウパラメータ設定
      m_pTalkWindow = GFL_NEW( m_pHeap ) App::Tool::TalkWindow( &m_appUtilHeap, m_pFieldWindowManager->GetRenderingManager() );
      m_pTalkWindow->SetWindowType( m_eTalkWindowType );
      m_pTalkWindow->Setup(m_pArcReadBuff, Field::FieldWindow::FieldTalkWindow::MSG_BUFLEN );
      if( m_pMsgData != NULL ){
        m_pTalkWindow->InitMsgData( m_pMsgData );
      }else{
        m_pTalkWindow->InitMsgData( m_uArcId, m_uGaixNo );
      }
      
      if( m_pFieldmap ){  ///< Fieldmap はここでしか使わないので FieldScriptSystem を渡させて処理するほうがスマート.
        m_pTalkWindow->InitTrainerMsgData( m_pFieldmap->GetGameManager()->GetFieldScriptSystem()->GetMsgDataNameList() ); ///< Coverity.12299
        m_pTalkWindow->SetScriptWordSet( m_pFieldmap->GetGameManager()->GetFieldScriptSystem()->GetSystemWork()->GetWordSet() );
      }
      if( m_eTalkWindowType != App::Tool::WIN_TYPE_DIARY ){ ///< 日記には SetWindowPos が効かない(assertするので).
        m_pTalkWindow->UpdateTree();                        ///< SetWindowPos 実行前に必要.
        m_pTalkWindow->SetWindowPos( &m_cPos );
        
        if( m_eMsgSpeed != Savedata::ConfigSave::MSG_SPEED_MAX ) m_pTalkWindow->SetMessageSpeed( m_eMsgSpeed ); ///< 日記を弾く必要はないが大きく変更したくないのでここに置く.
      }
      m_pTalkWindow->SetTimer( m_iTime );

      if( m_eTalkWindowType == App::Tool::WIN_TYPE_JOIN_FESTA || m_eTalkWindowType == App::Tool::WIN_TYPE_BATTLE_FES )
      {
        m_pTalkWindow->SetMessage( m_uMsgId, m_pJoinFestaPlayerName );
      }
      else
      {
        m_pTalkWindow->SetMessage( m_uMsgId );
      }

      m_pTalkWindow->CallMsgWin();
      if( m_eTalkWindowType <= App::Tool::WIN_TYPE_TALK_LAST ){
        m_pTalkWindow->SetMsgCursorUserVisible( false, false );
      }

      m_pFieldWindowManager->GetUIView()->AddSubView( m_pTalkWindow );

      //データを読み込んで不要になったアーカイブのクローズ
      this->UnloadResourceReq();
      m_ReadSeq = LOCAL_CLOSE_WAIT;
    }
    break;
  case LOCAL_CLOSE_WAIT:
    if( this->UnloadResourceWait()){
      break;
    }
#ifdef DEBUG_MSGLABEL
    if( m_uArcId == print::GetMessageArcId_Debug() ||
        m_eTalkWindowType == App::Tool::WIN_TYPE_MONEY ){  //お金ウィンドウはラベル表示無し IDがずれるので
        m_ReadSeq = LOCAL_MAX;
        break;
    }
    m_ReadSeq = DEBUG_OPEN_START;
#else
    m_ReadSeq = LOCAL_MAX;
#endif  // DEBUG_MSGLABEL
    //break throw;

#ifdef DEBUG_MSGLABEL
  case DEBUG_OPEN_START:
    {
      if( m_uArcId == print::GetMessageArcId() ){
        m_uArcDebLabelId = ARCID_DEBUG_MSG_LABEL_COM;
      }else if( m_uArcId == print::GetMessageArcId_Script() ){
        m_uArcDebLabelId = ARCID_DEBUG_MSG_LABEL;
      }else{
        GFL_ASSERT(0); //,"不明なメッセージラベルデータ\n");
        m_ReadSeq = LOCAL_MAX;
        break;
      }

      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
        oreq.arcId       = m_uArcDebLabelId;
        oreq.heapForFile = m_pDebHeap->GetLowerHandle();  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
        oreq.heapForReq  = m_pDebHeap->GetLowerHandle();  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
        pAsyncFileMan->AddArcFileOpenReq(oreq);
      }
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
        req.arcId             = m_uArcDebLabelId;
        req.datId             = m_uGaixNo;
        req.ppBuf             = &m_pArcDebLabelBuff;
        req.pBufSize          = &m_uArcDebLabelSize;
        req.heapForBuf        = m_pDebHeap;                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
        req.heapForReq        = m_pDebHeap->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
        req.align             = 4;
        pAsyncFileMan->AddArcFileLoadDataReq(req);
      }
    }
    m_ReadSeq = DEBUG_READ_WAIT;
    //break throw
  case DEBUG_READ_WAIT:
    if( pAsyncFileMan->IsArcFileOpenFinished( m_uArcDebLabelId ) == false ||
        pAsyncFileMan->IsArcFileLoadDataFinished(&m_pArcDebLabelBuff) == false ){
      break;
    }
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
      creq.arcId       = m_uArcDebLabelId;
      creq.heapForReq  = m_pDebHeap->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。
      pAsyncFileMan->AddArcFileCloseReq(creq);
      m_ReadSeq = DEBUG_CLOSE_WAIT;
    }
    //break throw;
  case DEBUG_CLOSE_WAIT:
    if(pAsyncFileMan->IsArcFileCloseFinished(m_uArcDebLabelId) )
    {
      if( m_uMsgId < (m_uArcDebLabelSize/ARCDEBSTRINGSIZE) )
      {
        Field::FieldScript::FieldScriptSystem   *pSys = Field::FieldScript::FieldScriptSystem::GetInstance();
        char      *pMessage = (char*)m_pArcDebLabelBuff + m_uMsgId * ARCDEBSTRINGSIZE;
        
        //GFL_PRINT( "msgId: %s\n", pMessage );
#if defined(GF_PLATFORM_CTR)
        //nw::ut::snprintf( reinterpret_cast<char*>(pSys->GetDebugMsgLabelPtr()), ARCDEBSTRINGSIZE, ARCDEBSTRINGSIZE, "%S", pMessage );
        mbstowcs( pSys->GetDebugMsgLabelPtr(), pMessage, ARCDEBSTRINGSIZE );
        GFL_PRINT("Label = \"%s\"\n",pMessage);
#else
        swprintf( pSys->GetDebugMsgLabelPtr(), ARCDEBSTRINGSIZE, L"%S", pMessage );
        GFL_PRINT("Label = \"%s\"\n",pMessage);
#endif
      }
      GflHeapFreeMemory( m_pArcDebLabelBuff );
      m_pArcDebLabelBuff = NULL;
      m_ReadSeq = LOCAL_MAX;
    }
    break;
#endif  // DEBUG_MSGLABEL

  case LOCAL_MAX:
    if( m_pTalkWindow->GetStatus() == App::Tool::TalkWindow::WIN_STATUS_WIN_EXIT
     && m_pTalkWindow->GetStrWinResult() == gfl2::str::StrWin::RES_DONE )
    {
      m_bDecide = true;
      return true;
    }
    break;
  }
  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        CloseMsgWin
 * @brief       CloseMsgWin関数.
 * @author      N.Takeda
 * @date        2015/05/27(水) 20:00:07
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  FieldTalkWindow::CloseMsgWin()
{
#ifdef DEBUG_MSGLABEL
  Field::FieldScript::FieldScriptSystem   *pSys = Field::FieldScript::FieldScriptSystem::GetInstance();
  pSys->SetDebugMsgLabel( NULL );
  Debug::DebugTypes::s_pDrawUtilTextUpper->ClearDrawText();
#endif  // DEBUG_MSGLABEL
  GFL_ASSERT( m_pTalkWindow );
  if( m_pTalkWindow ) m_pTalkWindow->CloseMsgWin();
}

//------------------------------------------------------------------------------
/**
 * @func        End
 * @brief       End関数.
 * @author      N.Takeda
 * @date        2015/05/27(水) 20:00:05
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  FieldTalkWindow::End()
{
  GFL_ASSERT( m_pTalkWindow );
  if( m_pTalkWindow ) m_pTalkWindow->End();
}


//------------------------------------------------------------------------------
/**
 * @func        NextMessage
 * @brief       Nextメッセージ関数.
 * @author      N.Takeda
 * @date        2015/05/27(水) 20:00:04
 *
 * @param[in] u32 msgid .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  FieldTalkWindow::NextMessage( u32 msgid )
{
  m_uMsgId = msgid;
  GFL_ASSERT( m_pTalkWindow );
  if( m_pTalkWindow ){
    m_pTalkWindow->SetMessage( m_uMsgId );
    m_pTalkWindow->CallMsgWin();
  }
}

//------------------------------------------------------------------------------
/**
 * @func        SetMsgCursorUserVisible
 * @brief       MsgCursorUserVisibleのセット.
 * @author      N.Takeda
 * @date        2015/05/27(水) 20:00:02
 *
 * @param[in] bool  flg .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  FieldTalkWindow::SetMsgCursorUserVisible( bool flg )
{
 /**
 * @note  N.Takeda
 *  SetMsgCursorUserVisible の第一引数
 *  * @param   bUser 自分でやる場合 true
      TalkWindow側の挙動を必要としない場合trueでお願いします. システムに戻す場合にfalseを呼んでください
    
    との事なので 表示時(true)に制御を取り(true)、非表示時(false)に制御を戻す(false)ので第二引数と同じ値になるはず.
 */
  GFL_ASSERT( m_pTalkWindow );
  if( m_pTalkWindow ) m_pTalkWindow->SetMsgCursorUserVisible( flg, flg );
}

//------------------------------------------------------------------------------
/**
 * @func        SetVisible
 * @brief       会話ウインドウへの可視状態の設定
 *              不可視状態時は描画、操作通知ともに無効となる。
 * @author      saita_kazuki
 * @date        2015/11/17(火) 15:37
 *
 * @param[in] bool  isVisible .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void FieldTalkWindow::SetVisible( bool isVisible)
{
  if( m_pTalkWindow)
  {
    m_pTalkWindow->SetVisible( isVisible);
  }
}

//------------------------------------------------------------------------------
/**
 * @func        SetMsgCursorUserVisible
 * @brief       会話ウインドウの可視状態の取得.
 * @author      saita_kazuki
 * @date        2015/11/17(火) 15:37
 *
 * @param[in]   なし (none)
 *
 * @retval      true 表示ON
 * @retval      false 表示OFF
 */
//------------------------------------------------------------------------------
bool FieldTalkWindow::IsVisible()
{
  if( m_pTalkWindow)
  {
    return m_pTalkWindow->IsVisible();
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        SetTimerIconVisible
 * @brief       会話ウインドウのタイマーアイコン表示セット
 * @author      Miyuki Iwasawa
 * @date        2016/05/05
 *
 * @param[in]   visible_f trueで表示
 *
 * @retval      true 表示ON
 * @retval      false 表示OFF
 */
//------------------------------------------------------------------------------
void FieldTalkWindow::SetTimerIconVisible( bool visible_f )
{
  if( m_pTalkWindow)
  {
    m_pTalkWindow->SetVisibleTimerIcon( visible_f );
  }
}

//------------------------------------------------------------------------------
/**
 * @func        IsTimerIconClose
 * @brief       会話ウインドウのタイマーアイコン非表示アニメの修了検知
 * @author      Miyuki Iwasawa
 * @date        2016/05/05
 *
 * @param[in]   visible_f trueで表示
 *
 * @retval      true 表示ON
 * @retval      false 表示OFF
 */
//------------------------------------------------------------------------------
bool FieldTalkWindow::IsTimerIconClose()
{
  if( m_pTalkWindow)
  {
    m_pTalkWindow->IsTimerIconOutAnimeEnd();
  }
  return true;
}


GFL_NAMESPACE_END( FieldWindow )
GFL_NAMESPACE_END( Field )

