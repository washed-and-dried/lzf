package main

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/gdamore/tcell/v2"
	"github.com/rivo/tview"
)

var ignoredDirs = map[string]bool{
	".git":    true,
	".github": true,
	".cache":  true,
	"tmp":     true,
}

func shouldIgnore(dir string) bool {
	_, ok := ignoredDirs[dir]

	return ok
}

func listFiles(dirpath string) *[]string {
	files := []string{}

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
			files = append(files, path)
			return nil
		}
	})

	return &files
}

func main() {
	files := *listFiles("/")
	app := tview.NewApplication()

	list := tview.NewList()
	list.ShowSecondaryText(false)

	inputField := tview.NewInputField().
		SetFieldWidth(30).
		SetChangedFunc(func(text string) {
			if len(text) == 0 {
				list = list.Clear()
				return
			}

			list = list.AddItem(files[len(text)], "", '\x00', nil)
		})

	flex := tview.NewFlex().SetDirection(tview.FlexRow).
		AddItem(list, 0, 10, false).
		AddItem(inputField, 0, 1, true)

	// Ctrl + N and Ctrl + P to navigate list
	app.SetInputCapture(func(event *tcell.EventKey) *tcell.EventKey {
		if event.Key() == tcell.KeyCtrlN {
			list.SetCurrentItem(list.GetCurrentItem() + 1)
			return nil
		} else if event.Key() == tcell.KeyCtrlP {
			list.SetCurrentItem(list.GetCurrentItem() - 1)
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
