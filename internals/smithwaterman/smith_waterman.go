// Package smithwaterman provides ComputeMatrix() -> Matrix{}
package smithwaterman

import "fmt"

const (
	MATCH    int = 2
	MISMATCH int = -1
	GAP      int = -2
)

const (
	enumMatch = iota
	enumGap1
	enumGap2
)

type pair struct {
	l int
	r int
}

type Matrix struct {
	matrix           map[pair]int
	rowSize, colSize int
	rowText, colText string
	scoreX, scoreY   int // represents the Matrix.Score Position on the matrix, required for constructing a traceback/match string
	Score            int
}

func newMatrix(row, col string) *Matrix {
	return &Matrix{
		matrix:  map[pair]int{},
		rowSize: len(row),
		colSize: len(col),
		rowText: row,
		colText: col,
	}
}

func (m *Matrix) get(i, j int) int {
	if val, ok := m.matrix[pair{i, j}]; ok {
		return val
	}
	return 0
}

func (m *Matrix) smithWaterman() int {
	maxScore := 0
	for i := 1; i < m.rowSize; i++ {
		for j := 1; j < m.colSize; j++ {
			match := m.get(i-1, j-1)
			if m.rowText[i-1] == m.colText[j-1] {
				match += MATCH
			} else {
				match += MISMATCH
			}
			gap1 := m.get(i-1, j) + GAP
			gap2 := m.get(i, j-1) + GAP
			maxVal := max(match, gap1, gap2)
			if maxVal > 0 {
				fmt.Println(">>>", maxVal, "<<<")
				m.matrix[pair{i, j}] = maxVal
			}
			if maxVal > maxScore {
				maxScore = maxVal
				m.scoreX = i
				m.scoreY = j
			}
		}
	}
	m.Score = maxScore
	return maxScore
}

// ComputeMatrix takes 2 strings to perform smithwaterman on.
//
// Returns Matrix{} and smithwaterman score
func ComputeMatrix(str1, str2 string) (*Matrix, int) {
	m := newMatrix(str1, str2)
	return m, m.smithWaterman()
}

func (m *Matrix) PrintMatrix() {
	for i := 0; i < m.rowSize; i++ {
		for j := 0; j < m.colSize; j++ {
			fmt.Printf("%4d", m.get(i, j))
		}
		fmt.Println()
	}
}
