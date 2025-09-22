package main

import (
	"fmt"

	"github.com/washed-and-dried/lzf/internals/smithwaterman"
)

func main() {
	matrix, score := smithwaterman.ComputeMatrix("arshpsps", "arshpsps")
	fmt.Println(score)
	matrix.PrintMatrix()
}
