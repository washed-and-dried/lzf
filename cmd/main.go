package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/gdamore/tcell/v2"
	"github.com/rivo/tview"
	// "flag"
	// "math/rand"
	// "strconv"
	// "github.com/gdamore/tcell/v2"
	// "github.com/rivo/tview"
)

func listFiles(dirpath string) []string {
	files := []string{}

	filepath.WalkDir(dirpath, func(path string, items os.DirEntry, err error) error {
		if err != nil {
			fmt.Fprintf(os.Stderr, "[ERROR] Could not recursive down the directory: %s", err)
			return filepath.SkipAll
		}

		if err != nil {
			fmt.Fprintf(os.Stderr, "[ERROR] Could not get directory or file info: %s", err)
			return filepath.SkipAll
		}

		if items.IsDir() {
			if strings.Contains(items.Name(), ".git") {
				// TODO: skip .git, .github, .cache etc
				return filepath.SkipDir
			}

			return nil
		} else {
			files = append(files, filepath.Clean(path))
			return nil
		}
	})

	return files
}

func main() {
	files := listFiles(".")
	app := tview.NewApplication()

	list := tview.NewList()
	list.ShowSecondaryText(false)

	inputField := tview.NewInputField().
		SetFieldWidth(30).
		SetChangedFunc(func(text string) {
			list = list.Clear()

			for _, file := range files {
				list = list.AddItem(file, "", '\x00', nil)
			}
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
