//============================================================================================
/**
 * @file    JoinFestaPacketUtil.h
 *
 * @author ichiraku_katsuhiko
 */
//============================================================================================

#ifndef __JOIN_FESTA_PACKET_UTIL_H__
#define __JOIN_FESTA_PACKET_UTIL_H__
#pragma once

#if defined(GF_PLATFORM_CTR)
// ライブラリ
#include <nex.h>
#endif // GF_PLATFORM_CTR

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketDefine.h"

namespace NetAppLib {
namespace JoinFesta {

// 前方宣言
class JoinFestaPersonalData;

/**
 * @brief JoinFesta用ユーティリティクラス。<p>
 *        JoinFestaで汎用的に使われる機能をstatic関数として提供する。
 */
class JoinFestaPacketUtil
{
private:
  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief    コンストラクタ(隠蔽化)
   */
  /* ----------------------------------------------------------------------------------------- */
  JoinFestaPacketUtil( void );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief    デストラクタ(隠蔽化)
   */
  /* ----------------------------------------------------------------------------------------- */
  ~JoinFestaPacketUtil(void);

public:
  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   通信アクションを履歴情報に登録する
   *
   * @note    savedata::ProfileHistory, 自身のJoinFestaPersonalDataが更新される
   *
   * @param[in]   action 履歴情報に登録する通信アクション
   */
  /* ----------------------------------------------------------------------------------------- */
  //static void RegistProfileHistory( savedata::ProfileHistory::E_HISTORY_ACTION action );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   nn::nex::SubscriptionDataからNetAppLib::JoinFesta::JoinFestaPacketData情報を抜出し格納
   *
   * @param[in]   src 入力元nn::nex::SubscriptionDataへの参照
   * @param[out]  dst 出力先NetAppLib::JoinFesta::JoinFestaPacketDataへのポインタ
   *
   * @return  取得に成功したならtrueを返却
   */
  /* ----------------------------------------------------------------------------------------- */
#if defined(GF_PLATFORM_CTR)
  static bool GetPacketDataFromSubscriptionData( nn::nex::SubscriptionData& src, NetAppLib::JoinFesta::JoinFestaPacketData* dst );
#endif // GF_PLATFORM_CTR

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   nn::nex::SubscriptionDataからNetAppLib::JoinFesta::JoinFestaPacketData情報を抜出し格納
   *
   * @param[in]   src   nn::nex::SubscriptionData::GetApplicationBuffer()で取得したバッファ
   * @param[out]  dst   出力先NetAppLib::JoinFesta::JoinFestaPacketDataへのポインタ
   *
   * @return  取得に成功したならtrueを返却
   */
  /* ----------------------------------------------------------------------------------------- */
#if defined(GF_PLATFORM_CTR)
  static bool GetPacketDataFromSubscriptionData( const nn::nex::qVector<nn::nex::qByte>& src, NetAppLib::JoinFesta::JoinFestaPacketData* dst );
#endif // GF_PLATFORM_CTR

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   指定のJoinFestaフレンドキーが一致するか判定
   *
   * @param[in]   friendKey1 比較対象となるJoinFestaフレンドキー1
   * @param[in]   friendKey2 比較対象となるJoinFestaフレンドキー2
   *
   * @return  指定JoinFestaフレンドキーが一致するならtrue
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool IsSameFriendKey( const JoinFestaFriendKey& friendKey1, const JoinFestaFriendKey& friendKey2 );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   指定のJoinFestaフレンドキーが自身のものか判定
   *
   * @param[in]   friendKey 調査対象となるJoinFestaフレンドキー
   *
   * @return  自分自身のJoinFestaフレンドキーならtrueを返却
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool IsMyFriendKey( const JoinFestaFriendKey& friendKey );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   指定のJoinFestaフレンドキーが有効なものか判定
   *
   * @param[in]   friendKey 調査対象となるJoinFestaフレンドキー
   *
   * @return  有効なJoinFestaフレンドキーならtrue、無効なJoinFestaフレンドキーならfalseを返却
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool IsValidFriendKey( const JoinFestaFriendKey& friendKey );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   transferdIDを比較してどちらのJoinFestaフレンドキーが大きな数字か判定
   *
   * @param[in]   friendKey1 調査対象となるJoinFestaフレンドキー1
   * @param[in]   friendKey2 調査対象となるJoinFestaフレンドキー2
   *
   * @note    数値が同じの場合（同一JoinFestaフレンドキーを渡した場合）はtrueを返却
   *
   * @return  JoinFestaフレンドキー1が大きいならtrue、JoinFestaフレンドキー2が大きいならfalseを返却
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool IsBiggerJoinFestaFriendKey( const JoinFestaFriendKey& friendKey1, const JoinFestaFriendKey& friendKey2 );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   指定コマンドが基本ステータス情報を含むコマンドか判定
   *
   * @param[in]  command  調査対象となるパケットコマンド
   *
   * @return  基本ステータス情報を含むコマンドであればtrueを返却
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool IsBasicCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND command );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   パケットデータのユーザーIDからJoinFestaフレンドキーを生成する
   *
   * @param[in]   isWifi   Wifi通信モードのユーザーIDならtrueを指定する
   * @param[in]   userId   ユーザーID
   *
   * @return  生成されたJoinFestaフレンドキー
   */
  /* ----------------------------------------------------------------------------------------- */
  static JoinFestaFriendKey ConvertUserIdToJoinFestaFriendKey( bool isWifi, bit64 userId );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   JoinFestaパーソナル情報の取得
   *
   * @param[in/out]   gameMgr     GameManagerへのポインタ
   * @param[in]       relation    検索するユーザーの種別（通りすがり・VIP・友達）
   * @param[in]       index       releationで指定したユーザー種別に格納される個人情報のインデックス（0～max）
   *
   * @return  パーソナル情報へのポインタ。該当者不在の場合はNULLが返却される。
   */
  /* ----------------------------------------------------------------------------------------- */
//  static const NetAppLib::JoinFesta::JoinFestaPersonalData* GetJoinFestaPersonalData( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION relation, u32 index );

  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief   自身を除くすべてのユーザーをオフラインに設定する
  *
  * @note    JoinFestaパーソナルマネージャをセーブデータをもとに再構築する
  */
  /* ----------------------------------------------------------------------------------------- */
  static void SetOfflineJoinFestaPersonalAll( void );

  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief   指定種別のユーザーをオフラインに設定する
  */
  /* ----------------------------------------------------------------------------------------- */
  static void SetOfflineJoinFestaPersonalWithRelation( JoinFestaDefine::E_JOIN_FESTA_RELATION relation );

  //------------------------------------------------------------------
  /**
  * @brief   JoinFesta用ユーザーセーブデータ領域の更新
  */
  //------------------------------------------------------------------
  static void UpdateJoinFestaPersonalSaveData();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   アトラクションに参加可能かチェックする
   * @retval  true : 参加出来る
   * @retval  false : 参加出来ない
   *
   * @note    条件　①アトラクションご褒美受取済み
   *                ②アトラクション開いているか
   *                ③クリア後解禁アトラクションの場合、クリアしているか
   *                ④制限時間の残りが６０秒以上あるか
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool CheckAttractionJoinEnable( JoinFestaPersonalData* pPersonalData );

  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief   更新日時順にソートする
  * @param   sortList     ソートしたいリスト
  * @param   sortListSize ソートリストサイズ
  */
  /* ----------------------------------------------------------------------------------------- */
  static void SortByUpdateDate( JoinFestaPersonalData* sortList[], const u32 sortListSize );

  /* ----------------------------------------------------------------------------------------- */
  /**
  * @brief   ファインダースタジオ写真Newフラグチェック
  * @retval  true:１人以上写真Newフラグがたっている   
  * @retval  false:１人も写真Newフラグがたっていない   
  */
  /* ----------------------------------------------------------------------------------------- */
  static bool CheckNewPhoto();

};




}    // JoinFesta
}    // NetAppLib




#endif // __JOIN_FESTA_PACKET_UTIL_H__

