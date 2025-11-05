# test_lpu.py
import tvm
from tvm import relax,target
from tvm.relax import vm
from tvm.script import relax as R
import numpy as np
import os



# 编译
def prepare_graph_lib(base_path):
    x = R.Tensor("x", shape=(2, 2), dtype="float32")
    y = R.Tensor("y", shape=(2, 2), dtype="float32")
    params = {"y": np.ones((2, 2), dtype="float32")}
    mod = tvm.IRModule.from_expr(R.Function([x, y], x + y))
    compiled_lib = R.build(mod, tvm.target.create("llvm"), params=params)
    dylib_path = os.path.join(base_path, "test_relay_add.so")
    compiled_lib.export_library(dylib_path)

# 运行

if __name__ == "__main__":
    prepare_graph_lib("./")

