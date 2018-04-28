#pragma once

#include <future>
#include <iostream>

#include <objects/class_builder.hpp>

#include <uv/async.hpp>

namespace no {
class iterable : public std::enable_shared_from_this<iterable> {
  public:
    static std::shared_ptr<iterable> create(v8::Isolate*            isolate,
                                            v8::Local<v8::Context>& context) {
        return std::shared_ptr<iterable>(new iterable(isolate, context));
    }

    [[nodiscard]] std::future<void> yield(v8::Local<v8::Value> value) {
        return resolve(true, value, false);
    }

    void end() {
        resolve(true, v8::Undefined(_isolate), true);
    }

    void reject(v8::Local<v8::Value> exception) {
        resolve(false, exception, false);
    }

    v8::Local<v8::Value> get() {
        if (_value.IsEmpty()) {
            v8::HandleScope scope(_isolate);

            auto context = _isolate->GetCurrentContext();

            const auto           argc       = 1;
            v8::Local<v8::Value> argv[argc] = {no::data(_isolate, this)};

            auto value = _initializer.Get(_isolate)->NewInstance(context, argc, argv).ToLocalChecked();
            _value.Reset(_isolate, value);
        }
        return _value.Get(_isolate);
    }

    bool valid() const {
        return !_iterator_released;
    }

    v8::Isolate* isolate() const {
        return _isolate;
    }

    v8::Local<v8::Context> context() const {
        return _context.Get(_isolate);
    }

  private:
    explicit iterable(v8::Isolate* isolate, v8::Local<v8::Context>& context)
        : _isolate(isolate)
        , _context(isolate, context)
        , _value()
        , _iterator_created(false)
        , _iterator_released(false)
        , _resolver()
        , _resolver_fulfilled(false)
        , _consume_promise() {
        if (_initializer.IsEmpty()) {
            auto name_asyncIterator = no::name(isolate, "asyncIterator");

            // polyfill Symbol.asyncIterator
            auto global        = no::object(context->Global());
            auto symbol        = global["Symbol"].as<no::object>();
            auto asyncIterator = symbol[name_asyncIterator];

            if (asyncIterator->IsUndefined()) {
                asyncIterator.set(v8::Symbol::New(isolate, no::name(isolate, "Symbol.asyncIterator")),
                                  no::property_attribute::read_only | no::property_attribute::dont_enum | no::property_attribute::dont_delete);
            }

            class_builder<iterable> clazz(isolate, "Iterator", constructor, &iterable::destructor);
            clazz.add_prototype_method(asyncIterator.as<v8::Name>(), &iterable::get_async_iterator);
            clazz.add_prototype_method("next", &iterable::next);

            _initializer.Reset(isolate, clazz.get_constructor());
        }
    }

    static std::shared_ptr<iterable> constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
        return static_cast<iterable*>(args[0].As<v8::External>()->Value())->shared_from_this();
    }

    void destructor() {
        if (_resolver_fulfilled) {
            _consume_promise.set_exception(std::make_exception_ptr(std::runtime_error("")));
        }

        _iterator_released = true;
    }

    std::future<void> resolve(bool                 success,
                              v8::Local<v8::Value> value,
                              bool                 done) {
        // already have a value waiting JS side to retrive
        if (_resolver_fulfilled) {
            throw std::runtime_error("");
        }

        // JS side has released their handle, it's invalid now
        if (_iterator_released) {
            throw std::runtime_error("");
        }

        v8::HandleScope scope(_isolate);

        auto context = _context.Get(_isolate);

        v8::Local<v8::Promise::Resolver> resolver;
        _consume_promise = std::promise<void>();

        if (_resolver.IsEmpty()) {
            resolver = no::data<v8::Promise::Resolver>(context);
            _resolver.Reset(_isolate, resolver);

            _resolver_fulfilled = true;
        } else {
            resolver = _resolver.Get(_isolate);
            _resolver.Reset();

            _consume_promise.set_value();
        }

        if (success) {
            auto object = no::data<v8::Object>(_isolate);
            object->Set(no::data(_isolate, "value"), value);
            object->Set(no::data(_isolate, "done"), no::data(_isolate, done));
            check_result(resolver->Resolve(context, object));
        } else {
            check_result(resolver->Reject(context, value));
        }

        _isolate->RunMicrotasks();

        return _consume_promise.get_future();
    }

    v8::Local<v8::Value> get_async_iterator(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (this->_iterator_created) {
            _isolate->ThrowException(no::data(_isolate, "You can only get iterable once").As<v8::String>());
            return v8::Local<v8::Value>();
        }

        this->_iterator_created = true;
        return args.Holder();
    }

    v8::Local<v8::Value> next(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();

        v8::Local<v8::Promise::Resolver> resolver;

        if (_resolver_fulfilled) {
            resolver = _resolver.Get(isolate);
            _resolver.Reset();

            _resolver_fulfilled = false;
            _consume_promise.set_value();
        } else {
            auto context = _context.Get(_isolate);
            resolver     = no::data<v8::Promise::Resolver>(context);
            _resolver.Reset(isolate, resolver);
        }

        return resolver;
    }

    static v8::Global<v8::Function> _initializer;

    v8::Isolate*            _isolate;
    v8::Global<v8::Context> _context;

    v8::Global<v8::Value> _value;

    bool _iterator_created;
    bool _iterator_released;

    v8::Global<v8::Promise::Resolver> _resolver;
    bool                              _resolver_fulfilled;

    std::promise<void> _consume_promise;
};

v8::Global<v8::Function> iterable::_initializer;
} // namespace no
