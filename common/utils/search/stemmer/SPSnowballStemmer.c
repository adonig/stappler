
#include <stdlib.h>
#include <string.h>


struct sb_stemmer;
typedef unsigned char sb_symbol;

/* FIXME - should be able to get a version number for each stemming
 * algorithm (which will be incremented each time the output changes). */

/** Returns an array of the names of the available stemming algorithms.
 *  Note that these are the canonical names - aliases (ie, other names for
 *  the same algorithm) will not be included in the list.
 *  The list is terminated with a null pointer.
 *
 *  The list must not be modified in any way.
 */
const char ** sb_stemmer_list(void);

/** Create a new stemmer object, using the specified algorithm, for the
 *  specified character encoding.
 *
 *  All algorithms will usually be available in UTF-8, but may also be
 *  available in other character encodings.
 *
 *  @param algorithm The algorithm name.  This is either the english
 *  name of the algorithm, or the 2 or 3 letter ISO 639 codes for the
 *  language.  Note that case is significant in this parameter - the
 *  value should be supplied in lower case.
 *
 *  @param charenc The character encoding.  NULL may be passed as
 *  this value, in which case UTF-8 encoding will be assumed. Otherwise,
 *  the argument may be one of "UTF_8", "ISO_8859_1" (ie, Latin 1),
 *  "CP850" (ie, MS-DOS Latin 1) or "KOI8_R" (Russian).  Note that
 *  case is significant in this parameter.
 *
 *  @return NULL if the specified algorithm is not recognised, or the
 *  algorithm is not available for the requested encoding.  Otherwise,
 *  returns a pointer to a newly created stemmer for the requested algorithm.
 *  The returned pointer must be deleted by calling sb_stemmer_delete().
 *
 *  @note NULL will also be returned if an out of memory error occurs.
 */
struct sb_stemmer * sb_stemmer_new(const char * algorithm, const char * charenc);

/** Delete a stemmer object.
 *
 *  This frees all resources allocated for the stemmer.  After calling
 *  this function, the supplied stemmer may no longer be used in any way.
 *
 *  It is safe to pass a null pointer to this function - this will have
 *  no effect.
 */
void                sb_stemmer_delete(struct sb_stemmer * stemmer);

/** Stem a word.
 *
 *  The return value is owned by the stemmer - it must not be freed or
 *  modified, and it will become invalid when the stemmer is called again,
 *  or if the stemmer is freed.
 *
 *  The length of the return value can be obtained using sb_stemmer_length().
 *
 *  If an out-of-memory error occurs, this will return NULL.
 */
const sb_symbol *   sb_stemmer_stem(struct sb_stemmer * stemmer,
				    const sb_symbol * word, int size);

/** Get the length of the result of the last stemmed word.
 *  This should not be called before sb_stemmer_stem() has been called.
 */
int                 sb_stemmer_length(struct sb_stemmer * stemmer);


typedef unsigned char symbol;

#define MAXINT INT_MAX
#define MININT INT_MIN

#define HEAD 2*sizeof(int)

#define SIZE(p)        ((int *)(p))[-1]
#define SET_SIZE(p, n) ((int *)(p))[-1] = n
#define CAPACITY(p)    ((int *)(p))[-2]

/* Or replace 'char' above with 'short' for 16 bit characters.

   More precisely, replace 'char' with whatever type guarantees the
   character width you need. Note however that sizeof(symbol) should divide
   HEAD, defined in header.h as 2*sizeof(int), without remainder, otherwise
   there is an alignment problem. In the unlikely event of a problem here,
   consult Martin Porter.

*/

struct SN_env {
    symbol * p;
    int c; int l; int lb; int bra; int ket;
    symbol * * S;
    int * I;
    unsigned char * B;
};

static struct SN_env * SN_create_env(int S_size, int I_size, int B_size);
static void SN_close_env(struct SN_env * z, int S_size);
static int SN_set_current(struct SN_env * z, int size, const symbol * s);

struct among
{   int s_size;     /* number of chars in string */
    const symbol * s;       /* search string */
    int substring_i;/* index to longest matching substring */
    int result;     /* result of the lookup */
    int (* function)(struct SN_env *);
};

static symbol * create_s(void);
static void lose_s(symbol * p);

static int skip_utf8(const symbol * p, int c, int lb, int l, int n) __attribute__ ((unused));

static int in_grouping_U(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) __attribute__ ((unused));
static int in_grouping_b_U(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) __attribute__ ((unused));
static int out_grouping_U(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) __attribute__ ((unused));
static int out_grouping_b_U(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) __attribute__ ((unused));

static int in_grouping(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) __attribute__ ((unused));
static int in_grouping_b(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) __attribute__ ((unused));
static int out_grouping(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) __attribute__ ((unused));
static int out_grouping_b(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) __attribute__ ((unused));

static int eq_s(struct SN_env * z, int s_size, const symbol * s) __attribute__ ((unused));
static int eq_s_b(struct SN_env * z, int s_size, const symbol * s) __attribute__ ((unused));
static int eq_v(struct SN_env * z, const symbol * p) __attribute__ ((unused));
static int eq_v_b(struct SN_env * z, const symbol * p) __attribute__ ((unused));

static int find_among(struct SN_env * z, const struct among * v, int v_size) __attribute__ ((unused));
static int find_among_b(struct SN_env * z, const struct among * v, int v_size) __attribute__ ((unused));

static int replace_s(struct SN_env * z, int c_bra, int c_ket, int s_size, const symbol * s, int * adjustment) __attribute__ ((unused));
static int slice_from_s(struct SN_env * z, int s_size, const symbol * s) __attribute__ ((unused));
static int slice_from_v(struct SN_env * z, const symbol * p) __attribute__ ((unused));
static int slice_del(struct SN_env * z) __attribute__ ((unused));

static int insert_s(struct SN_env * z, int bra, int ket, int s_size, const symbol * s) __attribute__ ((unused));
static int insert_v(struct SN_env * z, int bra, int ket, const symbol * p) __attribute__ ((unused));

static symbol * slice_to(struct SN_env * z, symbol * p) __attribute__ ((unused));
static symbol * assign_to(struct SN_env * z, symbol * p) __attribute__ ((unused));

static int len_utf8(const symbol * p) __attribute__ ((unused));

#include "stem_UTF_8_arabic.cc"
#include "stem_UTF_8_danish.cc"
#include "stem_UTF_8_dutch.cc"
#include "stem_UTF_8_english.cc"
#include "stem_UTF_8_finnish.cc"
#include "stem_UTF_8_french.cc"
#include "stem_UTF_8_german.cc"
#include "stem_UTF_8_greek.cc"
#include "stem_UTF_8_hungarian.cc"
#include "stem_UTF_8_indonesian.cc"
#include "stem_UTF_8_irish.cc"
#include "stem_UTF_8_italian.cc"
#include "stem_UTF_8_lithuanian.cc"
#include "stem_UTF_8_nepali.cc"
#include "stem_UTF_8_norwegian.cc"
#include "stem_UTF_8_portuguese.cc"
#include "stem_UTF_8_romanian.cc"
#include "stem_UTF_8_russian.cc"
#include "stem_UTF_8_spanish.cc"
#include "stem_UTF_8_swedish.cc"
#include "stem_UTF_8_tamil.cc"
#include "stem_UTF_8_turkish.cc"

typedef enum {
  ENC_UNKNOWN=0,
  ENC_UTF_8
} stemmer_encoding_t;

struct stemmer_encoding {
  const char * name;
  stemmer_encoding_t enc;
};
static struct stemmer_encoding encodings[] = {
  {"UTF_8", ENC_UTF_8},
  {0,ENC_UNKNOWN}
};

struct stemmer_modules {
  const char * name;
  stemmer_encoding_t enc;
  struct SN_env * (*create)(void);
  void (*close)(struct SN_env *);
  int (*stem)(struct SN_env *);
};
static struct stemmer_modules modules[] = {
  {"ar", ENC_UTF_8, arabic_UTF_8_create_env, arabic_UTF_8_close_env, arabic_UTF_8_stem},
  {"ara", ENC_UTF_8, arabic_UTF_8_create_env, arabic_UTF_8_close_env, arabic_UTF_8_stem},
  {"arabic", ENC_UTF_8, arabic_UTF_8_create_env, arabic_UTF_8_close_env, arabic_UTF_8_stem},
  {"da", ENC_UTF_8, danish_UTF_8_create_env, danish_UTF_8_close_env, danish_UTF_8_stem},
  {"dan", ENC_UTF_8, danish_UTF_8_create_env, danish_UTF_8_close_env, danish_UTF_8_stem},
  {"danish", ENC_UTF_8, danish_UTF_8_create_env, danish_UTF_8_close_env, danish_UTF_8_stem},
  {"de", ENC_UTF_8, german_UTF_8_create_env, german_UTF_8_close_env, german_UTF_8_stem},
  {"deu", ENC_UTF_8, german_UTF_8_create_env, german_UTF_8_close_env, german_UTF_8_stem},
  {"dut", ENC_UTF_8, dutch_UTF_8_create_env, dutch_UTF_8_close_env, dutch_UTF_8_stem},
  {"dutch", ENC_UTF_8, dutch_UTF_8_create_env, dutch_UTF_8_close_env, dutch_UTF_8_stem},
  {"el", ENC_UTF_8, greek_UTF_8_create_env, greek_UTF_8_close_env, greek_UTF_8_stem},
  {"ell", ENC_UTF_8, greek_UTF_8_create_env, greek_UTF_8_close_env, greek_UTF_8_stem},
  {"en", ENC_UTF_8, english_UTF_8_create_env, english_UTF_8_close_env, english_UTF_8_stem},
  {"eng", ENC_UTF_8, english_UTF_8_create_env, english_UTF_8_close_env, english_UTF_8_stem},
  {"english", ENC_UTF_8, english_UTF_8_create_env, english_UTF_8_close_env, english_UTF_8_stem},
  {"es", ENC_UTF_8, spanish_UTF_8_create_env, spanish_UTF_8_close_env, spanish_UTF_8_stem},
  {"esl", ENC_UTF_8, spanish_UTF_8_create_env, spanish_UTF_8_close_env, spanish_UTF_8_stem},
  {"fi", ENC_UTF_8, finnish_UTF_8_create_env, finnish_UTF_8_close_env, finnish_UTF_8_stem},
  {"fin", ENC_UTF_8, finnish_UTF_8_create_env, finnish_UTF_8_close_env, finnish_UTF_8_stem},
  {"finnish", ENC_UTF_8, finnish_UTF_8_create_env, finnish_UTF_8_close_env, finnish_UTF_8_stem},
  {"fr", ENC_UTF_8, french_UTF_8_create_env, french_UTF_8_close_env, french_UTF_8_stem},
  {"fra", ENC_UTF_8, french_UTF_8_create_env, french_UTF_8_close_env, french_UTF_8_stem},
  {"fre", ENC_UTF_8, french_UTF_8_create_env, french_UTF_8_close_env, french_UTF_8_stem},
  {"french", ENC_UTF_8, french_UTF_8_create_env, french_UTF_8_close_env, french_UTF_8_stem},
  {"ga", ENC_UTF_8, irish_UTF_8_create_env, irish_UTF_8_close_env, irish_UTF_8_stem},
  {"ger", ENC_UTF_8, german_UTF_8_create_env, german_UTF_8_close_env, german_UTF_8_stem},
  {"german", ENC_UTF_8, german_UTF_8_create_env, german_UTF_8_close_env, german_UTF_8_stem},
  {"gle", ENC_UTF_8, irish_UTF_8_create_env, irish_UTF_8_close_env, irish_UTF_8_stem},
  {"gre", ENC_UTF_8, greek_UTF_8_create_env, greek_UTF_8_close_env, greek_UTF_8_stem},
  {"greek", ENC_UTF_8, greek_UTF_8_create_env, greek_UTF_8_close_env, greek_UTF_8_stem},
  {"hu", ENC_UTF_8, hungarian_UTF_8_create_env, hungarian_UTF_8_close_env, hungarian_UTF_8_stem},
  {"hun", ENC_UTF_8, hungarian_UTF_8_create_env, hungarian_UTF_8_close_env, hungarian_UTF_8_stem},
  {"hungarian", ENC_UTF_8, hungarian_UTF_8_create_env, hungarian_UTF_8_close_env, hungarian_UTF_8_stem},
  {"id", ENC_UTF_8, indonesian_UTF_8_create_env, indonesian_UTF_8_close_env, indonesian_UTF_8_stem},
  {"ind", ENC_UTF_8, indonesian_UTF_8_create_env, indonesian_UTF_8_close_env, indonesian_UTF_8_stem},
  {"indonesian", ENC_UTF_8, indonesian_UTF_8_create_env, indonesian_UTF_8_close_env, indonesian_UTF_8_stem},
  {"irish", ENC_UTF_8, irish_UTF_8_create_env, irish_UTF_8_close_env, irish_UTF_8_stem},
  {"it", ENC_UTF_8, italian_UTF_8_create_env, italian_UTF_8_close_env, italian_UTF_8_stem},
  {"ita", ENC_UTF_8, italian_UTF_8_create_env, italian_UTF_8_close_env, italian_UTF_8_stem},
  {"italian", ENC_UTF_8, italian_UTF_8_create_env, italian_UTF_8_close_env, italian_UTF_8_stem},
  {"lit", ENC_UTF_8, lithuanian_UTF_8_create_env, lithuanian_UTF_8_close_env, lithuanian_UTF_8_stem},
  {"lithuanian", ENC_UTF_8, lithuanian_UTF_8_create_env, lithuanian_UTF_8_close_env, lithuanian_UTF_8_stem},
  {"lt", ENC_UTF_8, lithuanian_UTF_8_create_env, lithuanian_UTF_8_close_env, lithuanian_UTF_8_stem},
  {"ne", ENC_UTF_8, nepali_UTF_8_create_env, nepali_UTF_8_close_env, nepali_UTF_8_stem},
  {"nep", ENC_UTF_8, nepali_UTF_8_create_env, nepali_UTF_8_close_env, nepali_UTF_8_stem},
  {"nepali", ENC_UTF_8, nepali_UTF_8_create_env, nepali_UTF_8_close_env, nepali_UTF_8_stem},
  {"nl", ENC_UTF_8, dutch_UTF_8_create_env, dutch_UTF_8_close_env, dutch_UTF_8_stem},
  {"nld", ENC_UTF_8, dutch_UTF_8_create_env, dutch_UTF_8_close_env, dutch_UTF_8_stem},
  {"no", ENC_UTF_8, norwegian_UTF_8_create_env, norwegian_UTF_8_close_env, norwegian_UTF_8_stem},
  {"nor", ENC_UTF_8, norwegian_UTF_8_create_env, norwegian_UTF_8_close_env, norwegian_UTF_8_stem},
  {"norwegian", ENC_UTF_8, norwegian_UTF_8_create_env, norwegian_UTF_8_close_env, norwegian_UTF_8_stem},
  {"por", ENC_UTF_8, portuguese_UTF_8_create_env, portuguese_UTF_8_close_env, portuguese_UTF_8_stem},
  {"portuguese", ENC_UTF_8, portuguese_UTF_8_create_env, portuguese_UTF_8_close_env, portuguese_UTF_8_stem},
  {"pt", ENC_UTF_8, portuguese_UTF_8_create_env, portuguese_UTF_8_close_env, portuguese_UTF_8_stem},
  {"ro", ENC_UTF_8, romanian_UTF_8_create_env, romanian_UTF_8_close_env, romanian_UTF_8_stem},
  {"romanian", ENC_UTF_8, romanian_UTF_8_create_env, romanian_UTF_8_close_env, romanian_UTF_8_stem},
  {"ron", ENC_UTF_8, romanian_UTF_8_create_env, romanian_UTF_8_close_env, romanian_UTF_8_stem},
  {"ru", ENC_UTF_8, russian_UTF_8_create_env, russian_UTF_8_close_env, russian_UTF_8_stem},
  {"rum", ENC_UTF_8, romanian_UTF_8_create_env, romanian_UTF_8_close_env, romanian_UTF_8_stem},
  {"rus", ENC_UTF_8, russian_UTF_8_create_env, russian_UTF_8_close_env, russian_UTF_8_stem},
  {"russian", ENC_UTF_8, russian_UTF_8_create_env, russian_UTF_8_close_env, russian_UTF_8_stem},
  {"spa", ENC_UTF_8, spanish_UTF_8_create_env, spanish_UTF_8_close_env, spanish_UTF_8_stem},
  {"spanish", ENC_UTF_8, spanish_UTF_8_create_env, spanish_UTF_8_close_env, spanish_UTF_8_stem},
  {"sv", ENC_UTF_8, swedish_UTF_8_create_env, swedish_UTF_8_close_env, swedish_UTF_8_stem},
  {"swe", ENC_UTF_8, swedish_UTF_8_create_env, swedish_UTF_8_close_env, swedish_UTF_8_stem},
  {"swedish", ENC_UTF_8, swedish_UTF_8_create_env, swedish_UTF_8_close_env, swedish_UTF_8_stem},
  {"ta", ENC_UTF_8, tamil_UTF_8_create_env, tamil_UTF_8_close_env, tamil_UTF_8_stem},
  {"tam", ENC_UTF_8, tamil_UTF_8_create_env, tamil_UTF_8_close_env, tamil_UTF_8_stem},
  {"tamil", ENC_UTF_8, tamil_UTF_8_create_env, tamil_UTF_8_close_env, tamil_UTF_8_stem},
  {"tr", ENC_UTF_8, turkish_UTF_8_create_env, turkish_UTF_8_close_env, turkish_UTF_8_stem},
  {"tur", ENC_UTF_8, turkish_UTF_8_create_env, turkish_UTF_8_close_env, turkish_UTF_8_stem},
  {"turkish", ENC_UTF_8, turkish_UTF_8_create_env, turkish_UTF_8_close_env, turkish_UTF_8_stem},
  {0,ENC_UNKNOWN,0,0,0}
};

static const char * algorithm_names[] = {
  "arabic",
  "danish",
  "dutch",
  "english",
  "finnish",
  "french",
  "german",
  "greek",
  "hungarian",
  "indonesian",
  "irish",
  "italian",
  "lithuanian",
  "nepali",
  "norwegian",
  "portuguese",
  "romanian",
  "russian",
  "spanish",
  "swedish",
  "tamil",
  "turkish",
  0
};



#define CREATE_SIZE 1

static symbol * create_s(void) {
    symbol * p;
    void * mem = malloc(HEAD + (CREATE_SIZE + 1) * sizeof(symbol));
    if (mem == NULL) return NULL;
    p = (symbol *) (HEAD + (char *) mem);
    CAPACITY(p) = CREATE_SIZE;
    SET_SIZE(p, 0);
    return p;
}

static void lose_s(symbol * p) {
    if (p == NULL) return;
    free((char *) p - HEAD);
}

/*
   new_p = skip_utf8(p, c, lb, l, n); skips n characters forwards from p + c
   if n +ve, or n characters backwards from p + c - 1 if n -ve. new_p is the new
   position, or -1 on failure.

   -- used to implement hop and next in the utf8 case.
*/

static int skip_utf8(const symbol * p, int c, int lb, int l, int n) {
    int b;
    if (n >= 0) {
        for (; n > 0; n--) {
            if (c >= l) return -1;
            b = p[c++];
            if (b >= 0xC0) {   /* 1100 0000 */
                while (c < l) {
                    b = p[c];
                    if (b >= 0xC0 || b < 0x80) break;
                    /* break unless b is 10------ */
                    c++;
                }
            }
        }
    } else {
        for (; n < 0; n++) {
            if (c <= lb) return -1;
            b = p[--c];
            if (b >= 0x80) {   /* 1000 0000 */
                while (c > lb) {
                    b = p[c];
                    if (b >= 0xC0) break; /* 1100 0000 */
                    c--;
                }
            }
        }
    }
    return c;
}

/* Code for character groupings: utf8 cases */

static int get_utf8(const symbol * p, int c, int l, int * slot) {
    int b0, b1;
    if (c >= l) return 0;
    b0 = p[c++];
    if (b0 < 0xC0 || c == l) {   /* 1100 0000 */
        * slot = b0; return 1;
    }
    b1 = p[c++];
    if (b0 < 0xE0 || c == l) {   /* 1110 0000 */
        * slot = (b0 & 0x1F) << 6 | (b1 & 0x3F); return 2;
    }
    * slot = (b0 & 0xF) << 12 | (b1 & 0x3F) << 6 | (p[c] & 0x3F); return 3;
}

static int get_b_utf8(const symbol * p, int c, int lb, int * slot) {
    int b0, b1;
    if (c <= lb) return 0;
    b0 = p[--c];
    if (b0 < 0x80 || c == lb) {   /* 1000 0000 */
        * slot = b0; return 1;
    }
    b1 = p[--c];
    if (b1 >= 0xC0 || c == lb) {   /* 1100 0000 */
        * slot = (b1 & 0x1F) << 6 | (b0 & 0x3F); return 2;
    }
    * slot = (p[--c] & 0xF) << 12 | (b1 & 0x3F) << 6 | (b0 & 0x3F); return 3;
}

static int in_grouping_U(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) {
    do {
	int ch;
	int w = get_utf8(z->p, z->c, z->l, & ch);
	if (!w) return -1;
	if (ch > max || (ch -= min) < 0 || (s[ch >> 3] & (0X1 << (ch & 0X7))) == 0)
	    return w;
	z->c += w;
    } while (repeat);
    return 0;
}

static int in_grouping_b_U(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) {
    do {
	int ch;
	int w = get_b_utf8(z->p, z->c, z->lb, & ch);
	if (!w) return -1;
	if (ch > max || (ch -= min) < 0 || (s[ch >> 3] & (0X1 << (ch & 0X7))) == 0)
	    return w;
	z->c -= w;
    } while (repeat);
    return 0;
}

static int out_grouping_U(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) {
    do {
	int ch;
	int w = get_utf8(z->p, z->c, z->l, & ch);
	if (!w) return -1;
	if (!(ch > max || (ch -= min) < 0 || (s[ch >> 3] & (0X1 << (ch & 0X7))) == 0))
	    return w;
	z->c += w;
    } while (repeat);
    return 0;
}

static int out_grouping_b_U(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) {
    do {
	int ch;
	int w = get_b_utf8(z->p, z->c, z->lb, & ch);
	if (!w) return -1;
	if (!(ch > max || (ch -= min) < 0 || (s[ch >> 3] & (0X1 << (ch & 0X7))) == 0))
	    return w;
	z->c -= w;
    } while (repeat);
    return 0;
}

/* Code for character groupings: non-utf8 cases */

static int in_grouping(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) {
    do {
	int ch;
	if (z->c >= z->l) return -1;
	ch = z->p[z->c];
	if (ch > max || (ch -= min) < 0 || (s[ch >> 3] & (0X1 << (ch & 0X7))) == 0)
	    return 1;
	z->c++;
    } while (repeat);
    return 0;
}

static int in_grouping_b(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) {
    do {
	int ch;
	if (z->c <= z->lb) return -1;
	ch = z->p[z->c - 1];
	if (ch > max || (ch -= min) < 0 || (s[ch >> 3] & (0X1 << (ch & 0X7))) == 0)
	    return 1;
	z->c--;
    } while (repeat);
    return 0;
}

static int out_grouping(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) {
    do {
	int ch;
	if (z->c >= z->l) return -1;
	ch = z->p[z->c];
	if (!(ch > max || (ch -= min) < 0 || (s[ch >> 3] & (0X1 << (ch & 0X7))) == 0))
	    return 1;
	z->c++;
    } while (repeat);
    return 0;
}

static int out_grouping_b(struct SN_env * z, const unsigned char * s, int min, int max, int repeat) {
    do {
	int ch;
	if (z->c <= z->lb) return -1;
	ch = z->p[z->c - 1];
	if (!(ch > max || (ch -= min) < 0 || (s[ch >> 3] & (0X1 << (ch & 0X7))) == 0))
	    return 1;
	z->c--;
    } while (repeat);
    return 0;
}

static int eq_s(struct SN_env * z, int s_size, const symbol * s) {
    if (z->l - z->c < s_size || memcmp(z->p + z->c, s, s_size * sizeof(symbol)) != 0) return 0;
    z->c += s_size; return 1;
}

static int eq_s_b(struct SN_env * z, int s_size, const symbol * s) {
    if (z->c - z->lb < s_size || memcmp(z->p + z->c - s_size, s, s_size * sizeof(symbol)) != 0) return 0;
    z->c -= s_size; return 1;
}

static int eq_v(struct SN_env * z, const symbol * p) {
    return eq_s(z, SIZE(p), p);
}

static int eq_v_b(struct SN_env * z, const symbol * p) {
    return eq_s_b(z, SIZE(p), p);
}

static int find_among(struct SN_env * z, const struct among * v, int v_size) {

    int i = 0;
    int j = v_size;

    int c = z->c; int l = z->l;
    symbol * q = z->p + c;

    const struct among * w;

    int common_i = 0;
    int common_j = 0;

    int first_key_inspected = 0;

    while(1) {
        int k = i + ((j - i) >> 1);
        int diff = 0;
        int common = common_i < common_j ? common_i : common_j; /* smaller */
        w = v + k;
        {
            int i2; for (i2 = common; i2 < w->s_size; i2++) {
                if (c + common == l) { diff = -1; break; }
                diff = q[common] - w->s[i2];
                if (diff != 0) break;
                common++;
            }
        }
        if (diff < 0) { j = k; common_j = common; }
                 else { i = k; common_i = common; }
        if (j - i <= 1) {
            if (i > 0) break; /* v->s has been inspected */
            if (j == i) break; /* only one item in v */

            /* - but now we need to go round once more to get
               v->s inspected. This looks messy, but is actually
               the optimal approach.  */

            if (first_key_inspected) break;
            first_key_inspected = 1;
        }
    }
    while(1) {
        w = v + i;
        if (common_i >= w->s_size) {
            z->c = c + w->s_size;
            if (w->function == 0) return w->result;
            {
                int res = w->function(z);
                z->c = c + w->s_size;
                if (res) return w->result;
            }
        }
        i = w->substring_i;
        if (i < 0) return 0;
    }
}

/* find_among_b is for backwards processing. Same comments apply */

static int find_among_b(struct SN_env * z, const struct among * v, int v_size) {

    int i = 0;
    int j = v_size;

    int c = z->c; int lb = z->lb;
    symbol * q = z->p + c - 1;

    const struct among * w;

    int common_i = 0;
    int common_j = 0;

    int first_key_inspected = 0;

    while(1) {
        int k = i + ((j - i) >> 1);
        int diff = 0;
        int common = common_i < common_j ? common_i : common_j;
        w = v + k;
        {
            int i2; for (i2 = w->s_size - 1 - common; i2 >= 0; i2--) {
                if (c - common == lb) { diff = -1; break; }
                diff = q[- common] - w->s[i2];
                if (diff != 0) break;
                common++;
            }
        }
        if (diff < 0) { j = k; common_j = common; }
                 else { i = k; common_i = common; }
        if (j - i <= 1) {
            if (i > 0) break;
            if (j == i) break;
            if (first_key_inspected) break;
            first_key_inspected = 1;
        }
    }
    while(1) {
        w = v + i;
        if (common_i >= w->s_size) {
            z->c = c - w->s_size;
            if (w->function == 0) return w->result;
            {
                int res = w->function(z);
                z->c = c - w->s_size;
                if (res) return w->result;
            }
        }
        i = w->substring_i;
        if (i < 0) return 0;
    }
}


/* Increase the size of the buffer pointed to by p to at least n symbols.
 * If insufficient memory, returns NULL and frees the old buffer.
 */
static symbol * increase_size(symbol * p, int n) {
    symbol * q;
    int new_size = n + 20;
    void * mem = realloc((char *) p - HEAD,
                         HEAD + (new_size + 1) * sizeof(symbol));
    if (mem == NULL) {
        lose_s(p);
        return NULL;
    }
    q = (symbol *) (HEAD + (char *)mem);
    CAPACITY(q) = new_size;
    return q;
}

/* to replace symbols between c_bra and c_ket in z->p by the
   s_size symbols at s.
   Returns 0 on success, -1 on error.
   Also, frees z->p (and sets it to NULL) on error.
*/
static int replace_s(struct SN_env * z, int c_bra, int c_ket, int s_size, const symbol * s, int * adjptr)
{
    int adjustment;
    int len;
    if (z->p == NULL) {
        z->p = create_s();
        if (z->p == NULL) return -1;
    }
    adjustment = s_size - (c_ket - c_bra);
    len = SIZE(z->p);
    if (adjustment != 0) {
        if (adjustment + len > CAPACITY(z->p)) {
            z->p = increase_size(z->p, adjustment + len);
            if (z->p == NULL) return -1;
        }
        memmove(z->p + c_ket + adjustment,
                z->p + c_ket,
                (len - c_ket) * sizeof(symbol));
        SET_SIZE(z->p, adjustment + len);
        z->l += adjustment;
        if (z->c >= c_ket)
            z->c += adjustment;
        else
            if (z->c > c_bra)
                z->c = c_bra;
    }
    if (s_size) memmove(z->p + c_bra, s, s_size * sizeof(symbol));
    if (adjptr != NULL)
        *adjptr = adjustment;
    return 0;
}

static int slice_check(struct SN_env * z) {

    if (z->bra < 0 ||
        z->bra > z->ket ||
        z->ket > z->l ||
        z->p == NULL ||
        z->l > SIZE(z->p)) /* this line could be removed */
    {
#if 0
        fprintf(stderr, "faulty slice operation:\n");
        debug(z, -1, 0);
#endif
        return -1;
    }
    return 0;
}

static int slice_from_s(struct SN_env * z, int s_size, const symbol * s) {
    if (slice_check(z)) return -1;
    return replace_s(z, z->bra, z->ket, s_size, s, NULL);
}

static int slice_from_v(struct SN_env * z, const symbol * p) {
    return slice_from_s(z, SIZE(p), p);
}

static int slice_del(struct SN_env * z) {
    return slice_from_s(z, 0, 0);
}

static int insert_s(struct SN_env * z, int bra, int ket, int s_size, const symbol * s) {
    int adjustment;
    if (replace_s(z, bra, ket, s_size, s, &adjustment))
        return -1;
    if (bra <= z->bra) z->bra += adjustment;
    if (bra <= z->ket) z->ket += adjustment;
    return 0;
}

static int insert_v(struct SN_env * z, int bra, int ket, const symbol * p) {
    return insert_s(z, bra, ket, SIZE(p), p);
}

static symbol * slice_to(struct SN_env * z, symbol * p) {
    if (slice_check(z)) {
        lose_s(p);
        return NULL;
    }
    {
        int len = z->ket - z->bra;
        if (CAPACITY(p) < len) {
            p = increase_size(p, len);
            if (p == NULL)
                return NULL;
        }
        memmove(p, z->p + z->bra, len * sizeof(symbol));
        SET_SIZE(p, len);
    }
    return p;
}

static symbol * assign_to(struct SN_env * z, symbol * p) {
    int len = z->l;
    if (CAPACITY(p) < len) {
        p = increase_size(p, len);
        if (p == NULL)
            return NULL;
    }
    memmove(p, z->p, len * sizeof(symbol));
    SET_SIZE(p, len);
    return p;
}

static int len_utf8(const symbol * p) {
    int size = SIZE(p);
    int len = 0;
    while (size--) {
        symbol b = *p++;
        if (b >= 0xC0 || b < 0x80) ++len;
    }
    return len;
}


static struct SN_env * SN_create_env(int S_size, int I_size, int B_size)
{
    struct SN_env * z = (struct SN_env *) calloc(1, sizeof(struct SN_env));
    if (z == NULL) return NULL;
    z->p = create_s();
    if (z->p == NULL) goto error;
    if (S_size)
    {
        int i;
        z->S = (symbol * *) calloc(S_size, sizeof(symbol *));
        if (z->S == NULL) goto error;

        for (i = 0; i < S_size; i++)
        {
            z->S[i] = create_s();
            if (z->S[i] == NULL) goto error;
        }
    }

    if (I_size)
    {
        z->I = (int *) calloc(I_size, sizeof(int));
        if (z->I == NULL) goto error;
    }

    if (B_size)
    {
        z->B = (unsigned char *) calloc(B_size, sizeof(unsigned char));
        if (z->B == NULL) goto error;
    }

    return z;
error:
    SN_close_env(z, S_size);
    return NULL;
}

static void SN_close_env(struct SN_env * z, int S_size)
{
    if (z == NULL) return;
    if (S_size)
    {
        int i;
        for (i = 0; i < S_size; i++)
        {
            lose_s(z->S[i]);
        }
        free(z->S);
    }
    free(z->I);
    free(z->B);
    if (z->p) lose_s(z->p);
    free(z);
}

static int SN_set_current(struct SN_env * z, int size, const symbol * s)
{
    int err = replace_s(z, 0, z->l, size, s, NULL);
    z->c = 0;
    return err;
}


struct sb_stemmer {
    struct SN_env * (*create)(void);
    void (*close)(struct SN_env *);
    int (*stem)(struct SN_env *);

    struct SN_env * env;
};

extern const char **
sb_stemmer_list(void)
{
    return algorithm_names;
}

static stemmer_encoding_t
sb_getenc(const char * charenc)
{
    struct stemmer_encoding * encoding;
    if (charenc == NULL) return ENC_UTF_8;
    for (encoding = encodings; encoding->name != 0; encoding++) {
	if (strcmp(encoding->name, charenc) == 0) break;
    }
    if (encoding->name == NULL) return ENC_UNKNOWN;
    return encoding->enc;
}

extern struct sb_stemmer *
sb_stemmer_new(const char * algorithm, const char * charenc)
{
    stemmer_encoding_t enc;
    struct stemmer_modules * module;
    struct sb_stemmer * stemmer;

    enc = sb_getenc(charenc);
    if (enc == ENC_UNKNOWN) return NULL;

    for (module = modules; module->name != 0; module++) {
	if (strcmp(module->name, algorithm) == 0 && module->enc == enc) break;
    }
    if (module->name == NULL) return NULL;

    stemmer = (struct sb_stemmer *) malloc(sizeof(struct sb_stemmer));
    if (stemmer == NULL) return NULL;

    stemmer->create = module->create;
    stemmer->close = module->close;
    stemmer->stem = module->stem;

    stemmer->env = stemmer->create();
    if (stemmer->env == NULL)
    {
        sb_stemmer_delete(stemmer);
        return NULL;
    }

    return stemmer;
}

void
sb_stemmer_delete(struct sb_stemmer * stemmer)
{
    if (stemmer == 0) return;
    if (stemmer->close) {
        stemmer->close(stemmer->env);
        stemmer->close = 0;
    }
    free(stemmer);
}

const sb_symbol *
sb_stemmer_stem(struct sb_stemmer * stemmer, const sb_symbol * word, int size)
{
    int ret;
    if (SN_set_current(stemmer->env, size, (const symbol *)(word)))
    {
        stemmer->env->l = 0;
        return NULL;
    }
    ret = stemmer->stem(stemmer->env);
    if (ret < 0) return NULL;
    stemmer->env->p[stemmer->env->l] = 0;
    return (const sb_symbol *)(stemmer->env->p);
}

int
sb_stemmer_length(struct sb_stemmer * stemmer)
{
    return stemmer->env->l;
}
