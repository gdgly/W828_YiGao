; Copyright hyco 2010. All rights reserved.
;

; exported globals
;
    EXPORT  _enaio
    EXPORT  pulink
    EXPORT  user_entry
    EXPORT  _irq_int
    EXPORT	_fiq_int

    IMPORT  _iclock
    IMPORT  _idevs
    IMPORT  _iinit
    IMPORT  _itask
    IMPORT  _ikernel
    IMPORT  _kilev
    IMPORT  _noints
    IMPORT  _ioff
    IMPORT  _sched
    IMPORT  cpyvec
    IMPORT  ulink
    IMPORT  user_fiq_handler
    IMPORT  user_irq_handler
    IMPORT  comint
    IMPORT  _abort_code
	IMPORT  _iuser

    IMPORT _reset_handler      ; Added BELVAMO ( go to that address in response to a reset exception)

;
; SYSTEM CONFIGURATION PARAMETERS
;

; The following definitions enable (1) or disable (0)
;
ROMRAM      EQU        0        ; copy data from ROM to RAM at start-up time
CPYVEC      EQU        0        ; copy interrupt vectors
CEVIEW      EQU        0        ; enable CE-VIEW


_IRQ_MASK   EQU        0x80
_FIQ_MASK   EQU        0x40

_INT_MASK   EQU        (_IRQ_MASK | _FIQ_MASK)



    AREA    |C$$data|, DATA

    ALIGN

; The following variable should not be changed if 
; C EXECUTIVE and user tasks are linked into a single image.
; If user tasks are linked independently of C EXECUTIVE,
; the "ulink" parameter should be replaced with the offset
; of the start of the user text area.

pulink  DCD     ulink       ; link to user tasks

; SYSTEM START-UP VARIABLES
; These variables are added to simplify the initial test procedure.

_enaclk DCB     1           ; change to 1 to enable the clock
_enaio  DCB     1           ; change to 1 to activate I/O drivers
_enaint DCB     1           ; change to 1 to enable interrupts
        DCB     0           ; padding

; END OF CONFIGURATION PARAMETERS


    AREA    |C$$Code|,    CODE,    READONLY
    
	
user_entry

 IF CEVIEW=1
    EXPORT    _restrt
_restrt
 ENDIF

    mrs        a1,cpsr
    orr        a1,a1,#_INT_MASK        ; Disable interrupts!
    msr        cpsr_cxsf,a1


;
; CExecutive now has control of IRQs only
;
    mov        a1, #_INT_MASK       
    ldr        ip, =_ioff           ; initialise _ioff
    str        a1, [ip]

    bl        _ikernel
    cmp        a1, #0
    bne        abort                ; if out of heap, abort

    ldr        ip, =_enaint
    ldrb       a1, [ip]
    cmp        a1, #0
    beq        no_ints1
    bl        _iinit

; initialize context
;
    bl 		   _iuser               ; perform user initialisation function
    cmp        a1, #0               ;
    bne        abort                ; if unsuccesfull, abort

; initialize clock
;
no_ints1
    ldr        ip, =_enaclk
    ldrb       a1, [ip]
    cmp        a1, #0
    beq        no_clk
    bl        _iclock
    
no_clk
; initialize C EXECUTIVE
    bl        _idevs                ; allocate I/O buffers, init device tables
    cmp        a1, #0
    bne        abort                ; if out of heap, abort

    bl        _itask                ; allocate init tables, user stacks
    cmp        a1, #0
    bne        abort                ; if out of heap, abort

; BEGIN CE-VIEW SUPPORT CODE
 IF CEVIEW=1
    IMPORT    cvoff
    bl        cvoff
 ENDIF
; END CE-VIEW SUPPORT CODE

; enable processor interrupts
;

    ldr        ip, =_enaint
    ldrb       a1, [ip]
    cmp        a1, #0
    beq        no_ints

;	-----------------------------------
;	copy interrupt vector to address 0x0
    ldr        r0, =vect_start  ; source start  address
    mov        r1, #0			; destination start address
    ldr		   r2, =vect_end	; source end, also it is the limit

loopCopyVector  
	CMP     r0, r2          ; loop whilst r1 < r2
	LDRLO   r3, [r0], #4
	STRLO   r3, [r1], #4
	BLO     loopCopyVector
	
;  copy interrupt vector finished, now there is vectors resides in 0x0 
;  ------------------------------------------------------

    mrs        a1, cpsr

    ldr        ip, =_kilev
    str        a1, [ip]         ; initialize kernel interrupt level to noints
    ldr        ip, =_noints
    str        a1, [ip]         ; initialize interrupt level for noints
    
    bic        a1, a1, #0xC0    ; enable FIQ and IRQ interrupts!
    
    msr        cpsr_cxsf, a1  

	
no_ints
    bl        _sched            ; ��ʼ������ȣ�������  start scheduler, never returns

;
; We end up here if something bad happens (e.g. no heap available during init).
;
abort
    ldr     ip, =_abort_code
    strb    a1, [ip]
    b       abort


 IF CEVIEW=1
    EXPORT    _cvidevs

_cvidevs
    stmfd      sp!, {lr}
    ldr        ip, =_enaint
    ldrb       a1, [ip]
    cmp        a1, #0
    beq        cvnoints

cvnoints
    ldr        ip,    =_enaclk
    ldrb        a1, [ip]
    cmp        a1, #0
    beq        cvnoclk
    bl        _iclock
cvnoclk
    bl        _idevs                ; allocate I/O buffers, init device tables
    cmp        a1, #0
    bne        abort                ; if out of heap, abort
    ldmfd        sp!, {lr}
    mov        pc,lr

 ENDIF

; �������뵽�˽���


	IMPORT	DisplayInfo_DataAbort

; ����Ĵ����Ǻ��жϴ�������صĴ��룬�������������룬���е�interrupt��ش���Ӧ������
; 0x100 �ֽڣ��Ա���Է���sram��0x0��ʼ�� 0x100 = 256 �ֽ���
;
; exception vectors  �ж�����
;
; Code between 'vect_start' and 'vect_end' is copied to vector location ��0x0��.
; everything else (ie IRQ and FIQ handlers) are run from their link locations.
; ��0x40 �ֽ� 

vect_start

Reset_Handler           ldr     pc,_Reset_Handler       ; 0x00 == reset          (svc   mode)
Undefined_Handler       ldr     pc,_Undefined_Handler       ; 0x04 == undef          (undef mode)
SWI_Handler             ldr     pc,_SWI_Handler             ; 0x08 == SWI            (svc   mode)
PrefetchAbort_Handler   ldr     pc,_PrefetchAbort_Handler          ; 0x0c == prefetch abort (abort mode)
DataAbort_Handler       ldr     pc,_DataAbort_Handler       ; 0x10 == data abort     (abort mode)
Reserved_Handler        b       Reserved_Handler        ; 0x14 == reserved
IRQ_Handler             ldr     pc, _IRQ_Handler        ; 0x18 == IRQ            (irq   mode)
FIQ_Handler             ldr     pc, _FIQ_Handler        ; 0x1c == FIQ            (fiq   mode)

_Reset_Handler  dcd     _reset_handler					; 0x20 DCD
_IRQ_Handler    dcd     _irq_int						; 0x24 DCD  _irq_int�ĵ�ַ�Ǳ�����������
_FIQ_Handler    dcd     _fiq_int						; 0x28 DCD

_Undefined_Handler			dcd     _undefined_handler		; 0x2C DCD
_SWI_Handler				dcd		_swi_handler			; 0x30 DCD
_PrefetchAbort_Handler		dcd     _prefetchabort_handler	; 0x34 DCD
_DataAbort_Handler			dcd     _dataAbort_handler		; 0x38 DCD

vect_end



_swi_handler
	;step1, disable FIQ and IRQ interrupt via CPSR
;	SUB 	lr,lr,#8		;adjust lr	
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	DisplayInfo_SWI
	bl	DisplayInfo_SWI
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time

_undefined_handler
	;step1, disable FIQ and IRQ interrupt via CPSR
	SUB 	lr,lr,#4		;adjust lr	
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	DisplayInfo_Undefined
	bl	DisplayInfo_Undefined
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time

_prefetchabort_handler
	;step1, disable FIQ and IRQ interrupt via CPSR
	SUB 	lr,lr,#4		;adjust lr	
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	DisplayInfo_PrefetchAbort
	bl	DisplayInfo_PrefetchAbort
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time

_dataAbort_handler
	;step1, disable FIQ and IRQ interrupt via CPSR
	SUB 	lr,lr,#8		;adjust lr	
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	DisplayInfo_DataAbort
	bl	DisplayInfo_DataAbort
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time


; �����������code ������Ϊ�˽���δ������sram�У��ӿ촦���ٶ�
    AREA    |IntHandler$$Code|,    CODE,    READONLY

	EXPORT	 armInterruptResume
	EXPORT	 armInterruptPause

;
; irq handler  0x50 �ֽ�
;
; NOTE: When an irq interrupt is received, there is a window before
; interrupts are disabled that an FIQ interrupt can be received.
; irqģʽ��ջ�Ǵӵ͵��ߣ���irq��r13 ��ʼ���ӵ͵�ַ���ߵ�ַ�������5�����ݣ�������
;		r0, spsr, spsr+SVC+ARM mode, �û�interrupt handler��lr


_irq_int
    str		r0, [r13,#0]			; �� r0 ���뵽irq��ջ�У���������δ�����
    								; ��ʹ����r0���м��㣬����ֻ����r0����

    mrs     r0, spsr				; �ڽ�ֹ�жϵ�ָ���ִ�й����У�ARM�����жϣ�
    tst     r0, #_IRQ_MASK          ; ���������ָ������������Ա�������ָ���
    ldrne   r0, [r13,#0]			; ��ջ���Ƴ� r0
    subnes  pc, lr, #4              ; ����


    str     r0, [r13, #4]			; r0 ���spsr�� ���뵽ջ��
    orr     r0, r0, #0x13           ; spsr, ��ΪSupervisor mode
    bic     r0, r0, #0x20           ; ��ΪARM mode���жϴ������Ҫ��һ����ARMģʽ
    str     r0, [r13, #8]			; r0 ���spsr+SVC+ARM mode, �� ���뵽ջ��
    ;orr     r0, r0, #0xc0           ; ��ֹ fiq�� irq �ж�
    orr     r0, r0, #_IRQ_MASK      ; ��ֹ irq �ж�
    msr     spsr_c, r0				; ��r0 �ش浽spsr�У�c��ʾ���Ŀ���λ
    
    ldr     r0, [pc, #irq_handler-.-8]
    								; ����ָ���ʾ��ʹ�����pcƫ����(�죿���� �ҵ�irq_handler
    								; �ĵ�ַ�е����ݣ�����r0��-8 ������arm9 ��ˮ�ߵ�Ե��
    								; r0 ��ŵ��Ǻ���user_irq_handler��ڵ�ַ
    str     r0, [r13, #0xc]			; ���뵽irqջ��: interrupt handler
    
    sub     lr, lr, #4              ; ��FIQ/IRQ�ж�������������������
    								; ARM ��ָ����ɺ󣬴�������Ҫ���FIQ/IRQ pin �Ƿ�Ϊ��(�����жϣ�
    								; ͬʱ���cpsr�Ƿ�������Ӧ���жϣ����ԣ�IRQ/FIQ ��pc ���º�Ų�����
    								; ����pcֵΪΪ��ǰָ���ֵַ +8 ����ˮ�ߣ�+ 4 ������Ϊ��һ��ָ�룩
    								; arm�ں˹涨��lr ���Ǵ棨pc-4����ֵ��
    								; �����������Ҫlrָ����һ��ָ�����Ҫlr�ټ�4
    
    str     lr, [r13, #0x10]		; ���뵽irqջ��: lr
    mov     r0, r13					; ����ջָ��stack pointer�浽 r0 ��
    
    ldr     lr, [pc, #_comint-.-8] 	; ʹ�����ƫ��������lr��Ϊcomint����ڵ�ַ
    subs    pc, lr, #0              ; ����comint, תΪ Supv mode�� �˳�IRQ�ж�
    								; �������ָ������s��ǣ���r15(pc)��Ϊ��Ž����
    								; �Ĵ�������һ�ǳ������ת�������ֵ�����ǵ�ǰģ
    								; ʽ��SPSRֵ�����Ƶ�CPSR�У����ͨ�����������ж�
    								; ģʽ�з��ء���Ȼ������ģʽ�ı䣬r13��ֵҲ���ı�


irq_handler     DCD user_irq_handler        ; Handler entry point in driver
_comint         DCD comint          ; Common Handler entry point



;
; fiq handler  0x38 �ֽ�
; This code saves critical information and calls common interrupt (comint)
; handling code. Info is saved on fiq_stack.
; fiqģʽ��ջ�Ǵӵ͵��ߣ���fiq��r13 ��ʼ���ӵ͵�ַ���ߵ�ַ��ע�����ʹ����stmeaָ����� e=Empty ջָ��ָ��
; ��һ���յ�λ�ã� a= ascending ����, ���ӵ͵��ߣ��������5�����ݣ�������
;		r0, spsr, spsr+SVC+ARM mode, �û�interrupt handler��lr

;_fiq_int
;	mrs		r8, spsr
;	tst		r8, #_FIQ_MASK			; ARM allows interrupt during an instruction
;	subnes	pc, lr, #4				; which disables interrupts!, return immediately
;									; to complete the section of code.
;
;	orr		r9, r8, #0x13			; spsr, with Supervisor mode
;	bic		r9, r9, #0x20			; set mode to ARM
;	ldr		r10, [pc, #fiq_handler-.-8]
;	sub		lr, lr, #4				; correct pc for RTI sequence
;	stmea	sp, {r0,r8-r10,lr}		; fiq_stack: r0, &handler, spsr, lr
;	mov		r0, sp					; save stack pointer in r0
;	orr		r9, r9, #0xc0			; spsr, with Supervisor mode, ints disabled
;	msr		spsr_c, r9
;	ldr		lr, [pc, #_comint-.-8]
;	subs	pc, lr, #0				; branch to comint, change to Supv mode
;
;
;fiq_handler     DCD user_fiq_handler    ; Handler entry point in driver

_fiq_int
	;step1, disable FIQ and IRQ interrupt via CPSR
	SUB 	lr,lr,#4				;adjust lr
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	user_fiq_handler
	bl		user_fiq_handler		;20110504, �޸�fiq��handler��FIQ�ж��е��ã�
									;�޸ĺ�FIQ�ж϶�ջΪ����ַ��ʽ�����޸���init.s��line 174
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time

;fiq_handler     DCD user_fiq_handler    ; Handler entry point in driver

;	������������Ϊ 0x34 �ֽ�
;-----------------------------------------------------------------------------
; ����:	C����ԭ�� void armInterruptResume( UInt8 interrupt_level)
; ����:	Enable the interrupts using the CPSR flags 
; ����:	interrupt_level	-- arm ��interrupt �ı��,�������Ϊ��һ�ε��ж�ʹ�ܱ�־
; ����:	none
;----------------------------------------------------------------------------*/
armInterruptResume
	MRS      r1,CPSR
	AND      r0,r0,#3
	MOV      r0,r0,LSL #6
	BIC      r1,r1,r0
	MSR      CPSR_c,r1
	BX       r14

;-----------------------------------------------------------------------------
; ����:	C ����ԭ��  UInt8 armInterruptPause( void )
; ����:	Disable the interrupts using the CPSR flags 
; ����:	none
; ����:	interrupt_level	-- arm ��interrupt �ı��, ����ԭ�����ж�ʹ�ܱ�־
;----------------------------------------------------------------------------*/
armInterruptPause
	MRS      r1,CPSR
	MVN      r0,r1,LSR #6
	AND      r0,r0,#3
	ORR      r1,r1,#0xc0
	MSR      CPSR_c,r1
	BX       r14

  END