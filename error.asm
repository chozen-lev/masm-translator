Data segment
    Byte1 db 200 
    String1 db "тестовий рядок - test string"
    Word1 dw 0cfh*1110b-29d
    Dword1 dd 6fb24e9fh
Data ends

equ1 equ equ3
equ2 equ "1234"
equ3 equ 10011101b
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
    dec byte ptr Word2[eax+eax]
    dec Dword1[esi+edi+2]
l1:
    add al, bl
    add cl, dl
    add eax, eax
    add esi, edi

    jae near ptr l22
    jae near ptr l3

l2: idiv al
    idiv bh
    idiv eax
    idiv edx
    idiv esi
l3:
    cmp bh, byte ptr cs:[ebp+ecx]
    cmp cl, byte ptr es:[eax+esi]
    cmp dl, byte ptr ds:[edi+edi+2]
    cmp eax, byte ptr cs:[eax+ebx+4]
    cmp ebx, dword ptr cs:[ebx+edx+4]
    cmp esi, dword ptr ds:[ebx+edi+6]
    cmp esi, dword ptr es:[ebx+edi+6]
    cmp al, Byte1[ebx+eax]
    cmp dh, Byte1[edx+esi+8]
l4:
    and  ptr cs:[esi+ecx], 101111b
    and byte ss:[ebp+ebx], 25
    and byte ptr ds:[ebp+esp], 48d
    and byte ptr cs:[ebx+ecx], 101111b
    and byte ptr es:[ebx+ecx], equ3
    and word ptr ds:[eax+ebx], 74a2h
    and word ptr cs:[edx+ebx+2], 24h
    and dword ptr ds:[eax+edx+4], equ2
    and String1[esi+esi], 54h
    and String1[edi+eax], 'b'
    and Word1[eax+ebx+6], 133h
l5:
    jae Begin1
    jae l4
    jae l6

l6: mov al, 'a'
    mov ah, 1
    mov bl, 0fh
    mov eax, equ2
    mov ecx, "strn"
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
end