"""The recompiler's test images: hand assembled x86, one case per thing the analysis or code generator handles.

Each case's expected behaviour is in cases/<name>.cpp and its expected output in golden/<name>/.
"""
from make_xbe import D, F32, F64, L, R, R8


class Case:
    def __init__(self, build, ordinals=(), xbe=None, toml=None):
        self.build = build
        self.ordinals = ordinals
        self.xbe = xbe or {}
        self.toml = toml


CASES = {}


def case(**kw):
    def add(fn):
        CASES[fn.__name__] = Case(fn, **kw)
        return fn
    return add


def returns(t, name, value):
    """name: mov eax, value; ret"""
    t.label(name)
    t.emit("b8", D(value), "c3")


# jump tables: each form Analysis::ReadTable, SledTable and the bound search in Walk handle

@case()
def jump_bounded(img):
    t = img.text
    t.fn("dispatch")
    t.emit("83 f8 03")               # cmp eax, 3
    t.emit("77", R8("default"))      # ja default
    t.emit("ff 24 85", L("table"))   # jmp [eax*4 + table]
    t.label("table")
    t.emit(L("c0"), L("c1"), L("c2"), L("c3"))
    for i in range(4):
        returns(t, f"c{i}", 100 + i)
    returns(t, "default", 0xFFFFFFFF)


@case()
def jump_unbounded(img):
    t = img.text
    t.fn("dispatch")
    t.emit("ff 24 85", L("table"))   # jmp [eax*4 + table], no cmp: the table runs while its entries are code
    t.label("table")
    t.emit(L("c0"), L("c1"), L("c2"), D(0))
    for i in range(3):
        returns(t, f"c{i}", 100 + i)


@case()
def jump_byte_bounded(img):
    t = img.text
    t.fn("dispatch")
    t.emit("83 f8 05")                  # cmp eax, 5
    t.emit("77", R8("default"))         # ja default
    t.emit("0f b6 80", L("bytes"))      # movzx eax, byte [eax + bytes]
    t.emit("ff 24 85", L("table"))      # jmp [eax*4 + table]
    t.label("table")
    t.emit(L("c0"), L("c1"), L("c2"))
    t.label("bytes")
    t.emit("00 02 01 01 02 00")
    for i in range(3):
        returns(t, f"c{i}", 100 + i)
    returns(t, "default", 0xFFFFFFFF)


@case()
def jump_byte_unbounded(img):
    t = img.text
    t.fn("dispatch")
    t.emit("0f b6 80", L("bytes"))      # movzx eax, byte [eax + bytes]
    t.emit("ff 24 85", L("table"))      # jmp [eax*4 + table], the byte table runs while it indexes the dword table
    t.label("table")
    t.emit(L("c0"), L("c1"), L("c2"))
    t.label("bytes")
    t.emit("02 00 01 cc")
    for i in range(3):
        returns(t, f"c{i}", 100 + i)


@case()
def jump_backwards(img):
    t = img.text
    t.fn("dispatch")
    t.emit("f7 d8")                     # neg eax
    t.emit("ff 24 85", L("table_end"))  # jmp [eax*4 + table_end], a table that ends at its base
    t.emit("cc cc cc cc")               # keeps the jmp's own displacement out of the table
    t.emit(L("c2"), L("c1"))
    t.label("table_end")
    t.emit(D(0), D(0))
    returns(t, "c1", 101)
    returns(t, "c2", 102)


@case()
def jump_index0_unused(img):
    t = img.text
    t.fn("dispatch")
    t.emit("ff 24 85", L("table"))      # jmp [eax*4 + table], index 0 never taken
    t.label("table")
    t.emit("90 90 90 90", L("c1"), L("c2"), D(0))  # the first slot is no code address
    returns(t, "c1", 101)
    returns(t, "c2", 102)


@case()
def copy_sled(img):
    t = img.text
    t.fn("copy")
    t.emit("83 e1 03")            # and ecx, 3
    t.emit("f7 d9")               # neg ecx
    t.emit("81 c1", L("end"))     # add ecx, end
    t.emit("ff e1")               # jmp ecx
    t.emit("a4 a4 a4")            # movsb, three times
    t.label("end")
    t.emit("c3")


@case(toml=lambda a: f"[[switch]]\nat = {a['jump']:#x}\ntargets = [{a['t0']:#x}, {a['t1']:#x}]")
def switch_override(img):
    t = img.text
    t.fn("dispatch")
    t.label("jump")
    t.emit("ff e0")               # jmp eax, no table to find: the config names the targets
    t.emit("cc")
    returns(t, "t0", 100)
    t.emit("cc")
    returns(t, "t1", 101)


# exceptions

@case(xbe={"gap_fill": "false"})
def seh_scope(img):
    t, d = img.text, img.rdata
    d.label("scope_table")        # _except_handler3's table: previous level, filter, handler
    d.emit(D(0xFFFFFFFF), L("filter"), L("handler"))
    t.fn("guarded")
    t.emit("68", L("scope_table"))  # push scope_table
    t.emit("83 c4 04")              # add esp, 4
    t.emit("b8", D(1), "c3")
    t.emit("cc")
    returns(t, "filter", 2)         # reached only through the table
    t.emit("cc")
    returns(t, "handler", 3)


@case(xbe={"gap_fill": "false", "pointer_seeds": "false"})
def cxx_eh(img):
    t, d = img.text, img.rdata
    d.label("func_info")          # FuncInfo: magic, maxState, pUnwindMap, nTryBlocks, pTryBlockMap
    d.emit(D(0x19930520), D(1), L("unwind_map"), D(1), L("try_map"))
    d.label("unwind_map")         # toState, action
    d.emit(D(0xFFFFFFFF), L("unwind_action"))
    d.label("try_map")            # tryLow, tryHigh, catchHigh, nCatches, handlers
    d.emit(D(0), D(0), D(1), D(1), L("handlers"))
    d.label("handlers")           # adjectives, type, displacement, address
    d.emit(D(0), D(0), D(0), L("catch_block"))
    d.label("catch_ptr")          # the only way to the catch block, as the CRT reaches it
    d.emit(L("catch_block"))

    t.fn("try_fn")
    t.emit("68", L("thunk"))             # push thunk
    t.emit("64 ff 35 00 00 00 00")       # push dword fs:[0]
    t.emit("64 89 25 00 00 00 00")       # mov fs:[0], esp
    t.emit("e8", R("thrower"))           # call thrower
    t.emit("bb", D(0x111))               # mov ebx, 0x111, the path a throw skips
    t.label("continuation")
    t.emit("8b 0c 24")                   # mov ecx, [esp]
    t.emit("64 89 0d 00 00 00 00")       # mov fs:[0], ecx
    t.emit("83 c4 08")                   # add esp, 8
    t.emit("b8", D(0x777))               # mov eax, 0x777
    t.emit("c3")
    t.emit("cc")
    t.label("thrower")                   # what the CRT does: runs the catch block, then jumps where it says
    t.emit("8b 15", L("catch_ptr"))      # mov edx, [catch_ptr]
    t.emit("ff d2")                      # call edx
    t.emit("83 c4 04")                   # add esp, 4
    t.emit("ff e0")                      # jmp eax
    t.emit("cc")
    t.label("thunk")
    t.emit("b8", L("func_info"))         # mov eax, func_info
    t.emit("e9", R("frame_handler"))     # jmp __CxxFrameHandler
    t.emit("cc")
    t.label("frame_handler")
    t.emit("c3")
    t.emit("cc")
    t.label("catch_block")
    t.emit("b8", L("continuation"), "c3")
    t.emit("cc")
    t.label("unwind_action")
    t.emit("b8", D(0x55), "c3")


# x87, the register stack tracked at run time

@case()
def x87_stack(img):
    t, d = img.text, img.rdata
    d.label("a")
    d.emit(F32(1.5))
    d.label("b")
    d.emit(F32(2.25))
    d.label("x25")
    d.emit(F32(2.5))
    d.label("x27")
    d.emit(F32(2.7))
    d.label("three")
    d.emit(F32(3.0))
    d.label("cw_trunc")
    d.emit("7f 0e")          # precision 53 bits with truncation
    d.label("out32")
    d.emit(D(0))
    d.label("out64")
    d.emit(F64(0.0))
    d.label("out80")
    d.emit(bytes(16))

    t.fn("add")
    t.emit("d9 05", L("a"))       # fld dword [a]
    t.emit("d9 05", L("b"))       # fld dword [b]
    t.emit("de c1")               # faddp st1, st0
    t.emit("d9 1d", L("out32"))   # fstp dword [out32]
    t.emit("c3")

    t.fn("across_call")
    t.emit("d9 e8")               # fld1
    t.emit("e8", R("add_pi"))     # call add_pi
    t.emit("dd 1d", L("out64"))   # fstp qword [out64]
    t.emit("c3")
    t.fn("add_pi")
    t.emit("d9 eb")               # fldpi
    t.emit("de c1")               # faddp st1, st0
    t.emit("c3")

    t.fn("round_nearest")
    t.emit("d9 05", L("x25"))     # fld dword [x25]
    t.emit("db 1d", L("out32"))   # fistp dword [out32]
    t.emit("c3")
    t.fn("round_trunc")
    t.emit("d9 2d", L("cw_trunc"))  # fldcw [cw_trunc]
    t.emit("d9 05", L("x27"))       # fld dword [x27]
    t.emit("db 1d", L("out32"))     # fistp dword [out32]
    t.emit("c3")

    t.fn("divide")
    t.emit("d9 e8")               # fld1
    t.emit("d9 05", L("three"))   # fld dword [three]
    t.emit("de f9")               # fdivp st1, st0
    t.emit("dd 1d", L("out64"))   # fstp qword [out64]
    t.emit("c3")

    t.fn("is_below")              # the XDK's compare: fcomp, fnstsw ax, sahf, a jump on the carry
    t.emit("d9 05", L("a"))       # fld dword [a]
    t.emit("d8 1d", L("b"))       # fcomp dword [b]
    t.emit("df e0")               # fnstsw ax
    t.emit("9e")                  # sahf
    t.emit("72", R8("below"))     # jb below
    t.emit("b8", D(0), "c3")
    t.label("below")
    t.emit("b8", D(1), "c3")

    t.fn("fcomip")
    t.emit("d9 05", L("a"))       # fld dword [a]
    t.emit("d9 05", L("b"))       # fld dword [b]
    t.emit("df f1")               # fcomip st0, st1: b against a
    t.emit("dd d8")               # fstp st0
    t.emit("0f 97 c0")            # seta al
    t.emit("0f b6 c0")            # movzx eax, al
    t.emit("c3")

    t.fn("tbyte")
    t.emit("d9 05", L("a"))       # fld dword [a]
    t.emit("db 3d", L("out80"))   # fstp tbyte [out80]
    t.emit("db 2d", L("out80"))   # fld tbyte [out80]
    t.emit("dd 1d", L("out64"))   # fstp qword [out64]
    t.emit("c3")

    t.fn("wrap")                  # past the end of the register file: st is a ring
    t.emit(*["d9 e8"] * 9)        # fld1, nine times
    t.emit(*["dd d8"] * 9)        # fstp st0, nine times
    t.emit("c3")


# segment pushes take a four byte slot in 32 bit code, as their pops do

@case()
def push_segment(img):
    t = img.text
    t.fn("balance")
    t.emit("1e 06 07 1f c3")      # push ds; push es; pop es; pop ds; ret
    t.fn("slot_size")
    t.emit("89 e0")               # mov eax, esp
    t.emit("1e")                  # push ds
    t.emit("29 e0")               # sub eax, esp
    t.emit("1f c3")               # pop ds; ret
    t.fn("word_push")
    t.emit("89 e0")               # mov eax, esp
    t.emit("66 50")               # push ax
    t.emit("29 e0")               # sub eax, esp
    t.emit("66 59 c3")            # pop cx; ret
    t.fn("dword_push")
    t.emit("89 e0")               # mov eax, esp
    t.emit("50")                  # push eax
    t.emit("29 e0")               # sub eax, esp
    t.emit("59 c3")               # pop ecx; ret


@case()
def mmx_sse(img):
    t, d = img.text, img.rdata
    d.emit(bytes(-d.here() & 15))
    d.label("v4a")
    d.emit(F32(1.0), F32(2.0), F32(3.0), F32(4.0))
    d.label("v4b")
    d.emit(F32(0.5), F32(0.25), F32(2.0), F32(-1.0))
    d.label("out128")
    d.emit(bytes(16))
    d.label("q1")
    d.emit("01 00 ff 7f 00 80 ff ff")  # words 1, 32767, -32768, -1
    d.label("q2")
    d.emit("02 00 01 00 ff ff 01 00")  # words 2, 1, -1, 1
    d.label("out64")
    d.emit(bytes(8))
    d.label("f275")
    d.emit(F32(2.75))
    d.label("out32")
    d.emit(D(0))

    t.fn("paddw")
    t.emit("0f 6f 05", L("q1"))     # movq mm0, [q1]
    t.emit("0f fd 05", L("q2"))     # paddw mm0, [q2]
    t.emit("0f 7f 05", L("out64"))  # movq [out64], mm0
    t.emit("0f 77 c3")              # emms; ret
    t.fn("paddsw")
    t.emit("0f 6f 05", L("q1"))     # movq mm0, [q1]
    t.emit("0f ed 05", L("q2"))     # paddsw mm0, [q2]
    t.emit("0f 7f 05", L("out64"))  # movq [out64], mm0
    t.emit("0f 77 c3")              # emms; ret
    t.fn("pmullw_psrlq")
    t.emit("0f 6f 05", L("q1"))     # movq mm0, [q1]
    t.emit("0f d5 05", L("q2"))     # pmullw mm0, [q2]
    t.emit("0f 73 d0 10")           # psrlq mm0, 16
    t.emit("0f 7f 05", L("out64"))  # movq [out64], mm0
    t.emit("0f 77 c3")              # emms; ret
    t.fn("pshufw")
    t.emit("0f 70 05", L("q1"), "1b")  # pshufw mm0, [q1], 0x1B: reversed
    t.emit("0f 7f 05", L("out64"))     # movq [out64], mm0
    t.emit("0f 77 c3")                 # emms; ret

    t.fn("addps_mulps")
    t.emit("0f 28 05", L("v4a"))     # movaps xmm0, [v4a]
    t.emit("0f 58 05", L("v4b"))     # addps xmm0, [v4b]
    t.emit("0f 59 c0")               # mulps xmm0, xmm0
    t.emit("0f 29 05", L("out128"))  # movaps [out128], xmm0
    t.emit("c3")
    t.fn("shufps")
    t.emit("0f 28 0d", L("v4a"))     # movaps xmm1, [v4a]
    t.emit("0f c6 c9 1b")            # shufps xmm1, xmm1, 0x1B
    t.emit("0f 29 0d", L("out128"))  # movaps [out128], xmm1
    t.emit("c3")
    t.fn("cvt")
    t.emit("f3 0f 2c 05", L("f275"))   # cvttss2si eax, [f275]
    t.emit("f3 0f 2d 0d", L("f275"))   # cvtss2si ecx, [f275]
    t.emit("f3 0f 2a c1")              # cvtsi2ss xmm0, ecx
    t.emit("f3 0f 11 05", L("out32"))  # movss [out32], xmm0
    t.emit("c3")
    t.fn("comiss")
    t.emit("f3 0f 10 05", L("f275"))   # movss xmm0, [f275]
    t.emit("0f 2f 05", L("v4a"))       # comiss xmm0, [v4a]: 2.75 against 1.0
    t.emit("0f 97 c0")                 # seta al
    t.emit("0f b6 c0")                 # movzx eax, al
    t.emit("c3")


# library functions a link time code generated image calls with arguments in registers

@case(toml=lambda a: f"""[symbols]
regs = {a['regs']:#x}
fast = {a['fast']:#x}
plain = {a['plain']:#x}
wrapped = {a['wrapped']:#x}
missing = 0

[abi]
regs = ["esi", "stack", "ebx"]""")
def ltcg_args(img):
    t = img.text
    t.fn("call_regs")
    t.emit("be", D(7))            # mov esi, 7
    t.emit("bb", D(5))            # mov ebx, 5
    t.emit("6a 09")               # push 9
    t.emit("e8", R("regs"), "c3")
    t.fn("call_fast")
    t.emit("b9", D(3))            # mov ecx, 3
    t.emit("ba", D(4))            # mov edx, 4
    t.emit("6a 05")               # push 5
    t.emit("e8", R("fast"), "c3")
    t.fn("call_plain")
    t.emit("6a 02")               # push 2
    t.emit("6a 01")               # push 1
    t.emit("e8", R("plain"), "c3")
    t.fn("call_wrapped")
    t.emit("6a 06")               # push 6
    t.emit("e8", R("wrapped"), "c3")
    # the host replaces all but the last: an unreplaced body would trap
    for name in ("regs", "fast", "plain"):
        t.emit("cc")
        t.label(name)
        t.emit("cc")
    t.emit("cc")
    t.label("wrapped")
    t.emit("b8", D(0x70), "c2 04 00")  # mov eax, 0x70; ret 4


@case(toml=lambda a: f"""[[hook]]
at = {a['before']:#x}
call = "hook_before"

[[hook]]
at = {a['after']:#x}
call = "hook_after"
after = true

[[hook]]
at = {a['leave']:#x}
call = "hook_leave"
on_true = "ret"

[[hook]]
at = {a['jump']:#x}
call = "hook_jump"
on_true = {a['jump_to']:#x}

[[hook]]
at = {a['flags']:#x}
call = "hook_flags\"""")
def hooks(img):
    t = img.text
    t.fn("before_after")
    t.emit("b8", D(1))            # mov eax, 1
    t.label("before")
    t.emit("40")                  # inc eax
    t.label("after")
    t.emit("40")                  # inc eax
    t.emit("c3")
    t.fn("leave_fn")
    t.emit("b8", D(1))            # mov eax, 1
    t.label("leave")
    t.emit("b8", D(2), "c3")      # mov eax, 2; ret
    t.fn("jump_fn")
    t.emit("b8", D(1))            # mov eax, 1
    t.label("jump")
    t.emit("b8", D(2))            # mov eax, 2
    t.emit("83 c0 10")            # add eax, 16
    t.label("jump_to")
    t.emit("c3")
    t.fn("flags_fn")              # a hook sees the guest's flags and may change them
    t.emit("31 c0")               # xor eax, eax
    t.label("flags")
    t.emit("0f 94 c0")            # sete al
    t.emit("c3")


@case(ordinals=(49, 156))
def control_flow(img):
    t = img.text
    t.fn("call_next")
    t.emit("e8 00 00 00 00")      # call $+5 only pushes eip
    t.label("after_call")
    t.emit("58 c3")               # pop eax; ret
    t.fn("indirect_call")
    t.emit("ff d1 c3")            # call ecx; ret
    t.fn("callee")
    t.emit("b8", D(42), "c3")
    t.fn("tail")
    t.emit("e9", R("callee"))     # jmp callee, another function's tail
    t.fn("indirect_tail")
    t.emit("ff e1")               # jmp ecx
    t.emit("cc")
    t.fn("loop_fn")
    t.emit("31 c0")               # xor eax, eax
    t.label("loop_top")
    t.emit("40")                  # inc eax
    t.emit("e2", R8("loop_top"))  # loop loop_top
    t.emit("c3")
    t.fn("kernel_call")
    t.emit("ff 15", L("thunk_49"), "c3")  # call [thunk_49]
    t.fn("kernel_tail")
    t.emit("ff 25", L("thunk_156"))       # jmp [thunk_156]
    t.emit("cc")
    t.fn("trap")
    t.emit("0f 0b")               # ud2
    t.emit("cc")
    t.fn("flags_tail")            # flags live across a tail call
    t.emit("39 c8")               # cmp eax, ecx
    t.emit("e9", R("flags_tail_to"))
    t.fn("flags_tail_to")
    t.emit("0f 94 c0")            # sete al
    t.emit("0f b6 c0")            # movzx eax, al
    t.emit("c3")
    t.label("shared_tail")        # below the entry, reached by a jump: the function enters by label
    t.emit("b8", D(9), "c3")
    t.fn("shared_entry")
    t.emit("eb", R8("shared_tail"))


@case()
def int_ops(img):
    t = img.text

    def fn(name, *code):
        t.fn(name)
        t.emit(*code, "c3")

    fn("add64", "01 c8", "11 da")           # add eax, ecx; adc edx, ebx
    fn("sub64", "29 c8", "19 da")           # sub eax, ecx; sbb edx, ebx
    for name, jcc in (("jle", "7e"), ("jl", "7c"), ("jbe", "76"), ("jb", "72"), ("js", "78"), ("jo", "70")):
        t.fn("cmp_" + name)
        t.emit("39 c8", jcc, R8(f"cmp_{name}_yes"))  # cmp eax, ecx; jcc yes
        t.emit("b8", D(0), "c3")
        t.label(f"cmp_{name}_yes")
        t.emit("b8", D(1), "c3")
    t.fn("test_jle")
    t.emit("85 c0 7e", R8("test_yes"))      # test eax, eax; jle yes
    t.emit("b8", D(0), "c3")
    t.label("test_yes")
    t.emit("b8", D(1), "c3")
    t.fn("byte_cmp")
    t.emit("38 c8 7c", R8("byte_yes"))      # cmp al, cl; jl yes
    t.emit("b8", D(0), "c3")
    t.label("byte_yes")
    t.emit("b8", D(1), "c3")
    fn("shl", "d3 e0")                      # shl eax, cl
    fn("shr", "d3 e8")                      # shr eax, cl
    fn("sar", "d3 f8")                      # sar eax, cl
    fn("rol", "d3 c0")                      # rol eax, cl
    fn("ror", "d3 c8")                      # ror eax, cl
    fn("shld", "0f a5 d0")                  # shld eax, edx, cl
    fn("shrd", "0f ad d0")                  # shrd eax, edx, cl
    fn("mul", "f7 e1")                      # mul ecx
    fn("imul", "f7 e9")                     # imul ecx
    fn("imul2", "0f af c1")                 # imul eax, ecx
    fn("idiv", "f7 f9")                     # idiv ecx
    fn("div", "f7 f1")                      # div ecx
    fn("high8", "b4 12", "b0 34")           # mov ah, 0x12; mov al, 0x34
    fn("movsx", "0f be c0")                 # movsx eax, al
    fn("movzx", "0f b7 c0")                 # movzx eax, ax
    fn("cmove", "39 d2", "0f 44 c1")        # cmp edx, edx; cmove eax, ecx
    fn("setl", "39 c8", "0f 9c c0", "0f b6 c0")  # cmp eax, ecx; setl al; movzx eax, al
    fn("rep_stosd", "f3 ab")                # rep stosd
    fn("rep_movsd", "f3 a5")                # rep movsd
    fn("repne_scasb", "f2 ae")              # repne scasb
    fn("lock_xadd", "f0 0f c1 0e")          # lock xadd [esi], ecx
    fn("lock_cmpxchg", "f0 0f b1 0e")       # lock cmpxchg [esi], ecx
    fn("bsf", "0f bc c1")                   # bsf eax, ecx
    fn("bswap", "0f c8")                    # bswap eax
    fn("pushfd_popfd", "f9", "9c", "58")    # stc; pushfd; pop eax
    fn("cdq", "99")                         # cdq
    fn("leave_fn", "55", "89 e5", "83 ec 10", "c9")  # push ebp; mov ebp, esp; sub esp, 16; leave


# what the config says about code and data

@case(toml=lambda a: f"[[function]]\nat = {a['bounded']:#x}\nend = {a['bounded_end']:#x}",
      xbe={"data_ranges": lambda a: f"[[{a['data_start']:#x}, {a['data_end']:#x}]]"})
def analysis_overrides(img):
    t = img.text
    t.fn("start")
    t.emit("b8", D(1), "c3")
    t.emit("cc")
    t.label("gap_found")          # called by nothing: found by the gap fill
    t.emit("b8", D(2), "c3")
    t.emit("cc")
    t.label("data_start")         # would decode as a function if the config did not call it data
    t.emit("b8", D(3), "c3")
    t.label("data_end")
    t.emit("cc")
    t.fn("next_fn")
    t.emit("b8", D(4), "c3")
    t.emit("cc")
    t.label("bounded")            # decoded no further than its end
    t.emit("b8", D(5))
    t.label("bounded_end")
    t.emit("cc cc cc cc")
