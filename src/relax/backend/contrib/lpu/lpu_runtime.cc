#include "lpu_runtime.h"
#include <tvm/runtime/registry.h>
#include <dlpack/dlpack.h>
#include <nlohmann/json.hpp>
#include <dlfcn.h>

using json = nlohmann::json;

// 加载 liblpu.so
void* lpu_lib = nullptr;
void (*lpu_matmul)(int, int, int, const float*, const float*, float*) = nullptr;
void (*lpu_add)(int, const float*, const float*, float*) = nullptr;
void (*lpu_relu)(int, const float*, float*) = nullptr;

void LoadLPULib() {
  if (!lpu_lib) {
    lpu_lib = dlopen("liblpu.so", RTLD_LAZY);
    ICHECK(lpu_lib) << "Failed to load liblpu.so";
    *(void**)(&lpu_matmul) = dlsym(lpu_lib, "lpu_matmul");
    *(void**)(&lpu_add) = dlsym(lpu_lib, "lpu_add");
    *(void**)(&lpu_relu) = dlsym(lpu_lib, "lpu_relu");
  }
}

class LPURuntimeNode : public runtime::ModuleNode {
 public:
  explicit LPURuntimeNode(const std::string& json_str) {
    json j = json::parse(json_str);
    for (auto& func : j["functions"]) {
      std::string op = func["op"];
      kernels_[op] = func;
    }
  }

  PackedFunc GetFunction(const std::string& name,
                         const ObjectPtr<Object>& sptr_to_self) override {
    if (name == "run") {
      return PackedFunc([this](TVMArgs args, TVMRetValue* rv) {
        DLTensor* input = args[0];
        DLTensor* output = args[1];
        RunKernels(input, output);
      });
    }
    return PackedFunc();
  }

  const char* type_key() const override { return "lpu"; }


 private:
  void RunKernels(DLTensor* input, DLTensor* output) {
    LoadLPULib();
    float* in_ptr = static_cast<float*>(input->data);
    float* out_ptr = static_cast<float*>(output->data);
    int n = 1;
    for (int i = 0; i < input->ndim; ++i) n *= input->shape[i];

    // 简化：只跑第一个 kernel
    auto it = kernels_.begin();
    if (it->first == "nn.matmul") {
      // 简化：假设 A, B 已准备好
      lpu_matmul(n, n, n, in_ptr, in_ptr, out_ptr);
    } else if (it->first == "add") {
      lpu_add(n, in_ptr, in_ptr, out_ptr);
    } else if (it->first == "nn.relu") {
      lpu_relu(n, in_ptr, out_ptr);
    }
  }

  std::unordered_map<std::string, json> kernels_;
};

runtime::Module CreateLPURuntime(const std::string& json_str) {
  auto ptr = make_object<LPURuntimeNode>(json_str);
  return runtime::Module(ptr);
}

TVM_REGISTER_GLOBAL("relay.ext.lpu.create")
.set_body_typed(CreateLPURuntime);

}  // namespace contrib
}  // namespace relay
}  // namespace tvm

