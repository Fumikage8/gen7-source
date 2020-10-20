#if !defined( __APP_STATUS_TRAINER_MEMO_H_INCLUDED__ )
#define  __APP_STATUS_TRAINER_MEMO_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusTrainerMemo.h
 * @date    2015/12/02 14:58:36
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：トレーナーメモ(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <str/include/gfl2_Str.h>
#include <Layout/include/gfl2_Layout.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)

GFL_NAMESPACE_BEGIN(Savedata)
class MyStatus;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   トレーナーメモ(上)
//------------------------------------------------------------------------------
class TrainerMemo
{
  GFL_FORBID_COPY_AND_ASSIGN( TrainerMemo );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  TrainerMemo( DrawerBase* base, gfl2::str::MsgData* msgData );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~TrainerMemo( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup( const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   トレーナーメモのセットアップ
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup_TrainerMemo( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   タマゴメモのセットアップ
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup_EggMemo( const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   第１性格のセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetTrainerMemo_Seikaku( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   メインメモのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetTrainerMemo_Main( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   第２性格のセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetTrainerMemo_Seikaku2( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   メインメモのメッセージIDの取得
   *
   * @param   pp    ポケモンパラメータ
   *
   * @return  条件にあったメッセージID
   */
  //------------------------------------------------------------------
  u32 GetMainMemoIndex( const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   タマゴのメイン情報をセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetEggMemo_Main( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   タマゴの様子をセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetEggMemo_State( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   タマゴのメイン情報のメッセージIDの取得
   *
   * @param   pp    ポケモンパラメータ
   *
   * @return  条件にあったメッセージID
   */
  //------------------------------------------------------------------
  u32 GetMainMemoIndexEgg( const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メインメモのセット
   *
   * @param   pp      ポケモンパラメータ
   * @return  msgID   メッセージID
   */
  //------------------------------------------------------------------
  void SetMainMemo( const pml::pokepara::PokemonParam* pp, u32 msgID );

  //------------------------------------------------------------------
  /**
   * @brief   昔のバージョンかどうか
   *
   * @param   version   ROMバージョン
   *
   * @return  "true  = 旧バージョン"
   * @return  "false = 新バージョン"
   */
  //------------------------------------------------------------------
  bool IsOldVersion( u32 version ) const;

  //------------------------------------------------------------------------------
  /**
   * @brief   VCかどうか
   *
   * @param   version   ROMバージョン
   *
   * @return  "true  = VC"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------------------
  bool IsVirtualConsole( u32 version ) const;

private:
  enum {
    STR_MEMO_TOP,
    STR_MEMO_MIDDLE,
    STR_MEMO_BOTTOM,
    STR_MEMO_NUM,
  };

private:
  DrawerBase*                           m_pBase;

  gfl2::str::MsgData*                   m_pMsgData;

  Savedata::MyStatus*                   m_pMyStatus;

  gfl2::str::StrBuf*                    m_pStrBuf[STR_MEMO_NUM];

  const pml::pokepara::PokemonParam*    m_pPokeParam;

private:
  gfl2::lyt::LytPane*           m_pTrainerMemo;

  gfl2::lyt::LytTextBox*        m_pTextTrainerMemo;

  gfl2::lyt::LytPane*           m_pEggMemo;

  gfl2::lyt::LytTextBox*        m_pTextEggMemo;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_TRAINER_MEMO_H_INCLUDED__
