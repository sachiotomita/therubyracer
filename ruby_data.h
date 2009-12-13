#ifndef __ruby_data_h__
#define __ruby_data_h__

#include <ruby.h>
#include <v8.h>
#include <v8_object.h>
#include <stdio.h>
#include <string>

/**
 * A RubyValueSource takes a Destination class and a return
 * type as a template argument, it converts a Ruby Value to
 * the appropriate intermediate type and feed it to an 
 * instance Destination type.  
 * 
 * The destination type should have a default constructor, 
 * and provide the methods detailed in data_conversion.txt
 * \tparam DEST destination type for the converted data
 * \tparam RET is the return type of T's methods
 */
template<class DEST, class RET> class RubyValueSource {

    /**
    * An instance of the destination class
    */
    DEST dest;

    public:
    RubyValueSource() {}
    ~RubyValueSource() {}


    RET push(VALUE& value) {
        switch (TYPE(value)) {
        case T_FIXNUM:
          return dest.pushInt(FIX2INT(value));
        case T_FLOAT:
          return dest.pushDouble(NUM2DBL(value));
        case T_STRING:
          return convertString(value);
        case T_NIL:
          return dest.pushNull();
        case T_TRUE:
          return dest.pushBool(true);
        case T_FALSE:
          return dest.pushBool(false);
      //  case T_DATA:
          // if (rb_is_function(value)) {
          //             return dest.pushCode(new Code<RubyCaller>)
          //           }
        }
        return dest.pushUndefined();
    }
    
private:
    RET convertString(VALUE& value) {
        std::string stringValue(RSTRING(value)->ptr);
        return dest.pushString(stringValue);
    }
};

/**
 * A RubyDest class receives on of the types defined in
 * data_conversion.txt, and converts it to the appropriate
 * Ruby VALUE.
 */
class RubyDest {

    public:
    RubyDest();
    ~RubyDest();

    VALUE pushString(const std::string& value) {
        return rb_str_new2(value.c_str());
    }

    VALUE pushInt(int64_t value) {
        return INT2FIX(value);
    }

    VALUE pushDouble(double value) {
        return rb_float_new(value);
    }

    VALUE pushBool(bool value) {
        return value ? Qtrue : Qfalse;
    }

    VALUE pushNull() {
        return Qnil;
    }

    VALUE pushUndefined() {
        return Qnil;
    }
    
    VALUE pushObject(v8::Handle<v8::Object>& value) {
      v8_object* wrapper = new v8_object(value);
      return wrapper->ruby_value;
    }
};


#endif
