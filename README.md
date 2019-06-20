# mruby-bcc   [![Build Status](https://travis-ci.org/udzura/mruby-bcc.svg?branch=master)](https://travis-ci.org/udzura/mruby-bcc)

mruby Binding of BPF-Based Linux Tracing: see https://github.com/iovisor/bcc

## Install as a mrbgems

Add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

  # ... (snip) ...

  conf.gem github: 'udzura/mruby-bcc'
end
```

## example

```ruby
# TODO: Make this code work:
# a clone of https://github.com/iovisor/bcc/blob/master/examples/hello_world.py
bpf = BCC::BPF.new(
  text: 'int kprobe__sys_clone(void *ctx) { bpf_trace_printk("Hello, World!\\n"); return 0; }'
)
bpf.trace_print
```

## License

Apache License 2.0 (as BCC itself is): See LICENSE file
