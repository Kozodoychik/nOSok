
extern irq_handler
extern isr_handler

extern context_switch_handler

%macro IRQ 1
	global irq_%1
	irq_%1:
		push %1
		call irq_handler

		add esp, 4
		iret
%endmacro

%macro ISR_WITH_CODE 1
	global isr_%1
	isr_%1:
		pusha
		push ds
		push es
		push fs
		push gs

		push %1
		call isr_handler

		add esp, 4
		pop gs
		pop fs
		pop es
		pop ds
		popa
		iret
%endmacro

%macro ISR_WITHOUT_CODE 1
	global isr_%1
	isr_%1:
		push 0
		pusha
		push ds
		push es
		push fs
		push gs

		push %1
		call isr_handler

		add esp, 4
		pop gs
		pop fs
		pop es
		pop ds
		popa
		iret
%endmacro

global irq_32
irq_32:
	call context_switch_handler
	iret

%assign i 33
%rep 16
	IRQ i
	%assign i i+1
%endrep

ISR_WITHOUT_CODE	0
ISR_WITHOUT_CODE	1
ISR_WITHOUT_CODE	2
ISR_WITHOUT_CODE	3
ISR_WITHOUT_CODE	4
ISR_WITHOUT_CODE	5
ISR_WITHOUT_CODE	6
ISR_WITHOUT_CODE	7
ISR_WITH_CODE 		8
ISR_WITHOUT_CODE	9
ISR_WITH_CODE		10
ISR_WITH_CODE		11
ISR_WITH_CODE		12
ISR_WITH_CODE		13
ISR_WITH_CODE		14
ISR_WITHOUT_CODE	15
ISR_WITHOUT_CODE	16
ISR_WITH_CODE		17
ISR_WITHOUT_CODE	18
ISR_WITHOUT_CODE	19
ISR_WITHOUT_CODE	20
ISR_WITH_CODE		21
ISR_WITHOUT_CODE	22
ISR_WITHOUT_CODE	23
ISR_WITHOUT_CODE	24
ISR_WITHOUT_CODE	25
ISR_WITHOUT_CODE	26
ISR_WITHOUT_CODE	27
ISR_WITHOUT_CODE	28
ISR_WITH_CODE		29
ISR_WITH_CODE		30