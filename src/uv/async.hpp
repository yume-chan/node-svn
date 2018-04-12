#pragma once

#include <type_traits>

#include <uv/error.hpp>
#include <uv/future.hpp>

namespace uv {
template <class T>
struct async {
  public:
    explicit async(T&& callback)
        : async(std::forward<T>(callback), nullptr) {}

    explicit async(T&& callback, uv_loop_t* loop)
        : _handle(new async_handle(std::forward<T>(callback), loop)) {}

    template <class... Arg>
    decltype(auto) operator()(Arg&&... arg) {
        using R = std::invoke_result_t<T, Arg...>;

        running_scope scope(_handle);

        auto tuple  = std::forward_as_tuple(arg...);
        auto data   = async_data<R, Arg...>(_handle, std::move(tuple));
        auto future = data.promise.get_future();

        _handle->handle->data     = &data;
        _handle->handle->async_cb = &async::invoke_async<R, Arg...>;
        check_result(uv_async_send(_handle->handle));

        if constexpr (uv::is_future_v<R>) {
            return future.get().get();
        } else {
            return future.get();
        }
    }

  private:
    struct async_handle {
      public:
        async_handle(T&& callback, uv_loop_t* loop)
            : callback(std::forward<T>(callback))
            , handle(new uv_async_t)
            , running(false) {

            if (loop == nullptr) {
                loop = uv_default_loop();
            }

            try {
                check_result(uv_async_init(loop, handle, nullptr));
                uv_unref(reinterpret_cast<uv_handle_t*>(handle));
            } catch (...) {
                delete handle;
                throw;
            }
        }

        ~async_handle() {
            // FIXME: if there is a javascript loop keeps creating `async` handles,
            // libuv won't have any chance to call `delete_async_handle`
            // so this portion of memeory will temporarily leak.
            uv_close(reinterpret_cast<uv_handle_t*>(handle), delete_async_handle);
        }

        std::decay_t<T> callback;
        uv_async_t*     handle;

        std::atomic_bool running;

      private:
        static void delete_async_handle(uv_handle_t* handle) {
            delete reinterpret_cast<uv_async_t*>(handle);
        }
    };

    using shared_handle = std::shared_ptr<async_handle>;

    struct running_scope {
      public:
        running_scope(shared_handle handle)
            : _handle(handle) {
            if (_handle->running.exchange(true)) {
                throw std::runtime_error("");
            }
        }

        ~running_scope() {
            _handle->running = false;
        }

      private:
        shared_handle _handle;
    };

    template <class R, class... Arg>
    struct async_data {
        async_data(shared_handle        owner,
                   std::tuple<Arg...>&& arg)
            : owner(owner)
            , arg(std::move(arg))
            , promise() {}

        shared_handle      owner;
        std::tuple<Arg...> arg;
        std::promise<R>    promise;
    };

    shared_handle _handle;

    template <class R, class... Arg>
    static void invoke_async(uv_async_t* handle) {
        auto data = static_cast<async_data<R, Arg...>*>(handle->data);

        try {
            if constexpr (std::is_void_v<R>) {
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
};

template <class T>
decltype(auto) make_async(T&& callback) {
    return async<T>(std::forward<T>(callback));
}
} // namespace uv
