#include <node/client.hpp>
#include <node/v8.hpp>

int main() {
    v8::V8::Initialize();
    auto isolate = v8::Isolate::New(v8::Isolate::CreateParams());
    auto context = v8::Context::New(isolate);

    auto str  = v8::New<v8::String>(isolate, "test");
    auto arr  = v8::New<v8::Array>(isolate);
    auto func = v8::New<v8::Function>(context, nullptr);

    auto client = new node::client();


    delete client;

    isolate->Dispose();
}
