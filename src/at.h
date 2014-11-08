// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef H_BITCOIN_AT
#define H_BITCOIN_AT

#include <string>
#include <vector>

#include <inttypes.h>

//#include <boost/foreach.hpp>
//#include <boost/variant.hpp>

//#include "keystore.h"
//#include "bignum.h"

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
 int64_t a_reg[4];
 int64_t b_reg[4];
};

enum at_op_code
{
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

#endif
