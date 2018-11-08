/* This file was generated automatically by the Snowball to ISO C compiler */
/* http://snowballstem.org/ */
static int hungarian_UTF_8_stem(struct SN_env * z);
static int hungarian_UTF_8_r_double(struct SN_env * z);
static int hungarian_UTF_8_r_undouble(struct SN_env * z);
static int hungarian_UTF_8_r_factive(struct SN_env * z);
static int hungarian_UTF_8_r_instrum(struct SN_env * z);
static int hungarian_UTF_8_r_plur_owner(struct SN_env * z);
static int hungarian_UTF_8_r_sing_owner(struct SN_env * z);
static int hungarian_UTF_8_r_owned(struct SN_env * z);
static int hungarian_UTF_8_r_plural(struct SN_env * z);
static int hungarian_UTF_8_r_case_other(struct SN_env * z);
static int hungarian_UTF_8_r_case_special(struct SN_env * z);
static int hungarian_UTF_8_r_case(struct SN_env * z);
static int hungarian_UTF_8_r_v_ending(struct SN_env * z);
static int hungarian_UTF_8_r_R1(struct SN_env * z);
static int hungarian_UTF_8_r_mark_regions(struct SN_env * z);


static struct SN_env * hungarian_UTF_8_create_env(void);
static void hungarian_UTF_8_close_env(struct SN_env * z);


static const symbol hungarian_UTF_8_s_0_0[2] = { 'c', 's' };
static const symbol hungarian_UTF_8_s_0_1[3] = { 'd', 'z', 's' };
static const symbol hungarian_UTF_8_s_0_2[2] = { 'g', 'y' };
static const symbol hungarian_UTF_8_s_0_3[2] = { 'l', 'y' };
static const symbol hungarian_UTF_8_s_0_4[2] = { 'n', 'y' };
static const symbol hungarian_UTF_8_s_0_5[2] = { 's', 'z' };
static const symbol hungarian_UTF_8_s_0_6[2] = { 't', 'y' };
static const symbol hungarian_UTF_8_s_0_7[2] = { 'z', 's' };

static const struct among hungarian_UTF_8_a_0[8] =
{
/*  0 */ { 2, hungarian_UTF_8_s_0_0, -1, -1, 0},
/*  1 */ { 3, hungarian_UTF_8_s_0_1, -1, -1, 0},
/*  2 */ { 2, hungarian_UTF_8_s_0_2, -1, -1, 0},
/*  3 */ { 2, hungarian_UTF_8_s_0_3, -1, -1, 0},
/*  4 */ { 2, hungarian_UTF_8_s_0_4, -1, -1, 0},
/*  5 */ { 2, hungarian_UTF_8_s_0_5, -1, -1, 0},
/*  6 */ { 2, hungarian_UTF_8_s_0_6, -1, -1, 0},
/*  7 */ { 2, hungarian_UTF_8_s_0_7, -1, -1, 0}
};

static const symbol hungarian_UTF_8_s_1_0[2] = { 0xC3, 0xA1 };
static const symbol hungarian_UTF_8_s_1_1[2] = { 0xC3, 0xA9 };

static const struct among hungarian_UTF_8_a_1[2] =
{
/*  0 */ { 2, hungarian_UTF_8_s_1_0, -1, 1, 0},
/*  1 */ { 2, hungarian_UTF_8_s_1_1, -1, 2, 0}
};

static const symbol hungarian_UTF_8_s_2_0[2] = { 'b', 'b' };
static const symbol hungarian_UTF_8_s_2_1[2] = { 'c', 'c' };
static const symbol hungarian_UTF_8_s_2_2[2] = { 'd', 'd' };
static const symbol hungarian_UTF_8_s_2_3[2] = { 'f', 'f' };
static const symbol hungarian_UTF_8_s_2_4[2] = { 'g', 'g' };
static const symbol hungarian_UTF_8_s_2_5[2] = { 'j', 'j' };
static const symbol hungarian_UTF_8_s_2_6[2] = { 'k', 'k' };
static const symbol hungarian_UTF_8_s_2_7[2] = { 'l', 'l' };
static const symbol hungarian_UTF_8_s_2_8[2] = { 'm', 'm' };
static const symbol hungarian_UTF_8_s_2_9[2] = { 'n', 'n' };
static const symbol hungarian_UTF_8_s_2_10[2] = { 'p', 'p' };
static const symbol hungarian_UTF_8_s_2_11[2] = { 'r', 'r' };
static const symbol hungarian_UTF_8_s_2_12[3] = { 'c', 'c', 's' };
static const symbol hungarian_UTF_8_s_2_13[2] = { 's', 's' };
static const symbol hungarian_UTF_8_s_2_14[3] = { 'z', 'z', 's' };
static const symbol hungarian_UTF_8_s_2_15[2] = { 't', 't' };
static const symbol hungarian_UTF_8_s_2_16[2] = { 'v', 'v' };
static const symbol hungarian_UTF_8_s_2_17[3] = { 'g', 'g', 'y' };
static const symbol hungarian_UTF_8_s_2_18[3] = { 'l', 'l', 'y' };
static const symbol hungarian_UTF_8_s_2_19[3] = { 'n', 'n', 'y' };
static const symbol hungarian_UTF_8_s_2_20[3] = { 't', 't', 'y' };
static const symbol hungarian_UTF_8_s_2_21[3] = { 's', 's', 'z' };
static const symbol hungarian_UTF_8_s_2_22[2] = { 'z', 'z' };

static const struct among hungarian_UTF_8_a_2[23] =
{
/*  0 */ { 2, hungarian_UTF_8_s_2_0, -1, -1, 0},
/*  1 */ { 2, hungarian_UTF_8_s_2_1, -1, -1, 0},
/*  2 */ { 2, hungarian_UTF_8_s_2_2, -1, -1, 0},
/*  3 */ { 2, hungarian_UTF_8_s_2_3, -1, -1, 0},
/*  4 */ { 2, hungarian_UTF_8_s_2_4, -1, -1, 0},
/*  5 */ { 2, hungarian_UTF_8_s_2_5, -1, -1, 0},
/*  6 */ { 2, hungarian_UTF_8_s_2_6, -1, -1, 0},
/*  7 */ { 2, hungarian_UTF_8_s_2_7, -1, -1, 0},
/*  8 */ { 2, hungarian_UTF_8_s_2_8, -1, -1, 0},
/*  9 */ { 2, hungarian_UTF_8_s_2_9, -1, -1, 0},
/* 10 */ { 2, hungarian_UTF_8_s_2_10, -1, -1, 0},
/* 11 */ { 2, hungarian_UTF_8_s_2_11, -1, -1, 0},
/* 12 */ { 3, hungarian_UTF_8_s_2_12, -1, -1, 0},
/* 13 */ { 2, hungarian_UTF_8_s_2_13, -1, -1, 0},
/* 14 */ { 3, hungarian_UTF_8_s_2_14, -1, -1, 0},
/* 15 */ { 2, hungarian_UTF_8_s_2_15, -1, -1, 0},
/* 16 */ { 2, hungarian_UTF_8_s_2_16, -1, -1, 0},
/* 17 */ { 3, hungarian_UTF_8_s_2_17, -1, -1, 0},
/* 18 */ { 3, hungarian_UTF_8_s_2_18, -1, -1, 0},
/* 19 */ { 3, hungarian_UTF_8_s_2_19, -1, -1, 0},
/* 20 */ { 3, hungarian_UTF_8_s_2_20, -1, -1, 0},
/* 21 */ { 3, hungarian_UTF_8_s_2_21, -1, -1, 0},
/* 22 */ { 2, hungarian_UTF_8_s_2_22, -1, -1, 0}
};

static const symbol hungarian_UTF_8_s_3_0[2] = { 'a', 'l' };
static const symbol hungarian_UTF_8_s_3_1[2] = { 'e', 'l' };

static const struct among hungarian_UTF_8_a_3[2] =
{
/*  0 */ { 2, hungarian_UTF_8_s_3_0, -1, 1, 0},
/*  1 */ { 2, hungarian_UTF_8_s_3_1, -1, 1, 0}
};

static const symbol hungarian_UTF_8_s_4_0[2] = { 'b', 'a' };
static const symbol hungarian_UTF_8_s_4_1[2] = { 'r', 'a' };
static const symbol hungarian_UTF_8_s_4_2[2] = { 'b', 'e' };
static const symbol hungarian_UTF_8_s_4_3[2] = { 'r', 'e' };
static const symbol hungarian_UTF_8_s_4_4[2] = { 'i', 'g' };
static const symbol hungarian_UTF_8_s_4_5[3] = { 'n', 'a', 'k' };
static const symbol hungarian_UTF_8_s_4_6[3] = { 'n', 'e', 'k' };
static const symbol hungarian_UTF_8_s_4_7[3] = { 'v', 'a', 'l' };
static const symbol hungarian_UTF_8_s_4_8[3] = { 'v', 'e', 'l' };
static const symbol hungarian_UTF_8_s_4_9[2] = { 'u', 'l' };
static const symbol hungarian_UTF_8_s_4_10[4] = { 'b', 0xC5, 0x91, 'l' };
static const symbol hungarian_UTF_8_s_4_11[4] = { 'r', 0xC5, 0x91, 'l' };
static const symbol hungarian_UTF_8_s_4_12[4] = { 't', 0xC5, 0x91, 'l' };
static const symbol hungarian_UTF_8_s_4_13[4] = { 'n', 0xC3, 0xA1, 'l' };
static const symbol hungarian_UTF_8_s_4_14[4] = { 'n', 0xC3, 0xA9, 'l' };
static const symbol hungarian_UTF_8_s_4_15[4] = { 'b', 0xC3, 0xB3, 'l' };
static const symbol hungarian_UTF_8_s_4_16[4] = { 'r', 0xC3, 0xB3, 'l' };
static const symbol hungarian_UTF_8_s_4_17[4] = { 't', 0xC3, 0xB3, 'l' };
static const symbol hungarian_UTF_8_s_4_18[3] = { 0xC3, 0xBC, 'l' };
static const symbol hungarian_UTF_8_s_4_19[1] = { 'n' };
static const symbol hungarian_UTF_8_s_4_20[2] = { 'a', 'n' };
static const symbol hungarian_UTF_8_s_4_21[3] = { 'b', 'a', 'n' };
static const symbol hungarian_UTF_8_s_4_22[2] = { 'e', 'n' };
static const symbol hungarian_UTF_8_s_4_23[3] = { 'b', 'e', 'n' };
static const symbol hungarian_UTF_8_s_4_24[7] = { 'k', 0xC3, 0xA9, 'p', 'p', 'e', 'n' };
static const symbol hungarian_UTF_8_s_4_25[2] = { 'o', 'n' };
static const symbol hungarian_UTF_8_s_4_26[3] = { 0xC3, 0xB6, 'n' };
static const symbol hungarian_UTF_8_s_4_27[5] = { 'k', 0xC3, 0xA9, 'p', 'p' };
static const symbol hungarian_UTF_8_s_4_28[3] = { 'k', 'o', 'r' };
static const symbol hungarian_UTF_8_s_4_29[1] = { 't' };
static const symbol hungarian_UTF_8_s_4_30[2] = { 'a', 't' };
static const symbol hungarian_UTF_8_s_4_31[2] = { 'e', 't' };
static const symbol hungarian_UTF_8_s_4_32[5] = { 'k', 0xC3, 0xA9, 'n', 't' };
static const symbol hungarian_UTF_8_s_4_33[7] = { 'a', 'n', 'k', 0xC3, 0xA9, 'n', 't' };
static const symbol hungarian_UTF_8_s_4_34[7] = { 'e', 'n', 'k', 0xC3, 0xA9, 'n', 't' };
static const symbol hungarian_UTF_8_s_4_35[7] = { 'o', 'n', 'k', 0xC3, 0xA9, 'n', 't' };
static const symbol hungarian_UTF_8_s_4_36[2] = { 'o', 't' };
static const symbol hungarian_UTF_8_s_4_37[4] = { 0xC3, 0xA9, 'r', 't' };
static const symbol hungarian_UTF_8_s_4_38[3] = { 0xC3, 0xB6, 't' };
static const symbol hungarian_UTF_8_s_4_39[3] = { 'h', 'e', 'z' };
static const symbol hungarian_UTF_8_s_4_40[3] = { 'h', 'o', 'z' };
static const symbol hungarian_UTF_8_s_4_41[4] = { 'h', 0xC3, 0xB6, 'z' };
static const symbol hungarian_UTF_8_s_4_42[3] = { 'v', 0xC3, 0xA1 };
static const symbol hungarian_UTF_8_s_4_43[3] = { 'v', 0xC3, 0xA9 };

static const struct among hungarian_UTF_8_a_4[44] =
{
/*  0 */ { 2, hungarian_UTF_8_s_4_0, -1, -1, 0},
/*  1 */ { 2, hungarian_UTF_8_s_4_1, -1, -1, 0},
/*  2 */ { 2, hungarian_UTF_8_s_4_2, -1, -1, 0},
/*  3 */ { 2, hungarian_UTF_8_s_4_3, -1, -1, 0},
/*  4 */ { 2, hungarian_UTF_8_s_4_4, -1, -1, 0},
/*  5 */ { 3, hungarian_UTF_8_s_4_5, -1, -1, 0},
/*  6 */ { 3, hungarian_UTF_8_s_4_6, -1, -1, 0},
/*  7 */ { 3, hungarian_UTF_8_s_4_7, -1, -1, 0},
/*  8 */ { 3, hungarian_UTF_8_s_4_8, -1, -1, 0},
/*  9 */ { 2, hungarian_UTF_8_s_4_9, -1, -1, 0},
/* 10 */ { 4, hungarian_UTF_8_s_4_10, -1, -1, 0},
/* 11 */ { 4, hungarian_UTF_8_s_4_11, -1, -1, 0},
/* 12 */ { 4, hungarian_UTF_8_s_4_12, -1, -1, 0},
/* 13 */ { 4, hungarian_UTF_8_s_4_13, -1, -1, 0},
/* 14 */ { 4, hungarian_UTF_8_s_4_14, -1, -1, 0},
/* 15 */ { 4, hungarian_UTF_8_s_4_15, -1, -1, 0},
/* 16 */ { 4, hungarian_UTF_8_s_4_16, -1, -1, 0},
/* 17 */ { 4, hungarian_UTF_8_s_4_17, -1, -1, 0},
/* 18 */ { 3, hungarian_UTF_8_s_4_18, -1, -1, 0},
/* 19 */ { 1, hungarian_UTF_8_s_4_19, -1, -1, 0},
/* 20 */ { 2, hungarian_UTF_8_s_4_20, 19, -1, 0},
/* 21 */ { 3, hungarian_UTF_8_s_4_21, 20, -1, 0},
/* 22 */ { 2, hungarian_UTF_8_s_4_22, 19, -1, 0},
/* 23 */ { 3, hungarian_UTF_8_s_4_23, 22, -1, 0},
/* 24 */ { 7, hungarian_UTF_8_s_4_24, 22, -1, 0},
/* 25 */ { 2, hungarian_UTF_8_s_4_25, 19, -1, 0},
/* 26 */ { 3, hungarian_UTF_8_s_4_26, 19, -1, 0},
/* 27 */ { 5, hungarian_UTF_8_s_4_27, -1, -1, 0},
/* 28 */ { 3, hungarian_UTF_8_s_4_28, -1, -1, 0},
/* 29 */ { 1, hungarian_UTF_8_s_4_29, -1, -1, 0},
/* 30 */ { 2, hungarian_UTF_8_s_4_30, 29, -1, 0},
/* 31 */ { 2, hungarian_UTF_8_s_4_31, 29, -1, 0},
/* 32 */ { 5, hungarian_UTF_8_s_4_32, 29, -1, 0},
/* 33 */ { 7, hungarian_UTF_8_s_4_33, 32, -1, 0},
/* 34 */ { 7, hungarian_UTF_8_s_4_34, 32, -1, 0},
/* 35 */ { 7, hungarian_UTF_8_s_4_35, 32, -1, 0},
/* 36 */ { 2, hungarian_UTF_8_s_4_36, 29, -1, 0},
/* 37 */ { 4, hungarian_UTF_8_s_4_37, 29, -1, 0},
/* 38 */ { 3, hungarian_UTF_8_s_4_38, 29, -1, 0},
/* 39 */ { 3, hungarian_UTF_8_s_4_39, -1, -1, 0},
/* 40 */ { 3, hungarian_UTF_8_s_4_40, -1, -1, 0},
/* 41 */ { 4, hungarian_UTF_8_s_4_41, -1, -1, 0},
/* 42 */ { 3, hungarian_UTF_8_s_4_42, -1, -1, 0},
/* 43 */ { 3, hungarian_UTF_8_s_4_43, -1, -1, 0}
};

static const symbol hungarian_UTF_8_s_5_0[3] = { 0xC3, 0xA1, 'n' };
static const symbol hungarian_UTF_8_s_5_1[3] = { 0xC3, 0xA9, 'n' };
static const symbol hungarian_UTF_8_s_5_2[8] = { 0xC3, 0xA1, 'n', 'k', 0xC3, 0xA9, 'n', 't' };

static const struct among hungarian_UTF_8_a_5[3] =
{
/*  0 */ { 3, hungarian_UTF_8_s_5_0, -1, 2, 0},
/*  1 */ { 3, hungarian_UTF_8_s_5_1, -1, 1, 0},
/*  2 */ { 8, hungarian_UTF_8_s_5_2, -1, 2, 0}
};

static const symbol hungarian_UTF_8_s_6_0[4] = { 's', 't', 'u', 'l' };
static const symbol hungarian_UTF_8_s_6_1[5] = { 'a', 's', 't', 'u', 'l' };
static const symbol hungarian_UTF_8_s_6_2[6] = { 0xC3, 0xA1, 's', 't', 'u', 'l' };
static const symbol hungarian_UTF_8_s_6_3[5] = { 's', 't', 0xC3, 0xBC, 'l' };
static const symbol hungarian_UTF_8_s_6_4[6] = { 'e', 's', 't', 0xC3, 0xBC, 'l' };
static const symbol hungarian_UTF_8_s_6_5[7] = { 0xC3, 0xA9, 's', 't', 0xC3, 0xBC, 'l' };

static const struct among hungarian_UTF_8_a_6[6] =
{
/*  0 */ { 4, hungarian_UTF_8_s_6_0, -1, 1, 0},
/*  1 */ { 5, hungarian_UTF_8_s_6_1, 0, 1, 0},
/*  2 */ { 6, hungarian_UTF_8_s_6_2, 0, 2, 0},
/*  3 */ { 5, hungarian_UTF_8_s_6_3, -1, 1, 0},
/*  4 */ { 6, hungarian_UTF_8_s_6_4, 3, 1, 0},
/*  5 */ { 7, hungarian_UTF_8_s_6_5, 3, 3, 0}
};

static const symbol hungarian_UTF_8_s_7_0[2] = { 0xC3, 0xA1 };
static const symbol hungarian_UTF_8_s_7_1[2] = { 0xC3, 0xA9 };

static const struct among hungarian_UTF_8_a_7[2] =
{
/*  0 */ { 2, hungarian_UTF_8_s_7_0, -1, 1, 0},
/*  1 */ { 2, hungarian_UTF_8_s_7_1, -1, 1, 0}
};

static const symbol hungarian_UTF_8_s_8_0[1] = { 'k' };
static const symbol hungarian_UTF_8_s_8_1[2] = { 'a', 'k' };
static const symbol hungarian_UTF_8_s_8_2[2] = { 'e', 'k' };
static const symbol hungarian_UTF_8_s_8_3[2] = { 'o', 'k' };
static const symbol hungarian_UTF_8_s_8_4[3] = { 0xC3, 0xA1, 'k' };
static const symbol hungarian_UTF_8_s_8_5[3] = { 0xC3, 0xA9, 'k' };
static const symbol hungarian_UTF_8_s_8_6[3] = { 0xC3, 0xB6, 'k' };

static const struct among hungarian_UTF_8_a_8[7] =
{
/*  0 */ { 1, hungarian_UTF_8_s_8_0, -1, 3, 0},
/*  1 */ { 2, hungarian_UTF_8_s_8_1, 0, 3, 0},
/*  2 */ { 2, hungarian_UTF_8_s_8_2, 0, 3, 0},
/*  3 */ { 2, hungarian_UTF_8_s_8_3, 0, 3, 0},
/*  4 */ { 3, hungarian_UTF_8_s_8_4, 0, 1, 0},
/*  5 */ { 3, hungarian_UTF_8_s_8_5, 0, 2, 0},
/*  6 */ { 3, hungarian_UTF_8_s_8_6, 0, 3, 0}
};

static const symbol hungarian_UTF_8_s_9_0[3] = { 0xC3, 0xA9, 'i' };
static const symbol hungarian_UTF_8_s_9_1[5] = { 0xC3, 0xA1, 0xC3, 0xA9, 'i' };
static const symbol hungarian_UTF_8_s_9_2[5] = { 0xC3, 0xA9, 0xC3, 0xA9, 'i' };
static const symbol hungarian_UTF_8_s_9_3[2] = { 0xC3, 0xA9 };
static const symbol hungarian_UTF_8_s_9_4[3] = { 'k', 0xC3, 0xA9 };
static const symbol hungarian_UTF_8_s_9_5[4] = { 'a', 'k', 0xC3, 0xA9 };
static const symbol hungarian_UTF_8_s_9_6[4] = { 'e', 'k', 0xC3, 0xA9 };
static const symbol hungarian_UTF_8_s_9_7[4] = { 'o', 'k', 0xC3, 0xA9 };
static const symbol hungarian_UTF_8_s_9_8[5] = { 0xC3, 0xA1, 'k', 0xC3, 0xA9 };
static const symbol hungarian_UTF_8_s_9_9[5] = { 0xC3, 0xA9, 'k', 0xC3, 0xA9 };
static const symbol hungarian_UTF_8_s_9_10[5] = { 0xC3, 0xB6, 'k', 0xC3, 0xA9 };
static const symbol hungarian_UTF_8_s_9_11[4] = { 0xC3, 0xA9, 0xC3, 0xA9 };

static const struct among hungarian_UTF_8_a_9[12] =
{
/*  0 */ { 3, hungarian_UTF_8_s_9_0, -1, 1, 0},
/*  1 */ { 5, hungarian_UTF_8_s_9_1, 0, 3, 0},
/*  2 */ { 5, hungarian_UTF_8_s_9_2, 0, 2, 0},
/*  3 */ { 2, hungarian_UTF_8_s_9_3, -1, 1, 0},
/*  4 */ { 3, hungarian_UTF_8_s_9_4, 3, 1, 0},
/*  5 */ { 4, hungarian_UTF_8_s_9_5, 4, 1, 0},
/*  6 */ { 4, hungarian_UTF_8_s_9_6, 4, 1, 0},
/*  7 */ { 4, hungarian_UTF_8_s_9_7, 4, 1, 0},
/*  8 */ { 5, hungarian_UTF_8_s_9_8, 4, 3, 0},
/*  9 */ { 5, hungarian_UTF_8_s_9_9, 4, 2, 0},
/* 10 */ { 5, hungarian_UTF_8_s_9_10, 4, 1, 0},
/* 11 */ { 4, hungarian_UTF_8_s_9_11, 3, 2, 0}
};

static const symbol hungarian_UTF_8_s_10_0[1] = { 'a' };
static const symbol hungarian_UTF_8_s_10_1[2] = { 'j', 'a' };
static const symbol hungarian_UTF_8_s_10_2[1] = { 'd' };
static const symbol hungarian_UTF_8_s_10_3[2] = { 'a', 'd' };
static const symbol hungarian_UTF_8_s_10_4[2] = { 'e', 'd' };
static const symbol hungarian_UTF_8_s_10_5[2] = { 'o', 'd' };
static const symbol hungarian_UTF_8_s_10_6[3] = { 0xC3, 0xA1, 'd' };
static const symbol hungarian_UTF_8_s_10_7[3] = { 0xC3, 0xA9, 'd' };
static const symbol hungarian_UTF_8_s_10_8[3] = { 0xC3, 0xB6, 'd' };
static const symbol hungarian_UTF_8_s_10_9[1] = { 'e' };
static const symbol hungarian_UTF_8_s_10_10[2] = { 'j', 'e' };
static const symbol hungarian_UTF_8_s_10_11[2] = { 'n', 'k' };
static const symbol hungarian_UTF_8_s_10_12[3] = { 'u', 'n', 'k' };
static const symbol hungarian_UTF_8_s_10_13[4] = { 0xC3, 0xA1, 'n', 'k' };
static const symbol hungarian_UTF_8_s_10_14[4] = { 0xC3, 0xA9, 'n', 'k' };
static const symbol hungarian_UTF_8_s_10_15[4] = { 0xC3, 0xBC, 'n', 'k' };
static const symbol hungarian_UTF_8_s_10_16[2] = { 'u', 'k' };
static const symbol hungarian_UTF_8_s_10_17[3] = { 'j', 'u', 'k' };
static const symbol hungarian_UTF_8_s_10_18[5] = { 0xC3, 0xA1, 'j', 'u', 'k' };
static const symbol hungarian_UTF_8_s_10_19[3] = { 0xC3, 0xBC, 'k' };
static const symbol hungarian_UTF_8_s_10_20[4] = { 'j', 0xC3, 0xBC, 'k' };
static const symbol hungarian_UTF_8_s_10_21[6] = { 0xC3, 0xA9, 'j', 0xC3, 0xBC, 'k' };
static const symbol hungarian_UTF_8_s_10_22[1] = { 'm' };
static const symbol hungarian_UTF_8_s_10_23[2] = { 'a', 'm' };
static const symbol hungarian_UTF_8_s_10_24[2] = { 'e', 'm' };
static const symbol hungarian_UTF_8_s_10_25[2] = { 'o', 'm' };
static const symbol hungarian_UTF_8_s_10_26[3] = { 0xC3, 0xA1, 'm' };
static const symbol hungarian_UTF_8_s_10_27[3] = { 0xC3, 0xA9, 'm' };
static const symbol hungarian_UTF_8_s_10_28[1] = { 'o' };
static const symbol hungarian_UTF_8_s_10_29[2] = { 0xC3, 0xA1 };
static const symbol hungarian_UTF_8_s_10_30[2] = { 0xC3, 0xA9 };

static const struct among hungarian_UTF_8_a_10[31] =
{
/*  0 */ { 1, hungarian_UTF_8_s_10_0, -1, 1, 0},
/*  1 */ { 2, hungarian_UTF_8_s_10_1, 0, 1, 0},
/*  2 */ { 1, hungarian_UTF_8_s_10_2, -1, 1, 0},
/*  3 */ { 2, hungarian_UTF_8_s_10_3, 2, 1, 0},
/*  4 */ { 2, hungarian_UTF_8_s_10_4, 2, 1, 0},
/*  5 */ { 2, hungarian_UTF_8_s_10_5, 2, 1, 0},
/*  6 */ { 3, hungarian_UTF_8_s_10_6, 2, 2, 0},
/*  7 */ { 3, hungarian_UTF_8_s_10_7, 2, 3, 0},
/*  8 */ { 3, hungarian_UTF_8_s_10_8, 2, 1, 0},
/*  9 */ { 1, hungarian_UTF_8_s_10_9, -1, 1, 0},
/* 10 */ { 2, hungarian_UTF_8_s_10_10, 9, 1, 0},
/* 11 */ { 2, hungarian_UTF_8_s_10_11, -1, 1, 0},
/* 12 */ { 3, hungarian_UTF_8_s_10_12, 11, 1, 0},
/* 13 */ { 4, hungarian_UTF_8_s_10_13, 11, 2, 0},
/* 14 */ { 4, hungarian_UTF_8_s_10_14, 11, 3, 0},
/* 15 */ { 4, hungarian_UTF_8_s_10_15, 11, 1, 0},
/* 16 */ { 2, hungarian_UTF_8_s_10_16, -1, 1, 0},
/* 17 */ { 3, hungarian_UTF_8_s_10_17, 16, 1, 0},
/* 18 */ { 5, hungarian_UTF_8_s_10_18, 17, 2, 0},
/* 19 */ { 3, hungarian_UTF_8_s_10_19, -1, 1, 0},
/* 20 */ { 4, hungarian_UTF_8_s_10_20, 19, 1, 0},
/* 21 */ { 6, hungarian_UTF_8_s_10_21, 20, 3, 0},
/* 22 */ { 1, hungarian_UTF_8_s_10_22, -1, 1, 0},
/* 23 */ { 2, hungarian_UTF_8_s_10_23, 22, 1, 0},
/* 24 */ { 2, hungarian_UTF_8_s_10_24, 22, 1, 0},
/* 25 */ { 2, hungarian_UTF_8_s_10_25, 22, 1, 0},
/* 26 */ { 3, hungarian_UTF_8_s_10_26, 22, 2, 0},
/* 27 */ { 3, hungarian_UTF_8_s_10_27, 22, 3, 0},
/* 28 */ { 1, hungarian_UTF_8_s_10_28, -1, 1, 0},
/* 29 */ { 2, hungarian_UTF_8_s_10_29, -1, 2, 0},
/* 30 */ { 2, hungarian_UTF_8_s_10_30, -1, 3, 0}
};

static const symbol hungarian_UTF_8_s_11_0[2] = { 'i', 'd' };
static const symbol hungarian_UTF_8_s_11_1[3] = { 'a', 'i', 'd' };
static const symbol hungarian_UTF_8_s_11_2[4] = { 'j', 'a', 'i', 'd' };
static const symbol hungarian_UTF_8_s_11_3[3] = { 'e', 'i', 'd' };
static const symbol hungarian_UTF_8_s_11_4[4] = { 'j', 'e', 'i', 'd' };
static const symbol hungarian_UTF_8_s_11_5[4] = { 0xC3, 0xA1, 'i', 'd' };
static const symbol hungarian_UTF_8_s_11_6[4] = { 0xC3, 0xA9, 'i', 'd' };
static const symbol hungarian_UTF_8_s_11_7[1] = { 'i' };
static const symbol hungarian_UTF_8_s_11_8[2] = { 'a', 'i' };
static const symbol hungarian_UTF_8_s_11_9[3] = { 'j', 'a', 'i' };
static const symbol hungarian_UTF_8_s_11_10[2] = { 'e', 'i' };
static const symbol hungarian_UTF_8_s_11_11[3] = { 'j', 'e', 'i' };
static const symbol hungarian_UTF_8_s_11_12[3] = { 0xC3, 0xA1, 'i' };
static const symbol hungarian_UTF_8_s_11_13[3] = { 0xC3, 0xA9, 'i' };
static const symbol hungarian_UTF_8_s_11_14[4] = { 'i', 't', 'e', 'k' };
static const symbol hungarian_UTF_8_s_11_15[5] = { 'e', 'i', 't', 'e', 'k' };
static const symbol hungarian_UTF_8_s_11_16[6] = { 'j', 'e', 'i', 't', 'e', 'k' };
static const symbol hungarian_UTF_8_s_11_17[6] = { 0xC3, 0xA9, 'i', 't', 'e', 'k' };
static const symbol hungarian_UTF_8_s_11_18[2] = { 'i', 'k' };
static const symbol hungarian_UTF_8_s_11_19[3] = { 'a', 'i', 'k' };
static const symbol hungarian_UTF_8_s_11_20[4] = { 'j', 'a', 'i', 'k' };
static const symbol hungarian_UTF_8_s_11_21[3] = { 'e', 'i', 'k' };
static const symbol hungarian_UTF_8_s_11_22[4] = { 'j', 'e', 'i', 'k' };
static const symbol hungarian_UTF_8_s_11_23[4] = { 0xC3, 0xA1, 'i', 'k' };
static const symbol hungarian_UTF_8_s_11_24[4] = { 0xC3, 0xA9, 'i', 'k' };
static const symbol hungarian_UTF_8_s_11_25[3] = { 'i', 'n', 'k' };
static const symbol hungarian_UTF_8_s_11_26[4] = { 'a', 'i', 'n', 'k' };
static const symbol hungarian_UTF_8_s_11_27[5] = { 'j', 'a', 'i', 'n', 'k' };
static const symbol hungarian_UTF_8_s_11_28[4] = { 'e', 'i', 'n', 'k' };
static const symbol hungarian_UTF_8_s_11_29[5] = { 'j', 'e', 'i', 'n', 'k' };
static const symbol hungarian_UTF_8_s_11_30[5] = { 0xC3, 0xA1, 'i', 'n', 'k' };
static const symbol hungarian_UTF_8_s_11_31[5] = { 0xC3, 0xA9, 'i', 'n', 'k' };
static const symbol hungarian_UTF_8_s_11_32[5] = { 'a', 'i', 't', 'o', 'k' };
static const symbol hungarian_UTF_8_s_11_33[6] = { 'j', 'a', 'i', 't', 'o', 'k' };
static const symbol hungarian_UTF_8_s_11_34[6] = { 0xC3, 0xA1, 'i', 't', 'o', 'k' };
static const symbol hungarian_UTF_8_s_11_35[2] = { 'i', 'm' };
static const symbol hungarian_UTF_8_s_11_36[3] = { 'a', 'i', 'm' };
static const symbol hungarian_UTF_8_s_11_37[4] = { 'j', 'a', 'i', 'm' };
static const symbol hungarian_UTF_8_s_11_38[3] = { 'e', 'i', 'm' };
static const symbol hungarian_UTF_8_s_11_39[4] = { 'j', 'e', 'i', 'm' };
static const symbol hungarian_UTF_8_s_11_40[4] = { 0xC3, 0xA1, 'i', 'm' };
static const symbol hungarian_UTF_8_s_11_41[4] = { 0xC3, 0xA9, 'i', 'm' };

static const struct among hungarian_UTF_8_a_11[42] =
{
/*  0 */ { 2, hungarian_UTF_8_s_11_0, -1, 1, 0},
/*  1 */ { 3, hungarian_UTF_8_s_11_1, 0, 1, 0},
/*  2 */ { 4, hungarian_UTF_8_s_11_2, 1, 1, 0},
/*  3 */ { 3, hungarian_UTF_8_s_11_3, 0, 1, 0},
/*  4 */ { 4, hungarian_UTF_8_s_11_4, 3, 1, 0},
/*  5 */ { 4, hungarian_UTF_8_s_11_5, 0, 2, 0},
/*  6 */ { 4, hungarian_UTF_8_s_11_6, 0, 3, 0},
/*  7 */ { 1, hungarian_UTF_8_s_11_7, -1, 1, 0},
/*  8 */ { 2, hungarian_UTF_8_s_11_8, 7, 1, 0},
/*  9 */ { 3, hungarian_UTF_8_s_11_9, 8, 1, 0},
/* 10 */ { 2, hungarian_UTF_8_s_11_10, 7, 1, 0},
/* 11 */ { 3, hungarian_UTF_8_s_11_11, 10, 1, 0},
/* 12 */ { 3, hungarian_UTF_8_s_11_12, 7, 2, 0},
/* 13 */ { 3, hungarian_UTF_8_s_11_13, 7, 3, 0},
/* 14 */ { 4, hungarian_UTF_8_s_11_14, -1, 1, 0},
/* 15 */ { 5, hungarian_UTF_8_s_11_15, 14, 1, 0},
/* 16 */ { 6, hungarian_UTF_8_s_11_16, 15, 1, 0},
/* 17 */ { 6, hungarian_UTF_8_s_11_17, 14, 3, 0},
/* 18 */ { 2, hungarian_UTF_8_s_11_18, -1, 1, 0},
/* 19 */ { 3, hungarian_UTF_8_s_11_19, 18, 1, 0},
/* 20 */ { 4, hungarian_UTF_8_s_11_20, 19, 1, 0},
/* 21 */ { 3, hungarian_UTF_8_s_11_21, 18, 1, 0},
/* 22 */ { 4, hungarian_UTF_8_s_11_22, 21, 1, 0},
/* 23 */ { 4, hungarian_UTF_8_s_11_23, 18, 2, 0},
/* 24 */ { 4, hungarian_UTF_8_s_11_24, 18, 3, 0},
/* 25 */ { 3, hungarian_UTF_8_s_11_25, -1, 1, 0},
/* 26 */ { 4, hungarian_UTF_8_s_11_26, 25, 1, 0},
/* 27 */ { 5, hungarian_UTF_8_s_11_27, 26, 1, 0},
/* 28 */ { 4, hungarian_UTF_8_s_11_28, 25, 1, 0},
/* 29 */ { 5, hungarian_UTF_8_s_11_29, 28, 1, 0},
/* 30 */ { 5, hungarian_UTF_8_s_11_30, 25, 2, 0},
/* 31 */ { 5, hungarian_UTF_8_s_11_31, 25, 3, 0},
/* 32 */ { 5, hungarian_UTF_8_s_11_32, -1, 1, 0},
/* 33 */ { 6, hungarian_UTF_8_s_11_33, 32, 1, 0},
/* 34 */ { 6, hungarian_UTF_8_s_11_34, -1, 2, 0},
/* 35 */ { 2, hungarian_UTF_8_s_11_35, -1, 1, 0},
/* 36 */ { 3, hungarian_UTF_8_s_11_36, 35, 1, 0},
/* 37 */ { 4, hungarian_UTF_8_s_11_37, 36, 1, 0},
/* 38 */ { 3, hungarian_UTF_8_s_11_38, 35, 1, 0},
/* 39 */ { 4, hungarian_UTF_8_s_11_39, 38, 1, 0},
/* 40 */ { 4, hungarian_UTF_8_s_11_40, 35, 2, 0},
/* 41 */ { 4, hungarian_UTF_8_s_11_41, 35, 3, 0}
};

static const unsigned char hungarian_UTF_8_g_v[] = { 17, 65, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 17, 36, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 };

static const symbol hungarian_UTF_8_s_0[] = { 'a' };
static const symbol hungarian_UTF_8_s_1[] = { 'e' };
static const symbol hungarian_UTF_8_s_2[] = { 'e' };
static const symbol hungarian_UTF_8_s_3[] = { 'a' };
static const symbol hungarian_UTF_8_s_4[] = { 'a' };
static const symbol hungarian_UTF_8_s_5[] = { 'e' };
static const symbol hungarian_UTF_8_s_6[] = { 'a' };
static const symbol hungarian_UTF_8_s_7[] = { 'e' };
static const symbol hungarian_UTF_8_s_8[] = { 'e' };
static const symbol hungarian_UTF_8_s_9[] = { 'a' };
static const symbol hungarian_UTF_8_s_10[] = { 'a' };
static const symbol hungarian_UTF_8_s_11[] = { 'e' };
static const symbol hungarian_UTF_8_s_12[] = { 'a' };
static const symbol hungarian_UTF_8_s_13[] = { 'e' };

static int hungarian_UTF_8_r_mark_regions(struct SN_env * z) { /* forwardmode */
    z->I[0] = z->l; /* $p1 = <integer expression>, line 46 */
    {   int c1 = z->c; /* or, line 51 */
        if (in_grouping_U(z, hungarian_UTF_8_g_v, 97, 369, 0)) goto lab1; /* grouping v, line 48 */
        if (in_grouping_U(z, hungarian_UTF_8_g_v, 97, 369, 1) < 0) goto lab1; /* goto */ /* non v, line 48 */
        {   int c2 = z->c; /* or, line 49 */
            if (z->c + 1 >= z->l || z->p[z->c + 1] >> 5 != 3 || !((101187584 >> (z->p[z->c + 1] & 0x1f)) & 1)) goto lab3; /* among, line 49 */
            if (!(find_among(z, hungarian_UTF_8_a_0, 8))) goto lab3;
            goto lab2;
        lab3:
            z->c = c2;
            {   int ret = skip_utf8(z->p, z->c, 0, z->l, 1);
                if (ret < 0) goto lab1;
                z->c = ret; /* next, line 49 */
            }
        }
    lab2:
        z->I[0] = z->c; /* setmark p1, line 50 */
        goto lab0;
    lab1:
        z->c = c1;
        if (out_grouping_U(z, hungarian_UTF_8_g_v, 97, 369, 0)) return 0; /* non v, line 53 */
        {    /* gopast */ /* grouping v, line 53 */
            int ret = out_grouping_U(z, hungarian_UTF_8_g_v, 97, 369, 1);
            if (ret < 0) return 0;
            z->c += ret;
        }
        z->I[0] = z->c; /* setmark p1, line 53 */
    }
lab0:
    return 1;
}

static int hungarian_UTF_8_r_R1(struct SN_env * z) { /* backwardmode */
    if (!(z->I[0] <= z->c)) return 0; /* $(<integer expression> <= <integer expression>), line 58 */
    return 1;
}

static int hungarian_UTF_8_r_v_ending(struct SN_env * z) { /* backwardmode */
    int among_var;
    z->ket = z->c; /* [, line 61 */
    if (z->c - 1 <= z->lb || (z->p[z->c - 1] != 161 && z->p[z->c - 1] != 169)) return 0; /* substring, line 61 */
    among_var = find_among_b(z, hungarian_UTF_8_a_1, 2);
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 61 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 61 */
        if (ret <= 0) return ret;
    }
    switch (among_var) { /* among, line 61 */
        case 1:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_0); /* <-, line 62 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_1); /* <-, line 63 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int hungarian_UTF_8_r_double(struct SN_env * z) { /* backwardmode */
    {   int m_test1 = z->l - z->c; /* test, line 68 */
        if (z->c - 1 <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((106790108 >> (z->p[z->c - 1] & 0x1f)) & 1)) return 0; /* among, line 68 */
        if (!(find_among_b(z, hungarian_UTF_8_a_2, 23))) return 0;
        z->c = z->l - m_test1;
    }
    return 1;
}

static int hungarian_UTF_8_r_undouble(struct SN_env * z) { /* backwardmode */
    {   int ret = skip_utf8(z->p, z->c, z->lb, 0, -1);
        if (ret < 0) return 0;
        z->c = ret; /* next, line 73 */
    }
    z->ket = z->c; /* [, line 73 */
    {   int ret = skip_utf8(z->p, z->c, z->lb, z->l, - 1); /* hop, line 73 */
        if (ret < 0) return 0;
        z->c = ret;
    }
    z->bra = z->c; /* ], line 73 */
    {   int ret = slice_del(z); /* delete, line 73 */
        if (ret < 0) return ret;
    }
    return 1;
}

static int hungarian_UTF_8_r_instrum(struct SN_env * z) { /* backwardmode */
    z->ket = z->c; /* [, line 77 */
    if (z->c - 1 <= z->lb || z->p[z->c - 1] != 108) return 0; /* substring, line 77 */
    if (!(find_among_b(z, hungarian_UTF_8_a_3, 2))) return 0;
    z->bra = z->c; /* ], line 77 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 77 */
        if (ret <= 0) return ret;
    }
    {   int ret = hungarian_UTF_8_r_double(z); /* call double, line 78 */
        if (ret <= 0) return ret;
    }
    {   int ret = slice_del(z); /* delete, line 81 */
        if (ret < 0) return ret;
    }
    {   int ret = hungarian_UTF_8_r_undouble(z); /* call undouble, line 82 */
        if (ret <= 0) return ret;
    }
    return 1;
}

static int hungarian_UTF_8_r_case(struct SN_env * z) { /* backwardmode */
    z->ket = z->c; /* [, line 87 */
    if (!(find_among_b(z, hungarian_UTF_8_a_4, 44))) return 0; /* substring, line 87 */
    z->bra = z->c; /* ], line 87 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 87 */
        if (ret <= 0) return ret;
    }
    {   int ret = slice_del(z); /* delete, line 111 */
        if (ret < 0) return ret;
    }
    {   int ret = hungarian_UTF_8_r_v_ending(z); /* call v_ending, line 112 */
        if (ret <= 0) return ret;
    }
    return 1;
}

static int hungarian_UTF_8_r_case_special(struct SN_env * z) { /* backwardmode */
    int among_var;
    z->ket = z->c; /* [, line 116 */
    if (z->c - 2 <= z->lb || (z->p[z->c - 1] != 110 && z->p[z->c - 1] != 116)) return 0; /* substring, line 116 */
    among_var = find_among_b(z, hungarian_UTF_8_a_5, 3);
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 116 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 116 */
        if (ret <= 0) return ret;
    }
    switch (among_var) { /* among, line 116 */
        case 1:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_2); /* <-, line 117 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_3); /* <-, line 118 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int hungarian_UTF_8_r_case_other(struct SN_env * z) { /* backwardmode */
    int among_var;
    z->ket = z->c; /* [, line 124 */
    if (z->c - 3 <= z->lb || z->p[z->c - 1] != 108) return 0; /* substring, line 124 */
    among_var = find_among_b(z, hungarian_UTF_8_a_6, 6);
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 124 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 124 */
        if (ret <= 0) return ret;
    }
    switch (among_var) { /* among, line 124 */
        case 1:
            {   int ret = slice_del(z); /* delete, line 125 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_4); /* <-, line 127 */
                if (ret < 0) return ret;
            }
            break;
        case 3:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_5); /* <-, line 128 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int hungarian_UTF_8_r_factive(struct SN_env * z) { /* backwardmode */
    z->ket = z->c; /* [, line 133 */
    if (z->c - 1 <= z->lb || (z->p[z->c - 1] != 161 && z->p[z->c - 1] != 169)) return 0; /* substring, line 133 */
    if (!(find_among_b(z, hungarian_UTF_8_a_7, 2))) return 0;
    z->bra = z->c; /* ], line 133 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 133 */
        if (ret <= 0) return ret;
    }
    {   int ret = hungarian_UTF_8_r_double(z); /* call double, line 134 */
        if (ret <= 0) return ret;
    }
    {   int ret = slice_del(z); /* delete, line 137 */
        if (ret < 0) return ret;
    }
    {   int ret = hungarian_UTF_8_r_undouble(z); /* call undouble, line 138 */
        if (ret <= 0) return ret;
    }
    return 1;
}

static int hungarian_UTF_8_r_plural(struct SN_env * z) { /* backwardmode */
    int among_var;
    z->ket = z->c; /* [, line 142 */
    if (z->c <= z->lb || z->p[z->c - 1] != 107) return 0; /* substring, line 142 */
    among_var = find_among_b(z, hungarian_UTF_8_a_8, 7);
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 142 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 142 */
        if (ret <= 0) return ret;
    }
    switch (among_var) { /* among, line 142 */
        case 1:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_6); /* <-, line 143 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_7); /* <-, line 144 */
                if (ret < 0) return ret;
            }
            break;
        case 3:
            {   int ret = slice_del(z); /* delete, line 145 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int hungarian_UTF_8_r_owned(struct SN_env * z) { /* backwardmode */
    int among_var;
    z->ket = z->c; /* [, line 154 */
    if (z->c - 1 <= z->lb || (z->p[z->c - 1] != 105 && z->p[z->c - 1] != 169)) return 0; /* substring, line 154 */
    among_var = find_among_b(z, hungarian_UTF_8_a_9, 12);
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 154 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 154 */
        if (ret <= 0) return ret;
    }
    switch (among_var) { /* among, line 154 */
        case 1:
            {   int ret = slice_del(z); /* delete, line 155 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_8); /* <-, line 156 */
                if (ret < 0) return ret;
            }
            break;
        case 3:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_9); /* <-, line 157 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int hungarian_UTF_8_r_sing_owner(struct SN_env * z) { /* backwardmode */
    int among_var;
    z->ket = z->c; /* [, line 168 */
    among_var = find_among_b(z, hungarian_UTF_8_a_10, 31); /* substring, line 168 */
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 168 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 168 */
        if (ret <= 0) return ret;
    }
    switch (among_var) { /* among, line 168 */
        case 1:
            {   int ret = slice_del(z); /* delete, line 169 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_10); /* <-, line 170 */
                if (ret < 0) return ret;
            }
            break;
        case 3:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_11); /* <-, line 171 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int hungarian_UTF_8_r_plur_owner(struct SN_env * z) { /* backwardmode */
    int among_var;
    z->ket = z->c; /* [, line 193 */
    if (z->c <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((10768 >> (z->p[z->c - 1] & 0x1f)) & 1)) return 0; /* substring, line 193 */
    among_var = find_among_b(z, hungarian_UTF_8_a_11, 42);
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 193 */
    {   int ret = hungarian_UTF_8_r_R1(z); /* call R1, line 193 */
        if (ret <= 0) return ret;
    }
    switch (among_var) { /* among, line 193 */
        case 1:
            {   int ret = slice_del(z); /* delete, line 194 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_12); /* <-, line 195 */
                if (ret < 0) return ret;
            }
            break;
        case 3:
            {   int ret = slice_from_s(z, 1, hungarian_UTF_8_s_13); /* <-, line 196 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int hungarian_UTF_8_stem(struct SN_env * z) { /* forwardmode */
    {   int c1 = z->c; /* do, line 229 */
        {   int ret = hungarian_UTF_8_r_mark_regions(z); /* call mark_regions, line 229 */
            if (ret == 0) goto lab0;
            if (ret < 0) return ret;
        }
    lab0:
        z->c = c1;
    }
    z->lb = z->c; z->c = z->l; /* backwards, line 230 */

    {   int m2 = z->l - z->c; (void)m2; /* do, line 231 */
        {   int ret = hungarian_UTF_8_r_instrum(z); /* call instrum, line 231 */
            if (ret == 0) goto lab1;
            if (ret < 0) return ret;
        }
    lab1:
        z->c = z->l - m2;
    }
    {   int m3 = z->l - z->c; (void)m3; /* do, line 232 */
        {   int ret = hungarian_UTF_8_r_case(z); /* call case, line 232 */
            if (ret == 0) goto lab2;
            if (ret < 0) return ret;
        }
    lab2:
        z->c = z->l - m3;
    }
    {   int m4 = z->l - z->c; (void)m4; /* do, line 233 */
        {   int ret = hungarian_UTF_8_r_case_special(z); /* call case_special, line 233 */
            if (ret == 0) goto lab3;
            if (ret < 0) return ret;
        }
    lab3:
        z->c = z->l - m4;
    }
    {   int m5 = z->l - z->c; (void)m5; /* do, line 234 */
        {   int ret = hungarian_UTF_8_r_case_other(z); /* call case_other, line 234 */
            if (ret == 0) goto lab4;
            if (ret < 0) return ret;
        }
    lab4:
        z->c = z->l - m5;
    }
    {   int m6 = z->l - z->c; (void)m6; /* do, line 235 */
        {   int ret = hungarian_UTF_8_r_factive(z); /* call factive, line 235 */
            if (ret == 0) goto lab5;
            if (ret < 0) return ret;
        }
    lab5:
        z->c = z->l - m6;
    }
    {   int m7 = z->l - z->c; (void)m7; /* do, line 236 */
        {   int ret = hungarian_UTF_8_r_owned(z); /* call owned, line 236 */
            if (ret == 0) goto lab6;
            if (ret < 0) return ret;
        }
    lab6:
        z->c = z->l - m7;
    }
    {   int m8 = z->l - z->c; (void)m8; /* do, line 237 */
        {   int ret = hungarian_UTF_8_r_sing_owner(z); /* call sing_owner, line 237 */
            if (ret == 0) goto lab7;
            if (ret < 0) return ret;
        }
    lab7:
        z->c = z->l - m8;
    }
    {   int m9 = z->l - z->c; (void)m9; /* do, line 238 */
        {   int ret = hungarian_UTF_8_r_plur_owner(z); /* call plur_owner, line 238 */
            if (ret == 0) goto lab8;
            if (ret < 0) return ret;
        }
    lab8:
        z->c = z->l - m9;
    }
    {   int m10 = z->l - z->c; (void)m10; /* do, line 239 */
        {   int ret = hungarian_UTF_8_r_plural(z); /* call plural, line 239 */
            if (ret == 0) goto lab9;
            if (ret < 0) return ret;
        }
    lab9:
        z->c = z->l - m10;
    }
    z->c = z->lb;
    return 1;
}

static struct SN_env * hungarian_UTF_8_create_env(void) { return SN_create_env(0, 1, 0); }

static void hungarian_UTF_8_close_env(struct SN_env * z) { SN_close_env(z, 0); }

