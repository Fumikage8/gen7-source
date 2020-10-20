//=============================================================================================
/**
 * @file  btl_common.h
 * @brief ポケモンXY バトルシステム ローカル共通ヘッダ
 * @author  taya
 *
 * @date  2010.09.06  作成
 */
//=============================================================================================
#ifndef __BTL_COMMON_H__
#define __BTL_COMMON_H__

#include <pml/include/pmlib.h>

#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_set.h>

#include <heap/include/gfl2_heap.h>
#include <math/include/gfl2_math_random.h>
#include <Savedata/include/ConfigSave.h>

#include <Battle/include/battle_SetupParam.h>
#include <Battle/include/battle_fld_env_def.h>

#include "./btl_fx32.h"
#include "./btl_const.h"
#include "./btl_PokemonPosition.h"


//#include "pm_define.h"
//#include "waza_tool\wazadata.h"

/*--------------------------------------------------------------------------*/
/* Compile Flags                                                            */
/*--------------------------------------------------------------------------*/
#define BTL_OLD_PRINT_DEBUG   (0) ///< 有効にするとWBのデバッグ用Print処理が有効になる
#define BTL_OLD_IGNORE        (0) ///< 有効にするとWB独自仕様箇所（その可能性が高い箇所）が有効になる
#define BTL_TMP_IGNORE        (0) ///< 有効にすると一時的に無効化してある箇所が有効になる（最後は有効）
#define _BTL_ENCOUNT_SKIP     (0) ///< 有効にすると、エンカウント演出をスキップしてシーケンスを進める
#define _BTL_UI_SKIP          (0) ///< 有効にすると、クライアントのアクション選択をスキップして適当な値を返す


#if BTL_OLD_PRINT_DEBUG

#else
#define BTL_Printf( ... )            /* */
#define BTL_PrintfEx( ... )          /* */
#define TAYA_Printf( ... )           /* */

#endif

#include "./btl_DebugPrint.h"

GFL_NAMESPACE_BEGIN(btl)
/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef gfl2::heap::HeapBase*                HeapHandle;
typedef gfl2::str::StrBuf                    STRBUF;
typedef gfl2::str::STRCODE                   STRCODE;
typedef gfl2::math::Random                   AIRandSys;
typedef gfl2::math::SFMTRandom               RandSys;
typedef tinymt32_t                           RandContext;
typedef gflnet2::p2p::NetGame       GflNet;

typedef pml::pokepara::Sick                 PokeSick;
typedef pml::pokepara::PokemonParam         PokePara;
typedef pml::pokepara::DirtType             DirtType;
typedef pml::PokeParty                      PokeParty;
typedef WazaNo                              WazaID;
typedef pml::ItemNo                         ItemNo;
typedef pml::Sex                            Sex;

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  POKESICK_ORIGIN = pml::pokepara::SICK_MAHI,
  POKESICK_MAX    = pml::pokepara::SICK_MAX,
  TRID_NULL       = 0,
};

inline bool IsPokeSick( unsigned int sickID )
{
  return (sickID < POKESICK_MAX);
}

//--------------------------------------------------------------
/**
 *  メッセージ描画速度
 */
//--------------------------------------------------------------
enum MsgSpeed {
  MSGSPEED_SLOW       =  Savedata::ConfigSave::MSG_SLOW,    // 1文字につき2フレーム
  MSGSPEED_NORMAL     =  Savedata::ConfigSave::MSG_NORMAL,  // 1文字につき1フレーム
  MSGSPEED_FAST       =  Savedata::ConfigSave::MSG_FAST,    // 1文字につき0.5フレーム
  MSGSPEED_SUPER_FAST =  Savedata::ConfigSave::MSG_NO_WAIT, // 1文字につき0.333..フレーム
};

//--------------------------------------------------------------
/**
 *  バッグモード
 */
//--------------------------------------------------------------
enum BtlBagMode{
  BBAG_MODE_NORMAL = 0, // 通常モード
  BBAG_MODE_SHOOTER,    // サポートシューター
  BBAG_MODE_GETDEMO,    // 捕獲デモモード
  BBAG_MODE_PDC,        // ポケモンドリームキャッチ
};

//--------------------------------------------------------------
/**
 *  クライアントタイプ
 */
//--------------------------------------------------------------
typedef enum {

  BTL_CLIENT_TYPE_UI = 0,   ///< プレイヤー
  BTL_CLIENT_TYPE_AI,       ///< AI
  BTL_CLIENT_TYPE_RECPLAY,  ///< 録画再生

  BTL_CLIENT_TYPE_MAX,
  BTL_CLIENT_TYPE_NONE = BTL_CLIENT_TYPE_MAX,

}BtlClientType_tag;

typedef  u8  BtlClientType;

//--------------------------------------------------------------
/**
 *  「逃げる」を選択した時の反応タイプ
 */
//--------------------------------------------------------------
typedef enum {

  BTL_ESCAPE_MODE_WILD,       ///< すぐに「にげる」確定（野生戦など）
  BTL_ESCAPE_MODE_NG,       ///< しょうぶのさいちゅうに背中を見せられない
  BTL_ESCAPE_MODE_CONFIRM,  ///< にげると負けになるけどホントにいいの？と確認する

  BTL_ESCAPE_MODE_MAX,

}BtlEscapeMode;

//--------------------------------------------------------------
/**
 *  ダブルバトル時のチーム構成
 */
//--------------------------------------------------------------
typedef enum {
  BTL_DBLTEAM_ALONE_ALONE,  ///< 個人vs個人
  BTL_DBLTEAM_ALONE_TAG,    ///< 個人vsタッグ
  BTL_DBLTEAM_TAG_ALONE,    ///< タッグvs個人
  BTL_DBLTEAM_TAG_TAG,      ///< タッグvsタッグ
}BtlDblTeamFormation;

//--------------------------------------------------------------
/**
 *  はい／いいえ選択
 */
//--------------------------------------------------------------
typedef enum {
  BTL_YESNO_YES = 0,
  BTL_YESNO_NO,
}BtlYesNo;


typedef enum {
  BTL_VPOS_NEAR_1 = 0,
  BTL_VPOS_FAR_1,
  BTL_VPOS_NEAR_2,
  BTL_VPOS_FAR_2,
  BTL_VPOS_NEAR_3,
  BTL_VPOS_FAR_3,

  //ROYAL用定義
  BTL_VPOS_ROYAL_NEAR = 0,
  BTL_VPOS_ROYAL_RIGHT,
  BTL_VPOS_ROYAL_FAR,
  BTL_VPOS_ROYAL_LEFT,

  BTL_VPOS_ERROR = 0xff,

}BtlvPos;

typedef enum {
  BTL_VPOS_TR_NEAR_1,
  BTL_VPOS_TR_FAR_1,
  BTL_VPOS_TR_NEAR_2,
  BTL_VPOS_TR_FAR_2,
  BTL_VPOS_TR_ERROR = 0xff,
}BtlvTrainerPos;

//--------------------------------------------------------------
/**
 *  レベルアップ時の各種パラメータ上昇値
 */
//--------------------------------------------------------------
typedef struct {
  u8   level;
  u16  hp;
  u16  atk;
  u16  def;
  u16  sp_atk;
  u16  sp_def;
  u16  agi;
}BTL_LEVELUP_INFO;

//--------------------------------------------------------------
/**
 *  かわいがりエフェクト
 */
//--------------------------------------------------------------
enum FriendshipEffect {
  FREFF_HEART = 0,  ///< ハートが出る
  FREFF_FURIMUKI,   ///< ふりむく
  FREFF_BOTH,       ///< 両方
};


//--------------------------------------------------------------
/**
 *  定数
 */
//--------------------------------------------------------------
enum {
  STRID_NULL = 0xffff,   ///< 無効文字列ID
  WAZASERIAL_NULL = 0,   ///< ワザ実行ごとのシリアル無効値
};

//--------------------------------------------------------------
/**
 *  対戦勝敗判定構造体
 */
//--------------------------------------------------------------
typedef struct {
  u16         clientID;     ///< 勝敗判定したマシンのクライアントID
  u16         resultCode;   ///< 勝敗判定したマシンでの勝敗コード
  ResultCause resultCause;  ///< 勝敗判定の判定理由
}BTL_RESULT_CONTEXT;




//===================================================================
// ポケモンタイプを合成して１変数として扱うための措置
//===================================================================
typedef u16 PokeTypePair;

inline PokeTypePair PokeTypePair_Make( pml::PokeType type1, pml::PokeType type2, pml::PokeType type_ex )
{
  return ( ((type_ex & 0x1f) << 10) | ((type2 & 0x1f) << 5) | (type1 & 0x1f) );
}

inline PokeTypePair PokeTypePair_MakePure( pml::PokeType type )
{
  return PokeTypePair_Make( type, type, POKETYPE_NULL );
}

inline pml::PokeType PokeTypePair_GetType1( PokeTypePair pair )
{
  return pair & 0x1f;
}

inline pml::PokeType PokeTypePair_GetType2( PokeTypePair pair )
{
  return (pair >> 5 ) & 0x1f;
}

inline pml::PokeType PokeTypePair_GetTypeEx( PokeTypePair pair )
{
  return (pair >> 10) & 0x1f;
}

inline void PokeTypePair_Split( PokeTypePair pair, pml::PokeType* type1, pml::PokeType* type2, pml::PokeType* typeEx )
{
  *type1  = PokeTypePair_GetType1( pair );
  *type2  = PokeTypePair_GetType2( pair );
  *typeEx = PokeTypePair_GetTypeEx( pair );
}

inline bool PokeTypePair_IsMatch( PokeTypePair pair, pml::PokeType type )
{
  if( type != POKETYPE_NULL )
  {
    if( PokeTypePair_GetType1(pair) == type ){
      return true;
    }
    if( PokeTypePair_GetType2(pair) == type ){
      return true;
    }
    if( PokeTypePair_GetTypeEx(pair) == type ){
      return true;
    }
  }
  return false;
}
inline bool PokeTypePair_IsMatchEither( PokeTypePair pair1, PokeTypePair pair2 )
{
  pml::PokeType  types[2][3];

  PokeTypePair_Split( pair1, &(types[0][0]), &(types[0][1]), &(types[0][2]) );
  PokeTypePair_Split( pair2, &(types[1][0]), &(types[1][1]), &(types[1][2]) );

  for(int i=0; i<3; ++i)
  {
    if( types[0][i] == POKETYPE_NULL ){
      continue;
    }
    for( int j=0; j<3; ++j )
    {
      if( types[0][i] == types[1][j] ){
        return true;
      }
    }
  }

  return false;
}


inline bool PokeTypePair_IsPure( PokeTypePair pair )
{
  if( (PokeTypePair_GetType1(pair) == PokeTypePair_GetType2(pair))
  &&  (PokeTypePair_GetTypeEx(pair) == POKETYPE_NULL)
  ){
    return true;
  }
  return false;
}

inline PokeTypePair PokeTypePair_Replace( PokeTypePair pair, pml::PokeType targetType, pml::PokeType newType )
{
  pml::PokeType type1  = PokeTypePair_GetType1( pair );
  pml::PokeType type2  = PokeTypePair_GetType2( pair );
  pml::PokeType typeEx = PokeTypePair_GetTypeEx( pair );

  type1  = ( type1  == targetType ) ? ( newType ) : ( type1 );
  type2  = ( type2  == targetType ) ? ( newType ) : ( type2 );
  typeEx = ( typeEx == targetType ) ? ( newType ) : ( typeEx );

  return PokeTypePair_Make( type1, type2, typeEx );
}

inline bool PokeTypePair_IsAnyTypeExist( PokeTypePair pair )
{
  pml::PokeType type1  = PokeTypePair_GetType1( pair );
  pml::PokeType type2  = PokeTypePair_GetType2( pair );
  pml::PokeType typeEx = PokeTypePair_GetTypeEx( pair );

  if( ( type1  != POKETYPE_NULL ) ||
      ( type2  != POKETYPE_NULL ) ||
      ( typeEx != POKETYPE_NULL ) )
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------
// ワザデータアクセス
//----------------------------------------------------------------------------------


typedef pml::wazadata::WazaCategory               WazaCategory;
typedef pml::wazadata::WazaDamageType             WazaDamageType;
typedef pml::wazadata::SICKCONT_PARAM             SICKCONT_PARAM;
typedef pml::wazadata::WazaRankEffect             WazaRankEffect;
typedef pml::wazadata::WazaFlag                   WazaFlag;
typedef pml::wazadata::WazaSick                   WazaSick;
typedef pml::wazadata::WazaTarget                 WazaTarget;
typedef pml::wazadata::ZenryokuWazaEffect         ZenryokuWazaEffect;
typedef pml::waza::ZenryokuWazaKind               ZenryokuWazaKind;
typedef pml::waza::ZenryokuWazaSystem::ZenryokuWazaSource ZenryokuWazaSource;
typedef pml::battle::TypeAffinity::AffinityID      BtlTypeAff;
typedef pml::battle::TypeAffinity::AboutAffinityID BtlTypeAffAbout;



inline int WAZADATA_GetParam( WazaID wazaID, pml::wazadata::ParamID paramID )
{
  return pml::wazadata::GetParam( wazaID, paramID );
}
inline pml::PokeType WAZADATA_GetType( WazaID id )
{
  return pml::wazadata::GetType( id );
}
inline WazaCategory WAZADATA_GetCategory( WazaID id )
{
  return pml::wazadata::GetCategory( id );
}
inline WazaDamageType WAZADATA_GetDamageType( WazaID id )
{
  return pml::wazadata::GetDamageType( id );
}
inline SICKCONT_PARAM WAZADATA_GetSickCont( WazaID id )
{
  SICKCONT_PARAM  param = pml::wazadata::GetSickCont( id );
  return param;
}
inline WazaRankEffect WAZADATA_GetRankEffect( WazaID id, u32 idx, int* volume )
{
  return pml::wazadata::GetRankEffect( id, idx, volume );
}
inline u8 WAZADATA_GetRankEffectCount( WazaID id )
{
  return pml::wazadata::GetRankEffectCount( id );
}
inline u32 WAZADATA_GetPower( WazaID id )
{
  return pml::wazadata::GetPower( id );
}

inline bool WAZADATA_GetFlag( WazaID id, WazaFlag flag )
{
  return pml::wazadata::GetFlag( id, flag );
}
inline bool WAZADATA_IsAlwaysHit( WazaID id )
{
  return pml::wazadata::IsAlwaysHit( id );
}
inline bool WAZADATA_IsMustCritical( WazaID id )
{
  return pml::wazadata::IsMustCritical( id );
}
inline u32 WAZADATA_GetMaxPP( WazaID id, u32 ppup_cnt )
{
  return pml::wazadata::GetMaxPP( id, ppup_cnt );
}
inline BtlWeather WAZADATA_GetWeather( WazaID id )
{
  return pml::wazadata::GetWeather( id );
}

inline bool WAZADATA_IsDamage( WazaID id )
{
  return pml::wazadata::IsDamage( id );
}

inline WazaNo WAZADATA_GetZenryokuWazaNo( WazaID id )
{
  return pml::wazadata::GetZenryokuWazaNo( id );
}
inline u16 WAZADATA_GetZenryokuWazaPower( WazaID id )
{
  return pml::wazadata::GetZenryokuWazaPower( id );
}
inline ZenryokuWazaEffect WAZADATA_GetZenryokuWazaEffect( WazaID id )
{
  return pml::wazadata::GetZenryokuWazaEffect( id );
}

inline DirtType WAZADATA_GetDirtType( WazaID id )
{
  u32 param = pml::wazadata::GetParam( id, pml::wazadata::PARAM_ID_DIRT_TYPE );
  return static_cast<DirtType>( param );
}
inline u8 WAZADATA_GetDirtRate( WazaID id )
{
  return pml::wazadata::GetParam( id, pml::wazadata::PARAM_ID_DIRT_RATE );
}

//===================================================================
// 安全配列代入（klockwork対処）
//===================================================================
  template <class T>
  class  SafeAry {
    public:
      // ctor / dtor
      SafeAry() : m_adress(NULL), m_elems(0), m_dmy(0) {};
      ~SafeAry() {};

      // setup
      void Setup( T* adress, u32 elems )
      {
        m_adress = adress;
        m_elems  = elems;
      }

      T& operator []( u32 idx )
      {
        if( m_adress && (idx < m_elems) )
        {
          return m_adress[ idx ];
        }
        return m_dmy;
      }

      const T& operator []( u32 idx ) const
      {
        if( m_adress && (idx < m_elems) )
        {
          return m_adress[ idx ];
        }
        return m_dmy;
      }


    private:
      T*    m_adress;
      u32   m_elems;
      T     m_dmy;

  };
GFL_NAMESPACE_END(btl)

#endif  // #ifndef __BTL_COMMON_H__
