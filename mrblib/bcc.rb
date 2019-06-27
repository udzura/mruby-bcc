module BCC
  def self.entry_all(pid: nil, probe_name: nil, fn_name: nil, code: "")
    do_all(pid, probe_name, fn_name, code)
  end

  class USDT
    def initialize(pid: nil)
      @pid = pid
    end

    def enable_probe(probe: nil, fn_name: nil)
      @probe = probe
      @fn_name = fn_name
    end
    attr_reader :pid, :probe, :fn_name
  end

  class BPF
    def initialize(text: "", usdt_context: nil)
      ::BCC.entry_all(
        pid: usdt_context.pid,
        probe_name: usdt_context.probe,
        fn_name: usdt_context.fn_name,
        code: text
      )
    end

    def trace_fields(&do_each_line)
      f = File.open("/sys/kernel/debug/tracing/trace_pipe", "rb")
      while buf = f.readline
        #p buf
        next if buf.start_with? "CPU:"

        task = buf[0..15].lstrip()
        meta, _addr, msg = buf[17..-1].split(": ")
        pid, cpu, flags, ts = meta.split(" ")
        cpu = cpu[1..-2]
        do_each_line.call(task, pid, cpu, flags, ts, msg)
      end
    end
  end
end
