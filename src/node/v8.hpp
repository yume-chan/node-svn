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
    static inline v8::Local<v8::Object> New(v8::Isolate* isolate) {
        return v8::Object::New(isolate);
    }
};

template <>
struct Factory<v8::External> {
    static inline v8::Local<v8::External> New(v8::Isolate* isolate,
                                              void*        value) {
        return v8::External::New(isolate, value);
    }
};

template <>
struct Factory<v8::Promise::Resolver> {
    static inline v8::Local<v8::Promise::Resolver> New(v8::Local<v8::Context>& context) {
        return v8::Promise::Resolver::New(context).ToLocalChecked();
    }
};

template <>
struct Factory<v8::Array> {
    static inline v8::Local<v8::Array> New(v8::Isolate* isolate, int length = 0) {
        return v8::Array::New(isolate, length);
    }
};

template <>
struct Factory<v8::Function> {
    static inline v8::Local<v8::Function> New(v8::Local<v8::Context>& context,
                                              v8::FunctionCallback    callback,
                                              v8::Local<v8::Value>    data   = v8::Local<v8::Value>(),
                                              int                     length = 0) {
        return v8::Function::New(context, callback, data, length).ToLocalChecked();
    }
};

template <>
struct Factory<v8::FunctionTemplate> {
    static inline v8::Local<v8::FunctionTemplate> New(v8::Isolate*             isolate,
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
inline v8::Local<T> New(A0 a0) {
    return Factory<T>::New(a0);
}

template <class T, class A0, class A1>
inline v8::Local<T> New(A0 a0, A1 a1) {
    return Factory<T>::New(a0, a1);
}

template <class T, class A0, class A1, class A2>
inline v8::Local<T> New(A0 a0, A1 a1, A2 a2) {
    return Factory<T>::New(a0, a1, a2);
}

template <class T, class A0, class A1, class A2, class A3>
inline v8::Local<T> New(A0 a0, A1 a1, A2 a2, A3 a3) {
    return Factory<T>::New(a0, a1, a2, a3);
}

struct PropertyAttribute {
    static const v8::PropertyAttribute None               = v8::PropertyAttribute::None;
    static const v8::PropertyAttribute ReadOnly           = v8::PropertyAttribute::ReadOnly;
    static const v8::PropertyAttribute DontEnum           = v8::PropertyAttribute::DontEnum;
    static const v8::PropertyAttribute DontDelete         = v8::PropertyAttribute::DontDelete;
    static const v8::PropertyAttribute ReadOnlyDontEnum   = static_cast<v8::PropertyAttribute>(ReadOnly | DontEnum);
    static const v8::PropertyAttribute ReadOnlyDontDelete = static_cast<v8::PropertyAttribute>(ReadOnly | DontDelete);
    static const v8::PropertyAttribute DontEnumDontDelete = static_cast<v8::PropertyAttribute>(DontEnum | DontDelete);
    static const v8::PropertyAttribute All                = static_cast<v8::PropertyAttribute>(ReadOnly | DontEnum | DontDelete);
};

static v8::Local<v8::String> New(v8::Isolate*       isolate,
                                 const std::string& value,
                                 v8::NewStringType  type = v8::NewStringType::kNormal) {
    return v8::String::NewFromUtf8(isolate, value.c_str(), type, static_cast<int>(value.size())).ToLocalChecked();
}

static v8::Local<v8::Value> New(v8::Isolate*                            isolate,
                                const std::optional<const std::string>& value,
                                v8::NewStringType                       type = v8::NewStringType::kNormal) {
    if (value.has_value())
        return no::New(isolate, value.value(), type);

    return v8::Undefined(isolate);
}

template <std::size_t N>
static v8::Local<v8::String> NewName(v8::Isolate* isolate,
                                     const char (&value)[N]) {
    return v8::String::NewFromUtf8(isolate, value, v8::NewStringType::kInternalized, N - 1).ToLocalChecked();
}

static v8::Local<v8::Value> New(v8::Isolate* isolate,
                                const char*  value) {
    if (value == nullptr)
        return v8::Undefined(isolate);

    return v8::String::NewFromUtf8(isolate, value, v8::NewStringType::kNormal, -1).ToLocalChecked();
}

static v8::Local<v8::Value> New(v8::Isolate*      isolate,
                                const char*       value,
                                v8::NewStringType type,
                                int               size = -1) {
    if (value == nullptr)
        return v8::Undefined(isolate);

    return v8::String::NewFromUtf8(isolate, value, type, size).ToLocalChecked();
}

static v8::Local<v8::String> New(v8::Isolate*      isolate,
                                 const char*       value,
                                 int               size,
                                 v8::NewStringType type = v8::NewStringType::kNormal) {
    return v8::String::NewFromUtf8(isolate, value, type, size).ToLocalChecked();
}

static v8::Local<v8::Boolean> New(v8::Isolate* isolate, bool value) {
    return v8::Boolean::New(isolate, value);
}

static v8::Local<v8::Integer> New(v8::Isolate* isolate, int32_t value) {
    return v8::Integer::New(isolate, value);
}

static v8::Local<v8::Value> New(v8::Isolate*            isolate,
                                std::optional<int32_t>& value) {
    if (value.has_value())
        return no::New(isolate, *value);

    return v8::Undefined(isolate);
}

static v8::Local<v8::Integer> New(v8::Isolate* isolate, uint32_t value) {
    return v8::Integer::NewFromUnsigned(isolate, value);
}

static v8::Local<v8::Value> New(v8::Isolate* isolate, int64_t value) {
    if (value > INT32_MAX)
        return no::New(isolate, std::to_string(value));
    else
        return no::New(isolate, static_cast<int32_t>(value));
}

static v8::Local<v8::Number> New(v8::Isolate* isolate, double value) {
    return v8::Number::New(isolate, value);
}

static v8::Local<v8::External> New(v8::Isolate* isolate, void* value) {
    return v8::External::New(isolate, value);
}
} // namespace no
