3
emp 5000 178 0 4
dept 500 255 4 2
job 750 100 6 2
8
eno	integer  2 btree 1 5000 0000 9999 
ename 	string  -1 btree 0 5000 
dno     integer  2 btree 0 500  0000 9999  
jno     integer  2 btree 0 750  0000 9999 

dno     integer  2 btree 1 500  0000 9999  
dname   string  -1 noindex

jno     integer  2 btree 1 750  0000  9999 
jname   string  -1 noindex
