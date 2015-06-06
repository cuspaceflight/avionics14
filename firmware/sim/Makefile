all:
	gcc -O0 -Wall -Wextra -c -fPIC mission_control.c -o mission_control.o
	gcc -O0 -Wall -Wextra -c -fPIC ch.c -o ch.o
	gcc -O0 -Wall -Wextra -c -fPIC datalogging.c -o datalogging.o
	gcc -O0 -Wall -Wextra -c -fPIC pyro.c -o pyro.o
	gcc -O0 -Wall -Wextra -c -fPIC state_estimation.c -o state_estimation.o
	gcc -shared -Wl,-soname,libmission.so -o libmission.so mission_control.o \
		ch.o datalogging.o pyro.o state_estimation.o

clean:
	rm mission_control.o libmission.so
