cc=g++
INCLUDE=-I./mongoose/
IMServer:IMServer.cc ./mongoose/mongoose.c
	$(cc) -o $@ $^ 
.PHONY:clean
clean:
	rm -f IMServer