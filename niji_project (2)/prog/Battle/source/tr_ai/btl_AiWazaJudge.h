//============================================================================================
/**
 * @file  btl_AiWazaJudge.h
 * @brief 戦闘AI 選択肢「技」の採点
 * @author  taya
 * @date  11.10.18
 */
//============================================================================================
#ifndef BTL_AI_WAZA_JUDGE_H_INCLUDED
#define BTL_AI_WAZA_JUDGE_H_INCLUDED

#include "../btl_common.h"
#include "./btl_AiJudge.h"

GFL_NAMESPACE_BEGIN(btl)


//-----------------------------------
// 必要クラス前方参照
//-----------------------------------
class MainModule;
class ServerFlow;
class POKECON;
class BTL_POKEPARAM;
class AiScript;
class AiScriptHandler;
class AiScriptCommandHandler;

//---------------------------------------------------------------------------
/**
 *  AI制御クラス
 */
//---------------------------------------------------------------------------
class AiWazaJudge : public AiJudge
{
  GFL_FORBID_COPY_AND_ASSIGN( AiWazaJudge );

public:
  //-----------------------------------
  // ctor / dtor
  //-----------------------------------
  AiWazaJudge( 
    HeapHandle heapHandle, 
    AiScript* aiScript, 
    const MainModule* mainModule, 
    ServerFlow* svfWork, 
    const POKECON* pokeCon, 
    const u32 randSeed,
    u32 ai_bit );
  virtual ~AiWazaJudge();


  void SetJudgeParam( const bool* usableWazaFlags, BtlPokePos myPos, u8 myID, u16 itemId );
  void StartJudge( void );
  virtual void UpdateJudge( void );
  virtual bool IsJudgeFinished( void ) const;
  bool IsEnemyEscape( void ) const;
  bool IsWazaSelected( void ) const;
  s32 GetSelectedWazaScore( void ) const;
  void GetSelectedWaza( u8* wazaIdx, BtlPokePos* targetPos ) const;



public:

  enum ScoreStatus {
    STATUS_NORMAL,      ///< 通常通り計算されたスコア
    STATUS_DISABLE,     ///< このルールでは存在しない位置IDなどで使用してはいけない
    STATUS_DISCOURAGE,  ///< 対象がひん死などで使用が推奨されない
  };


  const MainModule*     m_mainModule;
  ServerFlow*           m_svfWork;
  const POKECON*        m_pokeCon;
  const BTL_POKEPARAM*  m_atkPoke;
  const BTL_POKEPARAM*  m_defPoke;

  AIRandSys* m_randGenerator;
  AiScript* m_script;
  AiScriptHandler* m_scriptHandler;
  AiScriptCommandHandler* m_scriptCommandHandler;

  u8                 m_pokeID;
  u8                 m_atkClientID;
  BtlPokePos         m_atkPos;
  BtlPokePos         m_defPos;
  u8                 m_currentWazaPos;
  WazaNo             m_currentWazaNo;
  u16                m_itemId;

  u32                m_AIStep;
  int                m_wazaScore[ BTL_POS_NUM ][ PTL_WAZA_MAX ];
  ScoreStatus        m_wazaScoreStatus[ BTL_POS_NUM ][ PTL_WAZA_MAX ];
  bool               m_usableWazaFlags[ PTL_WAZA_MAX ]; // こだわり・ふういん等で使えないワザ
  bool               m_bTokuseiAppeared[ BTL_POKEID_MAX ];
  s32                m_selectWazaScore;
  u8                 m_selectWazaPos;
  BtlPokePos         m_selectTargetPos;
  u8                 m_currentTargetIdx;
  u8                 m_bTargetSideFriend;
  bool               m_bEscape;
  bool               m_bDecided;
  bool               m_bFinished;

  

private:
  enum {
    PRINT_FLAG = gfl2::debug::PRINT_FLAG_LEVEL2 | (gfl2::debug::PRINT_FLAG_USER << 1),
  };

  

  void  wazaScore_Reset( void );
  int   wazaScore_Add( u8 wazaIdx, BtlPokePos targetPos, int score );
  void  wazaScore_SetScoreless( u8 wazaIdx, BtlPokePos targetPos );
  bool  wazaScore_IsScoreless( u8 wazaIdx, BtlPokePos targetPos );
  void  wazaScore_DecideBest( void );
  
  const BTL_POKEPARAM*  decideTargetPoke( BtlPokePos target_pos );
  bool  mainProc_Single( void );
  void  subProc_Core( void );
  bool    incrementTargetIndex( void );
  BtlPokePos  updateTargetPos( bool bFriendSide, u8 targetIdx ) const;
  bool        isTargettingCoveragePos( WazaNo waza_no, BtlPokePos targetPos );
  BtlPokePos  correctTargetPos( BtlPokePos targetPos, BtlPokePos atkPos, const BTL_POKEPARAM* atkPoke, u8 wazaIdx ) const;
  BtlPokePos  searchBestScorePos( u8 wazaIdx, BtlPokePos atkPos, BtlSide side ) const;

};  // class AiWazaJudge

GFL_NAMESPACE_END(btl)

#endif // BTL_AI_WAZA_JUDGE_H_INCLUDED
