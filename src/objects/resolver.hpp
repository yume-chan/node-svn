#include <memory>
#include <type_traits>

#include <node/v8.hpp>

namespace no {
class resolver : public std::enable_shared_from_this<resolver> {
  public:
    static std::shared_ptr<resolver> create(v8::Isolate*            isolate,
                                            v8::Local<v8::Context>& context) {
        return std::shared_ptr<resolver>(new resolver(isolate, context));
    }

    void resolve() {
        resolve(v8::Undefined(_isolate));
    }

    template <class T>
    void resolve(T input) {
        set_result<T, true>(input);
    }

    template <class T>
    void reject(T input) {
        set_result<T, false>(input);
    }

    v8::Local<v8::Promise::Resolver> value() const {
        return _value.Get(_isolate);
    }

    operator v8::Local<v8::Promise::Resolver>() const {
        return value();
    }

    operator v8::Local<v8::Value>() const {
        return value();
    }

  private:
    explicit resolver(v8::Isolate*            isolate,
                      v8::Local<v8::Context>& context)
        : _isolate(isolate)
        , _context(isolate, context)
        , _value(isolate, no::check_result(v8::Promise::Resolver::New(context))) {
    }

    template <class T, bool Success>
    void set_result(T input) {
        v8::HandleScope scope(_isolate);

        v8::Local<v8::Value> value;

        if constexpr (std::is_convertible_v<T, v8::Local<v8::Value>>) {
            value = static_cast<v8::Local<v8::Value>>(input);
        } else {
            value = no::data(_isolate, input);
        }

        if constexpr (Success) {
            no::check_result(_value.Get(_isolate)->Resolve(_context.Get(_isolate), value));
        } else {
            no::check_result(_value.Get(_isolate)->Reject(_context.Get(_isolate), value));
        }

        _isolate->RunMicrotasks();
    }

    v8::Isolate*            _isolate;
    v8::Global<v8::Context> _context;

    v8::Global<v8::Promise::Resolver> _value;
};
} // namespace no
