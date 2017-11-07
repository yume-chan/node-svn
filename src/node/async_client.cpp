#include "async_client.hpp"

#include <cpp/client.hpp>

#include <node/v8.hpp>

#define ReadOnlyDontDelete static_cast<PropertyAttribute>(PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete)

#define InternalizedString(value) v8::New<String>(isolate, value, NewStringType::kInternalized, sizeof(value) - 1)

#define SetReadOnly(object, name, value)                  \
    (object)->DefineOwnProperty(context,                  \
                                InternalizedString(name), \
                                value,                    \
                                ReadOnlyDontDelete)

#define SetPrototypeMethod(signature, prototype, name, callback, length)                  \
    /* Add a scope to hide extra variables */                                             \
    {                                                                                     \
        auto function = v8::FunctionTemplate::New(isolate,                /* isolate */   \
                                                  callback,               /* callback */  \
                                                  v8::Local<v8::Value>(), /* data */      \
                                                  signature,              /* signature */ \
                                                  length);                /* length */    \
        function->RemovePrototype();                                                      \
        prototype->Set(InternalizedString(name), function, PropertyAttribute::DontEnum);  \
    }

namespace node {
void async_client::init(v8::Local<v8::Object>   exports,
                        v8::Isolate*            isolate,
                        v8::Local<v8::Context>& context) {
    auto client    = v8::New<v8::FunctionTemplate>(isolate, create_instance);
    auto signature = v8::Signature::New(isolate, client);

    client->SetClassName(InternalizedString("AsyncClient"));
    client->ReadOnlyPrototype();

    client->InstanceTemplate()->SetInternalFieldCount(1);

    auto prototype = client->PrototypeTemplate();
    SetPrototypeMethod(signature, prototype, "add_to_changelist", add_to_changelist, 2);
    SetPrototypeMethod(signature, prototype, "get_changelists", get_changelists, 2);
    SetPrototypeMethod(signature, prototype, "remove_from_changelists", remove_from_changelists, 2);

    SetPrototypeMethod(signature, prototype, "add", add, 1);
    SetPrototypeMethod(signature, prototype, "cat", cat, 1);
    SetPrototypeMethod(signature, prototype, "checkout", checkout, 2);
    SetPrototypeMethod(signature, prototype, "commit", commit, 3);
    SetPrototypeMethod(signature, prototype, "info", info, 2);
    SetPrototypeMethod(signature, prototype, "remove", remove, 2);
    SetPrototypeMethod(signature, prototype, "revert", revert, 1);
    SetPrototypeMethod(signature, prototype, "status", status, 2);
    SetPrototypeMethod(signature, prototype, "update", update, 1);

    SetPrototypeMethod(signature, prototype, "get_working_copy_root", get_working_copy_root, 1);

    SetReadOnly(exports, "AsyncClient", client->GetFunction());
}

void async_client::create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!args.IsConstructCall()) {
        auto isolate = args.GetIsolate();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "Class constructor AsyncClient cannot be invoked without 'new'")));
        return;
    }

    auto result = new async_client();
    result->Wrap(args.This());
}

void async_client::create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::add_to_changelist(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::get_changelists(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::remove_from_changelists(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::add(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::cat(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::checkout(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::commit(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::info(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::remove(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::revert(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::status(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::update(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void async_client::get_working_copy_root(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

async_client::async_client()
    : _client(new svn::client()) {}

async_client::~async_client() {}

} // namespace node
