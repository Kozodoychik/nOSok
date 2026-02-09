
global switch_context
global init_first_task

switch_context:
    cli
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp

    mov esi, [esp+(8)*4]
    mov edi, [esp+(8+1)*4]

    mov eax, esp
    mov [esi], eax

    mov eax, [edi]
    mov esp, eax

    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax

    sti
    push eax
	mov eax, 0x20
	out 0x20, al
	out 0xa0, al
	pop eax
    ret

init_first_task:
    push ebp
    mov ebp, esp

    mov esi, [ebp+8]
    mov eax, esp
    mov [esi], eax

    pop ebp
    ret