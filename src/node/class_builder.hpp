#include <functional>

#include <node/error.hpp>
#include <node/v8.hpp>

namespace no {
template <class T>
class class_builder {
    using create_instance = std::function<T*(const v8::FunctionCallbackInfo<v8::Value>& args)>;
    using callback_method = std::function<v8::Local<v8::Value>(T&, const v8::FunctionCallbackInfo<v8::Value>&)>;

  public:
    class_builder(v8::Isolate*       isolate,
                  const std::string& name,
                  create_instance    constructor = default_constructor)
        : _isolate(isolate)
        , _template(create_template(isolate, name, constructor))
        , _signature(v8::Signature::New(isolate, _template))
        , _prototype(_template->PrototypeTemplate()) {}

    void add_prototype_method(const char*     name,
                              callback_method method,
                              int             length = 0) {
        add_prototype_method(no::New(_isolate, name, v8::NewStringType::kInternalized).As<v8::Name>(), method, length);
    }

    void add_prototype_method(v8::Local<v8::Name> name,
                              callback_method     method,
                              int                 length = 0) {
        auto data     = no::New(_isolate, new callback_method(method));
        auto function = v8::FunctionTemplate::New(_isolate,                         // isolate
                                                  invoke_method,                    // callback
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
    static v8::Local<v8::FunctionTemplate> create_template(v8::Isolate*       isolate,
                                                           const std::string& name,
                                                           create_instance    constructor) {
        auto result = no::New<v8::FunctionTemplate>(isolate,
                                                    invoke_constructor,
                                                    no::New(isolate, new create_instance(constructor)));

        result->SetClassName(no::New(isolate, name));
        result->ReadOnlyPrototype();

        result->InstanceTemplate()->SetInternalFieldCount(1);

        return result;
    }

    static T* default_constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
        return new T();
    }

    static void invoke_constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();
        if (!args.IsConstructCall()) {
            auto message = "Class constructor cannot be invoked without 'new'";
            isolate->ThrowException(v8::Exception::TypeError(no::New(isolate, message).As<v8::String>()));
            return;
        }

        auto constructor = static_cast<create_instance*>(args.Data().As<v8::External>()->Value());
        try {
            auto result = (*constructor)(args);
            args.This()->SetAlignedPointerInInternalField(0, result);
        } catch (...) {
            isolate->ThrowException(v8::Exception::TypeError(no::New(isolate, "error invoking constructor").As<v8::String>()));
        }
    }

    static void invoke_method(const v8::FunctionCallbackInfo<v8::Value>& args) {
        auto isolate = args.GetIsolate();

        v8::EscapableHandleScope scope(isolate);

        auto method = static_cast<callback_method*>(args.Data().As<v8::External>()->Value());
        auto _this  = static_cast<T*>(args.Holder()->GetAlignedPointerFromInternalField(0));

        try {
            auto result = (*method)(*_this, args);
            if (!result.IsEmpty()) {
                scope.Escape(result);
                args.GetReturnValue().Set(result);
            }
        } catch (no::type_error& error) {
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
