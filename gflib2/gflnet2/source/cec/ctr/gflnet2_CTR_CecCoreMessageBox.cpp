// ============================================================================
/*
 * @file gflnet2_CTR_CecCoreMessageBox.cpp
 * @brief すれ違い通信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecCoreMessageBox.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_ICecControlObjectRegister.h>
#include <gflnet2/include/cec/gflnet2_ICecMessageBoxEventListener.h>
#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


CTR_CecCoreMessageBox::CTR_CecCoreMessageBox( CTR_ICecControlObjectRegister* pCecControlObjectRegister ) :
  m_pCecControlObjectRegister( pCecControlObjectRegister ),
  m_pCecMessageBoxEventListener( NULL ),
  m_bIsCreateMessageBox( false ),
  m_bCreateMessageBoxResult( false ),
  m_bDeleteMessageBoxResult( false )
{
}


CTR_CecCoreMessageBox::~CTR_CecCoreMessageBox()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスが作成されているか調べます。
  * @retval true 作成されている
  * @retval false 作成されていない
  */
// --------------------------------------------------------------------------
bool CTR_CecCoreMessageBox::IsCreateMessageBox()
{
  SetCecCommandExecuteParameters( gflnet2::cec::IS_CREATE_MESSAGE_BOX, gflnet2::cec::CORE_BOX );
  m_pCecControlObjectRegister->QuickExecute( this );
  return m_bIsCreateMessageBox;
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスが作成されているか調べます。
  *         非同期で行われるので結果はイベントリスナーに通知されます。
  * @param  pEventListener 結果が通知されるイベントリスナーを指定します
  */
// --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::IsCreateMessageBoxAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pCecMessageBoxEventListener = pEventListener;
  SetCecCommandExecuteParameters( gflnet2::cec::IS_CREATE_MESSAGE_BOX, gflnet2::cec::CORE_BOX );
  m_pCecControlObjectRegister->RegisterControlObject( this );
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスが作成されている時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::OnIsCreateMessageBoxSuccess()
{
  m_bIsCreateMessageBox = true;
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスが作成されていない時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::OnIsCreateMessageBoxFailure()
{
  m_bIsCreateMessageBox = false;
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスを作成します。
  *         メッセージボックスを作成し、送信ボックスにデータを書き込む事によって、
  *         自動的にすれ違い通信が行われます。
  * @param  pCreateCoreParam メッセージボックスを作成する時のパラメータです
  * @retval true 作成に成功
  * @retval false 作成に失敗
  */
// --------------------------------------------------------------------------
bool CTR_CecCoreMessageBox::CreateMessageBox( const CecMessageBoxCreateCoreParam* pCreateCoreParam )
{
  gfl2::std::MemCopy( pCreateCoreParam, &m_CecMessageBoxCreateCoreParam, sizeof( CecMessageBoxCreateCoreParam ) );
  SetCecCommandExecuteParameters( gflnet2::cec::CREATE_MESSAGE_BOX, gflnet2::cec::CORE_BOX );
  m_pCecControlObjectRegister->QuickExecute( this );
  return m_bCreateMessageBoxResult;
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスを作成します。
  *         メッセージボックスを作成し、送信ボックスにデータを書き込む事によって、
  *         自動的にすれ違い通信が行われます。
  *         非同期で行われるので結果はイベントリスナーに通知されます。
  * @param  pCreateCoreParam メッセージボックスを作成する時のパラメータです
  * @param  pEventListener 結果が通知されるイベントリスナーを指定します
  */
// --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::CreateMessageBoxAsync( const CecMessageBoxCreateCoreParam* pCreateCoreParam, ICecMessageBoxEventListener* pEventListener )
{
  m_pCecMessageBoxEventListener = pEventListener;
  gfl2::std::MemCopy( pCreateCoreParam, &m_CecMessageBoxCreateCoreParam, sizeof( CecMessageBoxCreateCoreParam ) );
  SetCecCommandExecuteParameters( gflnet2::cec::CREATE_MESSAGE_BOX, gflnet2::cec::CORE_BOX );
  m_pCecControlObjectRegister->RegisterControlObject( this );
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスの作成に成功した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::OnCreateMessageBoxSuccess()
{
  m_bCreateMessageBoxResult = true;
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスの作成に失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::OnCreateMessageBoxFailure()
{
  m_bCreateMessageBoxResult = false;
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にあるメッセージボックスを削除します。
  *         メッセージボックスを削除するとすれ違い通信は行われなくなります。
  * @retval true 削除に成功
  * @retval false 削除に失敗
  */
// --------------------------------------------------------------------------
bool CTR_CecCoreMessageBox::DeleteMessageBox()
{
  SetCecCommandExecuteParameters( gflnet2::cec::DELETE_MESSAGE_BOX, gflnet2::cec::CORE_BOX );
  m_pCecControlObjectRegister->QuickExecute( this );
  return m_bDeleteMessageBoxResult;
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にあるメッセージボックスを削除します。
  *         メッセージボックスを削除するとすれ違い通信は行われなくなります。
  *         非同期で行われるので結果はイベントリスナーに通知されます。
  * @param  pEventListener 結果が通知されるイベントリスナーを指定します
  */
// --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::DeleteMessageBoxAsync( ICecMessageBoxEventListener* pEventListener )
{
  m_pCecMessageBoxEventListener = pEventListener;
  SetCecCommandExecuteParameters( gflnet2::cec::DELETE_MESSAGE_BOX, gflnet2::cec::CORE_BOX );
  m_pCecControlObjectRegister->RegisterControlObject( this );
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスの削除に成功した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::OnDeleteMessageBoxSuccess()
{
  m_bDeleteMessageBoxResult = true;
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体にメッセージボックスの削除に失敗した時にコールされるイベントハンドラです。
  */
// --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::OnDeleteMessageBoxFailure()
{
  m_bDeleteMessageBoxResult = false;
}


  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージボックスを作成する為のパラメータを取得します。
   * @return メッセージボックスを作成する為のパラメータ
   */
  // --------------------------------------------------------------------------
const CecMessageBoxCreateCoreParam* CTR_CecCoreMessageBox::GetCecMessageBoxCreateCoreParam() const
{
  return &m_CecMessageBoxCreateCoreParam;
}


  // --------------------------------------------------------------------------
  /**
   * @brief  非同期で実行されたコマンドが終了した時にコールされるイベントハンドラです。
   * @param  eResultCode 実行結果です
   *         この実行結果を参照してアプリ側へ成功、失敗の通知を行います。
   */
  // --------------------------------------------------------------------------
void CTR_CecCoreMessageBox::OnCommandResult( ResultCode eResultCode )
{
  CommandID eCommandID = GetCommandID();
  switch( eCommandID )
  {
    case gflnet2::cec::IS_CREATE_MESSAGE_BOX:
      m_pCecMessageBoxEventListener->OnIsCreateMessageBox( m_bIsCreateMessageBox );
    break;

    case gflnet2::cec::CREATE_MESSAGE_BOX:
      if( eResultCode == gflnet2::cec::SUCCESS )
      {
        m_pCecMessageBoxEventListener->OnCreateMessageBoxSuccess();
      }
      else
      {
        m_pCecMessageBoxEventListener->OnCreateMessageBoxFailure();
      }
    break;

    case gflnet2::cec::DELETE_MESSAGE_BOX:
      if( eResultCode == gflnet2::cec::SUCCESS )
      {
        m_pCecMessageBoxEventListener->OnDeleteMessageBoxSuccess();
      }
      else
      {
        m_pCecMessageBoxEventListener->OnDeleteMessageBoxFailure();
      }
    break;
  }
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif
