#if PM_DEBUG
//=============================================================================
/**
 * @file    DebugBattleProc.h
 * @brief   デバッグバトルProc
 * @author  obata_toshihiro
 * @data    2015.2.25
 * @note    sango_projectから移植
 */
//=============================================================================
#if !defined( DEBUG_BATTLE_PROC_H_INCLUDED )
#define DEBUG_BATTLE_PROC_H_INCLUDED 

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <System/include/nijiAllocator.h>
#include <System/include/AppRenderingPipeLine.h>
#include <Savedata/include/ConfigSave.h>
#include <Print/include/WordSet.h>
#include <GameSys/include/GameProc.h>
#include <Battle/include/battle_SetupParam.h>
#include <Debug/DebugBattle/source/DebugBattleCommon.h>
#include <Debug/DebugBattle/source/DebugBattleSaveData.h>
#include <Debug/DebugPokeMake/include/DebugPokeMake.h>

#ifdef  GF_PLATFORM_CTR
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#endif


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(battle)


/**
 * @class DebugBattleProc
 * @brief デバッグバトルのプロセス
 */
class DebugBattleProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( DebugBattleProc );

public:
  DebugBattleProc( void );
  ~DebugBattleProc();

  //---------------------------------------------------------------------------
  /**
   * @brief パラメータセットアップ
   * @brief heapSys  使用するシステムヒープ
   * @brief heapDev  使用するデバイスヒープ
   */
  //---------------------------------------------------------------------------
  void SetupParam( gfl2::heap::HeapBase* heapSys, gfl2::heap::HeapBase* heapDev );

  //---------------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //---------------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //---------------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //---------------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //---------------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //---------------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  //---------------------------------------------------------------------------
  /**
   * @brief	PROCの描画処理
   * @param pManager   プロセス管理マネージャー
   * @param displayNo  描画対象画面
   * 登録したPROCによる、フレームバッファへの描画を実行します。
   * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
   */
  //---------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );


  

private:

  gfl2::heap::HeapBase*        m_sysHeapBase;
  gfl2::heap::HeapBase*        m_devHeapBase;
  gfl2::heap::HeapBase*        m_sysHeap;
  gfl2::heap::HeapBase*        m_devHeap;
  u32                          m_devHeapSize;
  gfl2::heap::NwAllocator*     m_sysAllocator;
  gfl2::heap::NwAllocator*     m_devAllocator;
  gfl2::str::MsgData*          m_strData;
  gfl2::str::StrBuf*           m_strBuf;
  gfl2::str::StrBuf*           m_strBufTmp;
  print::WordSet*              m_wordSet;
  AppRenderingPipeLine*        m_renderingPipeLine;
  gfl2::lyt::LytSys*           m_lytSys;
  gfl2::lyt::LytWk*            m_lytWk;
  gfl2::lyt::LytMultiResID     m_lytResID;
  void*                        m_lytDataBuffer;
  gfl2::ui::DeviceManager*     m_uiMan;
  gfl2::ui::VectorDevice*      m_uiKey;
  gfl2::ui::Button*            m_uiButton;
  gfl2::ui::TouchPanel*        m_uiTp;

  pml::pokepara::PokemonParam* m_pokeData;
  pml::pokepara::PokemonParam* m_clipPokeData;
  pml::pokepara::PokemonParam* m_tmpPokeData;
  pml::pokepara::PokemonParam* m_intrudePokeData;
  pml::pokepara::PokemonParam* m_reinforcePokeData[ MAX_REINFORCE_POKE_NUM ];
  pml::PokeParty*              m_party[ PARTY_MAX ];
  DefaultPowerUpDesc           m_defaultPowerUpDesc;
  DefaultPowerUpDesc           m_clipDefaultPowerUpDesc;
  MiseaiData                   m_miseaiData[ BTL_CLIENT_NUM ];
  GameSys::GameData*           m_gameData;
  pml::PokeParty*              m_playerParty;
  DebugBattleSaveData*         m_saveData;
#ifdef  GF_PLATFORM_CTR
  gflnet2::p2p::NetGame*       m_netHandle;
  gflnet2::p2p::NetGame*       m_netGame;
  int mDisconnect;

  //print::XYTagProc   m_tagProc;
#endif

  debug::PokeMake::APP_PARAM m_debugPokeParam;
  BATTLE_SETUP_PARAM          m_btlSetupParam;
  BTL_FIELD_SITUATION         m_btlFieldSit;

  u32   m_seq;
  u32   m_sub_seq;
  u32   m_page;
  DBItemID   m_pointItem;
  u32   m_modifyPokeIndex;
  DBItemID  m_modifyPokeItemNo;
  u16   m_currentItemType;
  u16   m_currentItemArg;
  u16   m_currentItemStrID;
  bool  m_isDrawEnable;

  Savedata::ConfigSave::MSG_SPEED         m_prevsave_msgSpeed;
  Savedata::ConfigSave::BATTLE_ANIMATION  m_prevsave_effectOn;
  Savedata::ConfigSave::BATTLE_RULE       m_prevsave_irekaeRule;
  void* m_prevsave_misc;

  bool  m_isRecordDataLoaded;
  bool  m_bLoaded;
  int   m_itemValue[ ITEM_MAX ];

  typedef struct {
    u16  page;
    u16  cur;
    DBItemID l, r, u, d;
    u8   itemType;
    u16  itemArg;
    u16  strStartIdx;
  }ITEM_PARAM;

  typedef struct {
    u16 page;
    u16 paneID;
    u16 strID;
  }LABEL_PARAM;

  typedef struct {
    u16      lytDatIdx;
    u16      lytSelPaneIdx;
    DBItemID defaultItem;
  }PAGE_PARAM;


  static const ITEM_PARAM ItemTable[];
  static const LABEL_PARAM LabelTable[];
  static const PAGE_PARAM PageParamTable[];

  void CreateHeap( void );
  void CreateDeviceHeap( void );
  void DeleteHeap( void );
  void DeleteDeviceHeap( void );

  void SetupUiSystem( void );
  void SetupGameData( void );
  void SetupPokemonContainer( void );

  void  initialize( void );
  void  initGraphicSystem( void );
  void  quitGraphicSystem( void );
  void  init2D( void );
  void  quit2D( void );

  void LoadAllItems( void );

  void CallPokeMakeProc( gfl2::proc::Manager* procManager );
  void StoreMakePokeWithSaveData();
  void CallBattleProc( gfl2::proc::Manager* procManager );

  /**
   *  keyCtrl の結果
   */
  enum CtrlResult {
    CTRL_RESULT_NONE,          // とくに動きなし
    CTRL_RESULT_QUIT,          // デバッグバトル画面終了へ
    CTRL_RESULT_PAGE_PREV,     // ページ切り替え
    CTRL_RESULT_PAGE_NEXT,     // ページ切り替え
    CTRL_RESULT_BATTLE,        // バトル画面へ
    CTRL_RESULT_POKE_CREATE,   // ポケモン作成画面へ
    CTRL_RESULT_POKE_CUT,
    CTRL_RESULT_POKE_COPY,
    CTRL_RESULT_POKE_PASTE,
    CTRL_RESULT_SAVE,
    CTRL_RESULT_LOAD,
    CTRL_RESULT_LOAD_AND_RUN,
    CTRL_RESULT_RESET_LANG,
  };
  CtrlResult  keyControl( void );
  bool  checkKeyForItemSelect( void );
  CtrlResult  checkKeyForItemCtrl( void );
  void  redrawCurrentPageItems( void );
  void  redrawItem( DBItemID item );
  void  loadItem( DBItemID item );
  void  saveItem( DBItemID item );

  gfl2::lyt::LytWk*  setupLyt( gfl2::lyt::LytMultiResID lytResID, u32 lytDatID );
  
  BtlCompetitor  setupBtlSetupParam( void );
  void cleanupBtlSetupParam( void );
  void modeToParams( BtlMode mode, BtlCompetitor* competitor, BtlRule* rule, BtlMultiMode* multiMode, bool* bComm );
  void setupParty( pml::PokeParty* pDst, u32 idx, u32 memberMax=0 );
  void setupDefaultPowerUpDesc( BATTLE_SETUP_PARAM* setupParam, u32 tr_idx );
  void setupMiseaiData( BATTLE_SETUP_PARAM* setupParam );
  bool  savingProc( u32* seq );
  bool  loadingProc( u32* seq, bool* bSucceed );
  void  setupCommonParams( BtlMode mode, BtlCompetitor competitor, BtlRule rule );
  u32  calc_ai_bit( void ) const;
  void clipDataCtrl( CtrlResult result );
  void set_modifypoke_params( void );
  bool isBattleCommMode( void );
  void resetLanguage( void );
  void resetPokeLanguage( pml::pokepara::PokemonParam* poke );

  int  getCommConnectPlayerNum( void );

  void recoverGraphicSystems( void );
  void backupBattleRecordData( u32 idx );
  bool loadRecordData( BATTLE_SETUP_PARAM* dst, u32 idx );
  void  setup_sub_wild( BtlRule rule );
  void  setup_sub_standAloneTr( BtlRule rule, BtlMode mode, BtlCompetitor competitor );
  void  setup_sub_commTr( BtlRule rule, BtlMode mode, BtlCompetitor competitor, BtlMultiMode multiMode );

  void StartBattleBgm( BtlCompetitor competitor );
  void EndBattleBgm();
  void StartFadeIn();
  bool IsFadeFinished( void ) const;
  void ChangePage( s32 pageOffset );


  // ここからView関連

  DBItemID m_currentItemId;
  void ChangeSelectItem( DBItemID nextItem );
  void resetSelectPane( DBItemID itemId );
  void setSelectPane( DBItemID itemId );
  gfl2::lyt::LytTextBox* GetTextBoxPane( DBItemID itemId );
  void SetPokeName( DBItemID itemId, pml::pokepara::PokemonParam* pokePara );
  void redrawClipPoke( pml::pokepara::PokemonParam* pokePara );
  void redrawPokeNamePane( gfl2::lyt::LytTextBox* pPane, pml::pokepara::PokemonParam* pokePara );
  void CreateRenderingPipeLine( gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap );
  void DeleteRenderingPipeLine( void );


};  // class DebugBattleProc


GFL_NAMESPACE_END(battle)
GFL_NAMESPACE_END(debug)

#endif // DEBUG_BATTLE_PROC_H_INCLUDED
#endif // PM_DEBUG
