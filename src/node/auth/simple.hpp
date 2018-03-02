#pragma once

#include <cpp/client.hpp>

#include <node/v8.hpp>

namespace node {
using shared_callback = std::shared_ptr<v8::Persistent<v8::Function>>;

struct simple_auth_provider
    : public std::enable_shared_from_this<simple_auth_provider> {
  public:
    simple_auth_provider(v8::Isolate* isolate, bool async);
    ~simple_auth_provider();

    void add(const v8::Local<v8::Function>& function);
    void remove(const v8::Local<v8::Function>& function);

    std::optional<svn::simple_auth> operator()(const std::string&                      realm,
                                               const std::optional<const std::string>& username,
                                               bool                                    may_save);

    operator svn::simple_auth_provider();

  private:
    std::optional<svn::simple_auth> _invoke_sync(const std::string&                      realm,
                                                 const std::optional<const std::string>& username,
                                                 bool                                    may_save);

    v8::Isolate*            _isolate;
    bool                    _async;
    v8::Persistent<v8::Set> _functions;
};
} // namespace node
