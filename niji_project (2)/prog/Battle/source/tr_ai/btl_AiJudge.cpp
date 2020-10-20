/**
 * @file btl_AiJudge.cpp
 * @brief 戦闘AI( 何らかの判定を行うクラスの抽象化 )
 * @author obata_toshihiro
 * @date 2015.04.07
 */

#include "btl_AiJudge.h"

GFL_NAMESPACE_BEGIN( btl )


//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param minScriptNo      実行対象のスクリプト番号の最小値
 * @param maxScriptNo      実行対象のスクリプト番号の最大値
 * @param targetScriptBit  実行するAIのビットフラグ
 */
//---------------------------------------------------------------------------
AiJudge::AiJudge( BtlAiScriptNo minScriptNo, BtlAiScriptNo maxScriptNo, u32 targetScriptBit ) : 
  m_minScriptNo( minScriptNo ),
  m_maxScriptNo( maxScriptNo ),
  m_currentScriptNo( BTL_AISCRIPT_NO_NULL ),
  m_targetScriptBit( targetScriptBit )
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
AiJudge::~AiJudge()
{
}

//---------------------------------------------------------------------------
/**
 * @brief スクリプトを実行するための一時ヒープをアタッチする
 * @param heap  スクリプトを実行するための一時ヒープ
 */
//---------------------------------------------------------------------------
void AiJudge::AttackHeapForScriptExec( HeapHandle heap )
{
  m_heapForScriptExec = heap;
}

//---------------------------------------------------------------------------
/**
 * @brief スクリプトを実行するための一時ヒープを取得する
 */
//---------------------------------------------------------------------------
HeapHandle AiJudge::GetHeapForScriptExec( void ) const
{
  return m_heapForScriptExec;
}

//---------------------------------------------------------------------------
/**
 * @brief 実行するＡＩのビットフラグを変更する
 * @param targetScriptBit  実行するAIのビットフラグ
 */
//---------------------------------------------------------------------------
void AiJudge::ChangeAiBit( u32 targetScriptBit )
{
  m_targetScriptBit = targetScriptBit;
}

//---------------------------------------------------------------------------
/**
 * @brief 実行すべきスクリプト番号を取得する
 */
//---------------------------------------------------------------------------
BtlAiScriptNo AiJudge::GetCurrentScriptNo( void ) const
{
  return m_currentScriptNo;
}

//---------------------------------------------------------------------------
/**
 * @brief 実行するスクリプト番号をリセットする
 */
//---------------------------------------------------------------------------
void AiJudge::ResetScriptNo( void )
{
  m_currentScriptNo = this->GetNextScriptNo( m_minScriptNo, m_maxScriptNo );
}

//---------------------------------------------------------------------------
/**
 * @brief 実行するスクリプト番号を更新する
 */
//---------------------------------------------------------------------------
void AiJudge::UpdateScriptNo( void )
{
  m_currentScriptNo = static_cast<BtlAiScriptNo>( m_currentScriptNo + 1 );
  m_currentScriptNo = this->GetNextScriptNo( m_currentScriptNo, m_maxScriptNo );
}

/**
 * @brief 次に実行すべきスクリプトの番号を取得する
 * @param minScriptNo     実行対象スクリプトNo.の最小値
 * @param maxScriptNo     実行対象スクリプトNo.の最大値
 * @retval BTL_AISCRIPT_NO_NULL  実行すべきスクリプトが存在しない場合
 */
BtlAiScriptNo AiJudge::GetNextScriptNo( 
  BtlAiScriptNo minScriptNo, 
  BtlAiScriptNo maxScriptNo ) const
{
  u32 scriptNo = static_cast<u32>( minScriptNo );
  while( scriptNo <= static_cast<u32>( maxScriptNo ) )
  {
    if( ( ( 1 << scriptNo ) & m_targetScriptBit ) != 0 ) {
      return static_cast<BtlAiScriptNo>( scriptNo );
    }
    ++scriptNo;
  }
  return BTL_AISCRIPT_NO_NULL;
}

//---------------------------------------------------------------------------
/**
 * @brief 全てのスクリプトを実行し終えたか？
 * @retval true    全てのスクリプトを実行し終えた
 * @retval false   実行していないスクリプトが存在する
 */
//---------------------------------------------------------------------------
bool AiJudge::IsAllScriptFinished( void ) const
{
  return ( m_maxScriptNo < m_currentScriptNo );
}



GFL_NAMESPACE_END( btl )