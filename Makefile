ServCode = main.c threads.c clients.c
Serv = server
Target = $(Serv)
Req = $(ServCode) threads.h clients.h
withThread = -pthread

makeFiles: $(Req)
	gcc $(ServCode) -o $(Serv) $(withThread)

clean: $(Target)
	rm -f $(Target)