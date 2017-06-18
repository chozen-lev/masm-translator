Data1 segment
    Byte1 db 200 
    String1 db "тестовий рядок - test string"
    Word1 dw 0cfh*1110b-29d
    Dword1 dd 6fb24e9fh
Data1 ends

Data12 segment

equ11 equ equ1
equ1 equ 01010101b
equ21 equ "14"
equ23 equ "1"
equ2 equ "1234"
equ22 equ "1234567"
equ3 equ 25
equ4 equ 654d
equ5 equ 214h
equ6 equ 0a8c4bh

Code segment
Begin:
    clc

    jae l1
    jae l6

    dec byte ptr cs:[eax+ebx]
    dec word ptr ds:[ecx+edx+2]
    dec word ptr es:[ecx+edx+4]
    dec word ptr cs:[ecx+edx+6]
    dec dword ptr ds:[ecx+edx+8]
    dec dword ptr es:[ecx+edx+12345678h]
    dec byte ptr Word1[eax+eax]
    dec Dword1[esi+edi+2]
l1:
    add al, bl
    add cl, dl
    add eax, eax
    add esi, edi

    jae near ptr l2
    jae near ptr l3

l2: idiv al
    idiv bh
    idiv eax
    idiv edx
    idiv esi
l3:
    cmp bh, word ptr cs:[esi+ecx]
    cmp cl, byte ptr es:[eax+esi]
    cmp dl, byte ptr ds:[edi+edi+2]
    cmp eax, dword ptr cs:[eax+ebx+4]
    cmp ebx, dword ptr cs:[ebx+edx+4]
    cmp esi, dword ptr ds:[ebx+edi+6]
    cmp esi, dword ptr es:[ebx+edi+6]
    cmp al, Byte1[ebx+eax]
    cmp dh, Byte1[edx+esi+8]
l4:
    and byte ptr cs:[esi+ecx], 101111b
    and byte ptr cs:[eax+ebx], 25
    and byte ptr es:[eax+ebx], 48d
    and byte ptr cs:[ebx+ecx], 101111b
    and byte ptr es:[ebx+ecx], equ1
    and word ptr ds:[eax+ebx], 74a2h
    and word ptr cs:[edx+ebx+2], 24h
    and dword ptr ds:[eax+edx+4], 1
    and String1[esi+esi], 54h
    and String1[edi+eax], equ1
    and Word1[eax+ebx+6], 133h
l5:
    jae Begin
    jae l4
    jae l6

l6: mov al, 'a'
    mov ah, 1
    mov bl, 0fh
    mov eax, equ6
    mov ecx, "stig"
    mov edi, 1d
    mov edi, 0FFFFFh

    jae l7
    jae l8
l7:
    xor byte ptr cs:[ebx+eax], bl
    xor dword ptr ds:[edx+esi+2], ebx
    xor Byte1[ecx+edi], dl
    xor Dword1[ecx+esi+4], ecx
l8:
Code ends