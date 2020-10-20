//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventAddPokemon.cpp
 *  @brief  ポケモン追加イベント
 *  @author miyuki iwasawa 
 *  @date   2015.09.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#ifndef __EVENTADDPOKEMON_H__
#define __EVENTADDPOKEMON_H__

#include <Fade/include/gfl2_FadeManager.h>

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameProc.h"

#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

//前方宣言

namespace Field
{

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//--------------------------------------------------------------
///	ポケモン追加テーブル 特性モード定義
//==============================================================
enum PokeAddTableTokuseiMode
{
  /**
    2017/03/31 Fri. 14:02:58
    MMCat[45]   サブイベントでもらえるポケモンで意図通りの特性フラグになっていないものがある
    
    コンバータでは
       "ランダム" => -1, #POKE_ADD_TBL_TOKUSEI_RND
       "特性1"    => 0,  #POKE_ADD_TBL_TOKUSEI_1
       "特性2"    => 1,  #POKE_ADD_TBL_TOKUSEI_2
       "特性3"    => 2,  #POKE_ADD_TBL_TOKUSEI_3
    となっていたのに対し、コードでは
      POKE_ADD_TBL_TOKUSEI_RND = 0,  ///< ランダム
      POKE_ADD_TBL_TOKUSEI_1 = 1,        ///< 特性1
      POKE_ADD_TBL_TOKUSEI_2 = 2,        ///< 特性2
      POKE_ADD_TBL_TOKUSEI_3 = 3,        ///< 特性3
    とマッピングが一致していなかった
    コード側を合わせることで対処
  */

  POKE_ADD_TBL_TOKUSEI_RND = -1,  ///< ランダム
  POKE_ADD_TBL_TOKUSEI_1 = 0,        ///< 特性1
  POKE_ADD_TBL_TOKUSEI_2 = 1,        ///< 特性2
  POKE_ADD_TBL_TOKUSEI_3 = 2,        ///< 特性3
};

//--------------------------------------------------------------
///	ポケモン追加テーブル レア定義
//==============================================================
enum PokeAddTableRareMode
{
  POKE_ADD_TBL_RARE_RND = 0,  ///< ランダム
  POKE_ADD_TBL_RARE_RARE = 1,     ///< レア
  POKE_ADD_TBL_RARE_NORMAL = 2,   ///< 通常
};

//--------------------------------------------------------------
///	ポケモン追加テーブル 性別
//==============================================================
enum PokeAddTableSexMode
{
  POKE_ADD_TBL_SEX_RANDOM = 0,  ///< ランダム
  POKE_ADD_TBL_SEX_MALE = 1,        ///< オス
  POKE_ADD_TBL_SEX_FEMALE = 2,      ///< メス
};

//--------------------------------------------------------------
///	ポケモン追加テーブル タマゴで追加モード指定
//==============================================================
enum PokeAddTableEggMode
{
  POKE_ADD_TBL_EGG_MODE_NONE = 0,  ///< タマゴじゃない 
  POKE_ADD_TBL_EGG_MODE_SODATEYA,    ///< 
};

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
  
// ポケモン追加　イベント
class EventAddPokemon : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventAddPokemon); // コピーコンストラクタ＋代入禁止
  
public:
  //--------------------------------------------------------------
  ///	ポケモン追加パラメータ
  //==============================================================
  struct AddPokemonParam
  {
    u16 monsno;  ///< モンスターNO
    u8  form;    ///< フォルムNO
    u8  level;   ///< レベル

    u8  rare_mode;       ///< 強制レア PokeAddTableRareMode
    u8  sex_mode;        ///< せいべつ PokeAddTableSexMode
    u8  tokusei_mode;    ///< とくせいID PokeAddTableTokuseiMode
    s8  seikaku_id;      ///< せいかくID

    u16 item_no;        ///< アイテムNO
    u16 egg_mode;       ///< タマゴ指定モード

    u16 wazano;         ///< 固有技(押しだし)
    s8 talent_hp;       ///< 個体値：HP
    s8 talent_atk;      ///< 個体値：ATK
    s8 talent_def;      ///< 個体値：DEF
    s8 talent_spatk;    ///< 個体値：SPATK
    s8 talent_spdef;    ///< 個体値：SPDEF
    s8 talent_agi;      ///< 個体値：AGI
  };

  enum ParamType{
    PARAM_TYPE_ARC,
    PARAM_TYPE_PP,
  };
  enum{
    SEQ_ARC_INIT,
    SEQ_ARC_INIT_WAIT,
    SEQ_APP_CALL_CHECK,
    SEQ_APP_CALL,
    SEQ_FIELD_OPEN,
    SEQ_END,
  };

  //----------------------------------------------------------------------------
  /**
   *	@brief  ポケモン追加シーケンスコール
   *
   *	@param	p_gman    ゲームマネージャ
   *	@param  addPokeID  AddPokeID
   *
   *	@return 生成したイベント
   */
  //-----------------------------------------------------------------------------
  static EventAddPokemon* CallAddPokemon( GameSys::GameManager* p_gman, u32 addPokeID, AddPokemonMode addMode );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ポケモン追加シーケンスコール PokemonParam指定版
   *
   *	@param	p_gman    ゲームマネージャ
   *	@param  addPokeID  AddPokeID
   *
   *	@return 生成したイベント
   *	
   *	PokemonParamのコピーを作って保持するので、この関数を呼び出した後はPokemonParamは破棄しても構わない
   */
  //-----------------------------------------------------------------------------
  static EventAddPokemon* CallAddPokemonFromPP( GameSys::GameManager* p_gman, const pml::pokepara::PokemonParam& p_pp, AddPokemonMode addMode );

  void SetParam( u32 addPokeID, AddPokemonMode addMode );
  void SetParamFromPP( const pml::pokepara::PokemonParam& p_pp,AddPokemonMode addMode );
  void SetFadeParam( gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4& start_col, const gfl2::math::Vector4& end_col,
    u32 sync = gfl2::Fade::FADE_DEFAULT_SYNC, bool fade_in = true );

public:
  // コンストラクタデストラクタ
  EventAddPokemon( gfl2::heap::HeapBase* heap );
  virtual ~EventAddPokemon(){}
      

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
  bool InitializeWait( GameSys::GameManager* p_gman );
  //--------------------------------------------------------------
  /**
   * @brief 図鑑登録チェッカー
   * @param p_gman  ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  bool CheckZukanRegist(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief 図鑑登録アプリコール
   * @param p_gman  ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  void CallZukanRegist(GameSys::GameManager* p_gman);

  pml::pokepara::PokemonParam* MakePokeParam( GameSys::GameManager* p_gman, const AddPokemonParam* cp_param );
private:
  s32 m_Seq;
  s32 m_InitSeq;
  u32 m_AddPokeID;
  ParamType m_paramType;
  AddPokemonMode m_AddPokeMode;

  gfl2::Fade::DISP      m_FadeDisp;
  gfl2::Fade::FADE_TYPE m_FadeType;
  gfl2::math::Vector4   m_FadeStartCol;
  gfl2::math::Vector4   m_FadeEndCol;
  u32                   m_FadeSync;
  bool                  m_FadeInEnable;

  app::event::ZukanRegisterEventParam m_EventParam;

  GameSys::GameManager* m_pGameManager;
  gfl2::heap::HeapBase* m_pHeap;
  void* m_pBuffer;
  u32   m_BufferSize;
  pml::pokepara::PokemonParam* m_pPokeParam;
};


} // namespace Field
  
#endif  // __EVENTADDPOKEMON_H__

