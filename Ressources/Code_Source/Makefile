EXEC=readkeysFIFO
SRCS=$(wildcard readkeysFIFO.c) 


ARMADEUS_TOOLCHAIN_PATH= /home/lmi/workspace/buildroot/output/host/usr/bin/
#APF27 standard makefile for LMI labs
TARGET= /tftpboot/apf27-root/home
CC=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
LD=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
CFLAGS=-W -Wall -Wextra -g -c -mcpu=arm926ej-s -O0 -MD 
#-std=c99
OBJDIR=.obj
OBJS= $(addprefix $(OBJDIR)/, $(ASRC:.s=.o) $(SRCS:.c=.o))

.PHONY: all clean install

all: $(OBJDIR)/ $(EXEC)
	
$(EXEC): $(OBJS) $(LINKER_SCRIPT)
	$(LD) $(OBJS) $(LDFLAGS) -o $@ 

$(OBJDIR)/%o: %c
	$(CC) $(CFLAGS) $< -o $@
	
$(OBJDIR)/%o: %s
	$(AS) $(AFLAGS) -MD $(OBJDIR)/$*d $< -o $@	
			
$(OBJDIR)/:
	mkdir -p $(OBJDIR)

clean:
	rm -Rf $(OBJDIR) $(EXEC) *.map
	
install: 
	sudo cp $(EXEC) $(TARGET)

-include $(OBJS:.o=.d)


