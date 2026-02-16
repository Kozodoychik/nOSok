
global switch_context

extern current_task

switch_context:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp

    mov esi, [current_task]
    mov edi, [esi+12]

    mov eax, esp
    mov [esi], eax

    mov eax, [edi]
    mov esp, eax

    mov [current_task], edi

    mov eax, 0x20
	out 0x20, al
	out 0xa0, al

    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax

    ret
