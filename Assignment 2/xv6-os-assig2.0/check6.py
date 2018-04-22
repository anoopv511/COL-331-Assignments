import os

os.system("make clean")
os.system("make")

print("Running 6th Testcase...")
os.system("./test_assig2.sh assig2_6 > res_assig2_6")
os.system("grep \"pid: 5 pr: 10\" res_assig2_6 | wc -l > testCount")
f = open('testCount', 'r')
result = f.read()
result = int(result)
if result > 0:
	print("Pass")
else:
	print("Fail")