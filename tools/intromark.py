import os

def compile(filename):
        if not os.path.exists(os.path.dirname("build/doc/" + filename)):
                os.makedirs(os.path.dirname("build/doc/" + filename), exist_ok=True)

        with open(filename) as f:
                content = f.read()
        graph_poslist = []
        graphstart = None
        content_lineslist = content.splitlines()
        for i, line in enumerate(content_lineslist):
                if line == '```[graph]':
                        graphstart = i
                elif line == '```' and graphstart != None:
                        graph_text = content_lineslist[graphstart + 1: i]
                        graph_filename = ('build/intromark-tmp/' +
                                          filename + '.' +
                                          str(graphstart) +
                                          '-' +
                                          str(graphstart) +
                                          '.dot')

                        if not os.path.exists(os.path.dirname(graph_filename)):
                                os.makedirs(os.path.dirname(graph_filename), exist_ok=True)
                        with open(graph_filename, 'w') as f:
                                f.write('\n'.join(graph_text))

                        svg_filename = ('build/doc/svg/' +
                                        filename + '.' +
                                        str(graphstart) +
                                        '-' +
                                        str(i) +
                                        '.svg')

                        if not os.path.exists(os.path.dirname(svg_filename)):
                                os.makedirs(os.path.dirname(svg_filename), exist_ok=True)
                        os.system("dot -Tsvg " + graph_filename + " -o " + svg_filename)

                        graph_poslist.append((graphstart, i, svg_filename))

                        graphstart = None

        for start, end, svg_filename in graph_poslist:
                content_lineslist[start: end + 1] = [f'<img src="{('svg/' +
                                                                   filename + '.' +
                                                                   str(start) +
                                                                   '-' +
                                                                   str(end) +
                                        '.svg')}" alt="SVG Image" width="200" height="200">']

        with open("build/doc/" + filename, 'w') as f:
                f.write('\n'.join(content_lineslist))

def compile_all_subprojects_intromark():
        ret = '# Subprojects List\n\n'
        for root, dirs, files in os.walk('./src', topdown=False):
                if 'README.md' in files:
                        compile(root + '/README.md')
                        with open(root + '/README.md', 'r') as file:
                                content = file.read().splitlines()
                                if content[0][0:2] == "# ":
                                        ret += ('[' + content[0][2:] + '](' +
                                                root + '/README.md)' +
                                                ' (' + root + ')' +
                                                '\n\n')
                                else:
                                        ret += ('[' + root + '](' +
                                                root + '/README.md)' +
                                                ' (' + root + ')' +
                                                '\n\n')
                                for line in content:
                                        if len(line) > 0 and line[0] != '#':
                                                ret += line + '\n'
                                ret += '\n\n'
        with open("build/doc/Subprojects.md", 'w') as f:
                f.write(ret)