
.intel_syntax noprefix

# this is required so the linker won't
# try to create an executable stack
.section .note.GNU-stack,"",@progbits

.global endian32
.text

#	rdi - pointer to buffer
# rsi - 32 bit element count
endian32:
		mov			ecx, esi

_loop:

		mov 		eax, [rdi]
		bswap 	eax
		mov 		[rdi], eax
		add 		rdi, 4
		loop		_loop
		ret
