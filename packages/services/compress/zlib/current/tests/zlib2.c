//=================================================================
//
//        zlib2.c
//
//        zlib decompression test 2
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
// Author(s):     jskov
// Contributors:  jskov
// Date:          2001-03-09
// Description:   Tests zlib compression of gz compressed image.
//####DESCRIPTIONEND####


#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_KERNEL_API_C

#include <cyg/compress/zlib.h>
#include <stdlib.h>

#define NTHREADS 1
#define STACKSIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL > 8192 ? CYGNUM_HAL_STACK_SIZE_TYPICAL : 8192)

static cyg_handle_t thread[NTHREADS];

static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];


unsigned char gzip_test[] = {
0x1f, 0x8b, 0x08, 0x08, 0x23, 0xce, 0xa8, 0x3a, 0x00, 0x03, 0x61, 0x00, 0x8d, 0x56, 0xdb, 0x6e,
0xe3, 0x36, 0x10, 0x7d, 0xe7, 0x57, 0x10, 0x01, 0x8a, 0x66, 0x01, 0xd7, 0xdd, 0xb4, 0xdd, 0xed,
0xe5, 0x2d, 0x9b, 0x64, 0xd1, 0x00, 0x7b, 0x29, 0x92, 0x45, 0x17, 0x7d, 0xa4, 0xa9, 0x91, 0x4d,
0x44, 0x22, 0x55, 0x92, 0xb2, 0x93, 0xbf, 0xdf, 0x33, 0x43, 0x49, 0x96, 0x9c, 0x06, 0xa8, 0x9f,
0x4c, 0x91, 0x9c, 0x39, 0x33, 0x73, 0xe6, 0x0c, 0xef, 0xa8, 0xd2, 0x7f, 0x9a, 0xac, 0xe9, 0x2a,
0x24, 0xfd, 0x57, 0xbf, 0x69, 0x9c, 0xd5, 0x1f, 0x9c, 0x25, 0x9f, 0x48, 0xef, 0x2f, 0xd6, 0x17,
0x0a, 0xbf, 0x8b, 0xb5, 0xbe, 0xbe, 0x79, 0x7f, 0xfb, 0xe9, 0xf6, 0xcb, 0xed, 0xe7, 0x4f, 0xf7,
0xbc, 0xc6, 0x97, 0xb3, 0xab, 0xe0, 0x73, 0x74, 0x9b, 0x3e, 0x87, 0x78, 0xa6, 0x5b, 0x32, 0x3e,
0x69, 0x32, 0x76, 0xa7, 0xc9, 0x67, 0x97, 0x9f, 0x74, 0xde, 0xc1, 0xaa, 0x8d, 0x64, 0x32, 0x25,
0x1d, 0xa2, 0xb2, 0xe3, 0x71, 0x2c, 0x73, 0xc0, 0x36, 0x95, 0x5d, 0x17, 0xbc, 0x0e, 0xb5, 0xfe,
0x18, 0x2a, 0x57, 0x3b, 0x2b, 0xeb, 0xb4, 0x66, 0x1f, 0x3f, 0x2d, 0x7d, 0xe8, 0xbf, 0x29, 0x26,
0x6c, 0x8e, 0xbe, 0xc4, 0x40, 0x68, 0x37, 0xce, 0x4f, 0x36, 0xf0, 0x49, 0x7d, 0x8e, 0x6e, 0x8b,
0x4f, 0x8d, 0xbe, 0x0a, 0x15, 0xad, 0x74, 0x17, 0x1d, 0xae, 0x2e, 0x8c, 0xeb, 0x3e, 0x21, 0xe6,
0xcd, 0x93, 0x36, 0x6a, 0x66, 0x7e, 0xa5, 0x8d, 0xaf, 0xc4, 0xe8, 0xf2, 0x70, 0x6b, 0x2a, 0xe2,
0xc3, 0x1c, 0x8e, 0xea, 0x4c, 0xcc, 0xce, 0xf6, 0x8d, 0x89, 0x7a, 0x76, 0x55, 0xd0, 0xfe, 0x2c,
0x68, 0xf7, 0x14, 0x61, 0x9b, 0x5d, 0xcf, 0x61, 0x2e, 0x30, 0x71, 0x2e, 0x96, 0x2e, 0x00, 0xf0,
0xbf, 0x83, 0x59, 0x5e, 0x54, 0x0c, 0x70, 0x71, 0x73, 0xa5, 0x9d, 0x2f, 0x39, 0xb7, 0x06, 0xe5,
0x72, 0xde, 0x36, 0x7d, 0xe5, 0xfc, 0x56, 0x77, 0x21, 0xca, 0x01, 0x05, 0x23, 0x91, 0x42, 0x2d,
0x08, 0x7f, 0x01, 0xc2, 0x9b, 0x86, 0x6c, 0x8e, 0xc1, 0xa3, 0xc8, 0xd7, 0x2e, 0x95, 0x00, 0xd8,
0xdf, 0x47, 0xb2, 0x3b, 0xe3, 0x5d, 0x6a, 0x47, 0xd8, 0x46, 0xb5, 0xe3, 0x27, 0xbd, 0x25, 0x4f,
0xd1, 0x34, 0x0d, 0x32, 0x66, 0x2d, 0x75, 0x19, 0x11, 0xc2, 0x2f, 0xe3, 0x4b, 0xa1, 0xce, 0x07,
0x13, 0x49, 0x57, 0xb4, 0xa7, 0x26, 0x74, 0x2d, 0x6a, 0x8f, 0x42, 0xb7, 0x6d, 0xef, 0x99, 0x02,
0xf5, 0x10, 0x19, 0x1d, 0xbd, 0xe6, 0x08, 0xeb, 0x35, 0x45, 0x8e, 0xb0, 0x32, 0xd9, 0x08, 0xb2,
0x37, 0x8c, 0xec, 0x91, 0x6c, 0x9f, 0xcd, 0xa6, 0x99, 0x32, 0x37, 0xcf, 0x26, 0x3b, 0x34, 0x5e,
0x2c, 0xb6, 0x3a, 0x70, 0x54, 0x08, 0xcd, 0x78, 0x7d, 0x1f, 0xfa, 0x68, 0x49, 0x8e, 0xac, 0x61,
0xe8, 0x2d, 0x0c, 0xdd, 0xc2, 0xb5, 0x43, 0xca, 0xae, 0x0b, 0x24, 0x8a, 0xf3, 0x4a, 0x38, 0x5f,
0xb9, 0xbd, 0xab, 0x7a, 0xec, 0x03, 0x5b, 0x61, 0xaa, 0x72, 0x15, 0xff, 0xa9, 0x1d, 0x7c, 0x99,
0x72, 0xec, 0x99, 0x8d, 0x31, 0xe0, 0x99, 0x3f, 0xe5, 0x03, 0xb8, 0x40, 0x3a, 0xd2, 0xbf, 0xbd,
0x8b, 0x85, 0x51, 0x37, 0x8f, 0x3b, 0xb7, 0x71, 0x59, 0x5f, 0x4a, 0x58, 0xbf, 0x02, 0xcd, 0x07,
0x13, 0xb7, 0xb8, 0xfe, 0x35, 0xc4, 0x87, 0x29, 0xb5, 0xfa, 0x80, 0x95, 0x3e, 0xec, 0x1c, 0x17,
0x4e, 0x8a, 0x4e, 0x53, 0xb4, 0x6a, 0xa0, 0xc8, 0x54, 0x42, 0x3d, 0x94, 0x50, 0x1f, 0x5c, 0xe6,
0xe3, 0xd8, 0x85, 0x63, 0xbd, 0xe5, 0xe3, 0xbe, 0x78, 0x65, 0xda, 0x67, 0x8a, 0x6d, 0x2a, 0xb4,
0x71, 0x69, 0x6c, 0x60, 0x41, 0xf1, 0x1b, 0xa3, 0x28, 0xeb, 0x63, 0x26, 0x70, 0xa6, 0x0a, 0xb6,
0xe7, 0x7a, 0xc9, 0xa1, 0xdf, 0x71, 0x68, 0x41, 0xad, 0x09, 0x2c, 0x92, 0x6e, 0xaa, 0xca, 0x09,
0x4b, 0xd0, 0xb6, 0x40, 0x56, 0xa1, 0x9c, 0xbc, 0x54, 0x75, 0x0c, 0x6d, 0xa1, 0x41, 0xbf, 0x49,
0xd9, 0x78, 0x2b, 0xc0, 0xc1, 0xaa, 0xde, 0xe6, 0x1e, 0xa4, 0x00, 0x1a, 0x72, 0x0c, 0x7f, 0xc1,
0xe5, 0x29, 0x42, 0x36, 0xdd, 0x45, 0xda, 0xbb, 0xd0, 0xa7, 0x93, 0xf6, 0xd7, 0x5f, 0x77, 0xe4,
0x4f, 0x08, 0x90, 0x54, 0x84, 0x63, 0x93, 0x4a, 0x95, 0x8c, 0x4e, 0x14, 0x1d, 0x49, 0xc8, 0xb5,
0x6b, 0x08, 0xbd, 0x60, 0x16, 0x46, 0x70, 0xe1, 0x0f, 0xa5, 0x34, 0xff, 0x2e, 0xd7, 0xfa, 0xf2,
0xe5, 0x30, 0xf4, 0x18, 0x46, 0x39, 0xcc, 0x32, 0x85, 0xac, 0x88, 0x61, 0x23, 0xa6, 0xe5, 0x93,
0x71, 0x9e, 0x1b, 0xeb, 0x59, 0x27, 0xcb, 0x9d, 0x17, 0xa2, 0x18, 0xdc, 0xbf, 0x2b, 0xee, 0x3d,
0x1d, 0x8a, 0xb9, 0x22, 0x8c, 0xc5, 0x66, 0x49, 0x30, 0xcb, 0xca, 0xa8, 0x5f, 0x72, 0xe5, 0xd4,
0xcd, 0xcb, 0x1e, 0x20, 0xc5, 0xaf, 0x51, 0xbb, 0xc5, 0x85, 0xb1, 0x76, 0x33, 0xb2, 0xb2, 0x75,
0x50, 0xad, 0x83, 0xfa, 0x46, 0x35, 0x75, 0xad, 0xb0, 0xa9, 0xf0, 0x90, 0x09, 0x41, 0xc9, 0x42,
0x10, 0x8e, 0xdd, 0xfd, 0x7f, 0xc9, 0xce, 0xf5, 0x38, 0x91, 0x5d, 0x48, 0x95, 0x12, 0xc3, 0xf8,
0x9b, 0xc5, 0x58, 0x76, 0xad, 0xa0, 0x70, 0x48, 0xed, 0x50, 0x48, 0xdd, 0xfb, 0x4a, 0xc8, 0x71,
0x64, 0x2c, 0x97, 0x99, 0xe9, 0x6d, 0x1a, 0x8c, 0x86, 0xea, 0x69, 0x49, 0x81, 0x25, 0xe7, 0x4f,
0x68, 0x7e, 0xc1, 0x23, 0x69, 0x86, 0x78, 0xde, 0xf5, 0xc8, 0x1e, 0x24, 0x87, 0xed, 0x14, 0xfd,
0x28, 0x99, 0x5e, 0xd8, 0x66, 0xa9, 0x6a, 0xcd, 0x03, 0xd7, 0xb8, 0x5d, 0x88, 0x33, 0xc8, 0xe2,
0xf2, 0xea, 0xa8, 0xad, 0x0a, 0x42, 0xc8, 0x47, 0x7a, 0x70, 0x0e, 0x3b, 0x53, 0x21, 0x31, 0x6a,
0x9a, 0x7e, 0xe8, 0x97, 0x94, 0x82, 0x75, 0xa6, 0xe8, 0x24, 0x12, 0x5e, 0x1b, 0x4b, 0xaa, 0xa2,
0xda, 0xf9, 0xc2, 0xbf, 0x81, 0xaf, 0x9c, 0xed, 0x2e, 0x0f, 0xe3, 0x08, 0x6e, 0x64, 0x40, 0x86,
0x46, 0xea, 0xe4, 0x1a, 0x71, 0x2f, 0x92, 0x0f, 0xe3, 0x19, 0x4e, 0xa7, 0xc1, 0x00, 0xd9, 0x3b,
0x8a, 0xe5, 0x4a, 0x3a, 0x49, 0x37, 0x10, 0x73, 0xde, 0x4b, 0x92, 0x00, 0x25, 0x85, 0x45, 0x14,
0x88, 0x9a, 0xa5, 0xcd, 0x14, 0xa3, 0x26, 0xba, 0xc4, 0x21, 0x99, 0x2d, 0x03, 0xce, 0xb3, 0xe6,
0x54, 0xcf, 0x08, 0x67, 0xbc, 0xa8, 0xac, 0x3e, 0x10, 0xc2, 0x7d, 0xf0, 0xe1, 0xe0, 0x51, 0xc9,
0xbd, 0x01, 0x2e, 0x38, 0x3d, 0x11, 0xab, 0x32, 0xab, 0x66, 0x03, 0xf1, 0xfb, 0xa4, 0xed, 0x2e,
0xa0, 0x38, 0x6b, 0xfd, 0x65, 0xc9, 0x23, 0x0c, 0x2a, 0xaf, 0x37, 0xa4, 0x58, 0xce, 0x05, 0x53,
0xa4, 0xc4, 0xe1, 0xb3, 0xc7, 0x68, 0x77, 0x6e, 0x0f, 0x08, 0x5c, 0x23, 0x1e, 0xdc, 0x01, 0x42,
0x4d, 0x32, 0x92, 0x95, 0xe9, 0xb0, 0xc4, 0x28, 0x47, 0x4e, 0x41, 0xd3, 0xf1, 0x9e, 0xe4, 0x83,
0x1b, 0xf9, 0x87, 0x72, 0x97, 0x8b, 0x37, 0x71, 0x1b, 0x44, 0x3a, 0xe0, 0x3e, 0x0f, 0xad, 0x09,
0x36, 0x17, 0xd9, 0x23, 0xd1, 0x3b, 0x16, 0xe5, 0xc2, 0x1a, 0x7e, 0x64, 0xfc, 0x13, 0xfa, 0xa3,
0xdc, 0x9d, 0x8c, 0x08, 0xa4, 0x96, 0xb6, 0xf8, 0x3b, 0x8c, 0x0a, 0x7a, 0xa4, 0x68, 0x5d, 0x62,
0x4f, 0xc8, 0xd8, 0x8e, 0xab, 0xc7, 0x1c, 0x2e, 0x8f, 0x07, 0xc6, 0xd5, 0x3c, 0xf1, 0x9e, 0x68,
0x35, 0x13, 0x65, 0xa0, 0xda, 0xa8, 0xce, 0xab, 0x25, 0xd9, 0xc5, 0x7e, 0xdd, 0x8b, 0x58, 0xee,
0xcb, 0xeb, 0x66, 0x94, 0x70, 0x75, 0x6c, 0x88, 0xd4, 0x23, 0x0b, 0xa5, 0x55, 0xee, 0x31, 0x47,
0xf9, 0xd0, 0x5b, 0x7e, 0x80, 0xbd, 0xc7, 0xf5, 0x19, 0x38, 0x88, 0xe1, 0x4a, 0x95, 0x58, 0x0a,
0x59, 0xa9, 0xb0, 0x71, 0x78, 0x8e, 0x8d, 0xe3, 0x46, 0x38, 0xc6, 0x2f, 0x87, 0x34, 0x2e, 0x1a,
0x14, 0x72, 0xf3, 0x24, 0x44, 0x72, 0x43, 0x3c, 0x1c, 0x4a, 0x0b, 0x3f, 0x23, 0x25, 0x25, 0x1e,
0x98, 0x2e, 0x4e, 0xbb, 0x3e, 0x76, 0x21, 0x89, 0xf6, 0xaa, 0x32, 0x4a, 0x26, 0x6e, 0xaf, 0xf4,
0xd9, 0x70, 0x67, 0xcc, 0xe8, 0xb9, 0x79, 0x55, 0x7a, 0x30, 0x1c, 0x38, 0x4f, 0x15, 0xd4, 0xc3,
0x66, 0xc5, 0xbe, 0x7c, 0xf9, 0xbf, 0x12, 0xee, 0x1b, 0x74, 0x81, 0x9c, 0x2b, 0x1f, 0x87, 0xd2,
0xb6, 0xc6, 0x9b, 0x2d, 0xf1, 0x94, 0x62, 0x5f, 0xa9, 0x9f, 0x5e, 0x97, 0x2b, 0xc4, 0x43, 0x42,
0x50, 0x68, 0x81, 0x78, 0x34, 0x36, 0xf3, 0x0d, 0x61, 0xed, 0xc1, 0xa5, 0xd2, 0x18, 0xe7, 0x9b,
0x57, 0x0a, 0xd4, 0x45, 0x6a, 0x77, 0xae, 0x2b, 0xa3, 0xa2, 0x46, 0x2e, 0x30, 0xd0, 0x2d, 0xdb,
0x3c, 0x7f, 0xf3, 0xfa, 0xbb, 0x57, 0xe2, 0x27, 0xc4, 0x91, 0xc7, 0x2a, 0xf4, 0x99, 0x87, 0x99,
0xbc, 0xa3, 0x12, 0x98, 0x22, 0xaf, 0x57, 0x70, 0xd6, 0x23, 0x48, 0xcb, 0xad, 0xb4, 0x30, 0xc8,
0x98, 0x54, 0xc1, 0x54, 0x08, 0xc5, 0xef, 0xc0, 0xbb, 0xe1, 0x4d, 0xfd, 0x0e, 0x8f, 0x9d, 0x6a,
0x7a, 0x0f, 0x4a, 0xc6, 0xab, 0x41, 0xff, 0x87, 0x23, 0xaa, 0x1a, 0x5f, 0x61, 0x52, 0xad, 0xea,
0x47, 0x4e, 0x2f, 0xc8, 0xc2, 0x2a, 0x29, 0xa1, 0x88, 0x00, 0x1d, 0x0f, 0x95, 0xf2, 0xa8, 0xa9,
0xb5, 0x75, 0x61, 0x96, 0xbc, 0x85, 0x38, 0x7b, 0x77, 0x2f, 0xbf, 0xe6, 0xd7, 0x6a, 0xd8, 0x44,
0x77, 0xce, 0x81, 0x21, 0xc9, 0x4f, 0xa3, 0x8a, 0x0d, 0x93, 0x28, 0xce, 0xb9, 0x3b, 0x97, 0xc9,
0xb5, 0xfa, 0x06, 0x64, 0xed, 0xaa, 0x07, 0x2f, 0x0c, 0x00};

unsigned char gzip_test_ref[] =
"Red Hat eCos Public License v1.1



1. DEFINITIONS

1.1. \"Contributor\" means each entity that creates or
contributes to the creation of Modifications.

1.2. \"Contributor Version\" means the combination of the
Original Code, prior Modifications used by a
Contributor, and the Modifications made by that
particular Contributor.

1.3. \"Covered Code\" means the Original Code or
Modifications or the combination of the Original Code
and Modifications, in each case including portions
thereof.

1.4. \"Electronic Distribution Mechanism\" means a
mechanism generally accepted in the software development
community for the electronic transfer of data.

1.5. \"Executable\" means Covered Code in any form other
than Source Code.
1.6. \"Initial Developer\" means the individual or entity
identified as the Initial Developer in the Source Code
notice required by Exhibit A.

1.7. \"Larger Work\" means a work which combines Covered
Code or portions thereof with code not governed by the
terms of this License.

1.8. \"License\" means this document.

1.9. \"Modifications\" means any addition to or deletion
from the substance or structure of either the Original
Code or any previous Modifications. When Covered Code is
released as a series of files, a Modification is:

     A. Any addition to or deletion from the
     contents of a file containing Original Code or
     previous Modifications.

     B. Any new file that contains any part of the
     Original Code or previous Modifications.

1.10. \"Original Code\" means Source Code of computer
software code which is described in the Source Code
notice required by Exhibit A as Original Code, and
which, at the time of its release under this License is
not already Covered Code governed by this License.

1.11. \"Source Code\" means the preferred form of the
Covered Code for making modifications to it, including
all modules it contains, plus any associated interface
definition files, scripts used to control compilation
and installation of an Executable, or a list of source
code differential comparisons against either the
Original Code or another well known, available Covered
Code of the Contributor's choice. The Source Code can be
in a compressed or archival form, provided the
appropriate decompression or de-archiving software is
widely available for no charge.

1.12. \"You\" means an individual or a legal entity
exercising rights under, and complying with all of the
terms of, this License or a future version of this
License issued under Section 6.1. For legal entities,
\"You\" includes any entity which controls, is controlled
by, or is under common control with You. For purposes of
this definition, \"control\" means (a) the power, direct
or indirect, to cause the direction or management of
such entity, whether by contract or otherwise, or (b)
ownership of fifty percent (50%) or more of the
outstanding shares or beneficial ownership of such
entity.

1.13. \"Red Hat Branded Code\" is code that Red Hat
distributes and/or permits others to distribute under
different terms than the Red Hat eCos Public License.
Red Hat's Branded Code may contain part or all of the
Covered Code.
";

static void entry0( cyg_addrword_t data )
{
    int i;
    unsigned long len;
    int err;
    int buf_size = sizeof(gzip_test_ref)+512;
    unsigned char* unpacked = malloc(buf_size);
    
    if (NULL == unpacked)
        CYG_TEST_NA("Not enough memory for buffers");

    CYG_TEST_INFO("Decompressing");
    len = buf_size;
    err = uncompress(unpacked, &len, gzip_test, sizeof(gzip_test));

    switch (err) {
    case Z_OK:
        CYG_TEST_FAIL_FINISH("Unexpected success!");
        break;
    case Z_MEM_ERROR:
        CYG_TEST_NA("Not enough memory for decompression");
        break;
    case Z_BUF_ERROR:
        CYG_TEST_FAIL_FINISH("Decompressed data larger than original");
        break;
    case Z_DATA_ERROR:
        CYG_TEST_FAIL_FINISH("Decompression failed");
        break;
    case Z_BAD_CRC:
        // This is what we hoped for! zlib CRC is bad because it's
        // different from gz CRC.
        break;
    default:
        CYG_TEST_FAIL_FINISH("Unknown decompression error");
        break;
    }

    for (i = 0; i < sizeof(gzip_test_ref)-1; i++) {
        if (gzip_test_ref[i] != unpacked[i])
            CYG_TEST_FAIL_FINISH("Verify failed");
    }

    CYG_TEST_PASS_FINISH("zlib2 OK");
}

void zlib2_main( void )
{
    CYG_TEST_INIT();

    cyg_thread_create(4, entry0 , (cyg_addrword_t)0, "zlib1",
        (void *)stack[0], STACKSIZE,&thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    zlib2_main();
}

#else /* def CYGFUN_KERNEL_API_C */
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel C API layer disabled");
}
#endif /* def CYGFUN_KERNEL_API_C */

// EOF zlib1.c
