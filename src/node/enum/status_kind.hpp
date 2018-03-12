#pragma once

#include <node/v8.hpp>

namespace no {
namespace status_kind {
void init(v8::Local<v8::Object>&  exports,
          v8::Isolate*            isolate,
          v8::Local<v8::Context>& context);
}
} // namespace no
