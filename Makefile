
all: dirinfo
	cc -Wall -Wextra -Werror -Wvla dirinfo.c -o dirinfo

test: all FORCE
	@bash test.sh 1
	@bash test.sh 2
	@bash test.sh 3
	@bash test.sh 4

FORCE: ;

clean:
	rm dirinfo
