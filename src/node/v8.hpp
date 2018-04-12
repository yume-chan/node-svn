#pragma once

#include <optional>
#include <string>

#include <uv.h>
#include <v8.h>

namespace {
template <class T>
struct Factory;

template <>
struct Factory<v8::Object> {
    static inline v8::Local<v8::Object> data(v8::Isolate* isolate) {
        return v8::Object::New(isolate);
    }
};

template <>
struct Factory<v8::External> {
    static inline v8::Local<v8::External> data(v8::Isolate* isolate,
                                               void*        value) {
        return v8::External::New(isolate, value);
    }
};

template <>
struct Factory<v8::Promise::Resolver> {
    static inline v8::Local<v8::Promise::Resolver> data(v8::Local<v8::Context>& context) {
        return v8::Promise::Resolver::New(context).ToLocalChecked();
    }
};

template <>
struct Factory<v8::Array> {
    static inline v8::Local<v8::Array> data(v8::Isolate* isolate, int length = 0) {
        return v8::Array::New(isolate, length);
    }
};

template <>
struct Factory<v8::Function> {
    static inline v8::Local<v8::Function> data(v8::Local<v8::Context>& context,
                                               v8::FunctionCallback    callback,
                                               v8::Local<v8::Value>    data   = v8::Local<v8::Value>(),
                                               int                     length = 0) {
        return v8::Function::New(context, callback, data, length).ToLocalChecked();
    }
};

template <>
struct Factory<v8::FunctionTemplate> {
    static inline v8::Local<v8::FunctionTemplate> data(v8::Isolate*             isolate,
                                                       v8::FunctionCallback     callback  = nullptr,
                                                       v8::Local<v8::Value>     data      = v8::Local<v8::Value>(),
                                                       v8::Local<v8::Signature> signature = v8::Local<v8::Signature>(),
                                                       int                      length    = 0) {
        return v8::FunctionTemplate::New(isolate, callback, data, signature, length);
    }
};
}; // namespace

namespace no {
template <class T, class A0>
inline v8::Local<T> data(A0 a0) {
    return Factory<T>::data(a0);
}

template <class T, class A0, class A1>
inline v8::Local<T> data(A0 a0, A1 a1) {
    return Factory<T>::data(a0, a1);
}

template <class T, class A0, class A1, class A2>
inline v8::Local<T> data(A0 a0, A1 a1, A2 a2) {
    return Factory<T>::data(a0, a1, a2);
}

template <class T, class A0, class A1, class A2, class A3>
inline v8::Local<T> data(A0 a0, A1 a1, A2 a2, A3 a3) {
    return Factory<T>::data(a0, a1, a2, a3);
}

enum class property_attribute {
    none        = v8::PropertyAttribute::None,
    read_only   = v8::PropertyAttribute::ReadOnly,
    dont_enum   = v8::PropertyAttribute::DontEnum,
    dont_delete = v8::PropertyAttribute::DontDelete,
};

static property_attribute operator|(property_attribute left, property_attribute right) {
    return static_cast<property_attribute>(static_cast<int>(left) | static_cast<int>(right));
}

static void check_result(v8::Maybe<bool> value) {
    if (value.IsNothing()) {
        throw std::runtime_error("");
    }

    if (!value.ToChecked()) {
        throw std::runtime_error("");
    }
}

template <class T>
v8::Local<T> check_result(v8::MaybeLocal<T> value) {
    if (value.IsEmpty()) {
        throw std::runtime_error("");
    }

    return value.ToLocalChecked();
}

static v8::Local<v8::String> data(v8::Isolate*       isolate,
                                  const std::string& value) {
    return no::check_result(v8::String::NewFromUtf8(isolate,
                                                    value.c_str(),
                                                    v8::NewStringType::kNormal,
                                                    static_cast<int>(value.size())));
}

static v8::Local<v8::Value> data(v8::Isolate*                            isolate,
                                 const std::optional<const std::string>& value,
                                 v8::NewStringType                       type = v8::NewStringType::kNormal) {
    if (value.has_value())
        return no::data(isolate, value.value(), type);

    return v8::Undefined(isolate);
}

static v8::Local<v8::String> name(v8::Isolate*       isolate,
                                  const std::string& value) {
    return no::check_result(v8::String::NewFromUtf8(isolate,
                                                    value.c_str(),
                                                    v8::NewStringType::kInternalized,
                                                    static_cast<int>(value.size())));
}

template <std::size_t N>
static v8::Local<v8::String> name(v8::Isolate* isolate,
                                  const char (&value)[N]) {
    return v8::String::NewFromUtf8(isolate, value, v8::NewStringType::kInternalized, N - 1).ToLocalChecked();
}

static v8::Local<v8::Value> data(v8::Isolate* isolate,
                                 const char*  value) {
    if (value == nullptr)
        return v8::Undefined(isolate);

    return v8::String::NewFromUtf8(isolate, value, v8::NewStringType::kNormal, -1).ToLocalChecked();
}

static v8::Local<v8::Value> data(v8::Isolate*      isolate,
                                 const char*       value,
                                 v8::NewStringType type,
                                 int               size = -1) {
    if (value == nullptr)
        return v8::Undefined(isolate);

    return v8::String::NewFromUtf8(isolate, value, type, size).ToLocalChecked();
}

static v8::Local<v8::String> data(v8::Isolate*      isolate,
                                  const char*       value,
                                  int               size,
                                  v8::NewStringType type = v8::NewStringType::kNormal) {
    return v8::String::NewFromUtf8(isolate, value, type, size).ToLocalChecked();
}

static v8::Local<v8::Boolean> data(v8::Isolate* isolate, bool value) {
    return v8::Boolean::New(isolate, value);
}

static v8::Local<v8::Integer> data(v8::Isolate* isolate, int32_t value) {
    return v8::Integer::New(isolate, value);
}

static v8::Local<v8::Value> data(v8::Isolate*            isolate,
                                 std::optional<int32_t>& value) {
    if (value.has_value())
        return no::data(isolate, *value);

    return v8::Undefined(isolate);
}

static v8::Local<v8::Integer> data(v8::Isolate* isolate, uint32_t value) {
    return v8::Integer::NewFromUnsigned(isolate, value);
}

static v8::Local<v8::Value> data(v8::Isolate* isolate, int64_t value) {
    if (value > INT32_MAX)
        return no::data(isolate, std::to_string(value));
    else
        return no::data(isolate, static_cast<int32_t>(value));
}

static v8::Local<v8::Number> data(v8::Isolate* isolate, double value) {
    return v8::Number::New(isolate, value);
}

static v8::Local<v8::External> data(v8::Isolate* isolate, void* value) {
    return v8::External::New(isolate, value);
}
} // namespace no
