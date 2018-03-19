#pragma once

#include <functional>

#include <node/error.hpp>
#include <node/v8.hpp>

namespace no {
template <class T>
struct callable_wrapper {
    T value;
};

template <class T>
struct weak_data {
    v8::Global<v8::Value> global;
    std::shared_ptr<T>    instance;
};

template <typename T>
bool is_empty_weak_ptr(std::weak_ptr<T> const& weak) {
    using wt = std::weak_ptr<T>;
    return !weak.owner_before(wt{}) && !wt{}.owner_before(weak);
}

template <class T>
class class_builder {
  public:
    template <class F>
    class_builder(v8::Isolate*       isolate,
                  const std::string& name,
                  F                  constructor = default_constructor)
        : _isolate(isolate)
        , _template(create_template(isolate, name, constructor))
        , _signature(v8::Signature::New(isolate, _template))
        , _prototype(_template->PrototypeTemplate()) {}

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
    template <class F>
    static v8::Local<v8::FunctionTemplate> create_template(v8::Isolate*       isolate,
                                                           const std::string& name,
                                                           F                  constructor) {
        auto result = no::New<v8::FunctionTemplate>(isolate,
                                                    invoke_constructor<F>,
                                                    no::New(isolate, new callable_wrapper<F>{constructor}));

        result->SetClassName(no::New(isolate, name));
        result->ReadOnlyPrototype();

        result->InstanceTemplate()->SetInternalFieldCount(1);

        return result;
    }

    static T* default_constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
        return new T();
    }

    template <class F>
    static void invoke_constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();
        if (!args.IsConstructCall()) {
            auto message = "Class constructor cannot be invoked without 'new'";
            isolate->ThrowException(v8::Exception::TypeError(no::New(isolate, message).As<v8::String>()));
            return;
        }

        auto constructor = static_cast<callable_wrapper<F>*>(args.Data().As<v8::External>()->Value());
        try {
            auto instance = std::invoke(constructor->value, args);
            args.This()->SetAlignedPointerInInternalField(0, instance.get());

            auto data = new weak_data<T>();

            auto global = v8::Global<v8::Value>(isolate, args.This());
            global.SetWeak(data, weak_callback, v8::WeakCallbackType::kParameter);

            data->global   = std::move(global);
            data->instance = std::move(instance);
        } catch (...) {
            isolate->ThrowException(v8::Exception::TypeError(no::New(isolate, "error invoking constructor").As<v8::String>()));
        }
    }

    static void weak_callback(const v8::WeakCallbackInfo<weak_data<T>>& data) {
        delete data.GetParameter();
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
