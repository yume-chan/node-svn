#pragma once

#include <cpp/client.hpp>

#include <node/v8.hpp>

namespace no {
using shared_callback = std::shared_ptr<v8::Persistent<v8::Function>>;

struct simple_auth_provider {
  public:
    simple_auth_provider(v8::Isolate* isolate);

    void add(const v8::Local<v8::Function>& function);
    void remove(const v8::Local<v8::Function>& function);

    std::optional<svn::simple_auth> operator()(const std::string&                      realm,
                                               const std::optional<const std::string>& username,
                                               bool                                    may_save) const;

  private:
    static std::optional<svn::simple_auth> _invoke_implement(const simple_auth_provider*             _this,
                                                             const std::string&                      realm,
                                                             const std::optional<const std::string>& username,
                                                             bool                                    may_save);

    v8::Isolate*        _isolate;
    v8::Global<v8::Set> _functions;

    using invoke_type = std::function<std::optional<svn::simple_auth>(const simple_auth_provider*,
                                                                      const std::string&,
                                                                      const std::optional<const std::string>&,
                                                                      bool)>;

    const invoke_type _invoke;
};
} // namespace no
