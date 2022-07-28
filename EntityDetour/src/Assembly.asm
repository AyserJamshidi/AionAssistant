PUBLIC entityDetourCode

EXTERN entityAddress:QWORD
EXTERN aiondetour_EndAddress:QWORD
;EntityIntercept PROTO arg1:QWORD
EntityIntercept PROTO C

.CODE

entityDetourCode PROC
	; Overwritten bytes
	mov rax,[rax+20h]
	add rsp,20h
	
	; Store RAX into entityAddress
    push rax
    mov entityAddress,rax
	call EntityIntercept
    pop rax

	; Jump back
	jmp [aiondetour_EndAddress]
entityDetourCode ENDP

END