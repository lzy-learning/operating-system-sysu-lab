[bits 32]

global function_from_asm
extern id
extern function_from_c
extern function_from_cpp

name db "LZY"
	db 0
function_from_asm:
	push eax
	push dword[id]
	call function_from_c
	pop eax

	mov eax, name
	push eax
	call function_from_cpp
	pop eax

	pop eax
	ret
