#
# OpenSSL/crypto/rsa/Makefile
#

DIR=	rsa
TOP=	../..
CC=	cc
INCLUDES= -I.. -I$(TOP) -I../../include
CFLAG=-g
MAKEFILE=	Makefile
AR=		ar r

CFLAGS= $(INCLUDES) $(CFLAG)

GENERAL=Makefile
TEST=rsa_test.c
APPS=

LIB=$(BWLIBDIR)/libcrypto.a
LIBSRC= rsa_eay.c rsa_gen.c rsa_lib.c rsa_sign.c rsa_saos.c rsa_err.c \
	rsa_pk1.c rsa_ssl.c rsa_none.c rsa_oaep.c rsa_chk.c rsa_null.c \
	rsa_pss.c rsa_x931.c rsa_asn1.c rsa_depr.c
LIBOBJ= $(MF_CONFIG)/rsa_eay.o $(MF_CONFIG)/rsa_gen.o $(MF_CONFIG)/rsa_lib.o \
	$(MF_CONFIG)/rsa_sign.o $(MF_CONFIG)/rsa_saos.o $(MF_CONFIG)/rsa_err.o \
	$(MF_CONFIG)/rsa_pk1.o $(MF_CONFIG)/rsa_ssl.o $(MF_CONFIG)/rsa_none.o \
	$(MF_CONFIG)/rsa_oaep.o $(MF_CONFIG)/rsa_chk.o $(MF_CONFIG)/rsa_null.o \
	$(MF_CONFIG)/rsa_pss.o $(MF_CONFIG)/rsa_x931.o $(MF_CONFIG)/rsa_asn1.o \
	$(MF_CONFIG)/rsa_depr.o

SRC= $(LIBSRC)

EXHEADER= rsa.h
HEADER=	$(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

top:
	(cd ../..; $(MAKE) DIRS=crypto SDIRS=$(DIR) sub_all)

all:	lib

$(MF_CONFIG):
	@-mkdir $(MF_CONFIG)

lib:	$(MF_CONFIG) $(LIBOBJ)
	$(AR) $(LIB) $(LIBOBJ)
	$(RANLIB) $(LIB) || echo Never mind.
	@touch lib

$(MF_CONFIG)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

files:
	$(PERL) $(TOP)/util/files.pl Makefile >> $(TOP)/MINFO

links:
	@$(PERL) $(TOP)/util/mklink.pl ../../include/openssl $(EXHEADER)
	@$(PERL) $(TOP)/util/mklink.pl ../../test $(TEST)
	@$(PERL) $(TOP)/util/mklink.pl ../../apps $(APPS)

install:
	@[ -n "$(INSTALLTOP)" ] # should be set by top Makefile...
	@headerlist="$(EXHEADER)"; for i in $$headerlist ; \
	do  \
	(cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i; \
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i ); \
	done;

tags:
	ctags $(SRC)

tests:

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

depend:
	@[ -n "$(MAKEDEPEND)" ] # should be set by upper Makefile...
	$(MAKEDEPEND) -- $(CFLAG) $(INCLUDES) $(DEPFLAG) -- $(PROGS) $(LIBSRC)

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f $(MF_CONFIG)/*.o *.o */*.o *.obj lib tags core .pure .nfs* *.old *.bak fluff

# DO NOT DELETE THIS LINE -- make depend depends on it.

$(MF_CONFIG)/rsa_asn1.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_asn1.o: ../../include/openssl/asn1t.h ../../include/openssl/bio.h
$(MF_CONFIG)/rsa_asn1.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
$(MF_CONFIG)/rsa_asn1.o: ../../include/openssl/crypto.h ../../include/openssl/e_os2.h
$(MF_CONFIG)/rsa_asn1.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
$(MF_CONFIG)/rsa_asn1.o: ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_asn1.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_asn1.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
$(MF_CONFIG)/rsa_asn1.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
$(MF_CONFIG)/rsa_asn1.o: ../cryptlib.h rsa_asn1.c
$(MF_CONFIG)/rsa_chk.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
$(MF_CONFIG)/rsa_chk.o: ../../include/openssl/bn.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_chk.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_chk.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_chk.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_chk.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
$(MF_CONFIG)/rsa_chk.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
$(MF_CONFIG)/rsa_chk.o: rsa_chk.c
$(MF_CONFIG)/rsa_depr.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_depr.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_depr.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_depr.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_depr.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_depr.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_depr.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
$(MF_CONFIG)/rsa_depr.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
$(MF_CONFIG)/rsa_depr.o: ../cryptlib.h rsa_depr.c
$(MF_CONFIG)/rsa_eay.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_eay.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_eay.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_eay.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_eay.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_eay.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_eay.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_eay.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
$(MF_CONFIG)/rsa_eay.o: ../../include/openssl/symhacks.h ../cryptlib.h rsa_eay.c
$(MF_CONFIG)/rsa_err.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
$(MF_CONFIG)/rsa_err.o: ../../include/openssl/crypto.h ../../include/openssl/e_os2.h
$(MF_CONFIG)/rsa_err.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
$(MF_CONFIG)/rsa_err.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
$(MF_CONFIG)/rsa_err.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_err.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
$(MF_CONFIG)/rsa_err.o: ../../include/openssl/symhacks.h rsa_err.c
$(MF_CONFIG)/rsa_gen.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_gen.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_gen.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_gen.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_gen.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_gen.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_gen.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
$(MF_CONFIG)/rsa_gen.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
$(MF_CONFIG)/rsa_gen.o: ../cryptlib.h rsa_gen.c
$(MF_CONFIG)/rsa_lib.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_lib.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_lib.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_lib.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
$(MF_CONFIG)/rsa_lib.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
$(MF_CONFIG)/rsa_lib.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
$(MF_CONFIG)/rsa_lib.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
$(MF_CONFIG)/rsa_lib.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
$(MF_CONFIG)/rsa_lib.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
$(MF_CONFIG)/rsa_lib.o: ../cryptlib.h rsa_lib.c
$(MF_CONFIG)/rsa_none.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_none.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_none.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_none.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_none.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_none.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_none.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_none.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
$(MF_CONFIG)/rsa_none.o: ../../include/openssl/symhacks.h ../cryptlib.h rsa_none.c
$(MF_CONFIG)/rsa_null.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_null.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_null.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_null.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_null.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_null.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_null.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_null.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
$(MF_CONFIG)/rsa_null.o: ../../include/openssl/symhacks.h ../cryptlib.h rsa_null.c
$(MF_CONFIG)/rsa_oaep.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
$(MF_CONFIG)/rsa_oaep.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
$(MF_CONFIG)/rsa_oaep.o: ../cryptlib.h rsa_oaep.c
$(MF_CONFIG)/rsa_pk1.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_pk1.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_pk1.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_pk1.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_pk1.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_pk1.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_pk1.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_pk1.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
$(MF_CONFIG)/rsa_pk1.o: ../../include/openssl/symhacks.h ../cryptlib.h rsa_pk1.c
$(MF_CONFIG)/rsa_pss.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
$(MF_CONFIG)/rsa_pss.o: ../../include/openssl/symhacks.h ../cryptlib.h rsa_pss.c
$(MF_CONFIG)/rsa_saos.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/e_os2.h ../../include/openssl/ec.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/ecdh.h ../../include/openssl/ecdsa.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/err.h ../../include/openssl/evp.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/lhash.h ../../include/openssl/obj_mac.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/objects.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/pkcs7.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
$(MF_CONFIG)/rsa_saos.o: ../../include/openssl/x509.h ../../include/openssl/x509_vfy.h
$(MF_CONFIG)/rsa_saos.o: ../cryptlib.h rsa_saos.c
$(MF_CONFIG)/rsa_sign.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/e_os2.h ../../include/openssl/ec.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/ecdh.h ../../include/openssl/ecdsa.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/err.h ../../include/openssl/evp.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/lhash.h ../../include/openssl/obj_mac.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/objects.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/pkcs7.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
$(MF_CONFIG)/rsa_sign.o: ../../include/openssl/x509.h ../../include/openssl/x509_vfy.h
$(MF_CONFIG)/rsa_sign.o: ../cryptlib.h rsa_sign.c
$(MF_CONFIG)/rsa_ssl.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_ssl.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_ssl.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_ssl.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_ssl.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_ssl.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_ssl.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_ssl.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
$(MF_CONFIG)/rsa_ssl.o: ../../include/openssl/symhacks.h ../cryptlib.h rsa_ssl.c
$(MF_CONFIG)/rsa_x931.o: ../../e_os.h ../../include/openssl/asn1.h
$(MF_CONFIG)/rsa_x931.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
$(MF_CONFIG)/rsa_x931.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
$(MF_CONFIG)/rsa_x931.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
$(MF_CONFIG)/rsa_x931.o: ../../include/openssl/lhash.h ../../include/openssl/obj_mac.h
$(MF_CONFIG)/rsa_x931.o: ../../include/openssl/objects.h ../../include/openssl/opensslconf.h
$(MF_CONFIG)/rsa_x931.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
$(MF_CONFIG)/rsa_x931.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
$(MF_CONFIG)/rsa_x931.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
$(MF_CONFIG)/rsa_x931.o: ../../include/openssl/symhacks.h ../cryptlib.h rsa_x931.c
