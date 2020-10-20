// =================================================================
// SharedCount
// =================================================================
// -----------------------------------------------------------------
// �V�F�A�[�h�J�E���^�R���X�g���N�^
// �錾�̏��Ԃ̂����ŁA�w�b�_�ɏ����Ȃ�
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
		// �R�s�[���̃��t�@�����X�J�E���^�𑝂₷
		if(ref.m_pCount){
			ref.m_pCount->IncRefCount();
		}
		// ����������������ꂽ�̂Ń��t�@�����X�J�E���^�����炷
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
		// �R�s�[���̃��t�@�����X�J�E���^�𑝂₷
		if(ref.m_pCount){
			ref.m_pCount->IncWeakRefCount();
		}
		// ����������������ꂽ�̂Ń��t�@�����X�J�E���^�����炷
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
		// �R�s�[���̃��t�@�����X�J�E���^�𑝂₷
		if(ref.m_pCount){
			ref.m_pCount->IncWeakRefCount();
		}
		// ����������������ꂽ�̂Ń��t�@�����X�J�E���^�����炷
		DecRefCount();
		m_pCount = ref.m_pCount;
	}
	return *this;
}
