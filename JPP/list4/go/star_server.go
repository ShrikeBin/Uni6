package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

const (
	numUsers    = 10
	msgsPerUser = 10
)

type Message struct {
	From    int
	To      int
	Content int
}

var (
	srvLogMu sync.Mutex
	received [numUsers]int
	recvMu   sync.Mutex
)

func srvLog(who, msg string) {
	srvLogMu.Lock()
	defer srvLogMu.Unlock()
	fmt.Printf("[%s] %s\n", who, msg)
}

func server(inbox <-chan Message, deliveries []chan Message, done chan<- struct{}) {
	total := numUsers * msgsPerUser
	count := 0
	srvLog("SERVER", fmt.Sprintf("started, expecting %d messages", total))
	for msg := range inbox {
		srvLog("SERVER", fmt.Sprintf("relaying msg from User%d to User%d (seq=%d)",
			msg.From, msg.To, msg.Content))
		deliveries[msg.To] <- msg

		recvMu.Lock()
		received[msg.To]++
		recvMu.Unlock()

		count++
		if count == total {
			break
		}
	}
	srvLog("SERVER", "all messages delivered, shutting down.")
	close(done)
}

func user(id int, inbox <-chan Message, serverCh chan<- Message, wg *sync.WaitGroup) {
	defer wg.Done()
	name := fmt.Sprintf("User%d", id)
	srvLog(name, "started")

	go func() {
		for seq := 1; seq <= msgsPerUser; seq++ {
			time.Sleep(time.Duration(rand.Intn(4)+1) * 15 * time.Millisecond)
			dest := rand.Intn(numUsers)
			srvLog(name, fmt.Sprintf("sending msg #%d to User%d", seq, dest))
			serverCh <- Message{From: id, To: dest, Content: seq}
		}
		srvLog(name, "finished sending.")
	}()

	for msg := range inbox {
		srvLog(name, fmt.Sprintf("received msg from User%d (seq=%d)", msg.From, msg.Content))
	}
}

func runStarServer() {
	fmt.Println("\n=== Star Server (Go) ===")
	fmt.Printf("Users         : %d\n", numUsers)
	fmt.Printf("Messages each : %d\n", msgsPerUser)
	fmt.Println("========================")

	rand.Seed(time.Now().UnixNano())

	serverInbox := make(chan Message)
	deliveries := make([]chan Message, numUsers)
	for i := range deliveries {
		deliveries[i] = make(chan Message, 1)
	}

	done := make(chan struct{})
	go server(serverInbox, deliveries, done)

	var wg sync.WaitGroup
	for i := 0; i < numUsers; i++ {
		wg.Add(1)
		go user(i, deliveries[i], serverInbox, &wg)
	}

	<-done
	for i := range deliveries {
		close(deliveries[i])
	}

	wg.Wait()

	fmt.Println("\n=== FINAL REPORT ===")
	for i := 0; i < numUsers; i++ {
		fmt.Printf("User %d received: %d message(s)\n", i, received[i])
	}
}