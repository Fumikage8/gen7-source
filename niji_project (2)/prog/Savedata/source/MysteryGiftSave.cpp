//===================================================================
/**
 * @file    MysteryGiftSave.h
 * @brief   不思議な贈り物セーブデータ
 * @author  Shin Kosaka
 * @date    2012.09.13
 */
//===================================================================

#include  <system/include/Date/gfl2_Date.h>

#include "Savedata/include/MysteryGiftSave.h"
#include "Savedata/include/MyStatus.h"

#include "System/include/PokemonVersion.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <debug/include/gfl2_DebugPrint.h>
#include  <debug/include/gfl2_Assert.h>

#include "PokeTool/include/PokeToolRibbon.h"
//#include <poke_tool/poke_tool_PokeMemory.h>


GFL_NAMESPACE_BEGIN(Savedata)



//=============================================================================================
/**
*  コンストラクタ
*/
//=============================================================================================
MysteryGiftSave::MysteryGiftSave( void )
{
  // 型のサイズが変わってしまっていないかチェックする
  //STATIC_ASSERT( sizeof(m_coreData) == (18+129+4)*5 );

  this->Clear( NULL );
}
//=============================================================================================
/**
*  デストラクタ
*/
//=============================================================================================
MysteryGiftSave::~MysteryGiftSave()
{
}



//=============================================================================================
/**
*  受け取れるポケモン数取得
*/
//=============================================================================================
u32 MysteryGiftSave::GetPokeGiftNum( void )
{
  u32 dataNum = GetGiftDataNum();
  u32 count = 0;
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_poke && GetRecvFlag(index) == false )
    {
      count++;
    }
  }

  return count;
}

//=============================================================================================
/**
*  受け取れるアイテム数取得
*/
//=============================================================================================
u32 MysteryGiftSave::GetItemGiftNum( void )
{
  u32 dataNum = GetGiftDataNum();
  u32 count = 0;
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_item && GetRecvFlag(index) == false )
    {
      count++;
    }
  }

  return count;
}


//=============================================================================================
/**
*  受け取れるBP数取得
*/
//=============================================================================================
u32 MysteryGiftSave::GetBPGiftNum( void )
{
  u32 dataNum = GetGiftDataNum();
  u32 count = 0;
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_bp && GetRecvFlag(index) == false )
    {
      count++;
    }
  }

  return count;
}

//=============================================================================================
/**
*  受け取れるポケマメ数取得
*/
//=============================================================================================
u32 MysteryGiftSave::GetMameGiftNum( void )
{
  u32 dataNum = GetGiftDataNum();
  u32 count = 0;
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_mame && GetRecvFlag(index) == false )
    {
      count++;
    }
  }

  return count;
}

//=============================================================================================
/**
*  先頭のポケモンデータ取得（受け取り済みフラグが立ちます）
*  @param[out]   pOutput  出力先
*/
//=============================================================================================
void MysteryGiftSave::GetTopPokeGiftData( pml::pokepara::PokemonParam* pOutput , gfl2::heap::HeapBase * heap , bool isReceive )
{
  u32 dataNum = GetGiftDataNum();
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_poke && GetRecvFlag(index) == false )
    {
      const MYSTERY_GIFT_POKE_DATA* pPokeData = GetPokeGiftData( index );
      pml::pokepara::PokemonParam* pPokepara = CreatePokemonParamFromGiftData( pPokeData , heap->GetLowerHandle() , pPokeData->level );
      pOutput->CopyFrom( *pPokepara );
      GFL_SAFE_DELETE( pPokepara );

      if( isReceive )
      {
        SetRecvFlag( index );
      }

      break;
    }
  }
}

//=============================================================================================
/**
*  先頭のポケモンデータのタイプを取得（受け取り済みフラグが立ちます）
*  @param[out]   type  出力先
*/
//=============================================================================================
MysteryGiftSave::PokemonGiftType MysteryGiftSave::GetTopPokeGiftDataType( bool isReceive )
{
  u32 dataNum = GetGiftDataNum();
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_poke && GetRecvFlag(index) == false )
    {
      const MYSTERY_GIFT_POKE_DATA* pPokeData = GetPokeGiftData( index );
      if( isReceive )
      {
        SetRecvFlag( index );
      }
      if( pPokeData->egg ){
        return POKEMON_GIFT_TYPE_EGG;
      }else if( pPokeData->parentID == 0 ){
        return POKEMON_GIFT_TYPE_MINE;
      }else{
        return POKEMON_GIFT_TYPE_OTHER;
      }
    }
  }
  GFL_ASSERT(0); //@check ここにくるのはおかしい 
  return POKEMON_GIFT_TYPE_MINE;
}

//=============================================================================================
/**
*  先頭のポケモンデータの同時配布アイテム取得（受け取り済みフラグが立ちます）
*  @param[out]   itemno
*/
//=============================================================================================
u16 MysteryGiftSave::GetTopPokeGiftDataItem( bool isReceive )
{
  u32 dataNum = GetGiftDataNum();
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_poke && GetRecvFlag(index) == false )
    {
      const MYSTERY_GIFT_POKE_DATA* pPokeData = GetPokeGiftData( index );
      if( isReceive )
      {
        SetRecvFlag( index );
      }
      return pPokeData->setItem;
    }
  }
  GFL_ASSERT(0); //@check ここにくるのはおかしい 
  return 0;
}

//=============================================================================================
/**
*  先頭のポケモンデータの場所ID取得（受け取り済みフラグが立ちます
*  @param[out]   placeID
*/
//=============================================================================================
u16 MysteryGiftSave::GetTopPokeGiftDataPlaceID( bool isReceive )
{
  u32 dataNum = GetGiftDataNum();
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_poke && GetRecvFlag(index) == false )
    {
      const MYSTERY_GIFT_POKE_DATA* pPokeData = GetPokeGiftData( index );
      if( isReceive )
      {
        SetRecvFlag( index );
      }

      if( pPokeData->egg )
      {//タマゴ
        return pPokeData->getPlace;
      }
      else
      {//タマゴではない
        return pPokeData->bornPlace;
      }
    }
  }
  GFL_ASSERT(0); //@check ここにくるのはおかしい 
  return 0;
}

//=============================================================================================
/**
*  先頭のアイテムデータ取得（受け取り済みフラグが立ちます）
*  @param[out]   pOutput  出力先
*/
//=============================================================================================
void MysteryGiftSave::GetTopItemGiftData( MYSTERY_GIFT_ITEM_DATA* pOutput , bool isReceive )
{
  u32 dataNum = GetGiftDataNum();
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_item && GetRecvFlag(index) == false )
    {
      const MYSTERY_GIFT_ITEM_DATA* pItem = GetItemGiftData( index );
      *pOutput = *pItem;

      if( isReceive )
      {
        SetRecvFlag( index );
      }

      break;
    }
  }
}

//=============================================================================================
/**
*  先頭のBPデータ取得（受け取り済みフラグが立ちます）
*/
//=============================================================================================
u32 MysteryGiftSave::GetTopBPGiftData( bool isReceive )
{
  u32 dataNum = GetGiftDataNum();
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_bp && GetRecvFlag(index) == false )
    {
      u32 bp = GetBPGiftData( index );

      if( isReceive )
      {
        SetRecvFlag( index );
      }

      return bp;
    }
  }

  return 0;
}

//=============================================================================================
/**
*  先頭のポケマメデータ取得（受け取り済みフラグが立ちます）
*/
//=============================================================================================
u32 MysteryGiftSave::GetTopMameGiftData( bool isReceive )
{
  u32 dataNum = GetGiftDataNum();
  for( u32 index = 0 ; index < dataNum ; index++ )
  {
    MYSTERY_GIFT_TYPE type = GetGiftDataType( index );

    if( type == MYSTERY_GIFT_TYPE_mame && GetRecvFlag(index) == false )
    {
      u32 mame = GetMameGiftData( index );

      if( isReceive )
      {
        SetRecvFlag( index );
      }

      return mame;
    }
  }

  return 0;
}




//=============================================================================================
/**
*  不思議なおくりものデータポインタ取得
*/
//=============================================================================================
const MysteryGiftSave::MYSTERY_GIFT_DATA* MysteryGiftSave::GetGiftData( u32 index ) const
{
  if( index >= MYSTERY_GIFT_DATA_NUM )
    return NULL;

  return &(m_coreData.wonderGiftData[index]);
}


//=============================================================================================
/**
*  不思議なおくりものデータ設定
*/
//=============================================================================================
bool MysteryGiftSave::SetGiftData( u32 index , const MYSTERY_GIFT_DATA* pData )
{
  if( index >= MYSTERY_GIFT_DATA_NUM )
    return false;

  if( pData == NULL )
    return false;

  ::std::memcpy( &(m_coreData.wonderGiftData[index]) , pData , sizeof(MYSTERY_GIFT_DATA) );

  return true;
}

//=============================================================================================
/**
*  不思議なおくりものデータ設定
*/
//=============================================================================================
bool MysteryGiftSave::PushBackGiftData(const MYSTERY_GIFT_RECV_DATA* pData)
{
  //受信ビットを立てる
  u32 recvFlagBit = pData->dataBody.recvFlag & 1;
  if( recvFlagBit != 0  )
  {//1回のみ受信可能なデータ
    s32 flagIndex = 0;
    for( u32 i = 0 ; i < RECV_FLAG_NUM ; ++i )
    {
      if( pData->dataBody.eventID < (i+1) * 4 * 8 )
      {
        flagIndex = i;
        break;
      }
    }

    if( flagIndex >= 0 && flagIndex < RECV_FLAG_NUM )
    {
      u32 flagBit = pData->dataBody.eventID % 32;
      u32 flagBitResult = ( 1 << flagBit );
      //m_oldFlagIndex = flagIndex;
      //m_oldFlag = pCoreData->recvFlag[flagIndex];
      m_coreData.recvFlag[flagIndex] |= flagBitResult;
      GFL_PRINT("MysteryGiftSave:: recv flag update[%d][%d][%d][%d]\n" , flagIndex , pData->dataBody.eventID , flagBitResult , m_coreData.recvFlag[flagIndex] );
    }
  }

  //ランダムビット立てる
  if( recvFlagBit == 0  )
  {//何度でも受信又は、一日1回受信のときだけ立てる
    //ランダム配信Noチェック

    //std::memcpy( m_oldRandomFlag , pCoreData->randomDeliveryFlag , sizeof(m_oldRandomFlag) );//保存

    bool isRandomExists = false;
    for( s32 index = 0 ; index < Savedata::MysteryGiftSave::RANDOM_DELIVERY_FLAG_NUM - 1 ; ++index )
    {//おしだし
      m_coreData.randomDeliveryFlag[index].eventID = m_coreData.randomDeliveryFlag[index+1].eventID;
      m_coreData.randomDeliveryFlag[index].randNo = m_coreData.randomDeliveryFlag[index+1].randNo;
    }

    //最後の1件に記録
    m_coreData.randomDeliveryFlag[Savedata::MysteryGiftSave::RANDOM_DELIVERY_FLAG_NUM - 1].eventID = pData->dataBody.eventID;
    m_coreData.randomDeliveryFlag[Savedata::MysteryGiftSave::RANDOM_DELIVERY_FLAG_NUM - 1].randNo = pData->randomNo;

    GFL_PRINT("MysteryGiftSave::SeqWaitSave random save[%d][%d]",pData->dataBody.eventID,pData->randomNo);

    //ランダム配信を1周全部受け取ったかどうかチェック
    u32 randomMax = pData->randomNoMax;
    u32 randomRecvCount = 0;
    for( s32 index = 0 ; index < Savedata::MysteryGiftSave::RANDOM_DELIVERY_FLAG_NUM ; ++index )
    {
      if( m_coreData.randomDeliveryFlag[index].eventID == pData->dataBody.eventID && m_coreData.randomDeliveryFlag[index].randNo != 0 )
      {//同一イベントID
        randomRecvCount++;
      }
    }

    GFL_PRINT("MysteryGiftSave::SeqWaitSave random max[%d][%d]\n",randomMax , randomRecvCount);

    if( randomMax <= randomRecvCount )
    {//全部受け取った
      GFL_PRINT("MysteryGiftSave::SeqWaitSave random reset\n");
      for( s32 index = 0 ; index < Savedata::MysteryGiftSave::RANDOM_DELIVERY_FLAG_NUM ; ++index )
      {
        if( m_coreData.randomDeliveryFlag[index].eventID == pData->dataBody.eventID )
        {//同一イベントID
          m_coreData.randomDeliveryFlag[index].eventID = 0;
          m_coreData.randomDeliveryFlag[index].randNo = 0;
        }
      }
    }
  }

  //一日1回だけチェック
  u32 recvFlagBitToday = pData->dataBody.recvFlag & 4;
  if( recvFlagBitToday )
  {
    bool isExists = false;
    u64 dataTime = ( pData->dataBody.GetYear() % 100 ) * 10000 + pData->dataBody.GetMonth()*100 + pData->dataBody.GetDay();
    for( u32 index = 0 ; index < ONE_DAY_FLAG_NUM ; ++index )
    {
      ONE_DAY_FLAG* oneDayFlag = &m_coreData.oneDayFlag[index];
      u32 recvTime = oneDayFlag->year * 10000 + oneDayFlag->month*100 + oneDayFlag->day;
      if( oneDayFlag->eventID == pData->dataBody.eventID )
      {//イベントID一致
        oneDayFlag->year = pData->dataBody.GetYear();
        oneDayFlag->month = pData->dataBody.GetMonth();
        oneDayFlag->day = pData->dataBody.GetDay();

        isExists = true;

        break;
      }
    }

    if( isExists == false )
    {//空きに入れる
      for( u32 index = 0 ; index < ONE_DAY_FLAG_NUM ; ++index )
      {
        ONE_DAY_FLAG* oneDayFlag = &m_coreData.oneDayFlag[index];
        u32 recvTime = oneDayFlag->year * 10000 + oneDayFlag->month*100 + oneDayFlag->day;
        if( dataTime > recvTime )
        {//上書き可能
          oneDayFlag->eventID = pData->dataBody.eventID;
          oneDayFlag->year = pData->dataBody.GetYear();
          oneDayFlag->month = pData->dataBody.GetMonth();
          oneDayFlag->day = pData->dataBody.GetDay();

          isExists = true;

          break;
        }
      }

      GFL_ASSERT( isExists );//@fix
    }
  }


  u32 saveIndex = GetGiftDataNum();
  bool ret = SetGiftData(GetGiftDataNum(), &pData->dataBody);

  //時間更新
  if( ret )
  {
#if defined(GF_PLATFORM_CTR)
    nn::fnd::DateTime now = nn::fnd::DateTime::GetNow();
    u32 date = ( now.GetYear() % 100 ) * 10000 + now.GetMonth() * 100 + now.GetDay();
    m_coreData.wonderGiftData[saveIndex].recvDate = date;
#endif
  }

  return ret;
}

//=============================================================================================
/**
*  不思議なおくりものデータ順番入れ替え(通信アプリ用)
* @param[in]   index1   インデックス1
* @param[in]   index2   インデックス2
*/
//=============================================================================================
void MysteryGiftSave::SwapGiftData( u32 index1 , u32 index2 )
{
  if( index1 >= GetGiftDataNum() )
    return ;
  if( index2 >= GetGiftDataNum() )
    return ;

  const MYSTERY_GIFT_DATA* pData = GetGiftData( index1 );
  if( pData )
  {
    MYSTERY_GIFT_DATA data;
    ::std::memcpy( &(data) , pData , sizeof(MYSTERY_GIFT_DATA) );


    SetGiftData( index1 , GetGiftData( index2 ) );
    SetGiftData( index2 , &data );
  }
}

//=============================================================================================
/**
*  不思議なおくりものデータ削除(通信アプリ用)
* @param[in]   index        インデックス
* @param[in]   bDoDelete    可能な場合、実際に削除する
*/
//=============================================================================================
bool MysteryGiftSave::DeleteGiftData( u32 index1, const bool bDoDelete)
{
  bool ret = false;
  u32 dataNum = GetGiftDataNum();
  if( index1 < dataNum )
  {//データがある
    const bool bCanDelete  = GetRecvFlag( index1 );
    
    if(!bDoDelete) return bCanDelete;
    
    if( bCanDelete )
    {//配達員から受け取り済み(削除可能)
      for( u32 index = index1 ; index < dataNum ; ++index )
      {
        const MYSTERY_GIFT_DATA* pSlideData = GetGiftData( index + 1 );
        if( pSlideData )
        {//削除する場合データを詰める
          SetGiftData( index , pSlideData );
        }
      }

      //末尾に空のデータを入れる
      MYSTERY_GIFT_DATA* pLastData = &(m_coreData.wonderGiftData[MYSTERY_GIFT_DATA_NUM - 1]);
      if( pLastData )
      {
        ::std::memset( pLastData , 0 , sizeof(MYSTERY_GIFT_DATA) );
      }

      ret = true;
    }
  }

  return ret;
}

//=============================================================================================
/**
*  不思議なおくりものデータ数取得
*/
//=============================================================================================
u32 MysteryGiftSave::GetGiftDataNum( void ) const
{
  u32 count = 0;
  for( u32 i = 0 ; i < MYSTERY_GIFT_DATA_NUM ; ++i )
  {
    if( GetGiftData( i )->eventName[0] != 0 )
    {
      count++;
    }
  }

  return count;
}


//=============================================================================================
/**
*  不思議なおくりものデータタイプ取得
* @param[in]   index   インデックス
*/
//=============================================================================================
MysteryGiftSave::MYSTERY_GIFT_TYPE MysteryGiftSave::GetGiftDataType( u32 index ) const
{
  MYSTERY_GIFT_TYPE type = MYSTERY_GIFT_TYPE_poke;
  const MYSTERY_GIFT_DATA* pData = GetGiftData( index );
  if( pData )
  {
    type = (MYSTERY_GIFT_TYPE)pData->dataType;
  }

  return type;
}


//=============================================================================================
/**
*  不思議なおくりものポケモンデータ取得
* @param[in]   index   インデックス
*/
//=============================================================================================
const MysteryGiftSave::MYSTERY_GIFT_POKE_DATA* MysteryGiftSave::GetPokeGiftData( u32 index ) const
{
  if( GetGiftDataType( index ) != MYSTERY_GIFT_TYPE_poke )
    return NULL;

  const MYSTERY_GIFT_DATA* pData = GetGiftData( index );
  if( pData )
  {
    return (const MYSTERY_GIFT_POKE_DATA*)(pData->body);
  }

  return NULL;
}

//=============================================================================================
/**
*  不思議なおくりものアイテムデータ取得
* @param[in]   index   インデックス
*/
//=============================================================================================
const MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA* MysteryGiftSave::GetItemGiftData( u32 index ) const
{
  if( GetGiftDataType( index ) != MYSTERY_GIFT_TYPE_item )
    return NULL;

  const MYSTERY_GIFT_DATA* pData = GetGiftData( index );
  if( pData )
  {
    return (const MYSTERY_GIFT_ITEM_DATA*)(pData->body);
  }

  return NULL;
}

//=============================================================================================
/**
*  不思議なおくりものBP取得
* @param[in]   index   インデックス
*/
//=============================================================================================
u32 MysteryGiftSave::GetBPGiftData( u32 index ) const
{
  const MYSTERY_GIFT_DATA* pData = GetGiftData( index );
  if( pData )
  {
    return pData->GetBPData();
  }

  return 0;
}


//=============================================================================================
/**
*  不思議なおくりものまめ取得
* @param[in]   index   インデックス
*/
//=============================================================================================
u32 MysteryGiftSave::GetMameGiftData( u32 index ) const
{
  const MYSTERY_GIFT_DATA* pData = GetGiftData( index );
  if( pData )
  {
    return pData->GetMameData();
  }

  return 0;
}

//=============================================================================================
/**
*  不思議なおくりものデータを「配達員から受け取り済み」に設定
* @param[in]   index   インデックス
*/
//=============================================================================================
void MysteryGiftSave::SetRecvFlag( u32 index )
{
  const MYSTERY_GIFT_DATA* pData = GetGiftData( index );
  if( pData )
  {
    MYSTERY_GIFT_DATA data;
    ::std::memcpy( &(data) , pData , sizeof(MYSTERY_GIFT_DATA) );

    data.recvFlag |= 2;

    SetGiftData( index , &data );
  }
}

//=============================================================================================
/**
*  不思議なおくりものデータをゲーム内（配達員）で受け取り済みかどうかのフラグ取得
* @param[in]   index   インデックス
* @retval       true : 配達員から受け取り済み , false : まだ配達員から受け取ってない
*
*             falseが返ったら配達員からデータを受け取り、その後SetRecvFlagを呼んでください。
*             存在しないindexを指定した場合はtrueが返ります。
*/
//=============================================================================================
bool MysteryGiftSave::GetRecvFlag( u32 index )
{
  bool isRecv = true;
  const MYSTERY_GIFT_DATA* pData = GetGiftData( index );
  if( pData )
  {
    //GFL_PRINT("flag[%d]\n",flag);
    isRecv  = pData->GetRecvFlag();
  }

  return isRecv;
}

//=============================================================================================
/**
*  不思議なおくりものデータからPokemonParamを生成（PokemonParamは受け取った側で解放すること）
*/
//=============================================================================================
pml::pokepara::PokemonParam* MysteryGiftSave::CreatePokemonParamFromGiftData( const MYSTERY_GIFT_POKE_DATA* pPokeData , gfl2::heap::HeapBase * heap , s32 level )
{
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA * p_wonderpoke = pPokeData;

  Savedata::MyStatus * mystatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

  

  //InitialSpecで設定
  pml::pokepara::InitialSpec spec;

  //レベル
  if( level == 0 )
  {//ランダムでレベルを決定
    level = System::GflUse::GetPokemonCreateRand( pml::MAX_POKE_LEVEL ) + pml::MIN_POKE_LEVEL;
  }

  //個性乱数
  if( p_wonderpoke->rnd == 0 )
  {//ランダムを生成
    spec.personal_rnd = pml::pokepara::INIT_SPEC_PERSONALRND_RANDOM;
  }
  else
  {//そのまま設定
    spec.personal_rnd = p_wonderpoke->rnd;
  }

  //レア乱数
  if ( p_wonderpoke->isRare == 0 )
  {//直接指定
    spec.rare_rnd = p_wonderpoke->color_rnd;//色違いカラー乱数を指定
  }
  else if( p_wonderpoke->isRare == 2 )
  {//レアにする
    spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_TRUE;
  }
  else if( p_wonderpoke->isRare == 1 )
  {//ランダム
    spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_RANDOM;
  }
  else
  {//レアなし
    spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE;
  }

  //IDNo
  if( p_wonderpoke->parentID == 0 )
  {//プレイヤー自身のIDを設定
    spec.id =           mystatus->GetID();
  }
  else
  {//そのまま設定
    spec.id =           p_wonderpoke->parentID;
  }

  //モンスターNo
  spec.monsno =       static_cast<MonsNo>(p_wonderpoke->monsNo);
  //フォルムNo
  spec.formno =       p_wonderpoke->formNo;


  //性別
  spec.sex  =         p_wonderpoke->sex == 3 ? pml::pokepara::INIT_SPEC_SEX_BOTH : p_wonderpoke->sex;
  //性格
  spec.seikaku =      p_wonderpoke->seikaku == 255 ? pml::pokepara::INIT_SPEC_SEIKAKU_RANDOM : p_wonderpoke->seikaku;

  //特性
  switch( p_wonderpoke->speabino )
  {
  case 0: case 1: case 2:
    spec.tokusei_index = p_wonderpoke->speabino;//普通に設定
    break;
  case 3:
    spec.tokusei_index = pml::pokepara::INIT_SPEC_TOKUSEI_1_OR_2;//0か1
    break;
  case 4:
    spec.tokusei_index = pml::pokepara::INIT_SPEC_TOKUSEI_1_OR_2_OR_3;//0か1か2
    break;
  };

  //個体値
  spec.talentPower[pml::pokepara::POWER_HP] = p_wonderpoke->hp_rnd;
  spec.talentPower[pml::pokepara::POWER_ATK] = p_wonderpoke->pow_rnd;
  spec.talentPower[pml::pokepara::POWER_DEF] = p_wonderpoke->def_rnd;
  spec.talentPower[pml::pokepara::POWER_SPATK] = p_wonderpoke->spepow_rnd;
  spec.talentPower[pml::pokepara::POWER_SPDEF] = p_wonderpoke->spedef_rnd;
  spec.talentPower[pml::pokepara::POWER_AGI] = p_wonderpoke->agi_rnd;

  u8 v_num = 0;  //個体値のランダムV指定があるかどうかチェック
  for( u32 index = 0 ; index < pml::pokepara::POWER_NUM ; ++index )
  {
    //数をカウント 一番初めに見つかった値設定優先
    if( spec.talentPower[index] >= static_cast<u16>(Savedata::MysteryGiftSave::TALENT_V_1) &&
        spec.talentPower[index] <= static_cast<u16>(Savedata::MysteryGiftSave::TALENT_V_3)
      ){
      v_num = spec.talentPower[index]-static_cast<u16>(Savedata::MysteryGiftSave::TALENT_V_1)+1;
      break;
    }
  }

  if( v_num > 0 )
  {//ランダムV指定あり
    for( u32 index = 0 ; index < pml::pokepara::POWER_NUM ; ++index )
    {
      spec.talentPower[index] = pml::pokepara::INIT_SPEC_TALENT_RANDOM;
    }
    spec.talentVNum = v_num;
  }else{
    for( u32 index = 0 ; index < pml::pokepara::POWER_NUM ; ++index )
    { //
      if( spec.talentPower[index] > pml::MAX_TALENT_POWER ) //最大値以上の指定を見つけたらランダム指定に挿げ替える
      {
        spec.talentPower[index] = pml::pokepara::INIT_SPEC_TALENT_RANDOM;
      }
    }
  }

  spec.level = level;
  if( p_wonderpoke->egg )
  {//タマゴである
    spec.level = 1;
  }
  

  //spec.rare_try_count   default
  //spec.familiarity      default

  //PokemonParamで設定
  pml::pokepara::PokemonParam * pp;
  pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, spec );

  //親の情報
  gfl2::str::StrBuf tmpBuf( 32, heap );
  if( p_wonderpoke->oyanam[0] == 0 )
  {//プレイヤーの名前を入れる
    mystatus->GetNameString( &tmpBuf );
  }
  else
  {//そのまま入れる
    tmpBuf.SetStr( p_wonderpoke->oyanam );
  }
  pp->SetParentName( &tmpBuf );

  //親の性別
  if( p_wonderpoke->oyasex == 0 || p_wonderpoke->oyasex == 1 )
  {//男or女
    pp->SetParentSex( static_cast<pml::Sex>(p_wonderpoke->oyasex) );
  }
  else
  {//プレイヤーの性別を入れる
    pp->SetParentSex( static_cast<pml::Sex>( mystatus->GetSex() ) );
  }

  //捕まえた時のボール
  if( p_wonderpoke->ballID == 0 )
  {//0ならばモンスターボールにする
    pp->SetGetBall( item::BALLID_MONSUTAABOORU );
  }
  else
  {//普通に設定
    pp->SetGetBall( p_wonderpoke->ballID );
  }

  //アイテムでフォルムが変わる場合
  FormNo itemFormNo = pp->GetNextFormNoFromHoldItem( p_wonderpoke->itemID );
  if( pp->GetFormNo() != itemFormNo )
  {
    pp->ChangeFormNo( itemFormNo );
  }
  //持ち物
  pp->SetItem( p_wonderpoke->itemID );

  //捕まえたレベル
  pp->SetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL , p_wonderpoke->getLevel );  ///<捕まえたレベル

  //言語コード
  if( p_wonderpoke->countryCode == 0 )
  {//0ならばプレイヤーの言語コードを入れる
    pp->SetLangId( mystatus->GetPokeLanguageId() );
  }
  else
  {
    pp->SetLangId( p_wonderpoke->countryCode );
  }

  //コンディション
  pp->SetCondition( pml::pokepara::CONDITION_STYLE , p_wonderpoke->stype );
  pp->SetCondition( pml::pokepara::CONDITION_BEAUTIFUL , p_wonderpoke->beautiful );
  pp->SetCondition( pml::pokepara::CONDITION_CUTE , p_wonderpoke->cute );
  pp->SetCondition( pml::pokepara::CONDITION_CLEVER , p_wonderpoke->clever );
  pp->SetCondition( pml::pokepara::CONDITION_STRONG , p_wonderpoke->strong );
  pp->SetCondition( pml::pokepara::CONDITION_FUR , p_wonderpoke->fur );

  //リボン
  const s32 RIBBON_LIST[] = //リボン15種類
  {
    PokeTool::RIBBON_NO_BATTLE_CHAMP,     ///< 33バトルチャンプリボン（旧マリンリボン）
    PokeTool::RIBBON_NO_AREA_CHAMP,       ///< 34エリアチャンプリボン（旧ランドリボン）
    PokeTool::RIBBON_NO_NATIONAL_CHAMP,   ///< 35ナショナルチャンプリボン（旧スカイリボン）
    PokeTool::RIBBON_NO_COUNTRY,          ///< 22カントリーリボン
    PokeTool::RIBBON_NO_NATIONAL,         ///< 23ナショナルリボン
    PokeTool::RIBBON_NO_EARTH,            ///< 24アースリボン
    PokeTool::RIBBON_NO_WORLD,            ///< 25ワールドリボン
    PokeTool::RIBBON_NO_EVENT,            ///< 28イベントリボン（旧ヒストリーリボン）
    PokeTool::RIBBON_NO_WORLD_CHAMP,      ///< 36ワールドチャンプリボン（旧レッドリボン）
    PokeTool::RIBBON_NO_BIRTHDAY,         ///< 29バースデーリボン（旧グリーンリボン）
    PokeTool::RIBBON_NO_SPECIAL,          ///< 30スペシャルリボン（旧ブルーリボン）
    PokeTool::RIBBON_NO_MEMORIAL,         ///< 31メモリアルリボン（旧フェスティバルリボン）
    PokeTool::RIBBON_NO_WISH,             ///< 32ウィッシュリボン（旧カーニバルリボン）
    PokeTool::RIBBON_NO_CLASSIC,          ///< 26クラシックリボン
    PokeTool::RIBBON_NO_PREMIERE,         ///< 27プレミアリボン
  };

  for( u32 index = 0 ; index < GFL_NELEMS(RIBBON_LIST) ; ++index )
  {
    u32 flag = p_wonderpoke->ribbonID & ( 1 << index );
    //GFL_PRINT( "ribbon[%d][%d][%d]",p_wonderpoke->ribbonID,( 1 << index ) ,flag);
    if( flag != 0 )
    {//iビット目が立っている
      pp->SetRibbon( RIBBON_LIST[index] );
      //GFL_PRINT("ribbon %d bit enable\n",index);
    }
  }

  //タマゴ
  if( p_wonderpoke->egg )
  {//タマゴである
    pp->SetEggFlag();

    gfl2::str::StrBuf nickname( 32, heap );
    pml::personal::GetMonsName( &nickname, MONSNO_TAMAGO );
    pp->SetNickName( &nickname );
  }
  else if( gfl2::str::StrLen(p_wonderpoke->nickName) != 0 )
  {//ニックネーム
    gfl2::str::StrBuf * strbuf;
    strbuf = GFL_NEW( heap ) gfl2::str::StrBuf( p_wonderpoke->nickName, heap );
    pp->SetNickName( strbuf );
    GFL_DELETE strbuf;
  }

  //技
  pp->SetWaza( 0 , (WazaNo)p_wonderpoke->waza1 );
  pp->SetWaza( 1 , (WazaNo)p_wonderpoke->waza2 );
  pp->SetWaza( 2 , (WazaNo)p_wonderpoke->waza3 );
  pp->SetWaza( 3 , (WazaNo)p_wonderpoke->waza4 );

  //卵技
  pp->SetTamagoWazaNo( 0 , (WazaNo)p_wonderpoke->tamago_waza1 );
  pp->SetTamagoWazaNo( 1 , (WazaNo)p_wonderpoke->tamago_waza2 );
  pp->SetTamagoWazaNo( 2 , (WazaNo)p_wonderpoke->tamago_waza3 );
  pp->SetTamagoWazaNo( 3 , (WazaNo)p_wonderpoke->tamago_waza4 );

  //ROMバージョン
  if( p_wonderpoke->version == 0 )
  {
    pp->SetCassetteVersion( System::GetVersion() );
  }
  else
  {
    pp->SetCassetteVersion( p_wonderpoke->version );
  }

  //タマゴだったらなつき度に孵化歩数を入れる
  if( p_wonderpoke->egg )
  {
    pml::personal::PersonalData *personal_data  = GFL_NEW(heap) pml::personal::PersonalData( heap );
    personal_data->LoadData( (MonsNo)p_wonderpoke->monsNo , 0 );
    pp->SetFamiliarity( personal_data->GetParam( pml::personal::PARAM_ID_egg_birth ) );
    GFL_SAFE_DELETE( personal_data );
  }


  if( !mystatus->IsMyPokemon( *pp ) )
  {//親が別にいる
    pp->SetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_PARENT , p_wonderpoke->memories_level );///<親との思い出レベル
    pp->SetMemories( pml::pokepara::MEMORIES_CODE_WITH_PARENT , p_wonderpoke->memories_code );  ///<親との思い出事象コード
    pp->SetMemories( pml::pokepara::MEMORIES_DATA_WITH_PARENT , p_wonderpoke->memories_data );  ///<親との思い出内容データ
    pp->SetMemories( pml::pokepara::MEMORIES_FEEL_WITH_PARENT , p_wonderpoke->memories_feel );  ///<親との思い出気持ちデータ

    //過去親情報
    /*
    SetPastParentsName
    SetPastParentsSex
    SetOthersFamiliarity
    SetOthersFriendship
    SetFriendlyTrainerID
    */
  }

  //フォルム、性別等でありえない組み合わせを補正する
  pp->RegulateFormParams();

  if ( ! p_wonderpoke->egg )
  {
    //親情報を更新する
    pml::pokepara::OwnerInfo ownerInfo;
    mystatus->CreateOwnerInfo( ownerInfo );
    pp->UpdateOwnerInfo( &ownerInfo );
  }

  //配信フラグ
  pp->SetEventPokeFlag( true );//かならず立てる

  //努力値
  {
    pp->ChangeEffortPower( pml::pokepara::POWER_HP , p_wonderpoke->hp_effort );
    pp->ChangeEffortPower( pml::pokepara::POWER_ATK , p_wonderpoke->atk_effort );
    pp->ChangeEffortPower( pml::pokepara::POWER_DEF , p_wonderpoke->def_effort );
    pp->ChangeEffortPower( pml::pokepara::POWER_AGI , p_wonderpoke->agi_effort );
    pp->ChangeEffortPower( pml::pokepara::POWER_SPATK , p_wonderpoke->satk_effort );
    pp->ChangeEffortPower( pml::pokepara::POWER_SPDEF , p_wonderpoke->sdef_effort );
  }

  return pp;
}

//======================================================================================================================
// for SaveData
//======================================================================================================================

//=============================================================================================
/**
 * [for SaveData]  保存するデータバイナリへのポインタを返す
 *
 * @retval        void*
 */
//=============================================================================================
void*  MysteryGiftSave::GetData( void )
{
  return reinterpret_cast<void*>(&m_coreData);
}

//=============================================================================================
/**
 * [for SaveData]  保存するデータバイナリのサイズを返す
 *
 * @retval        u32
 */
//=============================================================================================
size_t MysteryGiftSave::GetDataSize( void )
{
  return sizeof(m_coreData);
}


//=============================================================================================
/**
* [for SaveData]  セーブデータを初期化する
*
* @param heap
*/
//=============================================================================================
void    MysteryGiftSave::Clear( gfl2::heap::HeapBase * heap )
{
  KOUSAKA_PRINT( "MysteryGiftSave::CORE SIZE=%d\n", sizeof(Savedata::MysteryGiftSave::CORE_DATA));
  KOUSAKA_PRINT( "MysteryGiftSave::MYSTERY_GIFT_POKE_DATA SIZE=%d\n", sizeof(Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA));
  KOUSAKA_PRINT( "MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA SIZE=%d\n", sizeof(Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA));
  KOUSAKA_PRINT( "MysteryGiftSave::MYSTERY_GIFT_DATA SIZE=%d\n", sizeof(Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA));
  KOUSAKA_PRINT( "MysteryGiftSave::RANDOM_DELIVERY_FLAG SIZE=%d\n", sizeof(Savedata::MysteryGiftSave::RANDOM_DELIVERY_FLAG));
  KOUSAKA_PRINT( "MysteryGiftSave::SERIAL_DATA SIZE=%d\n", sizeof(Savedata::MysteryGiftSave::SERIAL_DATA));

  ::std::memset( &m_coreData, 0x00, sizeof(m_coreData) );

  m_coreData.serialData.Clear();
}

//=============================================================================================
/**
*  不思議なおくりものデータ空き数取得（削除可能な数も合算）
*/
//=============================================================================================
u32 MysteryGiftSave::GetGiftDataVacanciesWithDeletables(void)
{
  const u32 dataNum   = GetGiftDataNum();
  u32       vacancies = 0;

  for(u32 index = 0; index < dataNum; ++index)
  {
    if(DeleteGiftData(index, false))
    {
      ++vacancies;
    }
  }

  return (vacancies + GetGiftDataVacancies());
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
MysteryGiftSave::ReceiveCheckResult   MysteryGiftSave::ReceiveCheck(const MYSTERY_GIFT_RECV_DATA* pTargetData)
{
  return ReceiveCheck(&(pTargetData->dataBody));
}


//=============================================================================================
/**
*  受信可能なおくりものかどうか判定
*/
//=============================================================================================
MysteryGiftSave::ReceiveCheckResult   MysteryGiftSave::ReceiveCheck(const MYSTERY_GIFT_DATA* pTargetData)
{
  Savedata::MysteryGiftSave * pData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetMysteryGiftSave();

  u16 eventID = pTargetData->eventID;

  
  u32 recvFlagBit = pTargetData->recvFlag & 1;
  if( recvFlagBit )
  {//一度だけ受け取れるおくりもの
    //受け取り済みチェック
    s32 flagIndex = 0;
    for( u32 i = 0 ; i < RECV_FLAG_NUM ; ++i )
    {
      if( eventID < (i+1) * RECV_FLAG_BIT )
      {
        flagIndex = i;
        break;
      }
    }

    bool isRecvEnable = false;
    if( flagIndex >= 0 && flagIndex < RECV_FLAG_NUM )
    {
      u32 flagBit = eventID % RECV_FLAG_BIT;
      u32 flagBitResult = ( 1 << flagBit );
      u32 flagTemp = pData->m_coreData.recvFlag[flagIndex] & flagBitResult;
      GFL_PRINT("WonderGiftProc::SeqWaitSave : recv flag check[%d][%d][%d][%d]\n" , flagIndex , eventID , flagBitResult , pData->m_coreData.recvFlag[flagIndex] );
      if( flagTemp != 0 )
      {//もう受け取っている
        return RECEIVE_CHECK_RESULT_NG;
      }
    }
  }

  //一日1回だけチェック
  u32 recvFlagBitToday = pTargetData->recvFlag & 4;
  if( recvFlagBitToday )
  {
    bool isEmptyExists = false;
    u64 dataTime = ( pTargetData->GetYear() % 100 ) * 10000 + pTargetData->GetMonth()*100 + pTargetData->GetDay();
    for( u32 index = 0 ; index < ONE_DAY_FLAG_NUM ; ++index )
    {
      ONE_DAY_FLAG* oneDayFlag = &pData->m_coreData.oneDayFlag[index];
      u32 recvTime = oneDayFlag->year * 10000 + oneDayFlag->month*100 + oneDayFlag->day;
      if( oneDayFlag->eventID == eventID )
      {//イベントID一致
        if( dataTime <= recvTime )
        {//一日たってない
          return RECEIVE_CHECK_RESULT_NG_TODAY;
        }
      }

      //履歴をチェックしておく
      if( dataTime > recvTime )
      {
        isEmptyExists = true;//上書き可能なデータが少なくとも1件ある 
      }
    }

    if( isEmptyExists == false )
    {//履歴に空きがない
      return REDEIVE_CHECK_RESULT_NG_TODAY_FULL;
    }
  }


  //アルバム一杯チェック
  if( pData->GetGiftDataVacancies() == 0 )
  {
    return RECEIVE_CHECK_RESULT_NG_FULL;
  }


  return RECEIVE_CHECK_RESULT_OK;
}


//------------------------------------------------------------------
/**
  *  @brief    commitSave巻き戻し用Cloneの作成
  */
//------------------------------------------------------------------
MysteryGiftSave::CORE_DATA*  MysteryGiftSave::CreateClone(gfl2::heap::HeapBase* pHeapBase)
{
  CORE_DATA*  pCoreData = NULL;

  GFL_PRINT("%s\n", __FUNCTION__);
  GFL_ASSERT(sizeof(m_coreData) == sizeof(CORE_DATA));

  pCoreData = GFL_NEW(pHeapBase)  CORE_DATA;
  SetupClone(*pCoreData);

  return pCoreData;
}


//------------------------------------------------------------------
/**
  *  @brief    commitSave巻き戻し用Cloneのセット
  */
//------------------------------------------------------------------
void  MysteryGiftSave::SetupClone(CORE_DATA& rCoreData)
{
  memcpy(&rCoreData, &m_coreData, sizeof(rCoreData));
}


//------------------------------------------------------------------
/**
  *  @brief    commitSave巻き戻し用Cloneを書き戻す
  */
//------------------------------------------------------------------
void  MysteryGiftSave::WriteBackClone(const CORE_DATA* pCoreData)
{
  GFL_PRINT("%s\n", __FUNCTION__);
  GFL_ASSERT(pCoreData);
  GFL_ASSERT(sizeof(m_coreData) == sizeof(CORE_DATA));

  if(pCoreData)
  {
    memcpy(&m_coreData, pCoreData, sizeof(m_coreData));
    GFL_PRINT("%s: writeback\n", __FUNCTION__);
  }
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  MysteryGiftSave::SERIAL_DATA::Clear(void)
{
  failedYear    = 2000;   ///<失敗したサーバ時間(UTC)：年
  failedMonth   =    1;   ///<失敗したサーバ時間(UTC)：月
  failedDay     =    1;   ///<失敗したサーバ時間(UTC)：日
  failedHour    =    0;   ///<失敗したサーバ時間(UTC)：時
  failedMinute  =    0;   ///<失敗したサーバ時間(UTC)：分
  NGflag        = false;  ///<シリアルＮＧフラグ
  NGCounter     =    0;   ///<NGカウンター
}


//------------------------------------------------------------------
/**
  *  @brief    シリアル認証結果を渡し、内部状態を更新する
  */
//------------------------------------------------------------------
bool  MysteryGiftSave::SERIAL_DATA::UpdateSerialAuthStatus(const bool isSerialOK, const u16 serverTime_Year, const u8 serverTime_Month, const u8 serverTime_Day, const u8 serverTime_Hour, const u8 serverTime_Minute)
{
/*
  開発中のallZeroなfailed系変数がセーブされたデータへの対策のため、デバッグ時のみ復旧処理を入れる
*/
#if PM_DEBUG
{
  if(!failedYear && !failedMonth && !failedDay && !failedHour && !failedMinute)
  {
    failedYear    = 2000;   ///<失敗したサーバ時間(UTC)：年
    failedMonth   =    1;   ///<失敗したサーバ時間(UTC)：月
    failedDay     =    1;   ///<失敗したサーバ時間(UTC)：日
    failedHour    =    0;   ///<失敗したサーバ時間(UTC)：時
    failedMinute  =    0;   ///<失敗したサーバ時間(UTC)：分
    GFL_PRINT("%s: reset failedYear/Month/Day/Hour/Minute\n", __FUNCTION__);
  }
}
#endif


  const gfl2::system::Date  serverTime(serverTime_Year, serverTime_Month, serverTime_Day, serverTime_Hour, serverTime_Minute);
  const gfl2::system::Date  lastNGTime(failedYear, failedMonth, failedDay, failedHour, failedMinute);


  /*  開発用Assert  */
  GFL_ASSERT(serverTime.IsValidDate());
  GFL_ASSERT(lastNGTime.IsValidDate());


  /*  先回の失敗から一定時間経過していればペナルティリセット  */
  QueryTimeoutPenalty(serverTime_Year, serverTime_Month, serverTime_Day, serverTime_Hour, serverTime_Minute);

  if(isSerialOK)
  {
    /*  認証OK：ペナルティ状態をリセット  */
    GFL_PRINT("%s: set free (serial OK)\n", __FUNCTION__);
    ClearPenaltyStatus();
  }
  else
  {
    /*  認証NG：ペナルティ更新  */
    if(NGCounter < SERIAL_AUTH_FAILED_COUNT_TO_PENALTY)
    {
      ++NGCounter;
    }

    failedYear    = static_cast<u16>(serverTime.GetYear());   ///<失敗したサーバ時間(UTC)：年
    failedMonth   = static_cast< u8>(serverTime.GetMonth());  ///<失敗したサーバ時間(UTC)：月
    failedDay     = static_cast< u8>(serverTime.GetDay());    ///<失敗したサーバ時間(UTC)：日
    failedHour    = static_cast< u8>(serverTime.GetHour());   ///<失敗したサーバ時間(UTC)：時
    failedMinute  = static_cast< u8>(serverTime.GetMinute()); ///<失敗したサーバ時間(UTC)：分
    
    GFL_PRINT(
      "%s: NGCount: %d, date: %4d/%02d/%02d %02d:%02d\n",
      __FUNCTION__,
      NGCounter,
      static_cast<s32>(failedYear),
      static_cast<s32>(failedMonth),
      static_cast<s32>(failedDay),
      static_cast<s32>(failedHour),
      static_cast<s32>(failedMinute)
    );
    
    NGflag  = (SERIAL_AUTH_FAILED_COUNT_TO_PENALTY <= NGCounter);
    GFL_PRINT("%s: penalty: %s\n", __FUNCTION__, NGflag ? "true" : "false");
  }


  return IsTimeoutPenalty();
}


//------------------------------------------------------------------
/**
  *  @brief    ペナルティ状態を更新
  */
//------------------------------------------------------------------
bool  MysteryGiftSave::SERIAL_DATA::QueryTimeoutPenalty(const u16 serverTime_Year, const u8 serverTime_Month, const u8 serverTime_Day, const u8 serverTime_Hour, const u8 serverTime_Minute)
{
/*
  開発中のallZeroなfailed系変数がセーブされたデータへの対策のため、デバッグ時のみ復旧処理を入れる
*/
#if PM_DEBUG
{
  if(!failedYear && !failedMonth && !failedDay && !failedHour && !failedMinute)
  {
    failedYear    = 2000;   ///<失敗したサーバ時間(UTC)：年
    failedMonth   =    1;   ///<失敗したサーバ時間(UTC)：月
    failedDay     =    1;   ///<失敗したサーバ時間(UTC)：日
    failedHour    =    0;   ///<失敗したサーバ時間(UTC)：時
    failedMinute  =    0;   ///<失敗したサーバ時間(UTC)：分
    GFL_PRINT("%s: reset failedYear/Month/Day/Hour/Minute\n", __FUNCTION__);
  }
}
#endif

  const gfl2::system::Date  serverTime(serverTime_Year, serverTime_Month, serverTime_Day, serverTime_Hour, serverTime_Minute);
  const gfl2::system::Date  lastNGTime(failedYear, failedMonth, failedDay, failedHour, failedMinute);
  const s32                 serverTimeSecond  = serverTime.GetDateTimeToSecond();
  const s32                 lastNGTimeSecond  = lastNGTime.GetDateTimeToSecond();

  const bool                bTimeCheck        = (lastNGTimeSecond <= serverTimeSecond);
  const bool                bLocalTimeCheck   = (0 < lastNGTimeSecond);
  const bool                bEasyCheck        = (0 < serverTimeSecond) && bTimeCheck && bLocalTimeCheck;   /*  値の簡易チェック  */

  /*  開発用Assert  */
  GFL_ASSERT(serverTime.IsValidDate());
  GFL_ASSERT(lastNGTime.IsValidDate());
  GFL_ASSERT(bTimeCheck);

  if(bEasyCheck)
  {
    const s64   elapsedSeconds = (serverTimeSecond - lastNGTimeSecond);   /*  経過秒数  */

    if(SERIAL_AUTH_TIMEOUT_PENALTY_SECONDS <= elapsedSeconds)
    {
      GFL_PRINT("%s: set free (timespan: %lld)\n", __FUNCTION__, elapsedSeconds);
      ClearPenaltyStatus();
    }
  }
  else if((!bTimeCheck) || (!bLocalTimeCheck))
  {
    /*
      GFNMcat[4811]   「ふしぎなおくりもの」でローカルに保持されている日時より、サーバーの日時が過去になった場合にアサートが発生する
      救済措置
      何らかの理由によりlastNGTimeが異常値となり、例えばはるか未来の値となった場合、ペナルティが延々と続いてしまう可能性が考えられるため、
      救済措置としてペナルティをはずす
      Assert自体は残す
    */
    GFL_PRINT("%s: forced ClearPenaltyStatus\n", __FUNCTION__);
    ClearPenaltyStatus();
  }
  
  return IsTimeoutPenalty();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  MysteryGiftSave::Debug_ClearSerialNGPenalty(void)
{
  /*  unlock  */
  m_coreData.serialData.Clear();
}
#endif


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  MysteryGiftSave::Debug_SetRecieveOnceFlag(const u16 targetEventID, const bool isReceived)
{
  const bool isValidEventID = IsValidEventID(targetEventID);
  
  GFL_ASSERT(isValidEventID);
  if(isValidEventID)
  {
    const u32   flagArrayElementBits  = sizeof(m_coreData.recvFlag[0]) * 8;
    const u32   flagArrayIndex        = targetEventID / flagArrayElementBits;
    const u32   flagBitShift          = targetEventID % flagArrayElementBits;
    const u32   flagBit               = (0x01 << flagBitShift);
    const bool  isValidArrayIndex     = (flagArrayIndex < GFL_NELEMS(m_coreData.recvFlag));
    
    GFL_ASSERT(isValidArrayIndex);
    if(!isValidArrayIndex)  return;

    m_coreData.recvFlag[flagArrayIndex] &= ~flagBit;
    if(isReceived)  m_coreData.recvFlag[flagArrayIndex]  |= flagBit;
  }
}
#endif


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
bool  MysteryGiftSave::Debug_IsRecieveOnceFlag(const u16 targetEventID /* < 2047 */) const
{
  const bool isValidEventID = IsValidEventID(targetEventID);
  bool        flag          = false;
  
  GFL_ASSERT(isValidEventID);
  if(isValidEventID)
  {
    const u32   flagArrayElementBits  = sizeof(m_coreData.recvFlag[0]) * 8;
    const u32   flagArrayIndex        = targetEventID / flagArrayElementBits;
    const u32   flagBitShift          = targetEventID % flagArrayElementBits;
    const u32   flagBit               = (0x01 << flagBitShift);
    const bool  isValidArrayIndex     = (flagArrayIndex < GFL_NELEMS(m_coreData.recvFlag));
    
    GFL_ASSERT(isValidArrayIndex);
    if(!isValidArrayIndex)  return false;

    flag  = ((m_coreData.recvFlag[flagArrayIndex] & flagBit) != 0);
  }
  
  return flag;
}
#endif


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
const MysteryGiftSave::RANDOM_DELIVERY_FLAG& MysteryGiftSave::Debug_GetRandomDeliveryFlag(const u32 randomDeliveryFlagIndex)
{
  const bool isValidIndex = (randomDeliveryFlagIndex < GFL_NELEMS(m_coreData.randomDeliveryFlag));
  
  GFL_ASSERT(isValidIndex);
  return m_coreData.randomDeliveryFlag[isValidIndex ? randomDeliveryFlagIndex : 0];
}
#endif


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  MysteryGiftSave::Debug_DeleteRandomDeliveryFlag(const u32 randomDeliveryFlagIndex)
{
  const u32   numOfElems    = GFL_NELEMS(m_coreData.randomDeliveryFlag);
  const bool  isValidIndex  = (randomDeliveryFlagIndex < numOfElems);
  
  GFL_ASSERT(isValidIndex);

  /*  前詰め  */
  if(randomDeliveryFlagIndex < (numOfElems-1))
  {
    const u32   srcIndex  = randomDeliveryFlagIndex+1;
    void*       pDst      = &(m_coreData.randomDeliveryFlag[randomDeliveryFlagIndex]);
    const void* pSrc      = &(m_coreData.randomDeliveryFlag[srcIndex]);
    const u32   bytes     = sizeof(m_coreData.randomDeliveryFlag[0]) * (numOfElems - srcIndex);
    
    memmove(pDst, pSrc, bytes);
  }
  /*  末尾の塗りつぶし  */
  memset(&(m_coreData.randomDeliveryFlag[numOfElems-1]), 0, sizeof(m_coreData.randomDeliveryFlag[0]));
}
#endif




#if PM_DEBUG
const MysteryGiftSave::ONE_DAY_FLAG&   MysteryGiftSave::Debug_GetOneDayFlag(const u32 oneDayFlagIndex /* < 10 */) const
{
  const bool  isValidIndex  = oneDayFlagIndex < GFL_NELEMS(m_coreData.oneDayFlag);
  GFL_ASSERT(isValidIndex);
  return m_coreData.oneDayFlag[isValidIndex ? oneDayFlagIndex : 0];
}
#endif

#if PM_DEBUG
void  MysteryGiftSave::Debug_ClearOneDayFlag(const u32 oneDayFlagIndex)
{
  const bool  isValidIndex  = oneDayFlagIndex < GFL_NELEMS(m_coreData.oneDayFlag);
  GFL_ASSERT(isValidIndex);

  if(isValidIndex)
    memset(&(m_coreData.oneDayFlag[oneDayFlagIndex]), 0, sizeof(m_coreData.oneDayFlag[oneDayFlagIndex]));
}
#endif

#if PM_DEBUG
void  MysteryGiftSave::Debug_ClearAllOneDayFlag(void)
{
  memset(&(m_coreData.oneDayFlag), 0, sizeof(m_coreData.oneDayFlag));
}
#endif






















//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  MysteryGiftSave::MYSTERY_GIFT_RECV_DATA::CRCCheck(u16* pCRCOut)
{
#if defined(GF_PLATFORM_CTR)
  const u16 storedCRC = crc;
  crc = 0;

  const u32 calcCRC   = nn::util::Crc16Ccitt::Calculate(this, sizeof(MYSTERY_GIFT_RECV_DATA));
  crc = storedCRC;

  GFL_PRINT("MYSTERY_GIFT_RECV_DATA::CRCCheck: (size, data, calc) : (%u, 0x%04x,0x%04x)\n", sizeof(MYSTERY_GIFT_RECV_DATA), storedCRC, calcCRC);

  if(pCRCOut) *pCRCOut  = calcCRC;

  return (storedCRC == calcCRC);
#else
  return true;
#endif
}


GFL_NAMESPACE_END(Savedata)

