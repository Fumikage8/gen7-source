//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventDebugPokeMake.cpp
 *  @brief  デバッグポケモン生成イベント
 *  @author miyuki iwasawa 
 *  @date   2015.09.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#ifndef __EVENTADDPOKEMON_H__
#define __EVENTADDPOKEMON_H__

#if PM_DEBUG

#include <heap/include/gfl2_Heap.h>

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameProc.h"

#include "Debug/DebugBattle/include/DebugBattleProc.h"

#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

//前方宣言


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Debug )

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------
  
// デバッグポケモン生成　イベント
class EventDebugPokeMake : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventDebugPokeMake); // コピーコンストラクタ＋代入禁止
  
public:
  enum DebugPokeMakeMode{
    MODE_ADD,
    MODE_EDIT,
    MODE_SODATEYA,
  };
  static u32 DebugPokeEditIndex;


  typedef void (*ReflectCallback)( debug::PokeMake::APP_PARAM* p_param, GameSys::GameManager* p_gman );

  //----------------------------------------------------------------------------
  /**
   *  @brief  デバッグポケモン生成 コール
   *
   *  @param  p_gman        ゲームマネージャ
   *
   *  @return 生成したイベント
   */
  //-----------------------------------------------------------------------------
  static EventDebugPokeMake* CallDebugPokeAdd( GameSys::GameManager* p_gman, bool field_control );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  デバッグポケモンエディット コール
   *
   *  @param  p_gman        ゲームマネージャ
   *  @param  add_poke_id   AddPokeID
   *
   *  @return 生成したイベント
   */
  //-----------------------------------------------------------------------------
  static EventDebugPokeMake* CallDebugPokeEdit( GameSys::GameManager* p_gman, u32 party_index, bool field_control );
  
  //----------------------------------------------------------------------------
  /**
  *  @brief  デバッグポケモンエディット(育て屋) コール
  *
  *  @param  p_gman        ゲームマネージャ
  *  @param  add_poke_id   AddPokeID
  *
  *  @return 生成したイベント
  */
  //-----------------------------------------------------------------------------
  static EventDebugPokeMake* CallDebugSodateyaPokeEdit( GameSys::GameManager* p_gman, u32 party_index, bool field_control ,debug::PokeMake::APP_PARAM* p_app_param, ReflectCallback p_reflect_callback );

  //--------------------------------------------------------------
  /**
   * @brief 呼び出しパラメータセットアップ  
   * @param p_gman  ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  void SetFieldControlFlag( bool flag ){ m_FieldControlFlag = flag; }
  void SetParamAdd();
  void SetParamEdit( u32 index );
  void SetParamSodateya();

public:
  // コンストラクタデストラクタ
  EventDebugPokeMake( gfl2::heap::HeapBase* heap );
  virtual ~EventDebugPokeMake(){}
      

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* p_gman);



private:
  void Initialize( GameSys::GameManager* p_gman );

  //--------------------------------------------------------------
  /**
   * @brief 編集パラメータの反映   
   * @param p_gman  ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  void ReflectParam(GameSys::GameManager* p_gman);
  
  //--------------------------------------------------------------
  /**
   * @brief DebugPokeMakeアプリコール
   * @param p_gman  ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  void CallDebugPokeMake(GameSys::GameManager* p_gman);

private:
  s32 m_Seq;
  debug::PokeMake::DebugPokeMake* m_pProc;

  u32 m_Mode;
  bool m_FieldControlFlag;
  bool m_CreateParam;
  u32 m_PartyIndex;
  debug::PokeMake::APP_PARAM* m_pParam;
  pml::PokeParty* m_pMyParty;
  pml::pokepara::PokemonParam* m_pPokeParam;
  DefaultPowerUpDesc m_DefaultPowerUpDesc;
  ReflectCallback m_pReflectCallback;
  GameSys::GameManager* m_pGameManager;
  gfl2::heap::HeapBase* m_pHeap;
};

GFL_NAMESPACE_END( Debug )
GFL_NAMESPACE_END( Field )

#endif //PM_DEBUG

#endif  // __EVENTADDPOKEMON_H__

