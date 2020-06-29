;EXTERN writei : PROTO
EXTERN jmpBackAddr : PROTO

writei PROTO C

.CODE
PUBLIC AION
PUBLIC F1

AION PROC
	;push rax ;save current rax
	;push rbx ;save current rbx
	;push rcx ;save current rcx
	;push rdx ;save current rdx
	;push rbp ;save current rbp
	;push rdi ;save current rdi
	;push rsi ;save current rsi
	;push r8 ;save current r8
	;push r9 ;save current r9
	;push r10 ;save current r10
	;push r11 ;save current r11
	;push r12 ;save current r12
	;push r13 ;save current r13
	;push r14 ;save current r14
	;push r15 ;save current r15

	;mov rdi, 42

	call writei

	;pop r15 ;restore current r15
	;pop r14 ;restore current r14
	;pop r13 ;restore current r13
	;pop r12 ;restore current r12
	;pop r11 ;restore current r11
	;pop r10 ;restore current r10
	;pop r9 ;restore current r9
	;pop r8 ;restore current r8
	;pop rsi ;restore current rsi
	;pop rdi ;restore current rdi
	;pop rbp ;restore current rbp
	;pop rdx ;restore current rdx
	;pop rcx ;restore current rcx
	;pop rbx ;restore current rbx
	;pop rax ;restore current rax
	;
	jmp qword ptr [jmpBackAddr]

AION ENDP

F1 PROC
	;call writei
	;mov [rsp+10],rbp
	;mov [rsp+20],rsi
	;mov [rsp+08],rbx
F1 ENDP
END