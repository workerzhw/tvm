#include <string>
#include <tvm/runtime/module.h>
//#include <tvm/tir/lowered_func.h>
#include <tvm/node/container.h>
#include <tvm/runtime/registry.h>
#include "../../../target/source/codegen_c.h"
#include <iostream>

namespace tvm {

namespace codegen {

class CodeGenHello : public CodeGenC {
public:
  std::string Finish() {
    decl_stream << "// hello tvm backend!" << std::endl;
    return CodeGenC::Finish();
  }
};

}

namespace runtime {

class HelloModuleNode : public ModuleNode {
public:
  explicit HelloModuleNode(std::string code) : code_(std::move(code)) {}
  const char *type_key() const final {
    return "hello";
  }
  PackedFunc GetFunction(
      const std::string &name,
      const ObjectPtr<Object> &sptr_to_self) final {
    auto code = code_;
    return PackedFunc([code](TVMArgs args, TVMRetValue *ret) {
      std::cout << code << std::endl;
    });
  }
  std::string GetSource(const std::string &format) final {
    return code_;
  }
private:
  std::string code_;
};

Module BulidHello(Array<tir::LoweredFunc> funcs, const std::string &target) {
  codegen::CodeGenHello cg;
  for (auto &f : funcs)
    cg.AddFunction(f);
  return Module(make_object<HelloModuleNode>(cg.Finish()));
}

TVM_REGISTER_GLOBAL("codegen.build_hello")
.set_body_typed(BulidHello);

}

}

