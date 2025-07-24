all: voice pointer

.PHONY: voice pointer
voice:
	$(MAKE) -C voice

pointer:
	$(MAKE) -C pointer

clean:
	$(MAKE) -C voice  clean
	$(MAKE) -C pointer clean 

