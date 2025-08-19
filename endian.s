
.intel_syntax noprefix
.global endian32
.text

#	rdi - pointer to buffer
# rsi - 32 bit element count
endian32:
	mov	rcx, rsi
again:
	mov 	rax, [rdi]
	bswap rax
	mov 	[rdi], rax
	add 	rdi, 4
	loop	again
	ret
