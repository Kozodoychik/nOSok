
global switch_context
global init_first_task

switch_context:
    cli
    push ebx
    push esi
    push edi
    push ebp

    mov esi, [esp+(4+1)*4]

    mov eax, esp
    mov [esi], eax

    mov esi, [esi+12]

    mov eax, [esi]
    mov esp, eax

    pop ebp
    pop edi
    pop esi
    pop ebx

    push eax
	mov eax, 0x20
	out 0x20, al
	out 0xa0, al
	pop eax
    sti
    ret

init_first_task:
    push ebp
    mov ebp, esp

    mov esi, [ebp+8]
    mov eax, esp
    mov [esi], eax

    pop ebp
    ret