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

import argparse
import sys
import re


# http://stackoverflow.com/questions/2319019/using-regex-to-remove-comments-from-source-files
# http://stackoverflow.com/a/18381470
def remove_comments(string):
    pattern = r"(\".*?\"|\'.*?\')|(/\*.*?\*/|//[^\r\n]*$)"
    # first group captures quoted strings (double or single)
    # second group captures comments (//single-line or /* multi-line */)
    regex = re.compile(pattern, re.MULTILINE | re.DOTALL)

    def _replacer(match):
        # if the 2nd group (capturing comments) is not None,
        # it means we have captured a non-quoted (real) comment string.
        if match.group(2) is not None:
            return ""
        else:  # otherwise, we will return the 1st group
            return match.group(1)  # captured quoted-string
    return regex.sub(_replacer, string)


# http://stackoverflow.com/questions/3609596/python-regular-expression-must-strip-whitespace-except-between-quotes
# http://stackoverflow.com/a/3609802
def stripwhite(text):
    # Remove whitespace characters not found in between double-quotes
    lst = text.split('"')
    for i, item in enumerate(lst):
        if not i % 2:
            lst[i] = re.sub("\s+", "", item)
    return '"'.join(lst)


def migrate_file_data(json_filename, variable_name, output_filename):
    """
    Provide a JSON filename, a variable name, and an (optional) output filename.
    It reads in the JSON file (discarding comments, turns it back into a string in "compressed" format - that is,
    without unneeded whitespaces, etc. It then writes out a file that basically is a C/C++ source file defining a
    string array by converting every character to hex.
    :return:
    """
    try:
        # 'with' keyword will handle closing the files
        with open(json_filename, 'rb') as json_input:
            with open(output_filename, 'w') as cpp_output:
                end_of_file = ''
                cpp_output.write('static const char {}[] = {{'.format(variable_name))
                line = json_input.readline()
                while line != end_of_file:
                    # TODO: Are comments allowed in JSON outside of data?
                    # http://www.json.org/
                    # https://plus.google.com/+DouglasCrockfordEsq/posts/RK8qyGVaGSr
                    # https://groups.yahoo.com/neo/groups/json/conversations/topics/156
                    uncommented_line = remove_comments(line)
                    stripped_line = stripwhite(uncommented_line)
                    if stripped_line != '':
                        # Don't write (now) empty line contents to the file
                        json_file_contents = list(stripped_line)
                        cpp_file_contents = ["0x{}".format(char.encode("hex")) for char in json_file_contents]
                        write_to_file = ", ".join(cpp_file_contents)
                        # Add the extra trailing comma to connect data from multiple lines
                        write_to_file = "{}, ".format(write_to_file)
                        cpp_output.write(write_to_file)
                    line = json_input.readline()
                # The End of File newline would have been removed in the while loop above. Re-add it.
                cpp_output.write("0x{}".format('\n'.encode("hex")))
                cpp_output.write('};\n')
    except IOError:
        print "Could not read file:", json_filename


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help="input file")
    parser.add_argument('-o', '--output', help="output file (defaults to standard out)")
    parser.add_argument('-s', '--symbol', default='json', help="symbol/variable name to create in generated file")
    args = parser.parse_args(argv)
    input_filename = args.input
    output_filename = args.output
    symbol = args.symbol

    # Generate a default output filename based on the input filename if output filename is not provided
    if output_filename is None:
        filename_parts = input_filename.split('.')[:-1]
        filename_parts.append('cpp')
        output_filename = '.'.join(filename_parts)

    migrate_file_data(json_filename=input_filename, variable_name=symbol, output_filename=output_filename)

if __name__ == "__main__":
    main(sys.argv[1:])

