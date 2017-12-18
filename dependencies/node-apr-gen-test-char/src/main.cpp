#include <node.h>
#include <v8.h>

#include "../../apr/tools/gen_test_char.c"

static void Run(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::String::Utf8Value path(args[0]);
    freopen(*path, "w", stdout);
    main(0, nullptr);
    fclose(stdout);
}

void init(v8::Local<v8::Object> exports)
{
    NODE_SET_METHOD(exports, "run", Run);
}

NODE_MODULE(gen_test_char, init)
