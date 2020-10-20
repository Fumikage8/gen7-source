//==============================================================================
/**
 * @file	P2pTimeout.cpp
 * @brief	P2Pタイムアウト管理
 * @author	ichiraku_katsuhiko
 * @date	2015/05/08 12:45:09
 */
// =============================================================================

// mine
#include "P2pTimeout.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)

  //------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------
void P2pTimeout::Initalize( int time )
{
  m_Sec = time;
  m_isUpdateEnable = false;
}

//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
P2pTimeout::P2pTimeout( void )
{
  m_listener = NULL;
  Initalize( DEFAULT_TIMEOUT );
}

//------------------------------------------------------------------
/**
 * @brief   エラーリスナー登録
 * @param   listener   通知を受け取るIrAdapterErrorListener
 */
//------------------------------------------------------------------
void P2pTimeout::RegistErrorListener( P2pTimeoutListener* listener )
{
  m_listener = listener;
}

//------------------------------------------------------------------
/**
 * @brief   エラーリスナー解除
 */
//------------------------------------------------------------------
void P2pTimeout::UnregistErrorListener( void )
{
  m_listener = NULL;
}

//------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------
void P2pTimeout::Update( void )
{
  if( m_isUpdateEnable == false )
  { // 更新しない
    return;
  }

#if defined(GF_PLATFORM_CTR)
  nn::fnd::TimeSpan time = nn::fnd::DateTime::GetNow() - m_Now;
  if( time.GetSeconds() >= m_Sec ) 
  {
    if( m_listener )
    {
      m_listener->OnP2pTimeoutError();
    }
    m_isUpdateEnable = false;  //もう更新しない
  }
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
 * @brief   停止
 */
//------------------------------------------------------------------
void P2pTimeout::Stop( void )
{
  m_isUpdateEnable = false;
}

//------------------------------------------------------------------
/**
 * @brief   スタート
 * @param  time 秒
 */
//------------------------------------------------------------------
void P2pTimeout::Start( int time )
{
#if defined(GF_PLATFORM_CTR)
  Initalize( time );
  m_Now = nn::fnd::DateTime::GetNow();
  m_isUpdateEnable = true;
#endif // GF_PLATFORM_CTR
}


GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

