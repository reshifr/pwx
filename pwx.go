package main

/*
#cgo CFLAGS: -I${SRCDIR}/windows_hello
#cgo LDFLAGS: -L${SRCDIR}/windows_hello/build/Release -lwindows_hello

#include <windows_hello.h>
*/
import "C"

func main() {
	C.windows_hello()
	C.windows_hello2()
}
