format ELF executable 3
entry main
segment readable writeable
	w dd 10 DUP (0)
segment readable executable
main:
	PUSH EBP
	MOV EBP, ESP
	SUB ESP, 4
.L1:
	SUB ESP, 40
.L2:
	MOV EAX, 0       ; Line 6
	PUSH EAX
	MOV EAX, 2       ; Line 6
	NEG EAX
	PUSH EAX
	POP EAX       ; Line 6
	POP EBX
	PUSH EAX
	MOV EAX, 4
	MUL EBX
	MOV EBX, EAX
	POP EAX
	MOV [w+EBX], EAX
.L3:
	MOV EAX, 0       ; Line 7
	PUSH EAX
	MOV EAX, 0       ; Line 7
	PUSH EAX
	POP EBX
	MOV EAX, 4       ; Line 7
	MUL EBX
	MOV EBX, EAX
	MOV EAX, [w+EBX]
	POP EBX
	PUSH EAX
	MOV EAX, 4
	MUL EBX
	MOV EBX, EAX
	MOV EAX, 44
	SUB EAX, EBX
	MOV EBX, EAX
	POP EAX
	MOV ESI, EBX
	NEG ESI
	MOV [EBP+ESI], EAX
.L4:
	MOV EAX, 0       ; Line 8
	PUSH EAX
	POP EBX
	MOV EAX, 4       ; Line 8
	MUL EBX
	MOV EBX, EAX
	MOV EAX, 44
	SUB EAX, EBX
	MOV EBX, EAX
	MOV ESI, EBX
	NEG ESI
	MOV EAX, [EBP+ESI]
	MOV [EBP-4], EAX
.L5:
	MOV EAX, [EBP-4]       ; Line 9
	CALL print_number
.L6:
	MOV EAX, 1       ; Line 10
	PUSH EAX
	MOV EAX, 0       ; Line 10
	PUSH EAX
	POP ECX
	PUSH ECX
	POP EBX
	MOV EAX, 4       ; Line 10
	MUL EBX
	MOV EBX, EAX
	MOV EAX, [w+EBX]
	PUSH EAX
	INC EAX
	PUSH ECX
	POP EBX
	PUSH EAX
	MOV EAX, 4
	MUL EBX
	MOV EBX, EAX
	POP EAX
	MOV [w+EBX], EAX
	POP EAX       ; Line 10
	POP EBX
	PUSH EAX
	MOV EAX, 4
	MUL EBX
	MOV EBX, EAX
	MOV EAX, 44
	SUB EAX, EBX
	MOV EBX, EAX
	POP EAX
	MOV ESI, EBX
	NEG ESI
	MOV [EBP+ESI], EAX
.L7:
	MOV EAX, 1       ; Line 11
	PUSH EAX
	POP EBX
	MOV EAX, 4       ; Line 11
	MUL EBX
	MOV EBX, EAX
	MOV EAX, 44
	SUB EAX, EBX
	MOV EBX, EAX
	MOV ESI, EBX
	NEG ESI
	MOV EAX, [EBP+ESI]
	MOV [EBP-4], EAX
.L8:
	MOV EAX, [EBP-4]       ; Line 12
	CALL print_number
.L9:
	MOV EAX, 0       ; Line 13
	PUSH EAX
	POP EBX
	MOV EAX, 4       ; Line 13
	MUL EBX
	MOV EBX, EAX
	MOV EAX, [w+EBX]
	MOV [EBP-4], EAX
.L10:
	MOV EAX, [EBP-4]       ; Line 14
	CALL print_number
.L11:
	MOV EAX, 0       ; Line 16
	MOV EDX, EAX
	MOV EAX, [EBP-4]       ; Line 16
	ADD EAX, EDX
	PUSH EAX
	POP EAX       ; Line 16
	MOV [EBP-4], EAX
.L12:
	MOV EAX, 0       ; Line 17
	MOV EDX, EAX
	MOV EAX, [EBP-4]       ; Line 17
	SUB EAX, EDX
	PUSH EAX
	POP EAX       ; Line 17
	MOV [EBP-4], EAX
.L13:
	MOV EAX, 1       ; Line 18
	MOV ECX, EAX
	MOV EAX, [EBP-4]       ; Line 18
	CWD
	MUL ECX
	PUSH EAX
	POP EAX       ; Line 18
	MOV [EBP-4], EAX
.L14:
	MOV EAX, [EBP-4]       ; Line 19
	CALL print_number
.L15:
	MOV EAX, 0       ; Line 21
	MOV EDX, EAX
	MOV EAX, [EBP-4]       ; Line 21
	CMP EAX, EDX
	JG .L16
	JMP .L17
.L16:
	MOV EAX, 10       ; Line 21
	MOV EDX, EAX
	MOV EAX, [EBP-4]       ; Line 21
	CMP EAX, EDX
	JL .L19
	JMP .L17
.L17:
	MOV EAX, 0       ; Line 21
	MOV EDX, EAX
	MOV EAX, [EBP-4]       ; Line 21
	CMP EAX, EDX
	JL .L18
	JMP .L20
.L18:
	MOV EAX, 10       ; Line 21
	NEG EAX
	PUSH EAX
	POP EAX       ; Line 21
	MOV EDX, EAX
	MOV EAX, [EBP-4]       ; Line 21
	CMP EAX, EDX
	JG .L19
	JMP .L20
.L19:
	MOV EAX, 100       ; Line 22
	MOV [EBP-4], EAX
	JMP .L21
.L20:
	MOV EAX, 200       ; Line 24
	MOV [EBP-4], EAX
.L21:
	MOV EAX, [EBP-4]       ; Line 25
	CALL print_number
.L22:
	MOV EAX, 0       ; Line 27
	JMP .L24
.L23:
.L24:
	ADD ESP, 44
	POP EBP
	MOV EAX,1
	XOR EBX, EBX
	INT 0x80
	POP EBP
	RET
;-------------------------------
;         print library         
;-------------------------------
print_number:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi

    sub esp, 32             ; local buffer

    test eax, eax
    jns .positive

    ; print '-'
    push eax

    sub esp, 1
    mov byte [esp], '-'

    mov eax, 4              ; sys_write
    mov ebx, 1              ; stdout
    mov ecx, esp
    mov edx, 1
    int 0x80

    add esp, 1
    pop eax

    neg eax

.positive:
    mov ebx, 10

    lea esi, [esp + 31]
    mov byte [esi], 10
    dec esi

.convert:
    xor edx, edx
    div ebx

    add dl, '0'
    mov [esi], dl
    dec esi

    test eax, eax
    jnz .convert

    inc esi

    lea edx, [esp + 32]
    sub edx, esi

    mov eax, 4              ; sys_write
    mov ebx, 1              ; stdout
    mov ecx, esi
    int 0x80

    add esp, 32

    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
;-------------------------------
