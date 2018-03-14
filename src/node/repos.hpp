#pragma once

#include <svn_repos.h>

#include <node.h>

#include <node/type_conversion.hpp>
#include <node/v8.hpp>

namespace no {
namespace repos {
static void create_repos(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto path = convert_string(args[0]);

    apr_initialize();

    apr_pool_t* pool;
    apr_pool_create_ex(&pool, nullptr, nullptr, nullptr);

    apr_hash_t* fs_config = apr_hash_make(pool);

    svn_repos_t* repos;
    svn_repos_create(&repos, path.c_str(), nullptr, nullptr, nullptr, fs_config, pool);

    // apr_terminate();
}

void init(v8::Local<v8::Object>&  exports,
          v8::Isolate*            isolate,
          v8::Local<v8::Context>& context) {
    NODE_SET_METHOD(exports, "create_repos", create_repos);
}
} // namespace repos
} // namespace no
