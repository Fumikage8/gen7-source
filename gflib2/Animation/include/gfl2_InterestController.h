#ifndef GFLIB2_INTEREST_CONTROLLER_H_INCLUDED
#define GFLIB2_INTEREST_CONTROLLER_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_JointFunctionBase.h>
#include <Animation/include/gfl2_InterestJointFunction.h>

#include <heap/include/gfl2_Heap.h>

namespace gfl2 { namespace animation {

/**
 * @brief 注目コントローラー
 */
class InterestController
{
public:

  /**
   * @brief コンストラクタ
   */
  InterestController();

  /**
   * @brief デストラクタ
   */
  ~InterestController();

  /**
   * @brief 初期化
   *
   * @param pHeap ヒープ
   * @param pModelNode モデルノード
   * @param pMotionConfigNode モーションコンフィグノード
   */
  void Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode, gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode);

  /**
   * @brief 終了
   */
  void Finalize();

  /**
   * @brief 更新
   */
  void Update();

  /**
   * @brief 終了確認
   *
   * @return true:終了 false:更新中
   */
  bool IsFinish() const;

#if 0
  void SetInterestWeight(f32 weight)
  {
    for (u32 i = 0; i < m_InterestJointFunctionCount; ++i)
    {
      m_pInterestJointFunctionList[i].SetWeight(weight);
    }
  }
#endif

  /**
   * @brief 注視位置設定
   *
   * @param pPosition 注視位置：NULLなら終了
   */
  void SetInterestPosition(const math::Vector3* pPosition)
  {
    m_pInterestPosition = pPosition;
  }

  /**
   * @brief 注視用のジョイント数取得
   *
   * @return 注目用ジョイント数
   */
  u32 GetInterestJointCount() const
  {
    return m_InterestJointFunctionCount;
  }

  /**
   * @brief 注視用のジョイントノード取得
   *
   * @param index
   * @return 注目用ジョイントノードのポインタ
   */
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* GetInterestJointNode(u32 index) const
  {
    return m_pInterestJointFunctionList[index].GetJointNode();
  }

  /**
   * @brief 注目タイプ設定
   *
   * @param index インデックス
   * @param type 注目タイプ
   */
  void SetType(u32 index, gfl2::renderingengine::InterestType type)
  {
    m_pInterestJointFunctionList[index].SetType(type);
  }

  /**
   * @brief 注目タイプ取得
   *
   * @param index インデックス
   *
   * @return 注目タイプを返す
   */
  gfl2::renderingengine::InterestType GetType(u32 index) const
  {
    return m_pInterestJointFunctionList[index].GetType();
  }

  /**
   * @brief 注目速度設定
   *
   * @param index インデックス
   * @param speed 注目速度
   */
  void SetSpeed(u32 index, f32 speed)
  {
    m_pInterestJointFunctionList[index].SetSpeed(speed);
  }

  /**
   * @brief 注目速度取得
   *
   * @param index インデックス
   *
   * @return 注目速度を返す
   */
  f32 GetSpeed(u32 index) const
  {
    return m_pInterestJointFunctionList[index].GetSpeed();
  }

  /**
   * @brief 角度制限：最小値設定
   *
   * @param index インデックス
   * @param low 角度制限：最小値
   */
  void SetLow(u32 index, f32 low)
  {
    m_pInterestJointFunctionList[index].SetLow(low);
  }

  /**
   * @brief 角度制限：最小値取得
   *
   * @param index インデックス
   *
   * @return 角度制限：最小値を返す
   */
  f32 GetLow(u32 index) const
  {
    return m_pInterestJointFunctionList[index].GetLow();
  }

  /**
   * @brief 角度制限：最大値設定
   *
   * @param index インデックス
   * @param high 角度制限：最大値
   */
  void SetHigh(u32 index, f32 high)
  {
    m_pInterestJointFunctionList[index].SetHigh(high);
  }

  /**
   * @brief 角度制限：最大値取得
   *
   * @param index インデックス
   *
   * @return 角度制限：最大値を返す
   */
  f32 GetHigh(u32 index) const
  {
    return m_pInterestJointFunctionList[index].GetHigh();
  }

  /**
   * @brief 角度制限：最小値２設定
   *
   * @param index インデックス
   * @param low 角度制限：最小値
   */
  void SetLow2(u32 index, f32 low)
  {
    m_pInterestJointFunctionList[index].SetLow2(low);
  }

  /**
   * @brief 角度制限：最小値２取得
   *
   * @param index インデックス
   *
   * @return 角度制限：最小値２を返す
   */
  f32 GetLow2(u32 index) const
  {
    return m_pInterestJointFunctionList[index].GetLow2();
  }

  /**
   * @brief 角度制限：最大値２設定
   *
   * @param index インデックス
   * @param high 角度制限：最大値
   */
  void SetHigh2(u32 index, f32 high)
  {
    m_pInterestJointFunctionList[index].SetHigh2(high);
  }

  /**
   * @brief 角度制限：最大値２取得
   *
   * @param index インデックス
   *
   * @return 角度制限：最大値２を返す
   */
  f32 GetHigh2(u32 index) const
  {
    return m_pInterestJointFunctionList[index].GetHigh2();
  }

  /**
   * @brief 注目解除にかかるフレーム数を設定
   *
   * @param index インデックス
   * @param resetFrameCount 注目解除にかかるフレーム数
   */
  void SetResetFrameCount(u32 index, s32 resetFrameCount)
  {
    m_pInterestJointFunctionList[index].SetResetFrameCount(resetFrameCount);
  }

  /**
   * @brief 注目解除にかかるフレーム数を取得
   *
   * @param index インデックス
   *
   * @return 注目解除にかかるフレーム数を返す
   */
  s32 GetResetFrameCount(u32 index) const
  {
    return m_pInterestJointFunctionList[index].GetResetFrameCount();
  }

	/**
	 * 顔向け補正の有効無効設定
	 * @param isEnabled true:補正有効, false:補正無効
	 */
	void SetAdjustAngleEnable(bool isEnabled)
  {
		this->m_isAdjustEnabled = isEnabled;
	}

	/**
	 * 顔向け補正の有効無効取得
	 */
	bool IsAdjustAngleEnabled() const
  {
		return GFL_BOOL_CAST(this->m_isAdjustEnabled);
	}

	/**
	 * 顔向け補正角度を設定
	 * @param tagAdjustAngle　顔向け補正角度（ラジアン）
	 */
	void SetTagAdjustAngle(f32 tagAdjustAngle)
  {
		this->m_TagAdjustAngle = tagAdjustAngle;
	}

	/**
	 * 顔向け補正角度をリセット
	 */
	void ResetTagAdjustAngle()
  {
    this->m_AdjustAngle = 0.0f;
		this->m_TagAdjustAngle = 0.0f;
	}

	/**
	 * 顔向け補正角度を取得
	 */
	f32 GetTagAdjustAngle() const
  {
		return this->m_TagAdjustAngle;
	}

	/**
	 * 顔向け傾きを設定
	 * @param tagDecline　顔向け傾き（ラジアン）
	 */
	void SetTagDecline(f32 tagDecline)
  {
		this->m_TagDecline = tagDecline;
	}

	/**
	 * 顔向け傾きを取得
	 */
	f32 GetTagDecline( ) const
  {
		return m_TagDecline;
	}

  /**
   *  ばね係数を設定
   */
  void SetSpringVal( f32 adjust_spring_val, f32 decline_spring_val )
  {
    m_AdjustSpringVal   = adjust_spring_val;
    m_DeclineSpringVal  = decline_spring_val; 
  }

private:

  void InitializeInterest();

	/**
	 * 顔向け補正行列を計算
	 */
  void CalculateAdjustMatrix();

private:

	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*           m_pModelNode;                 ///< 反映対象のモデルノード
	gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*    m_pMotionConfigNode;          ///< モーション設定リソースノード

  InterestJointFunction*                                                    m_pInterestJointFunctionList; ///< 注目ジョイントファンクションリスト
  u32                                                                       m_InterestJointFunctionCount; ///< 注目ジョイントファンクション数

  const math::Vector3*                                                      m_pInterestPosition;          ///< 注目位置

  b8                                                                        m_isFinish;                   ///< 更新終了フラグ
  b8                                                                        m_isAdjustEnabled;            ///< 角度補正フラグ

  // 角度補正用、よく分からないのでsangoからそのまま移植
  f32 m_TagAdjustAngle;
  f32 m_AdjustAngle;
  f32 m_TagDecline;
  f32 m_Decline;
  f32 m_AdjustSpringVal;
  f32 m_DeclineSpringVal;
};

}}

#endif