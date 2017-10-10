#include "client.hpp"

#include <cpp\client.hpp>

static std::string convert_string(v8::Local<v8::String>& value) {
    v8::String::Utf8Value utf8(value);
    return std::move(std::string(*utf8, utf8.length()));
}

static std::vector<std::string> convert_array(v8::Local<v8::Array>& value) {
    auto result = std::vector<std::string>(value->Length());

    return result;
}

namespace node {
client::client()
    : _client() {}

void client::add_to_changelist(v8::Local<v8::String>& path,
                               v8::Local<v8::String>& changelist,
                               v8::Local<v8::Number>& depth,
                               v8::Local<v8::Array>&  changelists) {
    auto raw_path        = convert_string(path);
    auto raw_changelist  = convert_string(changelist);
    auto raw_depth       = static_cast<svn_depth_t>(depth->Int32Value());
    auto raw_changelists = convert_array(changelists);

    _client->add_to_changelist(raw_path,
                               raw_changelist,
                               raw_depth,
                               raw_changelists);
}

void client::add_to_changelist(v8::Local<v8::Array>&  paths,
                               v8::Local<v8::String>& changelist,
                               v8::Local<v8::Number>& depth,
                               v8::Local<v8::Array>&  changelists) {
    auto raw_paths       = convert_array(paths);
    auto raw_changelist  = convert_string(changelist);
    auto raw_depth       = static_cast<svn_depth_t>(depth->Int32Value());
    auto raw_changelists = convert_array(changelists);

    _client->add_to_changelist(raw_paths,
                               raw_changelist,
                               raw_depth,
                               raw_changelists);
}

} // namespace node
