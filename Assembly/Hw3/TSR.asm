CR	EQU	0dh
LF	EQU	0ah
;
HTOA	MACRO	asc,hex		; 講義的東西  懶得改...
		LOCAL	next,lop	;
		mov 	dx,hex
		mov		di,0
		lea		bx,ascii_table
next:	mov		al,0
		mov		cx,4
lop:	sal		dx,1
		rcl		al,1
		loop	lop
		xlatb
		mov		BYTE	PTR	asc[di],al
		inc		di
		cmp		di,04h
		jnz		next
		ENDM
		.MODEL	SMALL
		.DATA	
ascii_table	DB	'0123456789ABCDEF'
string		DB  50 DUP (0)		;存鍵盤讀入ascii code的陣列
string1     DB  50 DUP (0)		;1~10 都是存指令的陣列
string2     DB  50 DUP (0)
string3     DB  50 DUP (0)
string4     DB  50 DUP (0)
string5     DB  50 DUP (0)
string6     DB  50 DUP (0)
string7     DB  50 DUP (0)
string8     DB  50 DUP (0)
string9     DB  50 DUP (0)
string10    DB  50 DUP (0)
buffer		DB  10 DUP (0)
index		DW	0				;記錄存到第幾個位置
index2		DW  0				;記錄要output第幾個string
;index		DB  1 DUP (0)	
cs_ip_ss_sp_flag	DB	CR,LF,"CS:" ;講義上的
cs_val	DD	?
		DB	CR,LF,"IP:"
ip_val	DD	?
		DB	CR,LF,"SS:"
ss_val	DD	?
		DB	CR,LF,"SP:"
sp_val	DD	?
		DB	CR,LF,"FLAG:"
fg_val	DD	?
		DB	00
old_sp	DW	?
.code
start:	jmp	init	;處理鍵盤輸入開始
main:	sti
		push	bp
		mov		bp,sp
		push	ax
		push	bx
		push	cx
		push 	dx
		push	si
		push	di
		push	ds
getkey: mov		ax,@DATA	;這樣才可以讀得到memory變數
		mov		ds,ax
		in		al,60h		; 把scan code 存到al
		;以下為scan code <-> ascii code 轉換的table		
		CMP     al , 16
		je      Q
		cmp		al , 17
		je		W
		cmp		al , 18
		je		E
		cmp		al , 19
		je		R
		cmp		al , 20
		je		T
		cmp		al , 21
		je		Y
		cmp		al , 22
		je		U
		cmp		al , 23
		je		I
		cmp		al , 24
		je		O
		cmp		al , 25
		je		P
		cmp		al , 26
		je		_26
		cmp		al , 27
		je		_27
		cmp		al , 30
		je		A
		cmp		al , 31
		je		S
		cmp		al , 32
		je		D
		cmp		al , 33
		je		F
		cmp		al , 34
		je		G
		cmp		al , 35
		je		H
		cmp		al , 36
		je		J
		cmp		al , 37
		je		K
		cmp		al , 38
		je		L
		cmp		al , 39
		je		_39
		cmp		al , 40
		je		_40
		cmp		al , 41
		je		_41
		cmp		al , 43
		je		_43
		cmp		al , 44
		je		Z
		cmp		al , 45
		je		X
		cmp		al , 46
		je		_C
		cmp		al , 47
		je		V
		cmp		al , 48
		je		B
		cmp		al , 49
		je		N
		cmp		al , 50
		je		M
		cmp		al , 51
		je		_51
		cmp		al , 52
		je		_52
		cmp		al , 53
		je		_53
		cmp     al , 2
		je      _1
		cmp     al , 3
		je      _2
		cmp     al , 4
		je      _3
		cmp     al , 5
		je      _4
		cmp     al , 6
		je      _5
		cmp     al , 7
		je      _6
		cmp     al , 8
		je      _7
		cmp     al , 9
		je      _8
		cmp     al , 10
		je      _9
		cmp     al , 11
		je      _0
		cmp     al , 57 ;space
		je		_space
		cmp     al , 28 ;enter
		je      jump_next
		cmp     al , 14 ; back
		je      back
		cmp     al,83	;當按下Del時，輸出上一次的指令
		je      printstr
		jmp     exit

jump_next: 		;按下enter以後
		mov     index2 , 0
		mov     cl , 50
		mov     si , 0
		cmp     string[0],0   ;string是空的就不處理
		je		exit
cp_loop:mov     al,string[si]	;所有的string都往下移一格
		mov     buffer[0],al
		mov     al,string1[si]
		mov     buffer[1],al
		mov     al,string2[si]
		mov     buffer[2],al
		mov     al,string3[si]
		mov     buffer[3],al
		mov     al,string4[si]
		mov     buffer[4],al
		mov     al,string5[si]
		mov     buffer[5],al
		mov     al,string6[si]
		mov     buffer[6],al
		mov     al,string7[si]
		mov     buffer[7],al
		mov     al,string8[si]
		mov     buffer[8],al
		mov     al,string9[si]
		mov     buffer[9],al
		
		mov     al,buffer[0]
		mov     string1[si], al
		mov     al,buffer[1]
		mov     string2[si], al
		mov     al,buffer[2]
		mov     string3[si], al
		mov     al,buffer[3]
		mov     string4[si], al
		mov     al,buffer[4]
		mov     string5[si], al
		mov     al,buffer[5]
		mov     string6[si], al
		mov     al,buffer[6]
		mov     string7[si], al
		mov     al,buffer[7]
		mov     string8[si], al
		mov     al,buffer[8]
		mov     string9[si], al
		mov     al,buffer[9]
		mov     string10,al
		mov     string[si],0 ;string 清空
		inc     si
		dec		cl
		cmp		cl , 0
		jne      cp_loop
		mov     index , 0
		jmp     exit
		;ascii table
_1:		mov al , '1'
		jmp     save_value
_2:		mov al , '2'
		jmp     save_value
_3:		mov al , '3'
		jmp     save_value
_4:		mov al , '4'
		jmp     save_value
_5:		mov al , '5'
		jmp     save_value
_6:		mov al , '6'
		jmp     save_value
_7:		mov al , '7'
		jmp     save_value
_8:		mov al , '8'
		jmp     save_value
_9:		mov al , '9'
		jmp     save_value
_0:		mov al , '0'
		jmp     save_value
_space: mov al , 32
		jmp     save_value
Q:		mov al , 'Q'
		jmp     save_value
W:		mov al , 'W'
		jmp     save_value
E:		mov al , 'E'
		jmp     save_value
R:		mov al , 'R'
		jmp     save_value
T:		mov al , 'T'
		jmp     save_value
Y:		mov al , 'Y'
		jmp     save_value
U:		mov al , 'U'
		jmp     save_value
I:		mov al , 'I'
		jmp     save_value
O:		mov al , 'O'
		jmp     save_value
P:		mov al , 'P'
		jmp     save_value
_26:	mov al , '['
		jmp     save_value
_27:	mov al , ']'
		jmp     save_value
A:		mov al , 'A'
		jmp     save_value
S:		mov al , 'S'
		jmp     save_value
D:		mov al , 'D'
		jmp     save_value
F:		mov al , 'F'
		jmp     save_value
G:		mov al , 'G'
		jmp     save_value
H:		mov al , 'H'
		jmp     save_value
J:		mov al , 'J'
		jmp     save_value
K:		mov al , 'K'
		jmp     save_value
L:		mov al , 'L'
		jmp     save_value
_39:	mov al , ':'
		jmp     save_value
_40:	mov al , 44
		jmp     save_value
_41:	mov al , '`'
		jmp     save_value
_43:	mov al , 92
		jmp     save_value
Z:		mov al , 'Z'
		jmp     save_value
X:		mov al , 'X'
		jmp     save_value
_C:		mov al , 'C'
		jmp     save_value
V:		mov al , 'V'
		jmp     save_value
B:		mov al , 'B'
		jmp     save_value
N:		mov al , 'N'
		jmp     save_value
M:		mov al , 'N'
		jmp     save_value
_51:	mov al , ','
		jmp     save_value
_52:	mov al , '.'
		jmp     save_value
_53:	mov al , '/'
		jmp     save_value		
back:	;按下backspace所做的事
		mov si , index
		cmp si , 0			;如果string已經空了，那就啥也不做
		je  exit
		dec si
		mov al , string[si]	;string倒退一個
		cmp al , 9
		je   exit
		mov string[si] , 0
		mov index , si
		jmp exit
		
save_value:		;把ascii code存到string裡
		mov     index2 , 0
		mov 	si , index
		mov     string[si] , al
		inc     si
		mov     index , si

		jmp		exit

_09:	inc index2	;按下enter的時候
		jmp exit

printstr:			;開始印字元
		mov     ah,0eh
		mov     al,10	;換行
		int     10h
		mov     al,8
		mov 	cl , 70  ;倒退70格
del:	int 10h
		loop del
		
		mov 	si,0
		;mov     index2 , 2
		;
nextchar:	;看index2是多少，就輸出string幾+1
		cmp     index2 , 0
		je      do1
		cmp     index2 , 1
		je      do2
		cmp     index2 , 2
		je      do3
		cmp     index2 , 3
		je      do4
		cmp     index2 , 4
		je      do5
		cmp     index2 , 5
		je      do6
		cmp     index2 , 6
		je      do7
		cmp     index2 , 7
		je      do8
		cmp     index2 , 8
		je      do9
		cmp     index2 , 9
		je      do10
		
do1:		mov 	al , string1[si]
		jmp 	print
do2:		mov 	al , string2[si]
		jmp 	print
do3:		mov 	al , string3[si]
		jmp 	print
do4:		mov 	al , string4[si]
		jmp 	print
do5:		mov 	al , string5[si]
		jmp 	print
do6:		mov 	al , string6[si]
		jmp 	print
do7:		mov 	al , string7[si]
		jmp 	print
do8:		mov 	al , string8[si]
		jmp 	print
do9:		mov 	al , string9[si]
		jmp 	print
do10:		mov 	al , string10[si]
		jmp 	print
		
print:	mov		ah,0eh
		;cmp     al , 09	
		;je      _09
		
		cmp     al , 00	;讀到底了
		je      inc_index
		int_10h:	int 	10h
		inc     si
		jmp 	nextchar	;印下一個字元

inc_index:
		;mov  al, 32
		;int  10h
		inc  index2			;如果再按一次del，就print 下一個string

;
exit:	;mov     index2 , si		;結束中斷
		pop		ds
		pop		di
		pop		si
		pop		dx	
		pop		cx
		pop		bx
		pop		ax
		pop		bp
		db		0eah
_offset	dw		0000h
_segment	dw	0000h
;
init:	;mov     di,0			;安裝TSR的部分
		mov		ax,3509h
		int		21h
		mov		ax,@code
		mov		ds,ax
		mov		_offset,bx
		mov		_segment,es
		mov		dx,offset main
		mov		ax,2509h
		int		21h
		mov		dx,SEG	zseg
		sub		dx,bx
		mov		ah,31h
		int		21h
		.STACK
zseg	SEGMENT
zseg	ENDS
		END		start