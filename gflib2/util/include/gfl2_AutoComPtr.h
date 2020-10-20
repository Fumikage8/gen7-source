#ifndef GFLIB2_TYPE_AUTOCOMPTR_H_INCLUDED
#define GFLIB2_TYPE_AUTOCOMPTR_H_INCLUDED

template < class Type_ >
class AutoComPtr;

template < class Type_ >
struct AutoComPtrRef {
	AutoComPtrRef( AutoComPtr< Type_ >& rhs ) : ref( rhs ) {}
	AutoComPtr< Type_ >& ref;

private:
  AutoComPtrRef< Type_ > & operator = ( const AutoComPtrRef< Type_ >& );
};

//! @brief COM用のスマートポインタ(主にDirectX用)
template < class Type_ >
class AutoComPtr
{
public:
	typedef Type_ element_type;

	//! @brief NULLポインタをセットするデフォルトコンストラクタ
	explicit AutoComPtr( Type_* ptr = 0 ) : ptr__( ptr ) {}
	//! @brief 所有権を譲渡するコピーコンストラクタ
	AutoComPtr( AutoComPtr< Type_ >& rhs ) : ptr__( rhs.release() ) {}
	//! @brief 所有権を譲渡するコピーコンストラクタ
	AutoComPtr( AutoComPtrRef< Type_ > rhs ) : ptr__( rhs.ref.release() ) {}

	template< class Other__ >
	operator AutoComPtr< Other__ >( void )
	{
		return (AutoComPtr< Other__ >(*this));
	}

	template< class Other__ >
	operator AutoComPtrRef< Other__ >()
	{
		return (AutoComPtrRef< Other__ >(*this));
	}

	template< class Other__ >
	AutoComPtr< Type_ >& operator = ( AutoComPtr< Other__ >& rhs )
	{
		reset( rhs.release() );
		return (*this);
	}

	template< class Other__ >
	AutoComPtr( AutoComPtr< Other__ >& rhs ) : ptr__( rhs.release() ) {}

	AutoComPtr< Type_ >& operator = ( AutoComPtr< Type_ >& rhs )
	{
		reset( rhs.release() );
		return (*this);
	}

	AutoComPtr< Type_ >& operator = ( AutoComPtrRef< Type_ >& rhs )
	{
		reset( rhs.ref.release() );
		return (*this);
	}

	//! @note 変数の寿命が尽きる際にRelease
	~AutoComPtr( void )
	{
		reset();
	}

	Type_* operator->( void ) const
	{
		return (ptr__);
	}

	Type_** operator&( void )
	{
		return &ptr__;
	}

	Type_* get( void ) const
	{
		return (ptr__);
	}

	Type_* release( void )
	{
		Type_* tmp = ptr__;
		ptr__ = 0;
		return (tmp);
	}

	void reset( Type_* ptr = 0 )
	{
		if( ptr != ptr__ && ptr__ )
		{
			ptr__->Release();
		}
		ptr__ = ptr;
	}

private:
	Type_*  ptr__;
};

#endif
