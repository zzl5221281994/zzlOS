;/************************************************************************************
;Thu Mar 31 15:04:03 2016
;
;MIT License
;Copyright (c) 2016 zhuzuolang
;
;Permission is hereby granted, free of charge, to any person obtaining a copy
;of this software and associated documentation files (the "Software"), to deal
;in the Software without restriction, including without limitation the rights
;to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;copies of the Software, and to permit persons to whom the Software is
;furnished to do so, subject to the following conditions:
;The above copyright notice and this permission notice shall be included in all
;copies or substantial portions of the Software.
;THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;SOFTWARE.
;************************************************************************************/
;����������8�ֽ�

;  BYTE7         BYTE6 BYTE5                               BYTE4 BYTE3 BYTE2    BYTE1 BYTE0
;  �λ�ַ2     /   ������    \                              |    �λ�ַ1    |      �ν���
;  31~24      /               \                                23  ~    0           15~0
;            /                 \
;           /                   \
;         7   6   5  4       3 2 1 0    7  6 5  4  3 2 1 0
;         G D/B   0  AVL    |�ν���2|   P  DPL  S   TYPE
;                             19~16
;Pλ������λ���Ƿ����ڴ棬1��ʾ����     Gλ���ν������ȣ�0��ʾ����Ϊ�ֽڣ�1��ʾ����Ϊ4KB
;DPL����Ȩ����0~3 ��ԽС��Ȩ��Խ��      Sλ��S=1��ʾ���ݶ�/����Σ�S=0��ʾϵͳ��/��������
;D/B��   
;
;
;
;
;
;�洢������������
SegDesc_Property_4KB  			EQU		8000H  ;4KB����
SegDesc_Property_32				EQU		4000H  ;32λ��

SegDesc_Property_DPL0			EQU		0000H  ;DPL����Ȩ��=0
SegDesc_Property_DPL1			EQU		0020H  ;DPL����Ȩ��=1
SegDesc_Property_DPL2			EQU		0040H  ;DPL����Ȩ��=2
SegDesc_Property_DPL3			EQU     0060H  ;DPL����Ȩ��=3

SegDesc_Property_R 				EQU		0090H  ;���ڵ�ֻ�����ݶ�	
SegDesc_Property_RW				EQU		0092H  ;���ڵĿɶ�д���ݶ�
SegDesc_Property_RWA			EQU     0093H  ;���ڵ��ѷ��ʿɶ�д���ݶ�	
SegDesc_Property_EXEC			EQU		0098H  ;���ڵ�ִֻ�д����	
SegDesc_Property_EXEC_R 		EQU		009AH  ;���ڵĿ�ִ�пɶ������		
SegDesc_Property_EXEC_C			EQU		009CH  ;���ڵĿ�ִ��һ�´����	
SegDesc_Property_EXEC_RC		EQU		009EH  ;���ڵĿɶ���ִ��һ�´����	
;ϵͳ��������
SegDesc_Property_LDT			EQU		0082H  ;�ֲ�����������
SegDesc_Property_TaskGate		EQU		0085H  ;�����Ŷ�
SegDesc_Property_386TSS			EQU     0089H  ;386����״̬��
SegDesc_Property_386CGate		EQU		008CH  ;386�����Ŷ�
SegDesc_Property_386IGate		EQU		008EH  ;386�ж��Ŷ�
SegDesc_Property_TGate			EQU		008FH  ;386�����Ŷ�

;CRO:
;|----------------------------------------------------------------------|
;|   31 30 29--------------18 17 16----------------5  4  3  2  1  0     |
;|   PG CD NW              AM    WP                NE ET TS EM MP PE    |
;|----------------------------------------------------------------------|
;******************************************************************************************/
;
;
;/*****************************************************************************************
; ѡ����ͼʾ:
;         �������ש����ש����ש����ש����ש����ש����ש����ש����ש����ש����ש����ש����ש����ש����ש�����
;         �� 15 �� 14 �� 13 �� 12 �� 11 �� 10 �� 9  �� 8  �� 7  �� 6  �� 5  �� 4  �� 3  �� 2  �� 1  �� 0  ��
;         �ǩ����ߩ����ߩ����ߩ����ߩ����ߩ����ߩ����ߩ����ߩ����ߩ����ߩ����ߩ����ߩ����贈���贈���ߩ�����
;         ��                                 ����������                                 �� TI ��   RPL    ��
;         �������������������������������������������������������������������������������ߩ����ߩ�����������
;
; RPL(Requested Privilege Level): ������Ȩ����������Ȩ��顣
;
; TI(Table Indicator): ������������ָʾλ
;	TI=0 ָʾ��ȫ����������GDT�ж�ȡ��������
;	TI=1 ָʾ�Ӿֲ���������LDT�ж�ȡ��������
;

;----------------------------------------------------------------------------
; ѡ��������ֵ˵��
; ����:
;*****************************************************************************************/

Selector_RPL0		EQU 	0	; ��
Selector_RPL1		EQU 	1	; �� RPL
Selector_RPL2		EQU 	2	; ��
Selector_RPL3		EQU 	3	; ��

Selector_TI_Gdt		EQU	0	; ��TI
Selector_TI_Ldt		EQU	4	; ��

KERNEL_SIZE						EQU		18000
;�ڴ�ֲ�ͼ���ݽṹ���