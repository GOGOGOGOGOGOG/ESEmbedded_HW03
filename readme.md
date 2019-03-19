HW03
===
This is the hw03 sample. Please follow the steps below.

# Build the Sample Program

1. Fork this repo to your own github account.

2. Clone the repo that you just forked.

3. Under the hw03 dir, use:

	* `make` to build.

	* `make clean` to clean the ouput files.

4. Extract `gnu-mcu-eclipse-qemu.zip` into hw03 dir. Under the path of hw03, start emulation with `make qemu`.

	See [Lecture 02 ─ Emulation with QEMU] for more details.

5. The sample is a minimal program for ARM Cortex-M4 devices, which enters `while(1);` after reset. Use gdb to get more details.

	See [ESEmbedded_HW02_Example] for knowing how to do the observation and how to use markdown for taking notes.

# Build Your Own Program

1. Edit main.c.

2. Make and run like the steps above.

3. Please avoid using hardware dependent C Standard library functions like `printf`, `malloc`, etc.

# HW03 Requirements

1. How do C functions pass and return parameters? Please describe the related standard used by the Application Binary Interface (ABI) for the ARM architecture.

2. Modify main.c to observe what you found.

3. You have to state how you designed the observation (code), and how you performed it.

	Just like how you did in HW02.

3. If there are any official data that define the rules, you can also use them as references.

4. Push your repo to your github. (Use .gitignore to exclude the output files like object files or executable files and the qemu bin folder)

[Lecture 02 ─ Emulation with QEMU]: http://www.nc.es.ncku.edu.tw/course/embedded/02/#Emulation-with-QEMU
[ESEmbedded_HW02_Example]: https://github.com/vwxyzjimmy/ESEmbedded_HW02_Example

--------------------

- [x] **If you volunteer to give the presentation next week, check this.**

--------------------

**★★★ Please take your note here ★★★**
# HW03 作業心得

1. 實驗題目：
撰寫組合語言觀察其中的變化以及記憶體和sp之間的關於傳遞參數和不同狀態變數下的變化，包括對於pointer integer pointer to pointer等等操作

2. 實驗步驟：
  - 先將資料夾 gnu-mcu-eclipse-qemu 完整複製到 ESEmbedded_HW03 資料夾中
  - 安裝cross compiler和GNU Debuger for ARM
  - 熟讀ARM ArchitectureReference ManualThumb-2 Supplement
3. 重要手冊資訊
本次作業最重要的是了解關於Core registers，parameter等指令在處理器中的影響和sp的變化
- 以 Core registers 來說：
```
The first four registers r0-r3 (a1-a4) are used to pass argument values into a subroutine and to return a result value from a function. They may also be used to hold intermediate values within a routine (but, in general, only between subroutine calls).
```
- 以parameter來說：
```
The base standard provides for passing arguments in core registers (r0-r3) and on the stack. For subroutines that take a small number of parameters, only registers are used, greatly reducing the overhead of a call.
```
## main.c檔設計：
本次作業我想測試關於pointer to pointer和pointer相關對於全域變數和區域變數之間的參數以及位址傳遞。於是我將main.c做了以下的更改：
```
int B=5;
int D=5;
int func(int *p) { 
   // int a;
      p = &B;  
}
/*
int add (int *b_add,int *c_add,int *cnt_add,int *sum)
{
*sum = *b_add+*c_add;
*b_add+=1;
*c_add+=9;
*cnt_add+=1;
return 0;
}
*/

int funD(int **p1) { 
   // int b;
    *p1 = &D;
}
void reset_handler(void)
{
//int b=15 ;
//int c=16;
//int cnt;
//int sum;
int A = 1;
int C = 3;
int a=0;
int c=0; //A = r3 

int *ptrA = &A;
int *ptrB = &C;

   
   func(&ptrA);  
   funD(&ptrB);
   a = *ptrA; 
   c = *ptrB;
   /*
	while(cnt<=3)
    {
   add(&b,&c,&cnt,&sum);
	}
   */	
}

```
ptrA和ptrB是整數指標，其目的是為了接收A和C的位址，而func和funD是一個int函式各以pointer 和pointer to pointer 做接收，而在func中我們試圖改變p內所指向的位址，但是一旦回到我們的reset_handler函式中後，其值仍舊不會改變，主要原因是在於我們的指標內容會在回到函式後重新變回A的位址，以assembly語言來說：
**只用pointer**
```
00000008 <func>:
   8:   b480            push    {r7}
   a:   b083            sub     sp, #12
   c:   af00            add     r7, sp, #0
   e:   6078            str     r0, [r7, #4] //ptrA的位址給r0並且存在r7+4的位址中 （200000d4）
  10:   4b03            ldr     r3, [pc, #12]   ; (20 <func+0x18>) //把全域變數的位址給r3
  12:   607b            str     r3, [r7, #4]  //把r3的值給(r7+4)的位址中複寫過去
  14:   4618            mov     r0, r3 
  16:   370c            adds    r7, #12
  18:   46bd            mov     sp, r7
  1a:   f85d 7b04       ldr.w   r7, [sp], #4 //回復成原來舊r7的位址 200000e0
  1e:   4770            bx      lr
  20:   00000088        andeq   r0, r0, r8, lsl #1

```
**使用pointer to pointer **


```
00000024 <funD>:
  24:   b480            push    {r7}
  26:   b083            sub     sp, #12
  28:   af00            add     r7, sp, #0
  2a:   6078            str     r0, [r7, #4] //ptrB的位址給r0並且存在r7+4的位址中 （200000d4）
  2c:   687b            ldr     r3, [r7, #4] //把r7+4的位址給r3
  2e:   4a04            ldr     r2, [pc, #16]   ; (40 <funD+0x1c>) //r2接收了全域變數的位址
  30:   601a            str     r2, [r3, #0] //r2的位址給r3+0並且存在內存中
  32:   4618            mov     r0, r3 
  34:   370c            adds    r7, #12
  36:   46bd            mov     sp, r7
  38:   f85d 7b04       ldr.w   r7, [sp], #4
  3c:   4770            bx      lr
  3e:   bf00            nop
  40:   0000008c        andeq   r0, r0, ip, lsl #1
```
其概念以簡單一點得觀念來說，就是我們讓指標中途改變方向了，亦即多了一條路徑給它使它不會只回原本的A位址而是指向後來的全域變數B，如圖：
**尚未回到handler函式前：**
![image](https://github.com/GOGOGOGOGOGOG/ESEmbedded_HW03/blob/master/2019-03-19%2017-54-27%20%E7%9A%84%E8%9E%A2%E5%B9%95%E6%93%B7%E5%9C%96.png)

**回到函式後：**
![image](https://github.com/GOGOGOGOGOGOG/ESEmbedded_HW03/blob/master/2019-03-19%2017-54-34%20%E7%9A%84%E8%9E%A2%E5%B9%95%E6%93%B7%E5%9C%96.png)

**r0和r3變回原樣**
![image](https://github.com/GOGOGOGOGOGOG/ESEmbedded_HW03/blob/master/2019-03-19%2017-54-47%20%E7%9A%84%E8%9E%A2%E5%B9%95%E6%93%B7%E5%9C%96.png)

**使用pointer to pointer之結果**
![image](https://github.com/GOGOGOGOGOGOG/ESEmbedded_HW03/blob/master/2019-03-19%2018-19-14%20%E7%9A%84%E8%9E%A2%E5%B9%95%E6%93%B7%E5%9C%96.png)

**使用pointer之結果**
![image](https://github.com/GOGOGOGOGOGOG/ESEmbedded_HW03/blob/master/2019-03-19%2018-25-15%20%E7%9A%84%E8%9E%A2%E5%B9%95%E6%93%B7%E5%9C%96.png)

相關assembly的程式解釋：
```
00000008 <func>:
   8:   b480            push    {r7}
   a:   b083            sub     sp, #12
   c:   af00            add     r7, sp, #0
   e:   6078            str     r0, [r7, #4] //ptrA的位址給r0並且存在r7+4的位址中 （200000d4）
  10:   4b03            ldr     r3, [pc, #12]   ; (20 <func+0x18>) //把全域變數的位址給r3
  12:   607b            str     r3, [r7, #4]  //把r3的值給(r7+4)的位址中複寫過去
  14:   4618            mov     r0, r3 
  16:   370c            adds    r7, #12
  18:   46bd            mov     sp, r7
  1a:   f85d 7b04       ldr.w   r7, [sp], #4 //回復成原來舊r7的位址 200000e0
  1e:   4770            bx      lr
  20:   00000088        andeq   r0, r0, r8, lsl #1

00000024 <funD>:
  24:   b480            push    {r7}
  26:   b083            sub     sp, #12
  28:   af00            add     r7, sp, #0
  2a:   6078            str     r0, [r7, #4] //ptrB的位址給r0並且存在r7+4的位址中 （200000d4）
  2c:   687b            ldr     r3, [r7, #4] //把r7+4的位址給r3
  2e:   4a04            ldr     r2, [pc, #16]   ; (40 <funD+0x1c>) //r2接收了全域變數的位址
  30:   601a            str     r2, [r3, #0] //r2的位址給r3+0並且存在內存中
  32:   4618            mov     r0, r3 
  34:   370c            adds    r7, #12
  36:   46bd            mov     sp, r7
  38:   f85d 7b04       ldr.w   r7, [sp], #4
  3c:   4770            bx      lr
  3e:   bf00            nop
  40:   0000008c        andeq   r0, r0, ip, lsl #1

00000044 <reset_handler>:
  44:   b580            push    {r7, lr}   //保留r7到內存的stack Keep the callee saved registers 一旦調用子函數 我們就需要lr來保存返回值
  46:   b086            sub     sp, #24    //s_stack_frame_size 和宣告幾個變數有關(6*4)
  48:   af00            add     r7, sp, #0 // adds the current value of the stack pointer i.e =>the address of the current stack top
  4a:   2301            movs    r3, #1     //將1的值放入r3暫存器中
  4c:   60fb            str     r3, [r7, #12]  //表示將r3的值存入(r7-12)所指向的位址中 （表 1）  從寄存器搬到內存(stack)中
  4e:   2303            movs    r3, #3  //將3的值放入r3暫存器中 
  50:   60bb            str     r3, [r7, #8] //表示將r3的值存入(r7-8)所指向的位址中 （表 3） 從寄存器搬到內存(stack)中
  52:   2300            movs    r3, #0 //將0的值放入r3暫存器中
  54:   617b            str     r3, [r7, #20] //表示將r3的值存入(r7-8)所指向的位址中 （表 0） 從寄存器搬到內存(stack)中
  56:   2300            movs    r3, #0 //將0的值放入r3暫存器中
  58:   613b            str     r3, [r7, #16] //表示將r3的值存入(r7-8)所指向的位址中 （表 0） 從寄存器搬到內存(stack)中
  5a:   f107 030c       add.w   r3, r7, #12  //*ptrA = &A; （guess）因為位址超過12位元 故須使用add.w ？
  5e:   607b            str     r3, [r7, #4] // 將ptrA從寄存器搬到內存中
  60:   f107 0308       add.w   r3, r7, #8 //*ptrB = &B;
  64:   603b            str     r3, [r7, #0] //將ptrB從寄存器搬到內存中
  66:   1d3b            adds    r3, r7, #4  //還原回去找ptrA所在的位址
  68:   4618            mov     r0, r3 //reset_handler透過r0傳遞值
  6a:   f7ff ffcd       bl      8 <func> //呼叫func
  6e:   463b            mov     r3, r7 //因為r7又變回原來的位址了 所以r3不再是指向全域變數
  70:   4618            mov     r0, r3 //r0也不再指向全域變數
  72:   f7ff ffd7       bl      24 <funD>
  76:   687b            ldr     r3, [r7, #4]
  78:   681b            ldr     r3, [r3, #0]  //呼叫內存的值
  7a:   617b            str     r3, [r7, #20]
  7c:   683b            ldr     r3, [r7, #0]
  7e:   681b            ldr     r3, [r3, #0]
  80:   613b            str     r3, [r7, #16] 
  82:   3718            adds    r7, #24
  84:   46bd            mov     sp, r7
86: bd80 pop {r7, pc}
```


