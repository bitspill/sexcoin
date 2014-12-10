// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

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

const int32_t c_code_page_bytes = 512;
const int32_t c_data_page_bytes = 512;

const int32_t c_call_stack_page_bytes = 256;
const int32_t c_user_stack_page_bytes = 256;


#include "at.h"

using namespace std;


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
    AutomatedTransaction(int8_t* p_code, int32_t csize, int8_t* p_data, int32_t dsize, int32_t cssize, int32_t ussize) {
        _state.reset();
        _code = p_code;
        _code_size = csize;
        _data = p_data;
        _data_size = dsize;
        _call_stack_size = cssize;
        _user_stack_size = ussize;
    }

    int get_fun( int16_t& fun )
    {
        if( _state.pc + ( int32_t )(sizeof( int16_t )) >= _code_size )
        return -1;
        else
        {
            fun = *( int16_t* )( _code + _state.pc + 1 );

            return 0;
        }
    }

    int get_val( int64_t& val )
    {
        if( _state.pc + ( int32_t )(sizeof( int64_t )) >= _code_size )
        return -1;
        else
        {
            val = *( int64_t* )( _code + _state.pc + 1 );

            return 0;
        }
    }

    int get_addr( int32_t& addr, bool is_code = false )
    {
        if( _state.pc + ( int32_t )(sizeof( int32_t )) >= _code_size )
        return -1;
        else
        {
            addr = *( int32_t* )( _code + _state.pc + 1 );

            if( addr < 0 || ( is_code && addr >= _code_size ) )
                return -1;
            else if( !is_code && ( ( addr * 8 ) < 0 || ( addr * 8 ) + ( int32_t )(sizeof( int64_t )) > _data_size ) )
            return -1;
            else
            return 0;
        }
    }

    int get_addrs( int32_t& addr1, int32_t& addr2 )
    {
        if( _state.pc + ( int32_t )(sizeof( int32_t )) + ( int32_t )(sizeof( int32_t )) >= _code_size )
        return -1;
        else
        {
            addr1 = *( int32_t* )( _code + _state.pc + 1 );
            addr2 = *( int32_t* )( _code + _state.pc + 1 + sizeof( int32_t ) );

            if( addr1 < 0 || addr2 < 0
                    || ( addr1 * 8 ) < 0 || ( addr2 * 8 ) < 0
                    || ( addr1 * 8 ) + ( int32_t )sizeof( int64_t ) > _data_size
                        || ( addr2 * 8 ) + ( int32_t )sizeof( int64_t ) > _data_size )
            return -1;
            else
            return 0;
        }
    }

    int get_addr_off( int32_t& addr, int8_t& off )
    {
        if( _state.pc + ( int32_t )(sizeof( int32_t )) + ( int32_t )(sizeof( int8_t )) >= _code_size )
        return -1;
        else
        {
            addr = *( int32_t* )( _code + _state.pc + 1 );
            off = *( int8_t* )( _code + _state.pc + 1 + sizeof( int32_t ) );

            if( addr < 0 || ( addr * 8 ) < 0
                    || ( addr * 8 ) + ( int32_t )(sizeof( int64_t )) > _data_size || _state.pc + off >= _code_size )
            return -1;
            else
            return 0;
        }
    }

    int get_addrs_off( int32_t& addr1, int32_t& addr2, int8_t& off )
    {
        if( _state.pc + ( int32_t )(sizeof( int32_t )) + ( int32_t )(sizeof( int32_t )) + ( int32_t )(sizeof( int8_t )) >= _code_size )
        return -1;
        else
        {
            addr1 = *( int32_t* )( _code + _state.pc + 1 );
            addr2 = *( int32_t* )( _code + _state.pc + 1 + sizeof( int32_t ) );
            off = *( int8_t* )( _code + _state.pc + 1 + sizeof( int32_t ) + sizeof( int32_t ) );

            if( addr1 < 0 || addr2 < 0
                    || ( addr1 * 8 ) < 0 || ( addr2 * 8 ) < 0
                    || ( addr1 * 8 ) + ( int32_t )(sizeof( int64_t )) > _data_size
                        || ( addr2 * 8 ) + ( int32_t )(sizeof( int64_t )) > _data_size || _state.pc + off >= _code_size )
            return -1;
            else
            return 0;
        }
    }

    int get_fun_addr( int16_t& fun, int32_t& addr )
    {
        if( _state.pc + ( int32_t )(sizeof( int16_t )) + ( int32_t )(sizeof( int32_t )) >= _code_size )
        return -1;
        else
        {
            fun = *( int16_t* )( _code + _state.pc + 1 );
            addr = *( int32_t* )( _code + _state.pc + 1 + sizeof( int16_t ) );

            if( addr < 0 || ( addr * 8 ) < 0 || ( addr * 8 ) + ( int32_t )(sizeof( int64_t )) > _data_size )
            return -1;
            else
            return 0;
        }
    }

    int get_fun_addrs( int16_t& fun, int32_t& addr1, int32_t& addr2 )
    {
        if (_state.pc + (int32_t)(sizeof(int16_t))
                + (int32_t)(sizeof(int32_t)) + (int32_t)(sizeof(int32_t)) >= _code_size)
            return -1;
        else {
            fun = *(int16_t * )(_code + _state.pc + 1);
            addr1 = *(int32_t * )(_code + _state.pc + 1 + sizeof(int16_t));
            addr2 = *(int32_t * )(_code + _state.pc + 1 + sizeof(int16_t) + sizeof(int32_t));

            if (addr1 < 0 || addr2 < 0
                    || (addr1 * 8) < 0 || (addr2 * 8) < 0
                    || (addr1 * 8) + (int32_t)(sizeof(int64_t)) > _data_size
                    || (addr2 * 8) + (int32_t)(sizeof(int64_t)) > _data_size)
                return -1;
            else
                return 0;
        }
    }

    int get_addr_val( int32_t& addr, int64_t& val )
    {
        if( _state.pc + ( int32_t )sizeof( int32_t ) + ( int32_t )sizeof( int64_t ) >= _code_size )
            return -1;
        else
        {
            addr = *( int32_t* )( _code + _state.pc + 1 );
            val = *( int64_t* )( _code + _state.pc + 1 + ( int32_t )sizeof( int32_t ) );

            if( addr < 0 || ( addr * 8 ) < 0 || ( addr * 8 ) + ( int32_t )sizeof( int64_t ) > _data_size )
                return -1;
            else
                return 0;
        }
    }

    int process_op( bool disassemble = false, bool determine_jumps = false)
    {
        int rc = 0;

        bool invalid = false;
        bool had_overflow = false;

        if (_code_size < 1 || _state.pc >= _code_size)
            return 0;

        if (determine_jumps)
            _state.jumps.insert(_state.pc);

        int8_t op = _code[_state.pc];

        if (op && disassemble && !determine_jumps) {
            cout << hex << setw(8) << setfill('0') <<_state.pc;
            if (_state.pc == _state.opc)
                cout << "* ";
            else
                cout << "  ";
        }

        if (op == at_op_code_NOP) {
            if (disassemble) {
                if (!determine_jumps)
                    cout << "NOP\n";
                while (true) {
                    ++rc;
                    if (_state.pc + rc >= _code_size || _code[_state.pc + rc] != at_op_code_NOP)
                        break;
                }
            }
            else
                while (true) {
                    ++rc;
                    ++_state.pc;
                    if (_state.pc >= _code_size || _code[_state.pc] != at_op_code_NOP)
                        break;
                }
        }
        else if (op == at_op_code_SET_VAL) {
            int32_t addr;
            int64_t val;
            rc = get_addr_val(addr, val);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t) + sizeof(int64_t);

                if (disassemble) {
                    if (!determine_jumps)
                        cout << "SET @" << hex << setw(8) << setfill('0')
                                << addr << " #" << setw(16) << setfill('0') << val << '\n';
                }
                else {
                   _state.pc += rc;
                    *(int64_t * )(_data + (addr * 8)) = val;
                }
            }
        }
        else if (op == at_op_code_SET_DAT) {
            int32_t addr1, addr2;
            rc = get_addrs(addr1, addr2);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t) + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps)
                        cout << "SET @" << hex << setw(8) << setfill('0')
                                << addr1 << " $" << setw(8) << setfill('0') << addr2 << '\n';
                }
                else {
                   _state.pc += rc;
                    *(int64_t * )(_data + (addr1 * 8)) = *(int64_t * )(_data + (addr2 * 8));
                }
            }
        }
        else if (op == at_op_code_CLR_DAT) {
            int32_t addr;
            rc = get_addr(addr);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps)
                        cout << "CLR @" << hex << setw(8) << setfill('0') << addr << '\n';
                }
                else {
                   _state.pc += rc;
                    *(int64_t * )(_data + (addr * 8)) = 0;
                }
            }
        }
        else if (op == at_op_code_INC_DAT || op == at_op_code_DEC_DAT || op == at_op_code_NOT_DAT) {
            int32_t addr;
            rc = get_addr(addr);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (op == at_op_code_INC_DAT)
                            cout << "INC @";
                        else if (op == at_op_code_DEC_DAT)
                            cout << "DEC @";
                        else
                            cout << "NOT @";

                        cout << hex << setw(8) << setfill('0') << addr << '\n';
                    }
                }
                else {
                   _state.pc += rc;

                    if (op == at_op_code_INC_DAT)
                        ++*(int64_t * )(_data + (addr * 8));
                    else if (op == at_op_code_DEC_DAT)
                        --*(int64_t * )(_data + (addr * 8));
                    else
                        *(int64_t * )(_data + (addr * 8)) = ~*(int64_t * )(_data + (addr * 8));
                }
            }
        }
        else if (op == at_op_code_ADD_DAT || op == at_op_code_SUB_DAT
                || op == at_op_code_MUL_DAT || op == at_op_code_DIV_DAT) {
            int32_t addr1, addr2;
            rc = get_addrs(addr1, addr2);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t) + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (op == at_op_code_ADD_DAT)
                            cout << "ADD @";
                        else if (op == at_op_code_SUB_DAT)
                            cout << "SUB @";
                        else if (op == at_op_code_MUL_DAT)
                            cout << "MUL @";
                        else
                            cout << "DIV @";

                        cout << hex << setw(8) << setfill('0')
                                << addr1 << " $" << setw(8) << setfill('0') << addr2 << '\n';
                    }
                }
                else {
                    int64_t val = *(int64_t * )(_data + (addr2 * 8));

                    if (op == at_op_code_DIV_DAT && val == 0)
                        rc = -2;
                    else {
                       _state.pc += rc;

                        if (op == at_op_code_ADD_DAT)
                            *(int64_t * )(_data + (addr1 * 8)) += *(int64_t * )(_data + (addr2 * 8));
                        else if (op == at_op_code_SUB_DAT)
                            *(int64_t * )(_data + (addr1 * 8)) -= *(int64_t * )(_data + (addr2 * 8));
                        else if (op == at_op_code_MUL_DAT)
                            *(int64_t * )(_data + (addr1 * 8)) *= *(int64_t * )(_data + (addr2 * 8));
                        else
                            *(int64_t * )(_data + (addr1 * 8)) /= *(int64_t * )(_data + (addr2 * 8));
                    }
                }
            }
        }
        else if (op == at_op_code_BOR_DAT
                || op == at_op_code_AND_DAT || op == at_op_code_XOR_DAT) {
            int32_t addr1, addr2;
            rc = get_addrs(addr1, addr2);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t) + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (op == at_op_code_BOR_DAT)
                            cout << "BOR @";
                        else if (op == at_op_code_AND_DAT)
                            cout << "AND @";
                        else
                            cout << "XOR @";

                        cout << hex << setw(8) << setfill('0')
                                << addr1 << " $" << setw(16) << setfill('0') << addr2 << '\n';
                    }
                }
                else {
                   _state.pc += rc;
                    int64_t val = *(int64_t * )(_data + (addr2 * 8));

                    if (op == at_op_code_BOR_DAT)
                        *(int64_t * )(_data + (addr1 * 8)) |= val;
                    else if (op == at_op_code_AND_DAT)
                        *(int64_t * )(_data + (addr1 * 8)) &= val;
                    else
                        *(int64_t * )(_data + (addr1 * 8)) ^= val;
                }
            }
        }
        else if (op == at_op_code_SET_IND) {
            int32_t addr1, addr2;
            rc = get_addrs(addr1, addr2);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t) + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps)
                        cout << "SET @" << hex << setw(8) << setfill('0')
                                << addr1 << " $($" << setw(8) << setfill('0') << addr2 << ")\n";
                }
                else {
                    int64_t addr = *(int64_t * )(_data + (addr2 * 8));

                    if (addr < 0 || (addr * 8) < 0 || (addr * 8) + (int32_t)sizeof(int64_t) > _data_size )
                    rc = -1;
                    else
                    {
                       _state.pc += rc;
                        *(int64_t * )(_data + (addr1 * 8)) = *(int64_t * )(_data + (addr * 8));
                    }
                }
            }
        }
        else if (op == at_op_code_SET_IDX) {
            int32_t addr1, addr2;
            rc = get_addrs(addr1, addr2);

            int32_t size = sizeof(int32_t) + sizeof(int32_t);

            if (rc == 0 || disassemble) {
                int32_t addr3;
                rc = get_addr(addr3);

                if (rc == 0 || disassemble) {
                    rc = 1 + size + sizeof(int32_t);

                    if (disassemble) {
                        if (!determine_jumps)
                            cout << "SET @" << hex << setw(8) << setfill('0')
                                    << addr1 << " $($" << setw(8) << setfill('0') << addr2
                                    << "+$" << setw(8) << setfill('0') << addr3 << ")\n";
                    }
                    else {
                        int64_t base = *(int64_t * )(_data + (addr2 * 8));
                        int64_t offs = *(int64_t * )(_data + (addr3 * 8));

                        int64_t addr = base + offs;

                        if (addr < 0 || (addr * 8) < 0 || (addr * 8) + (int32_t)sizeof(int64_t) > _data_size )
                        rc = -1;
                        else
                        {
                           _state.pc += rc;
                            *(int64_t * )(_data + (addr1 * 8)) = *(int64_t * )(_data + (addr * 8));
                        }
                    }
                }
            }
        }
        else if (op == at_op_code_PSH_DAT || op == at_op_code_POP_DAT) {
            int32_t addr;
            rc = get_addr(addr);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (op == at_op_code_PSH_DAT)
                            cout << "PSH $";
                        else
                            cout << "POP @";

                        cout << hex << setw(8) << setfill('0') << addr << '\n';
                    }
                }
                else if ((op == at_op_code_PSH_DAT && _state.us == (_user_stack_size / 8))
                        || (op == at_op_code_POP_DAT && _state.us == 0))
                    rc = -1;
                else {
                   _state.pc += rc;
                    if (op == at_op_code_PSH_DAT)
                        *(int64_t * )(_data + _data_size + _call_stack_size + _user_stack_size
                                - (++_state.us * 8)) = *(int64_t * )(_data + (addr * 8));
                    else
                        *(int64_t * )(_data + (addr * 8))
                                = *(int64_t * )(_data + _data_size + _call_stack_size + _user_stack_size - (_state.us-- * 8));
                }
            }
        }
        else if (op == at_op_code_JMP_SUB) {
            int32_t addr;
            rc = get_addr(addr, true);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps)
                        cout << "JSR :" << hex << setw(8) << setfill('0') << addr << '\n';
                }
                else {
                    if (_state.cs == (_call_stack_size / 8))
                        rc = -1;
                    else if (_state.jumps.count(addr)) {
                        *(int64_t * )(_data + _data_size + _call_stack_size - (++_state.cs * 8)) =_state.pc + rc;
                       _state.pc = addr;
                    }
                    else
                        rc = -2;
                }
            }
        }
        else if (op == at_op_code_RET_SUB) {
            rc = 1;

            if (disassemble) {
                if (!determine_jumps)
                    cout << "RET\n";
            }
            else {
                if (_state.cs == 0)
                    rc = -1;
                else {
                    int64_t val = *(int64_t * )(_data + _data_size + _call_stack_size - (_state.cs-- * 8));
                    int32_t addr = (int32_t) val;
                    if (_state.jumps.count(addr))
                       _state.pc = addr;
                    else
                        rc = -2;
                }
            }
        }
        else if (op == at_op_code_JMP_ADR) {
            int32_t addr;
            rc = get_addr(addr, true);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps)
                        cout << "JMP :" << hex << setw(8) << setfill('0') << addr << '\n';
                }
                else if (_state.jumps.count(addr))
                   _state.pc = addr;
                else
                    rc = -2;
            }
        }
        else if (op == at_op_code_BZR_DAT || op == at_op_code_BNZ_DAT) {
            int8_t off;
            int32_t addr;
            rc = get_addr_off(addr, off);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t) + sizeof(int8_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (op == at_op_code_BZR_DAT)
                            cout << "BZR $";
                        else
                            cout << "BNZ $";

                        cout << hex << setw(8) << setfill('0')
                                << addr << " :" << setw(8) << setfill('0') << (_state.pc + off) << '\n';
                    }
                }
                else {
                    int64_t val = *(int64_t * )(_data + (addr * 8));

                    if ((op == at_op_code_BZR_DAT && val == 0)
                            || (op == at_op_code_BNZ_DAT && val != 0)) {
                        if (_state.jumps.count(_state.pc + off))
                           _state.pc += off;
                        else
                            rc = -2;
                    }
                    else
                       _state.pc += rc;
                }
            }
        }
        else if (op == at_op_code_BGT_DAT || op == at_op_code_BLT_DAT
                || op == at_op_code_BGE_DAT || op == at_op_code_BLE_DAT
                || op == at_op_code_BEQ_DAT || op == at_op_code_BNE_DAT) {
            int8_t off;
            int32_t addr1, addr2;
            rc = get_addrs_off(addr1, addr2, off);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t) + sizeof(int32_t) + sizeof(int8_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (op == at_op_code_BGT_DAT)
                            cout << "BGT $";
                        else if (op == at_op_code_BLT_DAT)
                            cout << "BLT $";
                        else if (op == at_op_code_BGE_DAT)
                            cout << "BGE $";
                        else if (op == at_op_code_BLE_DAT)
                            cout << "BLE $";
                        else if (op == at_op_code_BEQ_DAT)
                            cout << "BEQ $";
                        else
                            cout << "BNE $";

                        cout << hex << setw(8) << setfill('0')
                                << addr1 << " $" << setw(8) << setfill('0')
                                << addr2 << " :" << setw(8) << setfill('0') << (_state.pc + off) << '\n';
                    }
                }
                else {
                    int64_t val1 = *(int64_t * )(_data + (addr1 * 8));
                    int64_t val2 = *(int64_t * )(_data + (addr2 * 8));

                    if ((op == at_op_code_BGT_DAT && val1 > val2)
                            || (op == at_op_code_BLT_DAT && val1 < val2)
                            || (op == at_op_code_BGE_DAT && val1 >= val2)
                            || (op == at_op_code_BLE_DAT && val1 <= val2)
                            || (op == at_op_code_BEQ_DAT && val1 == val2)
                            || (op == at_op_code_BNE_DAT && val1 != val2)) {
                        if (_state.jumps.count(_state.pc + off))
                           _state.pc += off;
                        else
                            rc = -2;
                    }
                    else
                       _state.pc += rc;
                }
            }
        }
        else if (op == at_op_code_SLP_DAT) {
            int32_t addr;
            rc = get_addr(addr, true);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps)
                        cout << "SLP @" << hex << setw(8) << setfill('0') << addr << '\n';
                }
                else {
                    // ToDo: NOTE: The "sleep_until" state value would be set to the current block + $addr.
                   _state.pc += rc;
                }
            }
        }
        else if (op == at_op_code_FIZ_DAT || op == at_op_code_STZ_DAT) {
            int32_t addr;
            rc = get_addr(addr);

            if (rc == 0 || disassemble) {
                if (disassemble) {
                    rc = 1 + sizeof(int32_t);

                    if (!determine_jumps) {
                        if (op == at_op_code_FIZ_DAT)
                            cout << "FIZ @";
                        else
                            cout << "STZ @";

                        cout << hex << setw(8) << setfill('0') << addr << '\n';
                    }
                }
                else {
                    if (*(int64_t * )(_data + (addr * 8)) == 0) {
                        if (op == at_op_code_STZ_DAT)
                            //_state.stopped = true;
                            _state.flags |= flag_stopped;
                        else {
                            _state.pc =_state.pcs;
                            //_state.finished = true;
                            _state.flags |= flag_terminated;
                        }
                    }
                    else {
                        rc = 1 + sizeof(int32_t);
                       _state.pc += rc;
                    }
                }
            }
        }
        else if (op == at_op_code_FIN_IMD || op == at_op_code_STP_IMD) {
            if (disassemble) {
                rc = 1;

                if (!determine_jumps) {
                    if (op == at_op_code_FIN_IMD)
                        cout << "FIN\n";
                    else
                        cout << "STP\n";
                }
            }
            else if (op == at_op_code_STP_IMD)
                //_state.stopped = true;
                _state.flags |= flag_stopped;
            else {
                _state.pc =_state.pcs;
                //_state.finished = true;
                _state.flags |= flag_terminated;
            }
        }
        else if (op == at_op_code_SET_PCS) {
            rc = 1;

            if (disassemble) {
                if (!determine_jumps)
                    cout << "PCS\n";
            }
            else {
               _state.pc += rc;
               _state.pcs =_state.pc;
            }
        }
        else if (op == at_op_code_EXT_FUN) {
            int16_t fun;
            rc = get_fun(fun);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int16_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (fun < 0x100)
                            cout << "FUN " << dec << fun << "\n";
                        else
                            cout << "FUN 0x" << hex << setw(4) << setfill('0') << fun << "\n";
                    }
                }
                else {
                    _state.pc += rc;
                    func(fun);
                }
            }
        }
        else if (op == at_op_code_EXT_FUN_DAT) {
            int16_t fun;
            int32_t addr;
            rc = get_fun_addr(fun, addr);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int16_t) + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (fun < 0x100)
                            cout << "FUN " << dec << fun << " $" << hex << setw(8) << setfill('0') << addr << "\n";
                        else
                            cout << "FUN 0x" << hex << setw(4) << setfill('0') << fun
                                    << " $" << hex << setw(8) << setfill('0') << addr << "\n";
                    }
                }
                else {
                    _state.pc += rc;
                    int64_t val = *(int64_t * )(_data + (addr * 8));

                    func1(fun, val);
                }
            }
        }
        else if (op == at_op_code_EXT_FUN_DAT_2) {
            int16_t fun;
            int32_t addr1, addr2;
            rc = get_fun_addrs(fun, addr1, addr2);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int16_t) + sizeof(int32_t) + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (fun < 0x100)
                            cout << "FUN " << dec << fun << " $" << hex << setw(8)
                                    << setfill('0') << addr1 << " $" << setw(8) << setfill('0') << addr2 << "\n";
                        else
                            cout << "FUN 0x" << hex << setw(4) << setfill('0') << fun << " $" << hex << setw(8)
                                    << setfill('0') << addr1 << " $" << setw(8) << setfill('0') << addr2 << "\n";
                    }
                }
                else {
                   _state.pc += rc;
                    int64_t val1 = *(int64_t * )(_data + (addr1 * 8));
                    int64_t val2 = *(int64_t * )(_data + (addr2 * 8));

                    func2(fun, val1, val2);
                }
            }
        }
        else if (op == at_op_code_EXT_FUN_RET) {
            int16_t fun;
            int32_t addr;
            rc = get_fun_addr(fun, addr);

            if (rc == 0 || disassemble) {
                rc = 1 + sizeof(int16_t) + sizeof(int32_t);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (fun < 0x100)
                            cout << "FUN @" << hex << setw(8) << setfill('0') << addr << ' ' << dec << fun << '\n';
                        else
                            cout << "FUN @" << hex << setw(8) << setfill('0') << addr
                                    << " 0x" << hex << setw(4) << setfill('0') << fun << '\n';
                    }
                }
                else {
                   _state.pc += rc;
                    *(int64_t * )(_data + (addr * 8)) = func(fun);
                }
            }
        }
        else if (op == at_op_code_EXT_FUN_RET_DAT || op == at_op_code_EXT_FUN_RET_DAT_2) {
            int16_t fun;
            int32_t addr1, addr2;
            rc = get_fun_addrs(fun, addr1, addr2);

            int32_t size = sizeof(int16_t) + sizeof(int32_t) + sizeof(int32_t); // ToDo: Verify these sizes match

            int32_t addr3;
            if ((rc == 0 || disassemble) && op == at_op_code_EXT_FUN_RET_DAT_2)
                //rc = get_addr(_code + size, _code_size, _data_size, state, addr3);
                rc = get_addr(addr3); // ToDo: Verify removal of _code+size does not break this

            if (rc == 0 || disassemble) {
                rc = 1 + size + (op == at_op_code_EXT_FUN_RET_DAT_2 ? sizeof(int32_t) : 0);

                if (disassemble) {
                    if (!determine_jumps) {
                        if (fun < 0x100)
                            cout << "FUN @" << hex << setw(8) << setfill('0') << addr1
                                    << ' ' << dec << fun << " $" << setw(8) << setfill('0') << addr2;
                        else
                            cout << "FUN @" << hex << setw(8) << setfill('0') << addr1
                                    << " 0x" << hex << setw(4) << setfill('0') << fun << " $" << setw(8) << setfill('0') << addr2;

                        if (op == at_op_code_EXT_FUN_RET_DAT_2)
                            cout << " $" << setw(8) << setfill('0') << addr3;

                        cout << "\n";
                    }
                }
                else {
                   _state.pc += rc;
                    int64_t val = *(int64_t * )(_data + (addr2 * 8));

                    if (op != at_op_code_EXT_FUN_RET_DAT_2)
                        *(int64_t * )(_data + (addr1 * 8)) = func1(fun, val);
                    else {
                        int64_t val2 = *(int64_t * )(_data + (addr3 * 8));
                        *(int64_t * )(_data + (addr1 * 8)) = func2(fun, val, val2);
                    }
                }
            }
        }
        else {
            if (!disassemble)
                rc = -2;
        }

        if (rc == -1 && disassemble && !determine_jumps)
            cout << "\n(overflow)\n";

        if (rc == -2 && disassemble && !determine_jumps)
            cout << "\n(invalid op)\n";

        if (rc >= 0)
            ++_state.steps;

        return rc;
    }

    void dump_state()
    {
        cout << "pc: " << hex << setw(8) << setfill('0') <<_state.pc << '\n';

        cout << "cs: " << dec << _state.cs << '\n';
        cout << "us: " << dec << _state.us << '\n';

        cout << "pcs: " << hex << setw(8) << setfill('0') <<_state.pcs << '\n';

        cout << "steps: " << dec << _state.steps << '\n';
    }

    void dump_bytes(int8_t *p_bytes, int num)
    {
        for (int i = 0; i < num; i += 16) {
            cout << hex << setw(8) << setfill('0') << i << ' ';

            for (int j = 0; j < 16; j++) {
                int val = (unsigned char) p_bytes[i + j];

                cout << ' ' << hex << setw(2) << setfill('0') << val;
            }

            cout << '\n';
        }
    }

    void list_code(bool determine_jumps = false)
    {
        int32_t opc =_state.pc;
        int32_t osteps = _state.steps;

        _state.pc = 0;
        _state.opc = opc;

        while (true) {
            int rc = process_op(true, determine_jumps);

            if (rc <= 0)
                break;

            _state.pc += rc;
        }

        _state.steps = osteps;
       _state.pc = opc;
    }

    void reset_machine()
    {
        _state.reset();
        list_code(true);

        memset(_data, 0, _data_size + _call_stack_size + _user_stack_size);

        g_first_call = true;

        for (map<int32_t, function_data>::iterator i = g_function_data.begin(); i != g_function_data.end(); ++i)
            i->second.offset = 0;
    }

    bool isFinished(){
        return _state.flags |= flag_terminated;
    }
    bool isStopped(){
        return _state.flags |= flag_stopped;
    }
    bool isPaused(){
        return _state.flags |= flag_paused;
    }
};


// The following is for debug/testing

int main()
{
    auto_ptr <int8_t> a_code(new int8_t[g_code_pages * c_code_page_bytes]);
    auto_ptr <int8_t> a_data(new int8_t[g_data_pages * c_data_page_bytes
            + g_call_stack_pages * c_call_stack_page_bytes + g_user_stack_pages * c_user_stack_page_bytes]);

    memset(a_code.get(), 0, g_code_pages * c_code_page_bytes);
    memset(a_data.get(), 0, g_data_pages * c_data_page_bytes
            + g_call_stack_pages * c_call_stack_page_bytes + g_user_stack_pages * c_user_stack_page_bytes);

    machine_state state;
    set <int32_t> break_points;

    AutomatedTransaction Test_AT (a_code.get(), g_code_pages * c_code_page_bytes,
                                  a_data.get(), g_data_pages * c_data_page_bytes,
                                  g_call_stack_pages * c_call_stack_page_bytes,
                                  g_user_stack_pages * c_user_stack_page_bytes);

    string cmd, next;
    while (cout << "\n> ", getline(cin, next)) {
        if (next.empty())
            continue;

        string::size_type pos = next.find(' ');
        if (pos == string::npos) {
            cmd = next;
            next.erase();
        }
        else {
            cmd = next.substr(0, pos);
            next.erase(0, pos + 1);
        }

        string arg_1, arg_2, arg_3;

        pos = next.find(' ');
        arg_1 = next.substr(0, pos);
        if (pos != string::npos) {
            arg_2 = next.substr(pos + 1);

            pos = arg_2.find(' ');
            if (pos != string::npos) {
                arg_3 = arg_2.substr(pos + 1);
                arg_2.erase(pos);
            }
        }

        if (cmd == "?" || cmd == "help") {
            cout << "commands:\n";
            cout << "---------\n";
            cout << "code <hex byte values> [<[0x]offset>]\n";
            cout << "data <hex byte values> [<[0x]offset>]\n";
            cout << "run\n";
            cout << "cont\n";
            cout << "dump {code|data|stacks}\n";
            cout << "list\n";
            cout << "load <file>\n";
            cout << "save <file>\n";
            cout << "size [{code|data|call|user} [<pages>]]\n";
            cout << "step [<num_steps>]\n";
            cout << "break <[0x]value>\n";
            cout << "reset\n";
            cout << "state\n";
            cout << "balance [<amount>]\n";
            cout << "function <[+]#> [<[0x]value1[,[0x]value2[,...]]>] [loop]\n";
            cout << "functions\n";
            cout << "help\n";
            cout << "exit" << endl;
        }
        else if ((cmd == "code" || cmd == "data") && arg_1.size() && (arg_1.size() % 2 == 0)) {
            int32_t offset = 0;
            if (!arg_2.empty()) {
                if (arg_2.size() > 2 && arg_2.find("0x") == 0) {
                    istringstream isstr(arg_2.substr(2));
                    isstr >> hex >> offset;
                }
                else
                    offset = atoi(arg_2.c_str());
            }

            int8_t *p_c = cmd == "code" ? a_code.get() : a_data.get() + offset;

            if (arg_2.empty()) {
                if (cmd == "code")
                    memset(p_c, 0, g_code_pages * c_code_page_bytes);
                else
                    memset(p_c, 0, g_data_pages * c_data_page_bytes);
            }

            for (size_t i = 0; i < arg_1.size(); i += 2) {
                unsigned int value;
                istringstream isstr(arg_1.substr(i, 2));
                isstr >> hex >> value;

                *(p_c + (i / 2)) = (int8_t) value;
            }

            if (cmd == "code")
                Test_AT.reset_machine();
        }
        else if (cmd == "run" || cmd == "cont") {
            if (cmd == "run")
                Test_AT.reset_machine();

            while (true) {
                if (!check_has_balance())
                    break;

                int rc = Test_AT.process_op(false, false);

                if (!check_has_balance())
                    break;

                --g_balance;

                if (rc >= 0) {
                    if (_state.stopped) {
                        cout << "(stopped)\n";
                        cout << "total steps: " << dec << _state.steps << '\n';

                        _state.stopped = false;
                        break;
                    }
                    else if (_state.finished) {
                        cout << "(finished)\n";
                        cout << "total steps: " << dec << _state.steps << '\n';

                        break;
                    }

                    if (break_points.count(_state.pc)) {
                        cout << "(break point)\n";
                        break;
                    }
                }
                else {
                    if (rc == -1)
                        cout << "error: overflow\n";
                    else if (rc == -2)
                        cout << "error: invalid code\n";
                    else
                        cout << "unexpected error\n";

                    break;
                }
            }
        }
        else if (cmd == "dump" && (next == "code" || next == "data" || next == "stacks")) {
            if (next == "code")
                Test_AT.dump_bytes(a_code.get(), g_code_pages * c_code_page_bytes);
            else if (next == "data")
                Test_AT.dump_bytes(a_data.get(), g_data_pages * c_data_page_bytes);
            else
                Test_AT.dump_bytes(a_data.get() + g_data_pages * c_data_page_bytes,
                        g_call_stack_pages * c_call_stack_page_bytes + g_user_stack_pages * c_user_stack_page_bytes);
        }
        else if (cmd == "list")
            Test_AT.list_code();
        else if (cmd == "load" && !arg_1.empty()) {
            ifstream inpf(arg_1.c_str(), ios::in | ios::binary);

            if (!inpf)
                cout << "error: unable to open '" << arg_1 << "' for input" << endl;
            else {
                // ToDo: Load is borked due to new class structure
                inpf.read((char *) &g_val, sizeof(g_val));
                inpf.read((char *) &g_val1, sizeof(g_val1));
                inpf.read((char *) &g_balance, sizeof(g_balance));
                inpf.read((char *) &g_first_call, sizeof(g_first_call));
                inpf.read((char *) &g_increment_func, sizeof(g_increment_func));

                inpf.read((char *) &_state.pc, sizeof(_state.pc));
                inpf.read((char *) &_state.cs, sizeof(_state.cs));
                inpf.read((char *) &_state.us, sizeof(_state.us));
                inpf.read((char *) &_state.pcs, sizeof(_state.pcs));
                inpf.read((char *) &_state.steps, sizeof(_state.steps));

                inpf.read((char *) &g_code_pages, sizeof(g_code_pages));
                a_code.reset(new int8_t[g_code_pages * c_code_page_bytes]);

                inpf.read((char *) a_code.get(), g_code_pages * c_code_page_bytes);

                inpf.read((char *) &g_data_pages, sizeof(g_data_pages));
                inpf.read((char *) &g_call_stack_pages, sizeof(g_call_stack_pages));
                inpf.read((char *) &g_user_stack_pages, sizeof(g_user_stack_pages));

                a_data.reset(new int8_t[g_data_pages * c_data_page_bytes
                        + g_call_stack_pages * c_call_stack_page_bytes + g_user_stack_pages * c_user_stack_page_bytes]);

                inpf.read((char *) a_data.get(), g_data_pages * c_data_page_bytes
                        + g_call_stack_pages * c_call_stack_page_bytes + g_user_stack_pages * c_user_stack_page_bytes);

                g_function_data.clear();

                size_t size;
                inpf.read((char *) &size, sizeof(size_t));

                for (size_t i = 0; i < size; i++) {
                    int32_t func;
                    inpf.read((char *) &func, sizeof(int32_t));

                    bool loop;
                    inpf.read((char *) &loop, sizeof(bool));

                    size_t offset;
                    inpf.read((char *) &offset, sizeof(size_t));

                    g_function_data[func].loop = loop;
                    g_function_data[func].offset = offset;

                    size_t _data_size;
                    inpf.read((char *) &_data_size, sizeof(size_t));

                    for (size_t j = 0; j < _data_size; j++) {
                        int64_t next;
                        inpf.read((char *) &next, sizeof(int64_t));

                        g_function_data[func].data.push_back(next);
                    }
                }

                inpf.close();

                list_code(state,
                        a_code.get(), g_code_pages * c_code_page_bytes,
                        a_data.get(), g_data_pages * c_data_page_bytes,
                        g_call_stack_pages * c_call_stack_page_bytes, g_user_stack_pages * c_user_stack_page_bytes, true);
            }
        }
        else if (cmd == "save" && !arg_1.empty()) {
            // ToDo: Save is also borked with Load.
            ofstream outf(arg_1.c_str(), ios::out | ios::binary);

            if (!outf)
                cout << "error: unable to open '" << arg_1 << "' for output" << endl;
            else {
                outf.write((const char *) &g_val, sizeof(g_val));
                outf.write((const char *) &g_val1, sizeof(g_val1));
                outf.write((const char *) &g_balance, sizeof(g_balance));
                outf.write((const char *) &g_first_call, sizeof(g_first_call));
                outf.write((const char *) &g_increment_func, sizeof(g_increment_func));

                outf.write((const char *) &_state.pc, sizeof(_state.pc));
                outf.write((const char *) &_state.cs, sizeof(_state.cs));
                outf.write((const char *) &_state.us, sizeof(_state.us));
                outf.write((const char *) &_state.pcs, sizeof(_state.pcs));
                outf.write((const char *) &_state.steps, sizeof(_state.steps));

                outf.write((const char *) &g_code_pages, sizeof(g_code_pages));
                outf.write((const char *) a_code.get(), g_code_pages * c_code_page_bytes);

                outf.write((const char *) &g_data_pages, sizeof(g_data_pages));
                outf.write((const char *) &g_call_stack_pages, sizeof(g_call_stack_pages));
                outf.write((const char *) &g_user_stack_pages, sizeof(g_user_stack_pages));

                outf.write((const char *) a_data.get(), g_data_pages * c_data_page_bytes
                        + g_call_stack_pages * c_call_stack_page_bytes + g_user_stack_pages * c_user_stack_page_bytes);

                size_t size = g_function_data.size();
                outf.write((const char *) &size, sizeof(size_t));

                for (map<int32_t, function_data>::iterator i = g_function_data.begin(); i != g_function_data.end(); ++i) {
                    outf.write((const char *) &i->first, sizeof(int32_t));
                    outf.write((const char *) &i->second.loop, sizeof(bool));
                    outf.write((const char *) &i->second.offset, sizeof(size_t));

                    size_t _data_size = i->second.data.size();
                    outf.write((const char *) &_data_size, sizeof(size_t));

                    for (size_t j = 0; j < _data_size; j++)
                        outf.write((const char *) &i->second.data[j], sizeof(int64_t));
                }

                outf.close();
            }
        }
        else if (cmd == "size") {
            if (arg_1.empty() || arg_2.empty()) {
                if (arg_1.empty() || arg_1 == "code")
                    cout << "code (" << g_code_pages << " * "
                            << c_code_page_bytes << ") = " << (g_code_pages * c_code_page_bytes) << " bytes\n";

                if (arg_1.empty() || arg_1 == "data")
                    cout << "data (" << g_data_pages << " * "
                            << c_data_page_bytes << ") = " << (g_data_pages * c_data_page_bytes) << " bytes\n";

                if (arg_1.empty() || arg_1 == "call")
                    cout << "call (" << g_call_stack_pages << " * "
                            << c_call_stack_page_bytes << ") = " << (g_call_stack_pages * c_call_stack_page_bytes) << " bytes\n";

                if (arg_1.empty() || arg_1 == "user")
                    cout << "user (" << g_user_stack_pages << " * "
                            << c_user_stack_page_bytes << ") = " << (g_user_stack_pages * c_user_stack_page_bytes) << " bytes\n";
            }
            else {
                int pages = atoi(arg_2.c_str());
                if (pages <= 0)
                    throw runtime_error("invalid # pages value: " + arg_2);

                if (arg_1 == "code") {
                    g_code_pages = pages;

                    a_code.reset(new int8_t[g_code_pages * c_code_page_bytes]);

                    memset(a_code.get(), 0, g_code_pages * c_code_page_bytes);
                }
                else {
                    if (arg_1 == "data")
                        g_data_pages = pages;
                    else if (arg_1 == "call")
                        g_call_stack_pages = pages;
                    else if (arg_1 == "user")
                        g_user_stack_pages = pages;
                    else
                        throw runtime_error("unexpected arg_1 '" + arg_1 + "' for 'size' command");

                    a_data.reset(new int8_t[g_data_pages * c_data_page_bytes
                            + g_call_stack_pages * c_call_stack_page_bytes + g_user_stack_pages * c_user_stack_page_bytes]);

                    memset(a_data.get(), 0, g_data_pages * c_data_page_bytes
                            + g_call_stack_pages * c_call_stack_page_bytes + g_user_stack_pages * c_user_stack_page_bytes);
                }
            }
        }
        else if (cmd == "step") {
            int32_t steps = 0;
            int32_t num_steps = 0;

            if (!arg_1.empty())
                num_steps = atoi(arg_1.c_str());

            if (_state.finished)
                Test_AT.reset_machine();

            while (true) {
                if (!check_has_balance())
                    break;

                int rc = Test_AT.process_op(false, false);

                if (!check_has_balance())
                    break;

                --g_balance;

                if (rc >= 0) {
                    ++steps;
                    if (_state.stopped || _state.finished || num_steps && steps >= num_steps) {
                        if (_state.stopped)
                            cout << "(stopped)\n";
                        else if (_state.finished)
                            cout << "(finished)\n";

                        break;
                    }
                    else if (!num_steps)
                        break;
                }
                else {
                    if (rc == -1)
                        cout << "error: overflow\n";
                    else if (rc == -2)
                        cout << "error: invalid code\n";
                    else
                        cout << "unexpected error\n";

                    break;
                }
            }
        }
        else if (cmd == "break") {
            if (arg_1.empty()) {
                for (set<int32_t>::iterator i = break_points.begin(); i != break_points.end(); ++i)
                    cout << *i << '\n';
            }
            else {
                int32_t bp;
                if (arg_1.size() > 2 && arg_1.find("0x") == 0) {
                    istringstream isstr(arg_1.substr(2));
                    isstr >> hex >> bp;
                }
                else
                    bp = atoi(arg_1.c_str());

                if (break_points.count(bp))
                    break_points.erase(bp);
                else
                    break_points.insert(bp);
            }
        }
        else if (cmd == "reset")
            Test_AT.reset_machine();
        else if (cmd == "state")
            Test_AT.dump_state();
        else if (cmd == "balance") {
            if (arg_1.empty())
                cout << dec << g_balance << '\n';
            else
                g_balance = atoi(arg_1.c_str());
        }
        else if (cmd == "function" && !arg_1.empty()) {
            bool is_increment_func = false;
            if (arg_1[0] == '+') {
                is_increment_func = true;
                arg_1.erase(0, 1);
            }

            int32_t func = atoi(arg_1.c_str());

            if (is_increment_func)
                g_increment_func = func;

            if ((!is_increment_func || !arg_2.empty()) && g_function_data.count(func))
                g_function_data.erase(func);

            if (!arg_2.empty()) {
                while (true) {
                    string::size_type pos = arg_2.find(',');

                    string next(arg_2.substr(0, pos));

                    int64_t val;
                    if (next.size() > 2 && next.find("0x") == 0) {
                        istringstream isstr(next);
                        isstr >> hex >> val;
                    }
                    else
                        val = atoi(next.c_str());

                    g_function_data[func].data.push_back(val);

                    if (pos == string::npos)
                        break;

                    arg_2.erase(0, pos + 1);
                }

                if (arg_3 == "true")
                    g_function_data[func].loop = true;
            }
        }
        else if (cmd == "functions") {
            for (map<int32_t, function_data>::iterator i = g_function_data.begin(); i != g_function_data.end(); ++i) {
                if (i->first == g_increment_func)
                    cout << '+';
                else
                    cout << ' ';

                cout << dec << setw(3) << setfill('0') << i->first;

                for (size_t j = 0; j < i->second.data.size(); j++)
                    cout << (j == 0 ? ' ' : ',') << "0x" << hex << setw(16) << i->second.data[j];

                if (i->second.loop)
                    cout << " true\n";
                else
                    cout << " false\n";
            }
        }
        else if (cmd == "quit" || cmd == "exit")
            break;
        else
            cout << "invalid command: " << cmd << endl;
    }
}