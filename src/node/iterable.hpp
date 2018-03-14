#pragma once

#include <node_object_wrap.h>

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

struct iterator_status {
    v8::Global<v8::Value>             value;
    v8::Global<v8::Promise::Resolver> resolver;
    bool                              resolver_fulfilled;
    std::promise<void>                consume_promise;
};

using shared_iterator_status = std::shared_ptr<iterator_status>;

class iterable {
  public:
    iterable(v8::Isolate* isolate, v8::Local<v8::Context>& context)
        : _isolate(isolate)
        , _iterator_created(false)
        , _status(std::make_shared<iterator_status>()) {
        if (_initializer.IsEmpty()) {
            auto asyncIteratorName = no::New(isolate, "asyncIterator", v8::NewStringType::kInternalized);

            // polyfill Symbol.asyncIterator
            auto symbol        = context->Global()->Get(no::New(isolate, "Symbol", v8::NewStringType::kInternalized)).As<v8::Object>();
            auto asyncIterator = symbol->Get(context, asyncIteratorName).ToLocalChecked().As<v8::Symbol>();

            if (asyncIterator->IsUndefined()) {
                asyncIterator = v8::Symbol::New(isolate, no::New(isolate, "Symbol.asyncIterator"));
                symbol->DefineOwnProperty(context, asyncIteratorName, asyncIterator, no::PropertyAttribute::All);
            }

            class_builder<iterable> clazz(isolate, "Iterator", create_instance);
            clazz.add_prototype_method(asyncIterator.As<v8::Name>(), &iterable::get_async_iterator);
            clazz.add_prototype_method("next", &iterable::next);

            _initializer.Reset(isolate, clazz.get_constructor());
        }
    }

    std::future<void> yield(v8::Local<v8::Value> value) {
        return resolve(true, value, false);
    }

    void end() {
        resolve(true, v8::Undefined(_isolate), true);
    }

    void reject(v8::Local<v8::Value> exception) {
        resolve(false, exception, false);
    }

    v8::Local<v8::Value> get() {
        if (_status->value.IsEmpty()) {
            v8::HandleScope scope(_isolate);

            auto context = _isolate->GetCurrentContext();
            auto value   = _initializer.Get(_isolate)->NewInstance(context).ToLocalChecked();
            value->SetAlignedPointerInInternalField(0, this);

            // due to the nature of AsyncItertor, it's highly possible that
            // it will live longer at JavaScript side than native side.
            // so make a copy of the smart pointer and wait for v8 garbage collection.
            // (It can also happen in reverse, that why shared pointer is used.)
            _status->value.Reset(_isolate, value);
            _status->value.SetWeak(new shared_iterator_status(_status), weak_callback, v8::WeakCallbackType::kParameter);
        }
        return _status->value.Get(_isolate);
    }

  private:
    static iterable* create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
        return nullptr;
    }

    static void weak_callback(const v8::WeakCallbackInfo<shared_iterator_status>& data) {
        delete data.GetParameter();
    }

    std::future<void> resolve(bool                 success,
                              v8::Local<v8::Value> value,
                              bool                 done) {
        if (_status->resolver_fulfilled) {
            throw std::runtime_error("");
        }

        v8::HandleScope scope(_isolate);

        auto context = _isolate->GetCurrentContext();

        v8::Local<v8::Promise::Resolver> resolver;
        _status->consume_promise = std::promise<void>();

        if (_status->resolver.IsEmpty()) {
            resolver = no::New<v8::Promise::Resolver>(context);
            _status->resolver.Reset(_isolate, resolver);

            _status->resolver_fulfilled = true;
        } else {
            resolver = _status->resolver.Get(_isolate);
            _status->resolver.Reset();

            _status->consume_promise.set_value();
        }

        if (success) {
            auto object = no::New<v8::Object>(_isolate);
            object->Set(no::New(_isolate, "value"), value);
            object->Set(no::New(_isolate, "done"), no::New(_isolate, done));
            check_result(resolver->Resolve(context, object));
        } else {
            check_result(resolver->Reject(context, value));
        }

        // **crucial!** v8 don't think it's idle so it won't run microtasks.
        // force it to run, or we will stuck on waiting `_consume_promise` forever.
        _isolate->RunMicrotasks();

        return _status->consume_promise.get_future();
    }

    v8::Local<v8::Value> get_async_iterator(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (this->_iterator_created) {
            _isolate->ThrowException(no::New(_isolate, "You can only get iterable once").As<v8::String>());
            return v8::Local<v8::Value>();
        }

        this->_iterator_created = true;
        return args.Holder();
    }

    v8::Local<v8::Value> next(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();

        v8::Local<v8::Promise::Resolver> resolver;

        if (_status->resolver_fulfilled) {
            resolver = _status->resolver.Get(isolate);
            _status->resolver.Reset();

            _status->resolver_fulfilled = false;
            _status->consume_promise.set_value();
        } else {
            auto context = isolate->GetCurrentContext();
            resolver     = no::New<v8::Promise::Resolver>(context);
            _status->resolver.Reset(isolate, resolver);
        }

        return resolver;
    }

    static v8::Global<v8::Function> _initializer;

    v8::Isolate* _isolate;

    bool _iterator_created;

    shared_iterator_status _status;
};

v8::Global<v8::Function> iterable::_initializer;
} // namespace no
