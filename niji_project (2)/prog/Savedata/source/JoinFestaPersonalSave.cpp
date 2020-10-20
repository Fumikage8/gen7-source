//===================================================================
/**
 * @file    JoinFestaPersonalSave.h
 * @brief   JoinFestaプレイヤーデータ管理
 * @author  ichiraku_katsuhiko
 * @date    2015/10/12
*/
//===================================================================
#include "../include/JoinFestaPersonalSave.h"


#include <util/include/gfl2_std_string.h>

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

GFL_NAMESPACE_BEGIN(Savedata)

//--------------------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
//--------------------------------------------------------------------------------------------
JoinFestaPersonalSave::JoinFestaPersonalSave( void )
{
  Reset();
  STATIC_ASSERT( (sizeof(SaveBody) == 25608) );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
//--------------------------------------------------------------------------------------------
JoinFestaPersonalSave::~JoinFestaPersonalSave(void)
{
  Reset();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   全パーソナルデータリストをリセットする
 */
//--------------------------------------------------------------------------------------------
void JoinFestaPersonalSave::Reset( void )
{
  gfl2::std::MemClear( &m_body, sizeof(SaveBody) );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   登録済みコアデータからJoinFestaフレンドキーを取得する
 *
 * @param   index     登録コアデータインデックス
 * @param   friendKey 取得したJoinFestaフレンドキー格納先領域へのポインタ
 *
 * @return  取得に成功したならtrueを返却する
 */
//--------------------------------------------------------------------------------------------
bool JoinFestaPersonalSave::GetJoinFestaFriendKey( u32 index, JoinFestaFriendKey *friendKey )
{
  if( index < m_body.m_registNum )
  {
    NetAppLib::JoinFesta::PacketPersonalInfo* personalInfo = &m_body.m_personal[index].m_personalInfo;
    *friendKey = personalInfo->GetJoinFestaFriendKey();
    return true;
  }

  return false;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   JoinFestaフレンドキーをもとに、パーソナルデータを取得する[内部クラス用]
 *
 * @param   friendKey 検索対象となるJoinFestaフレンドキー
 *
 * @return  検出したパーソナルデータへのポインタ。データが存在しない場合はNULLを返却
 */
//--------------------------------------------------------------------------------------------
NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData* JoinFestaPersonalSave::GetPersonalCoreDataPrivate( const JoinFestaFriendKey& friendKey )
{
  for( u32 i = 0; i < m_body.m_registNum; i++ )
  {
    JoinFestaFriendKey personalKey;

    if( GetJoinFestaFriendKey( i, &personalKey ) )
    {
      if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey(personalKey, friendKey) ){
        return &(m_body.m_personal[i]);
      }
    }
  }
  return NULL;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   インデックスをもとにパーソナルコアデータを取得する
 *
 * @param   index 取得するデータインデックス。GetNum()未満の正数値を指定すること
 *
 * @return  検出したパーソナルコアデータへのポインタ。データが存在しない場合はNULLを返却
 */
//--------------------------------------------------------------------------------------------
const NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData* JoinFestaPersonalSave::GetPersonalCoreData( u32 index )
{
  if( m_body.m_registNum <= index ){
    return NULL;
  }
  return &(m_body.m_personal[index]);
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   JoinFestaフレンドキーをもとに、パーソナルデータを取得する
 *
 * @param   friendKey 検索対象となるJoinFestaフレンドキー
 *
 * @return  検出したパーソナルデータへのポインタ。データが存在しない場合はNULLを返却
 */
//--------------------------------------------------------------------------------------------
const NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData* JoinFestaPersonalSave::GetPersonalCoreData( const JoinFestaFriendKey& friendKey )
{
  return GetPersonalCoreDataPrivate(friendKey);
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   JoinFestaフレンドキーをもとに、パーソナルデータを削除する
 *
 * @param   friendKey 削除対象となるJoinFestaフレンドキー
 *
 * @return  削除が行われたならtrueを返却する
 */
//--------------------------------------------------------------------------------------------
bool JoinFestaPersonalSave::RemovePersonalData( const JoinFestaFriendKey& friendKey )
{
  u32 listNum = m_body.m_registNum;

  for( u32 i = 0; i < listNum; i++ )
  {
    JoinFestaFriendKey personalKey;

    if( GetJoinFestaFriendKey( i, &personalKey ) )
    {
      if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey(personalKey, friendKey) )
      {
        if( i < (listNum-1) )
        {
          // 消す箇所を詰めます
          for( u32 k = i; k < listNum-1; k++ ){
            gfl2::std::MemCopy( &m_body.m_personal[k+1], &m_body.m_personal[k], sizeof(NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData) );
          }
        }
        // お尻を削除
        m_body.m_registNum--;
        return true;
      }
    }
  }
  return false;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   パーソナルデータを更新する
 *
 * @param   personal   登録対象のパーソナルデータ
 *
 * @return  更新されたならtrueを、一致するJoinFestaフレンドキーが見つからなかった際はfalseを返却
 */
//--------------------------------------------------------------------------------------------
bool JoinFestaPersonalSave::UpdatePersonalData( NetAppLib::JoinFesta::JoinFestaPersonalData* personal )
{
  NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData *target = GetPersonalCoreDataPrivate( personal->GetJoinFestaFriendKey() );

  if( target != NULL )
  {
    personal->GetCoreData( target );
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   パーソナルデータを追加する
 *
 * @param   personal   追加対象のパーソナルデータ
 *
 * @note    空きがない場合は、最も古い情報を削除する
 *
 * @return  追加できたならtrueを、データがすでに存在する場合はfalseを返却
 */
//--------------------------------------------------------------------------------------------
bool JoinFestaPersonalSave::AddPersonalData( const NetAppLib::JoinFesta::JoinFestaPersonalData* personal )
{
  const NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData *target = GetPersonalCoreData( personal->GetJoinFestaFriendKey() );
  if( target != NULL ){
    return false;   // 既に存在する
  }

  if( IsFull() )
  {
    // 最も古い情報を削除する
    JoinFestaFriendKey deleteFriendKey;
    if( GetJoinFestaFriendKey( 0, &deleteFriendKey ) )
    {
      RemovePersonalData( deleteFriendKey );
    }
    else
    {
      GFL_ASSERT(0);
      return false;
    }
  }

  personal->GetCoreData( &m_body.m_personal[m_body.m_registNum] );
  m_body.m_registNum++;
  ICHI_PRINT(">ICHI パーソナルデータ追加 %d件目\n", m_body.m_registNum );

  return true;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   パーソナルデータが未登録か判定
 *
 * @return  1件もパーソナルデータが未登録ならtrueを返却する
 */
//--------------------------------------------------------------------------------------------
bool JoinFestaPersonalSave::IsEmpty( void ) const
{
  return (GetNum() == 0);
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   パーソナルデータが制限値に達しているか判定
 *
 * @return  パーソナルデータ登録件数が制限値に達しているならtrueを返却
 */
//--------------------------------------------------------------------------------------------
bool JoinFestaPersonalSave::IsFull( void ) const
{
  return (GetNum() == LIST_MAX);
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   お気に入り登録されているパーソナルデータ件数を取得する
 *
 * @return  登録パーソナルデータ件数
 */
//--------------------------------------------------------------------------------------------
u32 JoinFestaPersonalSave::GetNum( void ) const
{
  return m_body.m_registNum;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   パーソナルデータリストを取得する
 *          件数はGetNum()で取得すること
 *
 * @return  パーソナルデータリストへのポインタ
 */
//--------------------------------------------------------------------------------------------
const NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData* JoinFestaPersonalSave::GetPersonalDataList( void )
{
  return m_body.m_personal;
}


//------------------------------------------------------------------------------------------
/**
* @brief    読み込んだデータをセットする関数
*
* @param    pData    先頭のポインタ
*/
//------------------------------------------------------------------------------------------
// @override
void JoinFestaPersonalSave::SetData( void* pData )
{
  gfl2::std::MemCopy( pData, &m_body, sizeof(SaveBody) );
}


//------------------------------------------------------------------------------------------
/**
* @brief    データの先頭ポインタを得る関数
*
* @return  先頭のポインタ
*/
//------------------------------------------------------------------------------------------
// @override
void* JoinFestaPersonalSave::GetData(void)
{
  return &m_body;
}


//------------------------------------------------------------------------------------------
/**
* @brief    データサイズ
*
* @return  バイト
*/
//------------------------------------------------------------------------------------------
// @override
size_t JoinFestaPersonalSave::GetDataSize(void)
{
  return sizeof(SaveBody);
}




GFL_NAMESPACE_END(Savedata)

