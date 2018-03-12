#include "node_kind.hpp"

#include "enum.hpp"

namespace no {
namespace node_kind {
void init(v8::Local<v8::Object>&  exports,
          v8::Isolate*            isolate,
          v8::Local<v8::Context>& context) {
    auto object = no::New<v8::Object>(isolate);

    SET_NODE_KIND(none);
    SET_NODE_KIND(file);
    SET_NODE_KIND(dir);
    SET_NODE_KIND(unknown);

    SetReadOnly(exports, "NodeKind", object);
}
} // namespace node_kind
} // namespace no
