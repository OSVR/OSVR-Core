"""
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http:www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 """

from os import path
import shutil
import tempfile
import unittest
from osvr_json_to_c import migrate_file_data


class OSVRJsonToCTestCase(unittest.TestCase):
    """Tests for `osvr_json_to_c.py`."""

    def setUp(self):
        # Create a temporary directory
        self.test_dir = tempfile.mkdtemp()
        self.test_json_filename = 'test.json'
        self.test_json_filepath = path.join(self.test_dir, self.test_json_filename)
        # 'with' keyword will handle the closing of files
        with open(self.test_json_filepath, 'w') as json_file:
            json_file_contents = """{
                                       "hello": "world"
                                    }"""
            json_file.write(json_file_contents)

    def tearDown(self):
        # Remove the directory after the test
        shutil.rmtree(self.test_dir)

    def test_set_variable_name_in_output(self):
        """
        Does the variable name set as expected?
        :return:
        """
        symbol = 'variable_name'
        temp_output_filepath = path.join(self.test_dir, 'test.cpp')
        migrate_file_data(json_filename=self.test_json_filepath, variable_name=symbol)
        # 'with' keyword will handle the closing of files
        with open(temp_output_filepath, 'rb') as cpp_file:
            # Reopen the file and check if what we read back is the same
            line = cpp_file.readline()
            self.assertTrue('static const char {}[]'.format(symbol) in line)

    def test_default_variable_name_in_output(self):
        """
        Does the variable name default as expected?
        :return:
        """
        symbol = 'json'
        output_filepath = path.join(self.test_dir, 'test.cpp')
        migrate_file_data(json_filename=self.test_json_filepath)
        # 'with' keyword will handle the closing of files
        with open(output_filepath, 'rb') as cpp_file:
            # Reopen the file and check if what we read back is the same
            line = cpp_file.readline()
            self.assertTrue('static const char {}[]'.format(symbol) in line)

    def test_default_output_filename(self):
        """
        Does the output filename default as expected?
        :return:
        """
        migrate_file_data(json_filename=self.test_json_filepath)
        output_filepath = path.join(self.test_dir, 'test.cpp')
        self.assertTrue(path.isfile(output_filepath))

    def test_set_output_filename(self):
        """
        Does the output filename set as expected?
        :return:
        """
        output_filename = 'test_output'
        output_filepath = path.join(self.test_dir, '{}.cpp'.format(output_filename))
        migrate_file_data(json_filename=self.test_json_filepath, output_filename=output_filepath)
        self.assertTrue(path.isfile(output_filepath))

    def test_file_read_error(self):
        """
        Does migrate_file_data() return an error message as expected if the input file is unreadable?
        :return:
        """
        error_message = migrate_file_data(json_filename='')
        self.assertTrue('Could not read file:' in error_message)

    def test_hello_world_json(self):
        """
        Does migrate_file_data() create the proper cpp file contents given the hello world test data?
        :return:
        """
        expected_cpp_contents = "static const char json[] = {0x7b, 0x22, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x22, 0x3a, 0x22, 0x77, 0x6f," \
                                " 0x72, 0x6c, 0x64, 0x22, 0x7d, 0x0a};\n"
        output_filepath = path.join(self.test_dir, 'test.cpp')
        migrate_file_data(json_filename=self.test_json_filepath)
        # 'with' keyword will handle the closing of files
        with open(output_filepath, 'rb') as cpp_file:
            # Reopen the file and check if what we read back is the same
            cpp_contents = cpp_file.read()
            self.assertEqual(cpp_contents, expected_cpp_contents)

if __name__ == '__main__':
    unittest.main()
