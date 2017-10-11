#include "client.hpp"

#include <cpp\client.hpp>
#include <node\v8.hpp>

static std::string convert_string(const v8::Local<v8::String>& value) {
    if (value.IsEmpty()) {
        auto isolate = v8::Isolate::GetCurrent();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
    }

    v8::String::Utf8Value utf8(value);
    auto                  length = utf8.length();

    if (strlen(*utf8) != length) {
        auto isolate = v8::Isolate::GetCurrent();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
    }

    return std::string(*utf8, utf8.length());
}

static std::vector<std::string> convert_array(const v8::Local<v8::Array>& value, bool canBeNull) {
    if (value.IsEmpty()) {
        if (canBeNull)
            return std::vector<std::string>();

        auto isolate = v8::Isolate::GetCurrent();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
    }

    auto length = value->Length();
    auto result = std::vector<std::string>(length);
    for (uint32_t i = 0; i < length; i++) {
        auto item = value->Get(i);
        if (!item->IsString()) {
            auto isolate = v8::Isolate::GetCurrent();
            isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
        }

        result.push_back(std::move(convert_string(item.As<v8::String>())));
    }
    return result;
}

static svn_depth_t convert_depth(const v8::Local<v8::Number>& value, svn_depth_t defaultValue) {
    if (value.IsEmpty())
        return defaultValue;

    return static_cast<svn_depth_t>(value->Int32Value());
}

namespace node {
client::client()
    : _client() {}

client::~client() {}

void client::add_to_changelist(const v8::Local<v8::String>& path,
                               const v8::Local<v8::String>& changelist,
                               const v8::Local<v8::Number>& depth,
                               const v8::Local<v8::Array>&  changelists) const {
    auto raw_path        = convert_string(path);
    auto raw_changelist  = convert_string(changelist);
    auto raw_depth       = convert_depth(depth, svn_depth_infinity);
    auto raw_changelists = convert_array(changelists, true);

    _client->add_to_changelist(raw_path,
                               raw_changelist,
                               raw_depth,
                               raw_changelists);
}

void client::add_to_changelist(const v8::Local<v8::Array>&  paths,
                               const v8::Local<v8::String>& changelist,
                               const v8::Local<v8::Number>& depth,
                               const v8::Local<v8::Array>&  changelists) const {
    auto raw_paths       = convert_array(paths, false);
    auto raw_changelist  = convert_string(changelist);
    auto raw_depth       = convert_depth(depth, svn_depth_infinity);
    auto raw_changelists = convert_array(changelists, true);

    _client->add_to_changelist(raw_paths,
                               raw_changelist,
                               raw_depth,
                               raw_changelists);
}

void client::get_changelists(const v8::Local<v8::String>&   path,
                             const v8::Local<v8::Function>& callback,
                             const v8::Local<v8::Array>&    changelists,
                             const v8::Local<v8::Number>&   depth) const {
    auto raw_path     = convert_string(path);
    auto raw_callback = [callback](const char* path, const char* changelist) -> void {
        auto isolate = v8::Isolate::GetCurrent();

        const auto           argc       = 2;
        v8::Local<v8::Value> argv[argc] = {v8::New<v8::String>(isolate, path),
                                           v8::New<v8::String>(isolate, changelist)};

        callback->Call(v8::Undefined(isolate), argc, argv);
    };
    auto raw_changelists = convert_array(changelists, true);
    auto raw_depth       = convert_depth(depth, svn_depth_infinity);

    _client->get_changelists(raw_path,
                             raw_callback,
                             raw_changelists,
                             raw_depth);
}

void client::remove_from_changelists(const v8::Local<v8::String>& path,
                                     const v8::Local<v8::Number>& depth,
                                     const v8::Local<v8::Array>&  changelists) const {
    auto raw_path        = convert_string(path);
    auto raw_depth       = convert_depth(depth, svn_depth_infinity);
    auto raw_changelists = convert_array(changelists, true);

    _client->remove_from_changelists(raw_path,
                                     raw_depth,
                                     raw_changelists);
}

void client::remove_from_changelists(const v8::Local<v8::Array>&  paths,
                                     const v8::Local<v8::Number>& depth,
                                     const v8::Local<v8::Array>&  changelists) const {
    auto raw_paths       = convert_array(paths, false);
    auto raw_depth       = convert_depth(depth, svn_depth_infinity);
    auto raw_changelists = convert_array(changelists, true);

    _client->remove_from_changelists(raw_paths,
                                     raw_depth,
                                     raw_changelists);
}
} // namespace node
