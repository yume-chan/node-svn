#pragma once

#include <node_object_wrap.h>
#include <v8.h>

namespace svn {
class client;
}

namespace node {
class client : public node::ObjectWrap {
  public:
    static void init(v8::Local<v8::Object> exports, v8::Isolate* isolate, v8::Local<v8::Context>& context);

    static void create_instance(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void add_to_changelist(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void get_changelists(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void remove_from_changelists(const v8::FunctionCallbackInfo<v8::Value>& args);

    explicit client();
    client(const client&) = delete;
    client(client&&)      = delete;

    ~client();

  private:
    std::unique_ptr<svn::client> _client;
};
} // namespace node
