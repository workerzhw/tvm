from tvm import relay

def _register_lpu_op(op_name, supported=True):
    @relay.op.register(op_name, "target.lpu")
    def _func_wrapper(attrs, args):
        return supported
    return _func_wrapper

_register_lpu_op("nn.matmul")
_register_lpu_op("add")
_register_lpu_op("nn.relu")

@relay.op.contrib.register_pattern_table("lpu")
def pattern_table():
    from tvm.relay.dataflow_pattern import wildcard, is_op
    return [
        ("lpu.matmul", is_op("nn.matmul")(wildcard(), wildcard())),
        ("lpu.add", is_op("add")(wildcard(), wildcard())),
        ("lpu.relu", is_op("nn.relu")(wildcard())),
    ]

