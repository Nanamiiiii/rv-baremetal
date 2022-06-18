MAKE	:=	make
TARGETS	:=	virt sifive_u

.PHONY: all virt sifive_u clean
all:
	$(MAKE) $(TARGETS)

virt:
	$(MAKE) -C virt

sifive_u:
	$(MAKE) -C sifive_u

clean:
	$(MAKE) -C virt clean
	$(MAKE) -C sifive_u clean