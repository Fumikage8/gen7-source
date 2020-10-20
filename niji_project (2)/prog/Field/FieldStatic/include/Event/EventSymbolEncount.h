//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventSymbolEncount.cpp
 *  @brief  シンボルエンカウトイベント
 *  @author miyuki iwasawa 
 *  @date   2015.09.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#ifndef __EVENTSYMBOLENCOUNT_H__
#define __EVENTSYMBOLENCOUNT_H__

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameProc.h"

// sound
#include "Sound/include/Sound.h"

#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

//前方宣言
namespace Field{

  class ZoneDataLoader;

  namespace Encount {
    class PokeSet;
    class EncountEffect;
    struct SymbolEncountParam;
  }

  namespace Event {
    class BattleDllReadThread;
  }
}

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//--------------------------------------------------------------
///	シンボルエンカウトテーブル 特性モード定義
//==============================================================
enum PokeAddTableTokuseiMode
{
  POKE_ADD_TBL_TOKUSEI_RND = 0,  ///< ランダム
  POKE_ADD_TBL_TOKUSEI_1 = 1,        ///< 特性1
  POKE_ADD_TBL_TOKUSEI_2 = 2,        ///< 特性2
  POKE_ADD_TBL_TOKUSEI_3 = 3,        ///< 特性3
};

//--------------------------------------------------------------
///	シンボルエンカウトテーブル レア定義
//==============================================================
enum PokeAddTableRareMode
{
  POKE_ADD_TBL_RARE_RND = 0,  ///< ランダム
  POKE_ADD_TBL_RARE_RARE = 1,     ///< レア
  POKE_ADD_TBL_RARE_NORMAL = 2,   ///< 通常
};

//--------------------------------------------------------------
///	シンボルエンカウトテーブル 性別
//==============================================================
enum PokeAddTableSexMode
{
  POKE_ADD_TBL_SEX_RANDOM = 0,  ///< ランダム
  POKE_ADD_TBL_SEX_MALE = 1,        ///< オス
  POKE_ADD_TBL_SEX_FEMALE = 2,      ///< メス
};

//--------------------------------------------------------------
///	シンボルエンカウトテーブル タマゴで追加モード指定
//==============================================================
enum PokeAddTableEggMode
{
  POKE_ADD_TBL_EGG_MODE_NONE = 0,  ///< タマゴじゃない 
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
  
// シンボルエンカウト　イベント
class EventSymbolEncount : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventSymbolEncount); // コピーコンストラクタ＋代入禁止
  
public:
  //----------------------------------------------------------------------------
  /**
   *	@brief  シンボルエンカウトシーケンスコール
   *
   *	@param	p_gman    ゲームマネージャ
   *	@param  symbolID 
   *	@param  call_option  enum EventBattleCallOption  
   *	@param  ret_bgm_id 
   *
   *	@return 生成したイベント
   */
  //-----------------------------------------------------------------------------
  void SetParam( PokeSet* p_pokeset, u32 symbol_id, u32 call_option = 0/*EVBTL_CALLOPT_NON*/, u32 ret_bgm_id = Sound::SOUND_ITEM_ID_NONE);

public:
  // コンストラクタデストラクタ
  EventSymbolEncount( gfl2::heap::HeapBase* heap ) : 
    GameSys::GameEvent( heap ), m_Seq(0),  m_InitSeq(0), m_SymbolID(0), m_pHeap(NULL), m_pBuffer(NULL), m_BufferSize(0), m_pPokeParam(NULL), m_pPokeSet(NULL) {}

  virtual ~EventSymbolEncount(){}
      

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
   * @brief バトルアプリコール
   * @param p_gman  ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  void CallBattle(GameSys::GameManager* p_gman);

  pml::pokepara::PokemonParam* MakePokeParam( GameSys::GameManager* p_gman, const SymbolEncountParam* cp_param );
private:
  s32 m_Seq;
  s32 m_InitSeq;
  GameSys::GameManager* m_pGameManager;
  gfl2::heap::HeapBase* m_pHeap;
  void* m_pBuffer;
  u32   m_BufferSize;
  pml::pokepara::PokemonParam* m_pPokeParam;
  
  PokeSet* m_pPokeSet;
  u32      m_SymbolID;
  u32      m_CallOption;
  u32      m_RetBGMID;
};


GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )

#endif  // __EVENTSYMBOLENCOUNT_H__

