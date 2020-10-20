//============================================================================================
/**
 * @file   JoinFestaBlackPlayerData.h
 * @brief  JoinFestaブラックリストのプレイヤーデータ用クラス
 * @author ichiraku_katsuhiko
 */
//============================================================================================

#ifndef __JOIN_FESTA_BLACK_PLAYER_DATA_H__
#define __JOIN_FESTA_BLACK_PLAYER_DATA_H__
#pragma once

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketDefine.h"


// 前方宣言
namespace NetAppLib {
  namespace JoinFesta {
    class JoinFestaPersonalData;
  }
}

namespace NetAppLib {
namespace JoinFestaUI {


/* ----------------------------------------------------------------------------------------- */
/**
* @brief  JoinFestaブラックリストのプレイヤーデータ用クラス
*/
/* ----------------------------------------------------------------------------------------- */
class JoinFestaBlackPlayerData
{
public:

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief パーソナルコアデータ（1人辺りのセーブデータ）
   *
   * @attention JoinFestaPlayerDataからブラックリスト表示に必要なものだけを抜き出しています。
   *            
   */
  /* ----------------------------------------------------------------------------------------- */
  struct CoreData
  {
    // パーソナル情報
    gfl2::str::STRCODE     name[ ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ];    //!< 名前         26byte(2x(12+1))
    //----------------------------------------------------------------------------------
    u8                     padding1[2];
    //----------------------------------------------------------------------------------
    u32                    myPrincipalId;                             //!< 自身のプリンシバルID           4Byte
    u64                    transferdId;                               //!< トランスファードID             8byte
    //----------------------------------------------------------------------------------
    u32                    addressId;                                 //!< 簡易アドレス情報(国など）      4Byte
    //----------------------------------------------------------------------------------
    Savedata::MyStatus::ICON_DATA         icon;                       //!< 着せ替え                      36byte
    //----------------------------------------------------------------------------------
    u8                     sex;                                       //!< 性別                           0byte
    u8                     regionCode;                                //!< リージョン                     1byte
    u8                     romCode;                                   //!< ROMコード
    u8                     languageId;                                //!< 言語コード
    u8                     padding2[3];
    //----------------------------------------------------------------------------------
    b8                     isUse;                                     //! セーブ使用済みフラグ

    /***************************************
    * @brief コンストラクタ
    **************************************/
    CoreData();
    
    /***************************************
    * @brief コンストラクタ
    **************************************/
    CoreData( const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData );
    
    /***************************************
    * @brief JoinFestaフレンドキーの取得
    **************************************/
    JoinFestaFriendKey GetJoinFestaFriendKey() const
    {
      JoinFestaFriendKey friendKey;
      friendKey.initialize( myPrincipalId, transferdId );
      return friendKey;
    }
  };

public:
  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief    コンストラクタ
   */
  /* ----------------------------------------------------------------------------------------- */
  JoinFestaBlackPlayerData();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief    デストラクタ
   */
  /* ----------------------------------------------------------------------------------------- */
  virtual ~JoinFestaBlackPlayerData();

public:
  CoreData              m_core;                   //!< パラメータ本体
};


}    // JoinFestaUI
}    // NetAppLib

#endif // __JOIN_FESTA_BLACK_PLAYER_DATA_H__
