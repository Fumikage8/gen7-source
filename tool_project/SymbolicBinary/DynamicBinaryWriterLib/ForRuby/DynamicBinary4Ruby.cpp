#include <iostream>

#include "RubyWrapper.h"

#include "../SymbolicBinaryWriter.h"

VALUE initialize_binaryWriter(VALUE self, VALUE filename)
{
	SymbolicBinaryWriter* ptr = RubyWrapper::GetPtr<SymbolicBinaryWriter>(self);
	
	new (ptr) SymbolicBinaryWriter(RSTRING_PTR(filename));
	return Qnil;
}

VALUE initialize_tableWriter(VALUE self, VALUE filename)
{
	SymbolicTableWriter* ptr = RubyWrapper::GetPtr<SymbolicTableWriter>(self);

	new (ptr) SymbolicTableWriter(RSTRING_PTR(filename));
	return Qnil;
}

VALUE wrap_bw_write(VALUE self, VALUE data)
{
	Check_Type(data, T_STRING);

	SymbolicBinaryWriter* ptr = RubyWrapper::GetPtr<SymbolicBinaryWriter>(self);

	ptr->write(RSTRING_PTR(data), RSTRING_LEN(data));

	return Qnil;
}
VALUE wrap_bw_writeWithSymbol(int argc, VALUE* argv, VALUE self)
{
	VALUE data_size, symbol, comment;

	SymbolicBinaryWriter* ptr = RubyWrapper::GetPtr<SymbolicBinaryWriter>(self);

	int nargs = rb_scan_args(argc, argv, "21", &data_size, &symbol, &comment); 

	switch(nargs){
		case 2:
			ptr->writeWithSymbol(NUM2INT(data_size), RSTRING_PTR(symbol));
			break;
		case 3:
			ptr->writeWithSymbol(NUM2INT(data_size), RSTRING_PTR(symbol), RSTRING_PTR(comment));
			break;
		default:
			throw SymbolicBinaryWriterException();
			break;			
	}

	return Qnil;
}

VALUE wrap_bw_recordDefSymbol(VALUE self, VALUE symbol, VALUE value, VALUE type)
{
	SymbolicBinaryWriter* ptr = RubyWrapper::GetPtr<SymbolicBinaryWriter>(self);

	ptr->recordDefSymbol(RSTRING_PTR(symbol), (unsigned char*)RSTRING_PTR(value), RSTRING_LEN(value), RSTRING_PTR(type));

	return Qnil;
}

VALUE wrap_bw_pos_setter(VALUE self, VALUE address)
{
	SymbolicBinaryWriter* ptr = RubyWrapper::GetPtr<SymbolicBinaryWriter>(self);

	ptr->setPos(NUM2INT(address));

	return Qnil;
}

VALUE wrap_bw_pos_getter(VALUE self, VALUE address)
{
	SymbolicBinaryWriter* ptr = RubyWrapper::GetPtr<SymbolicBinaryWriter>(self);

	return INT2NUM(ptr->getPos());
}


VALUE wrap_bw_close(VALUE self)
{
	SymbolicBinaryWriter* ptr = RubyWrapper::GetPtr<SymbolicBinaryWriter>(self);

	ptr->close();

	return Qnil;
}


VALUE wrap_tw_close(VALUE self)
{
	SymbolicTableWriter* ptr = RubyWrapper::GetPtr<SymbolicTableWriter>(self);

	ptr->close();

	return Qnil;
}

VALUE wrap_tw_recordDefSymbol(VALUE self, VALUE symbol, VALUE value, VALUE type)
{
	SymbolicTableWriter* ptr = RubyWrapper::GetPtr<SymbolicTableWriter>(self);

	ptr->recordDefSymbol(RSTRING_PTR(symbol), (unsigned char*)RSTRING_PTR(value), RSTRING_LEN(value), RSTRING_PTR(type));

	return Qnil;
}


extern "C"{
	void Init_SymbolicBinary()
	{
		VALUE cls_bw = RubyWrapper::DefineClass<SymbolicBinaryWriter>("SymbolicBinaryWriter", RUBY_METHOD_FUNC(initialize_binaryWriter), 1);

		rb_define_method(cls_bw, "write", RUBY_METHOD_FUNC(wrap_bw_write), 1);
		rb_define_method(cls_bw, "writeWithSymbol", RUBY_METHOD_FUNC(wrap_bw_writeWithSymbol), -1);
		rb_define_method(cls_bw, "recordDefSymbol", RUBY_METHOD_FUNC(wrap_bw_recordDefSymbol), 3);
		rb_define_method(cls_bw, "pos=", RUBY_METHOD_FUNC(wrap_bw_pos_setter), 1);
		rb_define_method(cls_bw, "pos", RUBY_METHOD_FUNC(wrap_bw_pos_getter), 0);
		rb_define_method(cls_bw, "close", RUBY_METHOD_FUNC(wrap_bw_close), 0);

		VALUE cls_tw = RubyWrapper::DefineClass<SymbolicTableWriter>("DefinitionTableWriter", RUBY_METHOD_FUNC(initialize_tableWriter), 1);

		rb_define_method(cls_tw, "recordDefSymbol", RUBY_METHOD_FUNC(wrap_tw_recordDefSymbol), 3);
		rb_define_method(cls_tw, "close", RUBY_METHOD_FUNC(wrap_tw_close), 0);		
	}
}
