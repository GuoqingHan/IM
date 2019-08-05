cc=g++
INCLUDE=-Imongoose/ -I/usr/include/mysql/
LIB=-L/usr/lib64/mysql -lmysqlclient -ljsoncpp
IMServer:IMServer.cc ./mongoose/mongoose.c 
	$(cc) -o $@ $^ $(INCLUDE) $(LIB) -std=c++11
.PHONY:clean
clean:
	rm -f IMServer