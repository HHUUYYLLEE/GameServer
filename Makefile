ServCode = main.c threads.c clients.c database.c deque.c
Serv = server
Target = $(Serv)
Req = $(ServCode) threads.h clients.h database.h deque.h
withThread = -pthread
withMySQL = `mysql_config --cflags --libs`

makeFiles: $(Req)
	gcc $(ServCode) -o $(Serv) $(withThread) $(withMySQL)

clean: $(Target)
	rm -f $(Target)