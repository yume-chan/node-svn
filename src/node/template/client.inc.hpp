#include <node_object_wrap.h>

#include <node/auth/simple.hpp>

namespace svn {
class client;
}

namespace no {
class CLASS_NAME : public node::ObjectWrap {
  public:
    static void init(v8::Local<v8::Object>&  exports,
                     v8::Isolate*            isolate,
                     v8::Local<v8::Context>& context);

    static void create_instance(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void add_simple_auth_provider(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void remove_simple_auth_provider(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void add_to_changelist(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void get_changelists(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void remove_from_changelists(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void blame(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void cat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void checkout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void cleanup(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void commit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void info(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void revert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void status(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void update(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void get_working_copy_root(const v8::FunctionCallbackInfo<v8::Value>& args);

    explicit CLASS_NAME(v8::Isolate* isolate, const std::optional<const std::string>& config_path);
    CLASS_NAME(const CLASS_NAME&) = delete;
    CLASS_NAME(CLASS_NAME&&)      = delete;

    ~CLASS_NAME();

  private:
    std::unique_ptr<svn::client> _client;
    no::simple_auth_provider   _simple_auth_provider;
};
} // namespace no
