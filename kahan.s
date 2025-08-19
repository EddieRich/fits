
.intel_syntax noprefix

# this is required so the linker won't
# try to create an executable stack
.section .note.GNU-stack,"",@progbits

.global kahanSum32
.text

#	rdi - pointer to buffer
# rsi - pointer to bad pixel counter
# rdx - pixel count
kahanSum32:
		mov				rcx, rdx
		pxor			xmm0, xmm0
		pxor			xmm1, xmm1

_loop:

		# Load x (current number from array)
		movss			xmm2, [rdi]		# xmm2 = x

		ucomiss		xmm2, xmm2
		jnp				_ok
		mov 			eax, [rsi]
		inc				eax
		mov 			[rsi], eax
		jmp				_next

_ok:

		cvtss2sd	xmm2, xmm2		# convert to double

		# y = x - c
		subsd			xmm2, xmm1		# xmm2 = y

    # t = sum + y
    movsd 		xmm3, xmm0		# Copy sum to xmm3 for intermediate calculation
    addsd 		xmm3, xmm2

    # c = (t - sum) - y
    subsd 		xmm0, xmm3		# sum - t (negated error term)
    subsd 		xmm0, xmm2		# (sum - t) - y  (this is the compensation)
    movsd 		xmm1, xmm0		# Store compensation in c

    # Update sum = t
    movsd xmm0, xmm3

_next:

		# Update pointers and loop counter
		add				rdi, 4				# Move to next float (4 bytes)
    loop			_loop					# dec rcx and loop to _loop if not zero

		cvtsd2ss	xmm0, xmm0		# convert back to float
		ret											# xmm0 
