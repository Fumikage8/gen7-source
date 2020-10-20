//=============================================================================
/**
 * @file   btl_Kawaigari.cpp
 * @date   2015/08/25 10:35:01
 * @author obata_toshihiro
 * @brief  可愛がり戦闘効果
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <niji_conv_header/message/msg_btl_std.h>

#include "./btl_common.h"
#include "./btl_Kawaigari.h"
#include "./btl_mainmodule.h"
#include "./btl_pokeparam.h"

GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief 戦闘開始後、最初の「○○は　どうする？」メッセージの代わりに、
 *        かわいがり特殊待機メッセージを出力するか判定
 *
 * @param mainModule  参照するメインモジュール
 * @param poke        表示対象のポケモン
 * @param rand        使用する乱数
 *
 * @return 特殊メッセージのメッセージID
 * @retval -1  特殊メッセージを表示しない場合
 */
//-----------------------------------------------------------------------------
int Kawaigari::Check1stReadyMessage( const MainModule* mainModule, const BTL_POKEPARAM* poke, RandSys* rand )
{
  u8 friendship = mainModule->GetPokeFriendship( poke );

  int strID = -1;

  if( friendship >= 150 )
  {
    if( CheckSpecial1stReadyMessage( mainModule ) )
    {
      strID = BTL_STRID_STD_FR_Ready_Special;
    }
    else if( rand->Next(100) < 33 )
    {
      u32 strNum = 0;
      u16 strID_150[ MAX_KAWAIGARI_READY_MESSAGE_NUM ];
      GetReadyMessage_Friendship150( &strNum, strID_150, GFL_NELEMS(strID_150), poke );
      int idx = rand->Next( strNum );
      strID = strID_150[ idx ];
    }
  }
  else if( friendship >= 100 )
  {
    if( rand->Next(100) < 33 )
    {
      u32 strNum = 0;
      u16 strID_100[ MAX_KAWAIGARI_READY_MESSAGE_NUM ];
      GetReadyMessage_Friendship100( &strNum, strID_100, GFL_NELEMS(strID_100), poke );
      int idx = rand->Next( strNum );
      strID = strID_100[ idx ];
    }
  }
  else if( friendship >= 50 )
  {
    if( rand->Next(100) < 33 )
    {
      u32 strNum = 0;
      u16 strID_50[ MAX_KAWAIGARI_READY_MESSAGE_NUM ];
      GetReadyMessage_Friendship50( &strNum, strID_50, GFL_NELEMS(strID_50), poke );
      int idx = rand->Next( strNum );
      strID = strID_50[ idx ];
    }
  }

  return strID;
}

/**
 * @brief 戦闘開始後、最初の「○○は　どうする？」メッセージの代わりに、
 *        "特別な戦闘用の" メッセージを出力するか判定
 *
 * @param mainModule  参照するメインモジュール
 *
 * @retval true    特別な戦闘用のメッセージを表示する
 * @retval false   特別な戦闘用のメッセージを表示しない
 */
bool Kawaigari::CheckSpecial1stReadyMessage( const MainModule* mainModule )
{
  static const trainer::TrType SPECIAL_BATTLE_TARGET[] = 
  {
    trainer::TRTYPE_COMBAT,
    trainer::TRTYPE_CAPTAIN1,
    trainer::TRTYPE_CAPTAIN2,
    trainer::TRTYPE_CAPTAIN3,
    trainer::TRTYPE_CAPTAIN4,
    trainer::TRTYPE_CAPTAIN5,
    trainer::TRTYPE_CAPTAIN6,
    trainer::TRTYPE_CAPTAIN7,
    trainer::TRTYPE_ROCK,
    trainer::TRTYPE_EVIL,
    trainer::TRTYPE_EVIL2,
    trainer::TRTYPE_GROUND,
    trainer::TRTYPE_ETHERBOSS,
    trainer::TRTYPE_SKULLBOSS1,
    trainer::TRTYPE_SKY,
    trainer::TRTYPE_BEAST,
    trainer::TRTYPE_RED,
    trainer::TRTYPE_GREEN,
    trainer::TRTYPE_CAPTAIN1L,
    trainer::TRTYPE_CAPTAIN2L,
    trainer::TRTYPE_CAPTAIN3L,
    trainer::TRTYPE_CAPTAIN6BIG4,
    trainer::TRTYPE_COMBATBIG4,
    trainer::TRTYPE_CAPTAIN5L,
    trainer::TRTYPE_ROCKBIG4,
    trainer::TRTYPE_CAPTAIN1NR,
    trainer::TRTYPE_CAPTAIN3NR,
    trainer::TRTYPE_CAPTAIN7NR,
  };

  trainer::TrType trtype = static_cast<trainer::TrType>( mainModule->GetClientTrainerType( BTL_CLIENT_ENEMY1 ) );

  for( u32 i=0; i<GFL_NELEMS(SPECIAL_BATTLE_TARGET); ++i )
  {
    if( trtype == SPECIAL_BATTLE_TARGET[i] )
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief なかよし度が150以上の場合に表示可能な、可愛がり特殊待機メッセージの候補を取得する
 *
 * @param[out] destMessageNum       メッセージ候補の数
 * @param[out] destMessageArray     メッセージ候補の格納先
 * @param      destMessageArraySize メッセージ候補の格納先配列のサイズ
 * @param      poke                 メッセージを表示するポケモン
 */
void Kawaigari::GetReadyMessage_Friendship150(
  u32*                 destMessageNum,
  u16*                 destMessageArray,
  u32                  destMessageArraySize, 
  const BTL_POKEPARAM* poke )
{
  static const u16 DEFAULT_MESSAGE[] = {
    BTL_STRID_STD_FR_Ready_150_01, 
    BTL_STRID_STD_FR_Ready_150_02, 
    BTL_STRID_STD_FR_Ready_150_03,
    BTL_STRID_STD_FR_Ready_150_04, 
    BTL_STRID_STD_FR_Ready_150_05,
  };
  GetReadyMessage_Common( destMessageNum, destMessageArray, destMessageArraySize, poke, DEFAULT_MESSAGE, GFL_NELEMS(DEFAULT_MESSAGE) );
}

/**
 * @brief なかよし度が100以上の場合に表示可能な、可愛がり特殊待機メッセージの候補を取得する
 *
 * @param[out] destMessageNum       メッセージ候補の数
 * @param[out] destMessageArray     メッセージ候補の格納先
 * @param      destMessageArraySize メッセージ候補の格納先配列のサイズ
 * @param      poke                 メッセージを表示するポケモン
 */
void Kawaigari::GetReadyMessage_Friendship100( 
  u32*                 destMessageNum, 
  u16*                 destMessageArray,
  u32                  destMessageArraySize,
  const BTL_POKEPARAM* poke )
{
  static const u16 DEFAULT_MESSAGE[] = {
    BTL_STRID_STD_FR_Ready_100_01,
    BTL_STRID_STD_FR_Ready_100_02, 
    BTL_STRID_STD_FR_Ready_100_03,
    BTL_STRID_STD_FR_Ready_100_04,
    BTL_STRID_STD_FR_Ready_100_05,
  };
  GetReadyMessage_Common( destMessageNum, destMessageArray, destMessageArraySize, poke, DEFAULT_MESSAGE, GFL_NELEMS(DEFAULT_MESSAGE) );
}

/**
 * @brief なかよし度が50以上の場合に表示可能な、可愛がり特殊待機メッセージの候補を取得する
 *
 * @param[out] destMessageNum       メッセージ候補の数
 * @param[out] destMessageArray     メッセージ候補の格納先
 * @param      destMessageArraySize メッセージ候補の格納先配列のサイズ
 * @param      poke                 メッセージを表示するポケモン
 */
void Kawaigari::GetReadyMessage_Friendship50(
  u32*                 destMessageNum,
  u16*                 destMessageArray,
  u32                  destMessageArraySize,
  const BTL_POKEPARAM* poke )
{
  static const u16 DEFAULT_MESSAGE[] = {
    BTL_STRID_STD_FR_Ready_50_01,
    BTL_STRID_STD_FR_Ready_50_02, 
    BTL_STRID_STD_FR_Ready_50_03,
    BTL_STRID_STD_FR_Ready_50_05,
  };
  GetReadyMessage_Common( destMessageNum, destMessageArray, destMessageArraySize, poke, DEFAULT_MESSAGE, GFL_NELEMS(DEFAULT_MESSAGE) );
}

/**
 * @brief 表示可能な可愛がり特殊待機メッセージの候補をリストアップする
 *
 * @param[out] destMessageNum       メッセージ候補の数
 * @param[out] destMessageArray     メッセージ候補の格納先
 * @param      destMessageArraySize メッセージ候補の格納先配列のサイズ
 * @param      poke                 メッセージを表示するポケモン
 * @param      defaultMessageArray  デフォルト候補メッセージ( 無条件に候補になるメッセージ )
 * @param      defaultMessageNum    デフォルト候補メッセージの数
 */
void Kawaigari::GetReadyMessage_Common( 
  u32*                 destMessageNum, 
  u16*                 destMessageArray, 
  u32                  destMessageArraySize,
  const BTL_POKEPARAM* poke,
  const u16*           defaultMessageArray, 
  u32                  defaultMessageNum )
{
  *destMessageNum = 0;

  // デフォルトメッセージを格納
  for( u32 i=0; i<defaultMessageNum; ++i )
  {
    if( *destMessageNum < destMessageArraySize )
    {
      destMessageArray[ *destMessageNum ] = defaultMessageArray[i];
      ++( *destMessageNum );
    }
  }
}

//-------------------------------------------------------------------------
/**
 * @brief 「○○は　どうする？」メッセージの代わりに、かわいがり特殊待機メッセージを出力するか判定
 *
 * @param mainModule  参照するメインモジュール
 * @param poke        表示対象のポケモン
 * @param rand        使用する乱数
 *
 * @return 特殊メッセージのメッセージID
 * @retval -1  特殊メッセージを表示しない場合
 */
//-------------------------------------------------------------------------
int Kawaigari::CheckCommonReadyMessage( const MainModule* mainModule, const BTL_POKEPARAM* poke, RandSys* rand )
{
  return -1;
}




GFL_NAMESPACE_END( btl )