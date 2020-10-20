#ifndef GFLIB2_INTEREST_JOINT_FUNCTION_H_INCLUDED
#define GFLIB2_INTEREST_JOINT_FUNCTION_H_INCLUDED

#include <Animation/include/gfl2_JointFunctionBase.h>

namespace gfl2 { namespace animation {

/**
 * @brief 注目ジョイントファンクション
 */
class InterestJointFunction : public JointFunctionBase
{
private:

  // 角度補正タイプ
  enum AdjustUpdateType
  {
    ADJUST_UPDATE_TYPE_YZ, // Y軸、Z軸の順に補正
    ADJUST_UPDATE_TYPE_ZX, // Z軸、X軸の順に補正

    ADJUST_UPDATE_TYPE_COUNT
  };

public:

  /**
   * @brief コンストラクタ
   */
  InterestJointFunction();

  /**
   * @brief デストラクタ
   */
  virtual ~InterestJointFunction();

  /**
   * @brief 注目ノードの設定
   */
  void SetInterestNode(gfl2::renderingengine::scenegraph::resource::MotionConfigResourceNode::InterestNode* pInterestNode, math::Quaternion* pOldRotation);

  /**
   * @brief 更新
   *
   * @param inout_pOverAngle0 角度限界を超えた角度0
   * @param inout_pOverAngle1 角度限界を超えた角度1
   *
   * @return true:角度制限に引っかかった false:角度制限以内
   */
  bool Update(f32* inout_pOverAngle0, f32* inout_pOverAngle1);

  /**
   * @brief 最初の回転角度を取得
   *
   * @param pInterestPosition 注視位置
   * @param out_pAngle0 角度0
   * @param out_pAngle1 角度1
   */
  void GetFirstAngle(const math::Vector3* pInterestPosition, f32* out_pAngle0, f32* out_pAngle1);

  /**
   * @brief 注目タイプ設定
   *
   * @param type 注目タイプ
   */
  void SetType(gfl2::renderingengine::InterestType type)
  {
    m_pInterestNode->Type = type;
  }

  /**
   * @brief 注目タイプ取得
   *
   * @return 注目タイプを返す
   */
  gfl2::renderingengine::InterestType GetType() const
  {
    return (gfl2::renderingengine::InterestType)m_pInterestNode->Type;
  }

  /**
   * @brief 注目速度設定
   *
   * @param speed 注目速度
   */
  void SetSpeed(f32 speed)
  {
    m_pInterestNode->Speed = speed;
  }

  /**
   * @brief 注目速度取得
   *
   * @return 注目速度を返す
   */
  f32 GetSpeed() const
  {
    return m_pInterestNode->Speed;
  }

  /**
   * @brief 角度制限：最小値設定
   *
   * @param low 角度制限：最小値
   */
  void SetLow(f32 low)
  {
    m_pInterestNode->Low = low;
  }

  /**
   * @brief 角度制限：最小値取得
   *
   * @return 角度制限：最小値を返す
   */
  f32 GetLow() const
  {
    return m_pInterestNode->Low;
  }

  /**
   * @brief 角度制限：最大値設定
   *
   * @param high 角度制限：最大値
   */
  void SetHigh(f32 high)
  {
    m_pInterestNode->High = high;
  }

  /**
   * @brief 角度制限：最大値取得
   *
   * @return 角度制限：最大値を返す
   */
  f32 GetHigh() const
  {
    return m_pInterestNode->High;
  }

  /**
   * @brief 角度制限：最小値２設定
   *
   * @param low 角度制限：最小値
   */
  void SetLow2(f32 low)
  {
    m_pInterestNode->Low2 = low;
  }

  /**
   * @brief 角度制限：最小値２取得
   *
   * @return 角度制限：最小値２を返す
   */
  f32 GetLow2() const
  {
    return m_pInterestNode->Low2;
  }

  /**
   * @brief 角度制限：最大値２設定
   *
   * @param high 角度制限：最大値
   */
  void SetHigh2(f32 high)
  {
    m_pInterestNode->High2 = high;
  }

  /**
   * @brief 角度制限：最大値２取得
   *
   * @return 角度制限：最大値２を返す
   */
  f32 GetHigh2() const
  {
    return m_pInterestNode->High2;
  }

  /**
   * @brief 注目解除にかかるフレーム数を設定
   *
   * @param resetFrameCount 注目解除にかかるフレーム数
   */
  void SetResetFrameCount(s32 resetFrameCount)
  {
    m_AfterFrameMax = resetFrameCount;
  }

  /**
   * @brief 注目解除にかかるフレーム数を取得
   *
   * @return 注目解除にかかるフレーム数を返す
   */
  s32 GetResetFrameCount() const
  {
    return m_AfterFrameMax;
  }

  /**
   * @brief 終了確認
   *
   * @return true:終了 false:更新中
   */
  bool IsFinish() const
  {
    return (m_AfterFrame >= m_AfterFrameMax);
  }

	/**
	 * 顔向け補正行列を計算
	 */
  void CalculateAdjustMatrix(f32 adjustAngle, f32 decline);

	/**
	 * 親の関数設定
	 */
  void SetParentFunction(InterestJointFunction* pParentFunction)
  {
    m_pParentFunction = pParentFunction;
  }
	/**
	 * 親の関数取得
	 */
  InterestJointFunction* GetParentFunction()
  {
    return m_pParentFunction;
  }
  const InterestJointFunction* GetParentFunction() const
  {
    return m_pParentFunction;
  }

	/**
	 * 子供の関数設定
	 */
  void SetChildFunction(InterestJointFunction* pChildFunction)
  {
    m_pChildFunction = pChildFunction;
  }
	/**
	 * 子供の関数取得
	 */
  InterestJointFunction* GetChildFunction()
  {
    return m_pChildFunction;
  }
  const InterestJointFunction* GetChildFunction() const
  {
    return m_pChildFunction;
  }

private:

  static void GetAngleXY(const math::Matrix34& jointMatrix, const math::Vector3& interestPosition, bool reverseFlag, f32* out_pAngle0, f32* out_pAngle1);
  static void GetAngleYZ(const math::Matrix34& jointMatrix, const math::Vector3& interestPosition, bool reverseFlag, f32* out_pAngle0, f32* out_pAngle1);
  static void GetAngleZY(const math::Matrix34& jointMatrix, const math::Vector3& interestPosition, bool reverseFlag, f32* out_pAngle0, f32* out_pAngle1);
  static void GetAngleXZ(const math::Matrix34& jointMatrix, const math::Vector3& interestPosition, bool reverseFlag, f32* out_pAngle0, f32* out_pAngle1);

  bool Clamp(f32* inout_pAngle, f32* out_pOverAngle);
  bool Clamp2(f32* inout_pAngle0, f32* inout_pAngle1, f32* out_pOverAngle0, f32* out_pOverAngle1);

  void UpdateX(f32 angle, u32 angleIndex);
  void UpdateY(f32 angle, u32 angleIndex);
  void UpdateZ(f32 angle, u32 angleIndex);

  void UpdateMatrix(f32 angle, f32 ax, f32 ay, f32 az, u32 angleIndex);
  void RotateMatrix(f32 angle, f32 ax, f32 ay, f32 az);

  void RotateX(f32 angle);
  void RotateY(f32 angle);
  void RotateZ(f32 angle);
  
  AdjustUpdateType GetAdjustUpdateType() const;

private:

  // とりあえずそのままもってきた

  /**
  * 楕円でクランプするクラス
  */
  class ClampEllipse {
  public:

    enum {
      CENTER_PLUS,
      CENTER_MINUS,
      CENTER_MAX
    };

    ClampEllipse( void ) {
    }

    /**
    * コンストラクタ
    * @param v　ベクトル２型　X軸半径　Y軸半径
    */
    explicit ClampEllipse( const math::Vector2& v ) {
      SetFocus( v );
    }

    /**
    * コンストラクタ
    * @param x　X軸半径
    * @param y　Y軸半径
    */
    ClampEllipse( const f32 x, const f32 y ) {
      SetFocus( x, y );
    }

    /**
    * 楕円の焦点を求める
    * @param x　X軸半径
    * @param y　Y軸半径
    */
    void SetFocus( const f32 x, const f32 y ) {
      const f32 a = x*x;
      const f32 b = y*y;

      m_A = x, m_B = y, m_A2 = a, m_B2 = b;
      // 焦点を設定
      if( a < b ){ // Y軸の方が大きい場合は、Y軸上に焦点がある
        m_Center[CENTER_MINUS].GFL_VECTOR_X = m_Center[CENTER_PLUS].GFL_VECTOR_X = 0.0f;
        const f32 cy = math::FSqrt( b - a );
        m_Center[CENTER_MINUS].GFL_VECTOR_Y = -cy;
        m_Center[CENTER_PLUS].GFL_VECTOR_Y = cy;
      } else { // X軸の方が大きい場合は、X軸上に焦点がある　等しい場合は円
        m_Center[CENTER_MINUS].GFL_VECTOR_Y = m_Center[CENTER_PLUS].GFL_VECTOR_Y = 0.0f;
        const f32 cx = math::FSqrt( a - b );
        m_Center[CENTER_MINUS].GFL_VECTOR_X = -cx;
        m_Center[CENTER_PLUS].GFL_VECTOR_X = cx;
      }
      m_Distance = GetDistance( x, 0.0f ); // x 軸上の点を基準とする
    }

    void SetFocus( const math::Vector2& v ) {
      SetFocus( v.x, v.y );
    }

    /**
    * 楕円の外の場合は楕円軌道上に設定する
    * @param v　入力ベクトル　内側の場合は変化なし
    * @param over 制限をオーバーした値
    */
    bool Clamp( math::Vector2* v, math::Vector2* over = 0 ) {
      if( m_A == 0.0f ){ // Y軸上のリミット　線分でのリミット
        f32 y = v->GFL_VECTOR_Y;
        f32 ly;
        s32 ret = false;
        if( 0.0f < y ){ // plus mode
          ly = m_B;
          if( ly < y ){
            y = ly;
            ret = true;
          }
        } else {
          ly = -m_B;
          if( y < ly ){
            y = ly;
            ret = true;
          }
        }
        if( over ){
          over->GFL_VECTOR_Y = 0.0f;
        }
        if(ret){
          if( over ){
            over->GFL_VECTOR_Y = v->GFL_VECTOR_Y - ly;
          }
          v->GFL_VECTOR_Y = y;
        }
        v->GFL_VECTOR_X = 0.0f;
        if( over ){
          over->GFL_VECTOR_X = v->GFL_VECTOR_X;
        }
        return true; // ret じゃないよ
      } else if( m_B == 0.0f ){ // X軸上のリミット　線分でのリミット
        f32 x = v->GFL_VECTOR_X;
        f32 lx;
        s32 ret = false;
        if( 0.0f < x ){ // plus mode
          lx = m_A;
          if( lx < x ){
            x = lx;
            ret = true;
          }
        } else {
          lx = -m_A;
          if( x < lx ){
            x = lx;
            ret = true;
          }
        }
        if( over ){
          over->GFL_VECTOR_X = 0.0f;
        }
        if(ret){
          if( over ){
            over->GFL_VECTOR_X = v->GFL_VECTOR_X - lx;
          }
          v->GFL_VECTOR_X = x;
        }
        v->GFL_VECTOR_Y = 0.0f;
        if( over ){
          over->GFL_VECTOR_Y = v->GFL_VECTOR_Y;
        }
        return true; // ret じゃないよ
      } else if( GetDistance( ) < GetDistance( *v ) ){ // 楕円の外側にある　Distance2で比較　Sqrtしないので高速
        if( v->GFL_VECTOR_X == 0.0f ){ // xが０だと傾きが無限大になってしまうので
          const f32 y = (0.0f < v->GFL_VECTOR_Y) ? m_B : -m_B;
          if( over ){
            over->GFL_VECTOR_X = 0.0f;
            over->GFL_VECTOR_Y = v->GFL_VECTOR_Y - y;
          }
          v->GFL_VECTOR_X = 0.0f;
          v->GFL_VECTOR_Y = y;
        } else {
          const f32 m = v->GFL_VECTOR_Y / v->GFL_VECTOR_X; // 直線の傾き
          f32 x = (m_A * m_B) / math::FSqrt( m_B2 + m_A2 * m * m ); // 楕円の式に代入して整理
          if( v->GFL_VECTOR_X < 0.0f ){ // ｘの解は±なので
            x = -x;
          }
          const f32 y = m * x;
          if( over ){
            over->GFL_VECTOR_X = v->GFL_VECTOR_X - x;
            over->GFL_VECTOR_Y = v->GFL_VECTOR_Y - y;
          }
          v->GFL_VECTOR_X = x;
          v->GFL_VECTOR_Y = y; // 直線の式に代入
        }
        return true;
      }
      return false;
    }

    /**
    * 楕円の外の場合は楕円軌道上に設定する
    * @param x　制限をかけるX値
    * @param y　制限をかけるY値
    * @param x_over　制限をオーバーしたX値
    * @param y_over　制限をオーバーしたY値
    */
    bool Clamp( f32* x, f32* y, f32* x_over = 0, f32* y_over = 0 ) {
      bool isRotateLimit = false;

      math::Vector2 pos( *x, *y );
      if( x_over && y_over ){
        math::Vector2 over;
        if( Clamp( &pos, &over ) ){
          *x = pos.GFL_VECTOR_X;
          *y = pos.GFL_VECTOR_Y;
          *x_over = over.GFL_VECTOR_X;
          *y_over = over.GFL_VECTOR_Y;

          isRotateLimit = !(math::IsAlmostZero(over.GFL_VECTOR_X)) || !(math::IsAlmostZero(over.GFL_VECTOR_Y));
        }else{
          *x_over = *y_over = 0.0f;
        }
      } else {
        if( Clamp( &pos ) ){
          *x = pos.GFL_VECTOR_X;
          *y = pos.GFL_VECTOR_Y;

          isRotateLimit = true;
        }
      }

      return isRotateLimit;
    }
  private:
  public:
    f32 GetDistance( const math::Vector3& pos ) const {
      //return pos.Distance( m_Center[CENTER_MINUS] ) + pos.Distance( m_Center[CENTER_PLUS] );
      math::Vector3 sub0 = pos - m_Center[CENTER_MINUS];
      math::Vector3 sub1 = pos - m_Center[CENTER_PLUS];
      return sub0.Length() + sub1.Length();
    }

    f32 GetDistance( const f32 x, const f32 y ) const {
      return GetDistance( math::Vector3( x, y, 0 ) );
    }

    f32 GetDistance( void ) const {
      return m_Distance;
    }

    math::Vector3 m_Center[CENTER_MAX];
    f32 m_A;
    f32 m_B;
    f32 m_A2;
    f32 m_B2;
    f32 m_Distance;
  };


private:

  gfl2::renderingengine::scenegraph::resource::MotionConfigResourceNode::InterestNode* m_pInterestNode;  ///< 注目ノード
  math::Quaternion*                                   m_pOldRotation;   ///< 前回の回転値

#if 0
  gfl2::renderingengine::InterestType  m_Type;           ///< 注目タイプ

  f32                                                 m_Speed;          ///< 注目速度：ラジアン

  f32                                                 m_Low;            ///< 角度制限、最小値：ラジアン
  f32                                                 m_High;           ///< 角度制限、最大値：ラジアン
  f32                                                 m_Low2;           ///< 角度制限２、最小値：ラジアン
  f32                                                 m_High2;          ///< 角度制限２、最大値：ラジアン
#endif

  ClampEllipse                                        m_Upper;          ///< 上半分の楕円
  ClampEllipse                                        m_Lower;          ///< 下半分の楕円

  f32                                                 m_OldAngles[2];   ///< 前回の角度

  f32                                                 m_AfterAngles[2]; ///< 注目解除開始時の角度
  s32                                                 m_AfterFrame;     ///< 注目解除開始からのフレーム数
  s32                                                 m_AfterFrameMax;  ///< 注目が終了するまでのフレーム数

  InterestJointFunction*                              m_pParentFunction;  ///< 親の関数
  InterestJointFunction*                              m_pChildFunction;   ///< 子供の関数
};

}}

#endif