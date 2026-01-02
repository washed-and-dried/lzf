package main

import (
	"fmt"
	"os"
	"path/filepath"
	"runtime"
	"sync"

	"github.com/gdamore/tcell/v2"
	"github.com/rivo/tview"
	"github.com/washed-and-dried/lzf/internals/smithwaterman"
	"github.com/washed-and-dried/lzf/internals/threadedsort"
)

var ignoredDirs = map[string]bool{
	".git":    true,
	".github": true,
	".cache":  true,
	"Android": true,
	"go":      true,
	"tmp":     true,
	"dev":     true,
	"mnt":     true,
	"proc":    true,
	"sys":     true,
}

var (
	app   *tview.Application
	count *tview.TextView
)

var (
	BACKGROUND_COLOR = tcell.GetColor("#FFAF00")
	FOREGROUND_COLOR = tcell.GetColor("#4E4E4E")
)

func shouldIgnore(dir string) bool {
	_, ok := ignoredDirs[dir]

	// NOTE: ignore all hidden dirs by default
	if !ok && len(dir) > 1 && dir[0] == '.' {
		return true
	}

	return ok
}

func listFiles(dirpath string, onichan chan string) {
	defer close(onichan)
	filepath.WalkDir(dirpath, func(path string, items os.DirEntry, err error) error {
		if err != nil {
			// FIXME: fmt.Fprintf(os.Stderr, "[ERROR] Could not get directory or file info: %s", err)
			return filepath.SkipDir
		}

		if items.IsDir() { // dirs
			if shouldIgnore(items.Name()) {
				return filepath.SkipDir
			}

			return nil
		} else { // files
			onichan <- path
			return nil
		}
	})
}

func updateListWithFiles(ls *tview.List, files *[]string, onichan chan string) {
	for file := range onichan {
		*files = append(*files, file)
		app.QueueUpdateDraw(func() {
			ls.AddItem(file, "", '\x00', func() {})
			count.SetText(fmt.Sprintf("fetching %d files", len(*files)))
		})
	}
}

func rankFiles(files_original []string, pattern string) []string {
	files := append([]string(nil), files_original...)
	length := len(files)
	scores := make([]int, length)

	// divide score calculation with smithwaterman across threads
	workers := min(runtime.GOMAXPROCS(0), length)
	chunk := (length + workers - 1) / workers // fancy round up

	var wg sync.WaitGroup
	for w := 0; w < workers; w++ {
		start := w * chunk
		end := min(length, start+chunk)
		if start >= end {
			continue
		}

		wg.Add(1)
		go func(start, end int) {
			defer wg.Done()
			for idx := start; idx < end; idx++ {
				score, _, _ := smithwaterman.ComputeMatrix(pattern, files[idx])
				scores[idx] = score
			}
		}(start, end)
	}
	wg.Wait()

	threadedsort.Sort(files, scores)

	return files
}

func dirExists(dir string) bool {
	if _, err := os.Stat(dir); err != nil {
		if os.IsNotExist(err) {
			return false
		} else {
			panic(err)
		}
	}

	return true
}

func main() {
	if len(os.Args) < 2 {
		fmt.Printf("USAGE: %s [DIRECTORY]\n", os.Args[0])
		os.Exit(69)
	}

	dir := os.Args[1]
	if !dirExists(dir) {
		fmt.Printf("Provided directory %s does not exist\n", dir)
		os.Exit(69)
	}

	onichan := make(chan string)
	go listFiles(dir, onichan)

	app = tview.NewApplication()

	list := tview.NewList()
	list.ShowSecondaryText(false)
	list.SetSelectedTextColor(FOREGROUND_COLOR)
	list.SetSelectedBackgroundColor(BACKGROUND_COLOR)

	files := []string{}
	go updateListWithFiles(list, &files, onichan)

	count = tview.NewTextView()
	count.SetBackgroundColor(tcell.GetColor("#626262"))

	inputField := tview.NewInputField().
		SetFieldBackgroundColor(BACKGROUND_COLOR).
		SetFieldTextColor(FOREGROUND_COLOR).
		SetChangedFunc(func(text string) {
			go func(text string) { // FIXME: what happens with immediate calls??
				sorted_files := rankFiles(files, text)
				app.QueueUpdateDraw(func() {
					list.Clear()
					for _, file := range sorted_files {
						list.AddItem(file, "", '\x00', func() {})
					}
				})
			}(text)
		})

	flex := tview.NewFlex().SetDirection(tview.FlexRow).
		AddItem(list, 0, 10, false).
		AddItem(count, 1, 0, false).
		AddItem(inputField, 1, 0, true)

	// Ctrl + N and Ctrl + P to navigate list
	app.SetInputCapture(func(event *tcell.EventKey) *tcell.EventKey {
		if event.Key() == tcell.KeyCtrlN {
			list.SetCurrentItem(list.GetCurrentItem() + 1)
			return nil
		} else if event.Key() == tcell.KeyCtrlP {
			list.SetCurrentItem(list.GetCurrentItem() - 1)
			return nil
		} else if event.Key() == tcell.KeyHome {
			list.SetCurrentItem(0)
			return nil
		} else if event.Key() == tcell.KeyEnter {
			if list.GetItemCount() <= 0 {
				return nil
			}
			app.Stop()
			fmt.Println(list.GetItemText(list.GetCurrentItem()))
			os.Exit(0)
		}

		return event
	})

	if err := app.SetRoot(flex, true).EnableMouse(true).Run(); err != nil {
		panic(err)
	}
}
