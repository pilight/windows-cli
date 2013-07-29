bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -c libs\binary.c -lws2_32 -lwsock32
move /Y binary.o libs\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -c libs\config.c -lws2_32 -lwsock32
move /Y config.o libs\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -c libs\gc.c -lws2_32 -lwsock32
move /Y gc.o libs\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -c libs\hardware.c -lws2_32 -lwsock32
move /Y hardware.o libs\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -c libs\json.c -lws2_32 -lwsock32
move /Y json.o libs\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -c libs\log.c -lws2_32 -lwsock32
move /Y log.o libs\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -c libs\options.c -lws2_32 -lwsock32
move /Y options.o libs\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -c libs\protocol.c -lws2_32 -lwsock32
move /Y protocol.o libs\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -c libs\socket.c -lws2_32 -lwsock32
move /Y socket.o libs\
bin\ld.exe -r libs\binary.o libs\config.o libs\gc.o libs\hardware.o libs\json.o libs\log.o libs\options.o libs\protocol.o libs\socket.o -o libs\libs.o

bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -Ilibs\ -c protocols\alecto.c -lws2_32 -lwsock32
move /Y alecto.o protocols\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -Ilibs\ -c protocols\arctech_dimmer.c -lws2_32 -lwsock32
move /Y arctech_dimmer.o protocols\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -Ilibs\ -c protocols\arctech_old.c -lws2_32 -lwsock32
move /Y arctech_old.o protocols\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -Ilibs\ -c protocols\arctech_switch.c -lws2_32 -lwsock32
move /Y arctech_switch.o protocols\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -Ilibs\ -c protocols\raw.c -lws2_32 -lwsock32
move /Y raw.o protocols\
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -Ilibs\ -c protocols\sartano.c -lws2_32 -lwsock32
move /Y sartano.o protocols\
bin\ld.exe -r protocols\alecto.o protocols\arctech_dimmer.o protocols\arctech_old.o protocols\arctech_switch.o protocols\raw.o protocols\sartano.o -o protocols\protocols.o

bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -Ilibs\ -Iprotocols\ control.c -o 433-control.exe protocols\protocols.o libs\libs.o -lws2_32 -lwsock32
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -Ilibs\ -Iprotocols\ receive.c -o 433-receive.exe protocols\protocols.o libs\libs.o -lws2_32 -lwsock32
bin\gcc.exe -ffast-math -Wno-conversion -g -static -I. -I.. -Ilibs\ -Iprotocols\ send.c -o 433-send.exe protocols\protocols.o libs\libs.o -lws2_32 -lwsock32