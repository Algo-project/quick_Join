all: build

build: compile.sh 
	sh compile.sh

.PHONY: clean

clean:
	rm -rf bin
