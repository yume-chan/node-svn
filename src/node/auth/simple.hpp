#pragma once

#include <cpp/client.hpp>

#include <node/v8.hpp>

namespace node {
using shared_callback = std::shared_ptr<v8::Persistent<v8::Function>>;

struct simple_auth_provider {
  public:
    simple_auth_provider(v8::Isolate* isolate, bool async);

    void add(const v8::Local<v8::Function>& function);
    void remove(const v8::Local<v8::Function>& function);

    std::optional<svn::simple_auth> operator()(const std::string&                      realm,
                                               const std::optional<const std::string>& username,
                                               bool                                    may_save);

  private:
    static std::optional<svn::simple_auth> _invoke_sync(simple_auth_provider*                   _this,
                                                        const std::string&                      realm,
                                                        const std::optional<const std::string>& username,
                                                        bool                                    may_save);

    v8::Isolate*                                                   _isolate;
    bool                                                           _async;
    v8::Persistent<v8::Set, v8::CopyablePersistentTraits<v8::Set>> _functions;

    using invoke_function = std::function<std::optional<svn::simple_auth>(simple_auth_provider*,
                                                                          const std::string&,
                                                                          const std::optional<const std::string>&,
                                                                          bool)>;

    invoke_function _invoke;
};
} // namespace node
