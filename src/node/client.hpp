#pragma once

#include <v8.h>

namespace svn {
class client;
}

namespace node {
class __declspec(dllexport) client {
  public:
    explicit client();
    client(const client&) = delete;
    client(client&&)      = delete;

    ~client();

    void add_to_changelist(v8::Local<v8::String>& path,
                           v8::Local<v8::String>& changelist,
                           v8::Local<v8::Number>& depth       = v8::Local<v8::Number>(),
                           v8::Local<v8::Array>&  changelists = v8::Local<v8::Array>());
    void add_to_changelist(v8::Local<v8::Array>&  paths,
                           v8::Local<v8::String>& changelist,
                           v8::Local<v8::Number>& depth       = v8::Local<v8::Number>(),
                           v8::Local<v8::Array>&  changelists = v8::Local<v8::Array>());

  private:
    svn::client* _client;
};
} // namespace node
