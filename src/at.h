// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef H_BITCOIN_AT
#define H_BITCOIN_AT

#include <cstdlib>
#include <memory.h>

#ifndef _WIN32

#  include <stdint.h>

#else
#  ifdef _MSC_VER
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
#  endif
#endif

#include <map>
#include <set>
#include <deque>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <stdexcept>

/*
  [State]
  0x00000000 ; flags (32 bits)
  0x00000000 ; pc (32 bits) program counter
  0x00000000 ; cs (32 bits) call stack counter
  0x00000000 ; us (32 bits) user stack counter
  0x00000000 ; pcs (32 bits) program counter starting point
  0x00000000 ; sleep_until (32 bits) execution to wait until >= this block
  0x0000000000000000 ; stopped at balance (64 bits)*
  0x0000000000000000000000000000000000000000000000000000000000000000 ; pseudo register A (256 bits)
  0x0000000000000000000000000000000000000000000000000000000000000000 ; pseudo register B (256 bits)
*/
struct at_state {
    int32_t flags;
    int32_t pc;
    int32_t cs;
    int32_t us;
    int32_t pcs;
    int32_t sleep_until;
    int64_t stopped_at;
    int64_t a1, a12, a3, a4;
    int64_t b1, b2, b3, b4;
    set <int32_t> jumps; // transient

    // helpful for debugging
    int32_t opc;
    int32_t steps;

    at_state() {
        // ToDo: What was 'pcs'?
        reset();
    }

    void reset() {
        pc = 0;
        opc = 0;

        cs = 0;
        us = 0;

        steps = 0;

        jumps.clear();

        stopped = false;
        finished = false;
    }
};

/*
    flags:
        (1<<31) Reserved
        (1<<30) A register is zero
        (1<<29) B register is zero
        (1<<28) Execution is paused (continue from last executed)
        (1<<27) Execution is stopped (start again from beginning)
        (1<<26) Execution is terminated (does not run again)
        (1<<25) Reserved
        (1<<24) ... (1<<1)
        (1<<0)  Reserved
 */
#define flag_a_zero         (1<<30)
#define flag_b_zero         (1<<29)
#define flag_paused         (1<<28)
#define flag_stopped        (1<<27)
#define flag_terminated     (1<<26)


enum at_op_code {
    at_op_code_NOP = 0x7f,
    at_op_code_SET_VAL = 0x01,
    at_op_code_SET_DAT = 0x02,
    at_op_code_CLR_DAT = 0x03,
    at_op_code_INC_DAT = 0x04,
    at_op_code_DEC_DAT = 0x05,
    at_op_code_ADD_DAT = 0x06,
    at_op_code_SUB_DAT = 0x07,
    at_op_code_MUL_DAT = 0x08,
    at_op_code_DIV_DAT = 0x09,
    at_op_code_BOR_DAT = 0x0a,
    at_op_code_AND_DAT = 0x0b,
    at_op_code_XOR_DAT = 0x0c,
    at_op_code_NOT_DAT = 0x0d,
    at_op_code_SET_IND = 0x0e,
    at_op_code_SET_IDX = 0x0f,
    at_op_code_PSH_DAT = 0x10,
    at_op_code_POP_DAT = 0x11,
    at_op_code_JMP_SUB = 0x12,
    at_op_code_RET_SUB = 0x13,
    at_op_code_JMP_ADR = 0x1a,
    at_op_code_BZR_DAT = 0x1b,
    at_op_code_BNZ_DAT = 0x1e,
    at_op_code_BGT_DAT = 0x1f,
    at_op_code_BLT_DAT = 0x20,
    at_op_code_BGE_DAT = 0x21,
    at_op_code_BLE_DAT = 0x22,
    at_op_code_BEQ_DAT = 0x23,
    at_op_code_BNE_DAT = 0x24,
    at_op_code_SLP_DAT = 0x25,
    at_op_code_FIZ_DAT = 0x26,
    at_op_code_STZ_DAT = 0x27,
    at_op_code_FIN_IMD = 0x28,
    at_op_code_STP_IMD = 0x29,
    at_op_code_SET_PCS = 0x30,
    at_op_code_EXT_FUN = 0x32,
    at_op_code_EXT_FUN_DAT = 0x33,
    at_op_code_EXT_FUN_DAT_2 = 0x34,
    at_op_code_EXT_FUN_RET = 0x35,
    at_op_code_EXT_FUN_RET_DAT = 0x36,
    at_op_code_EXT_FUN_RET_DAT_2 = 0x37,
};

struct at_header {
    int16_t version;
    int16_t reserved;
    int16_t code_pages; // (number of 256 byte pages required)
    int16_t data_pages; // (number of 256 byte pages required)
    int16_t call_stack; // (number of 256 byte pages required)
    int16_t user_stack; // (number of 256 byte pages required)
};

class AutomatedTransaction {
private:
    at_state _state;
    int8_t *_code;
    int32_t _code_size;
    int8_t *_data;
    int32_t _data_size;
    int32_t _call_stack_size;
    int32_t _user_stack_size;

public:
    AutomatedTransaction(int8_t *p_code, int32_t csize, int8_t *p_data, int32_t dsize, int32_t cssize, int32_t ussize);

    int get_fun(int16_t &fun);

    int get_val(int64_t &val);

    int get_addr(int32_t &addr, bool is_code = false);

    int get_addrs(int32_t &addr1, int32_t &addr2);

    int get_addr_val(int32_t &addr, int64_t &val);

    int get_addr_off(int32_t &addr, int8_t &off);

    int get_addrs_off(int32_t &addr1, int32_t &addr2, int8_t &off);

    int get_fun_addr(int16_t &fun, int32_t &addr);

    int get_fun_addrs(int16_t &fun, int32_t &addr1, int32_t &addr2);

    int process_op(bool disassemble = false, bool determine_jumps = false);

    void dump_state();

    void dump_bytes(int8_t *p_bytes, int num);

    void list_code(bool determine_jumps = false);

    void reset_machine();

    bool isFinished();

    bool isStopped();

    bool isPaused();
};

#endif
