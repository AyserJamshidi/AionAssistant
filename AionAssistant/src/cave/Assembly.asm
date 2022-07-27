PUBLIC my_hook

EXTERN entityAddress:QWORD
EXTERN detourEndAddress:QWORD
;EntityIntercept PROTO arg1:QWORD
EntityIntercept PROTO C

.CODE

my_hook PROC
	; Overwritten bytes
	mov rax,[rax+20h]
	add rsp,20h
	
	; Store RAX into entityAddress
    push rax
    mov entityAddress,rax
	call EntityIntercept
    pop rax

	; Jump back
	jmp [detourEndAddress]
my_hook ENDP

END