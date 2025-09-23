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
	matrix           map[pair]pair
	rowText, colText string
	tracebackStr     string
	rowSize, colSize int
	scorePos         pair // represents the Matrix.Score Position on the matrix, required for constructing a traceback/match string
	Score            int
}

func newMatrix(row, col string) *Matrix {
	return &Matrix{
		matrix:  map[pair]pair{},
		rowSize: len(row)+1,
		colSize: len(col)+1,
		rowText: row,
		colText: col,
	}
}

func (m *Matrix) get(i, j int) int {
	if val, ok := m.matrix[pair{i, j}]; ok {
		return val.l
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
				p := pair{maxVal, 0}
				switch maxVal {
				case match:
					p.r = enumMatch
				case gap1:
					p.r = enumGap1
				default:
					p.r = enumGap2
				}
				m.matrix[pair{i, j}] = p
			}
			if maxVal > maxScore {
				maxScore = maxVal
				m.scorePos = pair{i, j}
			}
		}
	}
	m.Score = maxScore
	return maxScore
}

func (m *Matrix) traceback() string {
	var str []byte

	pos := pair{m.scorePos.l, m.scorePos.r}

	for p, ok := m.matrix[pos]; ok; p, ok = m.matrix[pos] {
		switch p.r {
		case enumMatch:
			println(pos.l, pos.r)
			str = append([]byte{m.rowText[pos.l-1]}, str...)
			pos.l--
			pos.r--
		case enumGap1:
			pos.l--
		default:
			pos.r--
		}
	}

	m.tracebackStr = string(str)
	return m.tracebackStr
}

// ComputeMatrix takes 2 strings to perform smithwaterman on.
//
// Returns smithwaterman score, traceback string and *Matrix{}
func ComputeMatrix(str1, str2 string) (int, string, *Matrix) { // TODO: returning *Matrix{} is only useful for testing, should be removed later or gotten via a different method
	m := newMatrix(str1, str2)
	return m.smithWaterman(), m.traceback(), m
}

func (m *Matrix) PrintMatrix() {
	for i := 0; i < m.rowSize; i++ {
		for j := 0; j < m.colSize; j++ {
			fmt.Printf("%4d", m.get(i, j))
		}
		fmt.Println()
	}
}
