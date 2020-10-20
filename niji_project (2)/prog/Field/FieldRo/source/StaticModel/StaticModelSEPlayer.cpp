//===================================================================
/**
 * @file    StaticModelSEPlayer.cpp
 * @brief   配置モデルSE再生クラス
 * @author  saita_kazuki
 * @date    2015.06.25
 */
//===================================================================

#include "./StaticModelSEPlayer.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @brief コンストラクタ
 */
StaticModelSEPlayer::StaticModelSEPlayer() :
   m_pSETable( NULL)
{
}

/**
 * @brief デストラクタ
 */
StaticModelSEPlayer::~StaticModelSEPlayer()
{
}

/**
 * @brief 初期化
 * @param SEテーブルバイナリデータ
 */
void StaticModelSEPlayer::Initialize( void* pSETableData)
{
  m_pSETable = reinterpret_cast<Table*>( pSETableData);
}

/**
 * @brief 破棄
 */
void StaticModelSEPlayer::Terminate()
{
  m_pSETable = NULL;
}

/**
 * @brief SE再生
 * @param programID プログラム指定ID
 * @param index アニメーション番号
 * @param isInverse trueで逆再生
 */
void StaticModelSEPlayer::PlaySE( u8 programID, u32 index, bool isInverse)
{
  if( m_pSETable == NULL)
  {
    GFL_ASSERT( 0);
    return;
  }

  if( index >= EVENT_ANIMATION_INDEX_MAX)
  {
    GFL_ASSERT( 0);
    return;
  }

  // 一応チェック
  if( this->IsPlaySE( programID) == false)
  {
    return;
  }

  u32 id = this->GetSEID( programID, index, isInverse);
  // @fix NULLが設定されていたら再生しない
  if( id == SMID_NULL )
  {
    return;
  }
  Sound::PlaySE( id);
}

/**
 * @brief SE再生をするプログラムIDか
 * @param programID プログラム指定ID
 * @retval true SE再生する
 * @retval true SE再生しない
 */
bool StaticModelSEPlayer::IsPlaySE( u8 programID)
{
  if( programID != PROGRAM_ID_NONE)
  {
    return true;
  }
  return false;
}

/**
 * @brief SEのIDを取得
 * @param programID プログラム指定ID
 * @param index アニメーション番号
 * @param isInverse trueで逆再生
 */
u32 StaticModelSEPlayer::GetSEID( u8 programID, u32 index, bool isInverse)
{
  if( isInverse)
  {
    // 逆再生
    return m_pSETable[programID].SELabelID_Inverse[index];
  }
  else
  {
    // 順再生
    return m_pSETable[programID].SELabelID_Order[index];
  }
}

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )

