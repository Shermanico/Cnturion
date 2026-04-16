import sys

with open('utilities/color.h', 'r') as f:
    content = f.read()

content = content.replace('\\e', '\\033')
content = content.replace('//Reset', 'void initColors(void);\n\n//Reset')

content = '#ifndef COLOR_H\n#define COLOR_H\n\n' + content + '\n#endif\n'

with open('utilities/color.h', 'w') as f:
    f.write(content)
