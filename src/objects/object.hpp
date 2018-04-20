#pragma once

#include <string>

#include <node/error.hpp>
#include <node/v8.hpp>

namespace no {
template <class T>
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

class object {
  public:
    struct reference {
        template <class T>
        reference& operator=(T value) {
            return set(value, no::property_attribute::none);
        }

        template <class T>
        reference& set(T                      input,
                       no::property_attribute attribute = no::property_attribute::none) {
            v8::Local<v8::Value> value;

            if constexpr (std::is_same_v<no::remove_cvref_t<T>, no::object::reference>) {
                value = input.get();
            } else if constexpr (std::is_convertible_v<T, v8::Local<v8::Value>>) {
                value = static_cast<v8::Local<v8::Value>>(input);
            } else {
                value = no::data(_owner._isolate, input);
            }

            no::check_result(_owner._value->DefineOwnProperty(_owner._context,
                                                              _name,
                                                              value,
                                                              static_cast<v8::PropertyAttribute>(attribute)));
            return *this;
        }

        v8::Local<v8::Value> get() const {
            return _owner._get(_name);
        }

        operator v8::Local<v8::Value>() const {
            return get();
        }

        v8::Value* operator->() const {
            return get().operator->();
        }

        // c++ only allows explicit template specialization
        // in namespace (not class/struct), but it allows
        // partial template specialization here,
        // so add this `bool = true` part to comfort the standard.

        template <class T, bool = true>
        struct as_type { using type = v8::Local<T>; };

        template <bool D>
        struct as_type<no::object, D> { using type = no::object; };

        template <bool D>
        struct as_type<int32_t, D> { using type = int32_t; };

        template <class T>
        using as_type_v = typename as_type<T, true>::type;

        template <class T>
        as_type_v<T> as() {
            return as_factory<T, true>::as(get());
        }

      private:
        friend class object;

        explicit reference(const object& owner, v8::Local<v8::Name> name)
            : _owner(owner)
            , _name(name) {}

        const object&       _owner;
        v8::Local<v8::Name> _name;

        // c++ doesn't allow partial specialization of functions,
        // but it can easily be workarounded by the delegate-to-class trick

        template <class T, bool = true>
        struct as_factory {
            static as_type_v<T> as(v8::Local<v8::Value> value) {
                return value.As<T>();
            }
        };

        template <bool D>
        struct as_factory<no::object, D> {
            static no::object as(v8::Local<v8::Value> value) {
                return no::object(value.As<v8::Object>());
            }
        };
    };

    explicit object(v8::Isolate* isolate)
        : _isolate(isolate)
        , _context(isolate->GetCurrentContext())
        , _value(v8::Object::New(isolate)) {}

    explicit object(v8::Local<v8::Object> value)
        : _isolate(value->GetIsolate())
        , _context(value->CreationContext())
        , _value(value) {}

    template <size_t N>
    reference operator[](const char (&name)[N]) {
        return reference(*this, no::name(_isolate, name));
    }

    reference operator[](const std::string& name) {
        return reference(*this, no::name(_isolate, name));
    }

    reference operator[](v8::Local<v8::Name> name) {
        return reference(*this, name);
    }

    template <size_t N>
    v8::Local<v8::Value> operator[](const char (&name)[N]) const {
        return _get(no::name(_isolate, name));
    }

    v8::Local<v8::Value> operator[](const std::string& name) const {
        return _get(no::name(_isolate, name));
    }

    v8::Local<v8::Value> operator[](v8::Local<v8::Name> name) const {
        return _get(name);
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

    v8::Local<v8::Value> _get(v8::Local<v8::Name> name) const {
        return no::check_result(_value->Get(_context, name));
    }
};
} // namespace no
