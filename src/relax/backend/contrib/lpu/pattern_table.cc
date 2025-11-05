#include <tvm/relay/expr.h>
#include <tvm/relay/op.h>
#include <tvm/relay/pattern_functor.h>

namespace tvm {
namespace relay {
namespace contrib {

using namespace tvm::relay::pattern;

Array<Pattern> LPUOpPatternTable() {
  Array<Pattern> patterns;
  // matmul
  patterns.push_back(MakePattern("lpu.matmul",
    is_op("nn.matmul")(wildcard(), wildcard())));
  // add
  patterns.push_back(MakePattern("lpu.add",
    is_op("add")(wildcard(), wildcard())));
  // relu
  patterns.push_back(MakePattern("lpu.relu",
    is_op("nn.relu")(wildcard())));
  return patterns;
}

TVM_REGISTER_GLOBAL("relay.op.contrib.lpu.pattern_table")
.set_body_typed(LPUOpPatternTable);

}  // namespace contrib
}  // namespace relay
}  // namespace tvm

