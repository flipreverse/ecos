//==========================================================================
//
//      io.c
//
//      RedBoot I/O support
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                        
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas,hmt,jlarmour
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include "redboot.h"

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
// GDB interface functions
extern void ungetDebugChar(char c);
#endif

static void
do_channel(int argc, char *argv[]);

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
RedBoot_cmd("channel", 
            "Display/switch console channel", 
            "[-1|<channel number>]",
            do_channel
    );
#else
RedBoot_cmd("channel", 
            "Display/switch console channel", 
            "[<channel number>]",
            do_channel
    );
#endif

static void
do_channel(int argc, char *argv[])
{
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    if (argc == 2) { 
#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
        if (strcmp( argv[1], "-1") == 0) {
            console_selected = false;
            console_echo = true;
        } else 
#endif
        {
            unsigned long chan;
            if ( !parse_num( argv[1], &chan, NULL, NULL) ) {
                diag_printf("** Error: invalid channel '%s'\n", argv[1]);
            } else {
                if (chan < CYGNUM_HAL_VIRTUAL_VECTOR_NUM_CHANNELS) {
                    CYGACC_CALL_IF_SET_CONSOLE_COMM(chan);
                    CYGACC_CALL_IF_SET_DEBUG_COMM(chan);
                    if (chan != cur)
                        console_echo = true;
                }
                else {
                    diag_printf("**Error: bad channel number '%s'\n", argv[1]);
                }
            }
        }
    }
    /* else display */ 
    else {
        diag_printf("Current console channel id: ");
#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
        if (!console_selected)
            diag_printf("-1\n");
        else
#endif
            diag_printf("%d\n", cur);
    }
}

void 
mon_write_char(char c)
{
    hal_virtual_comm_table_t *__chan;

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    if (!console_selected) {
        int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
        int i;
        // Send output to all channels
        for (i = 0;  i < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS;  i++) {
            CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
            __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
            CYGACC_COMM_IF_PUTC(*__chan, c);
        }
        CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
    } else 
#endif
    {
        __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
        if (__chan)
            CYGACC_COMM_IF_PUTC(*__chan, c);
        else {
            __chan = CYGACC_CALL_IF_DEBUG_PROCS();
            CYGACC_COMM_IF_PUTC(*__chan, c);
        }
    }
}

static void 
mon_read_char(char *c)
{
    hal_virtual_comm_table_t* __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    
    if (__chan)
        *c = CYGACC_COMM_IF_GETC(*__chan);
    else {
        __chan = CYGACC_CALL_IF_DEBUG_PROCS();
        *c = CYGACC_COMM_IF_GETC(*__chan);
    }
}

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
static int _mon_timeout;
#endif

static bool
mon_read_char_with_timeout(char *c)
{
    bool res = false;
    hal_virtual_comm_table_t *__chan;

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    if (!console_selected) {
        int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
        int i, tot;
        // Try input from all channels
        tot = 0;
        while (tot < _mon_timeout) {
            for (i = 0;  i < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS;  i++, tot++) {
                CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
                __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
                res = CYGACC_COMM_IF_GETC_TIMEOUT(*__chan, c);
                if (res) {
                    // Input available on this channel, make it be the console
                    if (*c != '\0') {
                        // Don't chose this unless real data have arrived
                        console_selected = true;
                        CYGACC_CALL_IF_SET_DEBUG_COMM(i);
                        return res;
                    }
                }
            }
        }
        CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);        
    } else 
#endif
    {
        __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
        if (__chan)
            res = CYGACC_COMM_IF_GETC_TIMEOUT(*__chan, c);
        else {
            __chan = CYGACC_CALL_IF_DEBUG_PROCS();
            res = CYGACC_COMM_IF_GETC_TIMEOUT(*__chan, c);
        }
    }
    return res;
}

static void
mon_set_read_char_timeout(int ms)
{
    hal_virtual_comm_table_t *__chan;

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    if (!console_selected) {
        int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
        int i;
        // Set timeout to minimum on each channel; total amounts to desired value
        _mon_timeout = ms;
        ms = 1;
        for (i = 0;  i < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS;  i++) {
            CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
            if ((__chan = CYGACC_CALL_IF_CONSOLE_PROCS()) != 0) {
                CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_SET_TIMEOUT, ms);
            }
        }
        CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);        
    } else 
#endif
    {
        if ((__chan = CYGACC_CALL_IF_CONSOLE_PROCS()) != 0) {
            CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_SET_TIMEOUT, ms);
        }
        if ((__chan = CYGACC_CALL_IF_DEBUG_PROCS()) != 0) {
            CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_SET_TIMEOUT, ms);
        }
    }
}

#ifdef CYGFUN_REDBOOT_BOOT_SCRIPT
#define __STRINGIFY(x) #x
#define _STRINGIFY(x) __STRINGIFY(x)
#define _STARTUP_STR _STRINGIFY(CYG_HAL_STARTUP) "}"

//
// Read a character from script.
// Return true if script character found, false if not.
//
static int
getc_script(char *cp)
{
    static bool newline = true;
    bool skip;

    while (script && *script) {
	if (newline && *script == '{') {
	    skip = false;
	    ++script;

	    // skip if it isn't for this startup type
	    if (strncmp(script, _STARTUP_STR, strlen(_STARTUP_STR)))
		skip = true;

	    // skip past "{...}"
	    while (*script && *script++ != '}')
		;

	    // skip script line if neccessary
	    if (skip) {
		while (*script && *script++ != '\n')
		    ;
	    } else
		newline = false;

	} else {
	    *cp = *script++;
	    if (*cp == '\n')
		newline = true;
	    return true;
	}
    }
    return false;
}
#endif

//
// Read a line of input from the user
// Return:
//    n: 'n' valid characters received
//    0: '$' (GDB lead-in)
//   -1: No input before timeout
//   -2: ^C typed
//
int
_rb_gets(char *buf, int buflen, int timeout)
{
    char *ip = buf;   // Insertion point
    char *eol = buf;  // End of line
    char c;
    bool res = false;
    static char last_ch = '\0';
#if CYGNUM_REDBOOT_CMD_LINE_EDITING != 0
//
// Command line history support
//    ^P - Select previous line from history
//    ^N - Select next line from history
//    ^A - Move insertion [cursor] to start of line
//    ^E - Move cursor to end of line
//    ^B - Move cursor back [previous character]
//    ^F - Move cursor forward [next character]
//
#define _CL_NUM_LINES CYGNUM_REDBOOT_CMD_LINE_EDITING       // Number of lines to keep
    static char _cl_lines[_CL_NUM_LINES][CYGPKG_REDBOOT_MAX_CMD_LINE];
    static int  _cl_index = -1;      // Last known command line
    static int  _cl_max_index = -1;  // Last command in buffers
    int _index = _cl_index;  // Last saved line
    char *xp;
#endif

    while (true) {
#ifdef CYGFUN_REDBOOT_BOOT_SCRIPT
        if (getc_script(&c))
            do_idle(false);
        else
#endif
        if ((timeout > 0) && (eol == buf)) {
#define MIN_TIMEOUT 50
            mon_set_read_char_timeout(timeout > MIN_TIMEOUT ? MIN_TIMEOUT : timeout);
            while (timeout > 0) {
                res = mon_read_char_with_timeout(&c);
                if (res) {
                    // Got a character
                    do_idle(false);
                    break;
                }
                timeout -= MIN_TIMEOUT;
            }
            if (res == false) {
                do_idle(true);
                return _GETS_TIMEOUT;  // Input timed out
            }
        } else {
            mon_read_char(&c);
        }
        *eol = '\0';
        switch (c) {
#define CTRL(c) ((c)&0x1F)
#if CYGNUM_REDBOOT_CMD_LINE_EDITING != 0
        case CTRL('P'):
            // Fetch the previous line into the buffer
            if (_index >= 0) {
                // Erase the previous line [crude]
                while (ip != buf) {
                    mon_write_char('\b');
                    mon_write_char(' ');
                    mon_write_char('\b');
                    ip--;
                }
                strcpy(buf, _cl_lines[_index]);
                while (*ip) {
                    mon_write_char(*ip++);
                }
                eol = ip;
                // Move to previous line
                _index--;
                if (_index < 0) {
                    _index = _cl_max_index;
                }
            } else {
                mon_write_char(0x07);  // Audible bell on most devices
            }
            break;
        case CTRL('N'):
            // Fetch the next line into the buffer
            if (_index >= 0) {
                if (++_index > _cl_max_index) _index = 0;
                // Erase the previous line [crude]
                while (ip != buf) {
                    mon_write_char('\b');
                    mon_write_char(' ');
                    mon_write_char('\b');
                    ip--;
                }
                strcpy(buf, _cl_lines[_index]);
                while (*ip) {
                    mon_write_char(*ip++);
                }
                eol = ip;
            } else {
                mon_write_char(0x07);  // Audible bell on most devices
            }
            break;
        case CTRL('B'): 
            // Move insertion point backwards
            if (ip != buf) {
                mon_write_char('\b');
                ip--;
            }
            break;
        case CTRL('F'):
            // Move insertion point forwards
            if (ip != eol) {
                mon_write_char(*ip++);
            }
            break;
        case CTRL('E'):
            // Move insertion point to end of line
            while (ip != eol) {
                mon_write_char(*ip++);
            }
            break;
        case CTRL('A'):
            // Move insertion point to beginning of line
            if (ip != buf) {
                xp = eol;
                while (xp-- != buf) {
                    mon_write_char('\b');
                }
            }
            ip = buf;
            break;
        case CTRL('K'):
            // Kill to the end of line
            if (ip != eol) {
                xp = ip;
                while (xp++ != eol) {
                    mon_write_char(' ');
                }
                while (--xp != ip) {
                    mon_write_char('\b');
                }
                eol = ip;
            }
            break;
#endif // CYGNUM_REDBOOT_CMD_LINE_EDITING
        case CTRL('C'): // ^C
            if (ip == buf) {
                diag_printf("^C\n");
                return _GETS_CTRLC;
            }
            *ip++ = c;
            break;
        case '\n':
        case '\r':
            // If previous character was the "other" end-of-line, ignore this one
            if (((c == '\n') && (last_ch == '\r')) ||
                ((c == '\r') && (last_ch == '\n'))) {
                c = '\0';
                break;
            }
            // End of line
	    if (console_echo) {
                mon_write_char('\r');
                mon_write_char('\n');
	    }
            last_ch = c;
#if CYGNUM_REDBOOT_CMD_LINE_EDITING != 0
            // Save current line
            if (++_cl_index == _CL_NUM_LINES) _cl_index = 0;
            if (_cl_index > _cl_max_index) _cl_max_index = _cl_index;
            strcpy(_cl_lines[_cl_index], buf);
#endif
            return _GETS_OK;
        case '\b':
        case 0x7F:  // DEL
            if (ip != buf) {
#if CYGNUM_REDBOOT_CMD_LINE_EDITING != 0
                if (ip != eol) {
                    ip--;
                    mon_write_char('\b');
                    xp = ip;
                    while (xp != (eol-1)) {
                        *xp = *(xp+1);
                        mon_write_char(*xp++);
                    }
                    mon_write_char(' ');  // Erases last character
                    mon_write_char('\b');
                    while (xp-- != ip) {
                        mon_write_char('\b');
                    }
                } else {
                    if (console_echo) {
                        mon_write_char('\b');
                        mon_write_char(' ');
                        mon_write_char('\b');
                    }
                    ip--;
                }
                eol--;
#else
                if (console_echo) {
                    mon_write_char('\b');
                    mon_write_char(' ');
                    mon_write_char('\b');
                }
                ip--;
                eol--;
#endif
            }
            break;
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        case '+': // fall through
        case '$':
            if (ip == buf || last_ch != '\\')
            {
                // Give up and try GDB protocol
                ungetDebugChar(c);  // Push back character so stubs will see it
                return _GETS_GDB;
            }
            if (last_ch == '\\') {
#if CYGNUM_REDBOOT_CMD_LINE_EDITING != 0
                if (ip == eol) {
                    // Just save \$ as $
                    eol = --ip;
                } else {
                    mon_write_char('\b');
                    *--ip = c;
                    mon_write_char(c);
                    break;
                }
#else
                ip--;  // Save \$ as $
#endif
            }
            // else fall through
#endif
        default:
#if CYGNUM_REDBOOT_CMD_LINE_EDITING != 0
            // If the insertion point is not at the end of line, make space for it
            if (ip != eol) {
                xp = eol;
                *++eol = '\0';
                while (xp != ip) {
                    *xp = *(xp-1);
                    xp--;
                }
            }
#endif
            if (console_echo) {
                mon_write_char(c);
            }
            if (ip == eol) {
                // Advance both pointers
                *ip++ = c;
                eol = ip;
#if CYGNUM_REDBOOT_CMD_LINE_EDITING != 0
            } else {
                // Just insert the character
                *ip++ = c;
                xp = ip;
                while (xp != eol) {
                    mon_write_char(*xp++);
                }
                while (xp-- != ip) {
                    mon_write_char('\b');
                }
#endif
            }
        }
        last_ch = c;
        if (ip == buf + buflen) // Buffer full
            return buflen;
    }
}

bool
verify_action(char *fmt, ...)
{
    va_list ap;
    char ans[8];
#ifdef CYGFUN_REDBOOT_BOOT_SCRIPT
    // Don't ask if we're executing a script
    if (script && *script)
        return 1;
#endif

    va_start(ap, fmt);
    diag_vprintf(fmt, ap);
    diag_printf(" - are you sure (y/n)? ");
    _rb_gets(ans, sizeof(ans), 0);
    return ((ans[0] == 'y') || (ans[0] == 'Y'));
}
