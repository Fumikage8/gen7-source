//======================================================================
/**
 * @file FieldEventPokeFinderForPlacement.h
 * @date 2015/10/06 13:07:41
 * @author miyachi_soichi
 * @brief イベント：ポケファインダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EVENT_POKEFINDER_FOR_PLACEMENT_H_INCLUDED__
#define __FIELD_EVENT_POKEFINDER_FOR_PLACEMENT_H_INCLUDED__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// pokelib
#include "niji_conv_header/poke_lib/monsno_def.h"
// GameSys
#include "GameSys/include/GameEvent.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
// field
#include "Field/FieldStatic/include/FieldLocation.h"
// menu
#include "App/FinderMenu/include/FinderMenuAppParam.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(Field)
class Fieldmap;
GFL_NAMESPACE_BEGIN(Finder)
class Manager;
GFL_NAMESPACE_END(Finder)
#if PM_DEBUG
GFL_NAMESPACE_BEGIN(Debug)
class DebugPokeFinder;
GFL_NAMESPACE_END(Debug)
#endif
GFL_NAMESPACE_BEGIN(Event)

// ファインダー試練の返却定義
enum FINDER_RESULT
{
  FINDER_RESULT_CANCEL,   // キャンセル
  FINDER_RESULT_FULL,     // 6枚撮った
  FINDER_RESULT_SUCCESS,  // 成功した
};

/**
 *  @class  EventPokeFinderForPlacement
 *  @brief  イベント：ポケファインダー
 */
class EventPokeFinderForPlacement :public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventPokeFinderForPlacement);

public:
  /**
   *  初期化用構造体
   */
  struct SetupData
  {
    u32                   spot_id;  //!< 撮影スポットID
    u32                   mons_no;  //!< 試練用ポケモン指定
    FINDER_RESULT         *out_res; //!< 結果返却用ポインタ
    // ctor
    SetupData( void )
      : spot_id( 0 )
      , mons_no( MONSNO_NULL )
      , out_res( NULL )
    {
    }
  };

  /**
   *  状態
   */
  enum
  {
    INIT_NONE = 0,        //!< 未初期化
    INIT_RO,
    INIT_FADEOUT_WAIT,    //!< フェードアウト待ち
    INIT_FADEIN,           //!< フェードイン
    INIT_FADEIN_WAIT,      //!< フェードイン待ち
    MAIN,                  //!< メインフレーム
    TERM_FADEOUT,
    TERM_FADEOUT_WAIT,
    TERM_DELETE,
    TERM_DELETE_WAIT,
    MENU_FIELD_CLOSE,
    MENU_CALL,
    MENU_WAIT,
    MENU_FIELD_OPEN,
    TERM_POTAL,
    TERM_FADEIN,
    TERM_FADEIN_WAIT,
    END,                  //!< 終了
  };

public:
  /**
   *  @brief  コンストラクタ
   *  @param  pSystemHeap   GameEvent側で利用するヒープ領域
   */
  EventPokeFinderForPlacement( gfl2::heap::HeapBase *pSystemHeap );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EventPokeFinderForPlacement( void );

  /**
   *  @brief  初期化処理
   *  @note   インスタンス生成直後に実行すること
   *  @param  rData   初期化設定
   */
  void Initialize( const SetupData &rData );

  /**
   *  @brief  イベントを動かしてよいかどうかの判定関数
   *  @param  gmgr    ゲームマネージャ
   *  @retval true    起動していい
   *  @retval false   起動しない
   */
  virtual bool BootChk( GameSys::GameManager *gmgr );

  /**
   *  @brief  BootChkがTrueの場合に呼び出される初期化関数
   *  @param  gmgr    ゲームマネージャ
   */
  virtual void InitFunc( GameSys::GameManager *gmgr );

  /**
   *  @brief  呼び出される関数
   *  更新処理
   *  @param  gmgr    ゲームマネージャ
   *  @return GameSys::GMEVENT_RESULT   イベント制御関数の戻り値
   */
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager *gmgr );

  /**
   *  @brief  呼び出される関数
   *  終了処理関数
   *  @param  gmgr    ゲームマネージャ
   */
  virtual void EndFunc( GameSys::GameManager *gmgr );

private:
  /**
   *  @brief  フェードアウト
   */
  void RequestFadeOut( void );

  /**
   *  @brief  フェードイン
   */
  void RequestFadeIn( void );

  /**
   *  @brief  フェード処理待ち
   */
  bool IsEndFade( void );

  /**
   *  @brief  生成
   */
  void createPhotoBuffer( void );

  /**
   *  @brief  破棄
   */
  void destroyPhotoBuffer( void );

  /**
   *  @brief  キャラクタの表示操作
   */
  void setVisibleCharacter( bool flag );

  /**
   *  @brief  チュートリアル二回目対策
   */
  void setTutorialContinue( GameSys::GameManager *pGameManager );

private:
  gfl2::heap::HeapBase                  *m_pHeap;
  gfl2::heap::HeapBase                  *m_pWorkHeap;     //!< PokeModel生成Work用ヒープ
  gfl2::ro::Module                      *m_pFinderUIModule;
  Field::Fieldmap                       *m_pFieldmap;
  Field::Finder::Manager                *m_pManager;
  FINDER_RESULT                         *m_pOutRes;     // 試練用結果報告

  App::FinderMenu::APP_PARAM            m_rFinderMenuParam;
  Location                              m_playerLocation;

  u32                                   m_nSpotID;
  s32                                   m_nSubjectID;
  s32                                   m_TrialMonsNo;  // 試練用ポケモン指定
  u32                                   m_OldTotalValue;
  u32                                   m_FadeDelayFrame;
  bool                                  m_bKeepDisp[Field::MoveModel::FIELD_MOVE_MODEL_MAX];  // MoveModelの表示状態
  bool                                  m_bNeedMenu;
  bool                                  m_bFirst;

#if PM_DEBUG
  Debug::DebugPokeFinder                *m_pDebug;
#endif
}; // class EventPokeFinderForPlacement

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EVENT_POKEFINDER_FOR_PLACEMENT_H_INCLUDED__
