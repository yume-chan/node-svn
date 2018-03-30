#pragma once

#include <future>
#include <type_traits>

#include <uv/error.hpp>

namespace uv {
template <class Work, class AfterWork, class Result>
class work {
  public:
    work(Work work, AfterWork after_work)
        : _work(std::move(work))
        , _after_work(std::move(after_work)) {
        _handle.data = this;
        check_result(uv_queue_work(uv_default_loop(), &_handle, invoke_work, invoke_after_work));
    }

  private:
    static void invoke_work(uv_work_t* handle) {
        auto _this = static_cast<work*>(handle->data);
        try {
            if constexpr (std::is_void_v<Result>) {
                _this->_work();
                _this->_promise.set_value();
            } else {
                auto result = _this->_work();
                _this->_promise.set_value(result);
            }
        } catch (...) {
            _this->_promise.set_exception(std::current_exception());
        }
    }

    static void invoke_after_work(uv_work_t* handle, int status) {
        auto _this  = static_cast<work*>(handle->data);
        auto future = _this->_promise.get_future();
        _this->_after_work(std::move(future));

        delete _this;
    }

    ~work() {}

    uv_work_t _handle;

    const Work      _work;
    const AfterWork _after_work;

    std::promise<Result> _promise;
};

template <class Work, class AfterWork>
static void queue_work(Work work, AfterWork after_work) {
    new uv::work<Work, AfterWork, decltype(work())>(std::move(work), std::move(after_work));
}
} // namespace uv
