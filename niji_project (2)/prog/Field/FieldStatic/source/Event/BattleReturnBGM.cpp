//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file BattleReturnBGM.cpp
 *  @brief バトル終了処理イベント内でのBGM管理
 *  @author saita_kazuki
 *  @date 2016.03.24
 */
//=============================================================================

#include "./BattleReturnBGM.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

// sound
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN( Field )

  
/**
 * @brief バトル勝利BGMに対応したバトル後専用勝利BGMの取得
 * @return バトル後専用勝利BGMのID
 */
u32 BattleReturnBGM::GetWinBGM( u32 battleWinBGM )
{
  switch( battleWinBGM )
  {
  case STRM_BGM_WIN1:
    return STRM_BGM_WIN1_ST;
  case STRM_BGM_WIN2:
    return STRM_BGM_WIN2_ST;
  case STRM_BGM_WIN3:
    return STRM_BGM_WIN3_ST;
  case STRM_BGM_WIN4:
    return STRM_BGM_WIN4_ST;
  case STRM_BGM_WIN5:
    return STRM_BGM_WIN5_ST;
  case STRM_BGM_WIN6:
    return STRM_BGM_WIN6_ST;
  case STRM_BGM_WIN7:
    return STRM_BGM_WIN7_ST;
  case STRM_BGM_WIN8:
    return STRM_BGM_WIN8_ST;
  case STRM_BGM_WIN9:
    return STRM_BGM_WIN9_ST;
  default:
    GFL_ASSERT( 0 );
    return SMID_NULL; // 不正なバトル勝利BGM
  }
}

/**
 * @brief コンストラクタ
 */
BattleReturnBGM::BattleReturnBGM( const Description& desc )
  : m_desc( desc )
  , m_seq( SEQ_BATTLE_WIN )
{
}

/**
 * @brief デストラクタ
 */
BattleReturnBGM::~BattleReturnBGM()
{
}

/**
 * @brief BGMを変更する
 * @param next 次のBGMシーケンス
 */
void BattleReturnBGM::ChangeBGMReq( Seq next )
{
  // 変更タイプを取得
  ChangeType type = this->GetChangeType( next );

  switch( type )
  {
  case CHANGE_TYPE_NONE:
    // 何もしない
    break;

  case CHANGE_TYPE_NORMAL:
    this->ChangeBGMReq_Normal( next, m_desc.battleWinBGM );
    break;

  case CHANGE_TYPE_SILENCE:
    this->ChangeBGMReq_Silence( next );
    break;

  default:
    GFL_ASSERT( 0 ); // 不正な変更タイプ
    break;
  }
}

/**
 * @brief BGMを変更する必要があるか
 * @param next 次のBGMシーケンス
 * @retval true 変更する必要がある
 * @retval false 変更する必要がない(現在のBGMを継続)
 */
BattleReturnBGM::ChangeType BattleReturnBGM::GetChangeType( Seq next ) const
{
  if( next >= SEQ_MAX)
  {
    GFL_ASSERT( 0 ); // 不正なシーケンス
    return CHANGE_TYPE_NONE;
  }

  static const BGMPattern PATTERN_TABLE[] =
  {
    // now , next , type
    { SEQ_BATTLE_WIN, SEQ_EVOLVE, CHANGE_TYPE_SILENCE }, // 戦闘勝利→進化
    { SEQ_BATTLE_WIN, SEQ_ZUKAN , CHANGE_TYPE_NONE    }, // 戦闘勝利→図鑑
    { SEQ_BATTLE_WIN, SEQ_OTEIRE, CHANGE_TYPE_NONE    }, // 戦闘勝利→お手入れ
    { SEQ_EVOLVE    , SEQ_EVOLVE, CHANGE_TYPE_SILENCE }, // 進化→進化
    { SEQ_EVOLVE    , SEQ_ZUKAN , CHANGE_TYPE_NORMAL  }, // 進化→図鑑
    { SEQ_EVOLVE    , SEQ_OTEIRE, CHANGE_TYPE_NORMAL  }, // 進化→お手入れ
    { SEQ_ZUKAN     , SEQ_OTEIRE, CHANGE_TYPE_NONE    }, // 図鑑→お手入れ
  };

  u32 tableNum = GFL_NELEMS( PATTERN_TABLE );
  for( u32 i = 0; i < tableNum; ++i)
  {
    const BGMPattern& pattern = PATTERN_TABLE[i];
    if( (pattern.now == m_seq) && (pattern.next == next) )
    {
      return pattern.type;
    }
  }

  GFL_ASSERT( 0 ); // パターンが網羅できていない
  return CHANGE_TYPE_NONE;
}

/**
 * @brief BGM変更 通常
 */
void BattleReturnBGM::ChangeBGMReq_Normal( Seq next, u32 battleWinBGM )
{
  // バトルBGMに対応したBGMに変更
  u32 bgm = BattleReturnBGM::GetWinBGM( battleWinBGM );
  this->ChangeBGMReq( next, bgm );
}

/**
 * @brief BGM変更 無音
 */
void BattleReturnBGM::ChangeBGMReq_Silence( Seq next )
{
  this->ChangeBGMReq( next, STRM_BGM_SILENCE );
}

/**
 * @brief シーケンス更新とBGM変更を行う
 */
void BattleReturnBGM::ChangeBGMReq( Seq next, u32 bgm )
{
  if( bgm != SMID_NULL )
  {
    Sound::ChangeBGMReq( bgm );
    m_seq = next;
  }
}

GFL_NAMESPACE_END( Field )
