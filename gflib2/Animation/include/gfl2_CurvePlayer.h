#ifndef GFLIB2_CURVE_PLAYER_H_INCLUDED
#define GFLIB2_CURVE_PLAYER_H_INCLUDED

#include <types/include/gfl2_Typedef.h>

namespace gfl2 { namespace animation {

// エルミートキー：u16版
struct CurveKeyHermiteU16
{
  u16 value;
  u16 slope;

public:

  f32 GetValue(f32 scale, f32 offset) const
  {
    return CalcScaleOffset(this->value, scale, offset);
  }
  f32 GetSlope(f32 scale, f32 offset) const
  {
    return CalcScaleOffset(this->slope, scale, offset);
  }

  static f32 CalcScaleOffset(u16 u16Value, f32 scale, f32 offset)
  {
    return (u16Value / 65535.0f) * scale + offset;
  }
};

// エルミートキー：f32版
struct CurveKeyHermiteF32
{
  f32 value;
  f32 slope;

public:

  f32 GetValue(f32 scale, f32 offset) const
  {
    (void)scale;
    (void)offset;

    return value;
  }
  f32 GetSlope(f32 scale, f32 offset) const
  {
    (void)scale;
    (void)offset;

    return slope;
  }
};

// 最後に再生されたキー情報：パラメータを保存しておきキー探索を減らす
struct CurveKeyInfo
{
  u16 index;
  u16 frame;
  f32 value;
  f32 slope;

  CurveKeyInfo() :
    index(0),
    frame(0),
    value(0),
    slope(0)
  {
  }

  void Set(u16 index, u16 frame, f32 value, f32 slope)
  {
    this->index = index;
    this->frame = frame;
    this->value = value;
    this->slope = slope;
  }
};

struct CurveLastKey
{
  CurveKeyInfo keyL;
  CurveKeyInfo keyR;

  f32 c0;
  f32 c1;

public:

  CurveLastKey() :
    c0(0),
    c1(0)
  {
  }

  void SetKeyL(u16 index, u16 frame, f32 value, f32 slope)
  {
    this->keyL.Set(index, frame, value, slope);
  }
  void SetKeyR(u16 index, u16 frame, f32 value, f32 slope)
  {
    this->keyR.Set(index, frame, value, slope);
  }
  void CopyKeyLtoR()
  {
    this->keyR = this->keyL;
  }
  void CopyKeyRtoL()
  {
    this->keyL = this->keyR;
  }

  void Cache()
  {
    f32 q = 1.0f / (keyR.frame - keyL.frame);
    f32 qq = q * q;
    f32 tmp0 = keyL.value - keyR.value;
    f32 tmp1 = keyL.slope + keyR.slope;

    c0 = qq * (q * 2 * tmp0 + tmp1);
    c1 = q * (q * -3 * tmp0 - tmp1 - keyL.slope);
  }

  // レンジ内か？
  s32 CheckRange(f32 frame) const
  {
    if (keyL.frame > frame)
    {
      return -1;
    }
    if (keyR.frame <= frame)
    {
      return 1;
    }

    return 0;
  }


  // 値取得
  f32 GetValue(f32 frame) const
  {
    f32 p = frame - keyL.frame;
    return keyL.value + p * (p * (p * c0 + c1) + keyL.slope);
  }
   
};

enum CurveKeyType
{
  CURVE_KEY_TYPE_BASEPOSE,
  CURVE_KEY_TYPE_STATIC_0,
  CURVE_KEY_TYPE_STATIC_1,
  CURVE_KEY_TYPE_ONE_FRAME,
  CURVE_KEY_TYPE_HERMITE_U16,
  CURVE_KEY_TYPE_HERMITE_F32,

  CURVE_KEY_TYPE_COUNT
};

// @attention 将来的に整理、最適化
class CurvePlayer
{
public:

  CurvePlayer() : 
    m_type(CURVE_KEY_TYPE_COUNT),
    m_isKeyU8(0),
    m_keyCount(0),
  	m_currentIndex(0),
  	m_valueScale(0),
  	m_valueOffset(0),
  	m_slopeScale(0),
  	m_slopeOffset(0),
    m_pFrameListU8_or_U16(NULL),
    m_pKeyListU16_or_F32(NULL),
    m_beginFrame(0),
    m_endFrame(0)
  {
  }

  void Reset()
  {
    m_type = CURVE_KEY_TYPE_COUNT;
  }

  bool HasData() const
  {
    return (m_type != CURVE_KEY_TYPE_COUNT);
  }

  void Initialize(uptr& addrData, u64 flags, u32 index, u32 maxFrame)
  {
    m_isKeyU8 = true;
    m_pFrameListU8_or_U16 = NULL;
    m_pKeyListU16_or_F32 = NULL;

    m_type = (CurveKeyType)(((flags) >> (index * 3)) & 0x7);
    switch (m_type)
    {
    case CURVE_KEY_TYPE_BASEPOSE:
      break;
    case CURVE_KEY_TYPE_STATIC_0:
      m_lastKey.keyL.value = 0.0f;
      break;
    case CURVE_KEY_TYPE_STATIC_1:
      m_lastKey.keyL.value = 1.0f;
      break;
    case CURVE_KEY_TYPE_ONE_FRAME:
      {
        m_lastKey.keyL.value = *reinterpret_cast<f32*>(addrData);
        addrData += sizeof(f32);
      }
      break;

    case CURVE_KEY_TYPE_HERMITE_U16:
      {
        this->ReadFrameList(addrData, maxFrame);

        m_valueScale = *reinterpret_cast<f32*>(addrData);
        addrData += sizeof(f32);
        m_valueOffset = *reinterpret_cast<f32*>(addrData);
        addrData += sizeof(f32);
        m_slopeScale = *reinterpret_cast<f32*>(addrData);
        addrData += sizeof(f32);
        m_slopeOffset = *reinterpret_cast<f32*>(addrData);
        addrData += sizeof(f32);

        this->ReadKeyList<CurveKeyHermiteU16>(addrData);
      }
      break;

    case CURVE_KEY_TYPE_HERMITE_F32:
      {
        this->ReadFrameList(addrData, maxFrame);
        this->ReadKeyList<CurveKeyHermiteF32>(addrData);
      }
      break;

    default:
      break;
    }
  }

  void ReadFrameList(uptr& addrData, u32 maxFrame)
  {
    m_keyCount = *reinterpret_cast<u32*>(addrData);
    addrData += sizeof(u32);

    if (maxFrame <= 255)
    {
      // １バイトキー
      m_pFrameListU8_or_U16 = reinterpret_cast<const void*>(addrData);
      m_isKeyU8 = true;
      addrData += m_keyCount;
    }
    else
    {
      // ２バイトキー
      m_pFrameListU8_or_U16 = reinterpret_cast<const void*>(addrData);
      m_isKeyU8 = false;
      addrData += m_keyCount * sizeof(u16);
    }

    // アラインメントチェック
    // パディングで0xFFが入っている可能性
    while ((addrData % 4 != 0) && ((*(u8*)addrData) == 0xFF))
    {
      addrData += sizeof(u8);
    }
  }

  template <typename KeyType>
  void ReadKeyList(uptr& addrData)
  {
    m_pKeyListU16_or_F32 = reinterpret_cast<const void*>(addrData);
    addrData += m_keyCount * sizeof(KeyType);

    // 最初のキー設定
    if (m_isKeyU8)
    {
      this->ResetKeys<u8, KeyType>();
      m_beginFrame = this->GetFrame<u8>(0);
      m_endFrame = this->GetFrame<u8>(m_keyCount - 1);
    }
    else
    {
      this->ResetKeys<u16, KeyType>();
      m_beginFrame = this->GetFrame<u16>(0);
      m_endFrame = this->GetFrame<u16>(m_keyCount - 1);
    }
  }

  template <typename KeyType>
  f32 UpdateFrame(f32 frame)
  {
    if (m_isKeyU8)
    {
      return this->UpdateFrameCore<u8, KeyType>(frame);
    }
    else
    {
      return this->UpdateFrameCore<u16, KeyType>(frame);
    }
  }

  template <typename FrameType, typename KeyType>
  f32 UpdateFrameCore(f32 frame)
  {
    // 適切なフレームを検索する
    if (s32 range = m_lastKey.CheckRange(frame))
    {
      // 順再生
      if (range > 0)
      {
        // 範囲外のフレームなら補正
        if (frame > m_endFrame)
        {
          frame = (f32)m_endFrame;
        }

        this->SearchKeyNext<FrameType, KeyType>(frame);
      }
      // 逆再生
      else
      {
        // 範囲外のフレームなら補正
        if (frame < m_beginFrame)
        {
          frame = (f32)m_beginFrame;
        }

        this->SearchKeyPrev<FrameType, KeyType>(frame);
      }
    }

    return frame;
  }

  template <typename FrameType, typename KeyType>
  void SearchKeyNext(f32 frame)
  {
    // 次のキーに移動した可能性が高い
    u16 r_index = m_lastKey.keyR.index + 1;
    if (r_index >= m_keyCount)
    {
      return;
    }

    u16 r_frame = this->GetFrame<FrameType>(r_index);
    if (frame <= r_frame)
    {
      // Lは前回のRからコピーするだけ
      m_lastKey.CopyKeyRtoL();

      // Rの値を新しく取得
      const KeyType* pKeyR = this->GetKey<KeyType>(r_index);
      m_lastKey.SetKeyR(
        r_index,
        r_frame,
        pKeyR->GetValue(m_valueScale, m_valueOffset),
        pKeyR->GetSlope(m_slopeScale, m_slopeOffset)
        );

      m_lastKey.Cache();
      return;
    }

    // 最後のフレームまで検索
    ++r_index;
    for (; r_index < m_keyCount; ++r_index)
    {
      u16 r_frame = this->GetFrame<FrameType>(r_index);
      if (frame > r_frame)
      {
        continue;
      }

      u16 l_index = r_index - 1;
      const KeyType* pKeyL = this->GetKey<KeyType>(l_index);
      m_lastKey.SetKeyL(
        l_index,
        this->GetFrame<FrameType>(l_index),
        pKeyL->GetValue(m_valueScale, m_valueOffset),
        pKeyL->GetSlope(m_slopeScale, m_slopeOffset)
        );

      const KeyType* pKeyR = this->GetKey<KeyType>(r_index);
      m_lastKey.SetKeyR(
        r_index,
        r_frame,
        pKeyR->GetValue(m_valueScale, m_valueOffset),
        pKeyR->GetSlope(m_slopeScale, m_slopeOffset)
        );

      m_lastKey.Cache();
      return;
    }
  }

  template <typename FrameType, typename KeyType>
  void SearchKeyPrev(f32 frame)
  {
    // ループしている？
    if (frame == m_beginFrame)
    {
      this->ResetKeys<FrameType, KeyType>();
      return;
    }
    
    // @attention 遅いけどとりあえず全キーをチェック
    for (u16 l_index = 0; l_index < m_keyCount - 1; ++l_index)
    {
      u16 r_index = l_index + 1;
      u16 l_frame = this->GetFrame<FrameType>(l_index);
      u16 r_frame = this->GetFrame<FrameType>(r_index);
      if (!((l_frame <= frame) && (frame < r_frame)))
      {
        continue;
      }

      const KeyType* pKeyL = this->GetKey<KeyType>(l_index);
      m_lastKey.SetKeyL(
        l_index,
        this->GetFrame<FrameType>(l_index),
        pKeyL->GetValue(m_valueScale, m_valueOffset),
        pKeyL->GetSlope(m_slopeScale, m_slopeOffset)
        );

      const KeyType* pKeyR = this->GetKey<KeyType>(r_index);
      m_lastKey.SetKeyR(
        r_index,
        r_frame,
        pKeyR->GetValue(m_valueScale, m_valueOffset),
        pKeyR->GetSlope(m_slopeScale, m_slopeOffset)
        );

      m_lastKey.Cache();
      break;
    }
  }

  void GetValue(f32* out_pValue, f32 frame)
  {
    static void (CurvePlayer::*s_getValueFuncList[])(f32*, f32) =
    {
      &CurvePlayer::GetValueBasepose,
      &CurvePlayer::GetValueStatic,
      &CurvePlayer::GetValueStatic,
      &CurvePlayer::GetValueStatic,
      &CurvePlayer::GetValueHermiteU16,
      &CurvePlayer::GetValueHermiteF32,
    };
    (this->*s_getValueFuncList[m_type])(out_pValue, frame);
  }

  void GetValueBasepose(f32* out_pValue, f32 frame)
  {
    (void)out_pValue;
    (void)frame;

  }
  void GetValueStatic(f32* out_pValue, f32 frame)
  {
    (void)frame;

    *out_pValue = m_lastKey.keyL.value;
  }
  void GetValueHermiteU16(f32* out_pValue, f32 frame)
  {
    f32 newFrame = this->UpdateFrame<CurveKeyHermiteU16>(frame);
    *out_pValue = m_lastKey.GetValue(newFrame);
  }
  void GetValueHermiteF32(f32* out_pValue, f32 frame)
  {
    f32 newFrame = this->UpdateFrame<CurveKeyHermiteF32>(frame);
    *out_pValue = m_lastKey.GetValue(newFrame);
  }
  f32 GetValueStatic()
  {
    return m_lastKey.keyL.value;
  }
  f32 GetValueHermiteU16(f32 frame)
  {
    f32 newFrame = this->UpdateFrame<CurveKeyHermiteU16>(frame);
    return m_lastKey.GetValue(newFrame);
  }
  f32 GetValueHermiteF32(f32 frame)
  {
    f32 newFrame = this->UpdateFrame<CurveKeyHermiteF32>(frame);
    return m_lastKey.GetValue(newFrame);
  }

  template <typename FrameType>
  FrameType GetFrame(u32 index)
  {
    const FrameType* pFrameList = static_cast<const FrameType*>(m_pFrameListU8_or_U16);
    return pFrameList[index];
  }
  template <typename KeyType>
  const KeyType* GetKey(u32 index)
  {
    const KeyType* pKeyList = static_cast<const KeyType*>(m_pKeyListU16_or_F32);
    return &pKeyList[index];
  }

  // 初期化時やループ時に０フレームに合わせる用
  template <typename FrameType, typename KeyType>
  void ResetKeys()
  {
    u16 l_index = 0;
    const KeyType* pKeyL = this->GetKey<KeyType>(l_index);
    m_lastKey.SetKeyL(
      l_index,
      this->GetFrame<FrameType>(l_index),
      pKeyL->GetValue(m_valueScale, m_valueOffset),
      pKeyL->GetSlope(m_slopeScale, m_slopeOffset)
      );

    u16 r_index = 1;
    const KeyType* pKeyR = this->GetKey<KeyType>(r_index);
    m_lastKey.SetKeyR(
      r_index,
      this->GetFrame<FrameType>(r_index),
      pKeyR->GetValue(m_valueScale, m_valueOffset),
      pKeyR->GetSlope(m_slopeScale, m_slopeOffset)
      );

    m_lastKey.Cache();
  }

  CurveKeyType m_type;
  u8 m_isKeyU8;
  u8 : 8; // pad
  u8 : 8; // pad

  s32 m_keyCount;
  s32 m_currentIndex;

  f32 m_valueScale;
  f32 m_valueOffset;
  f32 m_slopeScale;
  f32 m_slopeOffset;

  const void* m_pFrameListU8_or_U16;
  const void* m_pKeyListU16_or_F32;

  CurveLastKey m_lastKey;
  s32 m_beginFrame;
  s32 m_endFrame;
};

}}

#endif