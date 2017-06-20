# Makefile for all of C++ SmtpStack
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2017/06/01

all:
	cd SipPlatform && make
	cd SmtpStack && make
	cd TestSmtpStack && make

clean:
	cd SipPlatform && make clean
	cd SmtpStack && make clean
	cd TestSmtpStack && make clean

install:

