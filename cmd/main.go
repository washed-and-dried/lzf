package main

import (
	"fmt"

	"github.com/washed-and-dried/lzf/internals/smithwaterman"
)

func main() {
	score, traceback, matrix := smithwaterman.ComputeMatrix("darshpsps", "afrshpsps")
	matrix.PrintMatrix()
	fmt.Println(score)
	fmt.Println(traceback)
}
