//=============================================================================
/**
 * @file BattleUIDataManager.h
 * @brief バトルUI側で使用するデータの管理
 * @author yuto uchida.
 * @date 2014/05/27
 * @note
 *      メッセージデータなどUI側で使用するデータ作成、開放などの制御を管理
 */
//=============================================================================
#ifndef __BATTLE_UI_DATA_MANAGER_H__
#define __BATTLE_UI_DATA_MANAGER_H__

#include <macro/include/gfl2_Macros.h>

#include <Savedata/include/SaveData.h>
#include <Savedata/include/MyItemSave.h>

#include <Print/include/PrintSystem.h>
#include <Print/include/WordSet.h>
#include <Print/include/ItemMessage.h>
#include <util/include/gfl2_List.h>

#include "Battle/source/btlv/btlv_core.h"
#include "pml/include/item/item_manager.h"
#include "App/Bag/include/BagAppParam.h"

#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetType.h"

#include "AppLib/include/Util/AppUtilFileControlState.h"

//  view
#include "BattleUIRecPlayLowerView.h"

GFL_NAMESPACE_BEGIN(btl)

  //  型の前方宣言
  class BTL_PARTY;
  class POKECON;

GFL_NAMESPACE_BEGIN(app)

// マクロ
//  ファイルの制御の流れを把握するために用意
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
  #define  StartPrefetchBtlFileLoad(req)     __StartPrefetchBtlFileLoad(req, __FILE__, __LINE__)
  #define  EndPrefetchBtlFileLoad(req)       __EndPrefetchBtlFileLoad(req, __FILE__, __LINE__)
  #define  IsPrefetchBtlFileLoad(req)        __IsPrefetchBtlFileLoad(req, __FILE__, __LINE__)
  #define  GetPrefetchBtlFileLoadData(req)   __GetPrefetchBtlFileLoadData(req, __FILE__, __LINE__)
#endif

  /**
    @fix  MMcat[240]   立ち位置Cのポケモンが「味方場」「味方全ポケ」の効果範囲の技を使用した後の、カーソル位置がおかしい
          立ち位置Cのポケモンで「味方場」の技（リフレクターなど）によりカーソル位置を記憶（立ち位置A）した後、「通常ポケ」が対象の技（でんじはなど）を選択すると、
          立ち位置Aが選択可能であった場合にはそれが採用されてしまう（でんじはを味方Aに撃とうとする）
          目的の違う別の技の情報を使用して初期カーソル位置を決定していることに問題があるため、カーソル位置の記憶に技Noを紐付けて管理するようにした
    @fix  MMcat[240]
          更にカーソル位置記憶数を8にし、技ごとにカーソル位置を記憶できるようにした
  */
  //  カーソル記憶クラス
  class PokeCursorMemory
  {
  public:
    PokeCursorMemory() { _Clear(); }

    void Reset()
    {
      _Clear();
    }

    void SetWazaCursorPos( const s32 cursorPos )
    {
      m_wazaSelectCursorPos = cursorPos;
    }

    void SetTargetCursorPos( const s32 cursorPos, const WazaNo wazaNo );

    s32 GetWazaCursorPos()    const { return m_wazaSelectCursorPos; }
    s32 GetTargetCursorPos(const WazaNo wazaNo) const;

  private:
    void _Clear();
    s32   FindTargetCursorPos(const WazaNo wazaNo) const;

    s32     m_wazaSelectCursorPos;

    static const u32  TargetSelectCursorPosInfoNum  = 8;
    struct TargetSelectCursorPosInfo
    {
      s32     cursorPos;
      WazaNo  wazaNo;
      
      TargetSelectCursorPosInfo(void);
      void  SetCursorPos(const s32 _cursorPos, const WazaNo _wazaNo)  {cursorPos  = _cursorPos; wazaNo = _wazaNo;}
      void  Clear(void)                                               {wazaNo = WAZANO_NULL;}
    }m_TargetSelectCursorPosInfo[TargetSelectCursorPosInfoNum];
    u32 m_TargetSelectCursorPosInfoCount;
  };

  class BattleUIDataManager
  {
  public:
    //  ポケモンリストタイプ
    enum PokeListType
    {
      MEMBER_MULTI_PARTY_R  = 0,
      MEMBER_MULTI_PARTY_L,
      MEMBER_SINGLE_PARTY,
    };

    //  ポケモン状態のタイプ
    enum StatusType
    {
      STATUS_TYPE_NONE = 0,
      STATUS_TYPE_COUNT,
      STATUS_TYPE_COUNT_SIGN,
      STATUS_TYPE_TURN
    };


    //  ファイルキャッシュ一覧
    enum FileCatchType
    {
      FILE_CATCH_TYPE_SKILL_SELECT_LYT = 0,
      FILE_CATCH_TYPE_MAX
    };

    enum
    {
      //  状態リストは固定値にする(いちいち動的の個数を探すのが面倒)
      STATUS_DATA_MAX = 80
    };

    //  通信対戦相手のパラメータ
    struct ENEMY_TRAINER_PARAM
    {
      const gfl2::str::StrBuf*            pName;
      print::GrammaticalAttribute         grammer;
      const ClientPublicInformation*      pInfoParam;
      u16                                 ratingNum;
      bool                                bRating;

      ENEMY_TRAINER_PARAM()
      {
        pName       = NULL;
        pInfoParam  = NULL;
        ratingNum   = 0;
        bRating     = true;
        grammer.Raw = 0;
      }
    };

    //  状態パラメータ
    struct _tag_status_param_turn_
    {
      u8  now, max;
    };

    struct _tag_status_param_count_
    {
      u8  num;
    };

    typedef struct _tag_status_param_
    {
      StatusType  type;

      s32 id;

      u32 titleNameId;
      u32 infoNameId;
      
      struct _tag_data_
      {
        union 
        {
          u32  work[ 4 ];

          struct _tag_status_param_turn_  turn;
          struct _tag_status_param_count_ count;
        };
      } data;

      _tag_status_param_()
      {
        type        = STATUS_TYPE_NONE;
        id          = -1;
        titleNameId = 0;
        infoNameId  = 0;
        gfl2::std::MemClear( &data, sizeof( data ) );
      }

    } STATUS_DATA;

    typedef struct _tag_poke_status_data_
    {
      s32         pokeDataId;

      u32         statusNum;
      STATUS_DATA aStatus[ STATUS_DATA_MAX ];

      _tag_poke_status_data_()
      {
        pokeDataId  = -1; 
        statusNum   = 0;
      }

    } POKE_STATUS_DATA;

    BattleUIDataManager(BattleViewSystem* btlv_core);
    virtual ~BattleUIDataManager();
  
    //------------------------------------------------------------------
    /**
     * @brief   初期化
     * @param   pResidentHeap : NEWが一度しか行われず、バトル終了まで開放しない時に使うヒープ
     */
    //------------------------------------------------------------------
    void Initialize( gfl2::heap::HeapBase* pResidentHeap , void *btlAppMsgBuf );
    //非同期処理の待ち部分
    bool InitializeWait( gfl2::heap::HeapBase* pResidentHeap );

    void Terminate();

    //メッセージの分割読み
    // @return true 終了
    bool LoadMessage(gfl2::heap::HeapBase* pResidentHeap);

    //  使用準備可能か
    //  ※ Initialize / Terminate以外の関数を使う場合に、IsReady関数がtrueにならないと使えない！！
    //     falseが返ってくると状態で使うとASSERTで止まるか、ハングする
    bool IsReady();

    //  インスタンスを破棄可能か
    bool IsDelete();

    //  更新
    void Update();

    /** バトルコアのデータを返す */
    const BattleViewSystem* GetCore() const { return m_btlv; }

    /** 自分自身のバトルパーティデータを取得 */
    const BTL_PARTY* GetBattleParty_Self( void ) const;

    /** 敵のバトルパーティデータを取得 */
    const BTL_PARTY* GetBattleParty_Enemy( int idx = 0 ) const;

    /** 友達のバトルパーティデータを取得 */
    const BTL_PARTY* GetBattleParty_Friend() const;

    /** バトルUI画面の入力SEが再生許可 */
    bool IsPlayInputSE() const;

    u8 GetFriendCleintID( void ) const;
    u8 GetClientID( void ) const;

    bool IsClientNPC( u8 clientID ) const;

    //  にげることができるか
    bool IsEsc();

    //  バトル対戦の種類を取得
    BtlCompetitor GetBattleCompetitor( void ) const;

    POKECON* GetBattleContainer( void ) const;

    //  配置されているポケモンデータ取得
    BTL_POKEPARAM* GetBattlePokeParam( BtlvPos pos ) const;

    bool IsPlayerInLeftSide( void );
    bool IsMultiMode( void );
  
    //  バトルルールを取得
    BtlRule GetBattleRule( void ) const;

    //  バトルの天候取得
    BtlWeather GetWeather() const;

    //  所持しているアイテム取得
    Savedata::MyItem* GetMyItem();

    //  通信モードか取得(対戦しているかは GetBattleCompetior() -> BTL_COMPETITOR_COMM で判別する )
    bool IsCommMode(void) const;

    bool GetEnableVoiceChat(void) const;
    bool IsDemoCapture(void) const;


    //  AIコントロールターンか
    bool IsAICtrlTurn();

    //------------------------------------------------------------------
    /**
     * @brief   ショートカットボールアイテムが使えるか
     */
    //------------------------------------------------------------------
    bool IsShortCutUseItemBall();

    //------------------------------------------------------------------
    /**
     * @brief   ファイルロードデータすべて開放しているか
     */
    //------------------------------------------------------------------
    bool IsFreeAllFileLoad();

    //------------------------------------------------------------------
    /**
     * @brief   ファイルロードデータを一括開放
     */
    //------------------------------------------------------------------
    void FreeAllFileLoad();

    //------------------------------------------------------------------
    /**
     * @brief   ファイルロード開始
     * @note    レイアウトバイナリの取得を先読みするために作成
     */
    //------------------------------------------------------------------
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    void __StartPrefetchBtlFileLoad( const FileCatchType type, const char* pCallerFileName, const u32 callerLineNo );
#else
    void StartPrefetchBtlFileLoad( const FileCatchType type );
#endif

    //------------------------------------------------------------------
    /**
     * @brief   ファイルロード終了
     */
    //------------------------------------------------------------------
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    void __EndPrefetchBtlFileLoad( const FileCatchType type, const char* pCallerFileName, const u32 callerLineNo );
#else
    void EndPrefetchBtlFileLoad( const FileCatchType type );
#endif

    //------------------------------------------------------------------
    /**
     * @brief   ファイルロードしているか
     */
    //------------------------------------------------------------------
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    bool __IsPrefetchBtlFileLoad( const FileCatchType type, const char* pCallerFileName, const u32 callerLineNo );
#else
    bool IsPrefetchBtlFileLoad( const FileCatchType type );
#endif

    //------------------------------------------------------------------
    /**
     * @brief   ファイルロードのデータ取得
     */
    //------------------------------------------------------------------
#if GFL_FS_DEBUG_MAKE_ARC_FILE_REQ_CALLER_LIST
    void* __GetPrefetchBtlFileLoadData( const FileCatchType type, const char* pCallerFileName, const u32 callerLineNo );
#else
    void* GetPrefetchBtlFileLoadData( const FileCatchType type );
#endif

    //------------------------------------------------------------------
    /**
     * @brief   バトルのルールなどによるポケモンリストタイプ取得
     */
    //------------------------------------------------------------------
    PokeListType  GetPokeListType();

    //------------------------------------------------------------------
    /**
     * @brief   バトルの味方・敵の配置リスト一覧を作成
     * @note
     *  行動選択開始前に必ず呼び出す
     *  作成したリストは下記のケースで使用される
     *    ・行動選択画面のポケモンアイコン配置
     *    ・わざ選択の敵効果タイプ取得
     */
    //------------------------------------------------------------------
    void CreatePokeList( const BtlRule rule );

    //------------------------------------------------------------------
    /**
     * @brief   バトルで使用するポケットデータを作成して出力
     * @note
     *    これを実行したら必ずDeletePocketData()を呼ぶ。
     *    そうしないとメモリリークを起こす
     */
    //------------------------------------------------------------------
    void OutputPocketData( ::app::bag::POCKET_DATA aOutputPocket[ BATTLE_BAG_POCKET_MAX ] );

    //  出力するバトルポケットを指定
    void OutputPocketData( ::app::bag::POCKET_DATA* pOutputPocketList, const item::BtlPocket btlPocketType );

    //------------------------------------------------------------------
    /**
     * @brief   バトルで使用するポケットデータ作成して出力したのを後処理する
     */
    //------------------------------------------------------------------
    void DeletePocketData( ::app::bag::POCKET_DATA aOutputPocket[ BATTLE_BAG_POCKET_MAX ] );

    //------------------------------------------------------------------
    /**
     * @brief   ポケモンのステータスリスト更新
     * @note    この処理は必ず毎ターンに一度のみにする。
     *          処理が重いので多用できない
     */
    //------------------------------------------------------------------
    const POKE_STATUS_DATA* UpdatePokeStatusList( const BTL_POKEPARAM* pPokeParam, const BtlvPos btlvPos );

    //------------------------------------------------------------------
    /**
     * @brief   味方ポケモン配置リスト取得
     */
    //------------------------------------------------------------------
    gfl2::util::List<BtlvPos>& GetSelfPokePosPlace() const { return *m_pSelfPosPlaceList; }

    //------------------------------------------------------------------
    /**
     * @brief   敵ポケモン配置リスト取得
     */
    //------------------------------------------------------------------
    gfl2::util::List<BtlvPos>& GetEnemyPokePosPlaceList() const { return *m_pEnemyPosPlaceList; }

    //------------------------------------------------------------------
    /**
     * @brief     わざからポケモン配置対象に対する効果タイプを取得
     * @param[out]  pPutFlg           : 表示可能
     * @param[in]   wazaNo            : 攻撃側が使用する技NO
     * @param[in]   attackerPokeId    : 攻撃側のポケモンid
     * @param[in]   rDefenderPokeList : 防御側のポケモンたちの位置
     * @return      相性値
     */
    //------------------------------------------------------------------
    BtlTypeAffAbout GetPokePosListKoukaType( bool* pPutFlg, const WazaNo wazaNo, const u32 attackerPokeId, gfl2::util::List<BtlvPos>& rDefenderPokeList );

    //------------------------------------------------------------------
    /**
     * @brief   わざからポケモンタイプに対する効果タイプを取得(自分自身のを取得)
     * @param[out]  pPutFlg           : 表示可能
     * @param[in]   wazaNo            : 攻撃側が使用する技NO
     * @param[in]   attackerPokeId    : 攻撃側のポケモンid
     * @return      相性値
     */
    //------------------------------------------------------------------
    BtlTypeAffAbout GetPokePosListKoukaType( bool* pPutFlg, const WazaNo wazaNo, const u32 attackerPokeId, const u32 defPokeId );

    //------------------------------------------------------------------
    /**
     * @brief   アイテム管理インスタンス作成
     */
    //------------------------------------------------------------------
    itemman::ITEM_MANAGER* CreateInstanceItemManager();

    //------------------------------------------------------------------
    /**
     * @brief   アイテム管理インスタンス破棄
     */
    //------------------------------------------------------------------
    void DeleteInstanceItemManager();

    //------------------------------------------------------------------
    /**
     * @brief   アイテム管理インスタンス取得
     */
    //------------------------------------------------------------------
    itemman::ITEM_MANAGER* GetInstanceItemManager() const { return m_pItemManager; }

    //------------------------------------------------------------------
    /**
     * @brief   アイテムテキスト管理インスタンス取得
     */
    //------------------------------------------------------------------
    print::ItemMessage* GetInstanceItemMessage() const { return m_pItemMessage; }

    //------------------------------------------------------------------
    /**
     * @brief   プレイヤーデータ取得
     */
    //------------------------------------------------------------------
    const Savedata::MyStatus* GetMyStatus(void) const;

    //------------------------------------------------------------------
    /**
     * @brief   ターン内でのユニークボタン入力フラグをクリア
     */
    //------------------------------------------------------------------
    void ClearUniquPushBtnFlg( const s32 pokePartyListIdx );
    void ClearAllUniquPushBtnFlg();
    
    //------------------------------------------------------------------
    /**
     * @brief   指定したpoke_idxがメガ進化できるかどうか
     */
    //------------------------------------------------------------------
    bool IsMegaPoke( const s32 self_party_poke_idx );

    //------------------------------------------------------------------
    /**
     * @brief   メガ進化ボタンを押しているかどうかの設定
     */
    //------------------------------------------------------------------
    void PushMegaBtn( const s32 pokePartyListIdx );

    //------------------------------------------------------------------
    /**
    * @brief   指定したpoke_idxがウルトラバーストできるかどうか
    */
    //------------------------------------------------------------------
    bool IsUltraBurstPoke(const s32 self_party_poke_idx);

    //------------------------------------------------------------------
    /**
    * @brief   ウルトラバーストボタンを押しているかどうかの設定
    */
    //------------------------------------------------------------------
    void PushUltraBurstBtn(const s32 pokePartyListIdx);

    //------------------------------------------------------------------
    /**
     * @brief   指定したデータから全力技ができるかどうか
     */
    //------------------------------------------------------------------
    bool IsZenryoku( const s32 pokePartyListIdx, const BTL_POKEPARAM* pBtlParam, const btl::ZenryokuWazaStatus*  pAuraWazaStatus );

    //------------------------------------------------------------------
    /**
     * @brief   全力技ボタンを押しているかどうかの設定
     */
    //------------------------------------------------------------------
    void PushZenryokuBtn( const s32 pokePartyListIdx );

    //------------------------------------------------------------------
    /**
     * @brief   技選択リストでわるあがきが、あるか
     */
    //------------------------------------------------------------------
    bool GetWazaSelectIdxByWaruagaki( const s32 self_party_poke_idx );

    //------------------------------------------------------------------
    /**
     * @brief   バトルUI画面で使用するメッセージデータ取得
     */
    //------------------------------------------------------------------
    gfl2::str::MsgData* GetAppMesssageData() const { return m_pAppMsgData; }

    //------------------------------------------------------------------
    /**
     * @brief   バトルUI画面で使用するわざ説明メッセージデータ取得
     */
    //------------------------------------------------------------------
    gfl2::str::MsgData* GetWazaInfoMesssageData() const { return m_pWzaaInfoMsgData; }

    //------------------------------------------------------------------
    /**
     * @brief   バトルUI画面で使用するステータスメッセージデータ取得
     */
    //------------------------------------------------------------------
    gfl2::str::MsgData* GetStatusMessageData() const { return m_pStatusMsgData; }

    //------------------------------------------------------------------
    /**
     * @brief     バトルUI画面で使用する特性のタイトルとインフォを文字列出力
     * @param[out]  pOutputTitleStrBuf  特性名のテキスト出力ポインタ
     * @param[out]  pOutputInfoStrBuf   特性の詳細テキスト出力ポインタ
     * @param[in]   tokuseiId           テキスト出力対象の特性id
     */
    //------------------------------------------------------------------
    void OutputTokuseiStrBuf( gfl2::str::StrBuf* pOutputTitleStrBuf, gfl2::str::StrBuf* pOutputInfoStrBuf, const u32 tokuseiId );

    //------------------------------------------------------------------
    /**
     * @brief   バトルUI画面で使用するアイテムのタイトルとインフォを文字列出力
     * @param[out]  pOutputTitleStrBuf  アイテム名のテキスト出力ポインタ
     * @param[out]  pOutputInfoStrBuf   アイテムの詳細テキスト出力ポインタ
     * @param[in]   itemId              テキスト出力対象のアイテムid
     */
    //------------------------------------------------------------------
    void OutputItemStrBuf( gfl2::str::StrBuf* pOutputTitleStrBuf, gfl2::str::StrBuf* pOutputInfoStrBuf, const u32 itemId );

    //------------------------------------------------------------------
    /**
     * @brief   バトルUI画面で使用する状態のタイトルとインフォを文字列出力
     * @param[out]  pOutputTitleStrBuf  状態名のテキスト出力ポインタ
     * @param[out]  pOutputInfoStrBuf   状態の詳細テキスト出力ポインタ
     * @param[in]   titleMsgId          状態のタイトルテキストid
     * @param[in]   infoMsgId           状態のインフォテキストid
     * @param[in]   pPokeParam          現在対象のポケモンパラメータ
     */
    //------------------------------------------------------------------
    void OutputStatusStrBuf( gfl2::str::StrBuf* pOutputTitleStrBuf, gfl2::str::StrBuf* pOutputInfoStrBuf, const u32 titleMsgId, const u32 infoMsgId, const BTL_POKEPARAM* pPokeParam );

    //------------------------------------------------------------------
    /**
      * @brief      通信対戦トレーナー数を取得
      * @return     登場しているトレーナ数を取得
      */
    //------------------------------------------------------------------
    u32 GetEnemyTrainerNum();

    //------------------------------------------------------------------
    /**
      * @brief      通信対戦トレーナー情報を取得
      * @param[out] pOutputParam: パラメータ出力用のアドレス
      * @param[in]  idx: 対戦情報idx
      */
    //------------------------------------------------------------------
    void  OutputEnemyTrainerParam( ENEMY_TRAINER_PARAM* pOutputParam, const u32 idx );

    //------------------------------------------------------------------
    /**
     * @brief   バトルUI画面用のワードセット
     */
    //------------------------------------------------------------------
    print::WordSet* GetWordSet() { return m_pWordSet; }

    //------------------------------------------------------------------
    /**
     * @brief     バトルUI画面用の文字列バッファ
     * @param[in] tempIdx( 0 - 1 まで保障)
     * @note
     *          このクラス内では文字列バッファは作成していない
     *          BattleViewSystemクラスに作業用文字列バッファを取得している
     *          UI処理中はUI処理のみが動いていて使ってないと考え問題ないと判断
     */
    //------------------------------------------------------------------
    gfl2::str::StrBuf*  GetTempStrBuf( const u32 tempIdx )
    {
      GFL_ASSERT( tempIdx < 2 );
      return m_btlv->GetWorkStr( (s32)tempIdx );
    }

    //------------------------------------------------------------------
    /**
     * @brief   バトル用で使うカーソルリソースバッファ取得
     */
    //------------------------------------------------------------------
    void* GetMenuCusrorLytResourceBuffer() { return m_pMenuCursorLytBuffer; }

    //------------------------------------------------------------------
    /**
      * @brief  所持しているモンスターボールリストの中の先頭のidを取得
      * @return 所持しているモンスターボールリストの先頭のアイテムid / 所持していない場合は Savedata::MyItem::ITEM_NONE を返す
      */
    //------------------------------------------------------------------
    u32 GetHaveFirstListMonsterBallId();

    //------------------------------------------------------------------
    /**
      * @brief  汎用アイコングラフィックデータ取得
      * @return グラフィックデータのインスタンス
      */
    //------------------------------------------------------------------
    ::app::tool::AppCommonGrpIconData& GetAppCommonGrpIconData() { return m_appCommonGrpIconData; }

    //------------------------------------------------------------------
    /**
      * @brief  インフォのウィンドウView取得
      * @return インフォのウィンドウViewのインスタンス
      */
    //------------------------------------------------------------------
    ::app::tool::InfoWindowSheetType* GetInfoWindowView() { return m_pInfoWindowView; }

    //------------------------------------------------------------------
    /**
      * @brief  バトル再生時のView取得
      * @return Viewのインスタンス
      */
    //------------------------------------------------------------------
    BattleUIRecPlayLowerView* GetRecPlayLowerView() const { return m_pRecPlayLowerView; }

    //------------------------------------------------------------------
    /**
      * @brief  トップ画面のカーソル記憶取得
      */
    //------------------------------------------------------------------
    u32 GetTopActViewCursorMemory() const { return m_topActCursorType; }

    //------------------------------------------------------------------
    /**
      * @brief  トップ画面のカーソル記憶
      */
    //------------------------------------------------------------------
    void SetTopActViewCursorMemory( const u32 cursorIdx )
    {
      m_topActCursorType  = cursorIdx;
    }

    //------------------------------------------------------------------
    /**
      * @brief  ポケモンのカーソル記憶取得
      */
    //------------------------------------------------------------------
    PokeCursorMemory& GetPokeCursorMemory( const u32 pokeIdx );

    //------------------------------------------------------------------
    /**
      * @brief  ポケモンのカーソル記憶可能個数
      */
    //------------------------------------------------------------------
    u32 GetPokeCursorMemoryLength() const { return GFL_NELEMS( m_aPokeCursorMemory ); }
    
    //------------------------------------------------------------------
    /**
      * @brief  ポケモンのカーソルリセット
      */
    //------------------------------------------------------------------
    void ResetPokeCursorMemory( const u32 pokeIdx );

    //------------------------------------------------------------------
    /**
      * @brief  バッグを使用できるか
      */
    //------------------------------------------------------------------
    bool IsUseBag();

    //------------------------------------------------------------------
    /**
      * @brief  ショートカットボタンで使用したボールアイテムidを設定
      */
    //------------------------------------------------------------------
    void SetLastShortcutUseBallItemId( const u16 itemId );

    //------------------------------------------------------------------
    /**
      * @brief  ショートカットボタンで使用したボールアイテムidを取得
      */
    //------------------------------------------------------------------
    u16 GetLastShortcutUseBallItemId() const;

  private:
    enum
    {
      POCKET_DATA_MAX = 36
    };

    enum
    {
      FILE_CATCH_OPEN_BATTLE_UI = 0,
      FILE_CATCH_OPEN_BATTLE_POKE_LIST_UI,
      FILE_CATCH_OPEN_MAX
    };

    struct UniquePushBtnFlg
    {
      u8 bMega: 1;
      u8 bZenryoku: 1; 
      u8 bUltraBurst : 1;
    };

    //  アイテムデータからバトルポケットアイテムデータを更新
    void _UpdateBtlPocketItemData( ::app::bag::POCKET_DATA* pOutputPocketDataList, const item::BtlPocket btlPocketType, const u32 bagPocketType, Savedata::MyItem* pChkItemData );

    void _Clear();

    //  UIで共通で使用するパラメータアクセサー
    gfl2::str::MsgData*       m_pAppMsgData;
    gfl2::str::MsgData*       m_pWzaaInfoMsgData;
    gfl2::str::MsgData*       m_pStatusMsgData;
    gfl2::str::MsgData*       m_pTokuseInfoMsgData;
    gfl2::str::MsgData*       m_pItemInfoMsgData;

    int m_loadMsgCnt;

    //  クラス内部で使用するヒープ
    ::app::util::Heap m_appUtilHeap;

    //  アイコングラフィックデータ
    ::app::tool::AppCommonGrpIconData  m_appCommonGrpIconData;

    print::WordSet*           m_pWordSet;
    ::app::tool::MenuCursor*  m_pMenuCursor;
    void*                     m_pMenuCursorLytBuffer;
    itemman::ITEM_MANAGER*    m_pItemManager;

    //  バトル時の味方の配置リスト
    gfl2::util::List<BtlvPos>* m_pSelfPosPlaceList;

    //  バトル時の敵の配置リスト
    gfl2::util::List<BtlvPos>* m_pEnemyPosPlaceList;

    BattleViewSystem* m_btlv;

    //  アイテムテキスト管理
    print::ItemMessage*                   m_pItemMessage;

    //  インフォウィンドウView
    //  Viewは生成するが描画しない、使う側がこれを子Viewとして扱う
    //  ここで生成する理由はいくつもの場面で何度も使われるのでそのつど生成するのは無駄なのでここで生成
    //  infowindowは一つしか表示しないからできること
    ::app::tool::InfoWindowSheetType*     m_pInfoWindowView;

    //  バトル再生時に使用する下画面View(再生のみ作成)
    BattleUIRecPlayLowerView*             m_pRecPlayLowerView;

    //  メッセージロードを短縮するためのローダー
    //  用意するデータ下記
    //  アイテム、特性、わざ
    WordSetLoader*                        m_pWordSetLoader;

    //  戦闘中のステータスデータ保存( 登場するポケモンの最大数確保 )
    POKE_STATUS_DATA                      m_aStatusDataBuffer[ 32 ];

    //  ポケモンごとのカーソル記憶(自分が配置できるポケモンは最大で２体まで)
    PokeCursorMemory                      m_aPokeCursorMemory[ 2 ];

    //  行動選択のカーソル
    u32                                   m_topActCursorType;

    //  先読み用のファイル制御ステート
    //  ※もう使わないかも
    //    画面の終了演出中に次の画面のファイル先読みとして作成したが、終了演出がなくなったことで先読みできる隙間時間の箇所がなくなったから。
    ::app::util::AppUtilFileControlState* m_pCacthFileCtrlStateList[ FILE_CATCH_TYPE_MAX ];

    //  open/closeをあらかじめする(バトルUI)
    ::app::util::AppUtilFileControlState* m_pCatchFileOnepCloseList[ FILE_CATCH_OPEN_MAX ];

    //  処理準備のステップ
    u32 m_readyStep;

    //  ユニークボタン入力フラグ
    UniquePushBtnFlg  m_aUniquPushBtnList[ pml::PokeParty::MAX_MEMBERS ];

    //  このクラスが使用するヒープサイズ
    static const u32 ms_useSystemHeapSize;
    static const u32 ms_useDeviceHeapSize;
  };

GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(btl)

#endif //__BATTLE_UI_DATA_MANAGER_H__
