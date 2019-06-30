package integration_test

import (
	"fmt"
	"io"
	"os/exec"
	"strings"

	. "github.com/onsi/ginkgo"
	. "github.com/onsi/gomega"
)

const (
	rediscliCmd    = "redis-cli"
	signKey        = "mysecret12345"
	tokenRegexp    = `^v1\.[0-9a-zA-Z]{16}\.[0-9a-f]{64}$`
	tokenTTL       = "10"
	maxPayloadName = 200
	maxPayloadData = int(1e6 * 8)
	startCmd       = "sessiongate.start"
	endCmd         = "sessiongate.end"
	psetCmd        = "sessiongate.pset"
	pgetCmd        = "sessiongate.pget"
	pdelCmd        = "sessiongate.pdel"
	expireCmd      = "sessiongate.expire"
)

var (
	fakeWhateverToken         = fmt.Sprintf("v1.%s.%s", strings.Repeat("a", 16), strings.Repeat("b", 64))
	fakeInvalidTokenFormat    = "<token>"
	fakeInvalidTokenSignature = fmt.Sprintf("v1.%s.%s", strings.Repeat("a", 16), strings.Repeat("b", 64))
)

var _ = Describe("Integration", func() {
	Describe(startCmd, func() {
		const cmd = startCmd

		itShouldFailWithWrongNumberOfArguments(cmd, signKey)
		itShouldFailWithWrongNumberOfArguments(cmd, signKey, tokenTTL, "extra")
		itShouldFailWithEmptySignKey(cmd, "", tokenTTL)
		It("should fail with invalid <ttl>", func() {
			out := rediscli(cmd, signKey, "")
			err := "<ttl> must be a valid integer that represents seconds"
			Expect(out).To(Equal(err))
		})
		It("should fail with invalid <ttl>", func() {
			out := rediscli(cmd, signKey, "asd")
			err := "<ttl> must be a valid integer that represents seconds"
			Expect(out).To(Equal(err))
		})
		It("should fail with invalid <ttl>", func() {
			out := rediscli(cmd, signKey, "-1")
			err := "<ttl> must be a valid integer that represents seconds"
			Expect(out).To(Equal(err))
		})
		It("should succeed", func() {
			mustToken()
		})
	})

	Describe(endCmd, func() {
		const cmd = endCmd

		itShouldFailWithWrongNumberOfArguments(cmd, signKey)
		itShouldFailWithWrongNumberOfArguments(cmd, signKey, fakeWhateverToken, "extra")
		itShouldFailWithEmptySignKey(cmd, "", fakeWhateverToken)
		itShouldFailWithEmptyToken(cmd, signKey, "")
		itShouldFailWithInvalidTokenFormat(cmd, signKey, fakeInvalidTokenFormat)
		itShouldFailWithInvalidTokenSignature(cmd, signKey, fakeInvalidTokenSignature)
		It("should fail when trying to end an already ended token", func() {
			token := mustToken()
			mustEnd(token)
			out := rediscli(cmd, signKey, token)
			err := "the session id contained in <token> does not exist"
			Expect(out).To(Equal(err))
		})
		It("should succeed", func() {
			token := mustToken()
			mustEnd(token)
		})
	})

	Describe(psetCmd, func() {
		const cmd = psetCmd
		const payloadName = "someName"
		const payloadData = "someData"

		itShouldFailWithWrongNumberOfArguments(cmd, signKey, fakeWhateverToken, payloadName)
		itShouldFailWithWrongNumberOfArguments(cmd, signKey, fakeWhateverToken, payloadName, payloadData, "extra")
		itShouldFailWithEmptySignKey(cmd, "", fakeWhateverToken, payloadName, payloadData)
		itShouldFailWithEmptyToken(cmd, signKey, "", payloadName, payloadData)
		itShouldFailWithInvalidTokenFormat(cmd, signKey, fakeInvalidTokenFormat, payloadName, payloadData)
		itShouldFailWithInvalidTokenSignature(cmd, signKey, fakeInvalidTokenSignature, payloadName, payloadData)
		It("should fail when trying to set a payload on an already ended token", func() {
			token := mustToken()
			mustEnd(token)
			out := rediscli(cmd, signKey, token, payloadName, payloadData)
			err := "the session id contained in <token> does not exist"
			Expect(out).To(Equal(err))
		})
		It("should fail with empty <payload_name>", func() {
			out := rediscli(cmd, signKey, mustToken(), "", payloadData)
			err := "<payload_name> must have at least one character"
			Expect(out).To(Equal(err))
		})
		It("should fail with max <payload_name> length exceeded", func() {
			over := strings.Repeat("a", maxPayloadName+1)
			out := rediscli(cmd, signKey, mustToken(), over, payloadData)
			err := fmt.Sprintf("<payload_name> length exceeds the maximum value allowed of %d", maxPayloadName)
			Expect(out).To(Equal(err))
		})
		It("should fail with empty <payload_data>", func() {
			out := rediscli(cmd, signKey, mustToken(), payloadName, "")
			err := "<payload_data> must have at least one character"
			Expect(out).To(Equal(err))
		})
		It("should fail with max <payload_data> size exceeded", func() {
			over := strings.Repeat("a", maxPayloadData+1)
			out := rediscli(cmd, signKey, mustToken(), payloadName, over)
			err := fmt.Sprintf("<payload_data> length exceeds the maximum value allowed of %d", maxPayloadData)
			Expect(out).To(Equal(err))
		})
		It("should succeed", func() {
			mustSet(mustToken(), payloadName, payloadData)
		})
	})

	Describe(pgetCmd, func() {
		const cmd = pgetCmd
		const payloadName = "someName"
		const payloadData = "someData"

		itShouldFailWithWrongNumberOfArguments(cmd, signKey, fakeWhateverToken)
		itShouldFailWithWrongNumberOfArguments(cmd, signKey, fakeWhateverToken, payloadName, "extra")
		itShouldFailWithEmptySignKey(cmd, "", fakeWhateverToken, payloadName)
		itShouldFailWithEmptyToken(cmd, signKey, "", payloadName)
		itShouldFailWithInvalidTokenFormat(cmd, signKey, fakeInvalidTokenFormat, payloadName)
		itShouldFailWithInvalidTokenSignature(cmd, signKey, fakeInvalidTokenSignature, payloadName)
		It("should fail when trying to get a payload on an already ended token", func() {
			token := mustToken()
			mustEnd(token)
			out := rediscli(cmd, signKey, token, payloadName)
			err := "the session id contained in <token> does not exist"
			Expect(out).To(Equal(err))
		})
		It("should fail with empty <payload_name>", func() {
			out := rediscli(cmd, signKey, mustToken(), "")
			err := "<payload_name> must have at least one character"
			Expect(out).To(Equal(err))
		})
		It("should fail with max <payload_name> length exceeded", func() {
			over := strings.Repeat("a", maxPayloadName+1)
			out := rediscli(cmd, signKey, mustToken(), over)
			err := fmt.Sprintf("<payload_name> length exceeds the maximum value allowed of %d", maxPayloadName)
			Expect(out).To(Equal(err))
		})
		It("should fail with non-existent <payload_name>", func() {
			out := rediscli(cmd, signKey, mustToken(), payloadName)
			err := "the requested <payload_name> does not exist"
			Expect(out).To(Equal(err))
		})
		It("should succeed setting and mutating multiple payloads", func() {
			token := mustToken()
			for i := 1; i < 10; i++ {
				pn := fmt.Sprintf("%s_%d", payloadName, i)
				pd := fmt.Sprintf("%s_%d", payloadData, i)
				mustSet(token, pn, pd)
				out := rediscli(cmd, signKey, token, pn)
				Expect(out).To(Equal(pd))
				anotherData := fmt.Sprintf("anotherData_%d", i)
				mustSet(token, pn, anotherData)
				out = rediscli(cmd, signKey, token, pn)
				Expect(out).To(Equal(anotherData))
			}
		})
	})

	Describe(pdelCmd, func() {
		const cmd = pdelCmd
		const payloadName = "someName"
		const payloadData = "someData"

		itShouldFailWithWrongNumberOfArguments(cmd, signKey, fakeWhateverToken)
		itShouldFailWithWrongNumberOfArguments(cmd, signKey, fakeWhateverToken, payloadName, "extra")
		itShouldFailWithEmptySignKey(cmd, "", fakeWhateverToken, payloadName)
		itShouldFailWithEmptyToken(cmd, signKey, "", payloadName)
		itShouldFailWithInvalidTokenFormat(cmd, signKey, fakeInvalidTokenFormat, payloadName)
		itShouldFailWithInvalidTokenSignature(cmd, signKey, fakeInvalidTokenSignature, payloadName)
		It("should fail when trying to delete a payload on an already ended token", func() {
			token := mustToken()
			mustEnd(token)
			out := rediscli(cmd, signKey, token, payloadName)
			err := "the session id contained in <token> does not exist"
			Expect(out).To(Equal(err))
		})
		It("should fail with empty <payload_name>", func() {
			out := rediscli(cmd, signKey, mustToken(), "")
			err := "<payload_name> must have at least one character"
			Expect(out).To(Equal(err))
		})
		It("should fail with max <payload_name> length exceeded", func() {
			over := strings.Repeat("a", maxPayloadName+1)
			out := rediscli(cmd, signKey, mustToken(), over)
			err := fmt.Sprintf("<payload_name> length exceeds the maximum value allowed of %d", maxPayloadName)
			Expect(out).To(Equal(err))
		})
		It("should fail with non-existent <payload_name>", func() {
			out := rediscli(cmd, signKey, mustToken(), payloadName)
			err := "the requested <payload_name> does not exist"
			Expect(out).To(Equal(err))
		})
		It("should succeed setting and deleting multiple payloads", func() {
			token := mustToken()
			for i := 1; i < 10; i++ {
				pn := fmt.Sprintf("%s_%d", payloadName, i)
				pd := fmt.Sprintf("%s_%d", payloadData, i)
				mustSet(token, pn, pd)
				mustDel(token, pn)
				out := rediscli(pgetCmd, signKey, token, pn)
				err := "the requested <payload_name> does not exist"
				Expect(out).To(Equal(err))
			}
		})
	})

	Describe(expireCmd, func() {
		const cmd = expireCmd
		const newTTL = "500"

		itShouldFailWithWrongNumberOfArguments(cmd, signKey, fakeWhateverToken)
		itShouldFailWithWrongNumberOfArguments(cmd, signKey, fakeWhateverToken, newTTL, "extra")
		itShouldFailWithEmptySignKey(cmd, "", fakeWhateverToken, newTTL)
		itShouldFailWithEmptyToken(cmd, signKey, "", newTTL)
		itShouldFailWithInvalidTokenFormat(cmd, signKey, fakeInvalidTokenFormat, newTTL)
		itShouldFailWithInvalidTokenSignature(cmd, signKey, fakeInvalidTokenSignature, newTTL)
		It("should fail with invalid <ttl>", func() {
			token := mustToken()
			out := rediscli(cmd, signKey, token, "")
			err := "<ttl> must be a valid integer that represents seconds"
			Expect(out).To(Equal(err))
		})
		It("should fail with invalid <ttl>", func() {
			token := mustToken()
			out := rediscli(cmd, signKey, token, "asd")
			err := "<ttl> must be a valid integer that represents seconds"
			Expect(out).To(Equal(err))
		})
		It("should fail with invalid <ttl>", func() {
			token := mustToken()
			out := rediscli(cmd, signKey, token, "-1")
			err := "<ttl> must be a valid integer that represents seconds"
			Expect(out).To(Equal(err))
		})
		It("should fail when trying to delete a payload on an already ended token", func() {
			token := mustToken()
			mustEnd(token)
			out := rediscli(cmd, signKey, token, newTTL)
			err := "the session id contained in <token> does not exist"
			Expect(out).To(Equal(err))
		})
		It("should succeed", func() {
			out := rediscli(cmd, signKey, mustToken(), newTTL)
			Expect(out).To(Equal("OK"))
		})
	})
})

func itShouldFailWithWrongNumberOfArguments(redisCmd string, redisArgs ...string) {
	It("should fail with wrong number of arguments", func() {
		out := rediscli(redisCmd, redisArgs...)
		err := fmt.Sprintf("ERR wrong number of arguments for '%s' command", redisCmd)
		Expect(out).To(Equal(err))
	})
}

func itShouldFailWithEmptySignKey(redisCmd string, redisArgs ...string) {
	It("should fail with empty <sign_key>", func() {
		out := rediscli(redisCmd, redisArgs...)
		err := "<sign_key> must have at least one character"
		Expect(out).To(Equal(err))
	})
}

func itShouldFailWithEmptyToken(redisCmd string, redisArgs ...string) {
	It("should fail with empty <token>", func() {
		out := rediscli(redisCmd, redisArgs...)
		err := "<token> must have at least one character"
		Expect(out).To(Equal(err))
	})
}

func itShouldFailWithInvalidTokenFormat(redisCmd string, redisArgs ...string) {
	It("should fail with invalid <token> format", func() {
		out := rediscli(redisCmd, redisArgs...)
		err := "<token> format is invalid"
		Expect(out).To(Equal(err))
	})
}

func itShouldFailWithInvalidTokenSignature(redisCmd string, redisArgs ...string) {
	It("should fail with invalid <token> signature", func() {
		out := rediscli(redisCmd, redisArgs...)
		err := "the signature contained in <token> is invalid"
		Expect(out).To(Equal(err))
	})
}

func mustToken() string {
	token := rediscli(startCmd, signKey, tokenTTL)
	Expect(token).To(MatchRegexp(tokenRegexp))
	return token
}

func mustEnd(token string) {
	out := rediscli(endCmd, signKey, token)
	Expect(out).To(Equal("OK"))
}

func mustSet(token, payloadName, payloadData string) {
	out := rediscli(psetCmd, signKey, token, payloadName, payloadData)
	Expect(out).To(Equal("OK"))
}

func mustDel(token, payloadName string) {
	out := rediscli(pdelCmd, signKey, token, payloadName)
	Expect(out).To(Equal("OK"))
}

func rediscli(redisCmd string, redisArgs ...string) string {
	cmd := exec.Command(rediscliCmd, "-s", unixSocket)
	stdinReader, stdinWriter := io.Pipe()
	go func() {
		defer stdinWriter.Close()
		fmt.Fprintf(stdinWriter, "%s ", redisCmd)
		for _, arg := range redisArgs {
			fmt.Fprintf(stdinWriter, "'%s' ", arg)
		}
	}()
	var combinedOut strings.Builder
	cmd.Stdin = stdinReader
	cmd.Stdout = &combinedOut
	cmd.Stderr = &combinedOut
	err := cmd.Run()
	Expect(err).NotTo(HaveOccurred())
	return strings.TrimSpace(combinedOut.String())
}
