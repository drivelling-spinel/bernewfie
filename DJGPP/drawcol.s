//================
//
// R_DrawColumn
//
//================
//
// 2/15/98 Lee Killough
//
// Converted C code with TFE fix to assembly and tuned
//
// 2/21/98 killough: added translucency support
//
// 11/98 killough: added hires support
//
//================

 .text
 .align 8
 .globl _R_DrawColumn
_R_DrawColumn:
 pushl %ebp
 pushl %esi
 pushl %edi
 pushl %ebx
 movl _dc_yh,%esi
 movl _dc_yl,%edx
 movl _dc_x,%eax
 incl %esi
 movl _ylookup(,%edx,4),%ebx
 subl %edx,%esi
 jle end
 addl _columnofs(,%eax,4),%ebx
 movl _dc_texheight,%eax
 subl _centery,%edx
 movl _dc_source,%ebp
 imull _dc_iscale,%edx
 leal -1(%eax),%ecx
 movl _dc_colormap,%edi
 addl _dc_texturemid,%edx

 cmpl $0, _hires
 jne hi

 testl %eax,%ecx
 je powerof2
 sall $16,%eax

red1:
 subl %eax,%edx
 jge red1

red2:
 addl %eax,%edx
 jl red2

 .align 8,0x90
nonp2loop:
 movl %edx,%ecx
 sarl $16,%ecx
 addl _dc_iscale,%edx
 movzbl (%ecx,%ebp),%ecx
 movb (%edi,%ecx),%cl
 movb %cl,(%ebx)
 addl $320,%ebx
 cmpl %eax,%edx
 jge wraparound
 decl %esi
 jg nonp2loop
 popl %ebx
 popl %edi
 popl %esi
 popl %ebp
 ret

 .align 8
wraparound:
 subl %eax,%edx
 decl %esi
 jg nonp2loop
 popl %ebx
 popl %edi
 popl %esi
 popl %ebp
 ret

 .align 8
end:
 popl %ebx
 popl %edi
 popl %esi
 popl %ebp
 ret

 .align 8
p2loop:
 movl %edx,%eax
 sarl $16,%eax
 addl _dc_iscale,%edx
 andl %ecx,%eax
 movzbl (%eax,%ebp),%eax
 movb (%eax,%edi),%al
 movb %al,(%ebx)
 movl %edx,%eax
 addl _dc_iscale,%edx
 sarl $16,%eax
 andl %ecx,%eax
 movzbl (%eax,%ebp),%eax
 movb (%eax,%edi),%al
 movb %al,320(%ebx)
 addl $640,%ebx

powerof2:
 addl $-2,%esi
 jge p2loop
 jnp end
 sarl $16,%edx
 andl %ecx,%edx
 xorl %eax,%eax
 movb (%edx,%ebp),%al
 movb (%eax,%edi),%al
 movb %al,(%ebx)
 popl %ebx
 popl %edi
 popl %esi
 popl %ebp
 ret

// killough 11/98: add hires support

 .align 8
hi:
 testl %eax,%ecx
 je powerof2_hi
 sall $16,%eax

red1_hi:
 subl %eax,%edx
 jge red1_hi

red2_hi:
 addl %eax,%edx
 jl red2_hi

 .align 8,0x90
nonp2loop_hi:
 movl %edx,%ecx
 sarl $16,%ecx
 addl _dc_iscale,%edx
 movzbl (%ecx,%ebp),%ecx
 movb (%edi,%ecx),%cl
 movb %cl,(%ebx)
 addl $640,%ebx
 cmpl %eax,%edx
 jge wraparound_hi
 decl %esi
 jg nonp2loop_hi
 popl %ebx
 popl %edi
 popl %esi
 popl %ebp
 ret

 .align 8
wraparound_hi:
 subl %eax,%edx
 decl %esi
 jg nonp2loop_hi
 popl %ebx
 popl %edi
 popl %esi
 popl %ebp
 ret

 .align 8
p2loop_hi:
 movl %edx,%eax
 sarl $16,%eax
 addl _dc_iscale,%edx
 andl %ecx,%eax
 movzbl (%eax,%ebp),%eax
 movb (%eax,%edi),%al
 movb %al,(%ebx)
 movl %edx,%eax
 addl _dc_iscale,%edx
 sarl $16,%eax
 andl %ecx,%eax
 movzbl (%eax,%ebp),%eax
 addl $1280,%ebx
 movb (%eax,%edi),%al
 movb %al,-640(%ebx)

powerof2_hi:
 addl $-2,%esi
 jge p2loop_hi
 jnp end
 sarl $16,%edx
 andl %ecx,%edx
 xorl %eax,%eax
 movb (%edx,%ebp),%al
 movb (%eax,%edi),%al
 movb %al,(%ebx)
 popl %ebx
 popl %edi
 popl %esi
 popl %ebp
 ret

//----------------------------------------------------------------------------
// $Id: drawcol.s,v 1.1.1.1 2000-07-29 13:20:39 fraggle Exp $
//----------------------------------------------------------------------------
//
// $Log: drawcol.s,v $
// Revision 1.1.1.1  2000-07-29 13:20:39  fraggle
// imported sources
//
// Revision 1.3  1998/03/04  12:33:29  killough
// Fix problem with last translucent pixel being drawn
//
// Revision 1.2  1998/02/23  04:18:24  killough
// Add translucency support, more tuning
//
// Revision 1.1  1998/02/17  06:37:37  killough
// Initial version
//
//
//----------------------------------------------------------------------------

