//=============================================================================================
/**
 * @file  btl_Event.cpp
 * @brief ポケモンXY バトルシステム イベント＆ハンドラ処理
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#if 0
#include "poke_tool\poketype.h"

#include "btl_common.h"
#include "btl_pokeparam.h"
#include "btl_sideeff.h"
#include "btl_field.h"

#include "handler\hand_waza.h"

#include "btl_event.h"
#include "btl_event_factor.h"
#endif

#include  "btl_Calc.h"
#include  "btl_Field.h"
#include  "btl_SideEff.h"
#include  "btl_PosEff.h"
#include  "btl_Event.h"
#include  "btl_EventFactor.h"
#include  "btl_ServerFlow.h"

#include  "handler/hand_waza.h"


//--------------------------------------------------------
//--------------------------------------------------------
#define FACTOR_SERIAL_ENABLE    (0)    // EVENT_FACTOR生成ごとにシリアルを付与する


GFL_NAMESPACE_BEGIN(btl)


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  FACTOR_PER_POKE = EVENT_WAZA_STICK_MAX + 4,                 ///< ワザ貼り付き最大数+使用ワザ+とくせい+アイテム+オーラ = 12
  FACTOR_MAX_FOR_POKE = FACTOR_PER_POKE * BTL_EXIST_POS_MAX,  ///< ポケモンごと最大数(12) * 場所数(6) = 72
  FACTOR_MAX_FOR_SIDEEFF = BTL_SIDEEFF_MAX * BTL_SIDE_NUM,    ///< サイドエフェクト最大数(17)×陣営数(4) = 68
  FACTOR_MAX_FOR_POSEFF = BTL_POSEFF_MAX * BTL_EXIST_POS_MAX, ///< 位置エフェクト最大数(5) * 場所数(6) = 36
  FACTOR_MAX_FOR_FIELD = FieldStatus::EFF_MAX,                ///< フィールドエフェクト最大数 = 11
//  FACTOR_MAX_COPY = 30,                                       ///< とくせい（マジシャンなど）によるコピーハンドラ ポケモン数最大(6)×自分以外のポケ数(5)
  FACTOR_MAX_ALLOWANCE = 16,                                  ///< 余力

  // 登録できるイベントファクター最大数（72 + 68 + 11 + 36 + 16) = 203
  FACTOR_REGISTER_MAX = FACTOR_MAX_FOR_POKE + FACTOR_MAX_FOR_SIDEEFF + FACTOR_MAX_FOR_FIELD + FACTOR_MAX_FOR_POSEFF + FACTOR_MAX_ALLOWANCE,
  EVENTVAL_STACK_DEPTH = 96,   ///< イベント変数スタックの容量

  MAINPRI_BIT_WIDTH = 8,
  SUBPRI_BIT_WIDTH  = 24,
  MAINPRI_MAX =  ((1 << MAINPRI_BIT_WIDTH) - 1),
  SUBPRI_MAX  =  ((1 << SUBPRI_BIT_WIDTH) - 1),
  MAINPRI_MASK = MAINPRI_MAX,
  SUBPRI_MASK = SUBPRI_MAX,
};

/**
 * 値の書き換え可否状態
 */
typedef enum {

  REWRITE_FREE = 0,   ///< 自由に可能
  REWRITE_ONCE,       ///< １回だけ可能
  REWRITE_END,        ///< １回書き換えたのでもう終了
  REWRITE_MUL,        ///< 乗算のみ可能
  REWRITE_LOCK,       ///< 不可

}RewriteState;

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

/**
* イベントデータ実体
*/
struct _EVENT_FACTOR {
  BTL_EVENT_FACTOR*            prev;
  BTL_EVENT_FACTOR*            next;
  const BtlEventHandlerTable*  handlerTable;
  BtlEventSkipCheckHandler     skipCheckHandler;
  BtlEventFactorType           factorType;
  u32                          priority;           ///< ソートプライオリティ

  #if FACTOR_SERIAL_ENABLE
  u32                          serialNumber;       ///< 生成された順番（バトル開始時点から）
  #endif

  u32       currentStackPtr   : 16;  ///< 登録時イベントスタックポインタ
  u32       numHandlers       :  8;  ///< ハンドラテーブル要素数
  u32       callingFlag       :  1;  ///< 呼び出し中を再呼び出ししないためのフラグ
  u32       sleepFlag         :  1;  ///< 休眠フラグ
  u32       tmpItemFlag       :  1;  ///< アイテム用一時利用フラグ
  u32       rmReserveFlag     :  1;  ///< 削除予約フラグ
  u32       recallEnableFlag  :  1;  ///< 再帰呼び出し許可
  u32       existFlag         :  1;  ///< 現在処理中イベントによりAddされた
  u32       rotationSleepFlag :  1;  ///< 現在処理中イベントによりAddされた
  u32       nextReserveFlag   :  1;  ///< 現材処理中イベントの、呼び出し中ハンドラの次回に呼び出す予約フラグ
  int       work[ EVENT_HANDLER_WORK_ELEMS ];
  u16       subID;      ///< イベント実体ID。ワザならワザID, とくせいならとくせいIDなど
  u8        dependID;   ///< 依存対象物ID。ワザ・とくせい・アイテムならポケID、場所依存なら場所idなど。
  u8        pokeID;     ///< ポケ依存イベントならポケID／それ以外BTL_POKEID_NULL

};

/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR  Factors[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FactorStack[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FirstFactorPtr;
static u32 StackPtr;
static u32 EventStackPtr;

#if FACTOR_SERIAL_ENABLE
static u32 FactorSerial;
#endif

/**
* イベント変数スタック
*/
typedef struct {
  u32   sp;                                 ///< スタックポインタ
  u16   label[ EVENTVAL_STACK_DEPTH ];      ///< 変数ラベル
  int   value[ EVENTVAL_STACK_DEPTH ];      ///< 変数
  fx32  mulMax[ EVENTVAL_STACK_DEPTH ];     ///< 乗算対応実数の最大値
  fx32  mulMin[ EVENTVAL_STACK_DEPTH ];     ///< 乗算対応実数の最小値
  u8    rewriteState[ EVENTVAL_STACK_DEPTH ];///< 書き換え可否状態
}VAR_STACK;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/


namespace ev_local {

static VAR_STACK VarStack = {0};

void printLinkDebug( void );
void CallHandlersSub( ServerFlow* flowWork, BtlEventType eventID, BtlEventFactorType type, bool fSkipCheck );
void CallHandlersCore( ServerFlow* flowWork, BtlEventType eventID, BtlEventFactorType targetType, bool fSkipCheck );
BTL_EVENT_FACTOR* call_handler( ServerFlow* flowWork, BtlEventType eventID, BtlEventFactorType targetType, bool fSkipCheck, BTL_EVENT_FACTOR* factor );
bool check_handler_skip( ServerFlow* flowWork, BTL_EVENT_FACTOR* factor, BtlEventType eventID );
BTL_EVENT_FACTOR* pushFactor( void );
void popFactor( BTL_EVENT_FACTOR* factor );
void clearFactorWork( BTL_EVENT_FACTOR* factor );
void varStack_Init( void );
int evar_getNewPoint( const VAR_STACK* stack, BtlEvVarLabel label );
int evar_getExistPoint( const VAR_STACK* stack, BtlEvVarLabel label );
fx32 evar_mulValueRound( const VAR_STACK* stack, int ptr, fx32 value );


/**
 *  ファクタータイプがポケ依存するタイプか判定
 */
inline bool isDependPokeFactorType( BtlEventFactorType factorType )
{
  if( (factorType == BTL_EVENT_FACTOR_WAZA)
  ||  (factorType == BTL_EVENT_FACTOR_TOKUSEI)
  ||  (factorType == BTL_EVENT_FACTOR_ITEM)
  ||  (factorType == BTL_EVENT_FACTOR_ZENRYOKUWAZA)
  ){
    return true;
  }
  return false;
}
inline u32 calcFactorPriority( BtlEventPriority mainPri, u32 subPri )
{
  mainPri = static_cast<BtlEventPriority>(
                BTL_EVPRI_MAX - static_cast<int>(mainPri) - 1 );

  return (mainPri << SUBPRI_BIT_WIDTH) | subPri;
}
inline u16 getFactorMainPriority( u32 pri )
{
  u16 mainPri = (pri >> SUBPRI_BIT_WIDTH) & MAINPRI_MASK;
  mainPri = BTL_EVPRI_MAX - mainPri - 1;
  return mainPri;
}


}   // end of namespace 'ev_local"

//=============================================================================================
/**
 * システム初期化 （バトル開始時、サーバマシンから１回呼び出す）
 */
//=============================================================================================
void EVENT_InitSystem( void )
{
  for(u32 i=0; i<GFL_NELEMS(Factors); i++)
  {
    ev_local::clearFactorWork( &Factors[i] );
    FactorStack[i] = &Factors[i];
  }

  FirstFactorPtr = NULL;
  StackPtr = 0;
  EventStackPtr = 0;

#if FACTOR_SERIAL_ENABLE
  FactorSerial = 0;
#endif

  ev_local::varStack_Init();
}


//=============================================================================================
/**
 * ターンごとの初期化処理
 */
//=============================================================================================
void EVENT_StartTurn( void )
{
  // スリープフラグを落とす
  BTL_EVENT_FACTOR* factor;

  for(factor=FirstFactorPtr; factor!=NULL; factor=factor->next)
  {
    factor->sleepFlag = false;
  }
}



//----------------------------------------------------------------------------------
/**
 * リンクリスト情報をPrint（デバッグ用）
 */
//----------------------------------------------------------------------------------
static void printLinkDebug( void )
{
#if 0
  static const char* typestr[] = {
    "waza",
    "pos ",
    "side",
    "fld ",
    "tok ",
    "item",
    "isol",
  };
  BTL_EVENT_FACTOR* fp;
  u32 cnt = 0;

  BTL_PRINT("***** [[EV Chain]] *****\n");
  for(fp=FirstFactorPtr; fp!=NULL; fp=fp->next)
  {
    BTL_PRINT( "%p [%02d_%s:pri=%08x(%p)] -> ", fp, fp->dependID, typestr[fp->factorType], fp->priority, &(fp->priority) );
    ++cnt;
    if( cnt % 4 == 0 ){
      BTL_PRINT("\n");
    }
  }
  if( !cnt ){
    BTL_PRINT("..empty\n");
  }else if( cnt % 4 ){
    BTL_PRINT("\n");
  }
  BTL_PRINT("************************\n");
#endif
}


//=============================================================================================
/**
 * イベント反応要素を追加
 *
 * @param   factorType      ファクタータイプ
 * @param   subID           タイプごとの個別ID（とくせいID，アイテムID等と一致）
 * @param   priority        プライオリティ ( = 数値が高いほど先に呼び出される )
 * @param   pokeID
 * @param   handlerTable
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR* EVENT_AddFactor( BtlEventFactorType factorType, u16 subID,
  BtlEventPriority mainPri, u32 subPri, u8 dependID,
  const BtlEventHandlerTable* handlerTable, u16 numHandlers )
{
  GFL_ASSERT(mainPri < MAINPRI_MAX);
  GFL_ASSERT(subPri < SUBPRI_MAX);

  {
    BTL_EVENT_FACTOR* newFactor;

    newFactor = ev_local::pushFactor();
    if( newFactor )
    {
      newFactor->priority = ev_local::calcFactorPriority( mainPri, subPri );

      BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactorInfo, dependID, factorType, newFactor->priority, newFactor );

    #if FACTOR_SERIAL_ENABLE
      newFactor->serialNumber = FactorSerial++;
    #endif
      newFactor->factorType = factorType;
      newFactor->prev = NULL;
      newFactor->next = NULL;
      newFactor->handlerTable = handlerTable;
      newFactor->numHandlers = numHandlers;

      newFactor->subID = subID;
      newFactor->callingFlag = false;
      newFactor->sleepFlag = false;
      newFactor->rotationSleepFlag = false;
      newFactor->tmpItemFlag = false;
      newFactor->skipCheckHandler = NULL;
      newFactor->dependID = dependID;
      newFactor->rmReserveFlag = false;
      newFactor->recallEnableFlag = false;
      newFactor->nextReserveFlag = false;
      newFactor->currentStackPtr = EventStackPtr;
      newFactor->existFlag = true;
      if( ev_local::isDependPokeFactorType(factorType) ){
        newFactor->pokeID = dependID;
      }else{
        newFactor->pokeID = BTL_POKEID_NULL;
      }

      gfl2::std::MemClear( newFactor->work, sizeof(newFactor->work) );

  //    BTL_Printf("イベント登録 依存ポケID=%d\n", newFactor->pokeID);

      // 最初の登録
      if( FirstFactorPtr == NULL )
      {
        FirstFactorPtr = newFactor;
        BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactor_Case0 );
      }
      // 現在先頭より高プライオリティ
      else if( newFactor->priority > FirstFactorPtr->priority )
      {
        FirstFactorPtr->prev = newFactor;
        newFactor->next = FirstFactorPtr;
        newFactor->prev = NULL;
        FirstFactorPtr = newFactor;
        BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactor_Case1,
          FirstFactorPtr->next->dependID, FirstFactorPtr->next->factorType, FirstFactorPtr->next );
      }
      // それ以外はふつうのリンクリスト接続
      else
      {
        BTL_EVENT_FACTOR *find, *last;

        BTL_PRINT("[EV] NewFactor ptr=%p, priority=%08x\n", newFactor, newFactor->priority);

        last = FirstFactorPtr;
        for( find=FirstFactorPtr->next; find!=NULL; find=find->next )
        {
          if( newFactor->priority > find->priority )
          {
            BTL_PRINT("  find[%p]  ... pri=%08x, prev=%p, next=%p\n", find, find->priority, find->prev, find->next );
            newFactor->next = find;
            newFactor->prev = find->prev;
            if( find->prev ){
              find->prev->next = newFactor;
            }
            find->prev = newFactor;
            BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactor_Case2,
                   newFactor->prev->dependID, newFactor->prev->factorType, newFactor->prev,
                   newFactor->dependID, newFactor->factorType, newFactor,
                   find->dependID, find->factorType, find );
            break;
          }
          last = find;
        }
        // 最後まで見つからない場合
        if( find == NULL )
        {
          last->next = newFactor;
          newFactor->prev = last;
          BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactor_Case3,
                   last->dependID, last->factorType, last,
                   newFactor->dependID, newFactor->factorType, newFactor,
                   newFactor->next, FirstFactorPtr
                 );
        }
      }
      BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_AddFactor,
          newFactor->dependID, newFactor->factorType, newFactor, newFactor->priority );
      printLinkDebug();
      return newFactor;
    }
    // スタックから見つからない
    else
    {
      GFL_ASSERT(0);
      return NULL;
    }
  }
}

//=============================================================================================
/**
 * 分離ハンドラを全削除
 */
//=============================================================================================
void EVENT_RemoveIsolateFactors( void )
{
  BTL_EVENT_FACTOR *factor, *next;

  factor = FirstFactorPtr;
  while( factor != NULL )
  {
    next = factor->next;
    if( factor->factorType == BTL_EVENT_FACTOR_ISOLATE )
    {
      EVENT_FACTOR_Remove( factor );
    }
    factor = next;
  }
}

//=============================================================================================
/**
 * イベント反応要素を削除
 *
 * @param   factor
 */
//=============================================================================================
void EVENT_FACTOR_Remove( BTL_EVENT_FACTOR* factor )
{
  if( (factor!=NULL) && (factor->existFlag) )
  {
    if( factor->callingFlag )
    {
      factor->rmReserveFlag = true;
      TAYA_PRINT("  [event] コール中なので削除予約だけして返します : %p\n", factor);
      return;
    }

    if( factor == FirstFactorPtr )
    {
      FirstFactorPtr = factor->next;
    }

    if( factor->prev )
    {
      factor->prev->next = factor->next;
    }

    if( factor->next )
    {
      factor->next->prev = factor->prev;
    }

    TAYA_PRINT("  [event] ちゃんと削除しました : %p\n", factor);

    printLinkDebug();
    ev_local::popFactor( factor );
  }
}


//=============================================================================================
/**
 * 登録されている反応要素を、依存ポケモン素早さ順にソート（全く同じ値はランダムで入れ替わる）
 */
//=============================================================================================
void EVENT_ReOrderFactorsByPokeAgility( ServerFlow* flowWork )
{
  BTL_EVENT_FACTOR *fp1, *fp2;
  u32 factorCount = 0;

  // デバッグ出力
  TAYA_Printf( " -------- Before Sort --------\n");
  for( fp1=FirstFactorPtr; fp1!=NULL; fp1=fp1->next )
  {
    TAYA_Printf( "DependID=%2d, Type=%d, Pri=%08x\n", fp1->dependID, fp1->factorType, fp1->priority );
  }


  // 全ポケ依存要素のプライオリティ再設定
  for( fp1=FirstFactorPtr; fp1!=NULL; fp1=fp1->next )
  {
    if( fp1->pokeID != BTL_POKEID_NULL )
    {
      const BTL_POKEPARAM* bpp = flowWork->Hnd_GetPokeParam( fp1->pokeID );
      u16 agility = flowWork->Hnd_CalcAgility( bpp, true );
      u16 mainPri = ev_local::getFactorMainPriority( fp1->priority );
      fp1->priority = ev_local::calcFactorPriority( static_cast<BtlEventPriority>(mainPri), agility );
    }
    ++factorCount;
  }

  // デバッグ出力
  TAYA_Printf( " -------- Renew Priority --------\n");
  for( fp1=FirstFactorPtr; fp1!=NULL; fp1=fp1->next )
  {
    TAYA_Printf( "DependID=%2d, Type=%d, Pri=%08x\n", fp1->dependID, fp1->factorType, fp1->priority );
  }

  // プライオリティ順ソート
  if( factorCount > 1 )
  {
    bool fIns;

    fp1 = FirstFactorPtr->next;
    while( fp1 != NULL )
    {
      for( fp2=FirstFactorPtr; fp2!=fp1; fp2=fp2->next )
      {
        fIns = false;

        if( fp1->priority > fp2->priority )
        {
          fIns = true;
          TAYA_Printf(" %d-%2d(Pri:%08x) > %d-%2d(Pri:%08x)\n",
                  fp1->factorType, fp1->dependID, fp1->priority,
                  fp2->factorType, fp2->dependID, fp2->priority);
        }
        else if( fp1->priority == fp2->priority ){
          fIns = calc::GetRand( 2 )? true : false;
          TAYA_Printf(" %d-%2d(Pri:%08x) = %d-%2d(Pri:%08x) .. %d\n",
                  fp1->factorType, fp1->dependID, fp1->priority,
                  fp2->factorType, fp2->dependID, fp2->priority, fIns);
        }

        if( fIns )
        {
          if( fp1->prev ){
            fp1->prev->next = fp1->next;
          }
          if( fp1->next ){
            fp1->next->prev = fp1->prev;
          }

          fp1->prev = fp2->prev;
          fp1->next = fp2;

          if( fp2->prev ){
            fp2->prev->next = fp1;
          }
          fp2->prev = fp1;

          if( fp2 == FirstFactorPtr ){
            FirstFactorPtr = fp1;
          }

          {
            BTL_EVENT_FACTOR* fp;
            TAYA_Printf( " -------- つなぎ替え経過 --------\n");
            for( fp=FirstFactorPtr; fp!=NULL; fp=fp->next )
            {
              TAYA_Printf( "DependID=%2d, Type=%d, Pri=%08x\n", fp->dependID, fp->factorType, fp->priority );
            }
          }

          break;
        }
      }

      fp1 = fp1->next;
    }
  }

  // デバッグ出力
  TAYA_Printf( " -------- After Sort --------\n");
  for( fp1=FirstFactorPtr; fp1!=NULL; fp1=fp1->next )
  {
    TAYA_Printf( "DependID=%2d, Type=%d, Pri=%08x\n", fp1->dependID, fp1->factorType, fp1->priority );
  }
}

//=============================================================================================
/**
 * ファクターの種類を分離型に変換
 *
 * @param   factor
 */
//=============================================================================================
void EVENT_FACTOR_ConvertForIsolate( BTL_EVENT_FACTOR* factor )
{
  factor->factorType = BTL_EVENT_FACTOR_ISOLATE;
}


//=============================================================================================
/**
 * 分離型かどうか判定
 *
 * @param   factor
 */
//=============================================================================================
bool EVENT_FACTOR_IsIsolateMode( BTL_EVENT_FACTOR* factor )
{
  return ( factor->factorType == BTL_EVENT_FACTOR_ISOLATE );
}

//=============================================================================================
/**
 * 一時停止
 * @param factor
 * @return  この呼び出しにより一時停止したら true
 */
//=============================================================================================
bool  EVENT_FACTOR_Sleep( BTL_EVENT_FACTOR* factor )
{
  if( !factor->sleepFlag ){
    factor->sleepFlag = true;
    return true;
  }
  return false;
}
//=============================================================================================
/**
 * 一時停止を解除
 * @param factor
 * @return  この呼び出しにより一時停止を解除したら true
 */
//=============================================================================================
bool  EVENT_FACTOR_Weak( BTL_EVENT_FACTOR* factor )
{
  if( factor->sleepFlag ){
    factor->sleepFlag = false;
    return true;
  }
  return false;
}


//=============================================================================================
/**
 * 自己が抱える Work の内容を初期状態にクリアする
 *
 * @param   factor
 */
//=============================================================================================
void EVENT_FACTOR_ClearWork( BTL_EVENT_FACTOR* factor )
{
  gfl2::std::MemClear( factor->work, sizeof(factor->work) );
}


//=============================================================================================
/**
 * 実体ID（ワザID、とくせいID，アイテムID等、要素タイプに準じる）を返す
 *
 * @param   factor
 *
 * @retval  u16
 */
//=============================================================================================
u16 EVENT_FACTOR_GetSubID( const BTL_EVENT_FACTOR* factor )
{
  return factor->subID;
}

//=============================================================================================
/**
 * 関連ポケモンIDを返す
 *
 * @param   factor
 *
 * @retval  u8
 */
//=============================================================================================
u8 EVENT_FACTOR_GetPokeID( const BTL_EVENT_FACTOR* factor )
{
  return factor->pokeID;
}
//=============================================================================================
/**
 * ワーク内容を取得
 *
 * @param   factor
 * @param   workIdx
 *
 * @retval  int
 */
//=============================================================================================
int EVENT_FACTOR_GetWorkValue( const BTL_EVENT_FACTOR* factor, u8 workIdx )
{
  GFL_ASSERT(workIdx < EVENT_HANDLER_WORK_ELEMS);
  return factor->work[ workIdx ];
}

//=============================================================================================
/**
 * アイテム用一時利用フラグをONにする（BTL_EVENT_USE_ITEM_TMP に反応するようになる）
 *
 * @param   factor
 */
//=============================================================================================
void EVENT_FACTOR_SetTmpItemFlag( BTL_EVENT_FACTOR* factor )
{
  factor->tmpItemFlag = true;
}
//=============================================================================================
/**
 * 同一ハンドラの再帰呼び出し許可（アイテム用）
 *
 * @param   factor
 */
//=============================================================================================
void EVENT_FACTOR_SetRecallEnable( BTL_EVENT_FACTOR* factor )
{
  if( factor->callingFlag ){
    factor->recallEnableFlag = true;
  }
}
//=============================================================================================
/**
 * ワーク内容を設定
 *
 * @param   factor
 * @param   workIdx
 * @param   value
 */
//=============================================================================================
void EVENT_FACTOR_SetWorkValue( BTL_EVENT_FACTOR* factor, u8 workIdx, int value )
{
  GFL_ASSERT(workIdx < EVENT_HANDLER_WORK_ELEMS);
  factor->work[ workIdx ] = value;
}
//=============================================================================================
/**
 * 全登録要素に対し、指定イベントの強制通知（スキップ条件を見ない）
 *
 * @param   flowWork
 * @param   eventID
 */
//=============================================================================================
void EVENT_ForceCallHandlers( ServerFlow* flowWork, BtlEventType eventID )
{
  ev_local::CallHandlersSub( flowWork, eventID, BTL_EVENT_FACTOR_MAX, false );
}
//=============================================================================================
/**
 * 全登録要素に対し、指定イベントの通知（スキップ条件をチェック：通常はこちらを呼ぶ）
 *
 * @param   flowWork
 * @param   eventID
 */
//=============================================================================================
void EVENT_CallHandlers( ServerFlow* flowWork, BtlEventType eventID )
{
  ev_local::CallHandlersSub( flowWork, eventID, BTL_EVENT_FACTOR_MAX, true );
}
//=============================================================================================
/**
 * 全登録要素に対し、指定イベントの通知（スキップ条件をチェック＆特定タイプのハンドラのみ）
 *
 * @param   flowWork
 * @param   eventID
 * @param   type
 */
//=============================================================================================
void EVENT_CallHandlersTargetType( ServerFlow* flowWork, BtlEventType eventID, BtlEventFactorType type )
{
  ev_local::CallHandlersSub( flowWork, eventID, type, true );
}
//=============================================================================================
/**
 * スキップチェックハンドラをアタッチする
 *
 * @param   factor
 * @param   handler
 */
//=============================================================================================
void EVENT_FACTOR_AttachSkipCheckHandler( BTL_EVENT_FACTOR* factor, BtlEventSkipCheckHandler handler )
{
  factor->skipCheckHandler = handler;
}

//=============================================================================================
/**
 * スキップチェックハンドラをデタッチする
 *
 * @param   factor
 */
//=============================================================================================
void EVENT_FACTOR_DettachSkipCheckHandler( BTL_EVENT_FACTOR* factor )
{
  factor->skipCheckHandler = NULL;
}

/**
 * @brief 指定したタイプ・依存対象のイベントファクターを検索する
 * @param factoryType  検索対象のファクタータイプ
 * @param dependID     検索対象の依存対象ID
 * @return 指定したタイプ・依存対象の、戦闘にあるイベントファクター
 * @retval NULL  指定したタイプ・依存対象のイベントファクターが存在しない場合
 */
BTL_EVENT_FACTOR*  EVENT_SeekFactor( BtlEventFactorType factorType, u8 dependID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->existFlag) && (factor->factorType == factorType) && (factor->dependID == dependID) && (factor->rmReserveFlag==false) )
    {
      BTL_PRINT("[event] seek hit! type=%d, dependID=%d, rmReserve=%d : ptr=%p\n",
              factor->factorType, factor->dependID, factor->rmReserveFlag, factor );
      return factor;
    }
  }
  return NULL;
}

/**
 * @brief 指定したイベントファクターの次に登録されている、同タイプ、同依存対象IDを持つイベントファクターを取得する
 * @param factor  検索対象のイベントファクター
 * @retval  NULL  指定したイベントファクターと同タイプ、同依存対象IDを持つイベントファクターが存在しない場合
 */
BTL_EVENT_FACTOR* EVENT_GetNextFactor( BTL_EVENT_FACTOR* factor )
{
  if( factor )
  {
    BtlEventFactorType type = factor->factorType;
    u8 dependID = factor->dependID;

    factor = factor->next;
    while( factor ){
      if( (factor->factorType == type) && (factor->dependID == dependID) && (factor->rmReserveFlag==false) ){
        return factor;
      }
      factor = factor->next;
    }
  }
  return NULL;
}
//=============================================================================================
/**
 * 指定ポケモンの貼り付き状態ワザハンドラを休止させる（マジックミラー対処として）
 *
 * @param   pokeID
 *
 */
//=============================================================================================
void EVENT_SleepFactorMagicMirrorUser( u16 pokeID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == BTL_EVENT_FACTOR_WAZA) && (factor->pokeID == pokeID) )
    {
      if( handler::waza::IsStick(factor, factor->work) )
      {
        factor->sleepFlag = true;
      }
    }
  }
}
void EVENT_WakeFactorMagicMirrorUser( u16 pokeID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == BTL_EVENT_FACTOR_WAZA) && (factor->pokeID == pokeID) )
    {
      factor->sleepFlag = false;
    }
  }
}

//=============================================================================================
/**
 * 指定ポケモンの「とくせい」「どうぐ」ハンドラを休止（ローテーション対処）
 *
 * @param   pokeID
 */
//=============================================================================================
void EVENT_SleepFactorRotation( u16 pokeID, BtlEventFactorType factorType )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->pokeID == pokeID)
    &&  (factor->factorType == factorType)
    ){
      factor->rotationSleepFlag = true;
    }
  }
}
//=============================================================================================
/**
 * 指定ポケモンの「とくせい」「どうぐ」ハンドラを休止から復帰（ローテーション対処）
 *
 * @param   pokeID
 * @param   factorType
 *
 * @retval  bool    復帰させたハンドラがあればtrue
 */
//=============================================================================================
bool EVENT_WakeFactorRotation( u16 pokeID, BtlEventFactorType factorType )
{
  BTL_EVENT_FACTOR* factor;
  bool result = false;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->pokeID == pokeID)
    &&  (factor->factorType == factorType)
    ){
      factor->rotationSleepFlag = false;
      result = true;
    }
  }

  return result;
}



//======================================================================================================
//  イベント変数スタック構造
//======================================================================================================
enum {
  EVAR_PRINT_FLG = false,
};


//------------------------------------------------------------------------------
/**
 * スタックが空になっているかチェック（毎ターン行う）
 *
 * 空になっていなければ → テスト時はASSERT停止する
 *                       →非テスト時は次善の策としてスタックを空にする
 */
//------------------------------------------------------------------------------
void EVENTVAR_CheckStackCleared( void )
{
  VAR_STACK* stack = &ev_local::VarStack;

  if( stack->sp ){
    GFL_ASSERT_MSG(0, "sp=%d", stack->sp);
    ev_local::varStack_Init();
  }
}

//------------------------------------------------------------------------------
/**
 * スタックPush
 */
//------------------------------------------------------------------------------
void EVENTVAR_Push( void )
{
  VAR_STACK* stack = &ev_local::VarStack;

  while( stack->sp < GFL_NELEMS(stack->label) )
  {
    if( stack->label[stack->sp] == BTL_EVAR_NULL ){
      break;
    }
    stack->sp++;
  }

  if( stack->sp < (GFL_NELEMS(stack->label)-1) )
  {
    stack->label[ stack->sp++ ] = BTL_EVAR_SYS_SEPARATE;
    #ifdef PM_BTL_DEBUG
    if( stack->sp >= (GFL_NELEMS(stack->label)/8*7) ){
      GFL_ASSERT_MSG(0, "Event StackPointer =%d 危険水域です！！", stack->sp);
    }
    #endif
    BTL_N_PrintfEx( EVAR_PRINT_FLG, DBGSTR_EVAR_Push, 0, stack->sp);
  }
  else
  {
    GFL_ASSERT_STOP(0);   // stack overflow
  }
}

void EVENTVAR_Pop( void )
{
  VAR_STACK* stack = &ev_local::VarStack;

  if( stack->sp > 0 )
  {
    u16 p = stack->sp;
    while( (p < GFL_NELEMS(stack->label)) && (stack->label[p] != BTL_EVAR_NULL) ){
      stack->label[p++] = BTL_EVAR_NULL;
    }

    stack->sp--;
    stack->label[ stack->sp ] = BTL_EVAR_NULL;

    if( stack->sp )
    {
      while( stack->sp )
      {
        stack->sp--;
        if( stack->label[stack->sp] == BTL_EVAR_SYS_SEPARATE ){
          stack->sp++;
          break;
        }
      }
    }
    BTL_N_PrintfEx( EVAR_PRINT_FLG, DBGSTR_EVAR_Pop, 0, stack->sp);
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//=============================================================================================
/**
 * ラベルの値を新規セット（サーバフロー用）
 *
 * @param   label
 * @param   value
 */
//=============================================================================================
void EVENTVAR_SetValue( BtlEvVarLabel label, int value )
{
  GFL_ASSERT(label!=BTL_EVAR_NULL);
  GFL_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &ev_local::VarStack;
    int p = ev_local::evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = 0;
    stack->mulMax[p] = 0;
    stack->rewriteState[p] = REWRITE_FREE;
  }
}
//=============================================================================================
/**
 * 上書き禁止ラベルの値を新規セット（サーバフロー用）
 *
 * @param   label
 * @param   value
 */
//=============================================================================================
void EVENTVAR_SetConstValue( BtlEvVarLabel label, int value )
{
  GFL_ASSERT(label!=BTL_EVAR_NULL);
  GFL_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &ev_local::VarStack;
    int p = ev_local::evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = 0;
    stack->mulMax[p] = 0;
    stack->rewriteState[p] = REWRITE_LOCK;
  }
}
//=============================================================================================
/**
 * 上書き１回のみ有効なラベルの値を新規セット（サーバフロー用）
 *
 * @param   label
 * @param   value
 */
//=============================================================================================
void EVENTVAR_SetRewriteOnceValue( BtlEvVarLabel label, int value )
{
  GFL_ASSERT(label!=BTL_EVAR_NULL);
  GFL_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &ev_local::VarStack;
    int p = ev_local::evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = 0;
    stack->mulMax[p] = 0;
    stack->rewriteState[p] = REWRITE_ONCE;
  }
}

//=============================================================================================
/**
 * 上書き１回のみ有効なラベルの上書き権利を復活させる
 *
 * @param   label
 */
//=============================================================================================
void EVENTVAR_RecoveryRewriteOncePermission( BtlEvVarLabel label )
{
  GFL_ASSERT(label!=BTL_EVAR_NULL);
  GFL_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &ev_local::VarStack;
    int p = ev_local::evar_getExistPoint( stack, label );
    if( p >= 0 )
    {
      stack->rewriteState[p] = REWRITE_ONCE;
    }
  }
}

//=============================================================================================
/**
 * 乗算対応ラベルの値を新規セット（サーバフロー用）
 *
 * @param   label
 * @param   value
 * @param   mulMin
 * @param   mulMax
 */
//=============================================================================================
void EVENTVAR_SetMulValue( BtlEvVarLabel label, int value, fx32 mulMin, fx32 mulMax )
{
  GFL_ASSERT(label!=BTL_EVAR_NULL);
  GFL_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &ev_local::VarStack;
    int p = ev_local::evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = mulMin;
    stack->mulMax[p] = mulMax;
    stack->rewriteState[p] = REWRITE_MUL;
  }
}
//=============================================================================================
/**
 * 既存ラベルの値を上書き（ハンドラ用）
 *
 * @param   label
 * @param   value
 *
 * @retval  bool    上書き出来たらtrue
 */
//=============================================================================================
bool EVENTVAR_RewriteValue( BtlEvVarLabel label, int value )
{
  GFL_ASSERT(label!=BTL_EVAR_NULL);
  GFL_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &ev_local::VarStack;
    int p = ev_local::evar_getExistPoint( stack, label );
    if( p >= 0 )
    {
      if( (stack->rewriteState[p] == REWRITE_FREE)
      ||  (stack->rewriteState[p] == REWRITE_ONCE)
      ){
        stack->label[p] = label;
        stack->value[p] = (int)value;
        if( stack->rewriteState[p] == REWRITE_ONCE ){
          stack->rewriteState[p] = REWRITE_END;
        }
        return true;
      }
      else if(  (stack->rewriteState[p] == REWRITE_LOCK)
      ||        (stack->rewriteState[p] == REWRITE_MUL)
      ){
        GFL_ASSERT_MSG(0, "this label(%d) can't rewrite\n", label);
      }
    }
    return false;
  }
}
//=============================================================================================
/**
 * 既存の乗算対応ラベルの値に数値を乗算する
 *
 * @param   label   ラベルID
 * @param   value   乗算する値
 */
//=============================================================================================
void EVENTVAR_MulValue( BtlEvVarLabel label, fx32 value )
{
  GFL_ASSERT(label!=BTL_EVAR_NULL);
  GFL_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &ev_local::VarStack;
    int p = ev_local::evar_getExistPoint( stack, label );
    if( p >= 0 )
    {
      if( stack->rewriteState[p] == REWRITE_MUL )
      {
        value = FX_Mul( stack->value[p], value );
        value = ev_local::evar_mulValueRound( stack, p, value );
        stack->value[p] = (int)value;
      }else{
        GFL_ASSERT_MSG(0, "this label(%d) can't mul\n", label);
      }
    }
  }
}
//=============================================================================================
/**
 * 既存ラベルの値を取得
 *
 * @param   label
 *
 * @retval  int
 */
//=============================================================================================
int EVENTVAR_GetValue( BtlEvVarLabel label )
{
  GFL_ASSERT(label!=BTL_EVAR_NULL);
  GFL_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);
  {
    VAR_STACK* stack = &ev_local::VarStack;

    int p = stack->sp;
    while( p < GFL_NELEMS(stack->label) )
    {
      if( stack->label[p] == label ){
        return stack->value[p];
      }
      if( stack->label[p] == BTL_EVAR_NULL ){
        break;
      }
      ++p;
    }
    GFL_ASSERT_MSG(0,"label-%d not found",label);  // label not found
    return 0;
  }
}
//=============================================================================================
/**
 * 指定ラベルが存在すればその値を取得、なければ何もしない
 *
 * @param   label
 * @param   value   [out]
 *
 * @retval  bool    ラベルあればtrue
 */
//=============================================================================================
bool EVENTVAR_GetValueIfExist( BtlEvVarLabel label, int* value )
{
  GFL_ASSERT(label!=BTL_EVAR_NULL);
  GFL_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);
  {
    VAR_STACK* stack = &ev_local::VarStack;

    int p = stack->sp;
    while( p < GFL_NELEMS(stack->label) )
    {
      if( stack->label[p] == label ){
        *value = stack->value[p];
        return true;
      }
      if( stack->label[p] == BTL_EVAR_NULL ){
        break;
      }
      ++p;
    }
    return false;
  }
}

namespace ev_local {

/**
 *  イベント通知共通下請け
 */
void CallHandlersSub( ServerFlow* flowWork, BtlEventType eventID, BtlEventFactorType type, bool fSkipCheck )
{
  ++EventStackPtr;
  CallHandlersCore( flowWork, eventID, type, fSkipCheck );
  --EventStackPtr;

  if( EventStackPtr == 0 )
  {
    BTL_EVENT_FACTOR* factor;
    for( factor=FirstFactorPtr; factor!=NULL; factor = factor->next )
    {
      factor->currentStackPtr = 0;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 指定イベント通知コア
 *
 * @param   flowWork
 * @param   eventID
 * @param   fSkipCheck
 */
//----------------------------------------------------------------------------------
void CallHandlersCore( ServerFlow* flowWork, BtlEventType eventID, BtlEventFactorType targetType, bool fSkipCheck )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; )
  {
    BTL_EVENT_FACTOR* next_factor = call_handler( flowWork, eventID, targetType, fSkipCheck, factor );

    if( next_factor == NULL ){
      break;
    }

    if( next_factor->existFlag && next_factor->nextReserveFlag )
    {
      next_factor->nextReserveFlag = false;
      factor = next_factor;
    }
    else
    {
      factor = factor->next;
    }
  }
}

 BTL_EVENT_FACTOR* call_handler( ServerFlow* flowWork, BtlEventType eventID, BtlEventFactorType targetType, bool fSkipCheck, BTL_EVENT_FACTOR* factor )
 {
   BTL_EVENT_FACTOR* next_factor = factor->next;
    if( next_factor ){
      next_factor->nextReserveFlag = true;
    }

   if( (factor->callingFlag) && (factor->recallEnableFlag == false) ){
     return next_factor;
   }
   if( factor->sleepFlag ){
     return next_factor;
   }
   if( factor->rotationSleepFlag ){
     return next_factor;
   }
   if( (targetType!=BTL_EVENT_FACTOR_MAX) && (factor->factorType != targetType) ){
     return next_factor;
   }
   if( (factor->currentStackPtr != 0) && (factor->currentStackPtr >= EventStackPtr) ){
     return next_factor;
   }
   if( factor->existFlag == false ){
     return next_factor;
   }
   if( (eventID == BTL_EVENT_USE_ITEM_TMP) && (factor->tmpItemFlag == false) ){
     return next_factor;
   }

   const BtlEventHandlerTable* tbl = factor->handlerTable;

   for(u32 i=0; i<factor->numHandlers; i++)
   {
     if( tbl[i].eventType == eventID )
     {
       if( !fSkipCheck || !check_handler_skip(flowWork, factor, eventID) )
       {
         factor->callingFlag = true;
         tbl[i].handler( factor, flowWork, factor->dependID, factor->work );

         if( factor->recallEnableFlag ){
           factor->recallEnableFlag = false;
         }else{
           factor->callingFlag = false;
         }
         // 呼び出し中に削除された
         if( factor->rmReserveFlag )
         {
#if PM_DEBUG
           if( factor->factorType == BTL_EVENT_FACTOR_ITEM ){
             TAYA_PRINT("  [Event] 呼び出し中に削除されたアイテムハンドラ [%p]\n", factor );
           }
#endif

           if( next_factor != NULL )
           {
             while( next_factor->existFlag == false ){
               next_factor = factor->next;
               if( next_factor == NULL ){
                 break;
               }
             }
             if( next_factor != NULL ){
               next_factor->nextReserveFlag = true;
             }
           }
           EVENT_FACTOR_Remove( factor );
         }
       }
       return next_factor;
     }
   }

   return next_factor;
 }


/**
 *  特定ハンドラをスキップするかチェック
 */
bool check_handler_skip( ServerFlow* flowWork, BTL_EVENT_FACTOR* factor, BtlEventType eventID )
{
  const BTL_POKEPARAM* bpp = NULL;

  if( factor->pokeID != BTL_POKEID_NULL ){
    bpp = flowWork->Hnd_GetPokeParam( factor->pokeID );
  }

  // とくせいハンドラ
  if( factor->factorType == BTL_EVENT_FACTOR_TOKUSEI )
  {
    if( (bpp!=NULL) && bpp->CheckSick(pml::wazadata::WAZASICK_IEKI) ){  // 「いえき」状態のポケモンは呼び出さない
      return true;
    }
  }

  // アイテムハンドラ
  if( factor->factorType == BTL_EVENT_FACTOR_ITEM )
  {
    //@バトル変更箇所
    //SBTS[108]対応 カイグラの進化は道具が使えなくても行われる
    if( eventID != BTL_EVENT_MEMBER_IN_EVO )
    {
      if( flowWork->Hnd_CheckFieldEffect(FieldStatus::EFF_MAGICROOM) ){  // 「マジックルーム」発動状態なら呼び出さない
        return true;
      }
      if( (bpp!=NULL) && bpp->CheckSick(pml::wazadata::WAZASICK_SASIOSAE) ){  // 「さしおさえ」状態のポケモンは呼び出さない
        return true;
      }
      if( (bpp!=NULL) && bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_ITEM_CANT_USE) ){  // 装備アイテム使えないフラグONなら呼び出さない
        return true;
      }
    }

  }


  // スキップチェックハンドラが登録されていたら判断させる
  {
    BTL_EVENT_FACTOR* fp;
    for(fp=FirstFactorPtr; fp!=NULL; fp=fp->next)
    {
      if( fp->skipCheckHandler != NULL )
      {
        if( fp->factorType == BTL_EVENT_FACTOR_TOKUSEI )
        {
          const BTL_POKEPARAM* skipBpp = flowWork->Hnd_GetPokeParam( fp->pokeID );
          if( skipBpp && skipBpp->CheckSick(pml::wazadata::WAZASICK_IEKI) ){
            // いえき状態はスキップチェックも行えないようにする
            continue;
          }
        }
        if( fp->rotationSleepFlag ){
          continue;
        }

        if( (fp->skipCheckHandler)( fp, flowWork, factor->factorType, eventID, factor->subID, factor->dependID ) ){
          BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_SkipByCheckHandler, factor->dependID, factor->factorType );
          return true;
        }
      }
    }
  }
  return false;
}

/**
 * イベントスタック Push
 *
 * @retval  BTL_EVENT_FACTOR*
 */
BTL_EVENT_FACTOR* pushFactor( void )
{
  if( StackPtr == FACTOR_REGISTER_MAX )
  {
    return NULL;
  }
  return FactorStack[ StackPtr++ ];
}

/**
 * イベントスタック Pop
 *
 * @retval  BTL_EVENT_FACTOR*
 */
void popFactor( BTL_EVENT_FACTOR* factor )
{
  if( StackPtr == 0 )
  {
    GFL_ASSERT(0);
    return;
  }
  clearFactorWork( factor );  //値をクリアしてからスタックに積む
  --StackPtr;
  FactorStack[ StackPtr ] = factor;
}

void clearFactorWork( BTL_EVENT_FACTOR* factor )
{
  gfl2::std::MemClear( factor, sizeof(*factor) );
}
/**
 *  イベント変数スタック初期化
 */
void varStack_Init( void )
{
  VAR_STACK* stack = &VarStack;

  stack->sp = 0;
  for(u32 i=0; i<GFL_NELEMS(stack->label); ++i)
  {
    stack->label[i] = BTL_EVAR_NULL;
  }
}
// 新規ラベル用に確保した位置のポインタを返す
int evar_getNewPoint( const VAR_STACK* stack, BtlEvVarLabel label )
{
  int p = stack->sp;
  while( p < GFL_NELEMS(stack->label) ){
    if( stack->label[p] == BTL_EVAR_NULL ){ break; }
    if( stack->label[p] == label ){
      GFL_ASSERT_MSG(0, "Find Same Label ID : %d\n", label);
      break;
    }
    ++p;
  }

  if( p < GFL_NELEMS(stack->value) )
  {
    #ifdef PM_BTL_DEBUG
    if( p >= (GFL_NELEMS(stack->label)/8*7) ){
      BTL_Printf("Var Stack sp=%d 危険水域です！！\n", p);
    }
    #endif
    BTL_PrintfEx( EVAR_PRINT_FLG, "[EVAR]  >regist pos:%d label=%d\n", p, label);
    return p;
  }
  GFL_ASSERT(0); // stack overflow
  return 0;
}
// 既存ラベル位置のポインタを返す
int evar_getExistPoint( const VAR_STACK* stack, BtlEvVarLabel label )
{
  int p = stack->sp;
  while( (p < GFL_NELEMS(stack->label)) && (stack->label[p] != BTL_EVAR_NULL) )
  {
    if( stack->label[p] == label ){
      return p;
    }
    ++p;
  }
  GFL_ASSERT_MSG(0, "label[%d] not found", label);
  return -1;
}
// 乗算対応変数の上下限を越えていたら丸めて返す
fx32 evar_mulValueRound( const VAR_STACK* stack, int ptr, fx32 value )
{
  if( stack->mulMax[ptr] || stack->mulMin[ptr] )  // どちらかが0以外なら乗算対応と見なす
  {
    if( stack->mulMin[ptr] > value ){
      value = stack->mulMin[ptr];
    }
    if( stack->mulMax[ptr] < value ){
      value = stack->mulMax[ptr];
    }
  }
  return value;
}




} // end of namespace 'ev_local"
GFL_NAMESPACE_END(btl)



