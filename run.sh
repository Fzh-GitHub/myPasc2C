# ./Pasc2c
# Windows
# ------------------------------------------------------------------------
# 首次运行
# .\build\Debug\Pasc2c.exe -c .\code.pascal -g .\Grammar.yml -p .\LR1-Parse-Table.yml -t .\Tokens.txt -o .\ast.txt -s .\LR1-Parse-Table.csv
# 若未修改语法文件，可以读取分析表后，直接运行
# .\build\Debug\Pasc2c.exe -c .\code.pascal -g .\Grammar.yml -l .\LR1-Parse-Table.yml -t .\Tokens.txt -s .\LR1-Parse-Table.csv -o .\ast.txt
# ------------------------------------------------------------------------

# MacOS
# ------------------------------------------------------------------------
# 首次运行
# ./build/Pasc2c -c ./code.pascal -g ./Grammar.yml -p ./LR1-Parse-Table.yml -t ./Tokens.txt -o ./ast.txt -s ./LR1-Parse-Table.csv
# 若未修改语法文件，可以读取分析表后，直接运行
# ./build/Pasc2c -c ./code.pascal -g ./Grammar.yml -l ./LR1-Parse-Table.yml -t ./Tokens.txt -s ./LR1-Parse-Table.csv -o ./ast.txt
# ------------------------------------------------------------------------