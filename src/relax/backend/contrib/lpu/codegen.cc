#include "codegen.h"
#include <tvm/relay/analysis.h>
#include <tvm/relay/transform.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace tvm {
namespace relay {
namespace contrib {

std::string LPUCodegen(const Function& func) {
  json j;
  j["version"] = 1;
  j["functions"] = json::array();

  auto visitor = [&](const CallNode* call) {
    if (call->op->IsInstance<OpNode>()) {
      std::string op_name = call->op.as<OpNode>()->name;
      json kernel;
      kernel["op"] = op_name;
      kernel["inputs"] = json::array();
      for (auto arg : call->args) {
        kernel["inputs"].push_back(arg->ToString());
      }
      j["functions"].push_back(kernel);
    }
  };

  PostOrderVisit(func->body, visitor);
  return j.dump(2);
}

TVM_REGISTER_GLOBAL("relay.ext.lpu.compile")
.set_body_typed([](Function func) {
  return LPUCodegen(func);
});

}  // namespace contrib
}  // namespace relay
}  // namespace tvm

