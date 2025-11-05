# test_lpu.py
import tvm
from tvm import relax,target
from tvm.relax import vm
from tvm.script import relax as R
import numpy as np

@tvm.script.ir_module
class Mod:
    @R.function
    def main(x: R.Tensor((4, 8), "float32"), y: R.Tensor((8, 4), "float32")) -> R.Tensor((4, 4), "float32"):
        with target.Target("lpu"):
            z = R.matmul(x, y)
        return z

# 编译
mod = relax.transform.LegalizeOps()(Mod)
mod = relax.transform.BindTarget("lpu")(mod)
ex = relax.build(mod, target="llvm")

# 运行
vm = relax.VirtualMachine(ex, tvm.cpu())
x_np = np.random.randn(4, 8).astype("float32")
y_np = np.random.randn(8, 4).astype("float32")
out = vm["main"](tvm.nd.array(x_np), tvm.nd.array(y_np))
print(out.numpy().shape)

