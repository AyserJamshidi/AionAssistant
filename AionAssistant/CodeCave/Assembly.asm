EXTERN jmpBackAddr : PROTO
interceptEntities PROTO C

.CODE
PUBLIC AION

AION PROC
	; We push these or else we will be corrupting a specific
	; part of the registers when we return to stack
	pushfq
	;push rax
	;push rcx
	;push rdx
	;push rbx
	push rbp
	push rsi
	;push rdi
	;push r8
	;push r9
	;push r10
	;push r11
	;push r12
	;push r13
	;push r14
	;push r15

	call interceptEntities
	
	;call writeHi

	; Place original registers
	;pop r15
	;pop r14
	;pop r13
	;pop r12
	;pop r11
	;pop r10
	;pop r9
	;pop r8
	;pop rdi
	pop rsi
	pop rbp
	;pop rbx
	;pop rdx
	;pop rcx
	;pop rax
	popfq

	jmp qword ptr [jmpBackAddr]
AION ENDP

END