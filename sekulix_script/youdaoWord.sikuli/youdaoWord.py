START_LINE = 39  # Start from 1
END_LINE = 73
FILENAME = "script\\word.txt"

if (not exists("1478853801633.png")):
    print("Can't find YoudaoWord!")
    exit(0)

file = open(FILENAME, "rt")
lines = file.readlines()

if (START_LINE < 1):
    startLine = 1
else:
    startLine = START_LINE

if (len(lines) < END_LINE):
   endLine = len(lines)
else:
    endLine = END_LINE
    
for index in range(START_LINE - 1, endLine):
    line = lines[index]
    word = line.strip()
    print("Line: %d: %s" % (index + 1, word))

    if (not exists("1478854929975.png")):
        print("Not exist ICON: add")
        exit(0)
    click("1478854929975.png")
    if (not exists("1478855089351.png", 5)):
        print("Add word failed for No ICON: WORD")
        exit(0)
    type(word)
    wait(8)
    type(Key.ENTER)
    if (exists("1478855370876.png", 3)):
        print("  --- word exist already!")
        click(Pattern("1478855370876.png").targetOffset(114,40))
