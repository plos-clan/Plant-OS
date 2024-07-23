def change_to(indent = 2):
        with open('.clang-format', 'r') as file:
                lines = file.read().splitlines()
        with open('.clang-format', 'w') as file:
                lines[1] = f'IndentWidth: {indent}'
                file.writelines(lines)