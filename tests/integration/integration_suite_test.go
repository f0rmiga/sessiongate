package integration_test

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"os/exec"
	"strings"
	"testing"
	"time"

	. "github.com/onsi/ginkgo"
	. "github.com/onsi/gomega"
)

const (
	unixSocket          = "redis.sock"
	modulePath          = "../../sessiongate.so"
	moduleLoadedMessage = "Module 'sessiongate' loaded"
	serverReadyMessage  = "The server is now ready to accept connections at " + unixSocket
)

var redisCmd *exec.Cmd

func TestIntegration(t *testing.T) {
	RegisterFailHandler(Fail)
	RunSpecs(t, "Integration Suite")
}

var _ = BeforeSuite(func() {
	redisCmd = exec.Command(
		"redis-server",
		"--port", "0", // Disable TCP.
		"--unixsocket", unixSocket,
		"--loadmodule", modulePath,
	)
	stdoutReader, stdoutWriter := io.Pipe()
	redisCmd.Stdout = stdoutWriter
	err := redisCmd.Start()
	Expect(err).NotTo(HaveOccurred())
	started := make(chan struct{})
	timeout := time.After(time.Second * 3)
	go func() {
		scanner := bufio.NewScanner(stdoutReader)
		moduleLoaded := false
		serverReady := false
		for scanner.Scan() {
			line := scanner.Text()
			fmt.Println(line)
			if strings.Contains(line, moduleLoadedMessage) {
				moduleLoaded = true
			}
			if strings.Contains(line, serverReadyMessage) {
				serverReady = true
			}
			if moduleLoaded && serverReady {
				close(started)
			}
		}
		err = scanner.Err()
		Expect(err).NotTo(HaveOccurred())
	}()
	select {
	case <-started:
		return
	case <-timeout:
		err := fmt.Errorf("something went wrong while starting Redis server with sessiongate.so module")
		log.Fatal(err)
	}
})

var _ = AfterSuite(func() {
	err := redisCmd.Process.Kill()
	Expect(err).NotTo(HaveOccurred())
	redisCmd.Wait()
})
