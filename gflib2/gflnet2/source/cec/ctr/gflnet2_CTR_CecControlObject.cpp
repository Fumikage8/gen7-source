// ============================================================================
/*
 * @file gflnet2_CTR_CecControlObject.cpp
 * @brief すれ違い通信の制御オブジェクトです
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecControlObject.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


CTR_CecControlObject::CTR_CecControlObject() :
  m_eCommandID( gflnet2::cec::NONE ),
  m_eMessageBoxType( gflnet2::cec::CORE_BOX )
{
}


CTR_CecControlObject::~CTR_CecControlObject()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  実行コマンドのIDを取得します。
  * @return 実行コマンドのID
  */
// --------------------------------------------------------------------------
CommandID CTR_CecControlObject::GetCommandID() const
{
  return m_eCommandID;
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージボックスのタイプを取得します。
  * @return メッセージボックスのタイプ
  */
// --------------------------------------------------------------------------
MessageBoxType CTR_CecControlObject::GetMessageBoxType() const
{
  return m_eMessageBoxType;
}


// --------------------------------------------------------------------------
/**
  * @brief  コマンドを実行する為のパラメータを設定します。
  * @param  eCommandID コマンドID
  * @param  eMessageBoxType メッセージボックスのタイプ
  */
// --------------------------------------------------------------------------
void CTR_CecControlObject::SetCecCommandExecuteParameters( CommandID eCommandID, MessageBoxType eMessageBoxType )
{
  m_eCommandID = eCommandID;
  m_eMessageBoxType = eMessageBoxType;
}


// --------------------------------------------------------------------------
/**
  * @brief  メッセージボックスを作成する為のパラメータを取得します。
  *         派生先のクラスで必要に応じて実装してください。
  * @return メッセージボックスを作成する為のパラメータ
  */
// --------------------------------------------------------------------------
const CecMessageBoxCreateCoreParam* CTR_CecControlObject::GetCecMessageBoxCreateCoreParam() const
{
  return NULL;
}


// --------------------------------------------------------------------------
/**
  * @brief  送信ボックスに設定する送信データを取得します。
  *         派生先のクラスで必要に応じて実装してください。
  * @return 送信ボックスに設定する送信データ
  */
// --------------------------------------------------------------------------
const CecSendPacket* CTR_CecControlObject::GetCecSendPacket() const
{
  return NULL;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif
