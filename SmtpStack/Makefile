# Makefile for SmtpStack
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2012/11/22

include ../Makefile.mk

INC=-I../SipPlatform

.SUFFIXES: .o .cpp
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< $(INC)

all: libsmtpstack.a

LIB_OBJ=SmtpClient.o SmtpResponse.o TlsFunction.o

libsmtpstack.a: $(LIB_OBJ)
	ar crv $@ $(LIB_OBJ)

clean:
	rm -f *.a
	rm -f *.o
	rm -f core.*

