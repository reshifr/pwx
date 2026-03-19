package main

/*
#cgo CFLAGS: -I${SRCDIR}/winhello
#cgo LDFLAGS: -L${SRCDIR}/winhello/build/Release -lwinhello

#include <winhello.h>
*/
import "C"

func main() {
	C.winhello()
	C.winhello2()
}
