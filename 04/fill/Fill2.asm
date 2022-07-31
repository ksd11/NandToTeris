// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

//set data
//base 设置屏幕的像素块的位置
@SCREEN
D=A
@base
M=D

//像素点编码
@cnt
M=1

@32768
D=A
@pix1
M=D

@49152
D=A
@pix2
M=D

@57344
D=A
@pix3
M=D

@61440
D=A
@pix4
M=D

@63488
D=A
@pix5
M=D

@64512
D=A
@pix6
M=D

@65024
D=A
@pix7
M=D

@65280
D=A
@pix8
M=D

@65408
D=A
@pix9
M=D

@65472
D=A
@pix10
M=D

@65504
D=A
@pix11
M=D

@65520
D=A
@pix12
M=D

@65528
D=A
@pix13
M=D

@65532
D=A
@pix14
M=D

@65534
D=A
@pix15
M=D

@65535
D=A
@pix16
M=D

(LOOP)
  @KBD
  D=M 
  @DELETE
  D; JEQ

(ADD)  
  @base
  D=M
  @KBD
  D=D-A
  @LOOP
  D; JEQ
  @base
  A=M
  M=-1
  @base
  M=M+1

  @LOOP
  0; JMP


(DELETE)
  @base
  D=M
  @LOOP
  D; JEQ
  @base
  A=M
  M=0
  @base
  M=M-1

  @LOOP
  0; JMP

