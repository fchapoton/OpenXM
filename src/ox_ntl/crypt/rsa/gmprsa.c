/* $OpenXM: OpenXM/src/ox_ntl/crypt/rsa/gmprsa.c,v 1.1 2004/08/16 03:59:58 iwane Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

#include "gmprsa.h"
#include "sha1.h"


#define STR_PRT(STR, N)   do { int _xxx; printf(#STR "[%d]=", N); for (_xxx = 0; _xxx < (N); _xxx++) printf("%02x", STR[_xxx]); printf("\n"); fflush(stdout); } while (0)


#define RSA_KEY_PRINT 0


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 * CONV
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void
rsa_uchar2mpz(mpz_ptr z, const unsigned char *buf, int len)
{
	char *str;
	int i;
	size_t size = len * 2 * sizeof(char) + 1;

	str = __gmp_allocate_func(size);
	for (i = 0; i < len; i++)
		sprintf(str + 2 * i, "%02x", buf[i]);

	mpz_set_str(z, str, 16);

	__gmp_free_func(str, size);
}


static int
rsa_char2ui(int ch)
{
	if (ch >= '0' && ch <= '9')
		return (ch - '0');
	else if (ch >= 'a' && ch <= 'z')
		return (ch - 'a' + 10);
	else
		return (ch - 'A' + 10);
}

static void
rsa_mpz2uchar(unsigned char *str, int len, const mpz_ptr z)
{
	char *ptr = mpz_get_str(NULL, 16, z);
	int length = strlen(ptr);
	int i = 0, j = 0;

	for (j = (length + 1) / 2; j < len; j++)
		str[i++] = '\0';
	
	j = 0;

	if (length % 2 != 0) {
		str[i++] = rsa_char2ui(ptr[j++]);
	}

	for (; i < len && j < length; i++, j += 2) {
		str[i] = rsa_char2ui(ptr[j]) * 16 + rsa_char2ui(ptr[j + 1]);
	}
	str[i] = '\0';

	__gmp_free_func(ptr, length);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 * INITIALIZE
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void
rsa_init(rsa_key *r)
{
	mpz_init(r->p);
	mpz_init(r->q);
	mpz_init(r->mod);
	mpz_init(r->private_key);
	mpz_init_set_si(r->public_key, 11);
}

void
rsa_clear(rsa_key *r)
{
	mpz_clear(r->p);
	mpz_clear(r->q);
	mpz_clear(r->mod);
	mpz_clear(r->public_key);
	mpz_clear(r->private_key);

}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 * PRIME
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void
rsa_genprime(mpz_ptr prime, int len, const mpz_ptr seed, int rep)
{
	const int LOOP = 4096;
	const int BLOCK = 160;
	int n, b;
	int offset = 2;
	int k, c, i;
	int shalen;
	mpz_t s, s1, sp;
	unsigned char sha[32];
	char *str;

	if (len < 2) {
		mpz_set_ui(prime, 3);
		return ;
	}

	n = (len - 1) / BLOCK;
	b = (len - 1) % BLOCK;

	mpz_init(s);
	mpz_init(s1);
	mpz_init(sp);


__RETRY:
	for (c = 0; c < LOOP; c++) {
		mpz_set_ui(prime, 0);
		mpz_setbit(prime, len - 1);

		for (k = 0; k <= n; k++) {
			mpz_add_ui(s1, seed, offset + c + k);

			str = mpz_get_str(NULL, 16, s1);
			shalen = strlen(str);
			sha1(sha, str, shalen);
			__gmp_free_func(str, shalen);

			rsa_uchar2mpz(s, sha, 20);

			mpz_mul_2exp(sp, s, 160 * k);

			mpz_add(prime, prime, sp);

			offset += n + 1;
		}


		mpz_setbit(prime, 0);
		mpz_setbit(prime, len - 1);
		for (i = (n + 1) * BLOCK; i >= len; i--)
			mpz_clrbit(prime, i);


#if RSA_KEY_PRINT
		fprintf(stderr, "*"); fflush(stderr);
#endif
		/* prime test */
		if (mpz_probab_prime_p(prime, rep))
			break;
	}

#if RSA_KEY_PRINT
	fprintf(stderr, "+"); fflush(stderr);
#endif

	if (c == LOOP) {
		offset += c + 13;
		goto __RETRY;
	}

	mpz_clear(s);
	mpz_clear(s1);
	mpz_clear(sp);

}


static int
rsa_get_block_size(const mpz_ptr n)
{
	return ((mpz_sizeinbase(n, 16) + 1) / 2);
}


/*
 *
 * bit >= 96 must
 * rep >= 80 should
 *
 */
int
rsa_keygen(
    rsa_key *rsa,
    const mpz_ptr _seed1, const mpz_ptr _seed2,
    unsigned int bit,
    int rep)
{
	int bitp, bitq;
	mpz_t gcd;
	mpz_t phi;
	mpz_t seed1, seed2;

	/* 12 = PS_MIN + 3 + 1 */
	if (bit < 12 * 8)
		return (-1); /* too short */


	bitp = bit / 2;
	bitq = bit - bitp;

	mpz_init_set_ui(gcd, 1);
	mpz_init(phi);

	mpz_init_set(seed1, _seed1);
	mpz_init_set(seed2, _seed2);

	for (;;) {
		if (mpz_cmp(seed1, seed2) == 0)
			goto _NEXT;

		rsa_genprime(rsa->p, bitp, seed1, rep);
		rsa_genprime(rsa->q, bitq, seed2, rep);

		mpz_clrbit(rsa->p, 0);
		mpz_clrbit(rsa->q, 0);

		mpz_mul(phi, rsa->p, rsa->q);

		mpz_gcd(gcd, rsa->public_key, phi);
#if RSA_KEY_PRINT
		fprintf(stderr, "@"); fflush(stderr);
#endif
		if (mpz_cmp_ui(gcd, 1) != 0)
			goto _NEXT;

		if (mpz_cmp(rsa->p, rsa->q) == 0)
			goto _NEXT;

		break;

_NEXT:
		mpz_add(seed1, seed1, gcd);
		mpz_sub(seed2, seed2, gcd);
	}

	mpz_setbit(rsa->p, 0);
	mpz_setbit(rsa->q, 0);

	mpz_invert(rsa->private_key, rsa->public_key, phi);

	mpz_mul(rsa->mod, rsa->p, rsa->q);

    	rsa->k = rsa_get_block_size(rsa->mod);

	mpz_clear(gcd);
	mpz_clear(phi);

	mpz_clear(seed1);
	mpz_clear(seed2);

	return (0);
}


void
rsa_set_publickey(
    rsa_key *rsa,
    mpz_ptr public_key,
    mpz_ptr mod)
{
	mpz_set(rsa->mod, mod);
	mpz_set(rsa->public_key, public_key);
    	rsa->k = rsa_get_block_size(rsa->mod);
}

void
rsa_set_key(
    rsa_key *rsa,
    mpz_ptr public_key,
    mpz_ptr private_key,
    mpz_ptr mod)
{
	mpz_set(rsa->private_key, private_key);
	rsa_set_publickey(rsa, public_key, mod);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 * ENCRYPT
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int
rsa_encrypt(
    unsigned char *eb,
    const unsigned char *msg,
    int size,
    int k,
    const mpz_ptr c,
    const mpz_ptr mod, 
    int padding)
{
	const int PS_MIN = 8;
	const int OCTET_SIZE = 256;

	int i;
	int s;      /* length of data */
	int ps;     /* length of padding string */
	const int DATA_MAX = k - PS_MIN - 3;
	mpz_t x, y, ebz;

	if (k <= 0)
		return (-2);

	mpz_init(x);
	mpz_init(y);
	mpz_init(ebz);


	/* k == ps + s + 3 */
	if (size > DATA_MAX) {
		s = DATA_MAX;
		ps = PS_MIN;
	} else {
		s = size;
		ps = DATA_MAX - size + PS_MIN;
	}

	/*-------------------------------------------------------------------*
	 * Encryption-block formatting
	 *-------------------------------------------------------------------*/
	eb[0] = 0x00;
	eb[1] = padding;


	switch (padding)  {
	case RSA_PKCS_1_PADDING_PUBLIC:
		for (i = 0; i < ps; i++) {
			/* 0 < random number < 255 */
			eb[i + 2] = (unsigned char)(random() % (OCTET_SIZE - 1) + 1);
		}
		break;
	case RSA_PKCS_1_PADDING_PRIVATE:
		for (i = 0; i < ps; i++) {
			eb[i + 2] = 0xff;
		}
		break;
	case RSA_PKCS_1_PADDING_PRIVATE0:
		for (i = 0; i < ps; i++) {
			eb[i + 2] = 0x00;
		}
		break;
	default:
		mpz_clear(x);
		mpz_clear(y);
		mpz_clear(ebz);
		return (-1);
	}

	eb[ps + 2] = 0x00;
	for (i = 0; i < s; i++) {
		eb[ps + 3 + i] = *msg++;
	}


	/*-------------------------------------------------------------------*
	 * Octet-string-to-integer conversion
	 *-------------------------------------------------------------------*/
	rsa_uchar2mpz(x, eb, k);

	/*-------------------------------------------------------------------*
	 * RSA computation
	 *-------------------------------------------------------------------*/
	mpz_powm(y, x, c, mod);

	/*-------------------------------------------------------------------*
	 * Integer-to-octet-string conversion
	 *-------------------------------------------------------------------*/
	rsa_mpz2uchar(eb, k, y);


	mpz_clear(x);
	mpz_clear(y);
	mpz_clear(ebz);

	return (s);
}


/****************************************************************************
 * RSA decryption.
 *
 * MEMO: $BJ?J8$,(B 0x00 $B$G;O$^$k$h$&$J%a%C%;!<%8$N>l9g(B,
 *       $B@5$7$/I|9f2=$9$k$3$H$,$G$-$J$$(B.
 *
 ****************************************************************************/
static int
rsa_decrypt(
    unsigned char *d,
    const unsigned char *eb,
    int k,
    const mpz_ptr exp,
    const mpz_ptr mod,
    int padding)
{
	const int PS_MIN = 8;

	mpz_t x, y;
	int i, n, bt;
	int ret = 0;

	mpz_init(x);
	mpz_init(y);

	/*--------------------------------------------------------------------*
	 * Octet-string-to-integer
	 *--------------------------------------------------------------------*/
	rsa_uchar2mpz(y, eb, k);

	/* error check */
	if (mpz_cmp(y, mod) >= 0 || mpz_sgn(y) < 0) {
		ret = -1;
		goto _ERR;
	}

	/*--------------------------------------------------------------------*
	 * RSA computation
	 *--------------------------------------------------------------------*/
	mpz_powm(x, y, exp, mod);

	/*--------------------------------------------------------------------*
	 * Integer-to-octet-string
	 *--------------------------------------------------------------------*/
	rsa_mpz2uchar(d, k, x);


	/*--------------------------------------------------------------------*
	 * Encryption-block parsing
	 * EB = 00 || BT || PS || 00 || D
	 *--------------------------------------------------------------------*/
	if (d[0] != 0x00) {
		ret = -2;
		goto _ERR;
	}


	if (padding == RSA_PKCS_1_PADDING_PUBLIC) { /* public key */
		bt = d[1];

		if (bt != RSA_PKCS_1_PADDING_PRIVATE0 && bt != RSA_PKCS_1_PADDING_PRIVATE) {
			ret = -3;
			goto _ERR;
		}

		if (bt == RSA_PKCS_1_PADDING_PRIVATE0) {
			for (i = 0; i < k - 3; i++) {
				if (d[i + 2] != 0x00) {
					i--;
					break;
				}
			}
		} else {
			for (i = 0; i < k - 3; i++) {
				if (d[i + 2] != 0xff) {
					break;
				}
			}
		}
	} else { /* private key */

		if (d[1] != RSA_PKCS_1_PADDING_PUBLIC) {
			ret = -4;
			goto _ERR;
		}

		for (i = 0; i < k - 3; i++) {
			if (d[i + 2] == 0x00) {
				break;
			}
		}
	}

	if (d[i + 2] != 0x00) {
		ret = -5;
		goto _ERR;
	}

	if (i < PS_MIN) { /* too short */
		ret = -6;
		goto _ERR;
	}

	if (i == k - 3) { /* parse error */
		ret = -7;
		goto _ERR;
	}

	n = k - 3 - i;
	memmove(d, d + i + 3, n); /* ........ */

	mpz_clear(x);
	mpz_clear(y);

	return (n);
_ERR:

	mpz_clear(x);
	mpz_clear(y);
	return (ret);
}

int
rsa_encrypt_by_public_key(rsa_key *rsa, unsigned char *eb, const unsigned char *msg, int len, int padding)
{
	return (rsa_encrypt(eb, msg, len, rsa->k, rsa->public_key, rsa->mod, RSA_PKCS_1_PADDING_PUBLIC));
}

int
rsa_encrypt_by_private_key(rsa_key *rsa, unsigned char *eb, const unsigned char *msg, int len, int padding)
{
	return (rsa_encrypt(eb, msg, len, rsa->k, rsa->private_key, rsa->mod, RSA_PKCS_1_PADDING_PRIVATE));
}

int
rsa_decrypt_by_public_key(rsa_key *rsa, unsigned char *buf, const unsigned char *eb, int padding)
{
	return (rsa_decrypt(buf, eb, rsa->k, rsa->public_key, rsa->mod, RSA_PKCS_1_PADDING_PUBLIC));
}

int
rsa_decrypt_by_private_key(rsa_key *rsa, unsigned char *buf, const unsigned char *eb, int padding)
{
	return (rsa_decrypt(buf, eb, rsa->k, rsa->private_key, rsa->mod, RSA_PKCS_1_PADDING_PRIVATE));
}


