#pragma once

#include <svn_repos.h>
#include <svn_hash.h>

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
    svn_hash_sets(fs_config, SVN_FS_CONFIG_COMPATIBLE_VERSION, "1.9");

    svn_repos_t* repos;
    svn_repos_create(&repos, path.c_str(), nullptr, nullptr, nullptr, fs_config, pool);

    // apr_terminate();
}

void initialize(no::object& exports) {
    exports["create_repos"].set(no::data<v8::Function>(exports.context(), create_repos), no::property_attribute::read_only);
}
} // namespace repos
} // namespace no
