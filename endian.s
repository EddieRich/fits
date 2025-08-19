
.intel_syntax noprefix
.global endian32
.text

#	rdi - pointer to buffer
# rsi - 32 bit element count
endian32:
	mov	ecx, esi
again:
	mov 	eax, [rdi]
	bswap eax
	mov 	[rdi], eax
	add 	rdi, 4
	loop	again
	ret
