LD      := g++  -fpermissive #-std=gnu11

LIBS = -lerpc -lpthread -lnuma -ldl #ERPC

LDFLAGS := -O3 -std=c++11 -flto -lgsl -lgslcblas -libverbs -lrt -lpthread -lmemcached -lnuma -lrdmacm -L ../eRPC $(LIBS)
CC  = g++ -fpermissive -Wno-write-strings
CFLAGS   = -O3 -std=c++11 -I../../include/mica -I../../include/libhrd -I../../include/ccKVS -I../../include/optik -I ../eRPC -L ../eRPC $(LIBS) -DINFINIBAND=true
APPS    := ccKVS-sc clean-o
PROF    := -g -fno-omit-frame-pointer




all: ${APPS}

ccKVS-sc: ../libhrd/hrd_conn.o ../libhrd/hrd_util.o ../mica/mica.o ../mica/city.o main.o client-sc.o worker.o cache.o util.o stats.o
	${LD} -o $@ $^ ${LDFLAGS}

ccKVS-lin: ../libhrd/hrd_conn.o ../libhrd/hrd_util.o ../mica/mica.o ../mica/city.o main.o client-lin.o worker.o cache.o util.o stats.o
	${LD}  -o $@ $^ ${LDFLAGS}


PHONY: clean
clean:
	rm -f *.o ../libhrd/*.o ../mica/*.o ${APPS}
clean-o:
	rm -f *.o ../libhrd/*.o ../mica/*.o
