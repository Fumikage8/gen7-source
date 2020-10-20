//============================================================================
/**
 *
 *  @file   ScriptWork.h
 *  @brief  スクリプトワーク
 *  @author Miyuki Iwasawa
 *  @date   2015.02.28
 *
 */
//============================================================================
#ifndef __SCRIPTWORK_H__
#define __SCRIPTWORK_H__ 

//system
#include "GameSys/include/GameEvent.h"

#include "Print/include/PrintSystem.h"
#include "Print/include/WordSet.h"

#include "niji_reference_files/script/FieldPawnTypes.h"   //SCRID
#include "FieldScript/include/ScriptTypes.h"  //SCRIPT_TYPE
#include "FieldScript/include/ScriptInfo.h"
#include "FieldScript/include/ScriptAssert.h"
#include "Pawn/include/PawnObject.h"
#include "FieldScript/include/ScriptDebug.h"

//#include "savedata/EventWorkDef.h" // イベントフラグ、ワーク定義
//#include "EventModelManager.h" 
//#include "..//movemodel/FieldMoveModelBase.h" // for MoveModelBase
//#include "system/camera/CameraPetitAnime.h"
//#include "app/strinput/strinput.h"

#include "niji_conv_header/field/flagwork/WorkDefine.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandContinuousPlayback.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"

// クラス前方宣言
namespace app {
  namespace tool {
    class ListMenu;
  }
}

namespace Field{
  class IFieldActorBase;

  namespace MoveModel{
    class FieldMoveModel;
  }
  namespace FieldWindow{
    class FieldYesNoWindow;
    class FieldListMenuWindow;
    class FieldTalkWindow;
  }

}

/* ------------------------------------------------------------------------- */
/**
 * @brief フィールド
 */
/* ------------------------------------------------------------------------- */
GFL_NAMESPACE_BEGIN( Field );
/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプト
 */
/* ------------------------------------------------------------------------- */
GFL_NAMESPACE_BEGIN( FieldScript );


/* ------------------------------------------------------------------------- */
/**
 * @brief コマンド運用エラーチェックBit定義
 * EventScriptCall.cpp の sc_CheckEndFuncTblと一致させる
 */
/* ------------------------------------------------------------------------- */
enum SCR_END_CHECK_BIT
{ 
  SCR_END_CHECK_BIT_NONE                = 0,
  SCR_END_CHECK_BIT_WIN_OPEN            = 1 << 0,
  SCR_END_CHECK_BIT_EVENT_CAMERA        = 1 << 1,
  SCR_END_CHECK_BIT_BG_PARTS            = 1 << 2,
  SCR_END_CHECK_BIT_MAPBLOCK_CALC_POS   = 1 << 3,
  SCR_END_CHECK_BIT_MOTION_BLUR         = 1 << 4,
  SCR_END_CHECK_BIT_HIDE_MESH_SYSTEM    = 1 << 5,
  SCR_END_CHECK_BIT_MAX                 = 1 << 6,
};

//------------------------------------------------------------------------------
/**
 * @class       MotionCommandParam
 * @brief       モーションコマンド用ワーク.
 * @author      N.Takeda
 * @date        2016/03/02(水) 15:45:03
 */
//------------------------------------------------------------------------------
class MotionCommandParam
{
private:
  u32 m_AnimationIDArray[ MoveModel::ActionCommandContinuousPlayback::REGIST_MAX_CNT ];
  s16 m_FrameStartArray[ MoveModel::ActionCommandContinuousPlayback::REGIST_MAX_CNT ];
  s16 m_LoopMaxArray[ MoveModel::ActionCommandContinuousPlayback::REGIST_MAX_CNT ];
  s16 m_BlendArray[ MoveModel::ActionCommandContinuousPlayback::REGIST_MAX_CNT ];
  u32 m_Num; //積み込まれた数

public:
  enum{ NO_INITLIZE = -1 };
  u32 GetEntryNum() const { return m_Num; }
  void Entry( int anime_id, int frame_start, int loop_max, int blend );
  void Clear();
  u32* GetAnimationIDArrayPtr(){  return m_AnimationIDArray;  }
  s16* Getm_FrameStartArrayPtr(){  return m_FrameStartArray;  }
  s16* Get_LoopMaxArrayPtr(){  return m_LoopMaxArray;  }
  s16* GetBlendArrayPtr(){  return m_BlendArray;  }
  
  MotionCommandParam()
   : m_Num( (static_cast<u32>(NO_INITLIZE)) )
  {}
};


//--------------------------------------------------------------
/// script::Work スクリプトワーク
//==============================================================
class Work
{ 
  GFL_FORBID_COPY_AND_ASSIGN( Work ); ///< コピーコンストラクタ + 代入禁止
public:
  /// ------- getters ---------
  MotionCommandParam  *GetMotionCommandParamPtr(){ return &m_cMotionCommandParam; }

  /// トレーナー視線定義
  enum TRAINER_EYE_HIT
  {
    TRAINER_EYE_HIT0,   ///< トレーナー一人目
    TRAINER_EYE_HIT1,   ///< トレーナー二人目
    TRAINER_EYE_HITMAX, ///< トレーナー視線データ最大数
  };

  /// ターゲットオブジェクトの無効値
  enum{
    TARGET_OBJID_NONE = 0xFFFFFFFF,
    TARGET_ACTOR_ID_NONE = 0,
  };
  
  /// トレーナー視線データ
  struct TRAINER_HITDATA
  {
    u32 event_id;       ///< イベントID
    u32 tr_id;          ///< トレーナーID
    u16 move_type;    ///< 視線動作タイプ SCR_EYE_TR_TYPE_SINGLE など
    SCRIPTID scr_id;  ///< スクリプトID
  };

  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   *
   * @param p_gman ゲームマネージャへのポインタ
   * @param p_heap ヒープへのポインタ
   * @param acmdlist_max アクションコマンド最大数
   */
  /* ------------------------------------------------------------------------- */
  Work( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* p_heap, u16 acmdlist_max = 128 );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ~Work();
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief イベントをセット
   *
   * @param event イベントへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  void SetBaseEvent( GameSys::GameEvent* event ) { m_pBaseEvent = event; }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief イベントを取得
   *
   * @return イベントへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  GameSys::GameEvent* GetBaseEvent( void ) const { return m_pBaseEvent; }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief シーケンスへのポインタを取得
   *
   * @return シーケンスへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  u16* GetSeq( void ) { return &m_Seq; }
 
  /* ------------------------------------------------------------------------- */
  /*
   * @brief 話しかけ対象のモデル/アクターを取得
   *
   * @param p_mdl 動作モデル
   */
  /* ------------------------------------------------------------------------- */
  MoveModel::FieldMoveModel* GetTargetModel( void ) const { return m_pTalkModel; }
  u32 GetTargetActorID( void ) const { return m_TargetActorID; }
  FieldActorType GetTargetActorType( void ) const { return m_TargetActorType; }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 対象MoveModel/Actorを設定
   *
   * @param p_mdl 動作モデル
   */
  /* ------------------------------------------------------------------------- */
  void SetTargetModel( MoveModel::FieldMoveModel* p_mdl );
  void SetTargetActor( u32 eventID, FieldActorType type );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  ワークを取得(イベントワーク/一時記憶ワーク両対応)
   *
   *  @param  u16 work_no  ワークナンバー
   *
   *  @retval ワークのアドレス
   */
  //-----------------------------------------------------------------------------
  u32 GetWorkValue( u16 work_no );

  /**
   *  @brief  ワークをセット(イベントワーク/一時記憶ワーク両対応)
   *
   *  @param  u16 work_no  ワークナンバー
   *  @param  value
   */
  //-----------------------------------------------------------------------------
  bool SetWorkValue( u16 work_no, u32 value );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  イベントワークを取得
   *
   *  @param  u16 work_no  ワークナンバー
   *
   *  @retval ワークのアドレス
   *
   *  @li work_no < 0x8000  通常のセーブワーク
   *  @li work_no >= 0x8000 スクリプト制御ワークの中に確保しているワーク
   */
  //-----------------------------------------------------------------------------
  u16* GetEventWork( u16 work_no );
  //-----------------------------------------------------------------------------
  /**
   *  @brief  イベントワークの値を取得
   *
   *  @param  u16 work_no  ワークナンバー
   *
   *  @retval ワークの値
   */
  //-----------------------------------------------------------------------------
  u16  GetEventWorkValue( u16 work_no );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief イベントワークの値をセット
   *
   * @param work_no ワークナンバー
   * @param value セットする値
   *
   * @return true:セットできた, false:できなかった
   */
  /* ------------------------------------------------------------------------- */
  bool SetEventWorkValue( u16 work_no, u32 value );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief  イベントフラグを取得
   *
   *  @param  u16 flag_no  フラグナンバー
   *  
   *  @retval フラグ値
   */
  //-----------------------------------------------------------------------------
  bool GetEventFlag( u16 flag_no ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  イベントフラグを設定
   *
   *  @param  u16 flag_no  フラグナンバー
   *  @param  val フラグ値
   */
  //-----------------------------------------------------------------------------
  void SetEventFlag( u16 flag_no, bool val );
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief  一時記憶ワークの値を取得
   *
   *  @param  u16 work_no 一時ワークNO
   *
   *  @return ワークの値
   */
  //-----------------------------------------------------------------------------
  u32 GetTempWorkValue( u16 work_no );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief  一時記憶ワークの値をセット
   *
   *  @param  u16 work_no 一時ワークNO
   *  @param  u32 value ワークにセットする値
   */
  //-----------------------------------------------------------------------------
  bool SetTempWorkValue( u16 work_no, u32 value );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  話しかけターゲット（SCWK_TARGET_OBJID）の存在チェック
   *
   *  @retval true：話しかけターゲットが存在しない
   *  @retval false：話しかけターゲットが存在する
   */
  //-----------------------------------------------------------------------------
  bool IsTargetObjidNone();

  //-----------------------------------------------------------------------------
  /**
   *  @brief  イベントポジション（イベント相対座標システム基準点）セット
   */
  //-----------------------------------------------------------------------------
  void SetEventPosition( const gfl2::math::Vector3& pos){ GFL_ASSERT(!m_bEventPosEnable); m_EventPos = pos; m_bEventPosEnable = true; }
  void SetEventPosition( float x, float y, float z){ GFL_ASSERT(!m_bEventPosEnable); m_EventPos.Set(x,y,z); m_bEventPosEnable = true; }
  void ResetEventPosition(){ m_EventPos.Set(0,0,0);  m_bEventPosEnable = false; }
  gfl2::math::Vector3 GetEventPosition(){ return m_EventPos; }
  bool CheckEventPositionEnable(){ return m_bEventPosEnable; }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief メッセージARCIDを取得
   *
   * @return メッセージARCID
   */
  /* ------------------------------------------------------------------------- */
  u32 GetMsgArcId( void ) const { return ( (m_ExtraMsgInstantArcID>=0) ? static_cast<u32>(m_ExtraMsgInstantArcID):scrInfo.m_MsgArcId); }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief メッセージARCIDを設定
   *
   * @param arc_id メッセージARCID
   */
  /* ------------------------------------------------------------------------- */
  void SetMsgArcId( u32 arc_id ){ scrInfo.m_MsgArcId = arc_id; }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief メッセージDATIDを取得
   *
   * @return メッセージDATID
   */
  /* ------------------------------------------------------------------------- */
  u32 GetMsgDatId( void ) const { return ( (m_ExtraMsgInstantDatID>=0) ? static_cast<u32>(m_ExtraMsgInstantDatID):scrInfo.m_MsgDatId); }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief メッセージDATIDを設定
   *
   * @param dat_id メッセージDATID
   */
  /* ------------------------------------------------------------------------- */
  void SetMsgDatId( u32 dat_id ){ scrInfo.m_MsgDatId = dat_id; }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief インスタントExtraMsg
   */
  /* ------------------------------------------------------------------------- */
  void ClearExtraMsgInstant( void ) { m_ExtraMsgInstantArcID = m_ExtraMsgInstantDatID = -1; }
  void SetExtraMsgInstant( s32 arcID, s32 datID ) { m_ExtraMsgInstantArcID = arcID; m_ExtraMsgInstantDatID = datID; }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプト情報を取得
   *
   * @return スクリプト情報クラス
   */
  /* ------------------------------------------------------------------------- */
  ScriptInfo * GetScriptInfo( void ) { return &scrInfo; }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプト情報を設定
   *
   * @param info スクリプト情報クラス
   */
  /* ------------------------------------------------------------------------- */
  void SetScriptInfo( const ScriptInfo * info ) { scrInfo = *info; }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief パラメータを設定
   *
   * @param p0 パラメータ0
   * @param p1 パラメータ1
   * @param p2 パラメータ2
   * @param p3 パラメータ3
   */
  /* ------------------------------------------------------------------------- */
  void SetParameters( u32 p0, u32 p1, u32 p2, u32 p3 );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief BGIDを設定
   *
   * @param bgid BGID
   */
  /* ------------------------------------------------------------------------- */
  void SetBGID( u32 bgid );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief POSIDを設定
   *
   * @param posid POSID
   */
  /* ------------------------------------------------------------------------- */
  void SetPosID( u32 posid );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief POSIDを取得
   *
   * @return POSID
   */
  /* ------------------------------------------------------------------------- */
  u32 GetPosID( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 子グローバルパラメータを取得
   *
   * @param index インデックス
   *
   * @return 子グローバルパラメータ
   *
   * @note グローバルコール時に子に渡すパラメータ
   */
  /* ------------------------------------------------------------------------- */
  cell GetChildGlobalParam( u32 index );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 子グローバルパラメータを設定
   *
   * @param index インデックス
   * @param value 値
   *
   * @note グローバルコール時に子に渡すパラメータ
   */
  /* ------------------------------------------------------------------------- */
  void SetChildGlobalParam( u32 index, cell value );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 親グローバルパラメータを取得
   *
   * @param index インデックス
   *
   * @return 親グローバルパラメータ
   *
   * @note グローバルコール時に親に渡すパラメータ
   */
  /* ------------------------------------------------------------------------- */
  cell GetParentGlobalParam( u32 index );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 親グローバルパラメータを設定
   *
   * @param index インデックス
   * @param value 値
   *
   * @note グローバルコール時に親に渡すパラメータ
   */
  /* ------------------------------------------------------------------------- */
  void SetParentGlobalParam( u32 index, cell value );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 親のワークの子グローバルパラメータに親グローバルパラメータをコピー
   *
   * @param parent 親のワーク
   */
  /* ------------------------------------------------------------------------- */
  void CopyGlobalParamsToParent( Work * parent );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 子ワークの親グローバルパラメータに子グローバルパラメータをコピー
   *
   * @param child 子のワーク
   */
  /* ------------------------------------------------------------------------- */
  void CopyGlobalParamsToChild( Work * child );

  void CreateFreeWord( void );
  void DeleteFreeWord( void );
  gfl2::str::StrBuf * GetFreeWord( void ) const { return m_pInputFreeWord; }



  /* ------------------------------------------------------------------------- */
  /**
   * @brief トレーナー視線データを取得
   *
   * @param hit_no 視線NO
   *
   * @return トレーナー視線データ
   */
  /* ------------------------------------------------------------------------- */
  TRAINER_HITDATA* GetTrainerHitData( TRAINER_EYE_HIT hit_no ) { 
    GFL_ASSERT( hit_no < TRAINER_EYE_HITMAX );
    return &m_TrainerHitData[ hit_no ];
  }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief トレーナー視線データを取得(const)
   *
   * @param hit_no 視線NO
   *
   * @return トレーナー視線データ
   */
  /* ------------------------------------------------------------------------- */
  const TRAINER_HITDATA* GetTrainerHitDataConst( TRAINER_EYE_HIT hit_no ) const { 
    GFL_ASSERT( hit_no < TRAINER_EYE_HITMAX );
    return &m_TrainerHitData[ hit_no ];
  }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief フェードフラグを設定
   *
   * @param is_fade フェードフラグ
   */
  /* ------------------------------------------------------------------------- */
  void SetFadeFlag( bool is_fade ) { m_bFadeFlag = is_fade; }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief フェード有効判定
   *
   * @return true:フェード有効, false:フェード無効
   */
  /* ------------------------------------------------------------------------- */
  bool IsFadeEnable( void ) const { return m_bFadeFlag; }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 地名表示を閉じないフラグを設定
   *
   * @param flag フラグ
   */
  /* ------------------------------------------------------------------------- */
  void SetPlaceNameNotCloseFlag( bool flag ) { m_bPlaceNameNotCloseFlag = flag; }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 地名表示を閉じないフラグを取得
   *
   * @return true:閉じない, false:通常通り閉じる
   */
  /* ------------------------------------------------------------------------- */
  bool IsPlaceNameNotClose( void ) const { return m_bPlaceNameNotCloseFlag; }



#if PM_DEBUG
  /* ------------------------------------------------------------------------- */
  /**
   * @brief キャラリスト登録
   * @param p_name
   * @param event_id
   * @param dynamic_mot_id  //非常駐モーションID -1なら登録無し
   */
  /* ------------------------------------------------------------------------- */
  int DebugChrEditListEntry( const char* p_name, u32 event_id, int dynamic_mot_id );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief キャラリスト取得
   */
  /* ------------------------------------------------------------------------- */
  const DebugChrEditList* DebugChrEditListGet(){ return m_DebugChrEditList; }
  int DebugChrEditListNumGet(){ return m_DebugChrEditListNum; }

#endif

private:

  /// 定数
  enum
  {
    MAGIC_NO = 0x3643f, ///< マジックナンバー(現在未使用)
    MSG_BUF_SIZE = 1024,  ///< メッセージバッファのサイズ
    GLOBAL_PARAM_NUM = 6, ///< グローバルパラメータの数

    TEMP_WORK_START = SCWK_AREA_START,  ///< 一時ワーク開始値
    TEMP_WORK_END = USERWK_AREA_END,    ///< 一時ワーク終了値
    TEMP_WORK_SIZE = TEMP_WORK_END - TEMP_WORK_START, ///< 一時ワーク数

    USERWK_SIZE = USERWK_AREA_END - USERWK_AREA_START,  ///< ユーザーワーク数
  };
  
  u32 m_MagicNo;  ///< マジックナンバー
  u16 m_Seq;  ///< シーケンス
  GameSys::GameManager*  m_pGameManager; ///< ゲームマネージャへのポインタ
  GameSys::GameEvent* m_pBaseEvent;  ///< 呼び出し元のイベントへのポインタ

  MoveModel::FieldMoveModel* m_pTalkModel;    ///< 対象動作モデルへのポインタ
  u32                        m_TargetActorID; ///< 話しかけアクターID
  FieldActorType             m_TargetActorType; ///< 話しかけアクターのタイプ

  ScriptInfo scrInfo; ///< スクリプト情報クラス
  cell m_ParentGlobalParams[ GLOBAL_PARAM_NUM ];  ///< グローバルコール用パラメータ親
  cell m_ChildGlobalParams[ GLOBAL_PARAM_NUM ];   ///< グローバルコール用パラメータ子
  
  u32 m_ScrTempWork[ TEMP_WORK_SIZE ];  ///< 一時ワーク
  
  bool                m_bEventPosEnable;  ///< イベントポジション(イベント相対座標システム基準点)
  gfl2::math::Vector3 m_EventPos;         ///< イベントポジション(イベント相対座標システム基準点)
  gfl2::str::StrBuf * m_pInputFreeWord;   ///< 文字入力されたフリーワードの保存域

  MotionCommandParam  m_cMotionCommandParam;

  s32                 m_ExtraMsgInstantArcID;
  s32                 m_ExtraMsgInstantDatID;

  bool      m_bFadeFlag;        ///< フェード有効フラグ
  bool      m_bPlaceNameNotCloseFlag; ///< 地名表示の中断抑制フラグ
  
  TRAINER_HITDATA   m_TrainerHitData[ TRAINER_EYE_HITMAX ];   ///< トレーナー視線データ


#if PM_DEBUG
  /// デバッグ キャラエディットリスト
  DebugChrEditList m_DebugChrEditList[DEBUG_CHR_EDIT_LIST_MAX];
  int m_DebugChrEditListNum;
#endif

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 一時ワークを取得
   *
   * @param work_no 一時ワークNO
   *
   * @return ワークへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  u32* GetTempWork( u16 work_no );


  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプトで操作を禁止するworkIdでないかチェック
   */
  /* ------------------------------------------------------------------------- */
  void checkBanWorkID( u16 work_no )const;
};

//--------------------------------------------------------------
/// 汎用メッセージワーク
//==============================================================
struct MsgWork {
  u32   arcID;
  u32   datID;
  void* pMsgBuf; ///< メッセージバッファ
};

#define FIELDTALKWINDOW_MAX   (2)

//--------------------------------------------------------------
/// シングルトン管理構造体
//==============================================================
struct SystemSingletones {
  MsgWork                                 *m_pMsgLoadWork;                              ///< メッセージロードワーク
  MsgWork                                 *m_pMsgSwapWork;                              ///< メッセージ交換ワーク
  Field::FieldWindow::FieldYesNoWindow    *m_pYesNoWin;                                 ///< はい/いいえワーク
  Field::FieldWindow::FieldListMenuWindow *m_pListMenu;                                 ///< リストメニュークラス
  Field::FieldWindow::FieldTalkWindow     *m_pFieldTalkWindow[ FIELDTALKWINDOW_MAX ];   ///< 話しかけ会話ワーク
  void                                    *m_pTemp;                                     ///< 汎用. 同時に複数使用しないことが前提の物に使う
  void                                    *m_pProc;                                     ///< 別proc起動用に使う
//  PlayerNickNameWork*     m_pPlayerNickNameWork;                                      ///< プレイヤーニックネームワーク
//  MsgWin::MsgWin32TmpWork*  m_pMsgWinWork;                                            ///< メッセージウィンドウ用ワーク
//  EventModelManager* m_pEventModelManager;
#if PM_DEBUG
  u32                                   m_load_msg_arc_id;                              ///< アーカイブをロードした際のメッセージアーカイブID
  u32                                   m_load_msg_data_id;                             ///< メッセージをロードした際のメッセージdatID
#endif
};

/* ------------------------------------------------------------------------- */
/**
 * @brief シングルトン初期化関数
 *
 * @param p_struct シングルトン管理構造体のポインタ
 */
/* ------------------------------------------------------------------------- */
inline void SystemSingletonesInit( SystemSingletones* p_struct )
{
  p_struct->m_pMsgLoadWork  = NULL;
  p_struct->m_pMsgSwapWork  = NULL;
  p_struct->m_pYesNoWin = NULL;
  p_struct->m_pListMenu = NULL;
  for( s32 ii=0 ; ii<FIELDTALKWINDOW_MAX ; ii++ ){
    p_struct->m_pFieldTalkWindow[ ii ] = NULL;
  }
  p_struct->m_pTemp = NULL;
//  p_struct->m_pPlayerNickNameWork = NULL;
//  p_struct->m_pMsgWinWork = NULL;
#if PM_DEBUG
  p_struct->m_load_msg_arc_id = 0;
  p_struct->m_load_msg_data_id = 0;
#endif
}



//--------------------------------------------------------------
/// script::SystemWork スクリプトシステムワーク(システムが持つ)
//==============================================================
class SystemWork
{ 
  GFL_FORBID_COPY_AND_ASSIGN( SystemWork ); ///< コピーコンストラクタ + 代入禁止
public:
  enum 
  {
    BUF_ID_NORMAL_MAX = 8, ///< WORDSET 通常の最大数
    BUF_ID_MAX = BUF_ID_NORMAL_MAX+1, ///< WORDSET 本当の最大数 +1=お金ウィンドウ
    BUF_ID_MONEY = 8, ///< WORDSET お金ウィンドウ用バッファID
  };

  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   *
   * @param p_gman ゲームマネージャへのポインタ
   * @param p_heap ヒープへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  SystemWork( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* p_heap );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ~SystemWork();
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief ワードセットクラスを取得
   *
   * @return ワードセットクラス
   */
  /* ------------------------------------------------------------------------- */
  print::WordSet* GetWordSet( void ) const { return m_pWordSet; }
  /* ------------------------------------------------------------------------- */
  /**
   * @brief チェックビットを立てる
   *
   * @param bit チェックビット
   */
  /* ------------------------------------------------------------------------- */
  void OnCheckBit( SCR_END_CHECK_BIT bit ); 
  /* ------------------------------------------------------------------------- */
  /**
   * @brief チェックビットを折る
   *
   * @param bit チェックビット
   */
  /* ------------------------------------------------------------------------- */
  void OffCheckBit( SCR_END_CHECK_BIT bit ); 
  /* ------------------------------------------------------------------------- */
  /**
   * @brief チェックビットを折る
   *
   * @param bit チェックビット
   */
  /* ------------------------------------------------------------------------- */
  bool CheckBit( SCR_END_CHECK_BIT bit ) const;

  /// シングルトン取得
  SystemSingletones* GetSingletones( void ) { return &m_Singletones; }

#if PM_DEBUG
  /// 終了チェックするIDを設定
  void SetEndCheckID( s32 id ) { m_EndCheckID = id; }
  /// 終了チェックするIDを取得
  s32 GetEndCheckID( void ) const { return m_EndCheckID; }
#endif // PM_DEBUG
  
private:
  print::WordSet* m_pWordSet;  ///< ワードセットクラス
  u32 m_CheckBit; ///< for SCR_END_CHECK_BIT

#if PM_DEBUG
  s32 m_EndCheckID; ///<終了チェックするID保持ワーク
#endif // PM_DEBUG

  SystemSingletones m_Singletones;  ///< シングルトンの実体
};

GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );

#endif // __SCRIPTWORK_H__
