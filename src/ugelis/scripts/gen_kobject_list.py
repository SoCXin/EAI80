#!/usr/bin/env python3
#
# Copyright (c) 2017 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

import sys
import argparse
import pprint
import os
import struct
from elf_helper import ElfHelper, kobject_to_enum

kobjects = [
    "k_alert",
    "k_msgq",
    "k_mutex",
    "k_pipe",
    "k_queue",
    "k_poll_signal",
    "k_sem",
    "k_stack",
    "k_thread",
    "k_timer",
    "_k_thread_stack_element",
    "device"
]

subsystems = [
    "adc_driver_api",
    "aio_cmp_driver_api",
    "counter_driver_api",
    "crypto_driver_api",
    "dma_driver_api",
    "flash_driver_api",
    "gpio_driver_api",
    "i2c_driver_api",
    "i2s_driver_api",
    "ipm_driver_api",
    "led_driver_api",
    "pinmux_driver_api",
    "pwm_driver_api",
    "entropy_driver_api",
    "rtc_driver_api",
    "sensor_driver_api",
    "spi_driver_api",
    "uart_driver_api",
]


header = """%compare-lengths
%define lookup-function-name _k_object_lookup
%language=ANSI-C
%global-table
%struct-type
%{
#include <kernel.h>
#include <toolchain.h>
#include <syscall_handler.h>
#include <string.h>
%}
struct _k_object;
%%
"""


# Different versions of gperf have different prototypes for the lookup
# function, best to implement the wrapper here. The pointer value itself is
# turned into a string, we told gperf to expect binary strings that are not
# NULL-terminated.
footer = """%%
struct _k_object *_k_object_gperf_find(void *obj)
{
    return _k_object_lookup((const char *)obj, sizeof(void *));
}

void _k_object_gperf_wordlist_foreach(_wordlist_cb_func_t func, void *context)
{
    int i;

    for (i = MIN_HASH_VALUE; i <= MAX_HASH_VALUE; i++) {
        if (wordlist[i].name) {
            func(&wordlist[i], context);
        }
    }
}

#ifndef CONFIG_DYNAMIC_OBJECTS
struct _k_object *_k_object_find(void *obj)
	ALIAS_OF(_k_object_gperf_find);

void _k_object_wordlist_foreach(_wordlist_cb_func_t func, void *context)
	ALIAS_OF(_k_object_gperf_wordlist_foreach);
#endif
"""


def write_gperf_table(fp, eh, objs, static_begin, static_end):
    fp.write(header)

    for obj_addr, ko in objs.items():
        obj_type = ko.type_name
        # pre-initialized objects fall within this memory range, they are
        # either completely initialized at build time, or done automatically
        # at boot during some PRE_KERNEL_* phase
        initialized = obj_addr >= static_begin and obj_addr < static_end

        byte_str = struct.pack("<I" if eh.little_endian else ">I", obj_addr)
        fp.write("\"")
        for byte in byte_str:
            val = "\\x%02x" % byte
            fp.write(val)

        fp.write(
            "\",{},%s,%s,%d\n" %
            (obj_type,
             "K_OBJ_FLAG_INITIALIZED" if initialized else "0",
             ko.data))

    fp.write(footer)


driver_macro_tpl = """
#define Z_SYSCALL_DRIVER_%(driver_upper)s(ptr, op) Z_SYSCALL_DRIVER_GEN(ptr, op, %(driver_lower)s, %(driver_upper)s)
"""

def write_validation_output(fp):
    fp.write("#ifndef __DRIVER_VALIDATION_GEN_H__\n")
    fp.write("#define __DRIVER_VALIDATION_GEN_H__\n")

    fp.write("""#define Z_SYSCALL_DRIVER_GEN(ptr, op, driver_lower_case, driver_upper_case) \\
		(Z_SYSCALL_OBJ(ptr, K_OBJ_DRIVER_##driver_upper_case) || \\
		 Z_SYSCALL_DRIVER_OP(ptr, driver_lower_case##_driver_api, op))
                """)

    for subsystem in subsystems:
        subsystem = subsystem.replace("_driver_api", "")

        fp.write(driver_macro_tpl % {
            "driver_lower": subsystem.lower(),
            "driver_upper": subsystem.upper(),
        })

    fp.write("#endif /* __DRIVER_VALIDATION_GEN_H__ */\n")


def write_kobj_types_output(fp):
    fp.write("/* Core kernel objects */\n")
    for kobj in kobjects:
        if kobj == "device":
            continue

        fp.write("%s,\n" % kobject_to_enum(kobj))

    fp.write("/* Driver subsystems */\n")
    for subsystem in subsystems:
        subsystem = subsystem.replace("_driver_api", "").upper()
        fp.write("K_OBJ_DRIVER_%s,\n" % subsystem)


def write_kobj_otype_output(fp):
    fp.write("/* Core kernel objects */\n")
    for kobj in kobjects:
        if kobj == "device":
            continue

        fp.write('case %s: return "%s";\n' % (kobject_to_enum(kobj), kobj))

    fp.write("/* Driver subsystems */\n")
    for subsystem in subsystems:
        subsystem = subsystem.replace("_driver_api", "")
        fp.write('case K_OBJ_DRIVER_%s: return "%s driver";\n' % (
            subsystem.upper(),
            subsystem
        ))

def write_kobj_size_output(fp):
    fp.write("/* Non device/stack objects */\n")
    for kobj in kobjects:
        # device handled by default case. Stacks are not currently handled,
        # if they eventually are it will be a special case.
        if kobj == "device" or kobj == "_k_thread_stack_element":
            continue

        fp.write('case %s: return sizeof(struct %s);\n' %
                (kobject_to_enum(kobj), kobj))

def parse_args():
    global args

    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("-k", "--kernel", required=False,
                        help="Input ugelis ELF binary")
    parser.add_argument(
        "-g", "--gperf-output", required=False,
        help="Output list of kernel object addresses for gperf use")
    parser.add_argument(
        "-V", "--validation-output", required=False,
        help="Output driver validation macros")
    parser.add_argument(
        "-K", "--kobj-types-output", required=False,
        help="Output k_object enum values")
    parser.add_argument(
        "-S", "--kobj-otype-output", required=False,
        help="Output case statements for otype_to_str()")
    parser.add_argument(
        "-Z", "--kobj-size-output", required=False,
        help="Output case statements for obj_size_get()")
    parser.add_argument("-v", "--verbose", action="store_true",
                        help="Print extra debugging information")
    args = parser.parse_args()
    if "VERBOSE" in os.environ:
        args.verbose = 1


def main():
    parse_args()

    if args.gperf_output:
        eh = ElfHelper(args.kernel, args.verbose, kobjects, subsystems)
        syms = eh.get_symbols()
        max_threads = syms["CONFIG_MAX_THREAD_BYTES"] * 8
        objs = eh.find_kobjects(syms)

        if eh.get_thread_counter() > max_threads:
            sys.stderr.write("Too many thread objects (%d)\n" % thread_counter)
            sys.stderr.write("Increase CONFIG_MAX_THREAD_BYTES to %d\n",
                             -(-thread_counter // 8))
            sys.exit(1)

        with open(args.gperf_output, "w") as fp:
            write_gperf_table(fp, eh, objs,
                              syms["_static_kernel_objects_begin"],
                              syms["_static_kernel_objects_end"])

    if args.validation_output:
        with open(args.validation_output, "w") as fp:
            write_validation_output(fp)

    if args.kobj_types_output:
        with open(args.kobj_types_output, "w") as fp:
            write_kobj_types_output(fp)

    if args.kobj_otype_output:
        with open(args.kobj_otype_output, "w") as fp:
            write_kobj_otype_output(fp)

    if args.kobj_size_output:
        with open(args.kobj_size_output, "w") as fp:
            write_kobj_size_output(fp)

if __name__ == "__main__":
    main()
