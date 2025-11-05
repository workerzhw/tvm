#pragma once
#include <tvm/runtime/module.h>
#include <tvm/runtime/packed_func.h>
#include <string>

namespace tvm {
namespace relay {
namespace contrib {

runtime::Module CreateLPURuntime(const std::string& json_str);

}  // namespace contrib
}  // namespace relay
}  // namespace tvm


