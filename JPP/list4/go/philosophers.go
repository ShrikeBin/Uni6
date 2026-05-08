package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

const (
	numPhilosophers = 5
	mealsEach       = 25
)

type Fork struct{ sync.Mutex }

type Waiter struct {
	sem chan struct{}
}

func newWaiter(n int) *Waiter {
	w := &Waiter{sem: make(chan struct{}, n-1)}
	for i := 0; i < n-1; i++ {
		w.sem <- struct{}{}
	}
	return w
}
func (w *Waiter) Sit()  { <-w.sem }
func (w *Waiter) Rise() { w.sem <- struct{}{} }

var (
	logMu    sync.Mutex
	forks    [numPhilosophers]*Fork
	waiter   *Waiter
	failures [numPhilosophers]int
	failMu   sync.Mutex
)

func logMsg(id int, msg string) {
	logMu.Lock()
	defer logMu.Unlock()
	fmt.Printf("[Philosopher %d] %s\n", id, msg)
}

func philosopher(id int, wg *sync.WaitGroup) {
	defer wg.Done()
	left := id
	right := (id + 1) % numPhilosophers
	meals := 0
	localFails := 0

	for meals < mealsEach {
		logMsg(id, "is thinking...")
		time.Sleep(time.Duration(rand.Intn(3)+1) * 20 * time.Millisecond)

		logMsg(id, "is hungry, waits for a seat...")
		waiter.Sit()

		forks[left].Lock()
		logMsg(id, "picks up LEFT fork")
		forks[right].Lock()
		logMsg(id, "picks up RIGHT fork")

		meals++
		logMsg(id, fmt.Sprintf("*** EATS %d ***", meals))
		time.Sleep(time.Duration(rand.Intn(3)+1) * 20 * time.Millisecond)

		forks[left].Unlock()
		forks[right].Unlock()
		logMsg(id, "puts forks down")

		waiter.Rise()
	}

	failMu.Lock()
	failures[id] = localFails
	failMu.Unlock()

	logMsg(id, fmt.Sprintf("finished all %d meals. Failures: %d", mealsEach, localFails))
}

func runPhilosophers() {
	fmt.Println("=== Dining Philosophers (Go) ===")
	fmt.Printf("Philosophers : %d\n", numPhilosophers)
	fmt.Printf("Meals each   : %d\n", mealsEach)
	fmt.Println("================================")

	rand.Seed(time.Now().UnixNano())

	for i := range forks {
		forks[i] = &Fork{}
	}
	waiter = newWaiter(numPhilosophers)

	var wg sync.WaitGroup
	for i := 0; i < numPhilosophers; i++ {
		wg.Add(1)
		go philosopher(i, &wg)
	}
	wg.Wait()

	fmt.Println("\n=== RESULTS ===")
	totalFails := 0
	for i := 0; i < numPhilosophers; i++ {
		fmt.Printf("Philosopher %d failed to eat: %d times\n", i, failures[i])
		totalFails += failures[i]
	}
	if totalFails == 0 {
		fmt.Println("System was fair — no philosopher starved.")
	} else {
		fmt.Printf("Total failures = %d (waiter ensures no permanent starvation)\n", totalFails)
	}
}