// =================================================================
// SharedCount
// =================================================================
// -----------------------------------------------------------------
// シェアードカウンタコンストラクタ
// 宣言の順番のせいで、ヘッダに書けない
// -----------------------------------------------------------------
gfl::core::SharedCount::SharedCount(const gfl::core::WeakCount& ref) : m_pCount(ref.m_pCount)
{
	if(m_pCount){
		m_pCount->IncRefCount();
	}
}

// -----------------------------------------------------------------
// SharedCount = SharedCount
// -----------------------------------------------------------------
gfl::core::SharedCount& gfl::core::SharedCount::operator=(const gfl::core::SharedCount& ref)
{
	if(m_pCount != ref.m_pCount){
		// コピー元のリファレンスカウンタを増やす
		if(ref.m_pCount){
			ref.m_pCount->IncRefCount();
		}
		// 自分が書き換えられたのでリファレンスカウンタを減らす
		DecRefCount();
		m_pCount = ref.m_pCount;
	}
	return *this;
}

// =================================================================
// WeakCount
// =================================================================
// -----------------------------------------------------------------
// WeakCount = SharedCount
// -----------------------------------------------------------------
gfl::core::WeakCount& gfl::core::WeakCount::operator=(const gfl::core::SharedCount& ref)
{
	if(m_pCount != ref.m_pCount){
		// コピー元のリファレンスカウンタを増やす
		if(ref.m_pCount){
			ref.m_pCount->IncWeakRefCount();
		}
		// 自分が書き換えられたのでリファレンスカウンタを減らす
		DecRefCount();
		m_pCount = ref.m_pCount;
	}
	return *this;
}

// -----------------------------------------------------------------
// WeakCount = WeakCount
// -----------------------------------------------------------------
gfl::core::WeakCount& gfl::core::WeakCount::operator=(const gfl::core::WeakCount& ref)
{
	if(m_pCount != ref.m_pCount){
		// コピー元のリファレンスカウンタを増やす
		if(ref.m_pCount){
			ref.m_pCount->IncWeakRefCount();
		}
		// 自分が書き換えられたのでリファレンスカウンタを減らす
		DecRefCount();
		m_pCount = ref.m_pCount;
	}
	return *this;
}
