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
template <class F>
struct async;

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
    uv_async_init(loop, raw, nullptr);

    uv_unref(reinterpret_cast<uv_handle_t*>(raw));

    return async_handle(raw, close_async_handle);
}

template <class F, class Result, class... Args>
struct async_data {
    async_data(async<F>* owner, std::tuple<Args...>&& args)
        : owner(owner)
        , args(std::move(args)) {}

    async<F>*            owner;
    std::promise<Result> promise;
    std::tuple<Args...>  args;
};

template <class F>
struct async {
  public:
    async(F&& callback)
        : callback(std::forward<F>(callback))
        , handle(make_async_handle()) {}

    template <class... Args>
    decltype(auto) operator()(Args&&... args) {
        using Result = std::invoke_result_t<F, Args...>;

        auto old = handle->data;
        if (old) {
            delete old;
        }

        auto tuple  = std::make_tuple<Args...>(std::forward<Args>(args)...);
        auto data   = new async_data<F, Result, Args...>(this, std::move(tuple));
        auto future = data->promise.get_future();

        handle->data     = data;
        handle->async_cb = &async::invoke_async<Result, Args...>;
        uv_async_send(handle.get());

        if constexpr (std::is_void_v<Result>) {
            future.get();
        } else {
            return future.get();
        }
    }

  private:
    template <class Result, class... Args>
    static void invoke_async(uv_async_t* handle) {
        auto data = static_cast<async_data<F, Result, Args...>*>(handle->data);
        if constexpr (std::is_void_v<Result>) {
            std::apply(data->owner->callback, data->args);
            data->promise.set_value();
        } else {
            auto result = std::apply(data->owner->callback, std::move(data->args));
            data->promise.set_value(std::move(result));
        }
    }

    F            callback;
    async_handle handle;
};

template <class F>
decltype(auto) make_async(F&& callback) {
    return async<F>(std::forward<F>(callback));
}
} // namespace uv
