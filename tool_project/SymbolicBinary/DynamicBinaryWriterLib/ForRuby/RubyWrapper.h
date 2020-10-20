#include <ruby.h>
#include <new>

namespace RubyWrapper{

	template<class T>
	T* GetPtr(VALUE self)
	{
		T* ptr;
		Data_Get_Struct(self, T, ptr);
		return ptr;
	}

	template<class T>
	void free(T* ptr)
	{
		ptr->~T();
		ruby_xfree(ptr);
	}

	template<class T>
	VALUE allocate(VALUE self)
	{
		void *ptr = ruby_xmalloc(sizeof(T));

		return Data_Wrap_Struct(self, NULL, (void(*)(T*))RubyWrapper::free<T>, ptr);
	}	

	template<class T>
	VALUE initialize(VALUE self)
	{
		T* ptr = RubyWrapper::GetPtr<T>(self);
		new (ptr) T();

		return Qnil;
	}

	template<class T>
	VALUE DefineClass(const char* classname)
	{
		VALUE cls = rb_define_class(classname, rb_cObject);
		rb_define_alloc_func(cls, RubyWrapper::allocate<T>);
		rb_define_private_method(cls, "initialize", (VALUE(*)(...))((VALUE(*)(VALUE))RubyWrapper::initialize<T>), 0);

		return cls;
	}

	template<class T>
	VALUE DefineClass(const char* classname, VALUE(*initializeFunc)(...), int argc)
	{
		VALUE cls = rb_define_class(classname, rb_cObject);
		rb_define_alloc_func(cls, RubyWrapper::allocate<T>);
		rb_define_private_method(cls, "initialize", initializeFunc, argc);

		return cls;
	}
}
