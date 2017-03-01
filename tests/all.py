#
# Created by Thulio Ferraz Assis on 2017-02-28.
#

import unittest
from rmtest import ModuleTestCase
from redis import ResponseError
import re as regex

hmacSecret = 'testhmacpass'

class TestCase(ModuleTestCase('../build/libsession_gate.so')):
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
            # - 32 from session ID
            # - 64 from token signature
            # - 2 dots to separate the token parts
            self.assertTrue(len(r.execute_command('sessiongate.start', hmacSecret, 15)) == 2 + 32 + 64 + 2)

    def testEnd(self):
        with self.redis() as r:
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret)
            self.assertTrue("wrong number of arguments for 'sessiongate.end' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, 'token', 'extra')
            self.assertTrue("wrong number of arguments for 'sessiongate.end' command" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, 'token')
            self.assertTrue("the token format is invalid" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, 'v1.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb')
            self.assertTrue("invalid token: the signatures do not match" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, token)
            self.assertTrue("the token is expired" in context.exception)

            # Change the token version
            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            token = regex.sub('^\w+', 'v0', token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.end', hmacSecret, token)
            self.assertTrue("invalid token: wrong token version" in context.exception)

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
                r.execute_command('sessiongate.pset', hmacSecret, 'token', 'payload_name', 'payload_data')
            self.assertTrue("the token format is invalid" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, 'v1.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb', 'payload_name', 'payload_data')
            self.assertTrue("invalid token: the signatures do not match" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', 'payload_data')
            self.assertTrue("the token is expired" in context.exception)

            # Change the token version
            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            token = regex.sub('^\w+', 'v0', token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pset', hmacSecret, token, 'payload_name', 'payload_data')
            self.assertTrue("invalid token: wrong token version" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
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
                r.execute_command('sessiongate.pget', hmacSecret, 'token', 'payload_name')
            self.assertTrue("the token format is invalid" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, 'v1.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb', 'payload_name')
            self.assertTrue("invalid token: the signatures do not match" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name')
            self.assertTrue("the token is expired" in context.exception)

            # Change the token version
            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            token = regex.sub('^\w+', 'v0', token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name')
            self.assertTrue("invalid token: wrong token version" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pget', hmacSecret, token, 'payload_name')
            self.assertTrue("the requested payload does not exist" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
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
                r.execute_command('sessiongate.pdel', hmacSecret, 'token', 'payload_name')
            self.assertTrue("the token format is invalid" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, 'v1.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb', 'payload_name')
            self.assertTrue("invalid token: the signatures do not match" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name')
            self.assertTrue("the token is expired" in context.exception)

            # Change the token version
            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            token = regex.sub('^\w+', 'v0', token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name')
            self.assertTrue("invalid token: wrong token version" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.pdel', hmacSecret, token, 'payload_name')
            self.assertTrue("the requested payload to delete does not exist" in context.exception)

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
            self.assertTrue("the requested payload does not exist" in context.exception)

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
                r.execute_command('sessiongate.expire', hmacSecret, 'token', 500)
            self.assertTrue("the token format is invalid" in context.exception)

            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, 'v1.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb', 500)
            self.assertTrue("invalid token: the signatures do not match" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            r.execute_command('sessiongate.end', hmacSecret, token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, token, 500)
            self.assertTrue("the token is expired" in context.exception)

            # Change the token version
            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            token = regex.sub('^\w+', 'v0', token)
            with self.assertRaises(ResponseError) as context:
                r.execute_command('sessiongate.expire', hmacSecret, token, 500)
            self.assertTrue("invalid token: wrong token version" in context.exception)

            token = r.execute_command('sessiongate.start', hmacSecret, 15)
            self.assertOk(r.execute_command('sessiongate.expire', hmacSecret, token, 500))


if __name__ == '__main__':
    unittest.main()
