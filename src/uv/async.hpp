#pragma once

#include <future>
#include <type_traits>

#include <uv/error.hpp>

namespace uv {
using async_handle = std::shared_ptr<uv_async_t>;

static void delete_async_handle(uv_handle_t* handle) {
    delete reinterpret_cast<uv_async_t*>(handle);
}

static void close_async_handle(uv_async_t* handle) {
    uv_close(reinterpret_cast<uv_handle_t*>(handle), delete_async_handle);
}

static async_handle make_async_handle(uv_loop_t* loop = nullptr) {
    auto raw = new uv_async_t();

    if (!loop) {
        loop = uv_default_loop();
    }

    check_uv_error(uv_async_init(loop, raw, nullptr));

    // this handle won't keep event loop running
    uv_unref(reinterpret_cast<uv_handle_t*>(raw));

    return async_handle(raw, close_async_handle);
}

template <class F>
struct async;

template <class F, class Result, class... Arg>
struct async_data {
    async_data(const async<F>*      owner,
               std::tuple<Arg...>&& arg)
        : owner(owner)
        , promise()
        , arg(std::move(arg)) {}

    const async<F>*      owner;
    std::promise<Result> promise;
    std::tuple<Arg...>   arg;
};

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

template <class F>
struct async {
  public:
    async(F&& callback)
        : callback(std::forward<F>(callback))
        , handle(make_async_handle()) {}

    template <class... Arg>
    decltype(auto) operator()(Arg&&... arg) const {
        using Result = std::invoke_result_t<F, Arg...>;

        auto tuple  = std::forward_as_tuple(arg...);
        auto data   = async_data<F, Result, Arg...>(this, std::move(tuple));
        auto future = data.promise.get_future();

        handle->data     = &data;
        handle->async_cb = &async::invoke_async<Result, Arg...>;
        check_uv_error(uv_async_send(handle.get()));

        if constexpr (std::is_void_v<Result>) {
            future.get();
        } else if constexpr (is_future_v<Result>) {
            using T = typename Result::value_type;

            auto result = future.get();
            if constexpr (std::is_void_v<T>) {
                result.get();
            } else {
                return result.get();
            }
        } else {
            return future.get();
        }
    }

  private:
    template <class Result, class... Arg>
    static void invoke_async(uv_async_t* handle) {
        auto data = static_cast<async_data<F, Result, Arg...>*>(handle->data);

        try {
            if constexpr (std::is_void_v<Result>) {
                std::apply(data->owner->callback, std::move(data->arg));
                data->promise.set_value();
            } else {
                auto result = std::apply(data->owner->callback, std::move(data->arg));
                data->promise.set_value(std::move(result));
            }
        } catch (...) {
            data->promise.set_exception(std::current_exception());
        }
    }

    // use `std::decay_t` to ensure copy/move,
    // instead of referencing the `callback`.
    std::decay_t<F> callback;

    // use a `std::shared_ptr<uv_async_t>` to
    // enable copy of `async` struct.
    async_handle handle;
};

#if !defined(_MSC_VER)
template <class F>
async(F &&)->async<F>;
#endif

template <class F>
decltype(auto) make_async(F&& callback) {
    return async<F>(std::forward<F>(callback));
}
} // namespace uv
