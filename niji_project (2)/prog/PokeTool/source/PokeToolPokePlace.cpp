//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		poke_place.cpp
 *	@brief  ポケモンの捕まえた場所のメッセージデータ取得
 *	@author	Toru=Nagihashi
 *	@date		2012.05.08
 *	@note   静的クラスです。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// niji
#include "../include/PokeToolPokePlace.h"

#include <Print/include/PrintSystem.h>
#include <System/include/PokemonVersion.h>

// arc
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN(PokeTool)

gfl2::str::MsgData *PokePlace::m_place_msg  = NULL;
gfl2::str::MsgData *PokePlace::m_spe_msg     = NULL;
gfl2::str::MsgData *PokePlace::m_out_msg    = NULL;
gfl2::str::MsgData *PokePlace::m_per_msg    = NULL;
  
/**
 * @brief 初期化
 *
 * @param heap    ヒープ
 * @param type    読み込みタイプ
 */
void PokePlace::Initialize( gfl2::heap::HeapBase *heap, LoadType type )
{
  if( type != LOAD_TYPE_REALTIME )
  {
    gfl2::str::MsgData::LoadType load_type = gfl2::str::MsgData::LOAD_PART;
    
    if( type == LOAD_TYPE_FULL )
    {
      load_type = gfl2::str::MsgData::LOAD_FULL;
    }

    m_place_msg  = GFL_NEW(heap) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_place_name_DAT, heap, load_type );
    m_spe_msg     = GFL_NEW(heap) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_place_name_spe_DAT, heap, load_type );
    m_out_msg    = GFL_NEW(heap) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_place_name_out_DAT, heap, load_type );
    m_per_msg    = GFL_NEW(heap) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_place_name_per_DAT, heap, load_type );
  }
}

/**
 * @brief 破棄
 */
void PokePlace::Finalize( void )
{
  GFL_SAFE_DELETE( m_place_msg );
  GFL_SAFE_DELETE( m_spe_msg );
  GFL_SAFE_DELETE( m_out_msg );
  GFL_SAFE_DELETE( m_per_msg );
}

/**
 *  @bief ポケモン捕まえた場所のアーカイブ内データのID取得
 *  @retval ポケモン捕まえた場所のアーカイブ内データのID
 */
gfl2::str::MsgData * PokePlace::GetMessageDatID( u32 place, gfl2::fs::ArcFile::ARCDATID * datID )
{
  if( place < PLACE_NO_START_SPECIAL )
  {
    if( place >= msg_place_name_max )
    {
      *datID = GARC_message_place_name_DAT;
      return m_place_msg;
    }
    else
    {
      *datID = GARC_message_place_name_DAT;
      return m_place_msg;
    }
  }
  else if( place < PLACE_NO_START_OUTER )
  {
    u32 temp = place-PLACE_NO_START_SPECIAL;
    if( temp >= msg_place_name_spe_max )
    {
      *datID = GARC_message_place_name_DAT;
      return m_place_msg;
    }
    else
    {
      *datID = GARC_message_place_name_spe_DAT;
      return m_spe_msg;
    }
  }
  else if( place < PLACE_NO_START_PERSON )
  {
    u32 temp = place-PLACE_NO_START_OUTER;
    if( temp >= msg_place_name_out_max )
    {
      *datID = GARC_message_place_name_DAT;
      return m_place_msg;
    }
    else
    {
      *datID = GARC_message_place_name_out_DAT;
      return m_out_msg;
    }
  }
  else if( place <= PLACE_NO_END )
  {
    u32 temp = place-PLACE_NO_START_PERSON;
    if( temp >= msg_place_name_per_max )
    {
      *datID = GARC_message_place_name_per_DAT;
      return m_per_msg;
    }
    else
    {
      *datID = GARC_message_place_name_per_DAT;
      return m_per_msg;
    }
  }
  *datID = GARC_message_place_name_DAT;
  return m_place_msg;
}

/**
 *  @bief 上記のアーカイブデータ内のメッセージ番号取得
 *  @retval 上記のアーカイブデータ内のメッセージ番号
 */
u32 PokePlace::GetMessageID( u32 place )
{
  if( place < PLACE_NO_START_SPECIAL )
  {
    if( place == MAPNAME_NOTHING )
    {
      return MAPNAME_NAZONOBASYO;
    }
    else if( place >= msg_place_name_max )
    {
      return MAPNAME_TOOIBASYO;
    }
    else
    {
      return place;
    }
  }
  else if( place < PLACE_NO_START_OUTER )
  {
    u32 temp = place-PLACE_NO_START_SPECIAL;
    if( temp >= msg_place_name_spe_max )
    {
      return MAPNAME_TOOIBASYO;
    }
    else
    {
      return temp;
    }
  }
  else if( place < PLACE_NO_START_PERSON )
  {
    u32 temp = place-PLACE_NO_START_OUTER;
    if( temp >= msg_place_name_out_max )
    {
      return MAPNAME_TOOIBASYO;
    }
    else
    {
      return temp;
    }
  }
  else if( place <= PLACE_NO_END )
  {
    u32 temp = place-PLACE_NO_START_PERSON;
    if( temp >= msg_place_name_per_max )
    {
      return MAPNAME_TOOKUNIIRUHITO;
    }
    else
    {
      return temp;
    }
  }
  else
  {
    return MAPNAME_TOOIBASYO;
  }
}

/**
 *  @bief 地名の名前を生成（解放は各自行なってください）
 *  @retval 地名の名前
 */
gfl2::str::StrBuf * PokePlace::CreatePlaceName( u32 place_no, gfl2::heap::HeapBase *temporary_heap )
{
  gfl2::fs::ArcFile::ARCDATID arc_dat_id;
  gfl2::str::MsgData *place_msg = PokePlace::GetMessageDatID( place_no, &arc_dat_id );
  gfl2::str::MsgData *place_msg_buff = NULL;

  u32 msg_id = PokePlace::GetMessageID( place_no );

  if( place_msg == NULL )
  {
    place_msg_buff  = GFL_NEW(temporary_heap->GetLowerHandle()) gfl2::str::MsgData( print::GetMessageArcId(), arc_dat_id, temporary_heap->GetLowerHandle() );
  }
  else
  {
    place_msg_buff  = place_msg;
  }

  gfl2::str::StrBuf *str = GFL_NEW(temporary_heap) gfl2::str::StrBuf( STRBUF_PLACE_NAME_LEN, temporary_heap );
  place_msg_buff->GetString( msg_id, *str );

  if( place_msg == NULL )
  {
    GFL_SAFE_DELETE(place_msg_buff);
  }

  return str;
}


/**
 *  @bief バージョンの名前を生成（解放は各自行なってください）
 *  @retval バージョンの名前
 */
gfl2::str::StrBuf * PokePlace::CreateVersionName( u32 version, gfl2::heap::HeapBase *temporary_heap)
{
  gfl2::str::MsgData *place_msg;

  if( m_spe_msg == NULL )
  {
    place_msg  = GFL_NEW(temporary_heap->GetLowerHandle()) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_place_name_spe_DAT, temporary_heap->GetLowerHandle() );
  }
  else
  {
    place_msg = m_spe_msg;
  }
  
  gfl2::str::StrBuf *str = GFL_NEW(temporary_heap) gfl2::str::StrBuf( STRBUF_PLACE_NAME_LEN, temporary_heap );
  switch( version )
  {
  default:
    place_msg->GetString( MAPNAME_TOOKUHANARETATOTI, *str );
    //GFL_ASSERT(0);
    break;
  case VERSION_SAPPHIRE:   ///< AGBサファイア
  case VERSION_RUBY:       ///< AGBルビー
  case VERSION_EMERALD:    ///< AGBエメラルド
    place_msg->GetString( MAPNAME_HOUEN, *str );
    break;
  case VERSION_RED:        ///< AGBファイアレッド
  case VERSION_GREEN:      ///< AGBリーフグリーン
    place_msg->GetString( MAPNAME_KANTO, *str );
    break;
  case VERSION_GOLD:       ///< DSハートゴールド
  case VERSION_SILVER:     ///< DSソウルシルバー
    place_msg->GetString( MAPNAME_ZYOTO, *str );
    break;
  case VERSION_DIAMOND:    ///< DSダイヤモンド
  case VERSION_PEARL:      ///< DSパール
  case VERSION_PLATINUM:    ///< DSプラチナ
    place_msg->GetString( MAPNAME_SINOH, *str );
    break;
  case VERSION_COLOSSEUM:  ///< GCコロシアム
    place_msg->GetString( MAPNAME_TOOKUHANARETATOTI, *str );
    break;
  case VERSION_WHITE:      ///< DSホワイト
  case VERSION_BLACK:      ///< DSブラック
  case VERSION_W2:         ///< DSホワイト２
  case VERSION_B2:         ///< DSブラック２
    place_msg->GetString( MAPNAME_IISHU, *str );
    break;
  case VERSION_X:          ///< 3DSkujira X
  case VERSION_Y:          ///< 3DSkujira Y
    place_msg->GetString( MAPNAME_KAROSU, *str );
    break;
  case VERSION_CTR_SAPPHIRE:    ///< 3DS sango サファイア
  case VERSION_CTR_RUBY:        ///< 3DS sango ルビー
    place_msg->GetString( MAPNAME_HOUEN, *str );
    break;

  case VERSION_SUN:       ///< 3DS SUN
  case VERSION_MOON:      ///< 3DS MOON
  case VERSION_SUN2:      ///< 3DS momiji SUN
  case VERSION_MOON2:     ///< 3DS momiji MOON
    place_msg->GetString( MAPNAME_AROORA, *str );
    break;
  case VERSION_HOLOHOLO:  ///< Pokemon HOLOHOLO
    place_msg->GetString( MAPNAME_HOLOHOLO, *str );
    break;

  case VERSION_VC_RED:      ///< VC 赤
  case VERSION_VC_GREEN:    ///< VC 緑
  case VERSION_VC_BLUE:     ///< VC 青
  case VERSION_VC_PIKACHU:  ///< VC ぴかちゅう
    place_msg->GetString( MAPNAME_VCKANTOH, *str );
    break;
  case VERSION_VC_GOLD:     ///< VC 金
  case VERSION_VC_SILVER:   ///< VC 銀
  case VERSION_VC_CRISTAL:  ///< VC クリスタル
    place_msg->GetString( MAPNAME_VCZYOTO, *str );
    break;
  }
  
  if( m_spe_msg == NULL )
  {
    GFL_SAFE_DELETE(place_msg);
  }

  return str;
}



/**
 *  @bief    IDを六桁表示するバージョン
 *  @retval  ６けたならtrue
 */
bool PokePlace::GetVersionSixID( u32 version )
{
  if( IsAlola( version ) || IsHoloholo( version ) )
  {
    return true;
  }
  return false;
}

/**
 * @brief   カロス地方かどうか
 * @param   version   ROMバージョン
 * @retval  true = カロス地方
 */
bool PokePlace::IsCalos( u8 version )
{
  if( (version >= VERSION_CALOS_MIN)  && (version <= VERSION_CALOS_MAX) )
  {
    return true;
  }
  return false;
}

/**
 * @brief   アローラ地方かどうか
 * @param   version   ROMバージョン
 * @retval  true = アローラ地方
 */
bool PokePlace::IsAlola( u8 version )
{
  if( version == VERSION_SUN || version == VERSION_MOON ||  version == VERSION_SUN2 || version == VERSION_MOON2 )
  {
    return true;
  }
  return false;
}


/**
 * @brief   ホロホロ地方かどうか
 * @param   version   ROMバージョン
 * @retval  true = ホロホロ地方
 */
bool PokePlace::IsHoloholo( u8 version )
{
  if( version == VERSION_HOLOHOLO )
  {
    return true;
  }
  return false;
}


/**
 * @brief   バーチャルコンソール地方かどうか
 * @param   version   ROMバージョン
 * @retval  true = バーチャルコンソール地方
 */
bool PokePlace::IsVc( u8 version )
{
  if( (version >= VERSION_VC_RED)  && (version <= VERSION_VC_CRISTAL) )
  {
    return true;
  }
  return false;
}




/**
 * @brief   産地マークIDを取得
 * @param   version   ROMバージョン
 * @return  産地マークID
 */
PokePlace::PlaceMarkID PokePlace::GetPlaceMarkID( u8 version )
{
  if( IsCalos( version ) )
  {
    return PLACE_MARK_CALOS;
  }
  else if( IsAlola( version ) )
  {
    return PLACE_MARK_ALOLA;
  }
  else if( IsHoloholo( version ) )
  {
    return PLACE_MARK_HOLOHOLO;
  }
  else if( IsVc( version ) )
  {
    return PLACE_MARK_VC;
  }
  return PLACE_MARK_NONE;
}


GFL_NAMESPACE_END(PokeTool)
