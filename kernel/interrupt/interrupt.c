/************************************************************************************
Wed Apr 13 15:38:40 2016

MIT License
Copyright (c) 2016 zhuzuolang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
************************************************************************************/
#include "F:\work\tolset\tinyOS\kernel\lib\global.h          "
#include "F:\work\tolset\tinyOS\kernel\debug\debug.h         "
#include "F:\work\tolset\tinyOS\kernel\bootInfo\bootInfo.h   "
#include "F:\work\tolset\tinyOS\kernel\kernelFun.h           "
#include "F:\work\tolset\tinyOS\kernel\graphics\font.h       "
#include "F:\work\tolset\tinyOS\kernel\IO\IO.h               "
#include "F:\work\tolset\tinyOS\kernel\multiTask\process.h   "
#include "F:\work\tolset\tinyOS\kernel\multiTask\message.h   "
#include "F:\work\tolset\tinyOS\kernel\drivers\hd.h          "
#include "interrupt.h                                        "
#include "sys_call.h                                         "
#define EXCEPTION_HANDERS_NUM 20
#define INTERRUPT_HANDERS_NUM 16
#define DEFAULT_ERRORCODE     8888

//全局锁,允许除了时钟中断外的所有中断和系统调用嵌套重入
PUBLIC u_int32 kernel_mutex=0;
//CPU exception
PRIVATE void divide_exception               (void){
	sys_exception_panic(DEFAULT_ERRORCODE,0);
}
PRIVATE void debug_exception                (void){
	sys_exception_panic(DEFAULT_ERRORCODE,1);
}

PRIVATE void nMask_exception                (void){
	sys_exception_panic(DEFAULT_ERRORCODE,2);
}
PRIVATE void breakPoint_exception           (void){
	sys_exception_panic(DEFAULT_ERRORCODE,3);
}

PRIVATE void overflow_exception             (void){
	sys_exception_panic(DEFAULT_ERRORCODE,4);
}
PRIVATE void overBounds_exception           (void){
	sys_exception_panic(DEFAULT_ERRORCODE,5);
}
PRIVATE void invalid_op_exception           (void){
	sys_exception_panic(DEFAULT_ERRORCODE,6);
}
PRIVATE void device_not_available_exception (void){
	sys_exception_panic(DEFAULT_ERRORCODE,7);
}
PRIVATE void double_fault_exception         (void){
	u_int32 errorCode=getErrorCode();
	sys_exception_panic(errorCode,8);
}
PRIVATE void coprocessor_seg_over_exception (void){
	sys_exception_panic(DEFAULT_ERRORCODE,9);
}
PRIVATE void invalid_TSS_exception          (void){
	u_int32 errorCode=getErrorCode();
	sys_exception_panic(errorCode,10);
}
PRIVATE void seg_not_present_exception      (void){
	u_int32 errorCode=getErrorCode();
	sys_exception_panic(errorCode,11);
}
PRIVATE void stack_seg_exception            (void){
	u_int32 errorCode=getErrorCode();
	sys_exception_panic(errorCode,12);
}
PRIVATE void general_protection_exception   (void){
	u_int32 errorCode=getErrorCode();
	sys_exception_panic(errorCode,13);
}
PRIVATE void page_fault_exception           (void){
	u_int32 errorCode=getErrorCode();
	sys_exception_panic(errorCode,14);
}
PRIVATE void reserved_exception             (void){
	sys_exception_panic(DEFAULT_ERRORCODE,15);
}
PRIVATE void x87FPU_exception               (void){
	sys_exception_panic(DEFAULT_ERRORCODE,16);
}
PRIVATE void parallel_check_exception       (void){
	u_int32 errorCode=getErrorCode();
	sys_exception_panic(errorCode,17);
}
PRIVATE void machine_check_exception        (void){
	sys_exception_panic(DEFAULT_ERRORCODE,18);
}
PRIVATE void SIMD_exception                 (void){
	sys_exception_panic(DEFAULT_ERRORCODE,19);
}

//PIC interrupt
// master 8259A
//#define HD_DRIVER 1
//#define FILE_SYS  2
//#define WINDOW    3
//#define SYSTEM    4


PUBLIC void IRQ0_clock1                     (void){
	global_clock++;
	process_table[current_exec_pid].status=STATUS_NO_TIME;
	sendEOI_Master();
	schedule();
}
PUBLIC void IRQ1_keyBoard1                  (void){
	u_int8 byte=io_in8(0x60);
	sendEOI_Master ();
	keyBoard_inPut_buf[keyBoard_bufLen++]=byte;
	if(keyBoard_bufLen>MAX_KEYBOARD_BUF)
		keyBoard_bufLen=0;
	keyBoard_inPut_buf[keyBoard_bufLen]='\0';
	//test
	//drawNum(keyBoard_bufLen,0,0,0x3c,0x00);
  // drawStr(keyBoard_inPut_buf,200,0,0x3c,0x00);
}
PUBLIC void IRQ2_slave1                     (void){
	sendEOI_Master ();
}
PUBLIC void IRQ3_port21                     (void){
	sendEOI_Master ();
}
PUBLIC void IRQ4_port11                     (void){
	sendEOI_Master ();
}
PUBLIC void IRQ5_LPT21                      (void){
	sendEOI_Master ();
}
PUBLIC void IRQ6_floppyDisk1                (void){
	sendEOI_Master ();
}
PUBLIC void IRQ7_LPT11                      (void){
	sendEOI_Master ();
}
// slave 8259A
PUBLIC void IRQ8_CMOS1                      (void){
	sendEOI_Slave  ();
	sendEOI_Master ();
}
PUBLIC void IRQ9_redirect_IRQ21             (void){
	sendEOI_Slave  ();
	sendEOI_Master ();
}
PUBLIC void IRQ10_reserved11                (void){
	sendEOI_Slave  ();
	sendEOI_Master ();
}
PUBLIC void IRQ11_reserved21                (void){
	sendEOI_Slave  ();
	sendEOI_Master ();
}
/*标记鼠标状态*/
u_int32 mousePhase=0;
PUBLIC void IRQ12_PS2Mouse1                 (void){
	u_int8 byte=io_in8(0x60);
	sendEOI_Slave  ();
	sendEOI_Master ();
	if(mousePhase==0)
	{
		mousePhase=1;
		return;
	}
	if(mousePhase==1)/*循环缓冲区*/
	{
		mouse_inPut_buf[mouse_write_pos++]=byte;
		mousePhase=2;
		if(mouse_write_pos==MAX_MOUSE_BUF)
			mouse_write_pos=0;
		return;
	}
	if(mousePhase==2)
	{
		mouse_inPut_buf[mouse_write_pos++]=byte;
		mousePhase=3;
		if(mouse_write_pos==MAX_MOUSE_BUF)
			mouse_write_pos=0;
		return;
	}
	if(mousePhase==3)
	{
		mouse_inPut_buf[mouse_write_pos++]=byte;
		mousePhase=1;
		if(mouse_write_pos==MAX_MOUSE_BUF)
			mouse_write_pos=0;
	/*收满3字节，向WINDOW服务发送鼠标移动消息*/
	struct MESSAGE msg;
	struct INT_MSG intMsg;
	intMsg.intNo=12;
	intMsg.status=mouse_read_pos;
	make_msg(&msg,0,3,INT_MSG_TYPE,BLOCK_NOT_NEED);
	msg.u.msg_int=intMsg;
	send_msg(&msg,0);
	mouse_read_pos+=3;
	mouse_read_pos=mouse_read_pos%MAX_MOUSE_BUF;
	return;
	}
	//test
	//int i;
	//for(i=0;i<mouse_write_pos;i++)
	//	drawNum(mouse_inPut_buf[i],16*((32*i)/1024),(i*32)%1024,0x3c,0x00);
	
}
PUBLIC void IRQ13_FPU_error1                (void){
	sendEOI_Slave  ();
	sendEOI_Master ();
}
PUBLIC void IRQ14_ATDisk1                   (void){
	sendEOI_Slave  ();
	sendEOI_Master ();
	u_int8 status=io_in8(REG_STATUS);
	/*此处应该加入对状态的错误与否来发送消息*/
	struct MESSAGE msg;
	struct INT_MSG intMsg;
	intMsg.intNo=14;
	intMsg.status=status;
	make_msg(&msg,0,HD_DRIVER,INT_MSG_TYPE,BLOCK_NOT_NEED);
	msg.u.msg_int=intMsg;
	send_msg(&msg,0);
}
PUBLIC void IRQ15_reserved31                (void){
	sendEOI_Slave  ();
	sendEOI_Master ();
}

//异常处理指针数组
PUBLIC u_int32 exception_hander[EXCEPTION_HANDERS_NUM]={
	                               (u_int32)divide_exception,
                                   (u_int32)debug_exception,
								   (u_int32)nMask_exception,
								   (u_int32)breakPoint_exception,
								   (u_int32)overflow_exception,
								   (u_int32)overBounds_exception,
								   (u_int32)invalid_op_exception,
								   (u_int32)device_not_available_exception,
								   (u_int32)double_fault_exception,
								   (u_int32)coprocessor_seg_over_exception,
								   (u_int32)invalid_TSS_exception,
								   (u_int32)seg_not_present_exception,
								   (u_int32)stack_seg_exception,
								   (u_int32)general_protection_exception,
								   (u_int32)page_fault_exception,
								   (u_int32)reserved_exception,
								   (u_int32)x87FPU_exception,
								   (u_int32)parallel_check_exception,
								   (u_int32)machine_check_exception,
								   (u_int32)SIMD_exception};
//中断处理指针数组
PUBLIC u_int32 interrupt_hander[INTERRUPT_HANDERS_NUM]={
	                                                   (u_int32)IRQ0_clock,
                                                       (u_int32)IRQ1_keyBoard,
													   (u_int32)IRQ2_slave,
													   (u_int32)IRQ3_port2,
													   (u_int32)IRQ4_port1,
													   (u_int32)IRQ5_LPT2,
													   (u_int32)IRQ6_floppyDisk,
													   (u_int32)IRQ7_LPT1,
													   (u_int32)IRQ8_CMOS,
													   (u_int32)IRQ9_redirect_IRQ2,
													   (u_int32)IRQ10_reserved1,
													   (u_int32)IRQ11_reserved2,
													   (u_int32)IRQ12_PS2Mouse,
													   (u_int32)IRQ13_FPU_error,
													   (u_int32)IRQ14_ATDisk,
													   (u_int32)IRQ15_reserved3};
													   
													   