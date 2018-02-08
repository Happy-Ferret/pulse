package pulse

// #cgo CFLAGS: -Wno-error=implicit-function-declaration
// #include "client.h"
// #cgo pkg-config: libpulse
import "C"

import (
	"io"
)

type UploadStream struct {
	*Stream
	io.Writer
}
