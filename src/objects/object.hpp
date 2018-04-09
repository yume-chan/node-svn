#pragma once

#include <string>

#include <node/error.hpp>
#include <node/v8.hpp>

namespace no {
class object {
  public:
    struct reference {
        const reference& operator=(const no::object& value) const {
            set(value.value(), no::property_attribute::none);
            return *this;
        }

        template <class T>
        const reference& operator=(v8::Local<T> value) const {
            set(value, no::property_attribute::none);
            return *this;
        }

        template <class T>
        const reference& operator=(T value) const {
            set(no::data(_owner._isolate, value));
            return *this;
        }

        void set(const no::object&      value,
                 no::property_attribute attribute = no::property_attribute::none) const {
            set(value.value(), attribute);
        }

        template <class T>
        void set(v8::Local<T>           value,
                 no::property_attribute attribute = no::property_attribute::none) const {
            no::check_result(_owner._value->DefineOwnProperty(_owner._context,
                                                              _name,
                                                              value,
                                                              static_cast<v8::PropertyAttribute>(attribute)));
        }

        template <class T>
        void set(T                      value,
                 no::property_attribute attribute = no::property_attribute::none) const {
            set(no::data(_owner._isolate, value), attribute);
        }

        v8::Local<v8::Value> get() const {
            return no::check_result(_owner._value->Get(_owner._context, _name));
        }

        operator v8::Local<v8::Value>() const {
            return get();
        }

        v8::Value* operator->() const {
            return get().operator->();
        }

        template <class T>
        struct as_type { using type = v8::Local<T>; };

        template <>
        struct as_type<no::object> { using type = no::object; };

        template <class T>
        using as_type_v = typename as_type<T>::type;

        template <class T>
        as_type_v<T> as() {
            return get().As<T>();
        }

        template <>
        no::object as<no::object>() {
            return no::object(as<v8::Object>());
        }

      private:
        friend class object;

        explicit reference(const object& owner, v8::Local<v8::Name> name)
            : _owner(owner)
            , _name(name) {}

        const object&       _owner;
        v8::Local<v8::Name> _name;
    };

    explicit object(v8::Isolate* isolate)
        : _isolate(isolate)
        , _context(isolate->GetCurrentContext())
        , _value(v8::Object::New(isolate)) {}

    explicit object(v8::Local<v8::Object> value)
        : _isolate(value->GetIsolate())
        , _context(_isolate->GetCurrentContext())
        , _value(value) {}

    template <size_t N>
    reference operator[](const char (&name)[N]) const {
        return reference(*this, no::name(_isolate, name));
    }

    reference operator[](const std::string& name) const {
        return reference(*this, no::name(_isolate, name));
    }

    reference operator[](v8::Local<v8::Name> name) const {
        return reference(*this, name);
    }

    v8::Local<v8::Object> value() const {
        return _value;
    }

    operator v8::Local<v8::Object>() const {
        return value();
    }

    operator v8::Local<v8::Value>() const {
        return value();
    }

    v8::Isolate* isolate() const {
        return _isolate;
    }

    v8::Local<v8::Context> context() const {
        return _context;
    }

  protected:
    v8::Isolate* const     _isolate;
    v8::Local<v8::Context> _context;
    v8::Local<v8::Object>  _value;
};
} // namespace no
