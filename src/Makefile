CFLAGS= -g -Wall
#CC=gcc
#AR=ar
#RANLIB=ranlib
CROSS_COMPILE=ppc_82xx-
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
RANLIB=$(CROSS_COMPILE)ranlib
LIBS=-L../lib_ppc 
#LIBS=-L../lib_x86
INCLUDE=-I../include
install:all
	
	cp ./main.cgi 		../bin/ -f
	cp ./deviceinfo.xml 	../bin/ -f
	cp ./main.cgi       	../bin/ -f
	cp ./status_report 	../bin/ -f
	cp ./status_report 	/home/nfsserver/ -f
	cp ./_checkException     ../bin/ -f
	cp ./_checkException     /home/nfsserver/ -f
	cp ./_checkPolicy 	../bin/ -f
	cp ./_checkPolicy 	/home/nfsserver/ -f
	cp ./status_report.sh 	../bin/ -f 
	cp ./checkPolicy.sh 	../bin/ -f 
	cp ./_bypassException   ../bin/ -f 	
	cp ./sh_round.sh       ../bin/ -f 
	cp ./_checkbypass       ../bin/ -f
	cp ./sh_bypass.sh       ../bin/ -f 
	cp ./sh_round.sh       /home/nfsserver/ -f 
	cp ./sh_bypass.sh      /home/nfsserver/-f
	cp ./test	      /home/nfsserver/ -f
	cp ../bin              /home/nfsserver -rf
 	
all: test _checkbypass _bypassException _checkException status_report main.cgi _checkPolicy interface.o bo.o dfgap_cfg_act.o misc.o envfile.o misc.o common.o  


test: interface.o bo.o dfgap_cfg_act.o envfile.o misc.o common.o
	$(CC) test.c  interface.o bo.o  dfgap_cfg_act.o envfile.o misc.o common.o -g -o test  $(LIBS) $(INCLUDE) -lmxml  -lpthread -lcurl -lcgi
	
_checkbypass: interface.o bo.o dfgap_cfg_act.o envfile.o misc.o common.o
	$(CC) _checkbypass.c  interface.o bo.o  dfgap_cfg_act.o envfile.o misc.o common.o -g -o _checkbypass  $(LIBS) $(INCLUDE) -lmxml  -lpthread -lcurl -lcgi
	
_bypassException: interface.o bo.o dfgap_cfg_act.o envfile.o misc.o common.o
	$(CC) _bypassException.c  interface.o bo.o  dfgap_cfg_act.o envfile.o misc.o common.o -g -o _bypassException  $(LIBS) $(INCLUDE) -lmxml  -lpthread -lcurl -lcgi
	
_checkException: interface.o bo.o dfgap_cfg_act.o envfile.o misc.o common.o
		$(CC) _checkException.c  interface.o bo.o  dfgap_cfg_act.o envfile.o misc.o common.o  -o _checkException  $(LIBS) $(INCLUDE) -lmxml  -lpthread -lcurl -lcgi

status_report: interface.o bo.o  dfgap_cfg_act.o envfile.o misc.o common.o 
	$(CC) status_report.c  interface.o bo.o  dfgap_cfg_act.o envfile.o misc.o common.o  -o status_report  $(LIBS) $(INCLUDE) -lmxml  -lpthread -lcurl -lcgi

main.cgi: interface.o  bo.o dfgap_cfg_act.o misc.o envfile.o misc.o common.o  
	$(CC) main.cgi.c interface.o bo.o  dfgap_cfg_act.o envfile.o misc.o common.o -o  main.cgi $(LIBS) $(INCLUDE) -lmxml -lcurl -lpthread -lcgi

_checkPolicy:interface.o bo.o dfgap_cfg_act.o envfile.o misc.o common.o 
	$(CC) _checkPolicy.c  interface.o bo.o  dfgap_cfg_act.o envfile.o misc.o common.o  -o _checkPolicy  $(LIBS) $(INCLUDE) -lmxml  -lpthread -lcurl -lcgi

interface.o:
	$(CC) -c interface.c $(LIBS) $(INCLUDE) 
envfile.o:
	$(CC) -c envfile.c $(LIBS) $(INCLUDE) 
common.o:
	$(CC)  -c common.c $(LIBS) $(INCLUDE) 
misc.o:
	$(CC)  -c misc.c $(LIBS) $(INCLUDE) 
dfgap_cfg_act.o:
	$(CC)  -c dfgap_cfg_act.c $(LIBS) $(INCLUDE) 
bo.o:
	$(CC)  -c bo.c $(LIBS) $(INCLUDE)

clean:
	@rm -f *.o *.a *.cgi
