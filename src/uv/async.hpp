#pragma once

#include <type_traits>

#include <uv/error.hpp>
#include <uv/future.hpp>

#include <iostream>

namespace uv {
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

template <class F>
struct async {
  public:
    explicit async(F&& callback)
        : async(std::forward<F>(callback), uv_default_loop()) {}

    explicit async(F&& callback, uv_loop_t* loop)
        : _running(false)
        , _callback(std::forward<F>(callback)) {
        auto raw = new uv_async_t();

        if (!loop) {
            loop = uv_default_loop();
        }

        try {
            check_result(uv_async_init(loop, raw, nullptr));

            // this handle won't keep event loop running
            uv_unref(reinterpret_cast<uv_handle_t*>(raw));

            _handle = async_handle(raw, close_async_handle);
        } catch (...) {
            delete raw;
            throw;
        }
    }

    template <class... Arg>
    decltype(auto) operator()(Arg&&... arg) {
        using Result = std::invoke_result_t<F, Arg...>;

        if (_running) {
            throw std::runtime_error("");
        }

        run_scope scope(this);

        auto tuple  = std::forward_as_tuple(arg...);
        auto data   = async_data<F, Result, Arg...>(this, std::move(tuple));
        auto future = data.promise.get_future();

        _handle->data     = &data;
        _handle->async_cb = &async::invoke_async<Result, Arg...>;
        check_result(uv_async_send(_handle.get()));

        if constexpr (uv::is_future_v<Result>) {
            return future.get().get();
        } else {
            return future.get();
        }
    }

  private:
    using async_handle = std::shared_ptr<uv_async_t>;

    static void close_async_handle(uv_async_t* handle) {
        // FIXME: if there is a javascript loop keeps creating `async` handles,
        // libuv won't have any chance to call `delete_async_handle`
        // so this portion of memeory will temporarily leak.
        uv_close(reinterpret_cast<uv_handle_t*>(handle), delete_async_handle);
    }

    static void delete_async_handle(uv_handle_t* handle) {
        delete reinterpret_cast<uv_async_t*>(handle);
    }

    template <class Result, class... Arg>
    static void invoke_async(uv_async_t* handle) {
        auto data = static_cast<async_data<F, Result, Arg...>*>(handle->data);

        try {
            if constexpr (std::is_void_v<Result>) {
                std::apply(data->owner->_callback, std::move(data->arg));
                data->promise.set_value();
            } else {
                auto result = std::apply(data->owner->_callback, std::move(data->arg));
                data->promise.set_value(std::move(result));
            }
        } catch (...) {
            data->promise.set_exception(std::current_exception());
        }
    }

    bool _running;

    struct run_scope {
        run_scope(async<F>* owner)
            : _owner(owner) {
            _owner->_running = true;
        }

        ~run_scope() {
            _owner->_running = false;
        }

      private:
        async<F>* _owner;
    };

    // use `std::decay_t` to ensure copy/move,
    // instead of referencing the `callback`.
    std::decay_t<F> _callback;

    // use a `std::shared_ptr<uv_async_t>` to
    // enable copy of `async` struct.
    async_handle _handle;
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
