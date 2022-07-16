PUBLIC my_hook

EXTERN jmpBackAddr:QWORD
EXTERN entityAddr:QWORD
EntityIntercept PROTO C

.CODE

my_hook PROC
	; Overwritten bytes
	mov rax,[rax+20h]
	add rsp,20h

	; Store RAX into entityAddr
	mov [entityAddr],rax

	; Jump back
	jmp [jmpBackAddr]
my_hook ENDP

END