//======================================================================
/**
 * @file FieldEffectRideLight.cpp
 * @date 2015/09/11 19:27:32
 * @author miyachi_soichi
 * @brief エフェクト：ライド光
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <fs/include/gfl2_BinLinkerAccessor.h>
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectRideLight.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// model
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectRideLight
 *  @brief  エフェクト：ライド光
 */
/**
 *  @brief  コンストラクタ
 */
IEffectRideBase::IEffectRideBase( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
IEffectRideBase::~IEffectRideBase( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void IEffectRideBase::Initialize( SetupData &setupData )
{
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;

  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;

  m_PlayFrame = 0;
  m_IsAlive   = false;

  m_Seq = 0;

}

/**
 *  @brief  破棄
 */
bool IEffectRideBase::Terminate( void )
{
  // 再生停止
  if( m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
  GFL_SAFE_DELETE( m_pEffectHandle );
  if( m_pEffectSystem && m_pEffectHeap )
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap = NULL;
  }
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void IEffectRideBase::Delete( void )
{
}

/**
 *  @brief  開始
 */
void IEffectRideBase::CreateEffect( gfl2::math::Vector3 pos )
{
  // 再生終了しているなら削除
  if( m_pEffectHandle->GetEmitterSet()
  && !m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }

  // 再生可能状態か
  bool isAlive = true;
  if( !m_pEffectHandle->GetEmitterSet() )
  {
    isAlive = false;
  }
  else if( !m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    isAlive = false;
  }

  // 再生できる
  if( !isAlive )
  {
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, pos, 0, GetResourceID() );
  }

  m_PlayFrame = 0;
  m_IsAlive   = true;
  m_Seq       = 0;

}

/**
 *  @brief  終了
 */
void IEffectRideBase::DeleteEffect( void )
{
  // 再生終了しているなら削除
  if( m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }


  m_IsAlive = false;
  m_PlayFrame = 0;
  m_Seq       = 0;
}

/**
 *  @brief  更新処理
 */
void IEffectRideBase::updateNormal( void )
{
  enum
  {
    SEQ_UPDATE,
    SEQ_FADEOUT,
    SEQ_END,
  };


  switch( m_Seq )
  {
  case SEQ_UPDATE:
    {
      // 再生が終わったら自身を切る
      bool bAlive = true;
      if( !m_pEffectHandle->GetEmitterSet() ){ bAlive = false; }
      else if( !m_pEffectHandle->IsValid() ){ bAlive = false; } //@fix GFNMCat[1374] サメハダーでダッシュ中に陸地に降りようとしたら操作不能になった
      else if( !m_pEffectHandle->GetEmitterSet()->IsAlive()  ){ bAlive = false;}

      if( bAlive == false )
      {
        m_IsAlive = false;
        m_Seq = SEQ_END;
        /*
          m_pEffectHandle->GetEmitterSet()->Fade();
          m_Seq = SEQ_FADEOUT;
        */
      }
    }
    break;

  case SEQ_FADEOUT:
    {
      if( m_PlayFrame >= 30 )
      {
        m_IsAlive = false;
        m_Seq = SEQ_END;
      }
    }
    break;

  case SEQ_END:
    break;

  }

  if( m_Seq != SEQ_END )
  {
    m_PlayFrame ++;
  }
}


/**
 *  @brief 再生フレーム数を取得
 */
u32 IEffectRideBase::GetFrame() const
{
  return m_PlayFrame;
}


  /**
   *  @brief 生存の確認
   */
b32 IEffectRideBase::IsAlive() const
{
  return m_IsAlive;
}

/**
 *  @brief 陸用演出のエフェクトID取得
 */
u32 EffectRideApperLand::GetResourceID( void ) const 
{
  return EFFECT_RESOURCE_RIDE_APPER_LAND; 
}

/**
 *  @brief 海用演出のエフェクトID取得
 */
u32 EffectRideApperSea::GetResourceID( void ) const 
{
  return EFFECT_RESOURCE_RIDE_APPER_SEA; 
}

//----------------------------------------------------------------------------------

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
