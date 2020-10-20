//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventRizaadonRide.cpp
 *  @brief  リザードンライドイベント
 *  @author miyuki iwasawa 
 *  @date   2015.11.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#ifndef __EVENTRIZAADONRIDE_H__
#define __EVENTRIZAADONRIDE_H__

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameProc.h"

#include "niji_reference_files/script/FieldPawnTypes.h"
#include "App/AppEvent/include/TownMap/TownMapEventListener.h"

//前方宣言

namespace Field
{

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
  
// ポケモン追加　イベント
class EventRizaadonRide : public GameSys::GameEvent,public App::Event::TownMapEventListener
{
  GFL_FORBID_COPY_AND_ASSIGN(EventRizaadonRide); // コピーコンストラクタ＋代入禁止
  
public:
  
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
  static EventRizaadonRide* CallRizaadonRide( GameSys::GameManager* p_gman);

public:
  // コンストラクタデストラクタ
  EventRizaadonRide( gfl2::heap::HeapBase* heap );

  virtual ~EventRizaadonRide(){}
      

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

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTermination
   * @brief   終了の通知
   * @param   warp_id  選ばれたワープID キャンセルの場合 App::TownMap::FLY_WARPID_CANCEL
   */
  //-----------------------------------------------------------------------------
  virtual void OnTermination(s32 warp_id);

private:
  void Initialize( GameSys::GameManager* p_gman );
  bool InitializeWait( GameSys::GameManager* p_gman );
  //--------------------------------------------------------------
  /**
   * @brief タウンマップアプリコール
   * @param p_gman  ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  void CallTownmap(GameSys::GameManager* p_gman);

private:
  s32 m_Seq;
  s32 m_WarpID;
  poke_3d::model::DressUpParam              m_DressUpParam;  //自分の服装を覚えておく
  Field::MoveModel::FieldMoveModelManager*  m_pFieldCharaModelManager;
  Field::MoveModel::FieldMoveModelPlayer*   m_pPlayer;

  GameSys::GameManager* m_pGameManager;
  gfl2::heap::HeapBase* m_pHeap;
  void* m_pBuffer;
  u32   m_BufferSize;
};


} // namespace Field
  
#endif  // __EVENTRIZAADONRIDE_H__

