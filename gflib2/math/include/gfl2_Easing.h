
#ifndef GFLIB2_MATH_EASING_H_INCLUDED
#define GFLIB2_MATH_EASING_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <math.h>
#include <math/include/gfl2_MathCommon.h>

namespace gfl2 { namespace math {

/**
 * @brief イージング
 */
class Easing
{
public:
  
  enum EaseFunc
  {
    LINEAR,

    IN_SINE,
    OUT_SINE,
    INOUT_SINE,

    IN_QUAD,
    OUT_QUAD,
    INOUT_QUAD,

    IN_CUBIC,
    OUT_CUBIC,
    INOUT_CUBIC,

    IN_QUART,
    OUT_QUART,
    INOUT_QUART,

    IN_QUINT,
    OUT_QUINT,
    INOUT_QUINT,

    IN_EXPO,
    OUT_EXPO,
    INOUT_EXPO,

    IN_CIRC,
    OUT_CIRC,
    INOUT_CIRC,

    IN_BACK,
    OUT_BACK,
    INOUT_BACK,

    IN_ELASTIC,
    OUT_ELASTIC,
    INOUT_ELASTIC,

    IN_BOUNCE,
    OUT_BOUNCE,
    INOUT_BOUNCE
  };

private:

  template <f32 (*Func)(f32)>
  class EaseMaker
  {
  public:
    
    static f32 In(f32 t)
    {
      return Func(t);
    }

    static f32 Out(f32 t)
    {
      return 1.0f - In(1.0f - t);
    }

    static f32 InOut(f32 t)
    {
      return (t < 0.5f)
        ? In(2.0f * t) * 0.5f
        : 0.5f + Out(2.0f * t - 1.0f) * 0.5f;
    }
  };

  //EASE_LINEAR,
  static f32 Linear(f32 t)
  {
    return t;
  }

  // sin(t)
  static f32 Sine(f32 t)
  {
    return sin((t - 1.0f) * (PI * 0.5f)) + 1.0f;
  }

  // t^2
  static f32 Quad(f32 t)
  {
    return t * t;
  }

  // t^3
  static f32 Cubic(f32 t)
  {
    return t * t * t;
  }

  // t^4
  static f32 Quart(f32 t)
  {
    return t * t * t * t;
  }

  // t^5
  static f32 Quint(f32 t)
  {
    return t * t * t * t * t;
  }

  // exp
  static f32 Expo(f32 t)
  {
    return (t == 0.0f)
      ? t
      : pow(2.0f, 10.0f * (t - 1.0f));
  }

  // circle
  static f32 Circ(f32 t)
  {
    return 1.0f - sqrt(1.0f - (t * t));
  }

  // back
  static f32 Back(f32 t)
  {
    return (t * t * t) + (-t * sin(t * PI));
  }

  // elastic
  static f32 Elastic(f32 t)
  {
    return sin(13.0f * (PI * 0.5f) * t) * pow(2.0f, 10.0f * (t - 1.0f));
  }

  // bounce
  static f32 BounceCore(f32 t)
  {
    if (t < 4.0f / 11.0f)
    {
      return (121.0f * t * t) / 16.0f;
    }

    if (t < 8.0f / 11.0f)
    {
      return (363.0f / 40.0f * t * t) - (99.0f / 10.0f * t) + (17.0f / 5.0f);
    }

    if (t < 9.0f / 10.0f)
    {
      return (4356.0f / 361.0f * t * t) - (35442.0f / 1805.0f * t) + (16061.0f / 1805.0f);
    }

    return (54.0f / 5.0f * t * t) - (513.0f / 25.0f * t) + (268.0f / 25.0f);
  }
  static f32 Bounce(f32 t)
  {
    return 1.0f - BounceCore(1.0f - t);
  }

public:

  /**
   * @brief 補間された値の取得
   *
   * @param func イージング関数
   * @param value0 t=0の時の値
   * @param value1 t=1の時の値
   * @param t 補間時間
   *
   * @return 補間された値を返す
   */
  static f32 GetValue(EaseFunc func, f32 value0, f32 value1, f32 t)
  {
    f32 new_t = GetValue(func, t);
    return value0 + (value1 - value0) * new_t;
  }

  /**
   * @brief 補間された値の取得
   *
   * @param func イージング関数
   * @param t 補間時間
   *
   * @return 補間された値を返す
   */
  static f32 GetValue(EaseFunc func, f32 t)
  {
    static f32 (*sc_easeFuncList[])(f32) =
    {
      &Linear,                    //LINEAR,

      &EaseMaker<Sine>::In,       //IN_SINE,
      &EaseMaker<Sine>::Out,      //OUT_SINE,
      &EaseMaker<Sine>::InOut,    //INOUT_SINE,

      &EaseMaker<Quad>::In,       //IN_QUAD,
      &EaseMaker<Quad>::Out,      //OUT_QUAD,
      &EaseMaker<Quad>::InOut,    //INOUT_QUAD,

      &EaseMaker<Cubic>::In,      //IN_CUBIC,
      &EaseMaker<Cubic>::Out,     //OUT_CUBIC,
      &EaseMaker<Cubic>::InOut,   //INOUT_CUBIC,

      &EaseMaker<Quart>::In,      //IN_QUART,
      &EaseMaker<Quart>::Out,     //OUT_QUART,
      &EaseMaker<Quart>::InOut,   //INOUT_QUART,

      &EaseMaker<Quint>::In,      //IN_QUINT,
      &EaseMaker<Quint>::Out,     //OUT_QUINT,
      &EaseMaker<Quint>::InOut,   //INOUT_QUINT,

      &EaseMaker<Expo>::In,       //IN_EXPO,
      &EaseMaker<Expo>::Out,      //OUT_EXPO,
      &EaseMaker<Expo>::InOut,    //INOUT_EXPO,

      &EaseMaker<Circ>::In,       //IN_CIRC,
      &EaseMaker<Circ>::Out,      //OUT_CIRC,
      &EaseMaker<Circ>::InOut,    //INOUT_CIRC,

      &EaseMaker<Back>::In,       //IN_BACK,
      &EaseMaker<Back>::Out,      //OUT_BACK,
      &EaseMaker<Back>::InOut,    //INOUT_BACK,

      &EaseMaker<Elastic>::In,    //IN_ELASTIC,
      &EaseMaker<Elastic>::Out,   //OUT_ELASTIC,
      &EaseMaker<Elastic>::InOut, //INOUT_ELASTIC,

      &EaseMaker<Bounce>::In,     //IN_BOUNCE,
      &EaseMaker<Bounce>::Out,    //OUT_BOUNCE,
      &EaseMaker<Bounce>::InOut,  //INOUT_BOUNCE
    };

    return sc_easeFuncList[func](t);
  }
};

}}

#endif
