	.arch armv4t
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"UserAgent_register.cpp"
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.text
.Ltext0:
	.section	.rodata
	.align	2
.LC0:
	.ascii	"UserAgent\000"
	.section	.text._Z20UserAgentRegisterExtv,"ax",%progbits
	.align	2
	.global	_Z20UserAgentRegisterExtv
	.hidden	_Z20UserAgentRegisterExtv
	.type	_Z20UserAgentRegisterExtv, %function
_Z20UserAgentRegisterExtv:
.LFB0:
	.file 1 "E:\\Apps\\AirPlay\\IwGame_Debug\\Libs\\UserAgent\\source\\generic\\UserAgent_register.cpp"
	.loc 1 22 0
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
.LCFI0:
	sub	sp, sp, #28
.LCFI1:
.LBB2:
	.loc 1 25 0
	ldr	r3, .L3
.LPIC0:
	add	r3, pc, r3
	.loc 1 25 0
	ldr	r2, .L3+4
	ldr	r2, [r3, r2]
	str	r2, [sp, #20]
	.loc 1 30 0
	mov	r2, #0
	str	r2, [sp, #16]
	.loc 1 35 0
	add	r2, sp, #20
	add	ip, sp, #16
	ldr	r1, .L3+8
	ldr	r1, [r3, r1]
	str	r1, [sp, #0]
	ldr	r1, .L3+12
	ldr	r3, [r3, r1]
	str	r3, [sp, #4]
	mov	r3, #0
	str	r3, [sp, #8]
	ldr	r3, .L3+16
.LPIC1:
	add	r3, pc, r3
	mov	r0, r3
	mov	r1, r2
	mov	r2, #4
	mov	r3, ip
	bl	s3eEdkRegister(PLT)
.LBE2:
	.loc 1 36 0
	add	sp, sp, #28
	ldr	lr, [sp], #4
	bx	lr
.L4:
	.align	2
.L3:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC0+8)
	.word	getUserAgent(GOT)
	.word	_Z13UserAgentInitv(GOT)
	.word	_Z18UserAgentTerminatev(GOT)
	.word	.LC0-(.LPIC1+8)
.LFE0:
	.size	_Z20UserAgentRegisterExtv, .-_Z20UserAgentRegisterExtv
	.section	.text.RegisterExt,"ax",%progbits
	.align	2
	.global	RegisterExt
	.type	RegisterExt, %function
RegisterExt:
.LFB1:
	.loc 1 55 0
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, lr}
.LCFI2:
	.loc 1 56 0
	bl	_Z20UserAgentRegisterExtv(PLT)
	.loc 1 57 0
	ldmfd	sp!, {r4, lr}
	bx	lr
.LFE1:
	.size	RegisterExt, .-RegisterExt
	.section	.debug_frame,"",%progbits
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x1
	.ascii	"\000"
	.uleb128 0x1
	.sleb128 -4
	.byte	0xe
	.byte	0xc
	.uleb128 0xd
	.uleb128 0x0
	.align	2
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.byte	0x4
	.4byte	.LCFI0-.LFB0
	.byte	0xe
	.uleb128 0x4
	.byte	0x4
	.4byte	.LCFI1-.LCFI0
	.byte	0xe
	.uleb128 0x20
	.byte	0x11
	.uleb128 0xe
	.sleb128 1
	.align	2
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.byte	0x4
	.4byte	.LCFI2-.LFB1
	.byte	0xe
	.uleb128 0x8
	.byte	0x11
	.uleb128 0xe
	.sleb128 1
	.byte	0x11
	.uleb128 0x4
	.sleb128 2
	.align	2
.LEFDE2:
	.text
.Letext0:
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LFB0
	.4byte	.LCFI0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI0
	.4byte	.LCFI1
	.2byte	0x2
	.byte	0x7d
	.sleb128 4
	.4byte	.LCFI1
	.4byte	.LFE0
	.2byte	0x2
	.byte	0x7d
	.sleb128 32
	.4byte	0x0
	.4byte	0x0
.LLST1:
	.4byte	.LFB1
	.4byte	.LCFI2
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI2
	.4byte	.LFE1
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0x0
	.4byte	0x0
	.section	.debug_info
	.4byte	0xdd
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF10
	.byte	0x4
	.4byte	.LASF11
	.4byte	0x0
	.4byte	0x0
	.4byte	.Ldebug_ranges0+0x0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF2
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF3
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF4
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF5
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x4
	.byte	0x4
	.uleb128 0x5
	.byte	0x1
	.4byte	.LASF12
	.byte	0x1
	.byte	0x15
	.4byte	.LASF13
	.4byte	.LFB0
	.4byte	.LFE0
	.4byte	.LLST0
	.4byte	0xa9
	.uleb128 0x6
	.4byte	.LBB2
	.4byte	.LBE2
	.uleb128 0x7
	.4byte	.LASF8
	.byte	0x1
	.byte	0x18
	.4byte	0xa9
	.byte	0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x7
	.4byte	.LASF9
	.byte	0x1
	.byte	0x1e
	.4byte	0xbc
	.byte	0x2
	.byte	0x91
	.sleb128 -16
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.4byte	0x64
	.4byte	0xb9
	.uleb128 0x9
	.4byte	0xb9
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.byte	0x4
	.byte	0x7
	.uleb128 0x8
	.4byte	0x48
	.4byte	0xcc
	.uleb128 0x9
	.4byte	0xb9
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.byte	0x1
	.4byte	.LASF14
	.byte	0x1
	.byte	0x36
	.4byte	.LFB1
	.4byte	.LFE1
	.4byte	.LLST1
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",%progbits
	.4byte	0x37
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0xe1
	.4byte	0x66
	.ascii	"UserAgentRegisterExt\000"
	.4byte	0xcc
	.ascii	"RegisterExt\000"
	.4byte	0x0
	.section	.debug_aranges,"",%progbits
	.4byte	0x24
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.4byte	0x0
	.4byte	0x0
	.section	.debug_ranges,"",%progbits
.Ldebug_ranges0:
	.4byte	.LFB0
	.4byte	.LFE0
	.4byte	.LFB1
	.4byte	.LFE1
	.4byte	0x0
	.4byte	0x0
	.section	.debug_str,"MS",%progbits,1
.LASF6:
	.ascii	"long long unsigned int\000"
.LASF0:
	.ascii	"unsigned char\000"
.LASF7:
	.ascii	"long unsigned int\000"
.LASF2:
	.ascii	"short unsigned int\000"
.LASF4:
	.ascii	"unsigned int\000"
.LASF9:
	.ascii	"flags\000"
.LASF10:
	.ascii	"GNU C++ 4.4.3\000"
.LASF11:
	.ascii	"E:\\Apps\\AirPlay\\IwGame_Debug\\Libs\\UserAgent\\s"
	.ascii	"ource\\generic\\UserAgent_register.cpp\000"
.LASF12:
	.ascii	"UserAgentRegisterExt\000"
.LASF13:
	.ascii	"_Z20UserAgentRegisterExtv\000"
.LASF5:
	.ascii	"long long int\000"
.LASF8:
	.ascii	"funcPtrs\000"
.LASF14:
	.ascii	"RegisterExt\000"
.LASF3:
	.ascii	"short int\000"
.LASF1:
	.ascii	"signed char\000"
	.ident	"GCC: (GNU) 4.4.3"
	.section	.note.GNU-stack,"",%progbits
