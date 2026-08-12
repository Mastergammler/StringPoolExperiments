default: build

build:
	ninja -v

run: build
	echo "_____________ PROGRAM START ________________"
	.build/main
