import os

os.system("make clean")
os.system("make")

print("Running 5th Testcase...")
os.system("./test_assig2.sh assig2_5 > res_assig2_5")
os.system("sed -i '1,23d' res_assig2_5")
os.system("head -n -2 res_assig2_5 > res_assig2_5new")
f = open('res_assig2_5new', 'r')
result = f.read()
if(result.find('3') == -1 and len(result) > 1):
	print("Pass")
else:
	print("Fail")