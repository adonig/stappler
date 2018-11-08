/* This file was generated automatically by the Snowball to ISO C compiler */
/* http://snowballstem.org/ */
static int swedish_UTF_8_stem(struct SN_env * z);
static int swedish_UTF_8_r_other_suffix(struct SN_env * z);
static int swedish_UTF_8_r_consonant_pair(struct SN_env * z);
static int swedish_UTF_8_r_main_suffix(struct SN_env * z);
static int swedish_UTF_8_r_mark_regions(struct SN_env * z);


static struct SN_env * swedish_UTF_8_create_env(void);
static void swedish_UTF_8_close_env(struct SN_env * z);


static const symbol swedish_UTF_8_s_0_0[1] = { 'a' };
static const symbol swedish_UTF_8_s_0_1[4] = { 'a', 'r', 'n', 'a' };
static const symbol swedish_UTF_8_s_0_2[4] = { 'e', 'r', 'n', 'a' };
static const symbol swedish_UTF_8_s_0_3[7] = { 'h', 'e', 't', 'e', 'r', 'n', 'a' };
static const symbol swedish_UTF_8_s_0_4[4] = { 'o', 'r', 'n', 'a' };
static const symbol swedish_UTF_8_s_0_5[2] = { 'a', 'd' };
static const symbol swedish_UTF_8_s_0_6[1] = { 'e' };
static const symbol swedish_UTF_8_s_0_7[3] = { 'a', 'd', 'e' };
static const symbol swedish_UTF_8_s_0_8[4] = { 'a', 'n', 'd', 'e' };
static const symbol swedish_UTF_8_s_0_9[4] = { 'a', 'r', 'n', 'e' };
static const symbol swedish_UTF_8_s_0_10[3] = { 'a', 'r', 'e' };
static const symbol swedish_UTF_8_s_0_11[4] = { 'a', 's', 't', 'e' };
static const symbol swedish_UTF_8_s_0_12[2] = { 'e', 'n' };
static const symbol swedish_UTF_8_s_0_13[5] = { 'a', 'n', 'd', 'e', 'n' };
static const symbol swedish_UTF_8_s_0_14[4] = { 'a', 'r', 'e', 'n' };
static const symbol swedish_UTF_8_s_0_15[5] = { 'h', 'e', 't', 'e', 'n' };
static const symbol swedish_UTF_8_s_0_16[3] = { 'e', 'r', 'n' };
static const symbol swedish_UTF_8_s_0_17[2] = { 'a', 'r' };
static const symbol swedish_UTF_8_s_0_18[2] = { 'e', 'r' };
static const symbol swedish_UTF_8_s_0_19[5] = { 'h', 'e', 't', 'e', 'r' };
static const symbol swedish_UTF_8_s_0_20[2] = { 'o', 'r' };
static const symbol swedish_UTF_8_s_0_21[1] = { 's' };
static const symbol swedish_UTF_8_s_0_22[2] = { 'a', 's' };
static const symbol swedish_UTF_8_s_0_23[5] = { 'a', 'r', 'n', 'a', 's' };
static const symbol swedish_UTF_8_s_0_24[5] = { 'e', 'r', 'n', 'a', 's' };
static const symbol swedish_UTF_8_s_0_25[5] = { 'o', 'r', 'n', 'a', 's' };
static const symbol swedish_UTF_8_s_0_26[2] = { 'e', 's' };
static const symbol swedish_UTF_8_s_0_27[4] = { 'a', 'd', 'e', 's' };
static const symbol swedish_UTF_8_s_0_28[5] = { 'a', 'n', 'd', 'e', 's' };
static const symbol swedish_UTF_8_s_0_29[3] = { 'e', 'n', 's' };
static const symbol swedish_UTF_8_s_0_30[5] = { 'a', 'r', 'e', 'n', 's' };
static const symbol swedish_UTF_8_s_0_31[6] = { 'h', 'e', 't', 'e', 'n', 's' };
static const symbol swedish_UTF_8_s_0_32[4] = { 'e', 'r', 'n', 's' };
static const symbol swedish_UTF_8_s_0_33[2] = { 'a', 't' };
static const symbol swedish_UTF_8_s_0_34[5] = { 'a', 'n', 'd', 'e', 't' };
static const symbol swedish_UTF_8_s_0_35[3] = { 'h', 'e', 't' };
static const symbol swedish_UTF_8_s_0_36[3] = { 'a', 's', 't' };

static const struct among swedish_UTF_8_a_0[37] =
{
/*  0 */ { 1, swedish_UTF_8_s_0_0, -1, 1, 0},
/*  1 */ { 4, swedish_UTF_8_s_0_1, 0, 1, 0},
/*  2 */ { 4, swedish_UTF_8_s_0_2, 0, 1, 0},
/*  3 */ { 7, swedish_UTF_8_s_0_3, 2, 1, 0},
/*  4 */ { 4, swedish_UTF_8_s_0_4, 0, 1, 0},
/*  5 */ { 2, swedish_UTF_8_s_0_5, -1, 1, 0},
/*  6 */ { 1, swedish_UTF_8_s_0_6, -1, 1, 0},
/*  7 */ { 3, swedish_UTF_8_s_0_7, 6, 1, 0},
/*  8 */ { 4, swedish_UTF_8_s_0_8, 6, 1, 0},
/*  9 */ { 4, swedish_UTF_8_s_0_9, 6, 1, 0},
/* 10 */ { 3, swedish_UTF_8_s_0_10, 6, 1, 0},
/* 11 */ { 4, swedish_UTF_8_s_0_11, 6, 1, 0},
/* 12 */ { 2, swedish_UTF_8_s_0_12, -1, 1, 0},
/* 13 */ { 5, swedish_UTF_8_s_0_13, 12, 1, 0},
/* 14 */ { 4, swedish_UTF_8_s_0_14, 12, 1, 0},
/* 15 */ { 5, swedish_UTF_8_s_0_15, 12, 1, 0},
/* 16 */ { 3, swedish_UTF_8_s_0_16, -1, 1, 0},
/* 17 */ { 2, swedish_UTF_8_s_0_17, -1, 1, 0},
/* 18 */ { 2, swedish_UTF_8_s_0_18, -1, 1, 0},
/* 19 */ { 5, swedish_UTF_8_s_0_19, 18, 1, 0},
/* 20 */ { 2, swedish_UTF_8_s_0_20, -1, 1, 0},
/* 21 */ { 1, swedish_UTF_8_s_0_21, -1, 2, 0},
/* 22 */ { 2, swedish_UTF_8_s_0_22, 21, 1, 0},
/* 23 */ { 5, swedish_UTF_8_s_0_23, 22, 1, 0},
/* 24 */ { 5, swedish_UTF_8_s_0_24, 22, 1, 0},
/* 25 */ { 5, swedish_UTF_8_s_0_25, 22, 1, 0},
/* 26 */ { 2, swedish_UTF_8_s_0_26, 21, 1, 0},
/* 27 */ { 4, swedish_UTF_8_s_0_27, 26, 1, 0},
/* 28 */ { 5, swedish_UTF_8_s_0_28, 26, 1, 0},
/* 29 */ { 3, swedish_UTF_8_s_0_29, 21, 1, 0},
/* 30 */ { 5, swedish_UTF_8_s_0_30, 29, 1, 0},
/* 31 */ { 6, swedish_UTF_8_s_0_31, 29, 1, 0},
/* 32 */ { 4, swedish_UTF_8_s_0_32, 21, 1, 0},
/* 33 */ { 2, swedish_UTF_8_s_0_33, -1, 1, 0},
/* 34 */ { 5, swedish_UTF_8_s_0_34, -1, 1, 0},
/* 35 */ { 3, swedish_UTF_8_s_0_35, -1, 1, 0},
/* 36 */ { 3, swedish_UTF_8_s_0_36, -1, 1, 0}
};

static const symbol swedish_UTF_8_s_1_0[2] = { 'd', 'd' };
static const symbol swedish_UTF_8_s_1_1[2] = { 'g', 'd' };
static const symbol swedish_UTF_8_s_1_2[2] = { 'n', 'n' };
static const symbol swedish_UTF_8_s_1_3[2] = { 'd', 't' };
static const symbol swedish_UTF_8_s_1_4[2] = { 'g', 't' };
static const symbol swedish_UTF_8_s_1_5[2] = { 'k', 't' };
static const symbol swedish_UTF_8_s_1_6[2] = { 't', 't' };

static const struct among swedish_UTF_8_a_1[7] =
{
/*  0 */ { 2, swedish_UTF_8_s_1_0, -1, -1, 0},
/*  1 */ { 2, swedish_UTF_8_s_1_1, -1, -1, 0},
/*  2 */ { 2, swedish_UTF_8_s_1_2, -1, -1, 0},
/*  3 */ { 2, swedish_UTF_8_s_1_3, -1, -1, 0},
/*  4 */ { 2, swedish_UTF_8_s_1_4, -1, -1, 0},
/*  5 */ { 2, swedish_UTF_8_s_1_5, -1, -1, 0},
/*  6 */ { 2, swedish_UTF_8_s_1_6, -1, -1, 0}
};

static const symbol swedish_UTF_8_s_2_0[2] = { 'i', 'g' };
static const symbol swedish_UTF_8_s_2_1[3] = { 'l', 'i', 'g' };
static const symbol swedish_UTF_8_s_2_2[3] = { 'e', 'l', 's' };
static const symbol swedish_UTF_8_s_2_3[5] = { 'f', 'u', 'l', 'l', 't' };
static const symbol swedish_UTF_8_s_2_4[5] = { 'l', 0xC3, 0xB6, 's', 't' };

static const struct among swedish_UTF_8_a_2[5] =
{
/*  0 */ { 2, swedish_UTF_8_s_2_0, -1, 1, 0},
/*  1 */ { 3, swedish_UTF_8_s_2_1, 0, 1, 0},
/*  2 */ { 3, swedish_UTF_8_s_2_2, -1, 1, 0},
/*  3 */ { 5, swedish_UTF_8_s_2_3, -1, 3, 0},
/*  4 */ { 5, swedish_UTF_8_s_2_4, -1, 2, 0}
};

static const unsigned char swedish_UTF_8_g_v[] = { 17, 65, 16, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 32 };

static const unsigned char swedish_UTF_8_g_s_ending[] = { 119, 127, 149 };

static const symbol swedish_UTF_8_s_0[] = { 'l', 0xC3, 0xB6, 's' };
static const symbol swedish_UTF_8_s_1[] = { 'f', 'u', 'l', 'l' };

static int swedish_UTF_8_r_mark_regions(struct SN_env * z) { /* forwardmode */
    z->I[0] = z->l; /* $p1 = <integer expression>, line 28 */
    {   int c_test1 = z->c; /* test, line 29 */
        {   int ret = skip_utf8(z->p, z->c, 0, z->l, + 3); /* hop, line 29 */
            if (ret < 0) return 0;
            z->c = ret;
        }
        z->I[1] = z->c; /* setmark x, line 29 */
        z->c = c_test1;
    }
    if (out_grouping_U(z, swedish_UTF_8_g_v, 97, 246, 1) < 0) return 0; /* goto */ /* grouping v, line 30 */
    {    /* gopast */ /* non v, line 30 */
        int ret = in_grouping_U(z, swedish_UTF_8_g_v, 97, 246, 1);
        if (ret < 0) return 0;
        z->c += ret;
    }
    z->I[0] = z->c; /* setmark p1, line 30 */
    /* try, line 31 */
    if (!(z->I[0] < z->I[1])) goto lab0; /* $(<integer expression> < <integer expression>), line 31 */
    z->I[0] = z->I[1]; /* $p1 = <integer expression>, line 31 */
lab0:
    return 1;
}

static int swedish_UTF_8_r_main_suffix(struct SN_env * z) { /* backwardmode */
    int among_var;

    {   int mlimit1; /* setlimit, line 37 */
        if (z->c < z->I[0]) return 0;
        mlimit1 = z->lb; z->lb = z->I[0];
        z->ket = z->c; /* [, line 37 */
        if (z->c <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((1851442 >> (z->p[z->c - 1] & 0x1f)) & 1)) { z->lb = mlimit1; return 0; } /* substring, line 37 */
        among_var = find_among_b(z, swedish_UTF_8_a_0, 37);
        if (!(among_var)) { z->lb = mlimit1; return 0; }
        z->bra = z->c; /* ], line 37 */
        z->lb = mlimit1;
    }
    switch (among_var) { /* among, line 38 */
        case 1:
            {   int ret = slice_del(z); /* delete, line 44 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            if (in_grouping_b_U(z, swedish_UTF_8_g_s_ending, 98, 121, 0)) return 0; /* grouping s_ending, line 46 */
            {   int ret = slice_del(z); /* delete, line 46 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int swedish_UTF_8_r_consonant_pair(struct SN_env * z) { /* backwardmode */

    {   int mlimit1; /* setlimit, line 50 */
        if (z->c < z->I[0]) return 0;
        mlimit1 = z->lb; z->lb = z->I[0];
        {   int m2 = z->l - z->c; (void)m2; /* and, line 52 */
            if (z->c - 1 <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((1064976 >> (z->p[z->c - 1] & 0x1f)) & 1)) { z->lb = mlimit1; return 0; } /* among, line 51 */
            if (!(find_among_b(z, swedish_UTF_8_a_1, 7))) { z->lb = mlimit1; return 0; }
            z->c = z->l - m2;
            z->ket = z->c; /* [, line 52 */
            {   int ret = skip_utf8(z->p, z->c, z->lb, 0, -1);
                if (ret < 0) { z->lb = mlimit1; return 0; }
                z->c = ret; /* next, line 52 */
            }
            z->bra = z->c; /* ], line 52 */
            {   int ret = slice_del(z); /* delete, line 52 */
                if (ret < 0) return ret;
            }
        }
        z->lb = mlimit1;
    }
    return 1;
}

static int swedish_UTF_8_r_other_suffix(struct SN_env * z) { /* backwardmode */
    int among_var;

    {   int mlimit1; /* setlimit, line 55 */
        if (z->c < z->I[0]) return 0;
        mlimit1 = z->lb; z->lb = z->I[0];
        z->ket = z->c; /* [, line 56 */
        if (z->c - 1 <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((1572992 >> (z->p[z->c - 1] & 0x1f)) & 1)) { z->lb = mlimit1; return 0; } /* substring, line 56 */
        among_var = find_among_b(z, swedish_UTF_8_a_2, 5);
        if (!(among_var)) { z->lb = mlimit1; return 0; }
        z->bra = z->c; /* ], line 56 */
        switch (among_var) { /* among, line 56 */
            case 1:
                {   int ret = slice_del(z); /* delete, line 57 */
                    if (ret < 0) return ret;
                }
                break;
            case 2:
                {   int ret = slice_from_s(z, 4, swedish_UTF_8_s_0); /* <-, line 58 */
                    if (ret < 0) return ret;
                }
                break;
            case 3:
                {   int ret = slice_from_s(z, 4, swedish_UTF_8_s_1); /* <-, line 59 */
                    if (ret < 0) return ret;
                }
                break;
        }
        z->lb = mlimit1;
    }
    return 1;
}

static int swedish_UTF_8_stem(struct SN_env * z) { /* forwardmode */
    {   int c1 = z->c; /* do, line 66 */
        {   int ret = swedish_UTF_8_r_mark_regions(z); /* call mark_regions, line 66 */
            if (ret == 0) goto lab0;
            if (ret < 0) return ret;
        }
    lab0:
        z->c = c1;
    }
    z->lb = z->c; z->c = z->l; /* backwards, line 67 */

    {   int m2 = z->l - z->c; (void)m2; /* do, line 68 */
        {   int ret = swedish_UTF_8_r_main_suffix(z); /* call main_suffix, line 68 */
            if (ret == 0) goto lab1;
            if (ret < 0) return ret;
        }
    lab1:
        z->c = z->l - m2;
    }
    {   int m3 = z->l - z->c; (void)m3; /* do, line 69 */
        {   int ret = swedish_UTF_8_r_consonant_pair(z); /* call consonant_pair, line 69 */
            if (ret == 0) goto lab2;
            if (ret < 0) return ret;
        }
    lab2:
        z->c = z->l - m3;
    }
    {   int m4 = z->l - z->c; (void)m4; /* do, line 70 */
        {   int ret = swedish_UTF_8_r_other_suffix(z); /* call other_suffix, line 70 */
            if (ret == 0) goto lab3;
            if (ret < 0) return ret;
        }
    lab3:
        z->c = z->l - m4;
    }
    z->c = z->lb;
    return 1;
}

static struct SN_env * swedish_UTF_8_create_env(void) { return SN_create_env(0, 2, 0); }

static void swedish_UTF_8_close_env(struct SN_env * z) { SN_close_env(z, 0); }

