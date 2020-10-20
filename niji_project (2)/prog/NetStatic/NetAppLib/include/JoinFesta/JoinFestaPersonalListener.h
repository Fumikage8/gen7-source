//============================================================================================
/**
 * @file   JoinFestaPersonalListener.h
 *
 * @brief  JoinFestaパーソナルデータ管理情報
 *
 * @author ichiraku_katsuhiko
 */
//============================================================================================
#ifndef __JOIN_FESTA_PERSONAL_LISTENER_H__
#define __JOIN_FESTA_PERSONAL_LISTENER_H__
#pragma once


namespace NetAppLib {
namespace JoinFesta {

class JoinFestaPersonalData;

/**
 * @brief  ジョインフェスタパーソナルデータ更新イベントリスナクラス。
 *
 * @note   ジョインフェスタパケットデータの更新を各UIへ通知させることが主な役割。
 */
class JoinFestaPersonalListener
{
public:

  //------------------------------------------------------------------
  /**
   * @brief ジョインフェスタパーソナルデータ追加イベント通知
   *
   * @param[in]  person  追加されたパーソナル情報
   */
  //------------------------------------------------------------------
   virtual void OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person ){}

  //------------------------------------------------------------------
  /**
   * @brief ジョインフェスタパーソナルデータ更新イベント通知
   *
   * @param[in]  person         更新されたパーソナル情報
   * @param[in]  isUpdate パケット更新フラグ
   */
  //------------------------------------------------------------------
   virtual void OnUpdateJoinFesatPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate ){}

   //------------------------------------------------------------------
   /**
   * @brief ジョインフェスタパーソナルデータ削除イベント通知
   *
   * @param[in]  person  削除されたパーソナル情報
   */
   //------------------------------------------------------------------
   virtual void OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person ){}

};

}    // JoinFesta
}    // NetAppLib

#endif // __JOIN_FESTA_PERSONAL_LISTENER_H__
