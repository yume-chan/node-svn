#pragma once

#include <functional>
#include <future>
#include <type_traits>

#include <uv.h>

static void check_uv_error(int error) {
    if (error != 0) {
        throw std::runtime_error(uv_strerror(error));
    }
}

namespace uv {
template <class Callable>
struct async_callable;

template <class Callable, class Result, class... Args>
struct async_data {
    async_data(async_callable<Callable> async, Args&&... args)
        : async(async)
        , args(std::make_tuple<Args...>(std::forward<Args>(args)...)) {}

    async_callable<Callable>& async;
    std::promise<Result>      promise;
    std::tuple<Args...>       args;
};

template <class Callable>
struct async_callable {
  public:
    async_callable(Callable callback)
        : callback(callback)
        , handle(new uv_async_t) {
        uv_async_init(uv_default_loop(), handle, nullptr);
    }

    template <class... Args>
    decltype(auto) operator()(Args&&... args) {
        using Result = std::invoke_result_t<Callable, Args...>;

        auto data   = async_data<async_callable<Callable>, Result, Args...>{this, std::forward<Args>(args)...};
        auto future = data.promise.get_future();

        handle->data     = data;
        handle->async_cb = &async_callable::invoke_async<Result, Args...>;
        uv_async_send(handle);

        if constexpr (std::is_void_v<Result>) {
            future.get();
        } else {
            return future.get();
        }
    }

  private:
    template <class Result, class... Args>
    static void invoke_async(uv_async_t* handle) {
        auto data = static_cast<async_data<Callable, Result, Args...>>(handle->data);
        if constexpr (std::is_void_v<Result>) {
            std::apply(data->async->callback, data->args);
            _this->promise.set_value();
        } else {
            auto result = std::apply(data->async->callback, data->args);
            _this->promise.set_value(std::move(result));
        }
    }

    Callable    callback;
    uv_async_t* handle;
};

template <class Callable>
decltype(auto) make_async(Callable callback) {
    return async_callable<Callable>(callback);
}
} // namespace uv

// template <int>
// struct placeholder_template {};

// namespace std {
// template <int N>
// struct is_placeholder<placeholder_template<N>>
//     : integral_constant<int, N + 1> {};
// } // namespace std

// template <class Callback, class Result, class... Args>
// class async {
//   public:
//     async(Callback callback)
//         : callback(callback)
//         , handle(new uv_async_t) {
//         static_assert(std::is_invocable_v<Callback, Args...>, "callback must be invocable");

//         check_uv_error(uv_async_init(uv_default_loop(), handle, invoke_async));
//         uv_unref(reinterpret_cast<uv_handle_t*>(handle));
//         handle->data = this;
//     }

//     async(const async&) = delete;

//     ~async() {
//         uv_close(reinterpret_cast<uv_handle_t*>(handle), delete_async);
//     }

//     Result operator()(Args... args) {
//         this->args = std::make_unique<std::tuple<Args...>>(std::forward<Args>(args)...);

//         promise = std::promise<Result>();

//         check_uv_error(uv_async_send(handle));

//         auto future = promise.get_future();
//         if constexpr (std::is_void_v<Result>) {
//             future.get();
//         } else {
//             return future.get();
//         }
//     }

//     std::function<Result(Args...)> bind() {
//         return bind_impl(std::make_integer_sequence<int, sizeof...(Args)>{});
//     }

//   private:
//     static void invoke_async(uv_async_t* handle) {
//         auto _this = static_cast<async*>(handle->data);
//         if constexpr (std::is_void_v<Result>) {
//             std::apply(_this->callback, *(_this->args));
//             _this->promise.set_value();
//         } else {
//             auto result = std::apply(_this->callback, *(_this->args));
//             _this->promise.set_value(std::move(result));
//         }
//     }

//     static void delete_async(uv_handle_t* handle) {
//         delete reinterpret_cast<uv_async_t*>(handle);
//     }

//     template <int... Is>
//     decltype(auto) bind_impl(std::integer_sequence<int, Is...>) {
//         auto shared = std::shared_ptr<async>(this);
//         return std::bind(&async::operator(), shared, placeholder_template<Is>{}...);
//     }

//     uv_async_t* handle;

//     Callback                             callback;
//     std::unique_ptr<std::tuple<Args...>> args;

//     std::promise<Result> promise;
// };

// template <class Callback, class... Args>
// static decltype(auto) make_async(Callback callback) {
//     using Result = std::invoke_result_t<Callback, Args...>;
//     return (new async<Callback, Result, Args...>(callback))->bind();
// }
// } // namespace uv
