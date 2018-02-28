#
# Created by Thulio Ferraz Assis on 2017-02-28.
#

import unittest
from rmtest import ModuleTestCase
from redis import ResponseError
import re as regex

hmacSecret = 'testhmacpass'

class TestCase(ModuleTestCase('./sessiongate.so')):
    def testStart(self):
        with self.redis() as r:
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.start', hmacSecret)
            self.assertTrue("wrong number of arguments for 'sessiongate.start' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.start', hmacSecret, 15, 'extra')
            self.assertTrue("wrong number of arguments for 'sessiongate.start' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.start', '', 15)
            self.assertTrue("<sign_key> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.start', hmacSecret, '')
            self.assertTrue("<ttl> must be a valid integer that represents seconds" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.start', hmacSecret, 'asd')
            self.assertTrue("<ttl> must be a valid integer that represents seconds" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.start', hmacSecret, '-1')
            self.assertTrue("<ttl> must be a valid integer that represents seconds" in context.exception)

            # Sum of the length of each part of the token + 2 dots:
            # - 2 from token version
            # - 16 from session ID
            # - 64 from token signature
            # - 2 dots to separate the token parts
            self.assertTrue(len(r.execute_command('sessiongate.start', hmacSecret, 15)) == 2 + 16 + 64 + 2)

    def testEnd(self):
        with self.redis() as r:
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret)
            self.assertTrue("wrong number of arguments for 'sessiongate.end' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, 'token', 'extra')
            self.assertTrue("wrong number of arguments for 'sessiongate.end' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', '', 'token')
            self.assertTrue("<sign_key> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, '')
            self.assertTrue("<token> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, 'token')
            self.assertTrue("<token> format is invalid" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, 'v1.' + ('a' * 16) + '.' + ('b' * 64))
            self.assertTrue("the signature contained in <token> is invalid" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, token)
            self.assertTrue("the session id contained in <token> does not exist" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            self.assertOk(r.execute_command('sessiongate.end', hmacSecret, token))

    def testPSet(self):
        with self.redis() as r:
            token = r.execute_command('sessiongate.start', hmacSecret, 15)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name')
            self.assertTrue("wrong number of arguments for 'sessiongate.pset' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', 'payload_data', 'extra')
            self.assertTrue("wrong number of arguments for 'sessiongate.pset' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', '', 'token', 'payload_name', 'payload_data')
            self.assertTrue("<sign_key> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, '', 'payload_name', 'payload_data')
            self.assertTrue("<token> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, 'token', 'payload_name', 'payload_data')
            self.assertTrue("<token> format is invalid" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, 'v1.' + ('a' * 16) + '.' + ('b' * 64), 'payload_name', 'payload_data')
            self.assertTrue("the signature contained in <token> is invalid" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', 'payload_data')
            self.assertTrue("the session id contained in <token> does not exist" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, token, '', 'payload_data')
            self.assertTrue("<payload_name> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, token, 'a' * 201, 'payload_data')
            self.assertTrue("<payload_name> length exceeds the maximum value allowed of 200" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', '')
            self.assertTrue("<payload_data> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', 'a' * (int(1e6 * 8) + 1))
            self.assertTrue("<payload_data> length exceeds the maximum value allowed of " + str(int(1e6 * 8)) in context.exception)

            self.assertOk(r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', 'payload_data'))

    def testPGet(self):
        with self.redis() as r:
            token = r.execute_command('sessiongate.start', hmacSecret, 15)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token)
            self.assertTrue("wrong number of arguments for 'sessiongate.pget' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name', 'extra')
            self.assertTrue("wrong number of arguments for 'sessiongate.pget' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', '', 'token', 'payload_name')
            self.assertTrue("<sign_key> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, '', 'payload_name')
            self.assertTrue("<token> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, 'token', 'payload_name')
            self.assertTrue("<token> format is invalid" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, 'v1.' + ('a' * 16) + '.' + ('b' * 64), 'payload_name')
            self.assertTrue("the signature contained in <token> is invalid" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name')
            self.assertTrue("the session id contained in <token> does not exist" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token, '')
            self.assertTrue("<payload_name> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token, 'a' * 201)
            self.assertTrue("<payload_name> length exceeds the maximum value allowed of 200" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name_does_not_exist')
            self.assertTrue("the requested <payload_name> does not exist" in context.exception)

            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', 'payload_data')
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name'), 'payload_data')

            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', 'payload_data_changed')
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name'), 'payload_data_changed')

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name1', 'payload_data1')
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name2', 'payload_data2')
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name3', 'payload_data3')
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name1'), 'payload_data1')
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name2'), 'payload_data2')
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name3'), 'payload_data3')

            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name1', 'payload_data4')
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name2', 'payload_data5')
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name3', 'payload_data6')
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name1'), 'payload_data4')
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name2'), 'payload_data5')
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name3'), 'payload_data6')

    def testPDel(self):
        with self.redis() as r:
            token = r.execute_command('sessiongate.start', hmacSecret, 15)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, token)
            self.assertTrue("wrong number of arguments for 'sessiongate.pdel' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name', 'extra')
            self.assertTrue("wrong number of arguments for 'sessiongate.pdel' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', '', 'token', 'payload_name')
            self.assertTrue("<sign_key> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, '', 'payload_name')
            self.assertTrue("<token> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, 'token', 'payload_name')
            self.assertTrue("<token> format is invalid" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, 'v1.' + ('a' * 16) + '.' + ('b' * 64), 'payload_name')
            self.assertTrue("the signature contained in <token> is invalid" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name')
            self.assertTrue("the session id contained in <token> does not exist" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, token, '')
            self.assertTrue("<payload_name> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, token, 'a' * 201)
            self.assertTrue("<payload_name> length exceeds the maximum value allowed of 200" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name_does_not_exist')
            self.assertTrue("the requested <payload_name> does not exist" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', 'payload_data')
            self.assertOk(r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name'))

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name1', 'payload_data1')
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name2', 'payload_data2')
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name3', 'payload_data3')
            self.assertOk(r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name1'))
            self.assertOk(r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name2'))
            self.assertOk(r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name3'))

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name1', 'payload_data1')
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name2', 'payload_data2')
            r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name3', 'payload_data3')
            self.assertOk(r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name2'))
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name1'), 'payload_data1')
            self.assertEqual(r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name3'), 'payload_data3')
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name2')
            self.assertTrue("the requested <payload_name> does not exist" in context.exception)

    def testExpire(self):
        with self.redis() as r:
            token = r.execute_command('sessiongate.start', hmacSecret, 15)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, token)
            self.assertTrue("wrong number of arguments for 'sessiongate.expire' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, token, 500, 'extra')
            self.assertTrue("wrong number of arguments for 'sessiongate.expire' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', '', 'token', 500)
            self.assertTrue("<sign_key> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, '', 500)
            self.assertTrue("<token> must have at least one character" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, 'token', 500)
            self.assertTrue("<token> format is invalid" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, token, '')
            self.assertTrue("<ttl> must be a valid integer that represents seconds" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, token, 'asd')
            self.assertTrue("<ttl> must be a valid integer that represents seconds" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, token, '-1')
            self.assertTrue("<ttl> must be a valid integer that represents seconds" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, 'v1.' + ('a' * 16) + '.' + ('b' * 64), 500)
            self.assertTrue("the signature contained in <token> is invalid" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, token, 500)
            self.assertTrue("the session id contained in <token> does not exist" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            self.assertOk(r.execute_command('sessiongate.expire', hmacSecret, token, 500))


if __name__ == '__main__':
    unittest.main()
