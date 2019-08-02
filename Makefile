cc=g++
INCLUDE=-Imongoose/ -I/usr/include/mysql/
LIB=-L/usr/lib64/mysql -lmysqlclient
IMServer:IMServer.cc ./mongoose/mongoose.c 
	$(cc) -o $@ $^ $(INCLUDE) $(LIB)
.PHONY:clean
clean:
	rm -f IMServer