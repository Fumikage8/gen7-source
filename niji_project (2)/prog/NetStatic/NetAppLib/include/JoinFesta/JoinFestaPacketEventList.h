//============================================================================================
/**
 * @file   JoinFestaPacketEventList.h
 * @brief  ジョインフェスタの通信をトリガとして発生するイベントを管理するクラス
 * @author ichiraku_katsuhiko
 *
 * @note   PSSから移植
 */
//============================================================================================
#ifndef __JOIN_FESTA_PACKET_EVENT_LIST_H__
#define __JOIN_FESTA_PACKET_EVENT_LIST_H__

// ライブラリ
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_std_string.h>
#include <base/include/gfl2_Singleton.h>

// niji
//#include

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"

namespace NetAppLib {
namespace JoinFesta {

// 前方宣言
class JoinFestaPersonalData;
class JoinFestaPersonalDataManager;


/* ----------------------------------------------------------------------------------------- */
/**
* @brief  JoinFesta通信で受信したパケットをトリガとして発生するイベントをリスト化するためのクラス
*
* @note   「通信対戦」「通信交換」など<p>
*          他プレイヤーが自身を招待するために送信してきたパケットを本クラスでリスト化し管理する。
*/
/* ----------------------------------------------------------------------------------------- */
class JoinFestaPacketEventList  : public gfl2::base::Singleton<JoinFestaPacketEventList>
{
public:

  /**
   * @brief イベント情報
   */
  typedef struct
  {
    u32                                  m_ticketId;  //!< チケットID
    JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND m_kind;      //!< コマンド種別
    JoinFestaFriendKey                   m_friendKey; //!< フレンドキー

    /**
     * @brief 初期化関数
     *
     * @param kind       種別
     * @param ticketId   チケットID
     * @param friendKey  JoinFestaフレンドキー
     */
    void initialize( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND kind, u32 ticketId, const JoinFestaFriendKey& friendKey )
    {
      m_kind     = kind;
      m_ticketId = ticketId;
      gfl2::std::MemCopy( &friendKey, &m_friendKey, sizeof(friendKey) );
    }
  }PacketEventInfo;

public:
  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief  コンストラクタ
  *
  * @param[in]  personalMgr パーソナルマネージャ
  * @param[in]  heap        内部で使用するヒープ
  */
  /* ----------------------------------------------------------------------------------------- */
  JoinFestaPacketEventList( JoinFestaPersonalDataManager* personalMgr, gfl2::heap::HeapBase* heap );

  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief  デストラクタ
  */
  /* ----------------------------------------------------------------------------------------- */
  virtual ~JoinFestaPacketEventList( void );

public:
  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief イベントを登録する
   *
   * @attention バッファオーバーした場合は、登録されない
   *
   * @param[in] kind      パケットコマンド
   * @param[in] ticketId  チケットID
   * @param[in] friendKey イベント発行者のJoinFestaフレンドキー
   *
   * @return 登録に成功したならtrueを返却する
   */
  /* ----------------------------------------------------------------------------------------- */
  bool PushEvent( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND kind, u32 ticketId, const JoinFestaFriendKey& friendKey );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief  イベントを取得する
   *
   * @attention イベント発行者のパケット情報が更新されている場合はイベント情報から除外される。
   *
   * @param[in] isErase 取り出したイベントをリストから削除するならtrueを指定
   *
   * @return 取り出したイベント情報へのポインタ。データが存在しない場合はNULLを返す
   */
  /* ----------------------------------------------------------------------------------------- */
  const PacketEventInfo* PopEvent( bool isErase );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   メンバ変数のリセット
   */
  /* ----------------------------------------------------------------------------------------- */
  void Reset();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   イベント情報が存在するか判定
   *
   * @param   isSimpleCheck 簡易チェックの有無を指定
   *
   *               @arg 簡易チェックを有効にする。登録されているイベントが現在有効であるかのチェックを
   *               @arg 　行わず、単純にリストに要素が登録されているかのみを判定する。
   *
   * @return  イベント情報が存在するならtrueを返却
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsExistEvent( bool isSimpleCheck );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   イベント情報が存在するか判定
   *
   * @param   friendKey イベント発信者のJoinFestaフレンドキー
   *
   * @return  イベント情報が存在するならtrueを返却
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsExistEvent( const JoinFestaFriendKey& friendKey );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   イベント情報が存在するか判定
   *
   * @return  イベント情報が存在するならtrueを返却
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsExistEvent( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND type );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief  指定フレンドキーのイベント情報を削除
   *
   * @param[in] friendKey 削除対象のフレンドキー
   *
   * @return 該当イベントが存在していたならtrueを返却
   */
  /* ----------------------------------------------------------------------------------------- */
  bool EraseEvent( const JoinFestaFriendKey& friendKey );

private:
  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief  指定イベントが有効なイベントか判定
   *
   * @attention イベント発行者のパケット情報が更新されている場合は
   *            イベント情報から除外される。
   *
   * @param[in] personal イベント発行者のパーソナルデータ
   * @param[in] eventInf 判定対象イベント情報
   *
   * @return 指定イベントが有効値ならtrue、無効値ならfalseを返却
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsValidEventInfo( const JoinFestaPersonalData* personal, JoinFestaPacketEventList::PacketEventInfo* eventInf ) const;

private:
  static const u32 EVENT_BUFF_SIZE = 10;                            //!< バッファサイズ
  //
  JoinFestaPersonalDataManager*     m_personalMgr;                  //!< パーソナルマネージャ
  //
  gfl2::util::List<PacketEventInfo> m_infoList;                     //!< 登録メッセージリスト
};



}		// JoinFesta
}		// NetAppLib



#endif // __JOIN_FESTA_PACKET_EVENT_LIST_H__

