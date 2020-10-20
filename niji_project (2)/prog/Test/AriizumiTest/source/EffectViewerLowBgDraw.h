//======================================================================
/**
 * @file EffectViewerLowBgDraw.h
 * @date 2015/12/03 14:40:18
 * @author kijima_satoshi
 * @brief エフェクトビューワー用描画クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EFFECT_VIEWER_LOWBG_DRAW__
#define __EFFECT_VIEWER_LOWBG_DRAW__
#pragma once
#if PM_DEBUG

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

#include "AppLib/include/Util/AppRenderingManager.h"
#include "Test/AriizumiTest/include/EffectViewAppParam.h"
#include "EffectViewerLowBgListener.h"

#include "Print/include/WordSet.h"
#include "str/include/gfl2_Str.h"
#include "Battle/include/battle_def.h"



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )

struct BattleRuleData
{
  wchar_t rule_str[16];
  BtlRule rule_num;
};

struct WazaRangeData
{
  wchar_t range_str[16];
  int range_num;
};

struct MonsterBallData
{
  wchar_t ball_str[16];
  int ball_num;
};

class EffectViewerLowBgDraw:public app::ui::UIView, public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectViewerLowBgDraw);

 public:

  // -------------------------
  // Constructor & Destructor
  // -------------------------
  EffectViewerLowBgDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager );
  virtual ~EffectViewerLowBgDraw();


  // -------------------------
  // Methods
  // -------------------------
  void Init( EFFECT_APP_PARAM* pAppParam, void* pAppLytBuff );

  virtual void Update(void);
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  bool End(void);
  bool IsExit(void);  // 破棄チェック : trueで破棄完了

  void SetUIListener( EffectViewerLowBgListener *listener );


 private:

  // --------------------------------------------
  // 描画関連

  void Initialize2D( app::util::Heap *heap, void* pAppLytBuff );


  // --------------------------------------------
  // キー、タッチ入力関連


  // ボタンマネージャの生成
  void CreateEffectViewerButtonManager( void );

  /// ペインアクション通知用リスナ
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  /// キー入力通知用リスナ
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick);
  /// タッチパネルイベントの検知
  //  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );


  // --------------------------------------------
  // テキスト関連
  void InitializeText( app::util::Heap * pheap );
  void FinalizeText(void);


  // --------------------------------------------
  // UI操作関連
  void InitLayoutParam(void);

  void ChangeLayoutPage( int page );

  void StoreSelectPokemonParam (pml::pokepara::PokemonParam *poke_param);

  void SetTextBoxToPokeName( gfl2::lyt::LytTextBox *textBox, MonsNo no );
  void SetTextBoxToTrainerTypeName( gfl2::lyt::LytTextBox *textBox, int type );

  void SetTextBoxToNumber( gfl2::lyt::LytTextBox *textBox, int num );

  void UpdateLayoutDisp_PokemonList(void);
  void UpdateLayoutDisp_BattleRule( int rule_idx );
  void UpdateLayoutDisp_BattleRange( int range_idx );
  void UpdateLayoutDisp_SelectPokemon( MonsNo poke_num );
  void UpdateLayoutDisp_PokemonForm( int poke_form );
  void UpdateLayoutDisp_PokemonSex( int idx );
  void UpdateLayoutDisp_PokemonRare( bool flg );
  void UpdateLayoutDisp_ResourceNum( int num );
  void UpdateLayoutDisp_WazaName( u32 wazaNo );
  void UpdateLayoutDisp_WazaSubNumber( int sub_no );
  void UpdateLayoutDisp_AttackSide( int check_idx );
  void UpdateLayoutDisp_DeffenceSide( int check_idx );
  void UpdateLayoutDisp_BgmStat( bool st );
  void UpdateLayoutDisp_Frame( int frm );

  void UpdateLayoutDisp_Ball_ID( int slot, int ball_idx );
  void UpdateLayoutDisp_TrainerType( int tr_type );  // tmp

  void UpdateLayoutDisp_ALL(void);

  void ClickBgmPlay(void);
  void ClickPokemonRare(void);
  void ClickPokemonSex(void);

  void ClickDebugPrint(void);
  void SetDebugPrintStat(bool st);

  void SetAction_SetPokemon( int slot );
  void SetAction_PlayWaza(void);
  void SetAction_SetBattleRule(void);
  void SetAction_SetBall_ID(void);
  void SetAction_TargetSet(void);

  void CallKeyBoard( s32 msgId );


 private:

  // -------------------------
  // Members
  // -------------------------


  app::util::Heap* mpAppHeap;                                 // ヒープ
  gfl2::heap::HeapBase* mpTmpHeap;                            // DebugKeyboad用tmpヒープ
  app::util::AppRenderingManager *mpAppRenderingManager;      // Rendering Manager


  EffectViewerLowBgListener *mUiListener;                     //  UIイベントリスナー

  // テキスト関連
  static const int TEMP_STRBUF_LENGTH = 256;

  gfl2::str::MsgData *mpMsgData;                             // テストメッセージデータ
  gfl2::str::StrBuf  *mpTempStrBuf;
  gfl2::str::StrBuf  *mpTempStrBuf2;
  print::WordSet     *mpWordSet;


  // -------------------------
  // UIで操作するパラメーター
  // -------------------------

  bool mUI_IsDebugPrint;
  bool mUI_IsPlayBGM;
  int  mUI_ResourceNum;

  MonsNo mUI_SelectPokemonNum;
  int    mUI_SelectPokemonFormNum;
  bool   mUI_SelectPokemonRare;
  int    mUI_SelectPokemonSex;

  u32 mUI_SelectWazaNum;
  int mUI_SelectWazaSubNum;

  MonsNo mUI_PokemonList[EffectViewerUiInfo::POKEMON_MAX_NUM];

  int mUI_BattleRuleIdx;
  int mUI_BattleRangeIdx;

  int mUI_AttackPokeIdx;
  int mUI_DefencePokeIdx;

  int mUI_Ball_ID[2];
  int mUI_TrainerType;


  // UIのみ（上画面では使わない）
  int mUI_Page;
  gfl2::lyt::LytWk* mUILytWk;  // 参照回数が多そうなので


  // キーボード入力用
  bool mIsWaitKeyBoard;
  s32 mKeyBoardChoice;
  s32 mKeyboardMsgId;
  gfl2::str::StrBuf *mKeyBoardBuf;

  // 上画面へのアクション、パラメータ受け渡し用
  EffectViewerUiInfo *mpEffViewUiInfo;


};  // End of class

GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG

#endif // __EFFECT_VIEWER_LOWBG_DRAW__
