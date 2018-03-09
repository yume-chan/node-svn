#pragma once

#include <future>
#include <iostream>

#include <node_object_wrap.h>

#include <node/v8.hpp>
#include <uv/async.hpp>

#define INTERNALIZED_STRING(value) \
    v8::New(isolate, value, sizeof(value) - 1, v8::NewStringType::kInternalized)

#define SET_PROTOTYPE_METHOD(signature, prototype, name, callback, length)                \
    /* Add a scope to hide extra variables */                                             \
    {                                                                                     \
        auto function = v8::FunctionTemplate::New(isolate,                /* isolate */   \
                                                  callback,               /* callback */  \
                                                  v8::Local<v8::Value>(), /* data */      \
                                                  signature,              /* signature */ \
                                                  length);                /* length */    \
        function->RemovePrototype();                                                      \
        prototype->Set(name, function, v8::PropertyAttribute::DontEnum);                  \
    }

namespace node {
class iterator : public node::ObjectWrap {
  public:
    iterator(v8::Isolate* isolate, v8::Local<v8::Context>& context) {
        if (_initializer.IsEmpty()) {
            auto context = isolate->GetCurrentContext();

            auto clazz     = v8::New<v8::FunctionTemplate>(isolate, create_instance);
            auto signature = v8::Signature::New(isolate, clazz);

            clazz->SetClassName(INTERNALIZED_STRING("Iterator"));
            clazz->ReadOnlyPrototype();

            clazz->InstanceTemplate()->SetInternalFieldCount(1);

            auto prototype = clazz->PrototypeTemplate();
            SET_PROTOTYPE_METHOD(signature, prototype, v8::Symbol::GetIterator(isolate), get_iterator, 0);
            SET_PROTOTYPE_METHOD(signature, prototype, INTERNALIZED_STRING("next"), next, 0);

            _initializer.Reset(isolate, clazz->GetFunction(context).ToLocalChecked());
        }

        const auto           argc       = 1;
        v8::Local<v8::Value> argv[argc] = {v8::New(isolate, this)};
        _iterator.Reset(isolate, _initializer.Get(isolate)->CallAsConstructor(context, argc, argv).ToLocalChecked());
    }

  private:
    static void create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();

        auto result = static_cast<iterator*>(args[0].As<v8::External>()->Value());
        result->Wrap(args.This());
    }

    static void get_iterator(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto _this = node::ObjectWrap::Unwrap<iterator>(args.Holder());

        if (_this->_iterator_created) {
            auto isolate = args.GetIsolate();
            isolate->ThrowException(v8::New(isolate, "You can only get async_iterator once").As<v8::String>());
            return;
        }

        _this->_iterator_created = true;
        args.GetReturnValue().Set(args.Holder());
    }

    static void next(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static v8::Global<v8::Function> _initializer;

    bool                              _iterator_created;
    std::promise<void>                _consume_promise;
    bool                              _has_unconsumed_value;
    v8::Global<v8::Value>             _iterator;
    v8::Global<v8::Value>             _next_value;
    v8::Global<v8::Promise::Resolver> _pending_resolver;
};

class async_iterator : public node::ObjectWrap {
  public:
    async_iterator(v8::Isolate* isolate, v8::Local<v8::Context>& context)
        : _isolate(isolate)
        , _iterator_created(false)
        , _consume_promise()
        , _has_unconsumed_value(false)
        , _iterator()
        , _next_value()
        , _pending_resolver() {
        if (_initializer.IsEmpty()) {
            auto context = isolate->GetCurrentContext();

            auto clazz     = v8::New<v8::FunctionTemplate>(isolate, create_instance);
            auto signature = v8::Signature::New(isolate, clazz);

            clazz->SetClassName(INTERNALIZED_STRING("AsyncIterator"));
            clazz->ReadOnlyPrototype();

            clazz->InstanceTemplate()->SetInternalFieldCount(1);

            auto symbol        = context->Global()->Get(INTERNALIZED_STRING("Symbol")).As<v8::Object>();
            auto asyncIterator = symbol->Get(context, INTERNALIZED_STRING("asyncIterator")).ToLocalChecked().As<v8::Symbol>();

            if (asyncIterator->IsUndefined()) {
                asyncIterator = v8::Symbol::New(isolate, INTERNALIZED_STRING("Symbol.asyncIterator"));
                symbol->DefineOwnProperty(context, INTERNALIZED_STRING("asyncIterator"), asyncIterator, v8::PropertyAttributeEx::All);
            }

            auto prototype = clazz->PrototypeTemplate();
            SET_PROTOTYPE_METHOD(signature, prototype, asyncIterator, get_iterator, 0);
            SET_PROTOTYPE_METHOD(signature, prototype, INTERNALIZED_STRING("next"), next, 0);

            _initializer.Reset(isolate, clazz->GetFunction(context).ToLocalChecked());
        }

        const auto           argc       = 1;
        v8::Local<v8::Value> argv[argc] = {v8::New(isolate, this)};

        auto value = _initializer.Get(isolate)->CallAsConstructor(context, argc, argv).ToLocalChecked();
        _iterator.Reset(isolate, value);
    }

    template <class T>
    std::future<void> resolve(v8::Local<T>& value,
                              bool          done) {
        if (_has_unconsumed_value) {
            throw std::runtime_error("");
        }

        auto isolate = _isolate;
        auto context = isolate->GetCurrentContext();

        _consume_promise = std::promise<void>();

        if (_next_value.IsEmpty()) {
            _next_value.Reset(isolate, value.As<v8::Value>());

            _has_unconsumed_value = false;
            _consume_promise.set_value();

            return _consume_promise.get_future();
        }

        v8::Local<v8::Promise::Resolver> resolver;

        if (_pending_resolver.IsEmpty()) {
            resolver = v8::New<v8::Promise::Resolver>(context);
            _pending_resolver.Reset(isolate, resolver);

            _has_unconsumed_value = true;
        } else {
            resolver = _pending_resolver.Get(isolate);
            _pending_resolver.Reset();

            _has_unconsumed_value = false;
            _consume_promise.set_value();
        }

        auto result = v8::New<v8::Object>(isolate);
        result->Set(INTERNALIZED_STRING("value"), _next_value.Get(isolate));
        result->Set(INTERNALIZED_STRING("done"), v8::New(isolate, done));

        resolver->Resolve(result);

        _next_value.Reset(isolate, value);
        return _consume_promise.get_future();
    }

    v8::Local<v8::Value> get() {
        return _iterator.Get(_isolate);
    }

  private:
    static void create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();

        auto result = static_cast<async_iterator*>(args[0].As<v8::External>()->Value());
        result->Wrap(args.This());
    }

    static void get_iterator(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto _this   = node::ObjectWrap::Unwrap<async_iterator>(args.Holder());
        auto isolate = _this->_isolate;

        if (_this->_iterator_created) {
            isolate->ThrowException(v8::New(isolate, "You can only get async_iterator once").As<v8::String>());
            return;
        }

        _this->_iterator_created = true;
        args.GetReturnValue().Set(args.Holder());
    }

    static void next(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();
        auto context = isolate->GetCurrentContext();

        auto _this = node::ObjectWrap::Unwrap<async_iterator>(args.Holder());

        if (_this->_has_unconsumed_value) {
            auto promise = _this->_pending_resolver.Get(isolate)->GetPromise();
            args.GetReturnValue().Set(promise);

            _this->_pending_resolver.Reset();

            _this->_has_unconsumed_value = false;
            _this->_consume_promise.set_value();
            return;
        }

        auto resolver = v8::New<v8::Promise::Resolver>(context);
        _this->_pending_resolver.Reset(isolate, resolver);

        args.GetReturnValue().Set(resolver->GetPromise());
    }

    static v8::Global<v8::Function> _initializer;

    v8::Isolate* _isolate;

    v8::Global<v8::Value> _iterator;
    bool                  _iterator_created;

    std::promise<void> _consume_promise;

    v8::Global<v8::Value>             _next_value;
    bool                              _has_unconsumed_value;
    v8::Global<v8::Promise::Resolver> _pending_resolver;
};

v8::Global<v8::Function> async_iterator::_initializer;
} // namespace node
