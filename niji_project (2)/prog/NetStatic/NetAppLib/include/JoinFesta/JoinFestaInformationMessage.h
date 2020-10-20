//============================================================================================
/**
 * @file  JoinFestaInformationMessage.h
 *
 * @author ichiraku_katsuhiko
 */
//============================================================================================
#ifndef __JOIN_FESTA_INFORMATION_MESSAGE_H__
#define __JOIN_FESTA_INFORMATION_MESSAGE_H__

// ライブラリ
#include <util/include/gfl2_List.h>

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"


namespace NetAppLib {
namespace JoinFesta {

class InformationMessageData
{
public:
  InformationMessageData();
  InformationMessageData( const InformationMessageData& data );
  virtual~InformationMessageData();
  InformationMessageData& operator = ( const InformationMessageData& data );

  void SetMessageID( u32 messageID ){ m_messageID = messageID; }
  u32 GetMessageID(){ return m_messageID; }

  void SetPersonalData( const NetAppLib::JoinFesta::JoinFestaPersonalData& personal ){ m_personalData.Copy( personal ); }
  NetAppLib::JoinFesta::JoinFestaPersonalData& GetPersonalData(){ return m_personalData; }

private:
  u32                                           m_messageID;       //!< メッセージID
  NetAppLib::JoinFesta::JoinFestaPersonalData   m_personalData;    //!< パーソナルデータ
};


/* ----------------------------------------------------------------------------------------- */
/**
* @brief JoinFestaインフォメーションメッセージ管理用クラス。<p>
*        JoinFesta通信で受信した他プレイヤー要求をテロップ表示させる。
*
* @note  本メッセージはJoinFestaPacketManagerクラスを経由して通知される。
*/
/* ----------------------------------------------------------------------------------------- */
class JoinFestaInformationMessage
{
public:
  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief  コンストラクタ
  *
  * @param[in]  heap        内部で使用するヒープ
  */
  /* ----------------------------------------------------------------------------------------- */
  JoinFestaInformationMessage( gfl2::heap::HeapBase* heap );

  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief  デストラクタ
  */
  /* ----------------------------------------------------------------------------------------- */
  virtual ~JoinFestaInformationMessage( void );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   メッセージを登録する
   *
   * @param[in]  personal 通信で受信したパーソナル情報
   *
   */
  /* ----------------------------------------------------------------------------------------- */
  void PushMessage( JoinFestaPersonalData& personal );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   メッセージを取得する
   *
   * @param[out]  messageData 取得したメッセージデータ
   *
   * @return      取得できたらtrue
   */
  /* ----------------------------------------------------------------------------------------- */
  bool PopMessage( InformationMessageData& messageData );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   メッセージが存在するか
   * @return  存在する場合true
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsMessageExist() const;

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   メンバ変数のリセット
   */
  /* ----------------------------------------------------------------------------------------- */
  void Reset();

private:

  void eraseMessage( const JoinFestaPersonalData& personal );

private:

  static const u32   MESSAGE_BUFF_SIZE    = 10;               //!< バッファサイズ

private:

  gfl2::util::List<InformationMessageData>    m_infoList;     //!< 登録メッセージリスト

};



}		// JoinFesta
}		// NetAppLib



#endif // __JOIN_FESTA_INFORMATION_MESSAGE_H__

