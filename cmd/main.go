package main

import (
	"fmt"
	"math/rand"
	"os"
	"strconv"

	"github.com/gdamore/tcell/v2"
	"github.com/rivo/tview"
)

func main() {
	app := tview.NewApplication()

	list := tview.NewList()
	list.ShowSecondaryText(false)

	inputField := tview.NewInputField().
		SetFieldWidth(30).
		SetChangedFunc(func(text string) {
			list = list.Clear().
				AddItem("List item "+strconv.FormatInt(rand.Int63(), 10), "", '\x00', nil).
				AddItem("List item "+strconv.FormatInt(rand.Int63(), 10), "", '\x00', nil).
				AddItem("List item "+strconv.FormatInt(rand.Int63(), 10), "", '\x00', nil).
				AddItem("List item "+strconv.FormatInt(rand.Int63(), 10), "", '\x00', nil)
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
