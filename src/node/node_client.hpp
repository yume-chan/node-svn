#pragma once

#include <node/auth/simple.hpp>

namespace svn {
class client;
}

namespace no {
class client : public std::enable_shared_from_this<client> {
  public:
    static void init(v8::Local<v8::Object>&  exports,
                     v8::Isolate*            isolate,
                     v8::Local<v8::Context>& context);

    client(const client&) = delete;
    client(client&&)      = delete;

    static int64_t size() {
        return 16 * 1024;
    }

  private:
    client(v8::Isolate* isolate, const std::optional<const std::string>& config_path);

    static std::shared_ptr<client> constructor(const v8::FunctionCallbackInfo<v8::Value>& args);

    v8::Local<v8::Value> add_simple_auth_provider(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> remove_simple_auth_provider(const v8::FunctionCallbackInfo<v8::Value>& args);

    v8::Local<v8::Value> add_to_changelist(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> get_changelists(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> remove_from_changelists(const v8::FunctionCallbackInfo<v8::Value>& args);

    v8::Local<v8::Value> add(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> blame(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> cat(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> checkout(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> cleanup(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> commit(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> info(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> log(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> revert(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> status(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Value> update(const v8::FunctionCallbackInfo<v8::Value>& args);

    v8::Local<v8::Value> get_working_copy_root(const v8::FunctionCallbackInfo<v8::Value>& args);

    std::unique_ptr<svn::client> _client;
    no::simple_auth_provider     _simple_auth_provider;
};
} // namespace no
