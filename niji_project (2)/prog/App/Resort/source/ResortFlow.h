#if !defined( __RESORT_FLOW_H__ )
#define __RESORT_FLOW_H__
#pragma once
//======================================================================
/**
 * @file    ResortFlow.h
 * @date    2015/10/08 17:33:57
 * @author  fukushima_yuya
 * @brief   ポケリゾート：画面フロー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/Resort/source/lower/ResortLowerUIDraw.h>
#include <App/Resort/source/lower/ResortLowerSelectDraw.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )
class UpperHomeMenuDraw;
class UpperDraw;
class LowerSelectDraw;
class LowerGimDraw;
class LowerFarmListDraw;
class LowerFarmDraw;
class LowerCourseSelectDraw;
class LowerBeansSelectDraw;
class LowerAdventureResultDraw;
class ResortSequenceListener;
class ResortFlowListener;
class DrawerBase;
class TalkMessage;
class ResortInitLogic;
class ResortChangeIslandListener;
GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( Savedata )
class ResortSave;
class BoxPokemon;
class MyStatus;
class MyItem;
GFL_NAMESPACE_END( Savedata )

GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )
class CoreParam;
GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Fade )
class FadeManager;
GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//------------------------------------------------------------------------------
// @brief   画面表示の管理クラス
//------------------------------------------------------------------------------
class ResortFlow
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortFlow);

public:
  enum FlowType {
    FLOW_TYPE_NONE,
    FLOW_TYPE_FIRST_EVENT,        //!< 起動時イベント
    FLOW_TYPE_HOME_MENU,          //!< 管理人画面フロー
    FLOW_TYPE_MAME_BOX_FUREAI,    //!< ポケマメ選択(ふれあいリゾート)
    FLOW_TYPE_MAME_BOX_KINOMI,    //!< ポケマメ選択(きのみリゾート)
    FLOW_TYPE_MAME_BOX_TANKEN,    //!< ポケマメ選択(たんけんリゾート)
    FLOW_TYPE_MAME_BOX_UNDOU,     //!< ポケマメ選択(うんどうリゾート)
    FLOW_TYPE_MAME_BOX_ONSEN,     //!< ポケマメ選択(おんせんリゾート)
    FLOW_TYPE_FARM_FIELD,         //!< 「はたけのようす」フロー
    FLOW_TYPE_FARM_LIST,          //!< 「きのみリスト」フロー
    FLOW_TYPE_TANKEN_SELECT,      //!< 「たんけんコース選択」フロー
    FLOW_TYPE_TANKEN_RESULT,      //!< 「たんけん結果」フロー
    FLOW_TYPE_UNDOU_STATE,        //!< 「うんどう状態」フロー
    FLOW_TYPE_UNDOU_SELECT,       //!< 「うんどうコース選択」フロー
    FLOW_TYPE_UNDOU_TAKE_BACK,    //!< 「うんどう引き取る」フロー
    FLOW_TYPE_ONSEN_POKE_SELECT,  //!< 「おんせんポケモン選択」フロー
    FLOW_TYPE_ONSEN_EGG_STATE,    //!< 「おんせんタマゴ孵化」フロー
    FLOW_TYPE_BOX_POKE_EVENT,     //!< 「BOXポケモンイベント」フロー
    FLOW_TYPE_WILD_POKE_EVENT,    //!< 「野生ポケモンイベント」フロー
    FLOW_TYPE_POKE_GET,           //!< 「図鑑後」フロー
    FLOW_TYPE_TUTORIAL_1_START,   //!< チュートリアル：ポケマメタッチ開始  // kawa_resort_modify
    FLOW_TYPE_TUTORIAL_1,         //!< チュートリアル：ポケマメタッチ
    FLOW_TYPE_TUTORIAL_2,         //!< チュートリアル：ポケマメの木タッチ
    FLOW_TYPE_TUTORIAL_3,         //!< チュートリアル：ポケマメいれ
    FLOW_TYPE_TUTORIAL_4,         //!< チュートリアル：管理人メニュー
    FLOW_TYPE_END_RESORT,         //!< リゾート終了フロー
    FLOW_TYPE_NUM,
  };

private:
  enum MsgType
  {
    MSG_TYPE_NONE,
    MSG_TYPE_PLAYER_NAME,         //!< プレイヤー名
    MSG_TYPE_POKE_NICKNAME,       //!< ポケモンニックネーム
    MSG_TYPE_POKE_MONSNAME,       //!< 種族名
    MSG_TYPE_EVENT_GIM,           //!< 運動イベント用
    MSG_TYPE_NUM,
  };

public:
  struct DrawList
  {
    UpperHomeMenuDraw*          homeMenu;
    UpperDraw*                  upperMenu;
    LowerUIDraw*                lowerUI;
    LowerSelectDraw*            lowerSelect;
    LowerGimDraw*               lowerGim;
    LowerFarmListDraw*          lowerFarmList;
    LowerFarmDraw*              lowerFarm;
    LowerCourseSelectDraw*      lowerCourseSel;
    LowerBeansSelectDraw*       lowerBeansSel;
    LowerAdventureResultDraw*   lowerAdvRes;
    ResortSequenceListener*     seqListener;
    ResortFlowListener*         flowListener;
    ResortChangeIslandListener* islandListener;

    DrawList( void )
    {
      homeMenu        = 0;
      upperMenu       = 0;
      lowerUI         = 0;
      lowerSelect     = 0;
      lowerGim        = 0;
      lowerFarmList   = 0;
      lowerFarm       = 0;
      lowerCourseSel  = 0;
      lowerBeansSel   = 0;
      lowerAdvRes     = 0;
      seqListener     = 0;
      flowListener    = 0;
      islandListener  = 0;
    }
  };

  //! メニューの表示に必要なパラメータ
  struct MenuParam
  {
    u8  resortID;      //!< リゾートID
    u32 pokemonIndex;                           //!< 共用ポケモンインデックス
    pml::pokepara::CoreParam* pokeCoreParam;    //!< 共用ポケモンデータ

    MenuParam( void )
      : resortID( 0 )
      , pokeCoreParam( 0 )
    { ; }
  };

private:
  //! 会話メッセージパラメータ
  struct TalkParam
  {
    u32     msgID;
    MsgType msgType;
    gfl2::str::StrWin::FinishType finType;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  ResortFlow( DrawList& drawList, app::util::Heap* heap, print::WordSet* wordSet );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~ResortFlow( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   メニューフローの開始
   *
   * @param   flowType    フローの種類
   */
  //------------------------------------------------------------------
  void StartMenuFlow( FlowType flowType );

private:
  //------------------------------------------------------------------
  /**
   * @brief   フロー開始に伴うセットアップ
   *
   * @param   flowType    フローの種類
   */
  //------------------------------------------------------------------
  void SetupMenuFlow( FlowType flowType );

  //------------------------------------------------------------------
  /**
   * @brief   フロー開始に伴うセットアップ解除
   */
  //------------------------------------------------------------------
  void UnsetupMenuFlow( void );

  //------------------------------------------------------------------
  /**
   * @brief   管理人メニューの表示
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisible_HomeMenu( bool visible );

  //------------------------------------------------------------------
  /**
   * @brief  上画面の表示
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisible_UpperDraw( bool visible );

public:
  //------------------------------------------------------------------
  /**
   * @brief   メニューフローの更新
   */
  //------------------------------------------------------------------
  void UpdateMenuFlow( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メニューフローの更新
   */
  //------------------------------------------------------------------
  void UpdateMenuFlow_None( void ) { ; }            //!< 
  void UpdateMenuFlow_FirstEvent( void );           //!< 起動時イベント
  void UpdateMenuFlow_HomeMenu( void );             //!< 管理人画面
  void UpdateMenuFlow_SelectBeans( void );          //!< ポケマメ選択
  void UpdateMenuFlow_FarmField( void );            //!< はたけのようす
  void UpdateMenuFlow_FarmList( void );             //!< きのみリスト
  void UpdateMenuFlow_AdvSelect( void );            //!< たんけんコース選択
  void UpdateMenuFlow_AdvResult( void );            //!< たんけん結果
  void UpdateMenuFlow_GimState( void );             //!< うんどう状態
  void UpdateMenuFlow_GimSelect( void );            //!< うんどうコース選択
  void UpdateMenuFlow_GimTakeBack( void );          //!< うんどう引き取る
  void UpdateMenuFlow_HotspaSelect( void );         //!< おんせんポケモン選択
  void UpdateMenuFlow_HotspaEgg( void );            //!< おんせんタマゴ孵化
  void UpdateMenuFlow_PokeEventBox( void );         //!< ポケモンイベントBOX
  void UpdateMenuFlow_PokeEventWild( void );        //!< ポケモンイベント野生
  void UpdateMenuFlow_PokeEventGet( void );         //!< 図鑑後
  void UpdateMenuFlow_Tutorial1_Start( void );      //!< チュートリアル：ポケマメタッチ開始  // kawa_resort_modify
  void UpdateMenuFlow_Tutorial1( void );            //!< チュートリアル：ポケマメタッチ
  void UpdateMenuFlow_Tutorial2( void );            //!< チュートリアル：ポケマメの木タッチ
  void UpdateMenuFlow_Tutorial3( void );            //!< チュートリアル：ポケマメいれ
  void UpdateMenuFlow_Tutorial4( void );            //!< チュートリアル：管理人メニュー
  void UpdateMenuFlow_EndResort( void );            //!< リゾートの終了

private:
  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンスの設定
   *
   * @param   seq   シーケンスID
   */
  //------------------------------------------------------------------
  void SetMainSeq( u8 seq ) { m_MainSeq = seq; }

  //------------------------------------------------------------------
  /**
   * @brief   サブシーケンスの設定
   *
   * @param   seq   シーケンスID
   */
  //------------------------------------------------------------------
  void SetSubSeq( u8 seq ) { m_SubSeq = seq; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンイベントの終了
   */
  //------------------------------------------------------------------
  void EndPokeEvent( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンイベントの終了チェック
   *
   * @return  "true  == 終了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsEndPokeEvent( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リゾートの進行度の設定
   *
   * @param   progressID    進行度
   */
  //------------------------------------------------------------------
  void SetResortProgress( u8 progressID );

  //------------------------------------------------------------------
  /**
   * @brief   リゾートの進行度チェック
   *
   * @param   progressID    進行度
   *
   * @return  "true  == 終了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsResortProgress( u8 progressID ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   フェードイン
   *
   * @param   frame   フェードフレーム
   */
  //------------------------------------------------------------------
  void FadeIn( u32 frame = DEVELOP_FADE_FRAME );

  //------------------------------------------------------------------
  /**
   * @brief   フェードアウト
   *
   * @param   frame   フェードフレーム
   */
  //------------------------------------------------------------------
  void FadeOut( u32 frame = DEVELOP_FADE_FRAME );

  //------------------------------------------------------------------
  /**
   * @brief   フェードの終了判定
   *
   * @return  "true  == 終了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsFadeEnd( void );


//==============================================================================
// @brief   メニュー(下)関連
//==============================================================================
private:
  //------------------------------------------------------------------
  /**
   * @brief   下画面UIの表示を変更
   *
   * @param   type    表示タイプ
   */
  //------------------------------------------------------------------
  void SetLowerUIType( LowerUIDraw::DispType type );

private:
  enum LowerMenu {
    LOW_MENU_FARM,            //!< はたけのようす
    LOW_MENU_FARM_LIST,       //!< きのみリスト
    LOW_MENU_ADV_RESULT,      //!< たんけん結果
    LOW_MENU_GIM,             //!< うんどう
    LOW_MENU_SELECT,          //!< ボタン選択
    LOW_MENU_COURSE,          //!< コース選択
    LOW_MENU_BEANS_SELECT,    //!< ポケマメ選択
    LOW_MENU_UI,              //!< UI
    LOW_MENU_NUM,
  };
private:
  //------------------------------------------------------------------
  /**
   * @brief   メニュー(下)を表示
   *
   * @param   menuID    メニューID
   */
  //------------------------------------------------------------------
  void OpenLowerMenu( LowerMenu menuID );

  //------------------------------------------------------------------
  /**
   * @brief   メニュー(下)を表示
   */
  //------------------------------------------------------------------
  void OpenLowerMenu_Farm( void );          //!< はたけのようす
  void OpenLowerMenu_FarmList( void );      //!< きのみリスト
  void OpenLowerMenu_AdvResult( void );     //!< たんけん結果
  void OpenLowerMenu_Gim( void );           //!< うんどう
  void OpenLowerMenu_Select( void );        //!< ボタン選択
  void OpenLowerMenu_Course( void );        //!< コース選択
  void OpenLowerMenu_BeansSelect( void );   //!< ポケマメ選択
  void OpenLowerMenu_UI( void ) { ; }       //!< UI

private:
  //------------------------------------------------------------------
  /**
   * @brief   メニュー(下)を非表示
   *
   * @param   menuID    メニューID
   */
  //------------------------------------------------------------------
  void CloseLowerMenu( LowerMenu menuID );

  //------------------------------------------------------------------
  /**
   * @brief   メニュー(下)を非表示
   */
  //------------------------------------------------------------------
  void CloseLowerMenu_Farm( void );         //!< はたけのようす
  void CloseLowerMenu_FarmList( void );     //!< きのみリスト
  void CloseLowerMenu_AdvResult( void );    //!< たんけん結果
  void CloseLowerMenu_Gim( void );          //!< うんどう
  void CloseLowerMenu_Select( void );       //!< ボタン選択
  void CloseLowerMenu_Course( void );       //!< コース選択
  void CloseLowerMenu_BeansSelect( void );  //!< ポケマメ選択
  void CloseLowerMenu_UI( void ) { ; }      //!< UI

private:
  //------------------------------------------------------------------
  /**
   * @brief   メニュー(下)のリザルトを取得
   *
   * @param   menuID    メニューID
   */
  //------------------------------------------------------------------
  u8 GetLowerMenuResult( LowerMenu menuID );

  //------------------------------------------------------------------
  /**
   * @brief   メニュー(下)のリザルトを取得
   */
  //------------------------------------------------------------------
  u8 GetLowerMenuResult_Farm( void );           //!< はたけのようす
  u8 GetLowerMenuResult_FarmList( void );       //!< きのみリスト
  u8 GetLowerMenuResult_AdvResult( void );      //!< たんけん結果
  u8 GetLowerMenuResult_Gim( void );            //!< うんどう
  u8 GetLowerMenuResult_Select( void );         //!< ボタン選択
  u8 GetLowerMenuResult_Course( void );         //!< コース選択
  u8 GetLowerMenuResult_BeansSelect( void );    //!< ポケマメ選択
  u8 GetLowerMenuResult_UI( void );             //!< UI

private:
  enum {
    GIM_DECIDE_BTN_DISABLE,
    GIM_DECIDE_BTN_ENABLE,
    GIM_DECIDE_BTN_NUM,
  };

private:
  //------------------------------------------------------------------
  /**
   * @brief   うんどうの表示タイプを設定
   *
   * @param   enable    決定ボタンの表示フラグ
   */
  //------------------------------------------------------------------
  void SetDispType_Gim( u8 enable );

  //------------------------------------------------------------------
  /**
   * @brief   選択ボタンの表示タイプを設定
   *
   * @param   type    表示タイプ
   */
  //------------------------------------------------------------------
  void SetDispType_Select( LowerSelectDraw::SelectType type );

  //------------------------------------------------------------------
  /**
   * @brief   コース選択の表示タイプを設定
   *
   * @param   type    表示タイプ
   */
  //------------------------------------------------------------------
  void SetDispType_Course( u8 type );

  //------------------------------------------------------------------
  /**
   * @brief   ポケマメ選択の表示タイプを設定
   *
   * @param   type    表示タイプ
   */
  //------------------------------------------------------------------
  void SetDispType_BeansSelect( u8 type );

private:
  enum {
    CALL_RESORT_END,
    CALL_APP_BOX_GIM,
    CALL_APP_BOX_GIM_RE,
    CALL_APP_BOX_HOTSPA,
    CALL_APP_EGG_DEMO,
    CALL_APP_STATUS,
    CALL_APP_ZUKAN,
    CALL_APP_NUM,
  };

private:
  //------------------------------------------------------------------
  /**
   * @brief   アプリケーションのコール
   *
   * @param   callID    コールID
   */
  //------------------------------------------------------------------
  void CallApplication( u8 callID );

private:
  //------------------------------------------------------------------
  /**
   * @brief   インデックスの取得：うんどうグループ
   *
   * @param   グループインデックス
   */
  //------------------------------------------------------------------
  u8 GetIndex_GimGroup( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンが配置されているかどうかの取得
   *
   * @param   resortID    リゾートID
   *
   * @return  "true  == 配置あり"
   * @return  "false == 配置なし"
   */
  //------------------------------------------------------------------
  bool IsValidPokemon( u8 resortID );




//==============================================================================
// @brief   メッセージ関連
//==============================================================================
private:
  //------------------------------------------------------------------
  /**
   * @brief   会話の開始
   *
   * @param   msgID     メッセージID
   * @param   msgType   メッセージタイプ
   * @param   finType   終了タイプ
   */
  //------------------------------------------------------------------
  void TalkStart( u32 msgID, MsgType msgType = MSG_TYPE_NONE, gfl2::str::StrWin::FinishType finType = gfl2::str::StrWin::FINISH_USER );

  //------------------------------------------------------------------
  /**
   * @brief   会話の開始
   *
   * @param   param   会話パラメータ
   */
  //------------------------------------------------------------------
  void TalkStart( TalkParam param );

  //------------------------------------------------------------------
  /**
   * @brief   システムメッセージの開始
   *
   * @param   msgID     メッセージID
   */
  //------------------------------------------------------------------
  void SystemMsgStart( u32 msgID );

  //------------------------------------------------------------------
  /**
   * @brief   会話の終了確認
   *
   * @return  "true  == 終了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsTalkEnd( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   連続会話メッセージの開始
   *
   * @param   paramTable  会話パラメータ配列
   * @param   tableMax    メッセージ配列数
   */
  //------------------------------------------------------------------
  void TalkStart_Loop( const TalkParam* paramTable, u32 tableMax );

  //------------------------------------------------------------------
  /**
   * @brief   連続会話メッセージが終了したかどうか
   *
   * @return  "true  == 終了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsTalkEnd_Loop( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージタイプの設定
   *
   * @param   type    メッセージタイプ
   */
  //------------------------------------------------------------------
  void SetMessageType( MsgType type );


private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：プラスボーナス
   *
   * @param   bonus   ボーナス値
   */
  //------------------------------------------------------------------
  void SetMessage_PlusBonus( u8 bonus );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：マイナスボーナス
   *
   * @param   bonus   ボーナス値
   */
  //------------------------------------------------------------------
  void SetMessage_MinusBonus( u8 bonus );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：開発完了
   */
  //------------------------------------------------------------------
  void SetMessage_ResortDeveloped( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：リゾートの説明
   */
  //------------------------------------------------------------------
  void SetMessage_ResortDescription( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：ポケモンがいない
   *
   * @param   type    ポケマメいれのタイプ
   */
  //------------------------------------------------------------------
  void SetMessage_BeansSelectPokeNone( u8 type );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：ポケマメ入れ後
   *
   * @param   type    ポケマメいれのタイプ
   */
  //------------------------------------------------------------------
  void SetMessage_BeansSelectAfter( u8 type );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：ポケマメ入れ後(システム)
   *
   * @param   type    ポケマメいれのタイプ
   */
  //------------------------------------------------------------------
  void SetMessage_BeansSelectSystem( u8 type );


private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：おんせんタマゴ状態
   */
  //------------------------------------------------------------------
  void SetMessage_EggState( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：ポケモンイベント(BOX)性格
   */
  //------------------------------------------------------------------
  void SetMessage_EventSeikaku( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：ポケモンイベント(BOX)いちゃいちゃ
   *
   * @return  "true  == 一致"
   * @return  "false == 不一致"
   */
  //------------------------------------------------------------------
  bool SetMessage_EventFriendship( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：ポケモンイベント(BOX)通常
   */
  //------------------------------------------------------------------
  void SetMessage_EventNormal( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：ポケモンイベント(BOX)ポケマメ取得
   */
  //------------------------------------------------------------------
  void SetMessage_GetPokemame( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定：ポケモンイベント(BOX)きのみリゾート専用
   */
  //------------------------------------------------------------------
  void SetMessage_FarmResort( void );



//==============================================================================
// @brief   イベント関連
//==============================================================================
private: //!< FirstEvent
  //------------------------------------------------------------------
  /**
   * @brief   起動時イベント：開始分岐
   */
  //------------------------------------------------------------------
  void FirstEvent_Begin( void );

  //------------------------------------------------------------------
  /**
   * @brief   起動時イベント：ボーナス分岐
   */
  //------------------------------------------------------------------
  void FirstEvent_Bonus( void );

  //------------------------------------------------------------------
  /**
   * @brief   起動時イベント：開発分岐
   */
  //------------------------------------------------------------------
  void FirstEvent_Develop( void );

  //------------------------------------------------------------------
  /**
   * @brief   開発可能なリゾートIDの取得
   *
   * @return  開発可能なリゾートID
   * @return  開発不可の場合、RESORT_ID_MAXを返す
   */
  //------------------------------------------------------------------
  u8 GetDevelopResort( void );


private:  //!< HomeMenu
  //------------------------------------------------------------------
  /**
   * @brief   管理人イベント：開始分岐
   */
  //------------------------------------------------------------------
  void HomeMenuEvent_Begin( void );

  //------------------------------------------------------------------
  /**
   * @brief   管理人イベント：メニュー選択
   */
  //------------------------------------------------------------------
  void HomeMenuEvent_MenuSelect( void );

  //------------------------------------------------------------------
  /**
   * @brief   管理人イベント：開発
   */
  //------------------------------------------------------------------
  void HomeMenuEvent_Develop( void );

  //------------------------------------------------------------------
  /**
   * @brief   管理人イベント：交換
   */
  //------------------------------------------------------------------
  void HomeMenuEvent_Trade( void );

  //------------------------------------------------------------------
  /**
   * @brief   管理人イベント：マメビン流し
   */
  //------------------------------------------------------------------
  void HomeMenuEvent_BeansSend( void );

  //------------------------------------------------------------------
  /**
   * @brief   すべてのリゾートの開発が完了しているかどうか
   *
   * @return  "true  == 完了"
   * @return  "false == 開発可能"
   */
  //------------------------------------------------------------------
  bool IsAllResortDeveloped( void );

  //------------------------------------------------------------------
  /**
   * @brief   交換に必要なポケマメを所持しているか？
   *
   * @return  "true  = 所持"
   * @return  "false = 未所持"
   */
  //------------------------------------------------------------------
  bool IsHaveTradeBeans( void );


private:  //!< BeansSelect
  //------------------------------------------------------------------
  /**
   * @brief   きのみの生育を行っていないかどうか
   *
   * @return  "true  == 生育を行っていない"
   * @return  "false == 生育を行っている"
   */
  //------------------------------------------------------------------
  bool IsFarmPlantNone( void );

  //------------------------------------------------------------------
  /**
   * @brief   うんどうをおこなっているかどうか
   *
   * @return  "true  == うんどうしていない"
   * @return  "false == うんどうしている"
   */
  //------------------------------------------------------------------
  bool IsGimTrainingNone( void );


private:  //!< Farm
  //------------------------------------------------------------------
  /**
   * @brief   きのみリゾート：はたけのようす
   */
  //------------------------------------------------------------------
  void FarmEvent_FieldState( void );

  //------------------------------------------------------------------
  /**
   * @brief   きのみリゾート：きのみリスト
   */
  //------------------------------------------------------------------
  void FarmEvent_KinomiList( void );

  //------------------------------------------------------------------
  /**
   * @brief   はたけが全て埋まっているか？
   *
   * @return  "true  == 埋まっている"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsFarmNotSpace( void );

  //------------------------------------------------------------------
  /**
   * @brief   きのみリストの準備が完了しているか？
   *
   * @return  "true  == 完了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsReadyFarmList( void );


private:  //!< Adventure Select
  //------------------------------------------------------------------
  /**
   * @brief   たんけんコース選択：開始分岐
   */
  //------------------------------------------------------------------
  void AdventureSelectEvent_Begin( void );

  //------------------------------------------------------------------
  /**
   * @brief   たんけんコース選択：コース選択
   */
  //------------------------------------------------------------------
  void AdventureSelectEvent_Select( void );

  //------------------------------------------------------------------
  /**
   * @brief   たんけんコース選択：冒険中
   */
  //------------------------------------------------------------------
  void AdventureSelectEvent_Adventure( void );

  //------------------------------------------------------------------
  /**
   * @brief   たんけんコース選択：エラー表示
   */
  //------------------------------------------------------------------
  void AdventureSelectEvent_Error( void );


private:  //!< Adventure Result
  //------------------------------------------------------------------
  /**
   * @brief   たんけんリゾートの結果アイテム数の取得
   * @return  count   アイテム数
   */
  //------------------------------------------------------------------
  u32 GetAdvItemCount( void );


private:  //!< Gim State
  //------------------------------------------------------------------
  /**
   * @brief   うんどう状態：開始分岐
   */
  //------------------------------------------------------------------
  void GimStatetEvent_Begin( void );

  //------------------------------------------------------------------
  /**
   * @brief   うんどう状態：うんどう開始
   */
  //------------------------------------------------------------------
  void GimStatetEvent_Start( void );

  //------------------------------------------------------------------
  /**
   * @brief   うんどう状態：うんどう中
   */
  //------------------------------------------------------------------
  void GimStatetEvent_Training( void );

  //------------------------------------------------------------------
  /**
   * @brief   うんどう状態：うんどう終了
   */
  //------------------------------------------------------------------
  void GimStatetEvent_Finish( void );

  //------------------------------------------------------------------
  /**
   * @brief   うんどうリゾートの選択ポケモン数の取得
   *
   * @return  count   ポケモン数
   */
  //------------------------------------------------------------------
  u8 GetGimPokeCount( void );


private:  //!< Hotspa Egg
  //------------------------------------------------------------------
  /**
   * @brief   おんせんタマゴイベント：開始分岐
   */
  //------------------------------------------------------------------
  void HotspaEggEvent_Begin( void );

  //------------------------------------------------------------------
  /**
   * @brief   おんせんタマゴイベント：デモの開始
   */
  //------------------------------------------------------------------
  void HotspaEggEvent_CallDemo( void );

  //------------------------------------------------------------------
  /**
   * @brief   おんせんタマゴイベント：タマゴ状態
   */
  //------------------------------------------------------------------
  void HotspaEggEvent_State( void );

  //------------------------------------------------------------------
  /**
   * @brief   たまごのなつき度の取得(孵化までの状態を取得)
   *
   * @return  なつき度
   */
  //------------------------------------------------------------------
  u32 GetEggFamiliarity( void );


private:  //!< PokemonEvent
  //------------------------------------------------------------------
  /**
   * @brief   Boxポケモンイベント：開始分岐
   */
  //------------------------------------------------------------------
  void BoxPokemonEvent_Begin( void );

  //------------------------------------------------------------------
  /**
   * @brief   野生ポケモンイベント：開始分岐
   */
  //------------------------------------------------------------------
  void WildPokemonEvent_Begin( void );

  //------------------------------------------------------------------
  /**
   * @brief   野生ポケモンイベント：引き取りイベント
   */
  //------------------------------------------------------------------
  void WildPokemonEvent_TakeCharge( void );

  //------------------------------------------------------------------
  /**
   * @brief   もらったポケマメのセット
   */
  //------------------------------------------------------------------
  void SetPokemame( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンイベントワークの設定
   *
   * @param   work    イベントワーク
   */
  //------------------------------------------------------------------
  void SetPokemonEventWork( u8 work );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンイベントワークの取得
   *
   * @return  イベントワーク
   */
  //------------------------------------------------------------------
  u8 GetPokemonEventWork( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータのセット
   */
  //------------------------------------------------------------------
  void SetPokemonParam( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンを引き取れるか？
   *
   * @return  "true  == 引き取れる"
   * @return  "false == 引き取れない"
   */
  //------------------------------------------------------------------
  bool IsWildPokeTakeCharge( void );


private:  //!< Tutorial
  //------------------------------------------------------------------
  /**
   * @brief   チュートリアルイベント：指カーソルの表示
   *
   * @param   progID    表示する進行度
   */
  //------------------------------------------------------------------
  void VisibleFingerCursor( u8 progID );

  //------------------------------------------------------------------
  /**
  * @brief   チュートリアルイベント：指カーソルの非表示
   */
  //------------------------------------------------------------------
  void InvisibleFingerCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   チュートリアルイベント：モデルアニメの再生
   *
   * @param   animID    モデルアニメID
   */
  //------------------------------------------------------------------
  void StartModelAnim( u32 animID );

  //------------------------------------------------------------------
  /**
   * @brief   チュートリアルイベント：ふれあい上画面のセットアップ
   */
  //------------------------------------------------------------------
  void SetupUpperFriendship( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   メニューパラメータの取得
   * @return  メニューパラメータ構造体のポインタ
   */
  //------------------------------------------------------------------
  MenuParam* GetMenuParam( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンの取得：セーブデータから取得
   *
   * @param   resortID  リゾートID
   * @param   index     インデックス
   * @param   cp        pml::pokepara::CoreParam
   */
  //------------------------------------------------------------------
  void GetPokemon_FromSavedata( u8 resortID, int index, pml::pokepara::CoreParam* cp );

  //------------------------------------------------------------------
  /**
   * @brief   ボックス内のポケモンの数を取得
   *
   * @return  ボックス内のポケモン数
   */
  //------------------------------------------------------------------
  u32 GetBoxPokemonCount( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボックス内の空きの数を取得
   *
   * @return  ボックス内の空きの数
   */
  //------------------------------------------------------------------
  u32 GetBoxSpaceCount( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   Drawクラスのインスタンスの設定
   *
   * @param   drawList   Drawクラスのインスタンスをまとめた構造体
   */
  //------------------------------------------------------------------
  void SetDrawList( DrawList& drawList );

  //------------------------------------------------------------------
  /**
   * @brief   インデックスの設定：うんどうグループ
   *
   * @param   グループインデックス
   */
  //------------------------------------------------------------------
  void SetGimGroupIndex( u8 gimGroupIndex ) { m_GimGroupIndex = gimGroupIndex; }
  //------------------------------------------------------------------
  /**
   * @brief   インデックスの取得：うんどうグループ
   *
   * @param   グループインデックス
   */
  //------------------------------------------------------------------
  u8 GetGimGroupIndex( void ) const { return m_GimGroupIndex; }

  //------------------------------------------------------------------
  /**
   * @brief   会話メッセージの設定
   *
   * @param   talkMsg   会話メッセージクラス
   */
  //------------------------------------------------------------------
  void SetTalkMessage( TalkMessage* talkMsg ) { m_pTalkMessage = talkMsg; }

  //------------------------------------------------------------------
  /**
   * @brief   ロジックの設定
   *
   * @param   logic   ロジッククラス
   */
  //------------------------------------------------------------------
  void SetInitLogic( ResortInitLogic* logic ) { m_pInitLogic = logic; }


private:
  //------------------------------------------------------------------------------
  /**
   * @brief   温泉イベント開始時のメッセージIDを取得
   * @return  メッセージID
   */
  //------------------------------------------------------------------------------
  u32 GetEventStartHotSpaMsgID(void);

  //------------------------------------------------------------------------------
  /**
   * @brief   運動イベント開始時のメッセージIDを取得
   * @param   gim_index   運動のインデックス
   * @param   val         in:運動開始時の値 out:上昇値
   * @return  メッセージID
   */
  //------------------------------------------------------------------------------
  u32 GetEventStartGimMsgID( int gim_index, s32 * val );


private:
  static const u32 TUTORIAL_BEANS_ANIM_PICKUP_WAIT_FRAME = 15;  // kawa_resort_modify
  static const u32 TUTORIAL_FADE_WAIT_FRAME = 15;
  static const u32 DEVELOP_FADE_FRAME       = 8;    //!< 開発時のフェード処理のフレーム

private:
  Savedata::ResortSave*       m_pResortSave;

  Savedata::BoxPokemon*       m_pBoxPokemon;

  Savedata::MyStatus*         m_pMyStatus;

  Savedata::MyItem*           m_pMyItem;

  app::util::Heap*            m_pHeap;

  print::WordSet*             m_pWordSet;

  gfl2::Fade::FadeManager*    m_pFadeMan;
  
private:
  TalkMessage*        m_pTalkMessage;

  ResortInitLogic*    m_pInitLogic;

private:
  DrawList      m_DrawList;

  MenuParam     m_MenuParam;

  FlowType      m_StartFlowType;

  u8            m_MainSeq;

  u8            m_SubSeq;

  u32           m_WaitFrame;

  bool          m_FadeFlg;

  u8            m_PartyMemberNum;

  u8            m_GimGroupIndex;

private:
  const TalkParam*    m_pParamTable;

  u32                 m_MsgTableMax;

  u32                 m_MsgSeq;

  s32                 m_MsgValue;
};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif // __RESORT_FLOW_H__
