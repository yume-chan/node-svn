#pragma once

#include <functional>

#include <cpp/malloc.hpp>

#include <node/error.hpp>
#include <node/v8.hpp>

namespace no {
template <class T>
struct callable_wrapper {
    T value;
};

template <class TC, class TD>
struct class_info {
    TC constructor;
    TD destructor;
};

template <class T, class F>
struct weak_data {
    weak_data(v8::Isolate*           isolate,
              v8::Local<v8::Object>& value,
              std::shared_ptr<T>&    instance,
              F                      destructor)
        : isolate(isolate)
        , handle(v8::Global<v8::Object>(isolate, value))
        , instance(instance)
        , destructor(destructor) {
        isolate->AdjustAmountOfExternalAllocatedMemory(10 * memory_delta.exchange(0));

        handle.SetWeak(this, weak_callback, v8::WeakCallbackType::kParameter);
        handle.MarkIndependent();
    }

    v8::Isolate*           isolate;
    v8::Global<v8::Object> handle;
    std::shared_ptr<T>     instance;
    F                      destructor;

  private:
    static void weak_callback(const v8::WeakCallbackInfo<weak_data<T, F>>& info) {
        auto data = info.GetParameter();
        std::invoke(data->destructor, data->instance.get());
        delete data;

        auto isolate = info.GetIsolate();
        isolate->AdjustAmountOfExternalAllocatedMemory(10 * memory_delta.exchange(0));
    }
};

template <class T>
class class_builder {
  public:
    template <int32_t size, class TC, class TD>
    explicit class_builder(v8::Isolate* isolate,
                           const char (&name)[size],
                           TC constructor,
                           TD destructor)
        : _isolate(isolate) {
        auto info = new class_info<TC, TD>{constructor, destructor};
        _template = no::New<v8::FunctionTemplate>(isolate,
                                                  invoke_constructor<TC, TD>,
                                                  no::New(isolate, info));

        _template->SetClassName(no::NewName(isolate, name));
        _template->ReadOnlyPrototype();

        _template->InstanceTemplate()->SetInternalFieldCount(1);

        _signature = v8::Signature::New(isolate, _template);
        _prototype = _template->PrototypeTemplate();
    }

    template <int32_t size, class TC>
    explicit class_builder(v8::Isolate* isolate,
                           const char (&name)[size],
                           TC constructor)
        : class_builder(isolate, name, constructor, default_destructor) {
    }

    template <int32_t size>
    explicit class_builder(v8::Isolate* isolate,
                           const char (&name)[size])
        : class_builder(isolate, name, default_constructor, default_destructor) {
    }

    template <size_t N, class F>
    void add_prototype_method(const char (&name)[N],
                              F   method,
                              int length = 0) {
        add_prototype_method(no::NewName(_isolate, name),
                             method,
                             length);
    }

    template <class F>
    void add_prototype_method(v8::Local<v8::Name> name,
                              F                   method,
                              int                 length = 0) {
        auto data     = no::New(_isolate, new callable_wrapper<F>{method});
        auto function = v8::FunctionTemplate::New(_isolate,                         // isolate
                                                  invoke_method<F>,                 // callback
                                                  data,                             // data
                                                  _signature,                       // signature
                                                  length,                           // length
                                                  v8::ConstructorBehavior::kThrow); // behavior
        function->RemovePrototype();
        _prototype->Set(name, function, v8::PropertyAttribute::DontEnum);
    }

    v8::Local<v8::Function> get_constructor() {
        return _template->GetFunction();
    }

  private:
    static std::shared_ptr<T> default_constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
        return std::make_shared<T>();
    }

    static void default_destructor(T* value) {
    }

    template <class TC, class TD>
    static void invoke_constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();
        if (!args.IsConstructCall()) {
            auto message = "Class constructor cannot be invoked without 'new'";
            isolate->ThrowException(v8::Exception::TypeError(no::New(isolate, message).As<v8::String>()));
            return;
        }

        auto info = static_cast<class_info<TC, TD>*>(args.Data().As<v8::External>()->Value());
        try {
            auto value    = args.This();
            auto instance = std::invoke(info->constructor, args);
            value->SetAlignedPointerInInternalField(0, instance.get());

            new weak_data<T, TD>(isolate, value, instance, info->destructor);
        } catch (...) {
            isolate->ThrowException(v8::Exception::TypeError(no::New(isolate, "error invoking constructor").As<v8::String>()));
        }
    }

    template <class F>
    static void invoke_method(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();

        v8::EscapableHandleScope scope(isolate);

        auto method = static_cast<callable_wrapper<F>*>(args.Data().As<v8::External>()->Value());
        auto _this  = static_cast<T*>(args.Holder()->GetAlignedPointerFromInternalField(0));

        try {
            auto result = std::invoke(method->value, *_this, args);
            if (!result.IsEmpty()) {
                scope.Escape(result);
                args.GetReturnValue().Set(result);
            }
        } catch (const no::type_error& error) {
            isolate->ThrowException(v8::Exception::TypeError(no::New(isolate, error.what()).As<v8::String>()));
        } catch (...) {
            isolate->ThrowException(v8::Exception::Error(no::New(isolate, "error invoking method").As<v8::String>()));
        }
    }

    v8::Isolate* _isolate;

    v8::Local<v8::FunctionTemplate> _template;
    v8::Local<v8::Signature>        _signature;
    v8::Local<v8::ObjectTemplate>   _prototype;
};
} // namespace no
