# Makefile for TestSmtpStack
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2017/06/01

include ../Makefile.mk

INC=-I../SmtpStack -I../SipPlatform
LIB=-L../SmtpStack -lsmtpstack \
	-L../SipPlatform -lsipplatform \
	-lssl -lcrypto -lpthread

.SUFFIXES: .o .cpp
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< $(INC)

all: TestSmtpStack.exe

SRV_OBJ=TestSmtpStack.o TestSmtpResponse.o TestSmtpClient.o

TestSmtpStack.exe: $(SRV_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SRV_OBJ) $(INC) $(LIB)

clean:
	rm -f *.exe
	rm -f *.o
	rm -f core.*

