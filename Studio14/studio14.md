CSE 442S 11/15/2021 Studio 14

## Q1:
Zhuoran Sun
Zhikuan Wei

## Q2:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ mkdir TestDir
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_fprintf TestDir
error: failed to open file TestDir due to the following error
	Is a directory
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_fprintf TestFile Hello World
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ cat TestFile 
Hello
World
```

## Q3:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_fputs TestDir
error: failed to open file TestDir due to the following error
	Is a directory
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_fputs TestFile F P U T S
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ cat TestFile 
F
P
U
T
S
```

## Q4:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_write TestDir
error: failed to open file TestDir due to the following error
	Is a directory
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_write TestFile W R I T E
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ cat TestFile
W
R
I
T
E
```

## Q5:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_append TestDir
error: failed to open file TestDir due to the following error
	Is a directory
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_append TestFile A P P E N D I N G
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ cat TestFile
W
R
I
T
E
A
P
P
E
N
D
I
N
G
```

## Q6:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_formatted TestDir
error: failed to open file TestDir due to the following error
	Is a directory
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_formatted TestFormatted Hello World
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_formatted TestFormatted Life
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ ./studio14_formatted TestFormatted Goodbye World
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio14 $ cat TestFormatted 
5 Hello
5 World
4 Life
7 Goodbye
5 World
```


