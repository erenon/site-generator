valgrind:
	valgrind  --log-file=/tmp/val.grd --leak-check=full --track-origins=yes ./Debug/site-generator Debug/ Debug/output/ --img Debug/output/img/
	
C_SRCS += \
./src/dir.c \
./src/generator.c \
./src/genlib.c \
./src/site-generator.c 

OBJS += \
./dir.o \
./generator.o \
./genlib.o \
./site-generator.o 

./%.o: ./src/%.c
	gcc -O0 -Wall -c -std=c89 -o"$@" "$<"

clean:
	-$(RM) $(OBJS)

site-generator: $(OBJS) $(C_SRCS)
	gcc -o site-generator $(OBJS)
	
all: site-generator clean
