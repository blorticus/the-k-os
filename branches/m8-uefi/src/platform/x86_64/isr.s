.globl isr000
.globl isr001
.globl isr002
.globl isr003
.globl isr004
.globl isr005
.globl isr006
.globl isr007
.globl isr008
.globl isr009
.globl isr010
.globl isr011
.globl isr012
.globl isr013
.globl isr014
.globl isr015
.globl isr016
.globl isr017
.globl isr018
.globl isr019
.globl isr020
.globl isr021
.globl isr022
.globl isr023
.globl isr024
.globl isr025
.globl isr026
.globl isr027
.globl isr028
.globl isr029
.globl isr030
.globl isr031
.globl isr032
.globl isr033
.globl isr034
.globl isr035
.globl isr036
.globl isr037
.globl isr038
.globl isr039
.globl isr040
.globl isr041
.globl isr042
.globl isr043
.globl isr044
.globl isr045
.globl isr046
.globl isr047
.globl isr048
.globl isr049
.globl isr050
.globl isr051
.globl isr052
.globl isr053
.globl isr054
.globl isr055
.globl isr056
.globl isr057
.globl isr058
.globl isr059
.globl isr060
.globl isr061
.globl isr062
.globl isr063
.globl isr064
.globl isr065
.globl isr066
.globl isr067
.globl isr068
.globl isr069
.globl isr070
.globl isr071
.globl isr072
.globl isr073
.globl isr074
.globl isr075
.globl isr076
.globl isr077
.globl isr078
.globl isr079
.globl isr080
.globl isr081
.globl isr082
.globl isr083
.globl isr084
.globl isr085
.globl isr086
.globl isr087
.globl isr088
.globl isr089
.globl isr090
.globl isr091
.globl isr092
.globl isr093
.globl isr094
.globl isr095
.globl isr096
.globl isr097
.globl isr098
.globl isr099
.globl isr100
.globl isr101
.globl isr102
.globl isr103
.globl isr104
.globl isr105
.globl isr106
.globl isr107
.globl isr108
.globl isr109
.globl isr110
.globl isr111
.globl isr112
.globl isr113
.globl isr114
.globl isr115
.globl isr116
.globl isr117
.globl isr118
.globl isr119
.globl isr120
.globl isr121
.globl isr122
.globl isr123
.globl isr124
.globl isr125
.globl isr126
.globl isr127
.globl isr128
.globl isr129
.globl isr130
.globl isr131
.globl isr132
.globl isr133
.globl isr134
.globl isr135
.globl isr136
.globl isr137
.globl isr138
.globl isr139
.globl isr140
.globl isr141
.globl isr142
.globl isr143
.globl isr144
.globl isr145
.globl isr146
.globl isr147
.globl isr148
.globl isr149
.globl isr150
.globl isr151
.globl isr152
.globl isr153
.globl isr154
.globl isr155
.globl isr156
.globl isr157
.globl isr158
.globl isr159
.globl isr160
.globl isr161
.globl isr162
.globl isr163
.globl isr164
.globl isr165
.globl isr166
.globl isr167
.globl isr168
.globl isr169
.globl isr170
.globl isr171
.globl isr172
.globl isr173
.globl isr174
.globl isr175
.globl isr176
.globl isr177
.globl isr178
.globl isr179
.globl isr180
.globl isr181
.globl isr182
.globl isr183
.globl isr184
.globl isr185
.globl isr186
.globl isr187
.globl isr188
.globl isr189
.globl isr190
.globl isr191
.globl isr192
.globl isr193
.globl isr194
.globl isr195
.globl isr196
.globl isr197
.globl isr198
.globl isr199
.globl isr200
.globl isr201
.globl isr202
.globl isr203
.globl isr204
.globl isr205
.globl isr206
.globl isr207
.globl isr208
.globl isr209
.globl isr210
.globl isr211
.globl isr212
.globl isr213
.globl isr214
.globl isr215
.globl isr216
.globl isr217
.globl isr218
.globl isr219
.globl isr220
.globl isr221
.globl isr222
.globl isr223
.globl isr224
.globl isr225
.globl isr226
.globl isr227
.globl isr228
.globl isr229
.globl isr230
.globl isr231
.globl isr232
.globl isr233
.globl isr234
.globl isr235
.globl isr236
.globl isr237
.globl isr238
.globl isr239
.globl isr240
.globl isr241
.globl isr242
.globl isr243
.globl isr244
.globl isr245
.globl isr246
.globl isr247
.globl isr248
.globl isr249
.globl isr250
.globl isr251
.globl isr252
.globl isr253
.globl isr254
.globl isr255

.macro push_isr_regs
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rdi
    push %rsi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
.endm

.macro pop_isr_regs
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rsi
    pop %rdi
    pop %rbp
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
.endm

.align 8

isr000:
	push_isr_regs
	mov $0, %rdi
	call isr_divide_by_zero
	pop_isr_regs
	iretq

isr001:
	push_isr_regs
	mov $1, %rdi
	call isr_debug
	pop_isr_regs
	iretq

isr002:
	push_isr_regs
	mov $2, %rdi
	call isr_non_maskable_interrupt
	pop_isr_regs
	iretq

isr003:
	push_isr_regs
	mov $3, %rdi
	call isr_breakpoint
	pop_isr_regs
	iretq

isr004:
	push_isr_regs
	mov $4, %rdi
	call isr_overflow
	pop_isr_regs
	iretq

isr005:
	push_isr_regs
	mov $5, %rdi
	call isr_bound_range
	pop_isr_regs
	iretq

isr006:
	push_isr_regs
	mov $6, %rdi
	call isr_invalid_opcode
	pop_isr_regs
	iretq

isr007:
	push_isr_regs
	mov $7, %rdi
	call isr_device_not_available
	pop_isr_regs
	iretq

isr008:
	push_isr_regs
	mov $8, %rdi
	call isr_double_fault
	pop_isr_regs
	iretq

# coprocessor-segment-overrun: reserved and not supported
isr009:
	push_isr_regs
	mov $9, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr010:
	push_isr_regs
	mov $10, %rdi
	call isr_invalid_tss
	pop_isr_regs
	iretq

isr011:
	push_isr_regs
	mov $11, %rdi
	call isr_segment_not_present
	pop_isr_regs
	iretq

isr012:
	push_isr_regs
	mov $12, %rdi
	call isr_stack
	pop_isr_regs
	iretq

isr013:
	push_isr_regs
	mov $13, %rdi
	call isr_general_protection
	pop_isr_regs
haltp:
    jmp haltp
	iretq

isr014:
	push_isr_regs
	mov $14, %rdi
	call isr_page_fault
	pop_isr_regs
	iretq

# reserved
isr015:
	push_isr_regs
	mov $15, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr016:
	push_isr_regs
	mov $16, %rdi
	call isr_floating_point_exception_pending
	pop_isr_regs
	iretq

isr017:
	push_isr_regs
	mov $17, %rdi
	call isr_alignment_check
	pop_isr_regs
	iretq

isr018:
	push_isr_regs
	mov $18, %rdi
	call isr_machine_check
	pop_isr_regs
	iretq

isr019:
	push_isr_regs
	mov $19, %rdi
	call isr_simd_floating_point
	pop_isr_regs
	iretq

# reserved
isr020:
	push_isr_regs
	mov $20, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# reserved
isr021:
	push_isr_regs
	mov $21, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# reserved
isr022:
	push_isr_regs
	mov $22, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# reserved
isr023:
	push_isr_regs
	mov $23, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# reserved
isr024:
	push_isr_regs
	mov $24, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# reserved
isr025:
	push_isr_regs
	mov $25, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# reserved
isr026:
	push_isr_regs
	mov $26, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# reserved
isr027:
	push_isr_regs
	mov $27, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# reserved
isr028:
	push_isr_regs
	mov $28, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# reserved
isr029:
	push_isr_regs
	mov $29, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr030:
	push_isr_regs
	mov $30, %rdi
	call isr_security_exception
	pop_isr_regs
	iretq

# reserved
isr031:
	push_isr_regs
	mov $31, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

# 8259 PIC IRQs remapped here
isr032:
	push_isr_regs
	mov $32, %rdi
    mov $0, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr033:
	push_isr_regs
	mov $33, %rdi
    mov $1, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr034:
	push_isr_regs
	mov $34, %rdi
    mov $2, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr035:
	push_isr_regs
	mov $35, %rdi
    mov $3, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr036:
	push_isr_regs
	mov $36, %rdi
    mov $4, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr037:
	push_isr_regs
	mov $37, %rdi
    mov $5, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr038:
	push_isr_regs
	mov $38, %rdi
    mov $6, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr039:
	push_isr_regs
	mov $39, %rdi
    mov $7, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr040:
	push_isr_regs
	mov $40, %rdi
    mov $8, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr041:
	push_isr_regs
	mov $41, %rdi
    mov $9, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr042:
	push_isr_regs
	mov $42, %rdi
    mov $10, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr043:
	push_isr_regs
	mov $43, %rdi
    mov $11, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr044:
	push_isr_regs
	mov $44, %rdi
    mov $12, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr045:
	push_isr_regs
	mov $45, %rdi
    mov $13, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr046:
	push_isr_regs
	mov $46, %rdi
    mov $14, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr047:
	push_isr_regs
	mov $47, %rdi
    mov $15, %rsi
	call isr_irq_pic_spurious
	pop_isr_regs
	iretq

isr048:
	push_isr_regs
	mov $48, %rdi
    mov $16, %rsi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr049:
	push_isr_regs
    mov $49, %rdi
	call isr_routine_does_not_exist
    pop_isr_regs
	iretq

isr050:
	push_isr_regs
    mov $50, %rdi
	call isr_routine_does_not_exist
    pop_isr_regs
	iretq

isr051:
	push_isr_regs
	mov $51, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr052:
	push_isr_regs
	mov $52, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr053:
	push_isr_regs
	mov $53, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr054:
	push_isr_regs
	mov $54, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr055:
	push_isr_regs
	mov $55, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr056:
	push_isr_regs
	mov $56, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr057:
	push_isr_regs
	mov $57, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr058:
	push_isr_regs
	mov $58, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr059:
	push_isr_regs
	mov $59, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr060:
	push_isr_regs
	mov $60, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr061:
	push_isr_regs
	mov $61, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr062:
	push_isr_regs
	mov $62, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr063:
	push_isr_regs
	mov $63, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr064:
	push_isr_regs
	mov $64, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr065:
	push_isr_regs
	mov $65, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr066:
	push_isr_regs
	mov $66, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr067:
	push_isr_regs
	mov $67, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr068:
	push_isr_regs
	mov $68, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr069:
	push_isr_regs
	mov $69, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr070:
	push_isr_regs
	mov $70, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr071:
	push_isr_regs
	mov $71, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr072:
	push_isr_regs
	mov $72, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr073:
	push_isr_regs
	mov $73, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr074:
	push_isr_regs
	mov $74, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr075:
	push_isr_regs
	mov $75, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr076:
	push_isr_regs
	mov $76, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr077:
	push_isr_regs
	mov $77, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr078:
	push_isr_regs
	mov $78, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr079:
	push_isr_regs
	mov $79, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr080:
	push_isr_regs
	mov $80, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr081:
	push_isr_regs
	mov $81, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr082:
	push_isr_regs
	mov $82, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr083:
	push_isr_regs
	mov $83, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr084:
	push_isr_regs
	mov $84, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr085:
	push_isr_regs
	mov $85, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr086:
	push_isr_regs
	mov $86, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr087:
	push_isr_regs
	mov $87, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr088:
	push_isr_regs
	mov $88, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr089:
	push_isr_regs
	mov $89, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr090:
	push_isr_regs
	mov $90, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr091:
	push_isr_regs
	mov $91, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr092:
	push_isr_regs
	mov $92, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr093:
	push_isr_regs
	mov $93, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr094:
	push_isr_regs
	mov $94, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr095:
	push_isr_regs
	mov $95, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr096:
	push_isr_regs
	mov $96, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr097:
	push_isr_regs
	mov $97, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr098:
	push_isr_regs
	mov $98, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr099:
	push_isr_regs
	mov $99, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr100:
	push_isr_regs
	mov $100, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr101:
	push_isr_regs
	mov $101, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr102:
	push_isr_regs
	mov $102, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr103:
	push_isr_regs
	mov $103, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr104:
	push_isr_regs
	mov $104, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr105:
	push_isr_regs
	mov $105, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr106:
	push_isr_regs
	mov $106, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr107:
	push_isr_regs
	mov $107, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr108:
	push_isr_regs
	mov $108, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr109:
	push_isr_regs
	mov $109, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr110:
	push_isr_regs
	mov $110, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr111:
	push_isr_regs
	mov $111, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr112:
	push_isr_regs
	mov $112, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr113:
	push_isr_regs
	mov $113, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr114:
	push_isr_regs
	mov $114, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr115:
	push_isr_regs
	mov $115, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr116:
	push_isr_regs
	mov $116, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr117:
	push_isr_regs
	mov $117, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr118:
	push_isr_regs
	mov $118, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr119:
	push_isr_regs
	mov $119, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr120:
	push_isr_regs
	mov $120, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr121:
	push_isr_regs
	mov $121, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr122:
	push_isr_regs
	mov $122, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr123:
	push_isr_regs
	mov $123, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr124:
	push_isr_regs
	mov $124, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr125:
	push_isr_regs
	mov $125, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr126:
	push_isr_regs
	mov $126, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr127:
	push_isr_regs
	mov $127, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr128:
	push_isr_regs
	mov $128, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr129:
	push_isr_regs
	mov $129, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr130:
	push_isr_regs
	mov $130, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr131:
	push_isr_regs
	mov $131, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr132:
	push_isr_regs
	mov $132, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr133:
	push_isr_regs
	mov $133, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr134:
	push_isr_regs
	mov $134, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr135:
	push_isr_regs
	mov $135, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr136:
	push_isr_regs
	mov $136, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr137:
	push_isr_regs
	mov $137, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr138:
	push_isr_regs
	mov $138, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr139:
	push_isr_regs
	mov $139, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr140:
	push_isr_regs
	mov $140, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr141:
	push_isr_regs
	mov $141, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr142:
	push_isr_regs
	mov $142, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr143:
	push_isr_regs
	mov $143, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr144:
	push_isr_regs
	mov $144, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr145:
	push_isr_regs
	mov $145, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr146:
	push_isr_regs
	mov $146, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr147:
	push_isr_regs
	mov $147, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr148:
	push_isr_regs
	mov $148, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr149:
	push_isr_regs
	mov $149, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr150:
	push_isr_regs
	mov $150, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr151:
	push_isr_regs
	mov $151, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr152:
	push_isr_regs
	mov $152, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr153:
	push_isr_regs
	mov $153, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr154:
	push_isr_regs
	mov $154, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr155:
	push_isr_regs
	mov $155, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr156:
	push_isr_regs
	mov $156, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr157:
	push_isr_regs
	mov $157, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr158:
	push_isr_regs
	mov $158, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr159:
	push_isr_regs
	mov $159, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr160:
	push_isr_regs
	mov $160, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr161:
	push_isr_regs
	mov $161, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr162:
	push_isr_regs
	mov $162, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr163:
	push_isr_regs
	mov $163, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr164:
	push_isr_regs
	mov $164, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr165:
	push_isr_regs
	mov $165, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr166:
	push_isr_regs
	mov $166, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr167:
	push_isr_regs
	mov $167, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr168:
	push_isr_regs
	mov $168, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr169:
	push_isr_regs
	mov $169, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr170:
	push_isr_regs
	mov $170, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr171:
	push_isr_regs
	mov $171, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr172:
	push_isr_regs
	mov $172, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr173:
	push_isr_regs
	mov $173, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr174:
	push_isr_regs
	mov $174, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr175:
	push_isr_regs
	mov $175, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr176:
	push_isr_regs
	mov $176, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr177:
	push_isr_regs
	mov $177, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr178:
	push_isr_regs
	mov $178, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr179:
	push_isr_regs
	mov $179, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr180:
	push_isr_regs
	mov $180, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr181:
	push_isr_regs
	mov $181, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr182:
	push_isr_regs
	mov $182, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr183:
	push_isr_regs
	mov $183, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr184:
	push_isr_regs
	mov $184, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr185:
	push_isr_regs
	mov $185, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr186:
	push_isr_regs
	mov $186, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr187:
	push_isr_regs
	mov $187, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr188:
	push_isr_regs
	mov $188, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr189:
	push_isr_regs
	mov $189, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr190:
	push_isr_regs
	mov $190, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr191:
	push_isr_regs
	mov $191, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr192:
	push_isr_regs
	mov $192, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr193:
	push_isr_regs
	mov $193, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr194:
	push_isr_regs
	mov $194, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr195:
	push_isr_regs
	mov $195, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr196:
	push_isr_regs
	mov $196, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr197:
	push_isr_regs
	mov $197, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr198:
	push_isr_regs
	mov $198, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr199:
	push_isr_regs
	mov $199, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr200:
	push_isr_regs
	mov $200, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr201:
	push_isr_regs
	mov $201, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr202:
	push_isr_regs
	mov $202, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr203:
	push_isr_regs
	mov $203, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr204:
	push_isr_regs
	mov $204, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr205:
	push_isr_regs
	mov $205, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr206:
	push_isr_regs
	mov $206, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr207:
	push_isr_regs
	mov $207, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr208:
	push_isr_regs
	mov $208, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr209:
	push_isr_regs
	mov $209, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr210:
	push_isr_regs
	mov $210, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr211:
	push_isr_regs
	mov $211, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr212:
	push_isr_regs
	mov $212, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr213:
	push_isr_regs
	mov $213, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr214:
	push_isr_regs
	mov $214, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr215:
	push_isr_regs
	mov $215, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr216:
	push_isr_regs
	mov $216, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr217:
	push_isr_regs
	mov $217, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr218:
	push_isr_regs
	mov $218, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr219:
	push_isr_regs
	mov $219, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr220:
	push_isr_regs
	mov $220, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr221:
	push_isr_regs
	mov $221, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr222:
	push_isr_regs
	mov $222, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr223:
	push_isr_regs
	mov $223, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr224:
	push_isr_regs
	mov $224, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr225:
	push_isr_regs
	mov $225, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr226:
	push_isr_regs
	mov $226, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr227:
	push_isr_regs
	mov $227, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr228:
	push_isr_regs
	mov $228, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr229:
	push_isr_regs
	mov $229, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr230:
	push_isr_regs
	mov $230, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr231:
	push_isr_regs
	mov $231, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr232:
	push_isr_regs
	mov $232, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr233:
	push_isr_regs
	mov $233, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr234:
	push_isr_regs
	mov $234, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr235:
	push_isr_regs
	mov $235, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr236:
	push_isr_regs
	mov $236, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr237:
	push_isr_regs
	mov $237, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr238:
	push_isr_regs
	mov $238, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr239:
	push_isr_regs
	mov $239, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr240:
	push_isr_regs
	mov $240, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr241:
	push_isr_regs
	mov $241, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr242:
	push_isr_regs
	mov $242, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr243:
	push_isr_regs
	mov $243, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr244:
	push_isr_regs
	mov $244, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr245:
	push_isr_regs
	mov $245, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr246:
	push_isr_regs
	mov $246, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr247:
	push_isr_regs
	mov $247, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr248:
	push_isr_regs
	mov $248, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr249:
	push_isr_regs
	mov $249, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr250:
	push_isr_regs
	mov $250, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr251:
	push_isr_regs
	mov $251, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr252:
	push_isr_regs
	mov $252, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr253:
	push_isr_regs
	mov $253, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr254:
	push_isr_regs
	mov $254, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

isr255:
	push_isr_regs
	mov $255, %rdi
	call isr_routine_does_not_exist
	pop_isr_regs
	iretq

