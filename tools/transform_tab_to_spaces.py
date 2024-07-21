def transform_tab_to_spaces(filename):
        with open(filename, mode='r') as f:
                s = f.read()

        ret = ''
        for ch in s:
                if ch == '\t':
                        ret += '        '
                else:
                        ret += ch

        with open(filename, mode='w') as f:
                f.write(ret)