/*	$OpenBSD: kbd_tables.c,v 1.2 1996/04/18 23:48:16 niklas Exp $	*/
/*	$NetBSD: kbd_tables.c,v 1.2 1996/02/29 19:32:18 gwr Exp $	*/

/*
 * Copyright (c) 1996 Gordon W. Ross
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Gordon Ross
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Keyboard translation tables.  (See kbd_tables.h)
 */

#define	_KERNEL 1
#include "kbd_tables.h"


/*
 * Toggle keys are not yet supported, but could be with
 * fairly simple changes.  Both CapsLock and NumLock
 * could be easily done with "post-translation" on the
 * keysymbols from these tables (WITHOUT new tables).
 *
 * The "compose" operations are not implemented.
 */

#define	KEYSYM_COMPOSE KEYSYM_NOP


/*
 * Key release codes are decoded in this map.
 */
struct keymap keymap_release = {
    /*   0:             */	KEYSYM_HOLE,
    /*   1: L1/Stop     */	KEYSYM_NOP,
    /*   2:             */	KEYSYM_HOLE,
    /*   3: L2/Again    */	KEYSYM_NOP,
    /*   4:             */	KEYSYM_HOLE,
    /*   5: F1          */	KEYSYM_NOP,
    /*   6: F2          */	KEYSYM_NOP,
    /*   7: F10         */	KEYSYM_NOP,
    /*   8: F3          */	KEYSYM_NOP,
    /*   9: F11         */	KEYSYM_NOP,
    /*  10: F4          */	KEYSYM_NOP,
    /*  11: F12         */	KEYSYM_NOP,
    /*  12: F5          */	KEYSYM_NOP,
    /*  13: AltGraph    */	KEYSYM_CLRMOD | KBMOD_ALTGRAPH,
    /*  14: F6          */	KEYSYM_NOP,
    /*  15:             */	KEYSYM_HOLE,
    /*  16: F7          */	KEYSYM_NOP,
    /*  17: F8          */	KEYSYM_NOP,
    /*  18: F9          */	KEYSYM_NOP,
    /*  19: Alt_L       */	KEYSYM_CLRMOD | KBMOD_ALT_L,
    /*  20: T5_Up       */	KEYSYM_HOLE,
    /*  21: R1/Pause    */	KEYSYM_NOP,
    /*  22: R2/Print    */	KEYSYM_NOP,
    /*  23: R3/Break    */	KEYSYM_NOP,
    /*  24: T5_Left     */	KEYSYM_HOLE,
    /*  25: L3/Props    */	KEYSYM_NOP,
    /*  26: L4/Undo     */	KEYSYM_NOP,
    /*  27: T5_Down     */	KEYSYM_HOLE,
    /*  28: T5_Right    */	KEYSYM_HOLE,
    /*  29: Esc         */	KEYSYM_NOP,
    /*  30: 1           */	KEYSYM_NOP,
    /*  31: 2           */	KEYSYM_NOP,
    /*  32: 3           */	KEYSYM_NOP,
    /*  33: 4           */	KEYSYM_NOP,
    /*  34: 5           */	KEYSYM_NOP,
    /*  35: 6           */	KEYSYM_NOP,
    /*  36: 7           */	KEYSYM_NOP,
    /*  37: 8           */	KEYSYM_NOP,
    /*  38: 9           */	KEYSYM_NOP,
    /*  39: 0           */	KEYSYM_NOP,
    /*  40: minus       */	KEYSYM_NOP,
    /*  41: equal       */	KEYSYM_NOP,
    /*  42: grave/tilde */	KEYSYM_NOP,
    /*  43: BackSpace   */	KEYSYM_NOP,
    /*  44:             */	KEYSYM_HOLE,
    /*  45: R4/KP_Equal */	KEYSYM_NOP,
    /*  46: R5/KP_Div   */	KEYSYM_NOP,
    /*  47: R6/KP_Mult  */	KEYSYM_NOP,
    /*  48:             */	KEYSYM_NOP,
    /*  49: L5/Front    */	KEYSYM_NOP,
    /*  50: KP_Delete   */	KEYSYM_NOP,
    /*  51: L6/Copy     */	KEYSYM_NOP,
    /*  52:             */	KEYSYM_HOLE,
    /*  53: Tab         */	KEYSYM_NOP,
    /*  54: Q           */	KEYSYM_NOP,
    /*  55: W           */	KEYSYM_NOP,
    /*  56: E           */	KEYSYM_NOP,
    /*  57: R           */	KEYSYM_NOP,
    /*  58: T           */	KEYSYM_NOP,
    /*  59: Y           */	KEYSYM_NOP,
    /*  60: U           */	KEYSYM_NOP,
    /*  61: I           */	KEYSYM_NOP,
    /*  62: O           */	KEYSYM_NOP,
    /*  63: P           */	KEYSYM_NOP,
    /*  64: [           */	KEYSYM_NOP,
    /*  65: ]           */	KEYSYM_NOP,
    /*  66: Delete      */	KEYSYM_NOP,
    /*  67: Compose     */	KEYSYM_NOP,
    /*  68: R7/Home     */	KEYSYM_NOP,
    /*  69: R8/Up       */	KEYSYM_NOP,
    /*  70: R9/PgUp     */	KEYSYM_NOP,
    /*  71: KP_Minus    */	KEYSYM_NOP,
    /*  72: L7/Open     */	KEYSYM_NOP,
    /*  73: L8/Paste    */	KEYSYM_NOP,
    /*  74:             */	KEYSYM_HOLE,
    /*  75:             */	KEYSYM_HOLE,
    /*  76: Ctrl_L      */	KEYSYM_CLRMOD | KBMOD_CTRL_L,
    /*  77: A           */	KEYSYM_NOP,
    /*  78: S           */	KEYSYM_NOP,
    /*  79: D           */	KEYSYM_NOP,
    /*  80: F           */	KEYSYM_NOP,
    /*  81: G           */	KEYSYM_NOP,
    /*  82: H           */	KEYSYM_NOP,
    /*  83: J           */	KEYSYM_NOP,
    /*  84: K           */	KEYSYM_NOP,
    /*  85: L           */	KEYSYM_NOP,
    /*  86: ;           */	KEYSYM_NOP,
    /*  87: apostr.     */	KEYSYM_NOP,
    /*  88: backslash   */	KEYSYM_NOP,
    /*  89: Return      */	KEYSYM_NOP,
    /*  90: KP_Enter    */	KEYSYM_NOP,
    /*  91: R10/Left    */	KEYSYM_NOP,
    /*  92: R11/KP_5    */	KEYSYM_NOP,
    /*  93: R12/Right   */	KEYSYM_NOP,
    /*  94: KP_Insert   */	KEYSYM_NOP,
    /*  95: L9/Find     */	KEYSYM_NOP,
    /*  96:             */	KEYSYM_HOLE,
    /*  97: L10/Cut     */	KEYSYM_NOP,
    /*  98: Num_Lock    */	KEYSYM_NOP,
    /*  99: Shift_L     */	KEYSYM_CLRMOD | KBMOD_SHIFT_L,
    /* 100: Z           */	KEYSYM_NOP,
    /* 101: X           */	KEYSYM_NOP,
    /* 102: C           */	KEYSYM_NOP,
    /* 103: V           */	KEYSYM_NOP,
    /* 104: B           */	KEYSYM_NOP,
    /* 105: N           */	KEYSYM_NOP,
    /* 106: M           */	KEYSYM_NOP,
    /* 107: ,           */	KEYSYM_NOP,
    /* 108: .           */	KEYSYM_NOP,
    /* 109: /           */	KEYSYM_NOP,
    /* 110: Shift_R     */	KEYSYM_CLRMOD | KBMOD_SHIFT_R,
    /* 111: Linefeed    */	KEYSYM_NOP,
    /* 112: R13/End     */	KEYSYM_NOP,
    /* 113: R14/Down    */	KEYSYM_NOP,
    /* 114: R15/PgDn    */	KEYSYM_NOP,
    /* 115:             */	KEYSYM_HOLE,
    /* 116:             */	KEYSYM_HOLE,
    /* 117:             */	KEYSYM_HOLE,
    /* 118: L16/Help    */	KEYSYM_NOP,
    /* 119: CapsLock    */	KEYSYM_NOP,
    /* 120: Meta_L      */	KEYSYM_CLRMOD | KBMOD_META_L,
    /* 121: SpaceBar    */	KEYSYM_NOP,
    /* 122: Meta_R      */	KEYSYM_CLRMOD | KBMOD_META_R,
    /* 123:             */	KEYSYM_HOLE,
    /* 124:             */	KEYSYM_HOLE,
    /* 125: KP_Add      */	KEYSYM_NOP,
    /* 126:             */	KEYSYM_LAYOUT,	/* layout next */
    /* 127:             */	KEYSYM_RESET,	/* kbd ID next */
};


/*
 * This map is used when a control key is down.
 */
#define	CTL(c)	((c)&0x1F)
struct keymap keymap_control = {
    /*   0:             */	KEYSYM_HOLE,
    /*   1: L1/Stop     */	KEYSYM_NOP,
    /*   2:             */	KEYSYM_HOLE,
    /*   3: L2/Again    */	KEYSYM_NOP,
    /*   4:             */	KEYSYM_HOLE,
    /*   5: F1          */	KEYSYM_NOP,
    /*   6: F2          */	KEYSYM_NOP,
    /*   7: F10         */	KEYSYM_NOP,
    /*   8: F3          */	KEYSYM_NOP,
    /*   9: F11         */	KEYSYM_NOP,
    /*  10: F4          */	KEYSYM_NOP,
    /*  11: F12         */	KEYSYM_NOP,
    /*  12: F5          */	KEYSYM_NOP,
    /*  13: AltGraph    */	KEYSYM_SETMOD | KBMOD_ALTGRAPH,
    /*  14: F6          */	KEYSYM_NOP,
    /*  15:             */	KEYSYM_HOLE,
    /*  16: F7          */	KEYSYM_NOP,
    /*  17: F8          */	KEYSYM_NOP,
    /*  18: F9          */	KEYSYM_NOP,
    /*  19: Alt_L       */	KEYSYM_SETMOD | KBMOD_ALT_L,
    /*  20: T5_Up       */	KEYSYM_HOLE,
    /*  21: R1/Pause    */	KEYSYM_NOP,
    /*  22: R2/Print    */	KEYSYM_NOP,
    /*  23: R3/Break    */	KEYSYM_NOP,
    /*  24: T5_Left     */	KEYSYM_HOLE,
    /*  25: L3/Props    */	KEYSYM_NOP,
    /*  26: L4/Undo     */	KEYSYM_NOP,
    /*  27: T5_Down     */	KEYSYM_HOLE,
    /*  28: T5_Right    */	KEYSYM_HOLE,
    /*  29: Esc         */	0x1b,
    /*  30: 1           */	KEYSYM_NOP,
    /*  31: 2           */	CTL('@'),
    /*  32: 3           */	KEYSYM_NOP,
    /*  33: 4           */	KEYSYM_NOP,
    /*  34: 5           */	KEYSYM_NOP,
    /*  35: 6           */	CTL('^'),
    /*  36: 7           */	KEYSYM_NOP,
    /*  37: 8           */	KEYSYM_NOP,
    /*  38: 9           */	KEYSYM_NOP,
    /*  39: 0           */	KEYSYM_NOP,
    /*  40: minus _     */	CTL('_'),
    /*  41: equal       */	KEYSYM_NOP,
    /*  42: grave/tilde */	CTL('~'),
    /*  43: BackSpace   */	'\b',
    /*  44:             */	KEYSYM_HOLE,
    /*  45: R4/KP_Equal */	KEYSYM_NOP,
    /*  46: R5/KP_Div   */	KEYSYM_NOP,
    /*  47: R6/KP_Mult  */	KEYSYM_NOP,
    /*  48:             */	KEYSYM_NOP,
    /*  49: L5/Front    */	KEYSYM_NOP,
    /*  50: KP_Delete   */	KEYSYM_NOP,
    /*  51: L6/Copy     */	KEYSYM_NOP,
    /*  52:             */	KEYSYM_HOLE,
    /*  53: Tab         */	'\t',
    /*  54: Q           */	CTL('Q'),
    /*  55: W           */	CTL('W'),
    /*  56: E           */	CTL('E'),
    /*  57: R           */	CTL('R'),
    /*  58: T           */	CTL('T'),
    /*  59: Y           */	CTL('Y'),
    /*  60: U           */	CTL('U'),
    /*  61: I           */	CTL('I'),
    /*  62: O           */	CTL('O'),
    /*  63: P           */	CTL('P'),
    /*  64: [           */	CTL('['),
    /*  65: ]           */	CTL(']'),
    /*  66: Delete      */	0x7f,
    /*  67: Compose     */	KEYSYM_COMPOSE,
    /*  68: R7/Home     */	CTL('A'),	/* emacs */
    /*  69: R8/Up       */	CTL('P'),	/* emacs */
    /*  70: R9/PgUp     */	0x80|'v',	/* emacs */
    /*  71: KP_Minus    */	KEYSYM_NOP,
    /*  72: L7/Open     */	KEYSYM_NOP,
    /*  73: L8/Paste    */	KEYSYM_NOP,
    /*  74:             */	KEYSYM_HOLE,
    /*  75:             */	KEYSYM_HOLE,
    /*  76: Ctrl_L      */	KEYSYM_SETMOD | KBMOD_CTRL_L,
    /*  77: A           */	CTL('A'),
    /*  78: S           */	CTL('S'),
    /*  79: D           */	CTL('D'),
    /*  80: F           */	CTL('F'),
    /*  81: G           */	CTL('G'),
    /*  82: H           */	CTL('H'),
    /*  83: J           */	CTL('J'),
    /*  84: K           */	CTL('K'),
    /*  85: L           */	CTL('L'),
    /*  86: ;           */	KEYSYM_NOP,
    /*  87: apostr.     */	KEYSYM_NOP,
    /*  88: backslash   */	CTL('\\'),
    /*  89: Return      */	'\r',
    /*  90: KP_Enter    */	KEYSYM_NOP,
    /*  91: R10/Left    */	CTL('B'),	/* emacs */
    /*  92: R11/KP_5    */	KEYSYM_NOP,
    /*  93: R12/Right   */	CTL('F'),	/* emacs */
    /*  94: KP_Insert   */	KEYSYM_NOP,
    /*  95: L9/Find     */	KEYSYM_NOP,
    /*  96:             */	KEYSYM_HOLE,
    /*  97: L10/Cut     */	KEYSYM_NOP,
    /*  98: Num_Lock    */	KEYSYM_INVMOD | KBMOD_NUMLOCK,
    /*  99: Shift_L     */	KEYSYM_SETMOD | KBMOD_SHIFT_L,
    /* 100: Z           */	CTL('Z'),
    /* 101: X           */	CTL('X'),
    /* 102: C           */	CTL('C'),
    /* 103: V           */	CTL('V'),
    /* 104: B           */	CTL('B'),
    /* 105: N           */	CTL('N'),
    /* 106: M           */	CTL('M'),
    /* 107: ,           */	KEYSYM_NOP,
    /* 108: .           */	KEYSYM_NOP,
    /* 109: / ?         */	CTL('?'),
    /* 110: Shift_R     */	KEYSYM_SETMOD | KBMOD_SHIFT_R,
    /* 111: Linefeed    */	'\n',
    /* 112: R13/End     */	CTL('E'),	/* emacs */
    /* 113: R14/Down    */	CTL('N'),	/* emacs */
    /* 114: R15/PgDn    */	CTL('V'),	/* emacs */
    /* 115:             */	KEYSYM_HOLE,
    /* 116:             */	KEYSYM_HOLE,
    /* 117:             */	KEYSYM_HOLE,
    /* 118: L16/Help    */	KEYSYM_NOP,
    /* 119: CapsLock    */	KEYSYM_INVMOD | KBMOD_CAPSLOCK,
    /* 120: Meta_L      */	KEYSYM_SETMOD | KBMOD_META_L,
    /* 121: SpaceBar    */	CTL(' '),
    /* 122: Meta_R      */	KEYSYM_SETMOD | KBMOD_META_R,
    /* 123:             */	KEYSYM_HOLE,
    /* 124:             */	KEYSYM_HOLE,
    /* 125: KP_Add      */	KEYSYM_NOP,
    /* 126:             */	KEYSYM_HW_ERR,
    /* 127:             */	KEYSYM_ALL_UP,
};
#undef	CTL


/*
 * Keymaps for the "type 3" keyboard.
 * (lower-case, upper-case)
 */

struct keymap keymap_s3_lc = {
    /*   0:             */	KEYSYM_HOLE,
    /*   1: L1/Stop     */	KEYSYM_FUNC_L(1),
    /*   2:             */	KEYSYM_HOLE,
    /*   3: L2/Again    */	KEYSYM_FUNC_L(2),
    /*   4:             */	KEYSYM_HOLE,
    /*   5: F1          */	KEYSYM_FUNC_F(1),
    /*   6: F2          */	KEYSYM_FUNC_F(2),
    /*   7: F10         */	KEYSYM_HOLE,
    /*   8: F3          */	KEYSYM_FUNC_F(3),
    /*   9: F11         */	KEYSYM_HOLE,
    /*  10: F4          */	KEYSYM_FUNC_F(4),
    /*  11: F12         */	KEYSYM_HOLE,
    /*  12: F5          */	KEYSYM_FUNC_F(5),
    /*  13: AltGraph    */	KEYSYM_HOLE,
    /*  14: F6          */	KEYSYM_FUNC_F(6),
    /*  15:             */	KEYSYM_HOLE,
    /*  16: F7          */	KEYSYM_FUNC_F(7),
    /*  17: F8          */	KEYSYM_FUNC_F(8),
    /*  18: F9          */	KEYSYM_FUNC_F(9),
    /*  19: Alt_L       */	KEYSYM_SETMOD | KBMOD_ALT_L,
    /*  20: T5_Up       */	KEYSYM_HOLE,
    /*  21: R1/Pause    */	KEYSYM_FUNC_R(1),
    /*  22: R2/Print    */	KEYSYM_FUNC_R(2),
    /*  23: R3/Break    */	KEYSYM_FUNC_R(3),
    /*  24: T5_Left     */	KEYSYM_HOLE,
    /*  25: L3/Props    */	KEYSYM_FUNC_L(3),
    /*  26: L4/Undo     */	KEYSYM_FUNC_L(4),
    /*  27: T5_Down     */	KEYSYM_HOLE,
    /*  28: T5_Right    */	KEYSYM_HOLE,
    /*  29: Esc         */	0x1b,
    /*  30: 1           */	'1',
    /*  31: 2           */	'2',
    /*  32: 3           */	'3',
    /*  33: 4           */	'4',
    /*  34: 5           */	'5',
    /*  35: 6           */	'6',
    /*  36: 7           */	'7',
    /*  37: 8           */	'8',
    /*  38: 9           */	'9',
    /*  39: 0           */	'0',
    /*  40: minus       */	'-',
    /*  41: equal       */	'=',
    /*  42: grave/tilde */	'`',
    /*  43: BackSpace   */	'\b',
    /*  44:             */	KEYSYM_HOLE,
    /*  45: R4/KP_Equal */	KEYSYM_FUNC_R(4),
    /*  46: R5/KP_Div   */	KEYSYM_FUNC_R(5),
    /*  47: R6/KP_Mult  */	KEYSYM_FUNC_R(6),
    /*  48:             */	KEYSYM_HOLE,
    /*  49: L5/Front    */	KEYSYM_FUNC_L(5),
    /*  50: KP_Delete   */	KEYSYM_HOLE,
    /*  51: L6/Copy     */	KEYSYM_FUNC_L(6),
    /*  52:             */	KEYSYM_HOLE,
    /*  53: Tab         */	'\t',
    /*  54: Q           */	'q',
    /*  55: W           */	'w',
    /*  56: E           */	'e',
    /*  57: R           */	'r',
    /*  58: T           */	't',
    /*  59: Y           */	'y',
    /*  60: U           */	'u',
    /*  61: I           */	'i',
    /*  62: O           */	'o',
    /*  63: P           */	'p',
    /*  64: [           */	'[',
    /*  65: ]           */	']',
    /*  66: Delete      */	0x7f,
    /*  67: Compose     */	KEYSYM_HOLE,
    /*  68: R7/Home     */	KEYSYM_FUNC_R(7),
    /*  69: R8/Up       */	KEYSYM_STRING | 1,
    /*  70: R9/PgUp     */	KEYSYM_FUNC_R(9),
    /*  71: KP_Minus    */	KEYSYM_HOLE,
    /*  72: L7/Open     */	KEYSYM_FUNC_L(7),
    /*  73: L8/Paste    */	KEYSYM_FUNC_L(8),
    /*  74:             */	KEYSYM_HOLE,
    /*  75:             */	KEYSYM_HOLE,
    /*  76: Ctrl_L      */	KEYSYM_SETMOD | KBMOD_CTRL_L,
    /*  77: A           */	'a',
    /*  78: S           */	's',
    /*  79: D           */	'd',
    /*  80: F           */	'f',
    /*  81: G           */	'g',
    /*  82: H           */	'h',
    /*  83: J           */	'j',
    /*  84: K           */	'k',
    /*  85: L           */	'l',
    /*  86: ;           */	';',
    /*  87: apostr.     */	'\'',
    /*  88: backslash   */	'\\',
    /*  89: Return      */	'\r',
    /*  90: KP_Enter    */	KEYSYM_HOLE,
    /*  91: R10/Left    */	KEYSYM_STRING | 3,
    /*  92: R11/KP_5    */	KEYSYM_FUNC_R(11),
    /*  93: R12/Right   */	KEYSYM_STRING | 4,
    /*  94: KP_Insert   */	KEYSYM_HOLE,
    /*  95: L9/Find     */	KEYSYM_FUNC_L(9),
    /*  96:             */	KEYSYM_HOLE,
    /*  97: L10/Cut     */	KEYSYM_FUNC_L(10),
    /*  98: Num_Lock    */	KEYSYM_HOLE,
    /*  99: Shift_L     */	KEYSYM_SETMOD | KBMOD_SHIFT_L,
    /* 100: Z           */	'z',
    /* 101: X           */	'x',
    /* 102: C           */	'c',
    /* 103: V           */	'v',
    /* 104: B           */	'b',
    /* 105: N           */	'n',
    /* 106: M           */	'm',
    /* 107: ,           */	',',
    /* 108: .           */	'.',
    /* 109: /           */	'/',
    /* 110: Shift_R     */	KEYSYM_SETMOD | KBMOD_SHIFT_R,
    /* 111: Linefeed    */	'\n',
    /* 112: R13/End     */	KEYSYM_FUNC_R(13),
    /* 113: R14/Down    */	KEYSYM_STRING | 2,
    /* 114: R15/PgDn    */	KEYSYM_FUNC_R(15),
    /* 115:             */	KEYSYM_HOLE,
    /* 116:             */	KEYSYM_HOLE,
    /* 117:             */	KEYSYM_HOLE,
    /* 118: L16/Help    */	KEYSYM_HOLE,
    /* 119: CapsLock    */	KEYSYM_INVMOD | KBMOD_CAPSLOCK,
    /* 120: Meta_L      */	KEYSYM_SETMOD | KBMOD_META_L,
    /* 121: SpaceBar    */	' ',
    /* 122: Meta_R      */	KEYSYM_SETMOD | KBMOD_META_R,
    /* 123:             */	KEYSYM_HOLE,
    /* 124:             */	KEYSYM_HOLE,
    /* 125: KP_Add      */	KEYSYM_HOLE,
    /* 126:             */	KEYSYM_HW_ERR,
    /* 127:             */	KEYSYM_ALL_UP,
};


struct keymap keymap_s3_uc = {
    /*   0:             */	KEYSYM_HOLE,
    /*   1: L1/Stop     */	KEYSYM_FUNC_L(1),
    /*   2:             */	KEYSYM_HOLE,
    /*   3: L2/Again    */	KEYSYM_FUNC_L(2),
    /*   4:             */	KEYSYM_HOLE,
    /*   5: F1          */	KEYSYM_FUNC_F(1),
    /*   6: F2          */	KEYSYM_FUNC_F(2),
    /*   7: F10         */	KEYSYM_HOLE,
    /*   8: F3          */	KEYSYM_FUNC_F(3),
    /*   9: F11         */	KEYSYM_HOLE,
    /*  10: F4          */	KEYSYM_FUNC_F(4),
    /*  11: F12         */	KEYSYM_HOLE,
    /*  12: F5          */	KEYSYM_FUNC_F(5),
    /*  13: AltGraph    */	KEYSYM_HOLE,
    /*  14: F6          */	KEYSYM_FUNC_F(6),
    /*  15:             */	KEYSYM_HOLE,
    /*  16: F7          */	KEYSYM_FUNC_F(7),
    /*  17: F8          */	KEYSYM_FUNC_F(8),
    /*  18: F9          */	KEYSYM_FUNC_F(9),
    /*  19: Alt_L       */	KEYSYM_SETMOD | KBMOD_ALT_L,
    /*  20: T5_Up       */	KEYSYM_HOLE,
    /*  21: R1/Pause    */	KEYSYM_FUNC_R(1),
    /*  22: R2/Print    */	KEYSYM_FUNC_R(2),
    /*  23: R3/Break    */	KEYSYM_FUNC_R(3),
    /*  24: T5_Left     */	KEYSYM_HOLE,
    /*  25: L3/Props    */	KEYSYM_FUNC_L(3),
    /*  26: L4/Undo     */	KEYSYM_FUNC_L(4),
    /*  27: T5_Down     */	KEYSYM_HOLE,
    /*  28: T5_Right    */	KEYSYM_HOLE,
    /*  29: Esc         */	0x1b,
    /*  30: 1           */	'!',
    /*  31: 2           */	'@',
    /*  32: 3           */	'#',
    /*  33: 4           */	'$',
    /*  34: 5           */	'%',
    /*  35: 6           */	'^',
    /*  36: 7           */	'&',
    /*  37: 8           */	'*',
    /*  38: 9           */	'(',
    /*  39: 0           */	')',
    /*  40: minus       */	'_',
    /*  41: equal       */	'+',
    /*  42: grave/tilde */	'~',
    /*  43: BackSpace   */	'\b',
    /*  44:             */	KEYSYM_HOLE,
    /*  45: R4/KP_Equal */	KEYSYM_FUNC_R(4),
    /*  46: R5/KP_Div   */	KEYSYM_FUNC_R(5),
    /*  47: R6/KP_Mult  */	KEYSYM_FUNC_R(6),
    /*  48:             */	KEYSYM_HOLE,
    /*  49: L5/Front    */	KEYSYM_FUNC_L(5),
    /*  50: KP_Delete   */	KEYSYM_HOLE,
    /*  51: L6/Copy     */	KEYSYM_FUNC_L(6),
    /*  52:             */	KEYSYM_HOLE,
    /*  53: Tab         */	'\t',
    /*  54: Q           */	'Q',
    /*  55: W           */	'W',
    /*  56: E           */	'E',
    /*  57: R           */	'R',
    /*  58: T           */	'T',
    /*  59: Y           */	'Y',
    /*  60: U           */	'U',
    /*  61: I           */	'I',
    /*  62: O           */	'O',
    /*  63: P           */	'P',
    /*  64: [           */	'{',
    /*  65: ]           */	'}',
    /*  66: Delete      */	0x7f,
    /*  67: Compose     */	KEYSYM_HOLE,
    /*  68: R7/Home     */	KEYSYM_FUNC_R(7),
    /*  69: R8/Up       */	KEYSYM_STRING | 1,
    /*  70: R9/PgUp     */	KEYSYM_FUNC_R(9),
    /*  71: KP_Minus    */	KEYSYM_HOLE,
    /*  72: L7/Open     */	KEYSYM_FUNC_L(7),
    /*  73: L8/Paste    */	KEYSYM_FUNC_L(8),
    /*  74:             */	KEYSYM_HOLE,
    /*  75:             */	KEYSYM_HOLE,
    /*  76: Ctrl_L      */	KEYSYM_SETMOD | KBMOD_CTRL_L,
    /*  77: A           */	'A',
    /*  78: S           */	'S',
    /*  79: D           */	'D',
    /*  80: F           */	'F',
    /*  81: G           */	'G',
    /*  82: H           */	'H',
    /*  83: J           */	'J',
    /*  84: K           */	'K',
    /*  85: L           */	'L',
    /*  86: ;           */	':',
    /*  87: apostr.     */	'"',
    /*  88: backslash   */	'|',
    /*  89: Return      */	'\r',
    /*  90: KP_Enter    */	KEYSYM_HOLE,
    /*  91: R10/Left    */	KEYSYM_STRING | 3,
    /*  92: R11/KP_5    */	KEYSYM_FUNC_R(11),
    /*  93: R12/Right   */	KEYSYM_STRING | 4,
    /*  94: KP_Insert   */	KEYSYM_HOLE,
    /*  95: L9/Find     */	KEYSYM_FUNC_L(9),
    /*  96:             */	KEYSYM_HOLE,
    /*  97: L10/Cut     */	KEYSYM_FUNC_L(10),
    /*  98: Num_Lock    */	KEYSYM_HOLE,
    /*  99: Shift_L     */	KEYSYM_SETMOD | KBMOD_SHIFT_L,
    /* 100: Z           */	'Z',
    /* 101: X           */	'X',
    /* 102: C           */	'C',
    /* 103: V           */	'V',
    /* 104: B           */	'B',
    /* 105: N           */	'N',
    /* 106: M           */	'M',
    /* 107: ,           */	'<',
    /* 108: .           */	'>',
    /* 109: /           */	'?',
    /* 110: Shift_R     */	KEYSYM_SETMOD | KBMOD_SHIFT_R,
    /* 111: Linefeed    */	'\n',
    /* 112: R13/End     */	KEYSYM_FUNC_R(13),
    /* 113: R14/Down    */	KEYSYM_STRING | 2,
    /* 114: R15/PgDn    */	KEYSYM_FUNC_R(15),
    /* 115:             */	KEYSYM_HOLE,
    /* 116:             */	KEYSYM_HOLE,
    /* 117:             */	KEYSYM_HOLE,
    /* 118: L16/Help    */	KEYSYM_HOLE,
    /* 119: CapsLock    */	KEYSYM_INVMOD | KBMOD_CAPSLOCK,
    /* 120: Meta_L      */	KEYSYM_SETMOD | KBMOD_META_L,
    /* 121: SpaceBar    */	' ',
    /* 122: Meta_R      */	KEYSYM_SETMOD | KBMOD_META_R,
    /* 123:             */	KEYSYM_HOLE,
    /* 124:             */	KEYSYM_HOLE,
    /* 125: KP_Add      */	KEYSYM_HOLE,
    /* 126:             */	KEYSYM_HW_ERR,
    /* 127:             */	KEYSYM_ALL_UP,
};


/*
 * Keymaps for the "type 4" keyboard.
 * (lower-case, upper-case)
 */

struct keymap keymap_s4_lc = {
    /*   0:             */	KEYSYM_HOLE,
    /*   1: L1/Stop     */	KEYSYM_FUNC_L(1),
    /*   2:             */	KEYSYM_HOLE,
    /*   3: L2/Again    */	KEYSYM_FUNC_L(2),
    /*   4:             */	KEYSYM_HOLE,
    /*   5: F1          */	KEYSYM_FUNC_F(1),
    /*   6: F2          */	KEYSYM_FUNC_F(2),
    /*   7: F10         */	KEYSYM_FUNC_F(10),
    /*   8: F3          */	KEYSYM_FUNC_F(3),
    /*   9: F11         */	KEYSYM_FUNC_F(11),
    /*  10: F4          */	KEYSYM_FUNC_F(4),
    /*  11: F12         */	KEYSYM_FUNC_F(12),
    /*  12: F5          */	KEYSYM_FUNC_F(5),
    /*  13: AltGraph    */	KEYSYM_SETMOD | KBMOD_ALTGRAPH,
    /*  14: F6          */	KEYSYM_FUNC_F(6),
    /*  15:             */	KEYSYM_HOLE,
    /*  16: F7          */	KEYSYM_FUNC_F(7),
    /*  17: F8          */	KEYSYM_FUNC_F(8),
    /*  18: F9          */	KEYSYM_FUNC_F(9),
    /*  19: Alt_L       */	KEYSYM_SETMOD | KBMOD_ALT_L,
    /*  20: T5_Up       */	KEYSYM_HOLE,
    /*  21: R1/Pause    */	KEYSYM_FUNC_R(1),
    /*  22: R2/Print    */	KEYSYM_FUNC_R(2),
    /*  23: R3/Break    */	KEYSYM_FUNC_R(3),
    /*  24: T5_Left     */	KEYSYM_HOLE,
    /*  25: L3/Props    */	KEYSYM_FUNC_L(3),
    /*  26: L4/Undo     */	KEYSYM_FUNC_L(4),
    /*  27: T5_Down     */	KEYSYM_HOLE,
    /*  28: T5_Right    */	KEYSYM_HOLE,
    /*  29: Esc         */	0x1b,
    /*  30: 1           */	'1',
    /*  31: 2           */	'2',
    /*  32: 3           */	'3',
    /*  33: 4           */	'4',
    /*  34: 5           */	'5',
    /*  35: 6           */	'6',
    /*  36: 7           */	'7',
    /*  37: 8           */	'8',
    /*  38: 9           */	'9',
    /*  39: 0           */	'0',
    /*  40: minus       */	'-',
    /*  41: equal       */	'=',
    /*  42: grave/tilde */	'`',
    /*  43: BackSpace   */	'\b',
    /*  44:             */	KEYSYM_HOLE,
    /*  45: R4/KP_Equal */	KEYSYM_FUNC_R(4),
    /*  46: R5/KP_Div   */	KEYSYM_FUNC_R(5),
    /*  47: R6/KP_Mult  */	KEYSYM_FUNC_R(6),
    /*  48:             */	KEYSYM_FUNC_N(13),
    /*  49: L5/Front    */	KEYSYM_FUNC_L(5),
    /*  50: KP_Delete   */	KEYSYM_FUNC_N(10),
    /*  51: L6/Copy     */	KEYSYM_FUNC_L(6),
    /*  52:             */	KEYSYM_HOLE,
    /*  53: Tab         */	'\t',
    /*  54: Q           */	'q',
    /*  55: W           */	'w',
    /*  56: E           */	'e',
    /*  57: R           */	'r',
    /*  58: T           */	't',
    /*  59: Y           */	'y',
    /*  60: U           */	'u',
    /*  61: I           */	'i',
    /*  62: O           */	'o',
    /*  63: P           */	'p',
    /*  64: [           */	'[',
    /*  65: ]           */	']',
    /*  66: Delete      */	0x7f,
    /*  67: Compose     */	KEYSYM_COMPOSE,
    /*  68: R7/Home     */	KEYSYM_FUNC_R(7),
    /*  69: R8/Up       */	KEYSYM_STRING | 1,
    /*  70: R9/PgUp     */	KEYSYM_FUNC_R(9),
    /*  71: KP_Minus    */	KEYSYM_FUNC_N(15),
    /*  72: L7/Open     */	KEYSYM_FUNC_L(7),
    /*  73: L8/Paste    */	KEYSYM_FUNC_L(8),
    /*  74:             */	KEYSYM_HOLE,
    /*  75:             */	KEYSYM_HOLE,
    /*  76: Ctrl_L      */	KEYSYM_SETMOD | KBMOD_CTRL_L,
    /*  77: A           */	'a',
    /*  78: S           */	's',
    /*  79: D           */	'd',
    /*  80: F           */	'f',
    /*  81: G           */	'g',
    /*  82: H           */	'h',
    /*  83: J           */	'j',
    /*  84: K           */	'k',
    /*  85: L           */	'l',
    /*  86: ;           */	';',
    /*  87: apostr.     */	'\'',
    /*  88: backslash   */	'\\',
    /*  89: Return      */	'\r',
    /*  90: KP_Enter    */	KEYSYM_FUNC_N(11),
    /*  91: R10/Left    */	KEYSYM_STRING | 3,
    /*  92: R11/KP_5    */	KEYSYM_FUNC_R(11),
    /*  93: R12/Right   */	KEYSYM_STRING | 4,
    /*  94: KP_Insert   */	KEYSYM_FUNC_N(8),
    /*  95: L9/Find     */	KEYSYM_FUNC_L(9),
    /*  96:             */	KEYSYM_HOLE,
    /*  97: L10/Cut     */	KEYSYM_FUNC_L(10),
    /*  98: Num_Lock    */	KEYSYM_INVMOD | KBMOD_NUMLOCK,
    /*  99: Shift_L     */	KEYSYM_SETMOD | KBMOD_SHIFT_L,
    /* 100: Z           */	'z',
    /* 101: X           */	'x',
    /* 102: C           */	'c',
    /* 103: V           */	'v',
    /* 104: B           */	'b',
    /* 105: N           */	'n',
    /* 106: M           */	'm',
    /* 107: ,           */	',',
    /* 108: .           */	'.',
    /* 109: /           */	'/',
    /* 110: Shift_R     */	KEYSYM_SETMOD | KBMOD_SHIFT_R,
    /* 111: Linefeed    */	'\n',
    /* 112: R13/End     */	KEYSYM_FUNC_R(13),
    /* 113: R14/Down    */	KEYSYM_STRING | 2,
    /* 114: R15/PgDn    */	KEYSYM_FUNC_R(15),
    /* 115:             */	KEYSYM_HOLE,
    /* 116:             */	KEYSYM_HOLE,
    /* 117:             */	KEYSYM_HOLE,
    /* 118: L16/Help    */	KEYSYM_FUNC_L(16),
    /* 119: CapsLock    */	KEYSYM_INVMOD | KBMOD_CAPSLOCK,
    /* 120: Meta_L      */	KEYSYM_SETMOD | KBMOD_META_L,
    /* 121: SpaceBar    */	' ',
    /* 122: Meta_R      */	KEYSYM_SETMOD | KBMOD_META_R,
    /* 123:             */	KEYSYM_HOLE,
    /* 124:             */	KEYSYM_HOLE,
    /* 125: KP_Add      */	KEYSYM_FUNC_N(14),
    /* 126:             */	KEYSYM_HW_ERR,
    /* 127:             */	KEYSYM_ALL_UP,
};


struct keymap keymap_s4_uc = {
    /*   0:             */	KEYSYM_HOLE,
    /*   1: L1/Stop     */	KEYSYM_FUNC_L(1),
    /*   2:             */	KEYSYM_HOLE,
    /*   3: L2/Again    */	KEYSYM_FUNC_L(2),
    /*   4:             */	KEYSYM_HOLE,
    /*   5: F1          */	KEYSYM_FUNC_F(1),
    /*   6: F2          */	KEYSYM_FUNC_F(2),
    /*   7: F10         */	KEYSYM_FUNC_F(10),
    /*   8: F3          */	KEYSYM_FUNC_F(3),
    /*   9: F11         */	KEYSYM_FUNC_F(11),
    /*  10: F4          */	KEYSYM_FUNC_F(4),
    /*  11: F12         */	KEYSYM_FUNC_F(12),
    /*  12: F5          */	KEYSYM_FUNC_F(5),
    /*  13: AltGraph    */	KEYSYM_SETMOD | KBMOD_ALTGRAPH,
    /*  14: F6          */	KEYSYM_FUNC_F(6),
    /*  15:             */	KEYSYM_HOLE,
    /*  16: F7          */	KEYSYM_FUNC_F(7),
    /*  17: F8          */	KEYSYM_FUNC_F(8),
    /*  18: F9          */	KEYSYM_FUNC_F(9),
    /*  19: Alt_L       */	KEYSYM_SETMOD | KBMOD_ALT_L,
    /*  20: T5_Up       */	KEYSYM_HOLE,
    /*  21: R1/Pause    */	KEYSYM_FUNC_R(1),
    /*  22: R2/Print    */	KEYSYM_FUNC_R(2),
    /*  23: R3/Break    */	KEYSYM_FUNC_R(3),
    /*  24: T5_Left     */	KEYSYM_HOLE,
    /*  25: L3/Props    */	KEYSYM_FUNC_L(3),
    /*  26: L4/Undo     */	KEYSYM_FUNC_L(4),
    /*  27: T5_Down     */	KEYSYM_HOLE,
    /*  28: T5_Right    */	KEYSYM_HOLE,
    /*  29: Esc         */	0x1b,
    /*  30: 1           */	'!',
    /*  31: 2           */	'@',
    /*  32: 3           */	'#',
    /*  33: 4           */	'$',
    /*  34: 5           */	'%',
    /*  35: 6           */	'^',
    /*  36: 7           */	'&',
    /*  37: 8           */	'*',
    /*  38: 9           */	'(',
    /*  39: 0           */	')',
    /*  40: minus       */	'_',
    /*  41: equal       */	'+',
    /*  42: grave/tilde */	'~',
    /*  43: BackSpace   */	'\b',
    /*  44:             */	KEYSYM_HOLE,
    /*  45: R4/KP_Equal */	KEYSYM_FUNC_R(4),
    /*  46: R5/KP_Div   */	KEYSYM_FUNC_R(5),
    /*  47: R6/KP_Mult  */	KEYSYM_FUNC_R(6),
    /*  48:             */	KEYSYM_FUNC_N(13),
    /*  49: L5/Front    */	KEYSYM_FUNC_L(5),
    /*  50: KP_Delete   */	KEYSYM_FUNC_N(10),
    /*  51: L6/Copy     */	KEYSYM_FUNC_L(6),
    /*  52:             */	KEYSYM_HOLE,
    /*  53: Tab         */	'\t',
    /*  54: Q           */	'Q',
    /*  55: W           */	'W',
    /*  56: E           */	'E',
    /*  57: R           */	'R',
    /*  58: T           */	'T',
    /*  59: Y           */	'Y',
    /*  60: U           */	'U',
    /*  61: I           */	'I',
    /*  62: O           */	'O',
    /*  63: P           */	'P',
    /*  64: [           */	'{',
    /*  65: ]           */	'}',
    /*  66: Delete      */	0x7f,
    /*  67: Compose     */	KEYSYM_COMPOSE,
    /*  68: R7/Home     */	KEYSYM_FUNC_R(7),
    /*  69: R8/Up       */	KEYSYM_STRING | 1,
    /*  70: R9/PgUp     */	KEYSYM_FUNC_R(9),
    /*  71: KP_Minus    */	KEYSYM_FUNC_N(15),
    /*  72: L7/Open     */	KEYSYM_FUNC_L(7),
    /*  73: L8/Paste    */	KEYSYM_FUNC_L(8),
    /*  74:             */	KEYSYM_HOLE,
    /*  75:             */	KEYSYM_HOLE,
    /*  76: Ctrl_L      */	KEYSYM_SETMOD | KBMOD_CTRL_L,
    /*  77: A           */	'A',
    /*  78: S           */	'S',
    /*  79: D           */	'D',
    /*  80: F           */	'F',
    /*  81: G           */	'G',
    /*  82: H           */	'H',
    /*  83: J           */	'J',
    /*  84: K           */	'K',
    /*  85: L           */	'L',
    /*  86: ;           */	':',
    /*  87: apostr.     */	'"',
    /*  88: backslash   */	'|',
    /*  89: Return      */	'\r',
    /*  90: KP_Enter    */	KEYSYM_FUNC_N(11),
    /*  91: R10/Left    */	KEYSYM_STRING | 3,
    /*  92: R11/KP_5    */	KEYSYM_FUNC_R(11),
    /*  93: R12/Right   */	KEYSYM_STRING | 4,
    /*  94: KP_Insert   */	KEYSYM_FUNC_N(8),
    /*  95: L9/Find     */	KEYSYM_FUNC_L(9),
    /*  96:             */	KEYSYM_HOLE,
    /*  97: L10/Cut     */	KEYSYM_FUNC_L(10),
    /*  98: Num_Lock    */	KEYSYM_INVMOD | KBMOD_NUMLOCK,
    /*  99: Shift_L     */	KEYSYM_SETMOD | KBMOD_SHIFT_L,
    /* 100: Z           */	'Z',
    /* 101: X           */	'X',
    /* 102: C           */	'C',
    /* 103: V           */	'V',
    /* 104: B           */	'B',
    /* 105: N           */	'N',
    /* 106: M           */	'M',
    /* 107: ,           */	'<',
    /* 108: .           */	'>',
    /* 109: /           */	'?',
    /* 110: Shift_R     */	KEYSYM_SETMOD | KBMOD_SHIFT_R,
    /* 111: Linefeed    */	'\n',
    /* 112: R13/End     */	KEYSYM_FUNC_R(13),
    /* 113: R14/Down    */	KEYSYM_STRING | 2,
    /* 114: R15/PgDn    */	KEYSYM_FUNC_R(15),
    /* 115:             */	KEYSYM_HOLE,
    /* 116:             */	KEYSYM_HOLE,
    /* 117:             */	KEYSYM_HOLE,
    /* 118: L16/Help    */	KEYSYM_FUNC_L(16),
    /* 119: CapsLock    */	KEYSYM_INVMOD | KBMOD_CAPSLOCK,
    /* 120: Meta_L      */	KEYSYM_SETMOD | KBMOD_META_L,
    /* 121: SpaceBar    */	' ',
    /* 122: Meta_R      */	KEYSYM_SETMOD | KBMOD_META_R,
    /* 123:             */	KEYSYM_HOLE,
    /* 124:             */	KEYSYM_HOLE,
    /* 125: KP_Add      */	KEYSYM_FUNC_N(14),
    /* 126:             */	KEYSYM_HW_ERR,
    /* 127:             */	KEYSYM_ALL_UP,
};



/*
 * Strings indexed by:  (KEYSYM_STRING | idx)
 */
char kbd_stringtab[16][10] = {
	{ 0x1b, '[', 'H', 0 },	/* Home */
	{ 0x1b, '[', 'A', 0 },	/* Up   */
	{ 0x1b, '[', 'B', 0 },	/* Down */
	{ 0x1b, '[', 'D', 0 },	/* Left */
	{ 0x1b, '[', 'C', 0 },	/* Right */
};

/*
 * The "NumLock" map, which is used to remap
 * function keysyms when NumLock is on.
 */
unsigned short kbd_numlock_map[64] = {
	/* KEYSYM_FUNC_L: Identity map */
	0x600, 0x601, 0x602, 0x603, 0x604, 0x605, 0x606, 0x607,
	0x608, 0x609, 0x60a, 0x60b, 0x60c, 0x60d, 0x60e, 0x60f,

	/* KEYSYM_FUNC_R: remap to numbers... */
	0x610, 0x611, 0x612,
	'=', '/', '*',
	'7', '8', '9',
	'4', '5', '6',
	'1', '2', '3',
	0x61f,

	/* KEYSYM_FUNC_F: Identity map */
	0x620, 0x621, 0x622, 0x623, 0x624, 0x625, 0x626, 0x627,
	0x628, 0x629, 0x62a, 0x62b, 0x62c, 0x62d, 0x62e, 0x62f,

	/* KEYSYM_FUNC_N: remap just a few... */
	0x630, 0x631, 0x632, 0x633, 0x634, 0x635, 0x636,   '0',
	0x638,   '.',  '\r', 0x63b, 0x63c,   '+',   '-', 0x63f,
};


/*
 * Keyboard descriptions for each type.
 */

/* Treat type 2 as type 3 (close enough) */
#define	kbd_type2 kbd_type3

static struct keyboard kbd_type3 = {
	&keymap_release,
	&keymap_control,
	&keymap_s3_lc,
	&keymap_s3_uc,
};

static struct keyboard kbd_type4 = {
	&keymap_release,
	&keymap_control,
	&keymap_s4_lc,
	&keymap_s4_uc,
};

/* Treat type 5 as type 4 (close enough) */
#define	kbd_type5 kbd_type4

struct keyboard * keyboards[] = {
	0, /* type 0 */
	0, /* type 1 */
	&kbd_type2,
	&kbd_type3,
	&kbd_type4,
	&kbd_type5,
};		
int kbd_max_type = 5;
