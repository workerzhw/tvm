#pragma once
#include <tvm/relay/expr.h>
#include <tvm/relay/function.h>
#include <string>

namespace tvm {
namespace relay {
namespace contrib {

std::string LPUCodegen(const Function& func);

}  // namespace contrib
}  // namespace relay
}  // namespace tvm
   //

