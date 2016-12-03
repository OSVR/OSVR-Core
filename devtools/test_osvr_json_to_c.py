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
        # 'with' keyword will handle closing the files
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
        # 'with' keyword will handle closing the files
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
        migrate_file_data(json_filename=self.test_json_filepath, output_filename=output_filename)
        output_filepath = path.join(self.test_dir, '{}.cpp'.format(output_filename))
        print "input_filepath: {}".format(self.test_json_filepath)
        print "output_filepath: {}".format(output_filepath)
        # TODO: This assertion fails. Fix it.
        self.assertTrue(path.isfile(output_filepath))

    def test_file_read_error(self):
        """
        Does migrate_file_data() return an IOError as expected if the input file is unreadable?
        :return:
        """
        # TODO: Intentionally break this unit test as a placeholder until it can be properly implemented
        self.assertTrue(False)

if __name__ == '__main__':
    unittest.main()
