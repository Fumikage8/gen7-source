//=============================================================================================
/**
 * @file  btl_server_cmd.cpp
 * @brief ポケモンXY バトルシステム サーバコマンド生成，解釈処理
 * @author  taya
 *
 * @date  2011.01.18  作成
 */
//=============================================================================================

#include "./btl_common.h"
#include "./btl_server_cmd.h"


GFL_NAMESPACE_BEGIN(btl)
namespace scmd {

//--------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------
enum {
  QUE_RESERVE_ARG_SIZE = 3,   // Que領域予約時の付属情報サイズ   コマンドID(2) + 領域予約サイズ(1)
};

// Debug Print
#define __BTL_SC_DEBUG_PRINT_ENABLE   (1)

//--------------------------------------------------------------
/**
 *    サーバコマンド引数型生成マクロ
 */
//--------------------------------------------------------------
// 引数の数, 引数の数ごとのインデックス:両方4bit
#define SC_ARGFMT(cnt, idx)   ((((idx)&0x0f)<<4)|(cnt&0x0f))

//--------------------------------------------------------------
/**
 *    サーバコマンド引数型定義（引数の個数ごとに16パターンまで増やせる）
 */
//--------------------------------------------------------------
typedef enum {
  // 引数１個の型
  SC_ARGFMT_1byte = SC_ARGFMT(1,0),
  SC_ARGFMT_2byte = SC_ARGFMT(1,1),
  SC_ARGFMT_4byte = SC_ARGFMT(1,2),

  // 引数２個の型
  SC_ARGFMT_11byte = SC_ARGFMT(2,0),
  SC_ARGFMT_12byte = SC_ARGFMT(2,1),
  SC_ARGFMT_14byte = SC_ARGFMT(2,2),
  SC_ARGFMT_44bit  = SC_ARGFMT(2,3),
  SC_ARGFMT_53bit  = SC_ARGFMT(2,4),


  // 引数３個の型
  SC_ARGFMT_53bit_1byte = SC_ARGFMT(3,0),
  SC_ARGFMT_53bit_2byte = SC_ARGFMT(3,1),
  SC_ARGFMT_555bit      = SC_ARGFMT(3,2),
  SC_ARGFMT_5_5_14bit   = SC_ARGFMT(3,3),
  SC_ARGFMT_111byte     = SC_ARGFMT(3,4),
  SC_ARGFMT_112byte     = SC_ARGFMT(3,5),
  SC_ARGFMT_114byte     = SC_ARGFMT(3,6),
  SC_ARGFMT_144byte     = SC_ARGFMT(3,7),

  // 引数４個の型
  SC_ARGFMT_53bit_12byte    = SC_ARGFMT(4,0),
  SC_ARGFMT_5353bit         = SC_ARGFMT(4,1),
  SC_ARGFMT_5_5_14bit_1byte = SC_ARGFMT(4,2),
  SC_ARGFMT_5_5_14bit_2byte = SC_ARGFMT(4,3),
  SC_ARGFMT_5_5_6_2byte     = SC_ARGFMT(4,4),
  SC_ARGFMT_1111byte        = SC_ARGFMT(4,5),
  SC_ARGFMT_1122byte        = SC_ARGFMT(4,6),
  SC_ARGFMT_1144byte        = SC_ARGFMT(4,7),

  // 引数５個の型
  SC_ARGFMT_555bit_22byte    = SC_ARGFMT(5,0),
  SC_ARGFMT_5_3_7_1bit_2byte = SC_ARGFMT(5,1),
  SC_ARGFMT_3311bit_2byte    = SC_ARGFMT(5,2),


  // 引数６個の型
  SC_ARGFMT_555555bit         = SC_ARGFMT(6,0),
  SC_ARGFMT_32111bit_2byte    = SC_ARGFMT(6,1),
  SC_ARGFMT_5555bit_22byte    = SC_ARGFMT(6,2),
  SC_ARGFMT_556bit_222byte    = SC_ARGFMT(6,3),
  SC_ARGFMT_111122byte        = SC_ARGFMT(6,4),
  SC_ARGFMT_5_5_14_7_1_8bit   = SC_ARGFMT(6,5),

  // 引数７個の型
  SC_ARGFMT_1x7byte           = SC_ARGFMT(7,0),
  SC_ARGFMT_55511bit_22byte   = SC_ARGFMT(7,1),
  SC_ARGFMT_55555bit_22byte   = SC_ARGFMT(7,2),
  SC_ARGFMT_1111122byte       = SC_ARGFMT(7,3),

  // 引数８個の型
  SC_ARGFMT_1x8byte = SC_ARGFMT(8,0),

  // 引数９個の型
  SC_ARGFMT_1x9byte = SC_ARGFMT(9,0),


  // メッセージ型（可変引数）
  SC_ARGFMT_MSG    = SC_ARGFMT(0,1),
  SC_ARGFMT_MSG_SE = SC_ARGFMT(0,2),
  SC_ARGFMT_POINT  = SC_ARGFMT(0,3),

  // NULL型
  SC_ARGFMT_NULL   = SC_ARGFMT(0,0),

}ScArgFormat;


// 引数型から引数の数を取得
inline u8 SC_ARGFMT_GetArgCount( ScArgFormat fmt )
{
  u8 val = static_cast<u8>(fmt);
  return val & 0x0f;
}





//--------------------------------------------------------------
/**
 *    サーバコマンドから引数の型を取得するためのテーブル
 *    （サーバコマンドと順序を一致させる必要がある）
 */
//--------------------------------------------------------------
static const u8 ServerCmdToFmtTbl[ SC_MAX ] = {
  SC_ARGFMT_NULL,
  SC_ARGFMT_12byte,           // SC_OP_HP_MINUS
  SC_ARGFMT_12byte,           // SC_OP_HP_MINUS_SYNC_WAZAEFF
  SC_ARGFMT_12byte,           // SC_OP_HP_PLUS
  SC_ARGFMT_1byte,            // SC_OP_HP_ZERO
  SC_ARGFMT_53bit_1byte,      // SC_OP_PP_MINUS
  SC_ARGFMT_53bit_1byte,      // SC_OP_PP_MINUS_ORG
  SC_ARGFMT_53bit,            // SC_OP_WAZA_USED
  SC_ARGFMT_53bit_1byte,      // SC_OP_PP_PLUS
  SC_ARGFMT_53bit_1byte,      // SC_OP_PP_PLUS_ORG
  SC_ARGFMT_53bit_1byte,      // SC_OP_RANK_UP
  SC_ARGFMT_53bit_1byte,      // SC_OP_RANK_DOWN
  SC_ARGFMT_1x9byte,          // SC_OP_RANK_SET8
  SC_ARGFMT_1byte,            // SC_OP_RANK_RECOVER,
  SC_ARGFMT_1byte,            // SC_OP_RANK_RESET_UP
  SC_ARGFMT_1byte,            // SC_OP_RANK_RESET,
  SC_ARGFMT_14byte,           // SC_OP_ADD_CRITICAL
  SC_ARGFMT_1144byte,         // SC_OP_SICK_SET
  SC_ARGFMT_1byte,            // SC_OP_CURE_POKESICK
  SC_ARGFMT_12byte,           // SC_OP_CURE_WAZASICK
  SC_ARGFMT_53bit_12byte,     // SC_OP_MEMBER_IN
  SC_ARGFMT_5_5_14bit,        // SC_OP_SET_STATUS
  SC_ARGFMT_12byte,           // SC_OP_SET_WEIGHT
  SC_ARGFMT_114byte,          // SC_OP_WAZASICK_TURNCHECK
  SC_ARGFMT_112byte,          // SC_OP_CHANGE_POKETYPE
  SC_ARGFMT_111byte,          // SC_OP_EX_POKETYPE
  SC_ARGFMT_12byte,           // SC_OP_CONSUME_ITEM
  SC_ARGFMT_55511bit_22byte,  // SC_OP_UPDATE_USE_WAZA,
  SC_ARGFMT_11byte,           // SC_OP_SET_CONTFLAG
  SC_ARGFMT_11byte,           // SC_OP_RESET_CONTFLAG
  SC_ARGFMT_11byte,           // SC_OP_SET_TURNFLAG
  SC_ARGFMT_11byte,           // SC_OP_RESET_TURNFLAG
  SC_ARGFMT_11byte,           // SC_OP_SET_PERMFLAG
  SC_ARGFMT_NULL,             // SC_OP_INC_BATTLE_TURN_COUNT
  SC_ARGFMT_1byte,            // SC_OP_INC_POKE_TURN_COUNT
  SC_ARGFMT_12byte,           // SC_OP_CHANGE_TOKUSEI
  SC_ARGFMT_12byte,           // SC_OP_SET_ITEM
  SC_ARGFMT_5_3_7_1bit_2byte, // SC_OP_UPDATE_WAZANUMBER
  SC_ARGFMT_1byte,            // SC_OP_OUTCLEAR
  SC_ARGFMT_144byte,          // SC_OP_ADD_FLDEFF
  SC_ARGFMT_11byte,           // SC_OP_ADD_FLDEFF_DEPEND
  SC_ARGFMT_144byte,          // SC_OP_CHANGE_GROUND
  SC_ARGFMT_1byte,            // SC_OP_DEL_FLDEFF_DEPEND
  SC_ARGFMT_1byte,            // SC_OP_REMOVE_FLDEFF
  SC_ARGFMT_5_5_14bit,        // SC_OP_SET_POKE_COUNTER
  SC_ARGFMT_1byte,            // SC_OP_INC_KILLCOUNT
  SC_ARGFMT_11byte,           // SC_OP_BATONTOUCH
  SC_ARGFMT_12byte,           // SC_OP_MIGAWARI_CREATE
  SC_ARGFMT_1byte,            // SC_OP_MIGAWARI_DELETE
  SC_ARGFMT_1byte,            // SC_OP_SET_ILLUSION
  SC_ARGFMT_1byte,            // SC_OP_CLEAR_CONSUMED_ITEM
  SC_ARGFMT_11byte,           // SC_OP_CURESICK_DEPEND_POKE
  SC_ARGFMT_55555bit_22byte,  // SC_OP_WAZADMG_REC
  SC_ARGFMT_1byte,            // SC_OP_TURN_CHECK
  SC_ARGFMT_1byte,            // SC_OP_TURN_CHECK_FIELD
  SC_ARGFMT_1x7byte,          // SC_OP_SET_DORYOKU
  SC_ARGFMT_114byte,          // SC_OP_START_POSEFF
  SC_ARGFMT_11byte,           // SC_OP_REMOVE_POSEFF
  SC_ARGFMT_114byte,          // SC_OP_POSEFFECT_UPDATE_PARAM
  SC_ARGFMT_112byte,          // SC_OP_PGL_RECORD
  SC_ARGFMT_1144byte,         // SC_OP_SIDEEFFECT_ADD
  SC_ARGFMT_11byte,           // SC_OP_SIDEEFFECT_REMOVE
  SC_ARGFMT_11byte,           // SC_OP_SIDEEFFECT_INC_TURNCOUNT
  SC_ARGFMT_1byte,            // SC_OP_PUBLISH_CLIENT_INFO_APPEARED_POKEMON
  SC_ARGFMT_11byte,           // SC_OP_PUBLISH_CLIENT_INFO_HAVE_POKEMON_ITEM
  SC_ARGFMT_1111byte,         // SC_OP_SET_CURRENT_ROYAL_RANKING
  SC_ARGFMT_1byte,            // SC_OP_SET_ZENRYOKU_WAZA_USED
  SC_ARGFMT_11byte,           // SC_OP_SET_DIRT
  SC_ARGFMT_1byte,            // SC_OP_INC_WEATHER_PASSED_TURN
  SC_ARGFMT_11byte,           // SC_OP_SET_SPACT_PRIORITY
  SC_ARGFMT_5_5_14_7_1_8bit,  // SC_ACT_WAZA_EFFECT
  SC_ARGFMT_53bit,            // SC_ACT_TAMEWAZA_HIDE
  SC_ARGFMT_112byte,          // SC_ACT_WAZA_DMG
  SC_ARGFMT_112byte,          // SC_ACT_WAZA_DMG_PLURAL
  SC_ARGFMT_1byte,            // SC_ACT_WAZA_ICHIGEKI
  SC_ARGFMT_11byte,           // SC_ACT_SICK_ICON
  SC_ARGFMT_1byte,            // SC_ACT_CONF_DMG
  SC_ARGFMT_555bit,           // SC_ACT_RANKUP
  SC_ARGFMT_555bit,           // SC_ACT_RANKDOWN
  SC_ARGFMT_11byte,           // SC_ACT_DEAD
  SC_ARGFMT_1byte,            // SC_ACT_RELIVE
  SC_ARGFMT_11byte,           // SC_ACT_MEMBER_OUT_MSG
  SC_ARGFMT_12byte,           // SC_ACT_MEMBER_OUT
  SC_ARGFMT_5353bit,          // SC_ACT_MEMBER_IN
  SC_ARGFMT_1111byte,         // SC_ACTOP_WEATHER_START,
  SC_ARGFMT_1byte,            // SC_ACTOP_WEATHER_END,
  SC_ARGFMT_1byte,            // SC_ACTOP_MEGAEVO
  SC_ARGFMT_1byte,            // SC_ACT_SIMPLE_HP
  SC_ARGFMT_53bit,            // SC_ACT_USE_ITEM
  SC_ARGFMT_555555bit,        // SC_ACT_KINOMI_PRE_WAZA
  SC_ARGFMT_53bit,            // SC_ACT_KILL
  SC_ARGFMT_555bit,           // SC_ACTOP_MOVE
  SC_ARGFMT_14byte,           // SC_ACT_EXP
  SC_ARGFMT_1byte,            // SC_ACT_NADERU
  SC_ARGFMT_5555bit_22byte,   // SC_ACT_BALL_THROW      // pos拡大のため SC_ARGFMT_32111bit_2byte から変更（11.07.04）
  SC_ARGFMT_53bit,            // SC_ACT_BALL_THROW_CAPTURED
  SC_ARGFMT_12byte,           // SC_ACT_BALL_THROW_FORBIDDEN
  SC_ARGFMT_12byte,           // SC_ACTOP_CHANGE_TOKUSEI
  SC_ARGFMT_1122byte,         // SC_ACTOP_SWAP_TOKUSEI
  SC_ARGFMT_1byte,            // SC_ACT_FAKE_DISABLE
  SC_ARGFMT_2byte,            // SC_ACT_EFFECT_SIMPLE
  SC_ARGFMT_12byte,           // SC_ACT_EFFECT_BYPOS
  SC_ARGFMT_112byte,          // SC_ACT_EFFECT_BYVECTOR
  SC_ARGFMT_2byte,            // SC_ACT_EFFECT_FIELD
  SC_ARGFMT_12byte,           // SC_ACT_PLURALHIT_EX_2ND
  SC_ARGFMT_111byte,          // SC_ACT_CHANGE_FORM
  SC_ARGFMT_1byte,            // SC_ACT_MIGAWARI_CREATE
  SC_ARGFMT_1byte,            // SC_ACT_MIGAWARI_DELETE
  SC_ARGFMT_11byte,           // SC_ACT_HENSIN
  SC_ARGFMT_112byte,          // SC_ACT_MIGAWARI_DAMAGE
  SC_ARGFMT_4byte,            // SC_ACT_WIN_BGM
  SC_ARGFMT_1byte,            // SC_ACT_MSGWIN_HIDE
  SC_ARGFMT_53bit,            // SC_ACT_FRIENDSHIP_EFFECT
  SC_ARGFMT_556bit_222byte,   // SC_ACT_FRIENDSHIP_EFFECT_MSG
  SC_ARGFMT_1byte,            // SC_ACT_DEAD_PLURAL
  SC_ARGFMT_1111122byte,      // SC_ACT_ZENRYOKUWAZA_BEGIN
  SC_ARGFMT_1byte,            // SC_ACT_ZENRYOKUWAZA_END
  SC_ARGFMT_NULL,             // SC_ACT_OTEIRE_BUTTON_DISPLAY
  SC_ARGFMT_1byte,            // SC_ACT_INTRUDE_CALL
  SC_ARGFMT_11byte,           // SC_ACT_INTRUDE_APPEAR
  SC_ARGFMT_1byte,            // SC_ACT_INTRUDE_NOT_APPEARD
  SC_ARGFMT_1byte,            // SC_ACT_TURN_START
  SC_ARGFMT_1byte,            // SC_TOKWIN_IN
  SC_ARGFMT_1byte,            // SC_TOKWIN_OUT
  SC_ARGFMT_112byte,          // SC_MSG_WAZA
  SC_ARGFMT_MSG,              // SC_MSG_STD
  SC_ARGFMT_MSG,              // SC_MSG_SET
  SC_ARGFMT_MSG_SE,           // SC_MSG_STD_SE
  SC_ARGFMT_MSG_SE,           // SC_MSG_SET_SE
};

//------------------------------------------------------------------------
// globals
//------------------------------------------------------------------------
static int ArgBuffer[ BTL_SERVERCMD_ARG_MAX ];  // 可変個引数を全て格納するための一時バッファ



static inline u8 pack1_2args( int arg1, int arg2, int bits1, int bits2 )
{
  GFL_ASSERT(bits1+bits2<=8);
  GFL_ASSERT(bits1!=0);
  GFL_ASSERT(bits2!=0);
  {
    u8 mask1, mask2;

    mask1 = (1 << bits1) - 1;
    mask2 = (1 << bits2) - 1;

    GFL_ASSERT( arg1 <= mask1 );
    GFL_ASSERT( arg2 <= mask2 );

    return ((arg1 & mask1) << bits2) | (arg2 & mask2);
  }
}
static inline void unpack1_2args( u8 pack, int bits1, int bits2, int* args, int idx_start )
{
  GFL_ASSERT(bits1+bits2<=8);
  GFL_ASSERT(bits1!=0);
  GFL_ASSERT(bits2!=0);

  {
    u8 mask1 = (1 << bits1) - 1;
    u8 mask2 = (1 << bits2) - 1;

    args[ idx_start++ ] = (pack >> bits2) & mask1;
    args[ idx_start ] =  pack & mask2;
  }
}

static inline u32 pack_3args( int bytes, int arg1, int arg2, int arg3, int bits1, int bits2, int bits3 )
{
  GFL_ASSERT(bits1+bits2+bits3<=(bytes*8));
  GFL_ASSERT(bits1!=0);
  GFL_ASSERT(bits2!=0);
  GFL_ASSERT(bits3!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;

    GFL_ASSERT(arg1 <= mask1);
    GFL_ASSERT(arg2 <= mask2);
    GFL_ASSERT(arg3 <= mask3);

    return ( ((arg1&mask1)<<(bits2+bits3)) | ((arg2&mask2)<<bits3) | (arg3&mask3) );
  }
}
static inline void unpack_3args( int bytes, u32 pack, int bits1, int bits2, int bits3, int* args, int idx_start )
{
  GFL_ASSERT(bits1+bits2+bits3<=(bytes*8));
  GFL_ASSERT(bits1!=0);
  GFL_ASSERT(bits2!=0);
  GFL_ASSERT(bits3!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;

    args[ idx_start++ ] = (pack >> (bits2+bits3)) & mask1;
    args[ idx_start++ ] = (pack >> bits3) & mask2;
    args[ idx_start ] = pack & mask3;
  }
}

static inline u32 pack_4args( int bytes, int arg1, int arg2, int arg3, int arg4, int bits1, int bits2, int bits3, int bits4 )
{
  GFL_ASSERT(bits1+bits2+bits3+bits4<=(bytes*8));
  GFL_ASSERT(bits1!=0);
  GFL_ASSERT(bits2!=0);
  GFL_ASSERT(bits3!=0);
  GFL_ASSERT(bits4!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;
    u16 mask4 = (1 << bits4) - 1;

    GFL_ASSERT(arg1 <= mask1);
    GFL_ASSERT(arg2 <= mask2);
    GFL_ASSERT(arg3 <= mask3);
    GFL_ASSERT(arg4 <= mask4);

    return ( ((arg1&mask1)<<(bits2+bits3+bits4)) | ((arg2&mask2)<<(bits3+bits4)) | ((arg3&mask3)<<bits4) | (arg4&mask4) );
  }
}
static inline void unpack_4args( int bytes, u32 pack, int bits1, int bits2, int bits3, int bits4, int* args, int idx_start )
{
  GFL_ASSERT(bits1+bits2+bits3+bits4<=(bytes*8));
  GFL_ASSERT(bits1!=0);
  GFL_ASSERT(bits2!=0);
  GFL_ASSERT(bits3!=0);
  GFL_ASSERT(bits4!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;
    u16 mask4 = (1 << bits4) - 1;

    args[ idx_start++ ] = (pack >> (bits2+bits3+bits4)) & mask1;
    args[ idx_start++ ] = (pack >> (bits3+bits4)) & mask2;
    args[ idx_start++ ] = (pack >> (bits4)) & mask3;
    args[ idx_start ] = pack & mask4;
  }
}

static inline u32 pack_5args( int bytes, int arg1, int arg2, int arg3, int arg4, int arg5, int bits1, int bits2, int bits3, int bits4, int bits5 )
{
  GFL_ASSERT(bits1+bits2+bits3+bits4+bits5<=(bytes*8));
  GFL_ASSERT(bits1!=0);
  GFL_ASSERT(bits2!=0);
  GFL_ASSERT(bits3!=0);
  GFL_ASSERT(bits4!=0);
  GFL_ASSERT(bits5!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;
    u16 mask4 = (1 << bits4) - 1;
    u16 mask5 = (1 << bits5) - 1;

    GFL_ASSERT(arg1 <= mask1);
    GFL_ASSERT(arg2 <= mask2);
    GFL_ASSERT(arg3 <= mask3);
    GFL_ASSERT(arg4 <= mask4);
    GFL_ASSERT(arg5 <= mask5);

    return ( ((arg1&mask1)<<(bits2+bits3+bits4+bits5)) | ((arg2&mask2)<<(bits3+bits4+bits5)) |
              ((arg3&mask3)<<(bits4+bits5)) | ((arg4&mask4)<<bits5) | (arg5&mask5) );
  }
}
static inline void unpack_5args( int bytes, u32 pack, int bits1, int bits2, int bits3, int bits4, int bits5, int* args, int idx_start )
{
  GFL_ASSERT(bits1+bits2+bits3+bits4<=(bytes*8));
  GFL_ASSERT(bits1!=0);
  GFL_ASSERT(bits2!=0);
  GFL_ASSERT(bits3!=0);
  GFL_ASSERT(bits4!=0);
  GFL_ASSERT(bits5!=0);
  {
    u16 mask1 = (1 << bits1) - 1;
    u16 mask2 = (1 << bits2) - 1;
    u16 mask3 = (1 << bits3) - 1;
    u16 mask4 = (1 << bits4) - 1;
    u16 mask5 = (1 << bits5) - 1;

    args[ idx_start++ ] = (pack >> (bits2+bits3+bits4+bits5)) & mask1;
    args[ idx_start++ ] = (pack >> (bits3+bits4+bits5)) & mask2;
    args[ idx_start++ ] = (pack >> (bits4+bits5)) & mask3;
    args[ idx_start++ ] = (pack >> (bits5)) & mask4;
    args[ idx_start ] = pack & mask5;
  }
}

//---------------------------------------------------------------------------------------
/**
 * Que Read/Write
 */
//---------------------------------------------------------------------------------------

void scque_put1byte( SCQUE* que, u8 data )
{
  GFL_ASSERT_STOP(que->writePtr < BTL_SERVERCMD_QUE_SIZE);
  que->buffer[ que->writePtr++ ] = data;
}
u8 scque_read1byte( SCQUE* que )
{
  GFL_ASSERT(que->readPtr < que->writePtr);
  return que->buffer[ que->readPtr++ ];
}
void scque_put2byte( SCQUE* que, u16 data )
{
  GFL_ASSERT_STOP(que->writePtr < (BTL_SERVERCMD_QUE_SIZE-1));
  que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
  que->buffer[ que->writePtr++ ] = (data & 0xff);
}
u16 scque_read2byte( SCQUE* que )
{
  GFL_ASSERT_MSG(que->readPtr < (que->writePtr-1), "rp=%d, wp=%d", que->readPtr, que->writePtr);
  {
    u16 data = ( (que->buffer[que->readPtr] << 8) | que->buffer[que->readPtr+1] );
    que->readPtr += 2;
    return data;
  }
}
void scque_put3byte( SCQUE* que, u32 data )
{
  GFL_ASSERT_STOP(que->writePtr < (BTL_SERVERCMD_QUE_SIZE-2));
  que->buffer[ que->writePtr++ ] = (data >> 16)&0xff;
  que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
  que->buffer[ que->writePtr++ ] = (data & 0xff);
}
u32 scque_read3byte( SCQUE* que )
{
  GFL_ASSERT(que->readPtr < (que->writePtr-2));
  {
    u32 data = ( (que->buffer[que->readPtr]<<16) | (que->buffer[que->readPtr+1]<<8) | (que->buffer[que->readPtr+2]) );
    que->readPtr += 3;
    return data;
  }
}
void scque_put4byte( SCQUE* que, u32 data )
{
  GFL_ASSERT_STOP(que->writePtr < (BTL_SERVERCMD_QUE_SIZE-3));
  que->buffer[ que->writePtr++ ] = (data >> 24)&0xff;
  que->buffer[ que->writePtr++ ] = (data >> 16)&0xff;
  que->buffer[ que->writePtr++ ] = (data >> 8)&0xff;
  que->buffer[ que->writePtr++ ] = (data & 0xff);
}
u32 scque_read4byte( SCQUE* que )
{
  GFL_ASSERT(que->readPtr < (que->writePtr-3));
  {
    u32 data = ( (que->buffer[que->readPtr]<<24) | (que->buffer[que->readPtr+1]<<16)
               | (que->buffer[que->readPtr+2]<<8) | (que->buffer[que->readPtr+3]) );
    que->readPtr += 4;
    return data;
  }
}


//--------------------------------------------------------------------------
/**
 * コマンドナンバ、引数型に応じて全引数をQUEに格納
 *
 * @param   que
 * @param   cmd
 * @param   fmt
 * @param   args
 *
 */
//--------------------------------------------------------------------------
static void put_core( SCQUE* que, ServerCmd cmd, ScArgFormat fmt, const int* args )
{
  scque_put2byte( que, cmd );

  switch( fmt ) {
  case SC_ARGFMT_NULL:
    break;
  case SC_ARGFMT_1byte:
    scque_put1byte( que, args[0] );
    break;
  case SC_ARGFMT_2byte:
    scque_put2byte( que, args[0] );
    break;
  case SC_ARGFMT_4byte:
    scque_put4byte( que, (u32)(args[0]) );
    break;
  case SC_ARGFMT_11byte:
    scque_put1byte( que, args[0] );
    scque_put1byte( que, args[1] );
    break;
  case SC_ARGFMT_12byte:
    scque_put1byte( que, args[0] );
    scque_put2byte( que, args[1] );
    break;
  case SC_ARGFMT_14byte:
    scque_put1byte( que, args[0] );
    scque_put4byte( que, args[1] );
    break;
  case SC_ARGFMT_44bit:
    scque_put1byte( que, pack1_2args(args[0], args[1], 4, 4) );
    break;
  case SC_ARGFMT_53bit:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    break;
  case SC_ARGFMT_53bit_1byte:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    scque_put1byte( que, args[2] );
    break;
  case SC_ARGFMT_53bit_2byte:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    scque_put2byte( que, args[2] );
    break;
  case SC_ARGFMT_555bit:
    {
      u16 pack = pack_3args( 2, args[0],args[1],args[2], 5,5,5 );
      scque_put2byte( que, pack );
    }
    break;
  case SC_ARGFMT_5_5_14bit:
    {
      u32 pack = pack_3args( 3, args[0],args[1],args[2], 5,5,14 );
      scque_put3byte( que, pack );
    }
    break;
  case SC_ARGFMT_111byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put1byte( que, args[2] );
    }
    break;
  case SC_ARGFMT_112byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put2byte( que, args[2] );
    }
    break;
  case SC_ARGFMT_114byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put4byte( que, args[2] );
    }
    break;
  case SC_ARGFMT_144byte:
    {
      scque_put1byte( que, args[0] );
      scque_put4byte( que, args[1] );
      scque_put4byte( que, args[2] );
    }
    break;
  case SC_ARGFMT_5353bit:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    scque_put1byte( que, pack1_2args(args[2], args[3], 5, 3) );
    break;
  case SC_ARGFMT_53bit_12byte:
    scque_put1byte( que, pack1_2args(args[0], args[1], 5, 3) );
    scque_put1byte( que, args[2] );
    scque_put2byte( que, args[3] );
    break;
  case SC_ARGFMT_5_5_14bit_1byte:
    {
      u32 pack = pack_3args( 3, args[0],args[1],args[2], 5,5,14 );
      scque_put3byte( que, pack );
      scque_put1byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_5_5_14bit_2byte:
    {
      u32 pack = pack_3args( 3, args[0],args[1],args[2], 5,5,14 );
      scque_put3byte( que, pack );
      scque_put2byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_5_5_6_2byte:
    {
      u16 pack = pack_3args( 2, args[0],args[1],args[2], 5,5,6 );
      scque_put2byte( que, pack );
      scque_put2byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_1111byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put1byte( que, args[2] );
      scque_put1byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_1122byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put2byte( que, args[2] );
      scque_put2byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_1144byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put4byte( que, args[2] );
      scque_put4byte( que, args[3] );
    }
    break;
  case SC_ARGFMT_555bit_22byte:
    {
      u16 pack = pack_3args( 2, args[0],args[1],args[2], 5,5,5 );
      scque_put2byte( que, pack );
      scque_put2byte( que, args[3] );
      scque_put2byte( que, args[4] );
    }
    break;
  case SC_ARGFMT_5_3_7_1bit_2byte:
    {
      u16 pack1 = pack1_2args( args[0], args[1], 5, 3 );
      u16 pack2 = pack1_2args( args[2], args[3], 7, 1 );
      scque_put2byte( que, pack1 );
      scque_put2byte( que, pack2 );
      scque_put2byte( que, args[4] );
    }
    break;
  case SC_ARGFMT_3311bit_2byte:
    {
      u8 pack = pack_4args( 1, args[0], args[1], args[2], args[3], 3, 3, 1, 1 );
      scque_put1byte( que, pack );
      scque_put2byte( que, args[4] );
    }
    break;
  case SC_ARGFMT_5_5_14_7_1_8bit:
    {
      u32 pack_3  = pack_3args( 3, args[0],args[1],args[2], 5,5,14 );
      u8  pack_1  = pack1_2args( args[3], args[4], 7, 1 );
      u8  pack_1b = args[5];
      scque_put3byte( que, pack_3 );
      scque_put1byte( que, pack_1 );
      scque_put1byte( que, pack_1b );
    }
    break;
  case SC_ARGFMT_555555bit:
    {
      u16 pack1 = pack_3args( 2, args[0],args[1],args[2], 5,5,5 );
      u16 pack2 = pack_3args( 2, args[3],args[4],args[5], 5,5,5 );
      scque_put2byte( que, pack1 );
      scque_put2byte( que, pack2 );
    }
    break;
  case SC_ARGFMT_32111bit_2byte:
    {
      u8 pack = pack_5args( 1, args[0], args[1], args[2], args[3], args[4], 3, 2, 1, 1, 1 );
      scque_put1byte( que, pack );
      scque_put2byte( que, args[5] );
    }
    break;
  case SC_ARGFMT_5555bit_22byte:
    {
      u32 pack = pack_4args( 3, args[0],args[1],args[2],args[3], 5,5,5,5 );
      scque_put3byte( que, pack );
      scque_put2byte( que, args[4] );
      scque_put2byte( que, args[5] );
    }
    break;
  case SC_ARGFMT_55511bit_22byte:
    {
      u32 pack = pack_5args( 3, args[0],args[1],args[2],args[3],args[4], 5,5,5,1,1 );
      scque_put3byte( que, pack );
      scque_put2byte( que, args[5] );
      scque_put2byte( que, args[6] );
    }
    break;
  case SC_ARGFMT_55555bit_22byte:
    {
      u32 pack = pack_5args( 4, args[0],args[1],args[2],args[3],args[4], 5,5,5,5,5 );
      scque_put4byte( que, pack );
      scque_put2byte( que, args[5] );
      scque_put2byte( que, args[6] );
    }
    break;
  case SC_ARGFMT_556bit_222byte:
    {
      u16 pack = pack_3args( 2, args[0],args[1],args[2], 5,5,6 );
      scque_put2byte( que, pack );
      scque_put2byte( que, args[3] );
      scque_put2byte( que, args[4] );
      scque_put2byte( que, args[5] );
    }
    break;
  case SC_ARGFMT_111122byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put1byte( que, args[2] );
      scque_put1byte( que, args[3] );
      scque_put2byte( que, args[4] );
      scque_put2byte( que, args[5] );
    }
    break;

  case SC_ARGFMT_1111122byte:
    {
      scque_put1byte( que, args[0] );
      scque_put1byte( que, args[1] );
      scque_put1byte( que, args[2] );
      scque_put1byte( que, args[3] );
      scque_put1byte( que, args[4] );
      scque_put2byte( que, args[5] );
      scque_put2byte( que, args[6] );
    }
    break;

  case SC_ARGFMT_1x7byte:
    {
      for(int i=0; i<7; ++i){
        scque_put1byte( que, args[i] );
      }
    }
    break;

  case SC_ARGFMT_1x8byte:
    {
      for(int i=0; i<8; ++i){
        scque_put1byte( que, args[i] );
      }
    }
    break;

  case SC_ARGFMT_1x9byte:
    {
      for(int i=0; i<9; ++i){
        scque_put1byte( que, args[i] );
      }
    }
    break;

  case SC_ARGFMT_POINT:
    break;
  default:
    GFL_ASSERT(0);
    break;
  }/* switch(fmt) */
}

void printDebugEx( SCQUE* que, int p )
{
  #if __BTL_SC_DEBUG_PRINT_ENABLE
    BTL_PRINT("  /@/ ");
    p -= 8;
    if( p < 0 ){ p = 0; }
    for(int i=0; i<16; ++i)
    {
      BTL_PRINT("%d,", que->buffer[ p+i ]);
    }
  #endif
}


void SCQUE_PUT_Common( SCQUE* que, ServerCmd cmd, ... )
{
  if( cmd < GFL_NELEMS(ServerCmdToFmtTbl) )
  {
    va_list   list;
    u32 arg_cnt;
    ScArgFormat fmt;

    fmt = static_cast<ScArgFormat>( ServerCmdToFmtTbl[ cmd ] );
    arg_cnt = SC_ARGFMT_GetArgCount( fmt );

    GFL_ASSERT(arg_cnt <= GFL_NELEMS(ArgBuffer));

    va_start( list, cmd );
    for(u32 i=0; i<arg_cnt; ++i)
    {
      ArgBuffer[i] = va_arg( list, int );
    }
    va_end( list );

    put_core( que, cmd, fmt, ArgBuffer );

    #if __BTL_SC_DEBUG_PRINT_ENABLE
    BTL_PRINT("[SCQUE] PutCommon Cmd=%s(%d), args=%d, args= ", SCMD_GetLabelString( cmd ), cmd, arg_cnt);
    for(u32 i=0; i<arg_cnt; ++i){
      BTL_PRINT("%d,", ArgBuffer[i]);
    }
    printDebugEx( que, que->writePtr );
    BTL_PRINT("\n");
    #endif
  }
  else{
    GFL_ASSERT_MSG(0, "illegal cmd:%d", cmd);
  }
}



void SCQUE_PUT_MsgImpl( SCQUE* que, u8 scType, ... )
{
  enum {
    PRINT_FLAG = true,
  };

  {
    va_list   list;
    ScMsgArg   arg;
    u16 strID;

    va_start( list, scType );
    strID = va_arg( list, int );

    scque_put2byte( que, scType );
    scque_put2byte( que, strID );

    #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT( "[SCQUE] PutMsg Cmd=%d, strID=%d ", scType, strID );
    #endif

    if( scType == SC_MSG_STD_SE ){
      u32 seID = va_arg( list, int );
      scque_put4byte( que, seID );
      #if __BTL_SC_DEBUG_PRINT_ENABLE
        BTL_PRINT( " [SEID %d] ", seID);
      #endif

    }
    #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT( "args= ");
    #endif


    do {
      arg = va_arg( list, int );
      if( arg != MSGARG_TERMINATOR ){
        #if __BTL_SC_DEBUG_PRINT_ENABLE
          BTL_PRINT( "%d,", arg);
        #endif
      }
      scque_put4byte( que, arg );
    }while( arg != MSGARG_TERMINATOR );
    printDebugEx( que, que->writePtr );
    #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT( "\n");
    #endif

    va_end( list );
  }
}


//------------------------------------------------------
/**
 *
 *
 * @param   que
 * @param   reserveCmd
 * @param   replaceCmd
 */
//------------------------------------------------------
u16 _que_replace_ex_cmd( SCQUE* que, ServerCmd reserveCmd, ServerCmd replaceCmd, u8 exVarCount )
{
  u8  reserve_size;

    // 書き込みポインタを保持しておく
  u16 pos = que->writePtr;

  // 特殊コマンド以外は、実際に書き込んでみてその差分を書き込みサイズとして取得
  if( exVarCount == 0 )
  {
    ScArgFormat fmt = static_cast<ScArgFormat>( ServerCmdToFmtTbl[ reserveCmd ] );
    for(u32 i=0; i<GFL_NELEMS(ArgBuffer); ++i){
      ArgBuffer[i] = 0;
    }
    put_core( que, reserveCmd, fmt, ArgBuffer );
    reserve_size = que->writePtr - pos;
  }
  // 特殊コマンドは、コマンド(2byte) + 引数個数指定（1byte) + 引数(各1byte)の個数が書き込みサイズになる
  else
  {
    reserve_size = 2 + 1 + exVarCount;
  }

  GFL_ASSERT(reserve_size >= QUE_RESERVE_ARG_SIZE); // @check
  que->writePtr = pos;
  scque_put2byte( que, replaceCmd );
  scque_put1byte( que, reserve_size-QUE_RESERVE_ARG_SIZE );

  #if __BTL_SC_DEBUG_PRINT_ENABLE
    BTL_PRINT("[SCQUE] replace pos(%d) に Cmd(%d)の替わりにExCmd(%d) 埋め込み  -> %d byte スキップします\n",
                    pos, reserveCmd, replaceCmd, (reserve_size-QUE_RESERVE_ARG_SIZE) );
  #endif

  que->writePtr = pos + reserve_size;

  return pos;
}

//=============================================================================================
/**
 * キュー書き込みの現在位置を予約する
 *
 * @param   que
 * @param   cmd   書き込みコマンド
 *
　 * @retval  u16   現在位置
 */
//=============================================================================================
u16 SCQUE_RESERVE_Pos( SCQUE* que, ServerCmd cmd )
{
  if( cmd < GFL_NELEMS(ServerCmdToFmtTbl) )
  {
    u16 pos = _que_replace_ex_cmd( que, cmd, SCEX_RESERVE, 0 );
    #if __BTL_SC_DEBUG_PRINT_ENABLE
    BTL_PRINT("[SCQUE reserve_pos] pos=%d\n", pos);
    #endif
    return pos;
  }
  else{
    GFL_ASSERT_MSG(0, "illegal cmd:%d", cmd);
    return SCQUE::INVALID_PTR_VALUE;
  }
}

void SCQUE_PUT_ReservedPos( SCQUE* que, u16 pos, ServerCmd cmd, ... )
{
  if( cmd >= GFL_NELEMS(ServerCmdToFmtTbl) ){
    GFL_ASSERT(0);
    return;
  }
  if( pos == SCQUE::INVALID_PTR_VALUE ){
    GFL_ASSERT(0);
    return;
  }

  {
    va_list   list;
    ScArgFormat  fmt;
    u32 i, arg_cnt;

    fmt = static_cast<ScArgFormat>( ServerCmdToFmtTbl[ cmd ] );
    arg_cnt = SC_ARGFMT_GetArgCount( fmt );
    GFL_ASSERT(arg_cnt <= GFL_NELEMS(ArgBuffer));

    va_start( list, cmd );
    for(i=0; i<arg_cnt; ++i){
      ArgBuffer[i] = va_arg( list, int );
    }
    va_end( list );

    #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT( "[SCQUE put_reserved] pos=%d, cmd=%d, numArgs=%d / ", pos, cmd, arg_cnt );
      for( i=0; i<arg_cnt; ++i ){
        BTL_PRINT("%d, ", ArgBuffer[i]);
      }
      BTL_PRINT("\n");
    #endif


    u16 default_read_pos = que->readPtr;
    que->readPtr = pos;
    #if PM_DEBUG
      u16 reserved_cmd = scque_read2byte( que );
      u16 reserved_size = scque_read1byte( que );
      #if __BTL_SC_DEBUG_PRINT_ENABLE
        GFL_PRINT("[SCQUE put_reserved] cmd=%d, size=%d\n", reserved_cmd, reserved_size);
      #endif
    #else
      scque_read2byte( que );
      scque_read1byte( que );
    #endif
    que->readPtr = default_read_pos;

    if( cmd != SCEX_RESERVE )
    {
      u16 default_write_pos = que->writePtr;
      que->writePtr = pos;
      put_core( que, cmd, fmt, ArgBuffer );

      #if PM_DEBUG
      {
        u8 wrote_size = (que->writePtr - pos - QUE_RESERVE_ARG_SIZE); // 予約コマンド(2)+size(1)で3byteを引く
        GFL_ASSERT_MSG((wrote_size == reserved_size), "wrote=%d, reserved=%d", wrote_size, reserved_size);
      }
      #endif

      que->writePtr = default_write_pos;
    }
  }
}


//--------------------------------------------------------------------------
/**
 * 引数型に応じて全引数を配列に読み出し
 *
 * @param   que
 * @param   fmt
 * @param   args
 *
 */
//--------------------------------------------------------------------------
void read_core( SCQUE* que, ScArgFormat fmt, int* args )
{
  switch( fmt ) 
  {
  case SC_ARGFMT_NULL:
    break;
  case SC_ARGFMT_1byte:
    args[0] = scque_read1byte( que );
    break;
  case SC_ARGFMT_2byte:
    args[0] = scque_read2byte( que );
    break;
  case SC_ARGFMT_4byte:
    args[0] = (int)(scque_read4byte( que ));
    break;
  case SC_ARGFMT_11byte:
    args[0] = scque_read1byte( que );
    args[1] = scque_read1byte( que );
    break;
  case SC_ARGFMT_12byte:
    args[0] = scque_read1byte( que );
    args[1] = scque_read2byte( que );
    break;
  case SC_ARGFMT_14byte:
    args[0] = scque_read1byte( que );
    args[1] = scque_read4byte( que );
    break;
  case SC_ARGFMT_44bit:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 4, 4, args, 0 );
    }
    break;
  case SC_ARGFMT_53bit:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
    }
    break;
  case SC_ARGFMT_53bit_1byte:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
      args[2] = scque_read1byte( que );
    }
    break;
  case SC_ARGFMT_53bit_2byte:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
      args[2] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_555bit:
    {
      u16 pack = scque_read2byte( que );
      unpack_3args( 2, pack, 5,5,5, args, 0 );
    }
    break;
  case SC_ARGFMT_5_5_14bit:
    {
      u32 pack = scque_read3byte( que );
      unpack_3args( 3, pack, 5,5,14, args, 0 );
    }
    break;
  case SC_ARGFMT_111byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read1byte( que );
    }
    break;
  case SC_ARGFMT_112byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_114byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read4byte( que );
    }
    break;
  case SC_ARGFMT_144byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read4byte( que );
      args[2] = scque_read4byte( que );
    }
    break;
  case SC_ARGFMT_5353bit:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
      pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 2 );
    }
    break;
  case SC_ARGFMT_53bit_12byte:
    {
      u8 pack = scque_read1byte( que );
      unpack1_2args( pack, 5, 3, args, 0 );
      args[2] = scque_read1byte( que );
      args[3] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_5_5_14bit_1byte:
    {
      u32 pack = scque_read3byte( que );
      unpack_3args( 3, pack, 5,5,14, args, 0 );
      args[3] = scque_read1byte( que );
    }
    break;
  case SC_ARGFMT_5_5_14bit_2byte:
    {
      u32 pack = scque_read3byte( que );
      unpack_3args( 3, pack, 5,5,14, args, 0 );
      args[3] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_5_5_6_2byte:
    {
      u16 pack = scque_read2byte( que );
      unpack_3args( 2, pack, 5,5,6, args, 0 );
      args[3] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_1111byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read1byte( que );
      args[3] = scque_read1byte( que );
    }
    break;
  case SC_ARGFMT_1122byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read2byte( que );
      args[3] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_1144byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read4byte( que );
      args[3] = scque_read4byte( que );
    }
    break;
  case SC_ARGFMT_555bit_22byte:
    {
      u16 pack = scque_read2byte( que );
      unpack_3args( 2, pack, 5, 5, 5, args, 0 );
      args[3] = scque_read2byte( que );
      args[4] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_5_3_7_1bit_2byte:
    {
      u16 pack1 = scque_read2byte( que );
      u16 pack2 = scque_read2byte( que );
      unpack1_2args( pack1, 5, 3, args, 0 );
      unpack1_2args( pack2, 7, 1, args, 2 );
      args[4] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_3311bit_2byte:
    {
      u8 pack = scque_read1byte( que );
      unpack_4args( 1, pack, 3, 3, 1, 1, args, 0 );
      args[4] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_5_5_14_7_1_8bit:
    {
      u32 pack_3 = scque_read3byte( que );
      unpack_3args( 3, pack_3, 5,5,14, args, 0 );

      u8 pack_1 = scque_read1byte( que );
      unpack1_2args( pack_1, 7, 1, args, 3 );

      args[5] = scque_read1byte( que );
    }
    break;
  case SC_ARGFMT_555555bit:
    {
      u16 pack1 = scque_read2byte( que );
      u16 pack2 = scque_read2byte( que );
      unpack_3args( 2, pack1, 5, 5, 5, args, 0 );
      unpack_3args( 2, pack2, 5, 5, 5, args, 3 );
    }
    break;

  case SC_ARGFMT_32111bit_2byte:
    {
      u8 pack = scque_read1byte( que );
      unpack_5args( 1, pack, 3, 2, 1, 1, 1, args, 0 );
      args[5] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_5555bit_22byte:
    {
      u32 pack = scque_read3byte( que );

      unpack_4args( 3, pack, 5,5,5,5, args, 0 );
      args[4] = scque_read2byte( que );
      args[5] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_55511bit_22byte:
    {
      u32 pack = scque_read3byte( que );

      unpack_5args( 3, pack, 5,5,5,1,1, args, 0 );
      args[5] = scque_read2byte( que );
      args[6] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_55555bit_22byte:
    {
      u32 pack = scque_read4byte( que );

      unpack_5args( 4, pack, 5,5,5,5,5, args, 0 );
      args[5] = scque_read2byte( que );
      args[6] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_556bit_222byte:
    {
      u16 pack = scque_read2byte( que );

      unpack_3args( 2, pack, 5,5,6, args, 0 );
      args[3] = scque_read2byte( que );
      args[4] = scque_read2byte( que );
      args[5] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_111122byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read1byte( que );
      args[3] = scque_read1byte( que );
      args[4] = scque_read2byte( que );
      args[5] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_1111122byte:
    {
      args[0] = scque_read1byte( que );
      args[1] = scque_read1byte( que );
      args[2] = scque_read1byte( que );
      args[3] = scque_read1byte( que );
      args[4] = scque_read1byte( que );
      args[5] = scque_read2byte( que );
      args[6] = scque_read2byte( que );
    }
    break;
  case SC_ARGFMT_1x7byte:
    for(int i=0; i<7; ++i){
      args[i] = scque_read1byte( que );
    }
    break;

  case SC_ARGFMT_1x8byte:
    for(int i=0; i<8; ++i){
      args[i] = scque_read1byte( que );
    }
    break;

  case SC_ARGFMT_1x9byte:
    for(int i=0; i<9; ++i){
      args[i] = scque_read1byte( que );
    }
    break;

  case SC_ARGFMT_POINT:
    break;
  default:
    GFL_ASSERT_MSG(0, "fmt=%02x", fmt);
    break;
  }/* switch(fmt) */
}
void read_core_msg( SCQUE* que, ServerCmd scType, int* args )
{
  enum {
    PRINT_FLAG = 1,
  };

  int idx_begin = 1;

  args[0] = scque_read2byte( que );

  BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_ReadMsgParam, scType, args[0] );

  if( scType == SC_MSG_STD_SE ){
    args[1] = scque_read4byte( que );
    BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_PutMsg_SE, args[1] );
    ++idx_begin;
  }
  BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_SC_ArgsEqual );

  {
    int i = idx_begin;
    for(i=idx_begin; i<BTL_SERVERCMD_ARG_MAX; ++i)
    {
      args[i] = scque_read4byte( que );
      if( args[i] == MSGARG_TERMINATOR ){
        break;
      }
      BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_csv, args[i] );
      BTL_PRINT("%d,", args[i]);
    }
    BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_LF );
    printDebugEx( que, que->readPtr );
    BTL_PRINT("\n", args[1]);

    if( i == BTL_SERVERCMD_ARG_MAX ){
      GFL_ASSERT(0); // 引数使いすぎ
    }
  }
}
/**
 * 予約領域・スキップ領域を飛ばしてコマンド部読み込み
 */
ServerCmd  scque_readcmd_support_skip( SCQUE* que,  QUE_READING_STATE* cmdReadState )
{
  u32        readPtr = que->readPtr;
  ServerCmd  cmd = static_cast<ServerCmd>( scque_read2byte(que) );

  // 予約領域に何も書き込まれなかった場合は単純にスキップする (SCEX_RESERVE)
  // ワザエフェクトと同期して実行済みのコマンド（SCEX_SKIP）も同様に処理する
  while( (cmd==SCEX_RESERVE) || (cmd==SCEX_SKIP) )
  {
    u8 reserve_size = scque_read1byte( que );
    que->readPtr += reserve_size;

    // 予約領域が終端の場合ここで終了
    if( que->readPtr >= que->writePtr )
    {
      return SC_MAX;
    }
    #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT("[SCQUE] -Cmd Reserve or Skip %d-\n", cmd);
    #endif

    readPtr = que->readPtr;
    cmd = static_cast<ServerCmd>( scque_read2byte(que) );
  }
  if( cmdReadState ){
    *cmdReadState = readPtr;
  }
  return cmd;
}




//=============================================================================================
/**
 * SCQUE から コマンド１件読み出し
 *
 * @param[in]   que       読み出し元 SCQUE
 * @param[out]  args      読み出されたコマンドに付随する引数群を保存する配列
 * @param[in]   argElems  args の配列要素数
 *
 * @retval  ServerCmd   読み出されたコマンド
 */
//=============================================================================================
ServerCmd SCQUE_Read( SCQUE* que, int* args, u32 argElems )
{
  enum {
    PRINT_FLAG = true,
  };

  #if __BTL_SC_DEBUG_PRINT_ENABLE
    BTL_PRINT("[SCQUE start_reading]  rp/wp = %d/%d\n", que->readPtr, que->writePtr);
  #endif


  if( que->readPtr >= que->writePtr ){
    return SC_MAX;
  }

  ServerCmd cmd = scque_readcmd_support_skip( que, NULL );

  if( cmd == SC_MAX ){
    return SC_MAX;
  }

  if( cmd == SCEX_VALIABLE_ARGS )
  {
    u32 argCnt = scque_read1byte( que );
    if( argCnt < (argElems-1) )
    {
      args[0] = argCnt;
      for(u32 i=0; i<argCnt; ++i){
        args[1+i] = scque_read1byte( que );
      }
      #if __BTL_SC_DEBUG_PRINT_ENABLE
        BTL_PRINT( "[SCQUE readCmd Var], cmd=%d, Atgcnt=%d, rp/wp=%d/%d\n  args =",
          cmd, argCnt, que->readPtr, que->writePtr, que->buffer[que->readPtr]);
        for(u32 i=0; i<argCnt; ++i){
          BTL_PRINT("%d,", args[1+i]);
        }
        BTL_PRINT("\n");
      #endif
    }
    else{
      GFL_ASSERT_MSG(0, "argCnt=%d\n", argCnt);
    }
    return cmd;
  }

  if( cmd == SCEX_WAZAEFF_SYNC_CHAPTER )
  {
    // ダミー引数をスキップしてそのまま返す
    scque_read2byte( que );
    #if __BTL_SC_DEBUG_PRINT_ENABLE
    BTL_PRINT( "[SCQUE readCmd Chapter], cmd=%d, rp/wp=%d/%d\n", cmd, que->readPtr, que->writePtr );
    #endif
    return cmd;
  }


  if( cmd < GFL_NELEMS(ServerCmdToFmtTbl) )
  {
    ScArgFormat fmt = static_cast<ScArgFormat>( ServerCmdToFmtTbl[ cmd ] );

    BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_ReadCmd, cmd, fmt, que->readPtr, SCEX_RESERVE);
    #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT( "[SCQUE readCmd Nml], cmd=%d, fmt=%02x, rp/wp=%d/%d  ",
          cmd, fmt, que->readPtr, que->writePtr );
    #endif

    if( (fmt != SC_ARGFMT_MSG) && (fmt != SC_ARGFMT_MSG_SE) )
    {
      read_core( que, fmt, args );
      #if __BTL_SC_DEBUG_PRINT_ENABLE
      {
        u8 arg_cnt = SC_ARGFMT_GetArgCount( fmt );
        u8 i;
        BTL_N_PrintfEx( PRINT_FLAG, DBGSTR_SC_ArgsEqual );
        BTL_PRINT(" args= ");

        for(i=0; i<arg_cnt; ++i){
          BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_csv, args[i]);
          BTL_PRINT("%d,", args[i]);
        }
        BTL_N_PrintfSimpleEx( PRINT_FLAG, DBGSTR_LF );
      }
      printDebugEx( que, que->readPtr );
      BTL_PRINT("\n");
      #endif
    }
    else
    {
      read_core_msg( que, cmd, args );
    }

    return cmd;
  }
  else{
    GFL_ASSERT_MSG( cmd < GFL_NELEMS(ServerCmdToFmtTbl), "cmd=%d\n", cmd );
    return SC_MAX;
  }
}

//=============================================================================================
/**
 * ワザエフェクト同期コマンド読み出しの区切り位置をセットする（ここまででひと区切り）
 *
 * @param   que
 * @param   cmd   書き込みコマンド
 *
 * @retval  u16   現在位置
 */
//=============================================================================================
void SCQUE_PutWazaSyncChapter( SCQUE* que )
{
  scque_put2byte( que, SCEX_WAZAEFF_SYNC_CHAPTER );
  scque_put1byte( que, 1 );   // SKIP設定できるように、ダミーの引数を１個だけ追加
  scque_put1byte( que, 0 );

  #if __BTL_SC_DEBUG_PRINT_ENABLE
    BTL_PRINT("[SCQUE]  -- Chapter Cmd:%d--\n", SCEX_WAZAEFF_SYNC_CHAPTER);
  #endif
}
//------------------------------------------------------
/**
 *  読み出し状況を保存
 */
//------------------------------------------------------
QUE_READING_STATE  SCQUE_ReadStatePush( SCQUE* que )
{
  return que->readPtr;
}
//------------------------------------------------------
/**
 *  読み出し状況を元に戻す
 */
//------------------------------------------------------
void SCQUE_ReadStatePop( SCQUE* que, QUE_READING_STATE state )
{
  que->readPtr = state;
}
void SCQUE_ReplaceSkipCmd( SCQUE* que, QUE_READING_STATE state )
{
  u32 wp_backup = que->writePtr;
  u32 rp_backup = que->readPtr;

  {
    que->readPtr  = state;
      #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT(" [SCQUE] Prev Replace ... rp=>%d を設定\n", que->readPtr);
      #endif

    ServerCmd cmd = scque_readcmd_support_skip( que, &state );

    u8 varArgCnt = 0;
    if( (cmd == SCEX_VALIABLE_ARGS)
    ||  (cmd == SCEX_WAZAEFF_SYNC_CHAPTER)
    ){
      varArgCnt = scque_read1byte(que);
      #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT(" [SCQUE] Replace Cmd[%d] -> Skip 引数=%d個\n", cmd, varArgCnt);
      #endif
    }
    else{
      #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT(" [SCQUE] Replace Cmd[%d] -> Skip \n", cmd);
      #endif
    }

      #if __BTL_SC_DEBUG_PRINT_ENABLE
      BTL_PRINT(" [SCQUE] Replace ... wp=>%d へ上書き \n", state );
      #endif

    que->writePtr = state;
    _que_replace_ex_cmd( que, cmd, SCEX_SKIP, varArgCnt );
  }

  que->readPtr  = rp_backup;
  que->writePtr = wp_backup;
}


//=============================================================================================
/**
 * コマンドに付随する可変引数の追加開始
 *
 * @param   que
 * @param   argCnt
 */
//=============================================================================================
void SCQUE_PUT_ExArgStart( SCQUE* que, u8 argCnt )
{
  scque_put2byte( que, SCEX_VALIABLE_ARGS );
  scque_put1byte( que, argCnt );

  #if __BTL_SC_DEBUG_PRINT_ENABLE
  BTL_PRINT("[SCQUE] 可変個引数 セット開始  - %d個\n", argCnt);
  #endif
}
//=============================================================================================
/**
 * １バイト単位で引数を書き込み（特定コマンドに於いて可変引数に対処するための措置）
 *
 * @param   que
 * @param   arg
 *
 */
//=============================================================================================
void SCQUE_PUT_ArgOnly( SCQUE* que, u8 arg )
{
  scque_put1byte( que, arg );
  #if __BTL_SC_DEBUG_PRINT_ENABLE
  BTL_PRINT("[SCQUE] 可変個引数   -> %d\n", arg);
  #endif
}
//=============================================================================================
/**
 * １バイト単位で渡された引数を読み出す
 *
 * @param   que
 *
 */
//=============================================================================================
u8 SCQUE_READ_ArgOnly( SCQUE* que )
{
  return scque_read1byte( que );
}

#ifdef __BTL_SC_DEBUG_PRINT_ENABLE
  #undef __BTL_SC_DEBUG_PRINT_ENABLE
#endif




#if PM_DEBUG


namespace
{
  static const struct {
    ServerCmd    cmd;
    char*        name;
  }cmdNameTbl[] = {
    { SC_NULL, "SC_NULL" },
    { SC_OP_HP_MINUS, "SC_OP_HP_MINUS" },
    { SC_OP_HP_MINUS_SYNC_WAZAEFF, "SC_OP_HP_MINUS_SYNC_WAZAEFF" },
    { SC_OP_HP_PLUS, "SC_OP_HP_PLUS" },
    { SC_OP_HP_ZERO, "SC_OP_HP_ZERO" },
    { SC_OP_PP_MINUS, "SC_OP_PP_MINUS" },
    { SC_OP_PP_MINUS_ORG, "SC_OP_PP_MINUS_ORG" },
    { SC_OP_WAZA_USED, "SC_OP_WAZA_USED" },
    { SC_OP_PP_PLUS, "SC_OP_PP_PLUS" },
    { SC_OP_PP_PLUS_ORG, "SC_OP_PP_PLUS_ORG" },
    { SC_OP_RANK_UP, "SC_OP_RANK_UP" },
    { SC_OP_RANK_DOWN, "SC_OP_RANK_DOWN" },
    { SC_OP_RANK_SET8, "SC_OP_RANK_SET8" },
    { SC_OP_RANK_RECOVER, "SC_OP_RANK_RECOVER" },
    { SC_OP_RANK_UP_RESET, "SC_OP_RANK_UP_RESET" },
    { SC_OP_RANK_RESET, "SC_OP_RANK_RESET" },
    { SC_OP_ADD_CRITICAL, "SC_OP_ADD_CRITICAL" },
    { SC_OP_SICK_SET, "SC_OP_SICK_SET" },
    { SC_OP_CURE_POKESICK, "SC_OP_CURE_POKESICK" },
    { SC_OP_CURE_WAZASICK, "SC_OP_CURE_WAZASICK" },
    { SC_OP_MEMBER_IN, "SC_OP_MEMBER_IN" },
    { SC_OP_SET_STATUS, "SC_OP_SET_STATUS" },
    { SC_OP_SET_WEIGHT, "SC_OP_SET_WEIGHT" },
    { SC_OP_WAZASICK_TURNCHECK, "SC_OP_WAZASICK_TURNCHECK" },
    { SC_OP_CHANGE_POKETYPE, "SC_OP_CHANGE_POKETYPE" },
    { SC_OP_EX_POKETYPE, "SC_OP_EX_POKETYPE" },
    { SC_OP_CONSUME_ITEM, "SC_OP_CONSUME_ITEM" },
    { SC_OP_UPDATE_USE_WAZA, "SC_OP_UPDATE_USE_WAZA" },
    { SC_OP_SET_CONTFLAG, "SC_OP_SET_CONTFLAG" },
    { SC_OP_RESET_CONTFLAG, "SC_OP_RESET_CONTFLAG" },
    { SC_OP_SET_TURNFLAG, "SC_OP_SET_TURNFLAG" },
    { SC_OP_RESET_TURNFLAG, "SC_OP_RESET_TURNFLAG" },
    { SC_OP_SET_PERMFLAG, "SC_OP_SET_PERMFLAG" },
    { SC_OP_INC_BATTLE_TURN_COUNT, "SC_OP_INC_BATTLE_TURN_COUNT" },
    { SC_OP_INC_POKE_TURN_COUNT, "SC_OP_INC_POKE_TURN_COUNT" },
    { SC_OP_CHANGE_TOKUSEI, "SC_OP_CHANGE_TOKUSEI" },
    { SC_OP_SET_ITEM, "SC_OP_SET_ITEM" },
    { SC_OP_UPDATE_WAZANUMBER, "SC_OP_UPDATE_WAZANUMBER" },
    { SC_OP_OUTCLEAR, "SC_OP_OUTCLEAR" },
    { SC_OP_ADD_FLDEFF, "SC_OP_ADD_FLDEFF" },
    { SC_OP_ADD_FLDEFF_DEPEND, "SC_OP_ADD_FLDEFF_DEPEND" },
    { SC_OP_CHANGE_GROUND, "SC_OP_CHANGE_GROUND" },
    { SC_OP_DEL_FLDEFF_DEPEND, "SC_OP_DEL_FLDEFF_DEPEND" },
    { SC_OP_REMOVE_FLDEFF, "SC_OP_REMOVE_FLDEFF" },
    { SC_OP_SET_POKE_COUNTER, "SC_OP_SET_POKE_COUNTER" },
    { SC_OP_INC_KILLCOUNT, "SC_OP_INC_KILLCOUNT" },
    { SC_OP_BATONTOUCH, "SC_OP_BATONTOUCH" },
    { SC_OP_MIGAWARI_CREATE, "SC_OP_MIGAWARI_CREATE" },
    { SC_OP_MIGAWARI_DELETE, "SC_OP_MIGAWARI_DELETE" },
    { SC_OP_SET_ILLUSION, "SC_OP_SET_ILLUSION" },
    { SC_OP_CLEAR_CONSUMED_ITEM, "SC_OP_CLEAR_CONSUMED_ITEM" },
    { SC_OP_CURESICK_DEPEND_POKE, "SC_OP_CURESICK_DEPEND_POKE" },
    { SC_OP_WAZADMG_REC, "SC_OP_WAZADMG_REC" },
    { SC_OP_TURN_CHECK, "SC_OP_TURN_CHECK" },
    { SC_OP_TURN_CHECK_FIELD, "SC_OP_TURN_CHECK_FIELD" },
    { SC_OP_SET_DORYOKU, "SC_OP_SET_DORYOKU" },
    { SC_OP_START_POSEFF, "SC_OP_START_POSEFF" },
    { SC_OP_REMOVE_POSEFF, "SC_OP_REMOVE_POSEFF" },
    { SC_OP_POSEFFECT_UPDATE_PARAM, "SC_OP_POSEFFECT_UPDATE_PARAM" },
    { SC_OP_PGL_RECORD, "SC_OP_PGL_RECORD" },
    { SC_OP_SIDEEFFECT_ADD, "SC_OP_SIDEEFFECT_ADD" },
    { SC_OP_SIDEEFFECT_REMOVE, "SC_OP_SIDEEFFECT_REMOVE" },
    { SC_OP_SIDEEFFECT_INC_TURNCOUNT, "SC_OP_SIDEEFFECT_INC_TURNCOUNT" },
    { SC_OP_PUBLISH_CLIENT_INFO_APPEARED_POKEMON, "SC_OP_PUBLISH_CLIENT_INFO_APPEARED_POKEMON" },
    { SC_OP_PUBLISH_CLIENT_INFO_HAVE_POKEMON_ITEM, "SC_OP_PUBLISH_CLIENT_INFO_HAVE_POKEMON_ITEM" },
    { SC_OP_SET_CURRENT_ROYAL_RANKING, "SC_OP_SET_CURRENT_ROYAL_RANKING" },
    { SC_OP_SET_ZENRYOKU_WAZA_USED, "SC_OP_SET_ZENRYOKU_WAZA_USED" },
    { SC_OP_SET_DIRT, "SC_OP_SET_DIRT" },
    { SC_OP_INC_WEATHER_PASSED_TURN, "SC_OP_INC_WEATHER_PASSED_TURN" },
    { SC_OP_SET_SPACT_PRIORITY, "SC_OP_SET_SPACT_PRIORITY" },
    { SC_ACT_WAZA_EFFECT, "SC_ACT_WAZA_EFFECT" },
    { SC_ACT_TAMEWAZA_HIDE, "SC_ACT_TAMEWAZA_HIDE" },
    { SC_ACT_WAZA_DMG, "SC_ACT_WAZA_DMG" },
    { SC_ACT_WAZA_DMG_PLURAL, "SC_ACT_WAZA_DMG_PLURAL" },
    { SC_ACT_WAZA_ICHIGEKI, "SC_ACT_WAZA_ICHIGEKI" },
    { SC_ACT_SICK_ICON, "SC_ACT_SICK_ICON" },
    { SC_ACT_CONF_DMG, "SC_ACT_CONF_DMG" },
    { SC_ACT_RANKUP, "SC_ACT_RANKUP" },
    { SC_ACT_RANKDOWN, "SC_ACT_RANKDOWN" },
    { SC_ACT_DEAD, "SC_ACT_DEAD" },
    { SC_ACT_RELIVE, "SC_ACT_RELIVE" },
    { SC_ACT_MEMBER_OUT_MSG, "SC_ACT_MEMBER_OUT_MSG" },
    { SC_ACT_MEMBER_OUT, "SC_ACT_MEMBER_OUT" },
    { SC_ACT_MEMBER_IN, "SC_ACT_MEMBER_IN" },
    { SC_ACTOP_WEATHER_START, "SC_ACTOP_WEATHER_START" },
    { SC_ACTOP_WEATHER_END, "SC_ACTOP_WEATHER_END" },
    { SC_ACTOP_MEGAEVO, "SC_ACTOP_MEGAEVO" },
    { SC_ACT_SIMPLE_HP, "SC_ACT_SIMPLE_HP" },
    { SC_ACT_USE_ITEM, "SC_ACT_USE_ITEM" },
    { SC_ACT_KILL, "SC_ACT_KILL" },
    { SC_ACTOP_MOVE, "SC_ACTOP_MOVE" },
    { SC_ACT_EXP, "SC_ACT_EXP" },
    { SC_ACT_NADERU, "SC_ACT_NADERU" },
    { SC_ACT_BALL_THROW, "SC_ACT_BALL_THROW" },
    { SC_ACT_BALL_THROW_CAPTURED, "SC_ACT_BALL_THROW_CAPTURED" },
    { SC_ACT_BALL_THROW_FORBIDDEN, "SC_ACT_BALL_THROW_FORBIDDEN" },
    { SC_ACTOP_CHANGE_TOKUSEI, "SC_ACTOP_CHANGE_TOKUSEI" },
    { SC_ACTOP_SWAP_TOKUSEI, "SC_ACTOP_SWAP_TOKUSEI" },
    { SC_ACT_FAKE_DISABLE, "SC_ACT_FAKE_DISABLE" },
    { SC_ACT_EFFECT_SIMPLE, "SC_ACT_EFFECT_SIMPLE" },
    { SC_ACT_EFFECT_BYPOS, "SC_ACT_EFFECT_BYPOS" },
    { SC_ACT_EFFECT_BYVECTOR, "SC_ACT_EFFECT_BYVECTOR" },
    { SC_ACT_EFFECT_FIELD, "SC_ACT_EFFECT_FIELD" },
    { SC_ACT_PLURALHIT_EX_2ND, "SC_ACT_PLURALHIT_EX_2ND" },
    { SC_ACT_CHANGE_FORM, "SC_ACT_CHANGE_FORM" },
    { SC_ACT_MIGAWARI_CREATE, "SC_ACT_MIGAWARI_CREATE" },
    { SC_ACT_MIGAWARI_DELETE, "SC_ACT_MIGAWARI_DELETE" },
    { SC_ACT_HENSIN, "SC_ACT_HENSIN" },
    { SC_ACT_MIGAWARI_DAMAGE, "SC_ACT_MIGAWARI_DAMAGE" },
    { SC_ACT_WIN_BGM, "SC_ACT_WIN_BGM" },
    { SC_ACT_MSGWIN_HIDE, "SC_ACT_MSGWIN_HIDE" },
    { SC_ACT_FRIENDSHIP_EFFECT, "SC_ACT_FRIENDSHIP_EFFECT" },
    { SC_ACT_FRIENDSHIP_EFFECT_MSG, "SC_ACT_FRIENDSHIP_EFFECT_MSG" },
    { SC_ACT_DEAD_PLURAL, "SC_ACT_DEAD_PLURAL" },
    { SC_ACT_ZENRYOKUWAZA_BEGIN, "SC_ACT_ZENRYOKUWAZA_BEGIN" },
    { SC_ACT_ZENRYOKUWAZA_END, "SC_ACT_ZENRYOKUWAZA_END" },
    { SC_ACT_OTEIRE_BUTTON_DISPLAY, "SC_ACT_OTEIRE_BUTTON_DISPLAY" },
    { SC_ACT_INTRUDE_CALL, "SC_ACT_INTRUDE_CALL" },
    { SC_ACT_INTRUDE_APPEAR, "SC_ACT_INTRUDE_APPEAR" },
    { SC_ACT_INTRUDE_NOT_APPEARD, "SC_ACT_INTRUDE_NOT_APPEARD" },
    { SC_ACT_TURN_START, "SC_ACT_TURN_START" },
    { SC_TOKWIN_IN, "SC_TOKWIN_IN" },
    { SC_TOKWIN_OUT, "SC_TOKWIN_OUT" },
    { SC_MSG_WAZA, "SC_MSG_WAZA" },
    { SC_MSG_STD, "SC_MSG_STD" },
    { SC_MSG_SET, "SC_MSG_SET" },
    { SC_MSG_STD_SE, "SC_MSG_STD_SE" },
    { SC_MSG_SET_SE, "SC_MSG_SET_SE" },
    { SC_MAX, "SC_MAX" },
    { SCEX_RESERVE, "SCEX_RESERVE" },
    { SCEX_VALIABLE_ARGS, "SCEX_VALIABLE_ARGS" },
    { SCEX_WAZAEFF_SYNC_CHAPTER, "SCEX_WAZAEFF_SYNC_CHAPTER" },
    { SCEX_SKIP, "SCEX_SKIP" },
  };
}


/**
 * @brief サーバーコマンドの内、最も長いラベル名の文字数を取得する
 */
u32 SCMD_GetMaxLabelLength( void )
{
  u32 cmdNameMaxLen = 0;
  for(u32 i=0; i<GFL_NELEMS(cmdNameTbl); ++i)
  {
    u32 len = 0;
    for(len=0; cmdNameTbl[i].name[len] != '\0'; ++len);
    if( len > cmdNameMaxLen ){
      cmdNameMaxLen = len;
    }
  }
  return cmdNameMaxLen;
}

/**
 * @brief サーバーコマンドの定義名の文字列を取得する
 * @param cmd  文字列を取得するコマンド
 */
const char* SCMD_GetLabelString( ServerCmd cmd )
{
  u32 i=0;
  while(i < GFL_NELEMS(cmdNameTbl)){
    if( cmdNameTbl[i].cmd == cmd ){
      return cmdNameTbl[i].name;
    }
    ++i;
  }
  return NULL;
}

#endif // PM_DEBUG




}   // end of namespace 'scmd'
GFL_NAMESPACE_END(btl)



