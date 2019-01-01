#Notes:
#1. The make target depends on how eRPC was compiled:
#   * If DTRANSPORT=dpdk, use `make dpdk`
#   * If DTRANSPORT=raw, use `make raw`
#   * If DTRANSPORT=infiniband, use `make infiniband`
LIBS = -lerpc -lpthread -lnuma -ldl

error:
	@echo "Please choose one of the following targets: infiniband, raw, dpdk, clean"
	@exit 2
infiniband:
	g++ -c -std=c++11 rpc.h -o rpc.o -I ../eRPC -L ../eRPC $(LIBS) -libverbs  -DINFINIBAND=true
raw:
	g++ -c -std=c++11 rpc.h -o rpc.o -I ../eRPC -L ../eRPC $(LIBS) -libverbs -DRAW=true
dpdk:
	g++ -c -std=c++11 rpc.h -o rpc.o -I ../eRPC -L ../eRPC $(LIBS) -ldpdk -DDPDK=true -march=native
clean:
	rm rpc.o

