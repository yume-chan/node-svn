#include <functional>
#include <future>

#include <cpp/client.hpp>

#include <node/v8.hpp>
#include <uv/async.hpp>

namespace node {
struct simple_auth_provider {
  public:
    simple_auth_provider(v8::Isolate* isolate, v8::Local<v8::Function>& callback, bool is_async);
    ~simple_auth_provider();

    std::unique_ptr<svn::simple_auth> operator()(const std::string& realm,
                                                 const std::string& username,
                                                 bool               may_save);

  private:
    static void _invoke_sync(simple_auth_provider* _this,
                             const std::string&    realm,
                             const std::string&    username,
                             bool                  may_save);

    using invoke_function       = std::function<void(simple_auth_provider*, const std::string&, const std::string&, bool)>;
    using simple_auth_promise   = std::promise<std::unique_ptr<svn::simple_auth>>;
    using invoke_function_async = uv::async<invoke_function, void, simple_auth_provider*, const std::string&, const std::string&, bool>;

    v8::Isolate*             _isolate;
    v8::Global<v8::Function> _callback;
    bool                     _is_async;
    invoke_function          _invoke;
    simple_auth_promise      _promise;
};
} // namespace node
