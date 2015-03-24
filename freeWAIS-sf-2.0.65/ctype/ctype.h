/*                               -*- Mode: C -*- 
 * ctype.h -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Dmitry Kryukov <dima@ulysses.stack.serpukhov.su>
 * Created On      : Wed Mar 23 09:39:11 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Mon Nov 20 15:18:14 1995
 * Update Count    : 53
 * Status          : Unknown, Use with caution!
 */

#ifndef _CTYPE_H_
#define _CTYPE_H_

#define	_U	0x01
#define	_L	0x02
#define	_N	0x04
#define	_S	0x08
#define	_P	0x10
#define	_C	0x20
#define	_X	0x40
#define	_B	0x80

#ifndef CTYPE_C
#ifdef STDC_HEADERS
extern const char _fw_ctype_[];
#else
extern char _fw_ctype_[];
#endif /* STDC_HEADERS */
#endif /* CTYPE_C */

#define	isalnum(c)	((_fw_ctype_ + 1)[(unsigned char)c] & (_U|_L|_N))
#define	isalpha(c)	((_fw_ctype_ + 1)[(unsigned char)c] & (_U|_L))
#define	isascii(c)	((unsigned)(c) <= 0177)
#define	isblank(c)	((c) == '\t' || (c) == ' ')
#define	iscntrl(c)	((_fw_ctype_ + 1)[(unsigned char)c] & _C)
#define	isdigit(c)	((_fw_ctype_ + 1)[(unsigned char)c] & _N)
#define	isgraph(c)	((_fw_ctype_ + 1)[(unsigned char)c] & (_P|_U|_L|_N))
#define	islower(c)	((_fw_ctype_ + 1)[(unsigned char)c] & _L)
#define	isprint(c)	((_fw_ctype_ + 1)[(unsigned char)c] & (_P|_U|_L|_N|_B))
#define	ispunct(c)	((_fw_ctype_ + 1)[(unsigned char)c] & _P)
#define	isspace(c)	((_fw_ctype_ + 1)[(unsigned char)c] & _S)
#define	isupper(c)	((_fw_ctype_ + 1)[(unsigned char)c] & _U)
/*
#define	isxdigit(c)	((_fw_ctype_ + 1)[(unsigned char)c] & (_N|_X))
*/
#define	isxdigit(c)	((_fw_ctype_ + 1)[(unsigned char)c] & _X)
#define	toascii(c)	((c) & 0177)

/***********************************************************************/
/* in cyrillic we have low symbols from 0xc0 to 0xdf and upper symbols */
/*                       from 0xe0 to 0xff                             */
/***********************************************************************/
/*
#define index(s,c) \
({ \
     unsigned char * _index_; \
     for (_index_=s; *_index_ && *_index_ != c; _index_++); \
     (*_index_ == c)? _index_ : NULL; \
})
*/
extern unsigned char _lchars_[];
extern unsigned char _uchars_[];
/*
#define	tolower(c) \
({ \
	int __tolower_c = (unsigned char)(c); \
\
	isupper(__tolower_c) ? \
	    ( isascii(__tolower_c) ? __tolower_c - 'A' + 'a' : \
	    _lchars_[(int)(index(_uchars_,__tolower_c)-(char *)_uchars_)]) : \
	    __tolower_c; \
})
*/
/*
#define	toupper(c) \
({ \
	int __toupper_c = (unsigned char)(c); \
\
	islower(__toupper_c) ? \
	    ( isascii(__toupper_c) ? __toupper_c - 'a' + 'A' : \
	    _uchars_[(int)(index(_lchars_,__toupper_c) - (char *)_lchars_ )]) : \
	    __toupper_c; \
})
*/
#ifdef STDC_HEADERS
#define	tolower(c) \
	(isupper((unsigned char)(c)) ? \
	    ( isascii((unsigned char)(c)) ? ((unsigned char)(c) - 'A' + 'a') : \
	    (islower((unsigned char)(c)) ? (unsigned char)(c) : \
             _lchars_[(int)(index(_uchars_,(unsigned char)(c)) - \
                            (char *)_uchars_)])) \
         : (unsigned char)(c))

#define	toupper(c) \
        (islower((unsigned char)(c)) ? \
	    ( isascii((unsigned char)(c)) ? ((unsigned char)(c) - 'a' + 'A') : \
	    (isupper((unsigned char)(c)) ?(unsigned char)(c) : \
             _uchars_[(int)(index(_lchars_,(unsigned char)(c)) - \
                           (char *)_lchars_ )])) :  (unsigned char)(c))
#else
/* yet another version: I take 2 * 256 bytes to construct a - usually sparse -lookup
 * table
 * MN */
#undef toupper
#ifndef INTL_CTYPE_MODULE
extern char tToUP[];
extern char tToLOW[];
#endif
#define toupper(c)              ((int) (tToUP[ (int)(unsigned char)(c) ]))

#undef tolower
#define tolower(c)              ((int) (tToLOW[ (int)(unsigned char)(c) ]))

#endif /* STDC_HEADERS */

#ifdef strcmp 
#undef strcmp
#endif
#ifdef strncmp
#undef strncmp
#endif
#define strcmp(A,B) mystrcmp((A),(B))
#define strncmp(A,B,N) mystrncmp((A),(B),(N))
#define strcasecmp(A,B) mystrcasecmp((A),(B))
#ifndef _AP
#define _AP(ARGS) ()
#endif
int mystrcmp _AP((unsigned char *s1, unsigned char *s2));
int mystrncmp _AP((unsigned char *s1, unsigned char *s2, 
                          unsigned long n));
int mystrcasecmp _AP((unsigned char *s1, unsigned char *s2));
#endif /* !_CTYPE_H_ */

