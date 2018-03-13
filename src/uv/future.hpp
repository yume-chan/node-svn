#include <future>

namespace uv {
template <class T>
class future {
  public:
    using value_type = T;

    future(std::future<T>& future)
        : value(std::move(future)) {}

    T get() {
        return value.get();
    }

  private:
    std::future<T> value;
};

template <class T>
class future<T&> {
  public:
    using value_type = T&;

    future(std::future<T&>& future)
        : value(std::move(future)) {}

    T& get() {
        return value.get();
    }

  private:
    std::future<T&> value;
};

template <>
class future<void> {
  public:
    using value_type = void;

    future()         = default;
    future(future&&) = default;

    future& operator=(future&&) = default;

    future(std::future<void>&& value)
        : value(std::move(value)) {}

    void get() {
        value.get();
    }

  private:
    std::future<void> value;
};

template <class T>
struct is_future : std::false_type {};

template <class T>
struct is_future<future<T>> : std::true_type {};

template <class T>
inline constexpr bool is_future_v = is_future<T>::value;
} // namespace uv
