#pragma once

#include <functional>
#include <future>
#include <type_traits>

#include <uv.h>

template <int>
struct placeholder_template {};

namespace std {
template <int N>
struct is_placeholder<placeholder_template<N>>
    : integral_constant<int, N + 1> {};
} // namespace std

namespace uv {
template <class Callback, class Result, class... Args>
class async {
  public:
    async(Callback callback)
        : callback(callback)
        , uv_async(new uv_async_t) {
        static_assert(std::is_invocable_v<Callback, Args...>, "callback must be invocable");

        uv_async->data = this;
        uv_async_init(uv_default_loop(), uv_async, invoke_async);
    }

    async(const async&) = delete;

    ~async() {
        uv_close(reinterpret_cast<uv_handle_t*>(uv_async), delete_async);
    }

    Result operator()(Args... args) {
        this->args = std::make_unique<std::tuple<Args...>>(std::forward<Args>(args)...);

        promise = std::promise<Result>();

        uv_async_send(uv_async);

        auto future = promise.get_future();
        if constexpr (std::is_void_v<Result>) {
            future.get();
        } else {
            return future.get();
        }
    }

    std::function<Result(Args...)> bind() {
        return bind_impl(std::make_integer_sequence<int, sizeof...(Args)>{});
    }

  private:
    static void invoke_async(uv_async_t* handle) {
        auto _this = static_cast<async*>(handle->data);
        if constexpr (std::is_void_v<Result>) {
            std::apply(_this->callback, *(_this->args));
            _this->promise.set_value();
        } else {
            auto result = std::apply(_this->callback, *(_this->args));
            _this->promise.set_value(std::move(result));
        }
    }

    static void delete_async(uv_handle_t* handle) {
        delete reinterpret_cast<uv_async_t*>(handle);
    }

    template <int... Is>
    decltype(auto) bind_impl(std::integer_sequence<int, Is...>) {
        auto shared = std::shared_ptr<async>(this);
        return std::bind(&async::operator(), shared, placeholder_template<Is>{}...);
    }

    uv_async_t* uv_async;

    Callback                             callback;
    std::unique_ptr<std::tuple<Args...>> args;

    std::promise<Result> promise;
};

template <class Callback, class... Args>
static decltype(auto) make_async(Callback callback) {
    using Result = std::invoke_result_t<Callback, Args...>;
    return (new async<Callback, Result, Args...>(callback))->bind();
}
} // namespace uv
