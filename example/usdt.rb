pid = ARGV[0]

bpf_text = <<CLANG
#include <uapi/linux/ptrace.h>
int do_trace(struct pt_regs *ctx) {
    char buf[256];

    bpf_usdt_readarg_p(1, ctx, &buf, 256);
    bpf_trace_printk("%s\\n", buf);
    return 0;
};
CLANG

u = BCC::USDT.new(pid: pid.to_i)
u.enable_probe(probe: "mruby", fn_name: "do_trace")

b = BCC::BPF.new(text: bpf_text, usdt_context: u)

printf("%-18s %-16s %-6s %s\n", "TIME(s)", "COMM", "PID", "mruby-probe")

b.trace_fields do |task, pid, cpu, flags, ts, msg|
  printf("%-18.9f %-16s %-6d %s", ts, task, pid, msg)
end
