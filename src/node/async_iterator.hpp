#pragma once

#include <future>
#include <iostream>

#include <node/class_builder.hpp>
#include <uv/async.hpp>

namespace no {
static void check_result(v8::Maybe<bool> value) {
    if (value.IsNothing()) {
        throw std::runtime_error("empty");
    }

    if (!value.ToChecked()) {
        throw std::runtime_error("false");
    }
}

class async_iterator {
  public:
    async_iterator(v8::Isolate* isolate, v8::Local<v8::Context>& context)
        : _isolate(isolate)
        , _value()
        , _value_created(false)
        , _consume_promise()
        , _resolver_fulfilled(false)
        , _resolver() {
        if (_initializer.IsEmpty()) {
            auto asyncIteratorName = no::New(isolate, "asyncIterator", v8::NewStringType::kInternalized);

            // polyfill Symbol.asyncIterator
            auto symbol        = context->Global()->Get(no::New(isolate, "Symbol", v8::NewStringType::kInternalized)).As<v8::Object>();
            auto asyncIterator = symbol->Get(context, asyncIteratorName).ToLocalChecked().As<v8::Symbol>();

            if (asyncIterator->IsUndefined()) {
                asyncIterator = v8::Symbol::New(isolate, no::New(isolate, "Symbol.asyncIterator"));
                symbol->DefineOwnProperty(context, asyncIteratorName, asyncIterator, no::PropertyAttribute::All);
            }

            class_builder<async_iterator> clazz(isolate, "Iterator", create_instance);
            clazz.add_prototype_method(asyncIterator.As<v8::Name>(), &async_iterator::get_iterator);
            clazz.add_prototype_method("next", &async_iterator::next);

            _initializer.Reset(isolate, clazz.get_constructor());
        }

        auto value = _initializer.Get(isolate)->NewInstance(context).ToLocalChecked();
        value->SetAlignedPointerInInternalField(0, this);
        _value.Reset(isolate, value);

        auto resolver = no::New<v8::Promise::Resolver>(context);
        _resolver.Reset(_isolate, resolver);
    }

    template <class T>
    std::future<void> yield(v8::Local<T>& value) {
        return resolve(value, false);
    }

    void end() {
        resolve(v8::Undefined(_isolate), true);
    }

    v8::Local<v8::Value> get() {
        return _value.Get(_isolate);
    }

  private:
    static async_iterator* create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
        return nullptr;
    }

    template <class T>
    std::future<void> resolve(v8::Local<T> value,
                              bool         done) {
        if (_resolver_fulfilled) {
            throw std::runtime_error("");
        }

        v8::HandleScope scope(_isolate);

        auto context = _isolate->GetCurrentContext();

        v8::Local<v8::Promise::Resolver> resolver;
        _consume_promise = std::promise<void>();

        if (_resolver.IsEmpty()) {
            resolver = no::New<v8::Promise::Resolver>(context);
            _resolver.Reset(_isolate, resolver);

            _resolver_fulfilled = true;
        } else {
            resolver = _resolver.Get(_isolate);
            _resolver.Reset();

            _consume_promise.set_value();
        }

        auto object = no::New<v8::Object>(_isolate);
        object->Set(no::New(_isolate, "value"), value);
        object->Set(no::New(_isolate, "done"), no::New(_isolate, done));
        check_result(resolver->Resolve(context, object));

        return _consume_promise.get_future();
    }

    v8::Local<v8::Value> get_iterator(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = this->_isolate;

        if (this->_value_created) {
            isolate->ThrowException(no::New(isolate, "You can only get async_iterator once").As<v8::String>());
            return v8::Local<v8::Value>();
        }

        this->_value_created = true;
        return args.Holder();
    }

    v8::Local<v8::Value> next(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Local<v8::Promise::Resolver> resolver;

        if (_resolver_fulfilled) {
            resolver = _resolver.Get(_isolate);
            _resolver.Reset();

            _resolver_fulfilled = false;
            _consume_promise.set_value();
        } else {
            auto context = _isolate->GetCurrentContext();
            resolver     = no::New<v8::Promise::Resolver>(context);
            _resolver.Reset(_isolate, resolver);
        }

        return resolver;
    }

    static v8::Global<v8::Function> _initializer;

    v8::Isolate* _isolate;

    v8::Global<v8::Value> _value;
    bool                  _value_created;

    v8::Global<v8::Promise::Resolver> _resolver;
    bool                              _resolver_fulfilled;
    std::promise<void>                _consume_promise;
};

v8::Global<v8::Function> async_iterator::_initializer;
} // namespace no
