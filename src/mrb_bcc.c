/*
** mrb_bcc.c - BCC module
**
** Copyright (c) Uchio Kondo 2019
**
** See Copyright Notice in LICENSE
*/

#include <bcc/bcc_common.h>
#include <bcc/libbpf.h>
#include <bcc/bcc_usdt.h>

#include <mruby.h>
#include <mruby/data.h>
#include <mruby/error.h>
#include <mruby/string.h>
#include <error.h>
#include <stdio.h>
#include "mrb_bcc.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

#ifdef MRB_DEBUG
  #define _debug_p(...) printf(__VA_ARGS__)
#else
  #define _debug_p(...)
#endif

typedef struct {
  char *str;
  int len;
} mrb_bcc_data;

static const struct mrb_data_type mrb_bcc_data_type = {
  "mrb_bcc_data", mrb_free,
};

#define MRB_BCC_KPROBE 2

void *BCC_tmp;
mrb_state *mrb_tmp;

static mrb_value mrb_bcc_init(mrb_state *mrb, mrb_value self)
{
  mrb_bcc_data *data;
  char *str;
  int len;

  data = (mrb_bcc_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_bcc_data_type;
  DATA_PTR(self) = NULL;

  mrb_get_args(mrb, "s", &str, &len);
  data = (mrb_bcc_data *)mrb_malloc(mrb, sizeof(mrb_bcc_data));
  data->str = str;
  data->len = len;
  DATA_PTR(self) = data;

  return self;
}

void mrb_bcc_attach_callback(const char *binpath, const char *fn_name, uint64_t addr, int pid)
{
  if(!BCC_tmp || !mrb_tmp) {
    printf("wrong call of callback\n");
    abort();
  }

  int fd = bcc_func_load(BCC_tmp, MRB_BCC_KPROBE, fn_name,
                         bpf_function_start(BCC_tmp, fn_name),
                         bpf_function_size(BCC_tmp, fn_name),
                         bpf_module_license(BCC_tmp),
                         bpf_module_kern_version(BCC_tmp),
                         0, NULL, 0);
  if(fd < 0) {
    perror("func_load");
  } else {
    _debug_p("load ok: %s\n", fn_name);

    // self._get_uprobe_evname(b"p", binpath, addr, pid)
    // return b"%s_%s_0x%x_%d" % (prefix, self._probe_repl.sub(b"_", path), addr, pid)
    char eve_name[1024];
    mrb_value binpath_rb = mrb_str_new_cstr(mrb_tmp, binpath);
    mrb_value binpath_rb_normalized =
      mrb_funcall(mrb_tmp, binpath_rb, "tr", 2, mrb_str_new_lit(mrb_tmp, "\\-/.~@+:;"), mrb_str_new_lit(mrb_tmp, "_"));
    sprintf(eve_name, "%s_%s_0x%lx_%d", "p", mrb_string_value_cstr(mrb_tmp, &binpath_rb_normalized), addr, pid);
    _debug_p("event: %s\n", eve_name);
    if(bpf_attach_uprobe(fd, BPF_PROBE_ENTRY, eve_name, binpath, addr, pid) < 0){
      perror("bpf_attach_uprobe");
    }
  }
}

static mrb_value mrb_bcc_do_all(mrb_state *mrb, mrb_value self)
{
  /* mrb_bcc_data *data = DATA_PTR(self); */
  mrb_int pid;
  char *probe_name, *fn_name, *code;
  mrb_get_args(mrb, "izzz", &pid, &probe_name, &fn_name, &code);

  void *USDT = bcc_usdt_new_frompid((int)pid, (char *)NULL);
  if(bcc_usdt_enable_probe(USDT, probe_name, fn_name) < 0) {
    perror("enable_probe");
    mrb_sys_fail(mrb, "bcc_usdt_enable_probe failed");
  }

  const char *gencode = bcc_usdt_genargs(&USDT, 1);
  mrb_value code1 = mrb_str_new_cstr(mrb, gencode);
  mrb_value code2 = mrb_str_new_cstr(mrb, code);
  mrb_value real_code = mrb_str_plus(mrb, code1, code2);

  void *BCC = bpf_module_create_c_from_string(
                                        mrb_string_value_cstr(mrb, &real_code),
                                        0,
                                        NULL,
                                        0,
                                        1);

  _debug_p("funsize: %d\n", bpf_num_functions(BCC));
  /* for(int i = 0; i < bpf_num_functions(BCC); i++) { */
  /*   const char *fn_name = bpf_function_name(BCC, i); */
  /*   if(fn_name) { */
  /*   } */
  /* } */

  BCC_tmp = BCC;
  mrb_tmp = mrb;
  bcc_usdt_foreach_uprobe(USDT, mrb_bcc_attach_callback);
  BCC_tmp = NULL;
  mrb_tmp = NULL;

  return real_code;
}

void mrb_mruby_bcc_gem_init(mrb_state *mrb)
{
  /* TODO: tidy up classes */
  struct RClass *bcc;
  bcc = mrb_define_module(mrb, "BCC");
  /* mrb_define_method(mrb, bcc, "initialize", mrb_bcc_init, MRB_ARGS_REQ(1)); */
  mrb_define_module_function(mrb, bcc, "do_all", mrb_bcc_do_all, MRB_ARGS_REQ(4));
  DONE;
}

void mrb_mruby_bcc_gem_final(mrb_state *mrb)
{
}
