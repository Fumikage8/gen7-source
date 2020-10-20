//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file EncountWork.cpp
 *  @brief エンカウント常駐パラメータ管理モジュール
 *  @author iwasawa  
 *  @date 2012.07.28
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.25
 */
//=============================================================================

#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"

// gamesys
#include "GameSys/include/GameData.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/FieldAttribute.h"

// dll
#include "GameSys/include/DllReadThread.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

//----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
EncountWork::EncountWork(void) :
  m_walkCount(0),
  m_intervalCount(0),
  m_move_f(0),
  m_safety_f(0),
  m_nowProb(1),
  m_oldAttr(ATTR_ERROR),
  m_fishingContinueCnt(0),
  m_pokeSet(),
  m_oldIsEncount( false),
  m_encountEffect(),
  m_pDllRead( NULL )
{
  Initialize();
}

//----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
EncountWork::~EncountWork()
{
  Terminate();
}

//----------------------------------------------------------------------------
/**
 *  @brief  初期化処理
 */
//-----------------------------------------------------------------------------
void EncountWork::Initialize( void )
{
  m_pokeSet.Clear();
  gfl2::std::MemClear( m_pRoModules, sizeof( m_pRoModules));

  GFL_ASSERT( m_pDllRead == NULL );
  this->SafeDeleteDllRead();
}

//----------------------------------------------------------------------------
/**
 *  @brief  破棄処理
 */
//-----------------------------------------------------------------------------
void EncountWork::Terminate( void )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  現在のエンカウント率取得
 */
//-----------------------------------------------------------------------------
u32 EncountWork::GetNowEncountProb( const ENCPROB_PATTERN* data ) const
{
  if( m_safety_f ){
    if( m_walkCount <= data->safety ) return 0;  //セーフティ内なのでエンカウントしない 
  } 
  return m_nowProb;
}

//----------------------------------------------------------------------------
/**
 *  @brief  現在のエンカウント率パラメータ計算
 *
 *  @param  attr           現在地のアトリビュート
 *  @param  after_battle_f 野生戦直後のリセット時はtrue
 */
//-----------------------------------------------------------------------------
void EncountWork::CalcEncountProb( const bool c_move_f, const bool c_ex_move_f, const ENCPROB_PATTERN* data )
{
  if( data == NULL ) return;  //パターンデータがない場所ではカウントアップはしない

  // @fix SBTS[530]:ダニエル状態でエンカウントしない、対処。ダニエル状態の動作終了時にエンカウント補正をアップさせる
  if( m_move_f || c_move_f || c_ex_move_f ){ //一歩移動済みか通常のグリッド移動を伴う移動の時にカウントアップ
    AddWalkCountAndProb( data );
    m_move_f = true;  //バトル後最初の一歩フラグセット
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  マップチェンジ後の初期状態セット(フィールドが構築されてから呼び出すこと)
 */
//-----------------------------------------------------------------------------
void EncountWork::MapChangeUpdate( GameSys::GameManager* p_gman )
{
  Attr attr = ATTR_ERROR;
  Fieldmap* pFieldmap = p_gman->GetFieldmap();
  if( pFieldmap != NULL ){
    attr = pFieldmap->GetPlayerCharacter()->GetGroundAttribute();
  }

  EncountProbReset( attr, m_oldIsEncount );
}

//----------------------------------------------------------------------------
/**
 *  @brief  フィールドバトル発生時(トレーナー・野生戦問わず)に呼び出す初期化
 */
//-----------------------------------------------------------------------------
void EncountWork::BattleCallUpdate( Attr attr )
{
  EncountProbReset( attr, m_oldIsEncount, true );
}

//----------------------------------------------------------------------------
/**
 *  @brief  現在のエンカウント率パラメータリセット
 *
 *  @param  attr           現在地のアトリビュート
 *  @param  after_battle_f 野生戦直後のリセット時はtrue
 */
//-----------------------------------------------------------------------------
void EncountWork::EncountProbReset( Attr attr, bool isEncount,  bool after_battle_f )
{
  m_walkCount = 0;
  m_intervalCount = 0;

  m_oldAttr = attr;
  m_oldIsEncount = isEncount;
  m_nowProb = 1;
  m_move_f = 0;
  m_safety_f = after_battle_f;  //一度戦闘後にセーフティは有効になる
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 釣りのクリア
 */
/* -------------------------------------------------------------------------*/
void EncountWork::ClearFishingContinueCount( void )
{
  m_fishingContinueCnt=0;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 釣りの連続回数加算
 *
 * @param add_num 加算する継続数
 */
/* -------------------------------------------------------------------------*/
void EncountWork::AddFishingContinueCount( u32 add_num )
{
  m_fishingContinueCnt+=add_num;
  if( m_fishingContinueCnt > FISHING_CONTINUE_MAX )
  {
    m_fishingContinueCnt = FISHING_CONTINUE_MAX;
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 釣りの連続回数取得
 */
/* -------------------------------------------------------------------------*/
u16 EncountWork::GetFishingContinueCount( void ) const
{
  return m_fishingContinueCnt;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief エンカウントポケモンセットの取得
 */
/* -------------------------------------------------------------------------*/
PokeSet& EncountWork::GetPokeSet( void )
{
  return m_pokeSet;
}

#if PM_DEBUG
/* -------------------------------------------------------------------------*/
/**
 * @brief 釣り継続数をセット
 */
/* -------------------------------------------------------------------------*/
void EncountWork::DEBUG_SetFishingContinueCount( u32 cnt )
{
  m_fishingContinueCnt=cnt;
  if( m_fishingContinueCnt > FISHING_CONTINUE_MAX )
  {
    m_fishingContinueCnt = FISHING_CONTINUE_MAX;
  }
}
#endif // PM_DEBUG

//----------------------------------------------------------------------------
/**
 *  @brief  エンカウント率リセットチェック
 *  @param  attr 現在地のアトリビュート
 *  @param  isEncount 現在のアトリビュートでエンカウントできるか
 */
//-----------------------------------------------------------------------------
void EncountWork::CheckProbReset( Attr attr, bool isEncount, u8 attrNearBGPattern )
{
  // m_oldAttrがATTR_ERRORならば、一旦リセットする。
  if( m_oldAttr == ATTR_ERROR )
  {
    //エンカウントフィールドと非エンカウントフィールドをまたいだので補正値クリア
    EncountProbReset( attr, isEncount );
    return;
  }

  if( isEncount != m_oldIsEncount)
  {
    //エンカウントフィールドと非エンカウントフィールドをまたいだので補正値クリア
    EncountProbReset( attr, isEncount );
    return;
  }

  if( attr == m_oldAttr )
  {
    return; //同じアトリビュートなので更新要らない
  }

  if( isEncount == false )
  {
    m_oldAttr = attr;
    m_oldIsEncount = isEncount; // 現在地が非エンカウントフィールドならリセットはせずにフラグの更新のみする
    return;
  }

  if( Attribute::GetBattleNearBGID( attr, attrNearBGPattern) == Attribute::GetBattleNearBGID( m_oldAttr, attrNearBGPattern) )
  {
    m_oldAttr = attr; //戦闘背景属性が同じならリセットはせずに現在地の更新のみする
  }
  else
  {
    //異なる属性フィールドに移動したのでリセット
    EncountProbReset( attr, isEncount );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  戦闘導入演出インスタンス取得
 */
//-----------------------------------------------------------------------------
EncountEffect* EncountWork::GetEncountEffect()
{
  return &m_encountEffect;
}

//-----------------------------------------------------------------------------
/**
 * @brief 戦闘導入演出で使用するDll読み込み
 * @param pHeap ヒープ
 */
//-----------------------------------------------------------------------------
void EncountWork::LoadEncountEffectDll( gfl2::heap::HeapBase* pHeap)
{
  gfl2::ro::RoManager*        pRoManager   = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager);
  gfl2::fs::AsyncFileManager* pFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();

  // Roモジュール
  const char* const RO_MODULES[ RO_MODULE_NUM ] =
  {
    //"SeqEditor.cro",//Object,Background,SeqEditorはmomijiで統合されました。
    "Object.cro",
    "SequencePlayer.cro", //バトル内でこれだけ破棄するので、必ず最後に。DEMO_MODULE_INDEXとも位置を合わせること！
  };

  for( u32 i = 0; i < RO_MODULE_NUM; ++i)
  {
    // フィールド～バトル間で保持する + バトルでも使用するのでDLLは後方確保
    m_pRoModules[i] = pRoManager->LoadModule( pFileManager, RO_MODULES[i], pHeap->GetLowerHandle());
    pRoManager->StartModule( m_pRoModules[i]);
  }

#if PM_DEBUG
  gfl2::heap::HeapBase * pDllLoad = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );
  GFL_RELEASE_PRINT( "write heap free size  when had loaded dll free[0x%x] allocatable[0x%x]\n", pDllLoad->GetTotalFreeSize(), pDllLoad->GetTotalAllocatableSize() );
#endif// PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @brief 戦闘導入演出で使用するDLL破棄
 */
//-----------------------------------------------------------------------------
void EncountWork::DisposeEncountEffectDll()
{
  gfl2::ro::RoManager* pRoManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager);

  for( u32 i = 0; i < RO_MODULE_NUM; ++ i)
  {
    if( m_pRoModules[i])
    {
      pRoManager->DisposeModule( m_pRoModules[i]);
      m_pRoModules[i] = NULL;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 戦闘導入演出で使用するDLLが読み込まれているか
 * @retval true 読み込まれている
 * @retval false 読み込まれていない
 */
//-----------------------------------------------------------------------------
bool EncountWork::IsLoadEncountEffectDll() const
{
#if GFL_RO_ENABLE

  for( u32 i = 0; i < RO_MODULE_NUM; ++ i)
  {
    if( m_pRoModules[i] != NULL)
    {
      return true;
    }
  }
  return false;

#else //GFL_RO_ENABLE

  return true;

#endif //GFL_RO_ENABLE
}

//-----------------------------------------------------------------------------
/**
  * @brief バトル内でデモ用のDLLを破棄する関数
  */
//-----------------------------------------------------------------------------
void EncountWork::DisposeSequenceDemoDll()
{
  gfl2::ro::RoManager* pRoManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager);

  if( m_pRoModules[DEMO_MODULE_INDEX])
  {
    pRoManager->DisposeModule( m_pRoModules[DEMO_MODULE_INDEX]);
    m_pRoModules[DEMO_MODULE_INDEX] = NULL;
  }

  if( m_pDllRead )
  {
    m_pDllRead->DisposeModule( DEMO_MODULE_INDEX );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 戦闘導入演出で使用するDll読み込み スレッド版
 * @param pHeap スレッド用ヒープ
 * @param pHeapDll DLL読み込み用ヒープ
 */
//-----------------------------------------------------------------------------
void EncountWork::StartLoadEncountEffectDllThread( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pHeapDll )
{
  gfl2::ro::RoManager*        pRoManager   = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager);
  gfl2::fs::AsyncFileManager* pFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();

  // Roモジュール
  const char* RO_MODULES[ RO_MODULE_NUM ] =
  {
    //"SeqEditor.cro",//Object,Background,SeqEditorはmomijiで統合されました。
    "Object.cro",
    "SequencePlayer.cro", //バトル内でこれだけ破棄するので、必ず最後に。DEMO_MODULE_INDEXとも位置を合わせること！
  };

  GFL_ASSERT( m_pDllRead == NULL );
  this->SafeDeleteDllRead();

  m_pDllRead = GFL_NEW( pHeap ) GameSys::DllReadThread( pHeap->GetLowerHandle(), 4096 );
  m_pDllRead->SetData( RO_MODULES, RO_MODULE_NUM, pHeapDll->GetLowerHandle() );
  m_pDllRead->Start( gfl2::thread::Thread::THREAD_PRI_NORMAL );

#if PM_DEBUG
  gfl2::heap::HeapBase * pDllLoad = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );
  GFL_RELEASE_PRINT( "write heap free size  when had loaded dll free[0x%x] allocatable[0x%x]\n", pDllLoad->GetTotalFreeSize(), pDllLoad->GetTotalAllocatableSize() );
#endif// PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @brief 戦闘導入演出で使用するDLLが読み込み待ち スレッド版
 * @retval true 完了
 * @retval false 実行中
 * @note 必ずStartLoadEncountEffectDllとセットで使用する
 */
//-----------------------------------------------------------------------------
bool EncountWork::WaitLoadEncountEffectDllThread() const
{
#if GFL_RO_ENABLE

  if( m_pDllRead )
  {
    return ( m_pDllRead->IsAlive() == false );
  }

  return false;

#else //GFL_RO_ENABLE

  return true;

#endif //GFL_RO_ENABLE
}

//-----------------------------------------------------------------------------
/**
 * @brief 戦闘導入演出で使用するDLL破棄 スレッド版
 */
//-----------------------------------------------------------------------------
void EncountWork::DisposeEncountEffectDllThread()
{
  this->SafeDeleteDllRead();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  歩数&エンカウント率上昇計算
 */
//-----------------------------------------------------------------------------
void EncountWork::AddWalkCountAndProb( const ENCPROB_PATTERN* data )
{
  /*
   * カウンタストップチェック
   * 過去に歩数カウントをエンカウント率補正に用いる際に、
   * 計算結果のオーバーフローが問題になったケースがあるので、いくつでカンストさせるかは調整
   */
  if( m_walkCount >= 0xFFFF ){
    return; //カンストしたら何もしない
  }
  ++m_walkCount;  //歩数加算(振り向き含む)
  if( m_safety_f && (m_walkCount <= data->safety) ){
    return; //セーフティが有効で範囲内ならエンカウント率上昇計算をしない
  }
  m_intervalCount += m_move_f;
  if( m_intervalCount < data->interval ){
    return; //インターバル内は上昇しない
  }
  m_intervalCount = 0;  //
  if( (m_nowProb + data->up ) > data->max ){
    m_nowProb = data->max;  //上限補正
  }else{
    m_nowProb += data->up;
  }
  IWASAWA_PRINT("EncPer int=%d, add=%d, n=%d\n", data->interval, data->up, m_nowProb );
  SAITA_PRINT("EncPer int=%d, add=%d, n=%d\n", data->interval, data->up, m_nowProb );
}

void EncountWork::SafeDeleteDllRead()
{
  if( m_pDllRead )
  {
    m_pDllRead->DisposeModule();
    GFL_DELETE( m_pDllRead );
    m_pDllRead = NULL;
  }
}

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )
