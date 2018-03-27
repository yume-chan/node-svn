#pragma once

#include <node/error.hpp>
#include <node/v8.hpp>

namespace no {
struct object {
  public:
    explicit object(v8::Isolate* isolate)
        : _isolate(isolate)
        , _context(isolate->GetCurrentContext())
        , _value(v8::Object::New(isolate)) {}

    object(v8::Local<v8::Object> value)
        : _isolate(value->GetIsolate())
        , _context(_isolate->GetCurrentContext())
        , _value(value) {}

    auto operator[](v8::Local<v8::Value> name) {
        return value_ref(*this, name);
    }

    template <size_t N>
    auto operator[](const char (&name)[N]) {
        return value_ref(*this, no::NewName(_isolate, name));
    }

    operator v8::Local<v8::Object>() {
        return _value;
    }

    operator v8::Local<v8::Value>() {
        return _value;
    }

  private:
    v8::Isolate*           _isolate;
    v8::Local<v8::Context> _context;
    v8::Local<v8::Object>  _value;

    struct value_ref {
        template <class T>
        value_ref& operator=(v8::Local<T> value) {
            no::check_result(_owner._value->Set(_owner._context, _name, value));
            return *this;
        }

        template <class T>
        value_ref& operator=(T value) {
            return operator=(no::New(_owner._isolate, value));
        }

        value_ref& operator=(object value) {
            return operator=(value.operator v8::Local<v8::Value>());
        }

        v8::Local<v8::Value> get() {
            v8::Local<v8::Value> value;
            if (!_owner._value->Get(_owner._context, _name).ToLocal(&value))
                throw no::type_error("");
            return value;
        }

        operator v8::Local<v8::Value>() {
            return get();
        }

        v8::Value* operator->() {
            return get().operator->();
        }

      private:
        friend struct object;

        explicit value_ref(object& owner, v8::Local<v8::Value> name)
            : _owner(owner)
            , _name(name) {}

        object&              _owner;
        v8::Local<v8::Value> _name;
    };
};
} // namespace no
