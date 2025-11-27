; HISTORY:
; ==========
; Intel 8085 was a 8 bit microprocessor (limited in performance, memory addressing).
; Intel 8086 was a 16 bit microprocessor. It operated in real mode.
; Intel 80286 introduced 16 bit protected mode (with memory protection).
; Intel 80386 introduced 32 bit architecture.
; AMD Opteron introduced 64 bit architecture. It operated in long mode.
; AMD64's instruction set adopted by Intel, starting with Pentium 4, Core 2...
; To maintain compatibilty, all modern x86-family processors start in real mode.
; 
;
; 8 bit:
; -------
; Simple addressing (0x1234)
; Memory limit = 2^8 - 1 = 0xFFFF = 64KB
;
; 
; Real mode (16 bit):
; --------------------
; It has several segments like Code (CS), Data (DS), Extra (ES)
; It uses segment:offset addressing
; Actual address = segment * 0x10 + offset e.g. 0x1230:0x000F = 0x1230F
; Memory limit = 2^16 - 1 = 0xFFFFF
; 0xFFFFF = 0xF000:0xFFFF = 0xF0F0:0xF0FF = ... (multiple ways of accessing same memory)
; Any program can access any memory; no safety.
; 
;
; 16 bit protected mode:
; -----------------------
; CS, DS, ES don't hold addresses but selectors to GDT
; GDT (Global Descriptor Table) holds valid address limits for each segment
; LDT (Local Descriptor Table) is for individual program (paging used instead nowadays)
; Due to historical reasons, GDT, LDT have very weird formats.
; 
; Any memory access must be for address between BASE and LIMIT, which we'll specify.
; For flat memory model, BASE = 0x00000, LIMIT = 0xFFFFF;
; 
; 
; 32 bit protected mode:
; -----------------------
; 32 bit processor => can handle 2^32-1 = 4GB memory
; But 0x00000 -> 0xFFFFF = 1MB memory
; 
; So, Intel introduced GRANULARITY bit
; GRANULARITY = 0 => LIMIT = 0xFFFFF bytes = 1MB
; GRANULARITY = 1 => LIMIT = 0xFFFFF * 4KB = 4GB
; 
; Also, DEFAULT_OPERATION_SIZE (D) bit for compatibility
; D = 0 => CPU expects 16 bit instructions.
; D = 1 => CPU expects 32 bit instructions.
;
; There is also a ACCESS byte: P|DPL|S|E|C/W|R/A|A
; P = segment present in memory                                     = 1
; DPL = Desc. Privilege Level (0b00 = kernel, ..., 0b11 = user)     = 0
; S = is system segment? e.g. LDT, TSS                              = 0
; E = executable                                                    = 1 for CS, 0 for DS
; C/W = conforming / writable                                       = 0 for CS, 1 for DS
; R/A = readable/accessable                                         = 1
; A = accessed by CPU (set by CPU not us)                           = 0

; BTW, TSS means Task Switching Segment for hardware task switching
; needed for switching between kernel and user space to perform
; system services (printing to console, saving file...).

section .data 

global gdtr
global GDT_CODE_SEL
global GDT_DATA_SEL

struc gdt_entry_struct
	limit_low:   resb 2
	base_low:    resb 2
	base_middle: resb 1
	access:      resb 1
	flags_limit: resb 1 ; flags = G|D|L|Reserved, limit_high (L for long mode = 64 bit)
	base_high:   resb 1
endstruc

gdt_start:
    ; Null Descriptor: Base=0, Limit=0, Access=0.
    istruc gdt_entry_struct
        at limit_low,   dw 0
        at base_low,    dw 0
        at base_middle, db 0
        at access,      db 0
        at flags_limit, db 0
        at base_high,   db 0
    iend
gdt_code:
    ; Code Segment: Base=0, Limit=4GB, Access=Executable/Readable.
    istruc gdt_entry_struct
        at limit_low,   dw 0xFFFF
        at base_low,    dw 0
        at base_middle, db 0
        at access,      db 0b10011010
        at flags_limit, db 0b11001111
        at base_high,   db 0
    iend
gdt_data:
    ; Data Segment: Base=0, Limit=4GB, Access=Writable/Readable.
    istruc gdt_entry_struct
        at limit_low,   dw 0xFFFF
        at base_low,    dw 0
        at base_middle, db 0
        at access,      db 0b10010010
        at flags_limit, db 0b11001111
        at base_high,   db 0
    iend
gdt_end:

gdtr:
    dw gdt_end - gdt_start - 1
    dd gdt_start

GDT_CODE_SEL equ gdt_code - gdt_start
GDT_DATA_SEL equ gdt_data - gdt_start
