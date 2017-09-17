IN=syscall.c
OUT=$(patsubst %.c,%.out,$(IN))

$(OUT):
	gcc $(IN) -o $@

clean:
	rm -rf $(OUT)
