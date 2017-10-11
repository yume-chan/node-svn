#pragma once

#include <v8.h>

namespace svn {
class client;
}

namespace node {
class client {
  public:
    explicit client();
    client(const client&) = delete;
    client(client&&)      = delete;

    ~client();

    void add_to_changelist(const v8::Local<v8::String>& path,
                           const v8::Local<v8::String>& changelist,
                           const v8::Local<v8::Number>& depth       = v8::Local<v8::Number>(),
                           const v8::Local<v8::Array>&  changelists = v8::Local<v8::Array>()) const;
    void add_to_changelist(const v8::Local<v8::Array>&  paths,
                           const v8::Local<v8::String>& changelist,
                           const v8::Local<v8::Number>& depth       = v8::Local<v8::Number>(),
                           const v8::Local<v8::Array>&  changelists = v8::Local<v8::Array>()) const;

    void get_changelists(const v8::Local<v8::String>&   path,
                         const v8::Local<v8::Function>& callback,
                         const v8::Local<v8::Array>&    changelists = v8::Local<v8::Array>(),
                         const v8::Local<v8::Number>&   depth       = v8::Local<v8::Number>()) const;

    void remove_from_changelists(const v8::Local<v8::String>& path,
                                 const v8::Local<v8::Number>& depth       = v8::Local<v8::Number>(),
                                 const v8::Local<v8::Array>&  changelists = v8::Local<v8::Array>()) const;
    void remove_from_changelists(const v8::Local<v8::Array>&  paths,
                                 const v8::Local<v8::Number>& depth       = v8::Local<v8::Number>(),
                                 const v8::Local<v8::Array>&  changelists = v8::Local<v8::Array>()) const;

  private:
    std::unique_ptr<svn::client> _client;
};
} // namespace node
