package threadedsort

import (
	"runtime"
	"sync"
)

// based on https://codereview.stackexchange.com/questions/148025/multithreaded-bottom-up-merge-sort
func Sort(data []string, score []int) {
	n := len(data)
	if n <= 1 {
		return
	}

	workers := min(runtime.GOMAXPROCS(0), n)

	tmpData := make([]string, n)
	tmpScore := make([]int, n)

	chunk := (n + workers - 1) / workers // divide equally across threads

	var wg sync.WaitGroup
	for w := 0; w < workers; w++ {
		start := w * chunk
		end := min(n, start+chunk)
		if start >= end {
			continue
		}

		wg.Add(1)
		go func(s, e int) {
			defer wg.Done()

			// NOTE: can theoritically we replaced by sort.SliceStable
			// but according to the post, this allows to use both L1 and L2 caches effectively
			localBottomUpSort(
				data[s:e],
				score[s:e],
				tmpData[s:e],
				tmpScore[s:e],
			)
		}(start, end)
	}
	wg.Wait()

	for size := chunk; size < n; size <<= 1 {
		wg = sync.WaitGroup{}

		for start := 0; start < n; start += 2 * size {
			mid := min(n, start+size)
			end := min(n, start+2*size)

			if mid >= end {
				continue
			}

			wg.Add(1)
			go func(ll, rr, ee int) {
				defer wg.Done()
				merge(
					data, score,
					tmpData, tmpScore,
					ll, rr, ee,
				)
			}(start, mid, end)
		}

		wg.Wait()
		data, tmpData = tmpData, data
		score, tmpScore = tmpScore, score
	}
	copy(data, tmpData)
	copy(score, tmpScore)
}

// iterative merge sort basically! (avoids call stack which is helpful in big data)
func localBottomUpSort(
	data []string,
	score []int,
	tmpData []string,
	tmpScore []int,
) {
	n := len(data)
	for size := 1; size < n; size <<= 1 {
		for start := 0; start < n; start += 2 * size {
			mid := min(n, start+size)
			end := min(n, start+2*size)

			if mid >= end {
				continue
			}

			merge(
				data, score,
				tmpData, tmpScore,
				start, mid, end,
			)
		}
		data, tmpData = tmpData, data
		score, tmpScore = tmpScore, score
	}
}

func merge(
	data []string,
	score []int,
	tmpData []string,
	tmpScore []int,
	ll, rr, ee int,
) {
	i, j, k := ll, rr, ll

	for i < rr && j < ee {
		if score[i] > score[j] { // NOTE: descending order
			tmpData[k] = data[i]
			tmpScore[k] = score[i]
			i++
		} else {
			tmpData[k] = data[j]
			tmpScore[k] = score[j]
			j++
		}
		k++
	}

	for i < rr {
		tmpData[k] = data[i]
		tmpScore[k] = score[i]
		i++
		k++
	}
	for j < ee {
		tmpData[k] = data[j]
		tmpScore[k] = score[j]
		j++
		k++
	}
}
