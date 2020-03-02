# obj-m	+=	multiply.o
obj-m +=	multiplier.o

all:
	make -C ../../lab4/linux-3.14 M=$(PWD) modules

clean:
	make -C ../../lab4/linux-3.14 M=$(PWD) clean
